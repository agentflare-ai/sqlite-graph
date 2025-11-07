/* Cypher query planner - compiles AST to logical and physical plans */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-planner.h"
#include "graph_debug.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "graph_debug.h"

/* Forward declarations */
// TODO: implement cost-based join ordering
static double calculateJoinCost (LogicalPlanNode *pLeft,
                                 LogicalPlanNode *pRight, int joinType);
// TODO: integrate index selection with query planner
static int optimizeIndexUsage (LogicalPlanNode *pNode, PlanContext *pContext);
static LogicalPlanNode *compileExpression (CypherAst *pAst,
                                           PlanContext *pContext);

/*
** Helper: recursively search for a REL_PATTERN with adjacent NODE_PATTERNs
** within a PATTERN tree. If found, returns 1 and sets ppLeft/ppRel/ppRight.
*/
static int
findRelTriplet (CypherAst *pAst,
                CypherAst **ppLeft,
                CypherAst **ppRel,
                CypherAst **ppRight)
{
  int i;
  if (!pAst)
    return 0;

  if (cypherAstIsType (pAst, CYPHER_AST_PATTERN))
    {
      /* Check this level for node-rel-node */
      for (i = 0; i < pAst->nChildren; i++)
        {
          CypherAst *pChild = pAst->apChildren[i];
          if (pChild && cypherAstIsType (pChild, CYPHER_AST_REL_PATTERN))
            {
              if (i > 0 && (i + 1) < pAst->nChildren
                  && cypherAstIsType (pAst->apChildren[i - 1],
                                      CYPHER_AST_NODE_PATTERN)
                  && cypherAstIsType (pAst->apChildren[i + 1],
                                      CYPHER_AST_NODE_PATTERN))
                {
                  if (ppLeft)
                    *ppLeft = pAst->apChildren[i - 1];
                  if (ppRel)
                    *ppRel = pChild;
                  if (ppRight)
                    *ppRight = pAst->apChildren[i + 1];
                  return 1;
                }
            }
        }
      /* Recurse into children */
      for (i = 0; i < pAst->nChildren; i++)
        {
          if (findRelTriplet (pAst->apChildren[i], ppLeft, ppRel, ppRight))
            return 1;
        }
      return 0;
    }

  /* Generic recursion for other nodes */
  for (i = 0; i < pAst->nChildren; i++)
    {
      if (findRelTriplet (pAst->apChildren[i], ppLeft, ppRel, ppRight))
        return 1;
    }
  return 0;
}

/* Helper: find the first NODE_PATTERN anywhere under pAst (preorder) */
static CypherAst *
findFirstNodePattern (CypherAst *pAst)
{
  int i;
  if (!pAst)
    return NULL;
  if (cypherAstIsType (pAst, CYPHER_AST_NODE_PATTERN))
    return pAst;
  for (i = 0; i < pAst->nChildren; i++)
    {
      CypherAst *p = findFirstNodePattern (pAst->apChildren[i]);
      if (p)
        return p;
    }
  return NULL;
}

CypherPlanner *
cypherPlannerCreate (sqlite3 *pDb, GraphVtab *pGraph)
{
  CypherPlanner *pPlanner;

  pPlanner = sqlite3_malloc (sizeof (CypherPlanner));
  if (!pPlanner)
    return NULL;

  memset (pPlanner, 0, sizeof (CypherPlanner));
  pPlanner->pDb = pDb;

  /* Create planning context */
  pPlanner->pContext = sqlite3_malloc (sizeof (PlanContext));
  if (!pPlanner->pContext)
    {
      sqlite3_free (pPlanner);
      return NULL;
    }

  memset (pPlanner->pContext, 0, sizeof (PlanContext));
  pPlanner->pContext->pDb = pDb;
  pPlanner->pContext->pGraph = pGraph;

  /* Set default optimization settings */
  pPlanner->pContext->bUseIndexes = 1;
  pPlanner->pContext->bReorderJoins = 1;
  pPlanner->pContext->rIndexCostFactor = 0.1;

  return pPlanner;
}

void
cypherPlannerDestroy (CypherPlanner *pPlanner)
{
  int i;

  if (!pPlanner)
    return;

  /* Free context */
  if (pPlanner->pContext)
    {
      /* Free variable arrays */
      for (i = 0; i < pPlanner->pContext->nVariables; i++)
        {
          sqlite3_free (pPlanner->pContext->azVariables[i]);
        }
      sqlite3_free (pPlanner->pContext->azVariables);
      sqlite3_free (pPlanner->pContext->apVarNodes);

      /* Free index arrays */
      for (i = 0; i < pPlanner->pContext->nLabelIndexes; i++)
        {
          sqlite3_free (pPlanner->pContext->azLabelIndexes[i]);
        }
      sqlite3_free (pPlanner->pContext->azLabelIndexes);

      for (i = 0; i < pPlanner->pContext->nPropertyIndexes; i++)
        {
          sqlite3_free (pPlanner->pContext->azPropertyIndexes[i]);
        }
      sqlite3_free (pPlanner->pContext->azPropertyIndexes);

      sqlite3_free (pPlanner->pContext->zErrorMsg);
      sqlite3_free (pPlanner->pContext);
    }

  /* Free plans */
  logicalPlanNodeDestroy (pPlanner->pLogicalPlan);
  physicalPlanNodeDestroy (pPlanner->pPhysicalPlan);

  sqlite3_free (pPlanner->zErrorMsg);
  sqlite3_free (pPlanner);
}

/*
** Add a variable to the planning context.
** Returns SQLITE_OK on success, SQLITE_NOMEM on allocation failure.
*/
static int
planContextAddVariable (PlanContext *pContext, const char *zVar,
                        LogicalPlanNode *pNode)
{
  char **azNew;
  LogicalPlanNode **apNew;

  if (!pContext || !zVar)
    return SQLITE_MISUSE;

  /* Resize arrays if needed */
  if (pContext->nVariables >= pContext->nVariablesAlloc)
    {
      int nNew = pContext->nVariablesAlloc ? pContext->nVariablesAlloc * 2 : 8;

      azNew = sqlite3_realloc (pContext->azVariables, nNew * sizeof (char *));
      if (!azNew)
        return SQLITE_NOMEM;
      pContext->azVariables = azNew;

      apNew = sqlite3_realloc (pContext->apVarNodes,
                               nNew * sizeof (LogicalPlanNode *));
      if (!apNew)
        return SQLITE_NOMEM;
      pContext->apVarNodes = apNew;

      pContext->nVariablesAlloc = nNew;
    }

  /* Add variable */
  pContext->azVariables[pContext->nVariables] = sqlite3_mprintf ("%s", zVar);
  pContext->apVarNodes[pContext->nVariables] = pNode;

  if (!pContext->azVariables[pContext->nVariables])
    {
      return SQLITE_NOMEM;
    }

  pContext->nVariables++;
  return SQLITE_OK;
}

/*
** Compile a Cypher AST node into a logical plan node.
** Returns the compiled logical plan node, or NULL on error.
*/
static LogicalPlanNode *
compileAstNode (CypherAst *pAst, PlanContext *pContext)
{
  LogicalPlanNode *pLogical = NULL;
  LogicalPlanNode *pChild;
  const char *zAlias;
  int i;

  if (!pAst)
    return NULL;

  switch (pAst->type)
    {
    case CYPHER_AST_QUERY:
    case CYPHER_AST_SINGLE_QUERY:
      /* Compile children and combine them into a pipeline */
      if (pAst->nChildren > 0)
        {
          pLogical = compileAstNode (pAst->apChildren[0], pContext);

          for (i = 1; i < pAst->nChildren; i++)
            {
              pChild = compileAstNode (pAst->apChildren[i], pContext);
              if (pChild && pLogical)
                {
                  /* Build a pipeline: child consumes output from pLogical */
                  /* For example: MATCH (n) RETURN n becomes PROJECTION <- SCAN
                   */
                  logicalPlanNodeAddChild (pChild, pLogical);
                  pLogical = pChild;
                }
            }
        }
      break;

    case CYPHER_AST_MATCH:
      /* Compile MATCH clause */
      if (pAst->nChildren > 0)
        {
          /* Compile first pattern */
          pLogical = compileAstNode (pAst->apChildren[0], pContext);
          /* If multiple patterns separated by commas, join them */
          for (i = 1; i < pAst->nChildren; i++)
            {
              LogicalPlanNode *pNextPat
                  = compileAstNode (pAst->apChildren[i], pContext);
              if (pNextPat && pLogical)
                {
                  LogicalPlanNode *pJoin
                      = logicalPlanNodeCreate (LOGICAL_HASH_JOIN);
                  if (pJoin)
                    {
                      logicalPlanNodeAddChild (pJoin, pLogical);
                      logicalPlanNodeAddChild (pJoin, pNextPat);
                      pLogical = pJoin;
                    }
                }
            }
        }
      break;

    case CYPHER_AST_NODE_PATTERN:
      /* Node pattern becomes a scan operation */
      if (pAst->nChildren > 0
          && cypherAstIsType (pAst->apChildren[0], CYPHER_AST_IDENTIFIER))
        {
          zAlias = cypherAstGetValue (pAst->apChildren[0]);

          /* Check if this is a labeled node */
          if (pAst->nChildren > 1
              && cypherAstIsType (pAst->apChildren[1], CYPHER_AST_LABELS))
            {
              /* Prefer property index scan if properties present, else label
               * scan */
              const char *zFirstLabel = NULL;
              if (pAst->apChildren[1]->nChildren > 0)
                {
                  zFirstLabel
                      = cypherAstGetValue (pAst->apChildren[1]->apChildren[0]);
                }

              /* Check for MAP child to use property index */
              int iChild;
              const char *zPropKey = NULL;
              const char *zPropVal = NULL;
              for (iChild = 2; iChild < pAst->nChildren; iChild++)
                {
                  if (cypherAstIsType (pAst->apChildren[iChild],
                                       CYPHER_AST_MAP)
                      && pAst->apChildren[iChild]->nChildren > 0)
                    {
                      /* Use first property pair */
                      CypherAst *pPair
                          = pAst->apChildren[iChild]->apChildren[0];
                      if (cypherAstIsType (pPair, CYPHER_AST_PROPERTY_PAIR)
                          && pPair->nChildren > 0)
                        {
                          zPropKey = cypherAstGetValue (pPair);
                          zPropVal = cypherAstGetValue (pPair->apChildren[0]);
                        }
                      break;
                    }
                }

              if (zPropKey && zPropVal)
                {
                  pLogical = logicalPlanNodeCreate (LOGICAL_INDEX_SCAN);
                  if (pLogical)
                    {
                      logicalPlanNodeSetAlias (pLogical, zAlias);
                      if (zFirstLabel)
                        logicalPlanNodeSetLabel (pLogical, zFirstLabel);
                      logicalPlanNodeSetProperty (pLogical, zPropKey);
                      logicalPlanNodeSetValue (pLogical, zPropVal);
                      planContextAddVariable (pContext, zAlias, pLogical);
                    }
                }
              else
                {
                  /* Label scan fallback */
                  pLogical = logicalPlanNodeCreate (LOGICAL_LABEL_SCAN);
                  if (pLogical)
                    {
                      logicalPlanNodeSetAlias (pLogical, zAlias);
                      if (zFirstLabel)
                        logicalPlanNodeSetLabel (pLogical, zFirstLabel);
                      planContextAddVariable (pContext, zAlias, pLogical);
                    }
                }
            }
          else
            {
              /* Full node scan */
              pLogical = logicalPlanNodeCreate (LOGICAL_NODE_SCAN);
              if (pLogical)
                {
                  logicalPlanNodeSetAlias (pLogical, zAlias);
                  planContextAddVariable (pContext, zAlias, pLogical);
                }
            }
        }
      break;

    case CYPHER_AST_WHERE:
      /* WHERE clause becomes a filter */
      if (pAst->nChildren > 0)
        {
          CypherAst *pExpr = pAst->apChildren[0];

          if (cypherAstIsType (pExpr, CYPHER_AST_BINARY_OP)
              && strcmp (cypherAstGetValue (pExpr), "=") == 0
              && pExpr->nChildren >= 2)
            {

              /* Property filter: n.prop = value */
              if (cypherAstIsType (pExpr->apChildren[0], CYPHER_AST_PROPERTY))
                {
                  pLogical = logicalPlanNodeCreate (LOGICAL_PROPERTY_FILTER);
                  if (pLogical)
                    {
                      CypherAst *pProp = pExpr->apChildren[0];
                      if (pProp->nChildren >= 2)
                        {
                          const char *zVar
                              = cypherAstGetValue (pProp->apChildren[0]);
                          const char *zProp
                              = cypherAstGetValue (pProp->apChildren[1]);
                          const char *zValue
                              = cypherAstGetValue (pExpr->apChildren[1]);

                          logicalPlanNodeSetAlias (pLogical, zVar);
                          logicalPlanNodeSetProperty (pLogical, zProp);
                          logicalPlanNodeSetValue (pLogical, zValue);
                        }
                    }
                }
            }

          if (!pLogical)
            {
              /* Generic filter */
              pLogical = logicalPlanNodeCreate (LOGICAL_FILTER);
            }
        }
      break;

    case CYPHER_AST_RETURN:
      /* RETURN clause becomes a projection */
      pLogical = logicalPlanNodeCreate (LOGICAL_PROJECTION);
      if (pLogical && pAst->nChildren > 0)
        {
          /* Process projection list */
          CypherAst *pProjList = pAst->apChildren[0];
          if (cypherAstIsType (pProjList, CYPHER_AST_PROJECTION_LIST)
              && pProjList->nChildren > 0)
            {
              CypherAst *pItem = pProjList->apChildren[0];
              if (cypherAstIsType (pItem, CYPHER_AST_PROJECTION_ITEM)
                  && pItem->nChildren > 0)
                {
                  CypherAst *pExpr = pItem->apChildren[0];

                  if (cypherAstIsType (pExpr, CYPHER_AST_IDENTIFIER))
                    {
                      logicalPlanNodeSetAlias (pLogical,
                                               cypherAstGetValue (pExpr));
                    }
                  else if (cypherAstIsType (pExpr, CYPHER_AST_PROPERTY)
                           && pExpr->nChildren >= 2)
                    {
                      const char *zVar
                          = cypherAstGetValue (pExpr->apChildren[0]);
                      const char *zProp
                          = cypherAstGetValue (pExpr->apChildren[1]);

                      logicalPlanNodeSetAlias (pLogical, zVar);
                      logicalPlanNodeSetProperty (pLogical, zProp);
                    }
                }
            }
        }
      break;

    case CYPHER_AST_SKIP:
      /* SKIP clause becomes a skip operation */
      pLogical = logicalPlanNodeCreate (LOGICAL_SKIP);
      if (pLogical && pAst->nChildren > 0)
        {
          /* Get skip value from child (should be a literal) */
          const char *zSkip = cypherAstGetValue (pAst->apChildren[0]);
          if (zSkip)
            {
              logicalPlanNodeSetValue (pLogical, zSkip);
            }
        }
      break;

    case CYPHER_AST_LIMIT:
      /* LIMIT clause becomes a limit operation */
      pLogical = logicalPlanNodeCreate (LOGICAL_LIMIT);
      if (pLogical && pAst->nChildren > 0)
        {
          /* Get limit value from child (should be a literal) */
          const char *zLimit = cypherAstGetValue (pAst->apChildren[0]);
          if (zLimit)
            {
              logicalPlanNodeSetValue (pLogical, zLimit);
            }
        }
      break;

    case CYPHER_AST_IDENTIFIER:
    case CYPHER_AST_LITERAL:
    case CYPHER_AST_PROPERTY:
      /* Expression nodes - these are typically handled within their parent
       * context */
      /* For now, compile them as simple placeholder nodes that can be used by
       * parent */
      pLogical = compileExpression (pAst, pContext);
      break;

    case CYPHER_AST_COMPARISON:
      /* Comparison expression becomes a filter */
      pLogical = logicalPlanNodeCreate (LOGICAL_FILTER);
      if (pLogical && pAst->nChildren >= 2)
        {
          /* For basic comparisons, extract property and value */
          CypherAst *pLeft = pAst->apChildren[0];
          CypherAst *pRight = pAst->apChildren[1];

          /* Check if left side is a property access */
          if (cypherAstIsType (pLeft, CYPHER_AST_PROPERTY)
              && pLeft->nChildren >= 2)
            {
              const char *zVar = cypherAstGetValue (pLeft->apChildren[0]);
              const char *zProp = cypherAstGetValue (pLeft->apChildren[1]);
              const char *zValue = cypherAstGetValue (pRight);

              if (zVar)
                logicalPlanNodeSetAlias (pLogical, zVar);
              if (zProp)
                logicalPlanNodeSetProperty (pLogical, zProp);
              if (zValue)
                logicalPlanNodeSetValue (pLogical, zValue);
            }
        }
      break;

    case CYPHER_AST_AND:
      /* AND operator - combine left and right filters */
      if (pAst->nChildren >= 2)
        {
          LogicalPlanNode *pLeft
              = compileAstNode (pAst->apChildren[0], pContext);
          LogicalPlanNode *pRight
              = compileAstNode (pAst->apChildren[1], pContext);

          if (pLeft && pRight)
            {
              /* Create a filter node that contains both conditions */
              pLogical = logicalPlanNodeCreate (LOGICAL_FILTER);
              if (pLogical)
                {
                  logicalPlanNodeAddChild (pLogical, pLeft);
                  logicalPlanNodeAddChild (pLogical, pRight);
                }
            }
        }
      break;

    case CYPHER_AST_OR:
      /* OR operator - combine left and right filters */
      if (pAst->nChildren >= 2)
        {
          LogicalPlanNode *pLeft
              = compileAstNode (pAst->apChildren[0], pContext);
          LogicalPlanNode *pRight
              = compileAstNode (pAst->apChildren[1], pContext);

          if (pLeft && pRight)
            {
              /* Create a filter node that contains either condition */
              pLogical = logicalPlanNodeCreate (LOGICAL_FILTER);
              if (pLogical)
                {
                  logicalPlanNodeAddChild (pLogical, pLeft);
                  logicalPlanNodeAddChild (pLogical, pRight);
                }
            }
        }
      break;

    case CYPHER_AST_NOT:
      /* NOT operator - negate the child filter */
      if (pAst->nChildren > 0)
        {
          LogicalPlanNode *pChild
              = compileAstNode (pAst->apChildren[0], pContext);
          if (pChild)
            {
              pLogical = logicalPlanNodeCreate (LOGICAL_FILTER);
              if (pLogical)
                {
                  logicalPlanNodeAddChild (pLogical, pChild);
                  /* Mark this as a NOT filter using flags */
                  pLogical->iFlags |= 0x01; /* NOT flag */
                }
            }
        }
      break;

    case CYPHER_AST_LABELS:
      /* Labels list - typically handled by parent NODE_PATTERN */
      /* Return NULL since this should be processed in context */
      break;

    case CYPHER_AST_PATTERN:
      /* Pattern becomes a sequence of node/relationship matches */
      if (pAst->nChildren > 0)
        {
          pLogical = compileAstNode (pAst->apChildren[0], pContext);

          /* Combine multiple pattern elements with joins */
          for (i = 1; i < pAst->nChildren; i++)
            {
              pChild = compileAstNode (pAst->apChildren[i], pContext);
              if (pChild && pLogical)
                {
                  LogicalPlanNode *pJoin
                      = logicalPlanNodeCreate (LOGICAL_HASH_JOIN);
                  if (pJoin)
                    {
                      logicalPlanNodeAddChild (pJoin, pLogical);
                      logicalPlanNodeAddChild (pJoin, pChild);
                      pLogical = pJoin;
                    }
                }
            }
        }
      break;

    case CYPHER_AST_REL_PATTERN:
      /* Relationship pattern becomes an expand operation */
      pLogical = logicalPlanNodeCreate (LOGICAL_EXPAND);
      if (pLogical)
        {
          /* Extract relationship type if present */
          const char *zType = NULL;
          for (i = 0; i < pAst->nChildren; i++)
            {
              if (cypherAstIsType (pAst->apChildren[i], CYPHER_AST_IDENTIFIER))
                {
                  zType = cypherAstGetValue (pAst->apChildren[i]);
                  if (zType)
                    {
                      logicalPlanNodeSetLabel (pLogical, zType);
                    }
                  break;
                }
            }
          /* Fallback: some parsers might store type on the node value; honor
           * it */
          if (!zType && pAst->zValue)
            {
              /* zValue here encodes direction, not type; so ignore for label
               */
            }
        }
      break;

    case CYPHER_AST_MAP:
      /* Map literal - used in CREATE ({prop: value}) */
      /* For now, return NULL - this will be handled by parent CREATE node */
      break;

    case CYPHER_AST_PROPERTY_PAIR:
      /* Property key-value pair in a map */
      /* Handled by parent MAP node */
      break;

    case CYPHER_AST_PROJECTION_LIST:
    case CYPHER_AST_PROJECTION_ITEM:
      /* These are handled within RETURN processing */
      /* Return NULL - they shouldn't appear as standalone nodes to compile */
      break;

    case CYPHER_AST_CREATE:
      /* CREATE clause becomes one or more create operations (one per node pattern) */
      if (pAst->nChildren > 0)
        {
          CypherAst *pList = pAst->apChildren[0];
          LogicalPlanNode *pChain = NULL;

          if (pList && cypherAstIsType (pList, CYPHER_AST_PATTERN)
              && pList->nChildren > 0)
            {
              for (int e = 0; e < pList->nChildren; e++)
                {
                  CypherAst *pElt = pList->apChildren[e];

                  /* Prefer relationship CREATE if a triplet exists in this element */
                  CypherAst *pLeft = NULL, *pRel = NULL, *pRight = NULL;
                  if (findRelTriplet(pElt, &pLeft, &pRel, &pRight))
                    {
                      LogicalPlanNode *pCreate = logicalPlanNodeCreate (LOGICAL_CREATE);
                      if (!pCreate)
                        continue;
                      /* Extract endpoint aliases */
                      const char *zLeftVar = NULL;
                      const char *zRightVar = NULL;
                      if (pLeft && pLeft->nChildren > 0 && cypherAstIsType(pLeft->apChildren[0], CYPHER_AST_IDENTIFIER))
                        zLeftVar = cypherAstGetValue(pLeft->apChildren[0]);
                      if (pRight && pRight->nChildren > 0 && cypherAstIsType(pRight->apChildren[0], CYPHER_AST_IDENTIFIER))
                        zRightVar = cypherAstGetValue(pRight->apChildren[0]);
                      /* Relationship variable/type/direction */
                      const char *zRelType = NULL; const char *zRelVar = NULL; const char *zDir = pRel ? pRel->zValue : NULL;
                      int seenIdent = 0;
                      for (int ri = 0; pRel && ri < pRel->nChildren; ri++)
                        {
                          if (cypherAstIsType(pRel->apChildren[ri], CYPHER_AST_IDENTIFIER))
                            {
                              if (!seenIdent) { zRelType = cypherAstGetValue(pRel->apChildren[ri]); seenIdent = 1; }
                              else { zRelVar = cypherAstGetValue(pRel->apChildren[ri]); }
                            }
                        }
                      if (zRelVar) logicalPlanNodeSetAlias(pCreate, zRelVar);
                      if (zRelType) logicalPlanNodeSetLabel(pCreate, zRelType);
                      /* Compute $from/$to obeying direction */
                      const char *zFromVar = zLeftVar; const char *zToVar = zRightVar;
                      if (zDir && strcmp(zDir, "<-") == 0) { zFromVar = zRightVar; zToVar = zLeftVar; }
                      /* Build properties JSON from rel map plus $from/$to */
                      char *zJson = sqlite3_mprintf("{"); int firstp = 1;
                      if (pRel)
                        {
                          for (int ri = 0; ri < pRel->nChildren; ri++)
                            {
                              if (!cypherAstIsType(pRel->apChildren[ri], CYPHER_AST_MAP)) continue;
                              CypherAst *pMap = pRel->apChildren[ri];
                              for (int pj = 0; pj < pMap->nChildren; pj++)
                                {
                                  CypherAst *pPair = pMap->apChildren[pj];
                                  if (!cypherAstIsType(pPair, CYPHER_AST_PROPERTY_PAIR)) continue;
                                  const char *zKey = cypherAstGetValue(pPair);
                                  const char *zVal = (pPair->nChildren > 0) ? cypherAstGetValue(pPair->apChildren[0]) : NULL;
                                  if (!zKey || !zVal) continue;
                                  if (!firstp) { char *zOld = zJson; zJson = sqlite3_mprintf("%s, ", zOld); sqlite3_free(zOld); }
                                  char *zOld = zJson;
                                  if (zVal[0]=='\'' || zVal[0]=='\"') {
                                    int len = strlen(zVal); char *zUnq = sqlite3_mprintf("%.*s", len-2, zVal+1);
                                    zJson = sqlite3_mprintf("%s\"%s\": \"%s\"", zOld, zKey, zUnq); sqlite3_free(zUnq);
                                  } else {
                                    zJson = sqlite3_mprintf("%s\"%s\": %s", zOld, zKey, zVal);
                                  }
                                  sqlite3_free(zOld); firstp = 0;
                                }
                              break; /* only first map */
                            }
                        }
                      /* Append $from/$to (and $undirected if '-') */
                      if (!firstp) { char *zOld = zJson; zJson = sqlite3_mprintf("%s, ", zOld); sqlite3_free(zOld); }
                      char *zOld = zJson;
                      zJson = sqlite3_mprintf("%s\"$from\": \"%s\", \"$to\": \"%s\"%s", zOld,
                                                zFromVar?zFromVar:"", zToVar?zToVar:"",
                                                (zDir && strcmp(zDir, "-")==0)?", \"$undirected\": true":"");
                      sqlite3_free(zOld);
                      zOld = zJson; zJson = sqlite3_mprintf("%s}", zOld); sqlite3_free(zOld);
                      logicalPlanNodeSetProperty(pCreate, zJson); sqlite3_free(zJson);
                      if (pChain) { logicalPlanNodeAddChild(pCreate, pChain); }
                      pChain = pCreate;
                      continue;
                    }

                  CypherAst *pNode = findFirstNodePattern (pElt);
                  if (!pNode)
                    continue;

                  LogicalPlanNode *pCreate = logicalPlanNodeCreate (LOGICAL_CREATE);
                  if (!pCreate)
                    continue;

                  /* Alias */
                  if (pNode->nChildren > 0
                      && cypherAstIsType (pNode->apChildren[0],
                                          CYPHER_AST_IDENTIFIER))
                    {
                      const char *zAlias
                          = cypherAstGetValue (pNode->apChildren[0]);
                      if (zAlias)
                        logicalPlanNodeSetAlias (pCreate, zAlias);
                    }
                  /* Label */
                  for (i = 0; i < pNode->nChildren; i++)
                    {
                      if (cypherAstIsType (pNode->apChildren[i], CYPHER_AST_LABELS))
                        {
                          CypherAst *pLabels = pNode->apChildren[i];
                          const char *zLabel = NULL;
                          if (pLabels->nChildren > 0)
                            zLabel = cypherAstGetValue (pLabels->apChildren[0]);
                          else
                            zLabel = cypherAstGetValue (pLabels);
                          if (zLabel)
                            logicalPlanNodeSetLabel (pCreate, zLabel);
                          break;
                        }
                    }
                  /* Properties */
                  for (i = 0; i < pNode->nChildren; i++)
                    {
                      if (cypherAstIsType (pNode->apChildren[i], CYPHER_AST_MAP))
                        {
                          CypherAst *pMap = pNode->apChildren[i];
                          char *zJson = sqlite3_mprintf ("{");
                          int first = 1;
                          for (int j = 0; j < pMap->nChildren; j++)
                            {
                              CypherAst *pPair = pMap->apChildren[j];
                              if (!cypherAstIsType (pPair, CYPHER_AST_PROPERTY_PAIR))
                                continue;
                              const char *zKey = cypherAstGetValue (pPair);
                              if (!zKey)
                                continue;
                              const char *zValue = NULL;
                              if (pPair->nChildren > 0)
                                zValue = cypherAstGetValue (pPair->apChildren[0]);
                              if (zValue)
                                {
                                  if (!first)
                                    {
                                      char *zOld = zJson;
                                      zJson = sqlite3_mprintf ("%s, ", zOld);
                                      sqlite3_free (zOld);
                                    }
                                  char *zOld = zJson;
                                  if (zValue[0] == '\'' || zValue[0] == '"')
                                    {
                                      int len = strlen (zValue);
                                      char *zUnquoted
                                          = sqlite3_mprintf ("%.*s", len - 2,
                                                             zValue + 1);
                                      zJson = sqlite3_mprintf ("%s\"%s\": \"%s\"",
                                                               zOld, zKey,
                                                               zUnquoted);
                                      sqlite3_free (zUnquoted);
                                    }
                                  else
                                    {
                                      zJson = sqlite3_mprintf ("%s\"%s\": %s",
                                                               zOld, zKey,
                                                               zValue);
                                    }
                                  sqlite3_free (zOld);
                                  first = 0;
                                }
                            }
                          char *zOld = zJson;
                          zJson = sqlite3_mprintf ("%s}", zOld);
                          sqlite3_free (zOld);
                          logicalPlanNodeSetProperty (pCreate, zJson);
                          sqlite3_free (zJson);
                          break;
                        }
                    }

                  if (pChain)
                    {
                      logicalPlanNodeAddChild (pCreate, pChain);
                    }
                  pChain = pCreate;
                }

              pLogical = pChain ? pChain : logicalPlanNodeCreate (LOGICAL_CREATE);
            }
          else
            {
              /* Fallback: single create with no pattern */
              pLogical = logicalPlanNodeCreate (LOGICAL_CREATE);
            }
        }
      break;

    case CYPHER_AST_MERGE:
      /* MERGE clause becomes a merge operation */
      pLogical = logicalPlanNodeCreate (LOGICAL_MERGE);
      if (pLogical && pAst->nChildren > 0)
        {
          /* Extract basic pattern details: prefer relationship merge if found */
          CypherAst *pPatternList = pAst->apChildren[0];
GDBG("Planner MERGE: pPatternList type=%d nChildren=%d\n",
                  pPatternList ? pPatternList->type : -1,
                  pPatternList ? pPatternList->nChildren : -1);
          CypherAst *pNodePat = NULL;
          CypherAst *pRelPat = NULL;
          CypherAst *pLeftNode = NULL;
          CypherAst *pRightNode = NULL;
          int i;

          if (pPatternList)
            {
              /* First, try to find a relationship triplet anywhere */
              if (findRelTriplet (pPatternList, &pLeftNode, &pRelPat,
                                  &pRightNode))
                {
GDBG("Planner MERGE: relationship pattern detected\n");
                }
              else
                {
                  /* Fall back to first node pattern */
                  pNodePat = findFirstNodePattern (pPatternList);
                }
            }

          if (pRelPat && pLeftNode && pRightNode)
            {
              /* Relationship MERGE: (a)-[r:TYPE {..}]->(b) */
              CypherAst *pLeft = pLeftNode;
              CypherAst *pRel = pRelPat;
              CypherAst *pRight = pRightNode;
              if (cypherAstIsType (pRel, CYPHER_AST_REL_PATTERN))
                {
                  const char *zLeftVar = NULL;
                  const char *zRightVar = NULL;
                  const char *zRelType = NULL;
                  const char *zDir = pRel->zValue; /* "->", "<-" or "-" */
                  /* Left and right aliases */
                  if (pLeft && pLeft->nChildren > 0
                      && cypherAstIsType (pLeft->apChildren[0],
                                          CYPHER_AST_IDENTIFIER))
                    {
                      zLeftVar = cypherAstGetValue (pLeft->apChildren[0]);
                    }
                  if (pRight && pRight->nChildren > 0
                      && cypherAstIsType (pRight->apChildren[0],
                                          CYPHER_AST_IDENTIFIER))
                    {
                      zRightVar = cypherAstGetValue (pRight->apChildren[0]);
                    }
                  /* Compute endpoints based on direction */
                  const char *zFromVar = zLeftVar;
                  const char *zToVar = zRightVar;
                  if (zDir && strcmp (zDir, "<-") == 0)
                    {
                      zFromVar = zRightVar;
                      zToVar = zLeftVar;
                    }
                  /* In REL_PATTERN children, first IDENTIFIER (if present) is
                   * type; a second IDENTIFIER may be rel var */
                  int seenIdent = 0;
                  const char *zRelVar = NULL;
                  for (i = 0; i < pRel->nChildren; i++)
                    {
                      if (cypherAstIsType (pRel->apChildren[i],
                                           CYPHER_AST_IDENTIFIER))
                        {
                          if (!seenIdent)
                            {
                              zRelType
                                  = cypherAstGetValue (pRel->apChildren[i]);
                              seenIdent = 1;
                            }
                          else
                            {
                              zRelVar
                                  = cypherAstGetValue (pRel->apChildren[i]);
                            }
                        }
                    }
                  if (zRelVar)
                    {
                      logicalPlanNodeSetAlias (pLogical, zRelVar);
                    }
                  if (zRelType)
                    {
                      logicalPlanNodeSetLabel (pLogical, zRelType);
                    }

                  /* Build match properties JSON from REL_PATTERN's MAP child
                   * if any and inject $from/$to/$undirected */
                  char *zMatchJson = sqlite3_mprintf ("{");
                  int first = 1;
                  for (i = 0; i < pRel->nChildren; i++)
                    {
                      if (!cypherAstIsType (pRel->apChildren[i],
                                            CYPHER_AST_MAP))
                        continue;
                      CypherAst *pMap = pRel->apChildren[i];
                      for (int j = 0; j < pMap->nChildren; j++)
                        {
                          CypherAst *pPair = pMap->apChildren[j];
                          if (!cypherAstIsType (pPair,
                                                CYPHER_AST_PROPERTY_PAIR))
                            continue;
                          const char *zKey = cypherAstGetValue (pPair);
                          const char *zVal
                              = (pPair->nChildren > 0)
                                    ? cypherAstGetValue (pPair->apChildren[0])
                                    : NULL;
                          if (!zKey || !zVal)
                            continue;
                          if (!first)
                            {
                              char *zOld = zMatchJson;
                              zMatchJson = sqlite3_mprintf ("%s, ", zOld);
                              sqlite3_free (zOld);
                            }
                          char *zOld = zMatchJson;
                          if (zVal[0] == '\'' || zVal[0] == '"')
                            {
                              int len = strlen (zVal);
                              char *zUnq = sqlite3_mprintf ("%.*s", len - 2,
                                                            zVal + 1);
                              zMatchJson = sqlite3_mprintf ("%s\"%s\": \"%s\"",
                                                            zOld, zKey, zUnq);
                              sqlite3_free (zUnq);
                            }
                          else
                            {
                              zMatchJson = sqlite3_mprintf ("%s\"%s\": %s",
                                                            zOld, zKey, zVal);
                            }
                          sqlite3_free (zOld);
                          first = 0;
                        }
                      break; /* Only the first MAP child is considered */
                    }
                  /* Inject special keys for endpoints and direction */
                  if (!first)
                    {
                      char *zOld = zMatchJson;
                      zMatchJson = sqlite3_mprintf ("%s, ", zOld);
                      sqlite3_free (zOld);
                    }
                  char *zOld = zMatchJson;
                  zMatchJson = sqlite3_mprintf (
                      "%s\"$from\": \"%s\", \"$to\": \"%s\"%s", zOld,
                      zFromVar ? zFromVar : "", zToVar ? zToVar : "",
                      (zDir && strcmp (zDir, "-") == 0)
                          ? ", \"$undirected\": true"
                          : "");
                  sqlite3_free (zOld);
                  /* Close object */
                  zOld = zMatchJson;
                  zMatchJson = sqlite3_mprintf ("%s}", zOld);
                  sqlite3_free (zOld);
GDBG("Planner MERGE: rel match JSON = %s\n", zMatchJson);
                  logicalPlanNodeSetProperty (pLogical, zMatchJson);
                  sqlite3_free (zMatchJson);
                }
            }
          else if (pNodePat)
            {
GDBG("Planner MERGE: node pattern detected\n");
              /* Alias */
              if (pNodePat->nChildren > 0
                  && cypherAstIsType (pNodePat->apChildren[0],
                                      CYPHER_AST_IDENTIFIER))
                {
                  const char *zAlias
                      = cypherAstGetValue (pNodePat->apChildren[0]);
                  if (zAlias)
                    logicalPlanNodeSetAlias (pLogical, zAlias);
                }
              /* Label */
              for (i = 0; i < pNodePat->nChildren; i++)
                {
                  if (cypherAstIsType (pNodePat->apChildren[i],
                                       CYPHER_AST_LABELS))
                    {
                      CypherAst *pLabels = pNodePat->apChildren[i];
                      const char *zLabel = NULL;
                      if (pLabels->nChildren > 0)
                        {
                          zLabel = cypherAstGetValue (pLabels->apChildren[0]);
                        }
                      else
                        {
                          zLabel = cypherAstGetValue (pLabels);
                        }
                      if (zLabel)
                        {
                          logicalPlanNodeSetLabel (pLogical, zLabel);
                        }
                      break;
                    }
                }
              /* Properties -> JSON */
              for (i = 0; i < pNodePat->nChildren; i++)
                {
                  if (cypherAstIsType (pNodePat->apChildren[i],
                                       CYPHER_AST_MAP))
                    {
                      CypherAst *pMap = pNodePat->apChildren[i];
                      char *zJson = sqlite3_mprintf ("{");
                      int first = 1;
                      for (int j = 0; j < pMap->nChildren; j++)
                        {
                          CypherAst *pPair = pMap->apChildren[j];
                          if (!cypherAstIsType (pPair,
                                                CYPHER_AST_PROPERTY_PAIR))
                            continue;
                          const char *zKey = cypherAstGetValue (pPair);
                          if (!zKey)
                            continue;
                          const char *zValue = NULL;
                          if (pPair->nChildren > 0)
                            {
                              zValue
                                  = cypherAstGetValue (pPair->apChildren[0]);
                            }
                          if (zValue)
                            {
                              if (!first)
                                {
                                  char *zOld = zJson;
                                  zJson = sqlite3_mprintf ("%s, ", zOld);
                                  sqlite3_free (zOld);
                                }
                              char *zOld = zJson;
                              if (zValue[0] == '\'' || zValue[0] == '"')
                                {
                                  int len = strlen (zValue);
                                  char *zUnquoted = sqlite3_mprintf (
                                      "%.*s", len - 2, zValue + 1);
                                  zJson = sqlite3_mprintf ("%s\"%s\": \"%s\"",
                                                           zOld, zKey,
                                                           zUnquoted);
                                  sqlite3_free (zUnquoted);
                                }
                              else
                                {
                                  zJson = sqlite3_mprintf ("%s\"%s\": %s",
                                                           zOld, zKey, zValue);
                                }
                              sqlite3_free (zOld);
                              first = 0;
                            }
                        }
                      char *zOld = zJson;
                      zJson = sqlite3_mprintf ("%s}", zOld);
                      sqlite3_free (zOld);
                      logicalPlanNodeSetProperty (pLogical, zJson);
                      sqlite3_free (zJson);
                      break;
                    }
                }
            }

          /* Process optional ON CREATE/ON MATCH SET sections for MERGE */
          for (int childIdx = 1; childIdx < pAst->nChildren; childIdx++)
            {
              CypherAst *pOn = pAst->apChildren[childIdx];
              if (!pOn)
                continue;
              if (cypherAstIsType (pOn, CYPHER_AST_ON_CREATE)
                  && pOn->nChildren > 0)
                {
                  CypherAst *pSet = pOn->apChildren[0];
                  if (pSet && cypherAstIsType (pSet, CYPHER_AST_SET))
                    {
                      char *zJson = sqlite3_mprintf ("{");
                      int firstAssign = 1;
                      for (int k = 0; k < pSet->nChildren; k++)
                        {
                          CypherAst *pAssign = pSet->apChildren[k];
                          if (!pAssign
                              || !cypherAstIsType (pAssign,
                                                   CYPHER_AST_BINARY_OP))
                            continue;
                          const char *zProp = NULL;
                          if (pAssign->nChildren > 0 && pAssign->apChildren[0])
                            {
                              CypherAst *pLeft = pAssign->apChildren[0];
                              if (cypherAstIsType (pLeft, CYPHER_AST_PROPERTY)
                                  && cypherAstGetChildCount (pLeft) >= 2)
                                {
                                  /* Use only the property name (e.g., flag from a.flag) */
                                  zProp = cypherAstGetValue (
                                      cypherAstGetChild (pLeft, 1));
                                }
                              else
                                {
                                  /* Unsupported LHS for MERGE SET; skip */
                                  zProp = NULL;
                                }
                            }
                          const char *zVal = NULL;
                          CypherAst *pRhs = (pAssign->nChildren > 1) ? pAssign->apChildren[1] : NULL;
                          if (pRhs)
                            {
                              zVal = cypherAstGetValue(pRhs);
                            }
                          if (!zProp)
                            continue;
                          if (!firstAssign)
                            {
                              char *zOld = zJson;
                              zJson = sqlite3_mprintf ("%s, ", zOld);
                              sqlite3_free (zOld);
                            }
                          char *zOld = zJson;
                          /* If RHS is a property expression (e.g., a.level), encode as "a.level" */
                          if (pRhs && cypherAstIsType(pRhs, CYPHER_AST_PROPERTY) && cypherAstGetChildCount(pRhs) >= 2)
                            {
                              const char *zVarName = NULL;
                              const char *zKeyName = NULL;
                              CypherAst *pObj = cypherAstGetChild(pRhs, 0);
                              CypherAst *pKey = cypherAstGetChild(pRhs, 1);
                              if (pObj && cypherAstIsType(pObj, CYPHER_AST_IDENTIFIER))
                                zVarName = cypherAstGetValue(pObj);
                              if (pKey)
                                zKeyName = cypherAstGetValue(pKey);
                              if (zVarName && zKeyName)
                                {
                                  zJson = sqlite3_mprintf ("%s\"%s\": \"%s.%s\"", zOld, zProp, zVarName, zKeyName);
                                  sqlite3_free (zOld);
                                  firstAssign = 0;
                                  continue;
                                }
                            }
                          /* Fallbacks: literal strings or numbers */
                          if (zVal && (zVal[0] == '\'' || zVal[0] == '"'))
                            {
                              int len = (int)strlen (zVal);
                              char *zUnquoted = sqlite3_mprintf ("%.*s", len - 2, zVal + 1);
                              zJson = sqlite3_mprintf ("%s\"%s\": \"%s\"", zOld, zProp, zUnquoted);
                              sqlite3_free (zUnquoted);
                            }
                          else if (zVal)
                            {
                              /* Numbers, booleans, or identifiers (best-effort) */
                              zJson = sqlite3_mprintf ("%s\"%s\": %s", zOld, zProp, zVal);
                            }
                          else
                            {
                              zJson = sqlite3_mprintf ("%s\"%s\": null", zOld, zProp);
                            }
                          sqlite3_free (zOld);
                          firstAssign = 0;
                        }
                      char *zOld = zJson;
                      zJson = sqlite3_mprintf ("%s}", zOld);
                      sqlite3_free (zOld);
                      logicalPlanNodeSetValue (pLogical, zJson);
                      sqlite3_free (zJson);
                    }
                }
              else if (cypherAstIsType (pOn, CYPHER_AST_ON_MATCH)
                       && pOn->nChildren > 0)
                {
                  CypherAst *pSet = pOn->apChildren[0];
                  if (pSet && cypherAstIsType (pSet, CYPHER_AST_SET))
                    {
                      char *zJson = sqlite3_mprintf ("{");
                      int firstAssign = 1;
                      for (int k = 0; k < pSet->nChildren; k++)
                        {
                          CypherAst *pAssign = pSet->apChildren[k];
                          if (!pAssign
                              || !cypherAstIsType (pAssign,
                                                   CYPHER_AST_BINARY_OP))
                            continue;
                          const char *zProp = NULL;
                          if (pAssign->nChildren > 0 && pAssign->apChildren[0])
                            {
                              CypherAst *pLeft = pAssign->apChildren[0];
                              if (cypherAstIsType (pLeft, CYPHER_AST_PROPERTY)
                                  && cypherAstGetChildCount (pLeft) >= 2)
                                {
                                  /* Use only the property name (e.g., flag from a.flag) */
                                  zProp = cypherAstGetValue (
                                      cypherAstGetChild (pLeft, 1));
                                }
                              else
                                {
                                  /* Unsupported LHS for MERGE SET; skip */
                                  zProp = NULL;
                                }
                            }
                          const char *zVal = NULL;
                          CypherAst *pRhs2 = (pAssign->nChildren > 1) ? pAssign->apChildren[1] : NULL;
                          if (pRhs2)
                            {
                              zVal = cypherAstGetValue(pRhs2);
                            }
                          if (!zProp)
                            continue;
                          if (!firstAssign)
                            {
                              char *zOld2 = zJson;
                              zJson = sqlite3_mprintf ("%s, ", zOld2);
                              sqlite3_free (zOld2);
                            }
                          char *zOld2 = zJson;
                          /* If RHS is a property expression (e.g., a.level), encode as "a.level" */
                          if (pRhs2 && cypherAstIsType(pRhs2, CYPHER_AST_PROPERTY) && cypherAstGetChildCount(pRhs2) >= 2)
                            {
                              const char *zVarName2 = NULL;
                              const char *zKeyName2 = NULL;
                              CypherAst *pObj2 = cypherAstGetChild(pRhs2, 0);
                              CypherAst *pKey2 = cypherAstGetChild(pRhs2, 1);
                              if (pObj2 && cypherAstIsType(pObj2, CYPHER_AST_IDENTIFIER))
                                zVarName2 = cypherAstGetValue(pObj2);
                              if (pKey2)
                                zKeyName2 = cypherAstGetValue(pKey2);
                              if (zVarName2 && zKeyName2)
                                {
                                  zJson = sqlite3_mprintf ("%s\"%s\": \"%s.%s\"", zOld2, zProp, zVarName2, zKeyName2);
                                  sqlite3_free (zOld2);
                                  firstAssign = 0;
                                  continue;
                                }
                            }
                          if (zVal && (zVal[0] == '\'' || zVal[0] == '"'))
                            {
                              int len2 = (int)strlen (zVal);
                              char *zUnquoted2 = sqlite3_mprintf ("%.*s", len2 - 2, zVal + 1);
                              zJson = sqlite3_mprintf ("%s\"%s\": \"%s\"", zOld2, zProp, zUnquoted2);
                              sqlite3_free (zUnquoted2);
                            }
                          else if (zVal)
                            {
                              zJson = sqlite3_mprintf ("%s\"%s\": %s", zOld2, zProp, zVal);
                            }
                          else
                            {
                              zJson = sqlite3_mprintf ("%s\"%s\": null", zOld2, zProp);
                            }
                          sqlite3_free (zOld2);
                          firstAssign = 0;
                        }
                      char *zOld2 = zJson;
                      zJson = sqlite3_mprintf ("%s}", zOld2);
                      sqlite3_free (zOld2);
                      if (pLogical->pExtra)
                        sqlite3_free (pLogical->pExtra);
                      pLogical->pExtra = sqlite3_mprintf ("%s", zJson);
                      sqlite3_free (zJson);
                    }
                }
            }
        }
      break;

    case CYPHER_AST_SET:
      /* SET clause becomes a set operation */
      pLogical = logicalPlanNodeCreate (LOGICAL_SET);
      if (pLogical && pAst->nChildren > 0)
        {
          /* Process SET items - property assignments */
          for (i = 0; i < pAst->nChildren; i++)
            {
              pChild = compileAstNode (pAst->apChildren[i], pContext);
              if (pChild)
                {
                  logicalPlanNodeAddChild (pLogical, pChild);
                }
            }
        }
      break;

    case CYPHER_AST_DELETE:
      /* DELETE clause becomes a delete operation */
      pLogical = logicalPlanNodeCreate (LOGICAL_DELETE);
      if (pLogical && pAst->nChildren > 0)
        {
          /* Process expressions to delete - typically identifiers or patterns
           */
          for (i = 0; i < pAst->nChildren; i++)
            {
              if (cypherAstIsType (pAst->apChildren[i], CYPHER_AST_IDENTIFIER))
                {
                  const char *zVar = cypherAstGetValue (pAst->apChildren[i]);
                  if (zVar)
                    {
                      logicalPlanNodeSetAlias (pLogical, zVar);
                    }
                }
            }
        }
      break;

    case CYPHER_AST_REMOVE:
      /* REMOVE clause - typically for removing labels or properties */
      /* For now, treat similar to SET but with removal semantics */
      pLogical = logicalPlanNodeCreate (LOGICAL_SET); /* Reuse SET for now */
      if (pLogical && pAst->nChildren > 0)
        {
          pLogical->iFlags |= 0x02; /* REMOVE flag */
          for (i = 0; i < pAst->nChildren; i++)
            {
              pChild = compileAstNode (pAst->apChildren[i], pContext);
              if (pChild)
                {
                  logicalPlanNodeAddChild (pLogical, pChild);
                }
            }
        }
      break;

    default:
      /* Unsupported AST node type */
      pContext->zErrorMsg
          = sqlite3_mprintf ("Unsupported AST node type: %d", pAst->type);
      pContext->nErrors++;
      break;
    }

  return pLogical;
}

/*
** Compile expression AST nodes into logical plan fragments.
** This handles IDENTIFIER, LITERAL, and PROPERTY nodes that may appear
** as standalone expressions or within larger expression trees.
*/
static LogicalPlanNode *
compileExpression (CypherAst *pAst, PlanContext *pContext)
{
  LogicalPlanNode *pLogical = NULL;

  if (!pAst)
    return NULL;

  switch (pAst->type)
    {
    case CYPHER_AST_IDENTIFIER:
      /* Identifier references a variable - create a placeholder that can be
       * resolved later */
      /* For now, return NULL as identifiers are typically handled in their
       * parent context */
      break;

    case CYPHER_AST_LITERAL:
      /* Literal value - return NULL as literals are handled in context */
      /* The value can be accessed via cypherAstGetValue(pAst) */
      break;

    case CYPHER_AST_PROPERTY:
      /* Property access: obj.property */
      /* Return NULL - properties are handled by parent filter/projection nodes
       */
      /* The parent can access this via pAst->apChildren[0] (object) and
       * pAst->apChildren[1] (property) */
      break;

    default:
      /* For other expression types, try compiling as a full node */
      pLogical = compileAstNode (pAst, pContext);
      break;
    }

  return pLogical;
}

/*
** Compile an AST into a logical plan.
** Returns SQLITE_OK on success, error code on failure.
*/
int
cypherPlannerCompile (CypherPlanner *pPlanner, CypherAst *pAst)
{
  LogicalPlanNode *pRoot;

  if (!pPlanner || !pAst)
    return SQLITE_MISUSE;

  /* Clean up any previous plan */
  logicalPlanNodeDestroy (pPlanner->pLogicalPlan);
  pPlanner->pLogicalPlan = NULL;
  physicalPlanNodeDestroy (pPlanner->pPhysicalPlan);
  pPlanner->pPhysicalPlan = NULL;

  sqlite3_free (pPlanner->zErrorMsg);
  pPlanner->zErrorMsg = NULL;

  /* Reset context */
  pPlanner->pContext->pAst = pAst;
  pPlanner->pContext->nErrors = 0;
  sqlite3_free (pPlanner->pContext->zErrorMsg);
  pPlanner->pContext->zErrorMsg = NULL;

  /* Compile AST to logical plan */
  pRoot = compileAstNode (pAst, pPlanner->pContext);
  if (!pRoot)
    {
      if (pPlanner->pContext->zErrorMsg)
        {
          pPlanner->zErrorMsg = sqlite3_mprintf (
              "Compilation failed: %s", pPlanner->pContext->zErrorMsg);
        }
      else
        {
          pPlanner->zErrorMsg
              = sqlite3_mprintf ("Failed to compile AST to logical plan");
        }
      return SQLITE_ERROR;
    }

  pPlanner->pLogicalPlan = pRoot;

  /* Estimate costs */
  logicalPlanEstimateCost (pRoot, pPlanner->pContext);
  logicalPlanEstimateRows (pRoot, pPlanner->pContext);

  return SQLITE_OK;
}

/*
** Optimize the logical plan and generate physical plan.
** Returns SQLITE_OK on success, error code on failure.
*/
int
cypherPlannerOptimize (CypherPlanner *pPlanner)
{
  PhysicalPlanNode *pPhysical;

  if (!pPlanner || !pPlanner->pLogicalPlan)
    return SQLITE_MISUSE;

  /* Clean up any previous physical plan */
  physicalPlanNodeDestroy (pPlanner->pPhysicalPlan);
  pPlanner->pPhysicalPlan = NULL;

  /* Join reordering optimization */
  /* For Phase 1/2, basic join ordering is preserved from the query */
  /* Advanced join reordering based on cardinality would be added later */
  if (pPlanner->pContext->bReorderJoins)
    {
      /* The logicalPlanOptimizeJoins function would analyze join predicates */
      /* and reorder based on estimated cardinalities */
      logicalPlanOptimizeJoins (pPlanner->pLogicalPlan, pPlanner->pContext);
    }

  /* Index usage optimization */
  optimizeIndexUsage (pPlanner->pLogicalPlan, pPlanner->pContext);

  /* Convert logical plan to physical plan */
  pPhysical
      = logicalPlanToPhysical (pPlanner->pLogicalPlan, pPlanner->pContext);
  if (!pPhysical)
    {
      pPlanner->zErrorMsg
          = sqlite3_mprintf ("Failed to generate physical plan");
      return SQLITE_ERROR;
    }

  pPlanner->pPhysicalPlan = pPhysical;
  return SQLITE_OK;
}

/*
** Get the final physical execution plan.
** Returns NULL if planning failed or not yet completed.
*/
PhysicalPlanNode *
cypherPlannerGetPlan (CypherPlanner *pPlanner)
{
  return pPlanner ? pPlanner->pPhysicalPlan : NULL;
}

/*
** Get error message from planner.
** Returns NULL if no error occurred.
*/
const char *
cypherPlannerGetError (CypherPlanner *pPlanner)
{
  return pPlanner ? pPlanner->zErrorMsg : NULL;
}

/*
** Optimize join ordering using simple heuristics.
** This is a simplified version - full implementation would use dynamic
*programming.
*/
int
logicalPlanOptimizeJoins (LogicalPlanNode *pNode, PlanContext *pContext)
{
  int i;

  if (!pNode)
    return SQLITE_OK;

  /* Recursively optimize children first */
  for (i = 0; i < pNode->nChildren; i++)
    {
      logicalPlanOptimizeJoins (pNode->apChildren[i], pContext);
    }

  /* For join nodes, perform cost-based optimization */
  if (pNode->type == LOGICAL_HASH_JOIN
      || pNode->type == LOGICAL_NESTED_LOOP_JOIN)
    {
      if (pNode->nChildren >= 2)
        {
          LogicalPlanNode *pLeft = pNode->apChildren[0];
          LogicalPlanNode *pRight = pNode->apChildren[1];

          /* Calculate join costs for both orderings */
          double leftFirstCost
              = calculateJoinCost (pLeft, pRight, pNode->type);
          double rightFirstCost
              = calculateJoinCost (pRight, pLeft, pNode->type);

          /* Choose the more efficient ordering */
          if (rightFirstCost < leftFirstCost)
            {
              pNode->apChildren[0] = pRight;
              pNode->apChildren[1] = pLeft;
            }

          /* Update estimated rows for the join */
          pNode->iEstimatedRows
              = (pLeft->iEstimatedRows * pRight->iEstimatedRows) / 10;
        }
    }

  return SQLITE_OK;
}

/*
** Calculate the estimated cost of a join operation.
** Uses cardinality estimates and join type to compute relative cost.
*/
static double
calculateJoinCost (LogicalPlanNode *pLeft, LogicalPlanNode *pRight,
                   int joinType)
{
  double leftRows = pLeft->iEstimatedRows > 0 ? pLeft->iEstimatedRows : 1000.0;
  double rightRows
      = pRight->iEstimatedRows > 0 ? pRight->iEstimatedRows : 1000.0;
  double cost = 0.0;

  switch (joinType)
    {
    case LOGICAL_HASH_JOIN:
      /* Hash join: O(m + n) where m, n are input sizes */
      /* Cost includes hash table build (right side) + probe (left side) */
      cost = rightRows * 1.2 + leftRows * 1.0; /* Build cost > probe cost */
      break;

    case LOGICAL_NESTED_LOOP_JOIN:
      /* Nested loop: O(m * n) - very expensive for large inputs */
      cost
          = leftRows * rightRows * 0.001; /* Scale down to compare with hash */
      break;

    default:
      /* Unknown join type - assume expensive */
      cost = leftRows * rightRows * 0.01;
      break;
    }

  /* Add selectivity factor - assume 10% selectivity for joins */
  cost *= 0.1;

  return cost;
}

/*
** Analyze and optimize index usage for node scans.
** Replaces full table scans with index scans when beneficial.
*/
static int
optimizeIndexUsage (LogicalPlanNode *pNode, PlanContext *pContext)
{
  int i;

  if (!pNode)
    return SQLITE_OK;

  /* Recursively optimize children first */
  for (i = 0; i < pNode->nChildren; i++)
    {
      optimizeIndexUsage (pNode->apChildren[i], pContext);
    }

  /* Optimize node scan operations */
  if (pNode->type == LOGICAL_NODE_SCAN)
    {
      /* Check if we have a label filter that can use label index */
      if (pNode->zLabel && strlen (pNode->zLabel) > 0)
        {
          /* Convert to label index scan - much more efficient */
          pNode->type = LOGICAL_LABEL_SCAN;
          pNode->iEstimatedRows
              = pNode->iEstimatedRows / 10; /* Assume 10x improvement */
        }

      /* Check if we have property filters that can use property index */
      if (pNode->zProperty && pNode->zValue)
        {
          /* Convert to property index scan - highly selective */
          pNode->type = LOGICAL_INDEX_SCAN;
          pNode->iEstimatedRows
              = pNode->iEstimatedRows / 100; /* Assume 100x improvement */
        }
    }

  return SQLITE_OK;
}