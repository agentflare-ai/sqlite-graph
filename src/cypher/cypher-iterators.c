/* Iterator implementations using Volcano model (open/next/close) */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-executor.h"
#include "cypher-expressions.h"
#include "cypher-planner.h"
#include "cypher-write.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Forward declarations for write operation wrapper iterators */
static CypherIterator *createWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                                                   ExecutionContext *pContext);
static CypherIterator *mergeWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                                                  ExecutionContext *pContext);
static CypherIterator *setWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                                                ExecutionContext *pContext);
static CypherIterator *deleteWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                                                   ExecutionContext *pContext);
static CypherIterator *cypherExpandCreate (PhysicalPlanNode *pPlan,
                                           ExecutionContext *pContext);
static CypherIterator *cypherNestedLoopJoinCreate (PhysicalPlanNode *pPlan,
                                                    ExecutionContext *pContext);

CypherIterator *
cypherIteratorCreate (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  if (!pPlan || !pContext)
    return NULL;

  switch (pPlan->type)
    {
    case PHYSICAL_ALL_NODES_SCAN:
      return cypherAllNodesScanCreate (pPlan, pContext);

    case PHYSICAL_LABEL_INDEX_SCAN:
      return cypherLabelIndexScanCreate (pPlan, pContext);

    case PHYSICAL_PROPERTY_INDEX_SCAN:
      return cypherPropertyIndexScanCreate (pPlan, pContext);

    case PHYSICAL_FILTER:
      return cypherFilterCreate (pPlan, pContext);

    case PHYSICAL_PROJECTION:
      return cypherProjectionCreate (pPlan, pContext);

    case PHYSICAL_SORT:
      return cypherSortCreate (pPlan, pContext);

    case PHYSICAL_LIMIT:
      return cypherLimitCreate (pPlan, pContext);

    case PHYSICAL_CREATE:
      return createWriteIteratorWrapper (pPlan, pContext);

    case PHYSICAL_MERGE:
      return mergeWriteIteratorWrapper (pPlan, pContext);

    case PHYSICAL_SET:
      return setWriteIteratorWrapper (pPlan, pContext);

    case PHYSICAL_DELETE:
    case PHYSICAL_DETACH_DELETE:
      return deleteWriteIteratorWrapper (pPlan, pContext);

    case PHYSICAL_EXPAND:
      return cypherExpandCreate (pPlan, pContext);

    case PHYSICAL_NESTED_LOOP_JOIN:
      fprintf(stderr, "DEBUG: Creating NESTED_LOOP_JOIN iterator\n");
      return cypherNestedLoopJoinCreate (pPlan, pContext);

    default:
      /* Unsupported operator type */
      return NULL;
    }
}

void
cypherIteratorDestroy (CypherIterator *pIterator)
{
  int i;

  if (!pIterator)
    return;

  /* Safety check for invalid iterator */
  if (pIterator->xOpen == NULL && pIterator->xNext == NULL
      && pIterator->xClose == NULL && pIterator->xDestroy == NULL)
    {
      /* Invalid iterator with all NULL function pointers - skip */
      return;
    }

  /* Close iterator if still open */
  if (pIterator->bOpened && pIterator->xClose)
    {
      pIterator->xClose (pIterator);
    }

  /* Destroy child iterators */
  for (i = 0; i < pIterator->nChildren; i++)
    {
      cypherIteratorDestroy (pIterator->apChildren[i]);
    }
  sqlite3_free (pIterator->apChildren);

  /* Call iterator-specific destroy function */
  if (pIterator->xDestroy)
    {
      pIterator->xDestroy (pIterator);
    }

  sqlite3_free (pIterator);
}

static int mergeAppendProperty (char ***pazProps, CypherValue ***papValues,
                                int *pnCount, const char *zProp,
                                CypherValue *pValue);

typedef struct MergeIteratorData
{
  int bExecuted;
  MergeClauseDetails *pDetails;
} MergeIteratorData;

static int
mergeApplyMatchPropsFromDetails (const MergeClauseDetails *pDetails,
                                 MergeNodeOp *pOp, ExecutionContext *pContext)
{
  int rc = SQLITE_OK;
  int i;

  if (!pDetails || !pOp)
    return SQLITE_OK;
  if (pDetails->nMatchProps <= 0)
    return SQLITE_OK;

  for (i = 0; rc == SQLITE_OK && i < pDetails->nMatchProps; i++)
    {
      const MergePropertyEntry *pEntry = &pDetails->aMatchProps[i];
      CypherValue *pValue = NULL;

      /* Use the new expression evaluator instead of literal-only converter */
      rc = cypherEvaluateAstExpression (pEntry->pValueExpr, pContext, &pValue);
      if (rc != SQLITE_OK)
        {
          if (pValue)
            {
              cypherValueDestroy (pValue);
              sqlite3_free (pValue);
            }
          break;
        }

      rc = mergeAppendProperty (
          &pOp->azMatchProps, &pOp->apMatchValues, &pOp->nMatchProps,
          pEntry->zProperty ? pEntry->zProperty : "", pValue);
      if (rc != SQLITE_OK)
        {
          cypherValueDestroy (pValue);
          sqlite3_free (pValue);
        }
    }

  return rc;
}

static int
mergeLiteralAstToValue (CypherAst *pAst, CypherValue **ppValue)
{
  CypherValue *pValue;
  const char *zLiteral = NULL;
  int rc = SQLITE_OK;

  pValue = (CypherValue *)sqlite3_malloc (sizeof (CypherValue));
  if (!pValue)
    return SQLITE_NOMEM;
  cypherValueInit (pValue);

  if (pAst)
    {
      zLiteral = cypherAstGetValue (pAst);
      fprintf (stderr,
               "DEBUG mergeLiteralAstToValue: pAst=%p type=%d zLiteral='%s'\n",
               pAst, pAst->type, zLiteral ? zLiteral : "(null)");
    }

  if (!pAst)
    {
      cypherValueSetNull (pValue);
    }
  else if (pAst->type == CYPHER_AST_ARRAY)
    {
      /* Handle list literals [1, 2, 3] */
      int nElements = pAst->nChildren;
      CypherValue *apElements = NULL;
      int i, j;

      fprintf (
          stderr,
          "DEBUG mergeLiteralAstToValue: handling ARRAY with %d elements\n",
          nElements);

      if (nElements > 0)
        {
          apElements = (CypherValue *)sqlite3_malloc (sizeof (CypherValue)
                                                      * nElements);
          if (!apElements)
            {
              sqlite3_free (pValue);
              return SQLITE_NOMEM;
            }

          /* Recursively evaluate each element */
          for (i = 0; i < nElements; i++)
            {
              CypherValue *pElement = NULL;
              rc = mergeLiteralAstToValue (pAst->apChildren[i], &pElement);
              if (rc != SQLITE_OK)
                {
                  /* Clean up on error */
                  for (j = 0; j < i; j++)
                    {
                      cypherValueDestroy (&apElements[j]);
                    }
                  sqlite3_free (apElements);
                  sqlite3_free (pValue);
                  return rc;
                }
              /* Copy the evaluated element */
              apElements[i] = *pElement;
              sqlite3_free (pElement); /* Free the wrapper, not the content */
            }
        }

      /* cypherValueSetList takes ownership of apElements, don't free it! */
      cypherValueSetList (pValue, apElements, nElements);
    }
  else if (pAst->type == CYPHER_AST_LITERAL)
    {
      if (!zLiteral)
        {
          cypherValueSetNull (pValue);
        }
      else
        {
          size_t nLen = strlen (zLiteral);
          if ((nLen >= 2)
              && ((zLiteral[0] == '\'' && zLiteral[nLen - 1] == '\'')
                  || (zLiteral[0] == '"' && zLiteral[nLen - 1] == '"')))
            {
              char *zCopy
                  = sqlite3_mprintf ("%.*s", (int)(nLen - 2), zLiteral + 1);
              if (!zCopy)
                {
                  sqlite3_free (pValue);
                  return SQLITE_NOMEM;
                }
              rc = cypherValueSetString (pValue, zCopy);
              sqlite3_free (zCopy);
              if (rc != SQLITE_OK)
                {
                  sqlite3_free (pValue);
                  return rc;
                }
            }
          else if (sqlite3_stricmp (zLiteral, "true") == 0
                   || sqlite3_stricmp (zLiteral, "false") == 0)
            {
              cypherValueSetBoolean (pValue,
                                     sqlite3_stricmp (zLiteral, "true") == 0);
            }
          else if (sqlite3_stricmp (zLiteral, "null") == 0)
            {
              cypherValueSetNull (pValue);
            }
          else
            {
              /* Check if it's a valid number */
              const char *p = zLiteral;
              int bValid = 1;
              int bFloat = 0;

              /* Skip optional leading minus */
              if (*p == '-' || *p == '+')
                p++;

              /* Must have at least one digit */
              if (!*p || !isdigit (*p))
                {
                  bValid = 0;
                }
              else
                {
                  /* Parse integer part */
                  while (isdigit (*p))
                    p++;

                  /* Check for decimal point */
                  if (*p == '.')
                    {
                      bFloat = 1;
                      p++;
                      /* Must have at least one digit after decimal */
                      if (!isdigit (*p))
                        bValid = 0;
                      while (isdigit (*p))
                        p++;
                    }

                  /* Check for exponent */
                  if (*p == 'e' || *p == 'E')
                    {
                      bFloat = 1;
                      p++;
                      if (*p == '-' || *p == '+')
                        p++;
                      if (!isdigit (*p))
                        bValid = 0;
                      while (isdigit (*p))
                        p++;
                    }

                  /* Must be at end of string */
                  if (*p != '\0')
                    bValid = 0;
                }

              if (bValid)
                {
                  if (bFloat)
                    {
                      cypherValueSetFloat (pValue, atof (zLiteral));
                    }
                  else
                    {
                      cypherValueSetInteger (pValue, atoll (zLiteral));
                    }
                }
              else
                {
                  /* Not a valid number, treat as string */
                  rc = cypherValueSetString (pValue, zLiteral);
                  if (rc != SQLITE_OK)
                    {
                      sqlite3_free (pValue);
                      return rc;
                    }
                }
            }
        }
    }
  else
    {
      rc = cypherValueSetString (pValue, zLiteral);
      if (rc != SQLITE_OK)
        {
          sqlite3_free (pValue);
          return rc;
        }
    }

  *ppValue = pValue;
  fprintf (stderr, "DEBUG mergeLiteralAstToValue result: type=%d\n",
           pValue->type);
  if (pValue->type == CYPHER_VALUE_STRING)
    {
      fprintf (stderr, "DEBUG   string value: '%s'\n",
               pValue->u.zString ? pValue->u.zString : "(null)");
    }
  else if (pValue->type == CYPHER_VALUE_INTEGER)
    {
      fprintf (stderr, "DEBUG   integer value: %lld\n", pValue->u.iInteger);
    }
  return SQLITE_OK;
}

static int
mergeAppendProperty (char ***pazProps, CypherValue ***papValues, int *pnCount,
                     const char *zProp, CypherValue *pValue)
{
  char **azNew;
  CypherValue **apNew;

  azNew
      = (char **)sqlite3_realloc (*pazProps, sizeof (char *) * (*pnCount + 1));
  if (!azNew)
    return SQLITE_NOMEM;
  *pazProps = azNew;

  apNew = (CypherValue **)sqlite3_realloc (*papValues, sizeof (CypherValue *)
                                                           * (*pnCount + 1));
  if (!apNew)
    return SQLITE_NOMEM;
  *papValues = apNew;

  (*pazProps)[*pnCount] = sqlite3_mprintf ("%s", zProp ? zProp : "");
  if (!(*pazProps)[*pnCount])
    {
      return SQLITE_NOMEM;
    }
  (*papValues)[*pnCount] = pValue;
  (*pnCount)++;
  return SQLITE_OK;
}

static int
mergeExtractPropertiesFromAst (CypherAst *pMapAst, char ***pazProps,
                               CypherValue ***papValues, int *pnProps)
{
  int i;
  int rc = SQLITE_OK;

  if (!pMapAst || !cypherAstIsType (pMapAst, CYPHER_AST_MAP))
    {
      return SQLITE_OK;
    }

  for (i = 0; i < pMapAst->nChildren; i++)
    {
      CypherAst *pPair = pMapAst->apChildren[i];
      const char *zProp = NULL;
      CypherAst *pValueAst = NULL;
      CypherValue *pValueObj = NULL;

      if (!cypherAstIsType (pPair, CYPHER_AST_PROPERTY_PAIR))
        continue;
      zProp = cypherAstGetValue (pPair);
      if (pPair->nChildren > 0)
        {
          pValueAst = pPair->apChildren[0];
        }
      rc = mergeLiteralAstToValue (pValueAst, &pValueObj);
      if (rc != SQLITE_OK)
        return rc;
      rc = mergeAppendProperty (pazProps, papValues, pnProps,
                                zProp ? zProp : "", pValueObj);
      if (rc != SQLITE_OK)
        {
          cypherValueDestroy (pValueObj);
          sqlite3_free (pValueObj);
          return rc;
        }
    }
  return SQLITE_OK;
}

static int
mergeApplySetOpsForTarget (const char *zJson, const char *zTarget,
                           char ***pazProps, CypherValue ***papValues,
                           int *pnProps)
{
  const char *p;
  int rc = SQLITE_OK;

  if (!zJson || !zTarget || !*zTarget)
    return SQLITE_OK;
  p = zJson;
  while (isspace ((unsigned char)*p))
    p++;
  if (*p == '\0')
    return SQLITE_OK;
  if (*p != '[')
    return SQLITE_FORMAT;
  p++;

  while (1)
    {
      CypherValue map;
      const char *zStart;
      size_t nLength;
      char *zObject;
      const char *zPropName = NULL;
      const char *zTargetName = NULL;
      CypherValue *pValueEntry = NULL;
      int i;

      while (isspace ((unsigned char)*p))
        p++;
      if (*p == ']')
        break;
      if (*p == '\0')
        return SQLITE_FORMAT;
      if (*p != '{')
        return SQLITE_FORMAT;
      zStart = p;
      nLength = 0;
      int depth = 0;
      do
        {
          if (*p == '{')
            depth++;
          else if (*p == '}')
            depth--;
          p++;
          nLength++;
        }
      while (*p && depth > 0);
      if (depth != 0)
        return SQLITE_FORMAT;

      zObject = (char *)sqlite3_malloc (nLength + 1);
      if (!zObject)
        return SQLITE_NOMEM;
      memcpy (zObject, zStart, nLength);
      zObject[nLength] = '\0';

      cypherValueInit (&map);
      rc = cypherParseJsonProperties (zObject, &map);
      sqlite3_free (zObject);
      if (rc != SQLITE_OK)
        {
          cypherValueDestroy (&map);
          return rc;
        }

      if (map.type == CYPHER_VALUE_MAP)
        {
          for (i = 0; i < map.u.map.nPairs; i++)
            {
              const char *zKey = map.u.map.azKeys[i];
              CypherValue *pVal = &map.u.map.apValues[i];
              if (strcmp (zKey, "target") == 0
                  && pVal->type == CYPHER_VALUE_STRING)
                {
                  zTargetName = pVal->u.zString;
                }
              else if (strcmp (zKey, "property") == 0
                       && pVal->type == CYPHER_VALUE_STRING)
                {
                  zPropName = pVal->u.zString;
                }
              else if (strcmp (zKey, "value") == 0)
                {
                  pValueEntry = pVal;
                }
            }
        }

      if (zTargetName && zPropName && pValueEntry
          && strcmp (zTargetName, zTarget) == 0)
        {
          CypherValue *pCopy = cypherValueCopy (pValueEntry);
          if (!pCopy)
            {
              cypherValueDestroy (&map);
              return SQLITE_NOMEM;
            }
          rc = mergeAppendProperty (pazProps, papValues, pnProps, zPropName,
                                    pCopy);
          if (rc != SQLITE_OK)
            {
              cypherValueDestroy (pCopy);
              sqlite3_free (pCopy);
              cypherValueDestroy (&map);
              return rc;
            }
        }

      cypherValueDestroy (&map);
      while (isspace ((unsigned char)*p))
        p++;
      if (*p == ',')
        {
          p++;
          continue;
        }
    }

  return SQLITE_OK;
}

static int
mergeInitNodeOpFromPattern (const MergePatternNode *pPattern,
                            const char *zAliasFallback,
                            const char *zOnCreateJson,
                            const char *zOnMatchJson, MergeNodeOp *pOp)
{
  const char *zAlias = NULL;
  int rc = SQLITE_OK;
  int i;

  if (!pOp)
    return SQLITE_MISUSE;

  if (pPattern && pPattern->zAlias)
    {
      zAlias = pPattern->zAlias;
    }
  else
    {
      zAlias = zAliasFallback;
    }

  if (zAlias)
    {
      pOp->zVariable = sqlite3_mprintf ("%s", zAlias);
      if (!pOp->zVariable)
        return SQLITE_NOMEM;
    }

  if (pPattern && pPattern->nLabels > 0 && pPattern->azLabels)
    {
      pOp->azLabels
          = (char **)sqlite3_malloc (sizeof (char *) * pPattern->nLabels);
      if (!pOp->azLabels)
        return SQLITE_NOMEM;
      memset (pOp->azLabels, 0, sizeof (char *) * pPattern->nLabels);
      for (i = 0; i < pPattern->nLabels; i++)
        {
          pOp->azLabels[i] = sqlite3_mprintf ("%s", pPattern->azLabels[i]);
          if (!pOp->azLabels[i])
            return SQLITE_NOMEM;
        }
      pOp->nLabels = pPattern->nLabels;
    }

  if (pPattern)
    {
      rc = mergeExtractPropertiesFromAst (
          pPattern->pProperties, &pOp->azMatchProps, &pOp->apMatchValues,
          &pOp->nMatchProps);
      if (rc != SQLITE_OK)
        return rc;
    }

  if (zAlias)
    {
      rc = mergeApplySetOpsForTarget (
          zOnCreateJson, zAlias, &pOp->azOnCreateProps, &pOp->apOnCreateValues,
          &pOp->nOnCreateProps);
      if (rc != SQLITE_OK)
        return rc;
      rc = mergeApplySetOpsForTarget (
          zOnMatchJson, zAlias, &pOp->azOnMatchProps, &pOp->apOnMatchValues,
          &pOp->nOnMatchProps);
      if (rc != SQLITE_OK)
        return rc;
    }

  return rc;
}

static int
mergePopulateRelOpFromDetails (const MergeClauseDetails *pDetails,
                               PhysicalPlanNode *pPlan, MergeRelOp *pRelOp)
{
  int rc;
  const char *zFromAlias = NULL;
  const char *zToAlias = NULL;

  if (!pDetails || !pRelOp || !pPlan)
    return SQLITE_MISUSE;
  if (!pDetails->bHasRelationship)
    return SQLITE_MISUSE;

  pRelOp->pFromNode = cypherMergeNodeOpCreate ();
  pRelOp->pToNode = cypherMergeNodeOpCreate ();
  if (!pRelOp->pFromNode || !pRelOp->pToNode)
    return SQLITE_NOMEM;

  zFromAlias = pDetails->targetNode.zAlias ? pDetails->targetNode.zAlias
                                           : pPlan->zAlias;
  zToAlias
      = pDetails->relatedNode.zAlias ? pDetails->relatedNode.zAlias : NULL;

  rc = mergeInitNodeOpFromPattern (&pDetails->targetNode, zFromAlias,
                                   pPlan->zOnCreateJson, pPlan->zOnMatchJson,
                                   pRelOp->pFromNode);
  if (rc != SQLITE_OK)
    return rc;

  rc = mergeInitNodeOpFromPattern (&pDetails->relatedNode, zToAlias,
                                   pPlan->zOnCreateJson, pPlan->zOnMatchJson,
                                   pRelOp->pToNode);
  if (rc != SQLITE_OK)
    return rc;

  if (pDetails->relationship.zAlias)
    {
      pRelOp->zRelVar = sqlite3_mprintf ("%s", pDetails->relationship.zAlias);
      if (!pRelOp->zRelVar)
        return SQLITE_NOMEM;
    }
  else if (pPlan->zAlias)
    {
      pRelOp->zRelVar = sqlite3_mprintf ("%s", pPlan->zAlias);
      if (!pRelOp->zRelVar)
        return SQLITE_NOMEM;
    }

  if (pDetails->relationship.zType)
    {
      pRelOp->zRelType = sqlite3_mprintf ("%s", pDetails->relationship.zType);
      if (!pRelOp->zRelType)
        return SQLITE_NOMEM;
    }

  pRelOp->iDirection = pDetails->relationship.iDirection;

  rc = mergeExtractPropertiesFromAst (
      pDetails->relationship.pProperties, &pRelOp->azMatchProps,
      &pRelOp->apMatchValues, &pRelOp->nMatchProps);
  if (rc != SQLITE_OK)
    return rc;

  if (pDetails->relationship.zAlias)
    {
      rc = mergeApplySetOpsForTarget (
          pPlan->zOnCreateJson, pDetails->relationship.zAlias,
          &pRelOp->azOnCreateProps, &pRelOp->apOnCreateValues,
          &pRelOp->nOnCreateProps);
      if (rc != SQLITE_OK)
        return rc;
      rc = mergeApplySetOpsForTarget (
          pPlan->zOnMatchJson, pDetails->relationship.zAlias,
          &pRelOp->azOnMatchProps, &pRelOp->apOnMatchValues,
          &pRelOp->nOnMatchProps);
      if (rc != SQLITE_OK)
        return rc;
    }

  return SQLITE_OK;
}

/*
** AllNodesScan iterator implementation.
** Scans all nodes in the graph sequentially.
*/

typedef struct AllNodesScanData
{
  sqlite3_stmt *pStmt; /* SQL statement for node iteration */
} AllNodesScanData;

static int
allNodesScanOpen (CypherIterator *pIterator)
{
  AllNodesScanData *pData = (AllNodesScanData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  GraphVtab *pGraph = pIterator->pContext->pGraph;
  char *zSql;
  int rc;

  if (!pGraph)
    return SQLITE_ERROR;

  /* If a label is specified, filter by it */
  if (pPlan && pPlan->zLabel)
    {
      /* Labels are stored as JSON array, check if label exists in array */
      zSql = sqlite3_mprintf (
          "SELECT id FROM %s WHERE json_array_length(json_extract(labels, "
          "'$')) > 0 "
          "AND EXISTS (SELECT 1 FROM json_each(labels) WHERE value = %Q)",
          pGraph->zNodeTableName, pPlan->zLabel);
    }
  else
    {
      zSql = sqlite3_mprintf ("SELECT id FROM %s", pGraph->zNodeTableName);
    }

  rc = sqlite3_prepare_v2 (pGraph->pDb, zSql, -1, &pData->pStmt, 0);
  sqlite3_free (zSql);
  if (rc != SQLITE_OK)
    return rc;

  pIterator->bOpened = 1;
  pIterator->bEof = 0;

  return SQLITE_OK;
}

static int
allNodesScanNext (CypherIterator *pIterator, CypherResult *pResult)
{
  AllNodesScanData *pData = (AllNodesScanData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  CypherValue nodeValue;
  int rc;

  if (pIterator->bEof)
    return SQLITE_DONE;

  rc = sqlite3_step (pData->pStmt);
  if (rc != SQLITE_ROW)
    {
      pIterator->bEof = 1;
      return SQLITE_DONE;
    }

  /* Reset result for new row */
  pResult->nColumns = 0;

  memset (&nodeValue, 0, sizeof (nodeValue));
  nodeValue.type = CYPHER_VALUE_NODE;
  nodeValue.u.iNodeId = sqlite3_column_int64 (pData->pStmt, 0);

  rc = cypherResultAddColumn (pResult, pPlan->zAlias ? pPlan->zAlias : "node",
                              &nodeValue);
  if (rc != SQLITE_OK)
    return rc;

  pIterator->nRowsProduced++;

  return SQLITE_OK;
}

static int
allNodesScanClose (CypherIterator *pIterator)
{
  AllNodesScanData *pData = (AllNodesScanData *)pIterator->pIterData;
  sqlite3_finalize (pData->pStmt);
  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void
allNodesScanDestroy (CypherIterator *pIterator)
{
  sqlite3_free (pIterator->pIterData);
}

CypherIterator *
cypherAllNodesScanCreate (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  AllNodesScanData *pData;

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;

  pData = sqlite3_malloc (sizeof (AllNodesScanData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (AllNodesScanData));

  /* Set up iterator */
  pIterator->xOpen = allNodesScanOpen;
  pIterator->xNext = allNodesScanNext;
  pIterator->xClose = allNodesScanClose;
  pIterator->xDestroy = allNodesScanDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

/*
** LabelIndexScan iterator implementation.
** Scans nodes with a specific label using the label index.
*/

typedef struct LabelIndexScanData
{
  sqlite3_stmt *pStmt; /* SQL statement for node iteration */
  const char *zLabel;  /* Label to filter by */
} LabelIndexScanData;

static int
labelIndexScanOpen (CypherIterator *pIterator)
{
  LabelIndexScanData *pData = (LabelIndexScanData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  GraphVtab *pGraph = pIterator->pContext->pGraph;
  char *zSql;
  int rc;

  if (!pGraph || !pPlan->zLabel)
    return SQLITE_ERROR;

  pData->zLabel = pPlan->zLabel;

  /* Check if zLabel is a JSON array of multiple labels */
  if (pPlan->zLabel[0] == '[') {
    /* Multiple labels - parse JSON array and build SQL with AND conditions */
    zSql = sqlite3_mprintf("SELECT id FROM %s_nodes WHERE 1=1", pGraph->zTableName);

    /* Simple JSON array parser for labels */
    const char *p = pPlan->zLabel + 1;  /* Skip opening [ */
    while (*p && *p != ']') {
      /* Skip whitespace and quotes */
      while (*p && (*p == ' ' || *p == ',' || *p == '"')) p++;
      if (*p == ']') break;

      /* Extract label */
      const char *zLabelStart = p;
      while (*p && *p != '"' && *p != ',' && *p != ']') p++;

      if (p > zLabelStart) {
        char *zLabel = sqlite3_mprintf("%.*s", (int)(p - zLabelStart), zLabelStart);
        char *zTmp = sqlite3_mprintf("%s AND labels LIKE '%%%%\"%q\"%%%%'", zSql, zLabel);
        sqlite3_free(zLabel);
        sqlite3_free(zSql);
        zSql = zTmp;
      }

      /* Skip to next label */
      while (*p && *p != ',' && *p != ']') p++;
    }
  } else {
    /* Single label (backwards compatibility) */
    zSql = sqlite3_mprintf (
        "SELECT id FROM %s_nodes WHERE labels LIKE '%%%%\"%q\"%%%%'",
        pGraph->zTableName, pData->zLabel);
  }

  rc = sqlite3_prepare_v2 (pGraph->pDb, zSql, -1, &pData->pStmt, 0);
  sqlite3_free (zSql);
  if (rc != SQLITE_OK)
    return rc;

  pIterator->bOpened = 1;
  pIterator->bEof = 0;

  return SQLITE_OK;
}

static int
labelIndexScanNext (CypherIterator *pIterator, CypherResult *pResult)
{
  LabelIndexScanData *pData = (LabelIndexScanData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  CypherValue nodeValue;
  int rc;

  if (pIterator->bEof)
    return SQLITE_DONE;

  rc = sqlite3_step (pData->pStmt);
  if (rc != SQLITE_ROW)
    {
      pIterator->bEof = 1;
      return SQLITE_DONE;
    }

  /* Reset result for new row */
  pResult->nColumns = 0;

  memset (&nodeValue, 0, sizeof (nodeValue));
  nodeValue.type = CYPHER_VALUE_NODE;
  nodeValue.u.iNodeId = sqlite3_column_int64 (pData->pStmt, 0);

  rc = cypherResultAddColumn (pResult, pPlan->zAlias ? pPlan->zAlias : "node",
                              &nodeValue);
  if (rc != SQLITE_OK)
    return rc;

  pIterator->nRowsProduced++;

  return SQLITE_OK;
}

static int
labelIndexScanClose (CypherIterator *pIterator)
{
  LabelIndexScanData *pData = (LabelIndexScanData *)pIterator->pIterData;
  sqlite3_finalize (pData->pStmt);
  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void
labelIndexScanDestroy (CypherIterator *pIterator)
{
  sqlite3_free (pIterator->pIterData);
}

CypherIterator *
cypherLabelIndexScanCreate (PhysicalPlanNode *pPlan,
                            ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  LabelIndexScanData *pData;

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;

  pData = sqlite3_malloc (sizeof (LabelIndexScanData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (LabelIndexScanData));

  /* Set up iterator */
  pIterator->xOpen = labelIndexScanOpen;
  pIterator->xNext = labelIndexScanNext;
  pIterator->xClose = labelIndexScanClose;
  pIterator->xDestroy = labelIndexScanDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

/*
** PropertyIndexScan iterator implementation.
** Scans nodes/relationships with specific property values using property
*indexes.
*/

typedef struct PropertyIndexScanData
{
  const char *zProperty; /* Property to filter by */
  const char *zValue;    /* Value to match */
  sqlite3_stmt *pStmt;   /* SQL statement for property lookup */
} PropertyIndexScanData;

static int
propertyIndexScanOpen (CypherIterator *pIterator)
{
  PropertyIndexScanData *pData = (PropertyIndexScanData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  GraphVtab *pGraph = pIterator->pContext->pGraph;

  if (!pGraph || !pPlan->zProperty)
    return SQLITE_ERROR;

  pData->zProperty = pPlan->zProperty;
  pData->zValue = pPlan->zValue;

  char *zSql
      = sqlite3_mprintf ("SELECT id FROM %s_nodes WHERE "
                         "json_extract(properties, '$.%s') = %Q",
                         pGraph->zTableName, pData->zProperty, pData->zValue);

  if (!zSql)
    {
      return SQLITE_NOMEM;
    }

  int rc = sqlite3_prepare_v2 (pGraph->pDb, zSql, -1, &pData->pStmt, NULL);
  sqlite3_free (zSql);

  if (rc != SQLITE_OK)
    {
      return rc;
    }

  pIterator->bOpened = 1;
  pIterator->bEof = 0;

  return SQLITE_OK;
}

static int
propertyIndexScanNext (CypherIterator *pIterator, CypherResult *pResult)
{
  PropertyIndexScanData *pData = (PropertyIndexScanData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  CypherValue nodeValue;
  int rc;

  if (pIterator->bEof)
    return SQLITE_DONE;

  rc = sqlite3_step (pData->pStmt);
  if (rc != SQLITE_ROW)
    {
      pIterator->bEof = 1;
      return SQLITE_DONE;
    }

  memset (&nodeValue, 0, sizeof (nodeValue));
  nodeValue.type = CYPHER_VALUE_NODE;
  nodeValue.u.iNodeId = sqlite3_column_int64 (pData->pStmt, 0);

  rc = cypherResultAddColumn (pResult, pPlan->zAlias ? pPlan->zAlias : "node",
                              &nodeValue);
  if (rc != SQLITE_OK)
    return rc;

  pIterator->nRowsProduced++;

  return SQLITE_OK;
}

static int
propertyIndexScanClose (CypherIterator *pIterator)
{
  PropertyIndexScanData *pData = (PropertyIndexScanData *)pIterator->pIterData;
  sqlite3_finalize (pData->pStmt);
  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void
propertyIndexScanDestroy (CypherIterator *pIterator)
{
  sqlite3_free (pIterator->pIterData);
}

CypherIterator *
cypherPropertyIndexScanCreate (PhysicalPlanNode *pPlan,
                               ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  PropertyIndexScanData *pData;

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;

  pData = sqlite3_malloc (sizeof (PropertyIndexScanData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (PropertyIndexScanData));

  /* Set up iterator */
  pIterator->xOpen = propertyIndexScanOpen;
  pIterator->xNext = propertyIndexScanNext;
  pIterator->xClose = propertyIndexScanClose;
  pIterator->xDestroy = propertyIndexScanDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

/*
** Stub implementations for other iterators.
** These will be implemented as needed.
*/

/* Filter iterator implementation */
typedef struct FilterIteratorData
{
  CypherIterator *pSource;   /* Source iterator */
  CypherExpression *pFilter; /* Filter expression */
} FilterIteratorData;

/* Helper to get a property value from a node */
static char *
cypherValueGetProperty (CypherValue *pValue, const char *zProperty,
                        ExecutionContext *pContext)
{
  if (!pValue || pValue->type != CYPHER_VALUE_NODE || !zProperty || !pContext)
    {
      return NULL;
    }

  /* Query the properties column from the nodes table */
  sqlite3_stmt *stmt;
  char *zSql = sqlite3_mprintf (
      "SELECT json_extract(properties, '$.%q') FROM %s WHERE id = %lld",
      zProperty,
      pContext->pGraph->zNodeTableName ? pContext->pGraph->zNodeTableName
                                       : "graph_nodes",
      pValue->u.iNodeId);

  if (!zSql)
    return NULL;

  int rc = sqlite3_prepare_v2 (pContext->pDb, zSql, -1, &stmt, NULL);
  sqlite3_free (zSql);

  if (rc != SQLITE_OK)
    return NULL;

  char *zResult = NULL;
  if (sqlite3_step (stmt) == SQLITE_ROW)
    {
      const char *zValue = (const char *)sqlite3_column_text (stmt, 0);
      if (zValue)
        {
          zResult = sqlite3_mprintf ("%s", zValue);
        }
    }

  sqlite3_finalize (stmt);
  return zResult;
}

static int
filterIteratorOpen (CypherIterator *pIterator)
{
  FilterIteratorData *pData = (FilterIteratorData *)pIterator->pIterData;
  return pData->pSource->xOpen (pData->pSource);
}

static int
filterIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  FilterIteratorData *pData = (FilterIteratorData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  CypherValue filterResult;
  int rc;

  /* Keep fetching from source until we find a matching row */
  while ((rc = pData->pSource->xNext (pData->pSource, pResult)) == SQLITE_OK)
    {
      /* Use simple property comparison if available (faster path) */
      if (!pData->pFilter && pPlan->zProperty && pPlan->zValue)
        {
          /* Find the column with the variable name */
          const char *zVarName = pPlan->zAlias;
          CypherValue *pValue = NULL;

          for (int i = 0; i < pResult->nColumns; i++)
            {
              if (zVarName && pResult->azColumnNames[i]
                  && strcmp (pResult->azColumnNames[i], zVarName) == 0)
                {
                  pValue = &pResult->aValues[i];
                  break;
                }
            }

          if (pValue && pValue->type == CYPHER_VALUE_NODE)
            {
              /* Get property from node */
              char *zPropValue = cypherValueGetProperty (
                  pValue, pPlan->zProperty, pIterator->pContext);
              if (zPropValue)
                {
                  int passes = 0;
                  /* Normalize comparison value - remove quotes if present */
                  const char *zCompareValue = pPlan->zValue;
                  char *zNormalizedValue = NULL;
                  if (zCompareValue && (zCompareValue[0] == '\'' || zCompareValue[0] == '"'))
                    {
                      /* Strip quotes from literal string */
                      int len = strlen(zCompareValue);
                      if (len >= 2)
                        {
                          zNormalizedValue = sqlite3_mprintf("%.*s", len - 2, zCompareValue + 1);
                          zCompareValue = zNormalizedValue;
                        }
                    }

                  /* Compare based on operator in iFlags */
                  /* 1=Equal, 2=Greater, 3=Less, 4=GreaterEqual, 5=LessEqual,
                   * 6=NotEqual */
                  int cmp = strcmp (zPropValue, zCompareValue);
                  if (pPlan->iFlags == 1)
                    passes = (cmp == 0);
                  else if (pPlan->iFlags == 2)
                    passes = (atoi (zPropValue) > atoi (zCompareValue));
                  else if (pPlan->iFlags == 3)
                    passes = (atoi (zPropValue) < atoi (zCompareValue));
                  else if (pPlan->iFlags == 4)
                    passes = (atoi (zPropValue) >= atoi (zCompareValue));
                  else if (pPlan->iFlags == 5)
                    passes = (atoi (zPropValue) <= atoi (zCompareValue));
                  else if (pPlan->iFlags == 6)
                    passes = (cmp != 0);

                  sqlite3_free (zPropValue);
                  if (zNormalizedValue)
                    sqlite3_free (zNormalizedValue);

                  if (passes)
                    {
                      return SQLITE_OK;
                    }
                }
            }
        }
      else if (pData->pFilter)
        {
          /* Evaluate full filter expression */
          rc = cypherExpressionEvaluate (pData->pFilter, pIterator->pContext,
                                         &filterResult);
          if (rc != SQLITE_OK)
            return rc;

          /* Check if filter passes (truthy value) */
          if (!cypherValueIsNull (&filterResult)
              && (filterResult.type != CYPHER_VALUE_BOOLEAN
                  || filterResult.u.bBoolean))
            {
              cypherValueDestroy (&filterResult);
              return SQLITE_OK;
            }

          cypherValueDestroy (&filterResult);
        }
    }

  return rc;
}

static int
filterIteratorClose (CypherIterator *pIterator)
{
  FilterIteratorData *pData = (FilterIteratorData *)pIterator->pIterData;
  return pData->pSource->xClose (pData->pSource);
}

static void
filterIteratorDestroy (CypherIterator *pIterator)
{
  if (pIterator)
    {
      FilterIteratorData *pData = (FilterIteratorData *)pIterator->pIterData;
      if (pData)
        {
          if (pData->pSource)
            {
              cypherIteratorDestroy (pData->pSource);
            }
          sqlite3_free (pData);
        }
      /* Don't free pIterator - cypherIteratorDestroy will do that */
    }
}

CypherIterator *
cypherFilterCreate (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  FilterIteratorData *pData;

  /* Filter requires either pFilterExpr/pExecState OR (zProperty + zValue) for
   * simple filters */
  if (!pPlan || !pPlan->pChild)
    return NULL;
  if (!pPlan->pFilterExpr && !pPlan->pExecState
      && (!pPlan->zProperty || !pPlan->zValue))
    return NULL;

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;

  pData = sqlite3_malloc (sizeof (FilterIteratorData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (FilterIteratorData));

  /* Create source iterator */
  pData->pSource = cypherIteratorCreate (pPlan->pChild, pContext);
  if (!pData->pSource)
    {
      sqlite3_free (pData);
      sqlite3_free (pIterator);
      return NULL;
    }

  pData->pFilter = pPlan->pFilterExpr;

  /* Set up iterator */
  pIterator->xOpen = filterIteratorOpen;
  pIterator->xNext = filterIteratorNext;
  pIterator->xClose = filterIteratorClose;
  pIterator->xDestroy = filterIteratorDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

/* Nested Loop Join iterator implementation (for Cartesian product) */
typedef struct NestedLoopJoinIteratorData
{
  CypherIterator *pLeft;           /* Left (outer) iterator */
  CypherIterator *pRight;          /* Right (inner) iterator */
  CypherResult leftResult;         /* Current row from left */
  int bLeftHasRow;                 /* Whether we have a valid left row */
} NestedLoopJoinIteratorData;

static int
nestedLoopJoinIteratorOpen (CypherIterator *pIterator)
{
  NestedLoopJoinIteratorData *pData
      = (NestedLoopJoinIteratorData *)pIterator->pIterData;

  fprintf(stderr, "DEBUG: NLJ Open - starting\n");

  /* Open both child iterators */
  int rc = pData->pLeft->xOpen (pData->pLeft);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "DEBUG: NLJ Open - left failed with rc=%d\n", rc);
      return rc;
    }

  rc = pData->pRight->xOpen (pData->pRight);
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "DEBUG: NLJ Open - right failed with rc=%d\n", rc);
      pData->pLeft->xClose (pData->pLeft);
      return rc;
    }

  /* Get first row from left */
  fprintf(stderr, "DEBUG: NLJ Open - getting first left row\n");
  memset (&pData->leftResult, 0, sizeof (CypherResult));
  rc = pData->pLeft->xNext (pData->pLeft, &pData->leftResult);
  pData->bLeftHasRow = (rc == SQLITE_OK);
  fprintf(stderr, "DEBUG: NLJ Open - left has row: %d\n", pData->bLeftHasRow);

  pIterator->bOpened = 1;
  fprintf(stderr, "DEBUG: NLJ Open - complete\n");
  return SQLITE_OK;
}

static int
nestedLoopJoinIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  NestedLoopJoinIteratorData *pData
      = (NestedLoopJoinIteratorData *)pIterator->pIterData;
  CypherResult rightResult;
  int rc;

  fprintf(stderr, "DEBUG: NLJ Next - called\n");

  /* Keep producing results until we run out */
  while (pData->bLeftHasRow)
    {
      /* Try to get next row from right */
      fprintf(stderr, "DEBUG: NLJ Next - getting right row\n");
      memset (&rightResult, 0, sizeof (CypherResult));
      rc = pData->pRight->xNext (pData->pRight, &rightResult);
      fprintf(stderr, "DEBUG: NLJ Next - right rc=%d\n", rc);

      if (rc == SQLITE_OK)
        {
          /* Combine left and right results */
          memset (pResult, 0, sizeof (CypherResult));

          /* Copy columns from left (cypherResultAddColumn will deep copy) */
          for (int i = 0; i < pData->leftResult.nColumns; i++)
            {
              char *zColName
                  = sqlite3_mprintf ("%s", pData->leftResult.azColumnNames[i]);
              cypherResultAddColumn (pResult, zColName, &pData->leftResult.aValues[i]);
              sqlite3_free (zColName);
            }

          /* Copy columns from right (cypherResultAddColumn will deep copy) */
          for (int i = 0; i < rightResult.nColumns; i++)
            {
              char *zColName
                  = sqlite3_mprintf ("%s", rightResult.azColumnNames[i]);
              cypherResultAddColumn (pResult, zColName, &rightResult.aValues[i]);
              sqlite3_free (zColName);
            }

          cypherResultClear (&rightResult);
          return SQLITE_OK;
        }
      else
        {
          /* Right iterator exhausted - advance left and reset right */
          cypherResultClear (&pData->leftResult);
          memset (&pData->leftResult, 0, sizeof (CypherResult));
          rc = pData->pLeft->xNext (pData->pLeft, &pData->leftResult);

          if (rc == SQLITE_OK)
            {
              /* Got next left row - reset right iterator */
              pData->pRight->xClose (pData->pRight);
              pData->pRight->xOpen (pData->pRight);
              pData->bLeftHasRow = 1;
            }
          else
            {
              /* Left iterator also exhausted */
              pData->bLeftHasRow = 0;
              return rc;
            }
        }
    }

  return SQLITE_DONE;
}

static int
nestedLoopJoinIteratorClose (CypherIterator *pIterator)
{
  NestedLoopJoinIteratorData *pData
      = (NestedLoopJoinIteratorData *)pIterator->pIterData;

  pData->pLeft->xClose (pData->pLeft);
  pData->pRight->xClose (pData->pRight);
  cypherResultClear (&pData->leftResult);

  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void
nestedLoopJoinIteratorDestroy (CypherIterator *pIterator)
{
  NestedLoopJoinIteratorData *pData
      = (NestedLoopJoinIteratorData *)pIterator->pIterData;

  if (pData)
    {
      if (pData->pLeft)
        cypherIteratorDestroy (pData->pLeft);
      if (pData->pRight)
        cypherIteratorDestroy (pData->pRight);
      cypherResultClear (&pData->leftResult);
      sqlite3_free (pData);
    }
}

static CypherIterator *
cypherNestedLoopJoinCreate (PhysicalPlanNode *pPlan,
                            ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  NestedLoopJoinIteratorData *pData;

  /* Need at least 2 children for join */
  if (!pPlan || pPlan->nChildren < 2)
    return NULL;

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;

  pData = sqlite3_malloc (sizeof (NestedLoopJoinIteratorData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (NestedLoopJoinIteratorData));

  /* Create left and right child iterators */
  pData->pLeft = cypherIteratorCreate (pPlan->apChildren[0], pContext);
  pData->pRight = cypherIteratorCreate (pPlan->apChildren[1], pContext);

  if (!pData->pLeft || !pData->pRight)
    {
      if (pData->pLeft)
        cypherIteratorDestroy (pData->pLeft);
      if (pData->pRight)
        cypherIteratorDestroy (pData->pRight);
      sqlite3_free (pData);
      sqlite3_free (pIterator);
      return NULL;
    }

  /* Set up iterator */
  pIterator->xOpen = nestedLoopJoinIteratorOpen;
  pIterator->xNext = nestedLoopJoinIteratorNext;
  pIterator->xClose = nestedLoopJoinIteratorClose;
  pIterator->xDestroy = nestedLoopJoinIteratorDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

/* Projection iterator implementation */
typedef struct ProjectionIteratorData
{
  CypherIterator *pSource;          /* Source iterator */
  CypherExpression **apProjections; /* Projection expressions */
  int nProjections;                 /* Number of projections */
} ProjectionIteratorData;

static int
projectionIteratorOpen (CypherIterator *pIterator)
{
  ProjectionIteratorData *pData
      = (ProjectionIteratorData *)pIterator->pIterData;
  return pData->pSource->xOpen (pData->pSource);
}

static int
projectionIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  ProjectionIteratorData *pData
      = (ProjectionIteratorData *)pIterator->pIterData;
  int rc, i;

  /* Pass-through mode: if no projections, just pass source result directly */
  if (pData->nProjections == 0 || !pData->apProjections)
    {
      rc = pData->pSource->xNext (pData->pSource, pResult);
      return rc;
    }

  /* Projection mode: evaluate projection expressions */
  CypherResult sourceResult;

  /* Get next row from source */
  rc = pData->pSource->xNext (pData->pSource, &sourceResult);
  if (rc != SQLITE_OK)
    return rc;

  /* Create new result with projections */
  memset (pResult, 0, sizeof (CypherResult));

  for (i = 0; i < pData->nProjections; i++)
    {
      CypherValue projValue;

      /* Evaluate projection expression */
      rc = cypherExpressionEvaluate (pData->apProjections[i],
                                     pIterator->pContext, &projValue);
      if (rc != SQLITE_OK)
        {
          cypherResultDestroy (&sourceResult);
          return rc;
        }

      /* Add to result */
      char *zColName = sqlite3_mprintf ("col%d", i);
      if (!zColName)
        {
          cypherValueDestroy (&projValue);
          cypherResultDestroy (&sourceResult);
          return SQLITE_NOMEM;
        }

      rc = cypherResultAddColumn (pResult, zColName, &projValue);
      sqlite3_free (zColName);
      cypherValueDestroy (&projValue);

      if (rc != SQLITE_OK)
        {
          cypherResultDestroy (&sourceResult);
          return rc;
        }
    }

  cypherResultDestroy (&sourceResult);
  return SQLITE_OK;
}

static int
projectionIteratorClose (CypherIterator *pIterator)
{
  ProjectionIteratorData *pData
      = (ProjectionIteratorData *)pIterator->pIterData;
  return pData->pSource->xClose (pData->pSource);
}

static void
projectionIteratorDestroy (CypherIterator *pIterator)
{
  ProjectionIteratorData *pData
      = (ProjectionIteratorData *)pIterator->pIterData;

  if (pData)
    {
      if (pData->pSource)
        {
          cypherIteratorDestroy (pData->pSource);
        }
      sqlite3_free (pData);
    }
}

CypherIterator *
cypherProjectionCreate (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  ProjectionIteratorData *pData;

  /* Allow projection without explicit projection list (pass-through mode) */
  if (!pPlan || !pPlan->pChild)
    return NULL;

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;

  pData = sqlite3_malloc (sizeof (ProjectionIteratorData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (ProjectionIteratorData));

  /* Create source iterator */
  pData->pSource = cypherIteratorCreate (pPlan->pChild, pContext);
  if (!pData->pSource)
    {
      sqlite3_free (pData);
      sqlite3_free (pIterator);
      return NULL;
    }

  pData->apProjections = pPlan->apProjections;
  pData->nProjections = pPlan->nProjections;

  /* Set up iterator */
  pIterator->xOpen = projectionIteratorOpen;
  pIterator->xNext = projectionIteratorNext;
  pIterator->xClose = projectionIteratorClose;
  pIterator->xDestroy = projectionIteratorDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

/* Sort iterator implementation */
typedef struct SortIteratorData
{
  CypherIterator *pSource;       /* Source iterator */
  CypherResult *aResults;        /* Array of results to sort */
  int nResults;                  /* Number of results */
  int nAllocated;                /* Allocated size */
  int iCurrent;                  /* Current position */
  CypherExpression **apSortKeys; /* Sort key expressions */
  int nSortKeys;                 /* Number of sort keys */
} SortIteratorData;

static int
sortIteratorOpen (CypherIterator *pIterator)
{
  SortIteratorData *pData = (SortIteratorData *)pIterator->pIterData;
  CypherResult result;
  int rc;

  /* Open source iterator */
  rc = pData->pSource->xOpen (pData->pSource);
  if (rc != SQLITE_OK)
    return rc;

  /* Collect all results */
  pData->nResults = 0;
  pData->iCurrent = 0;

  while ((rc = pData->pSource->xNext (pData->pSource, &result)) == SQLITE_OK)
    {
      /* Resize array if needed */
      if (pData->nResults >= pData->nAllocated)
        {
          int nNew = pData->nAllocated ? pData->nAllocated * 2 : 16;
          CypherResult *aNew = sqlite3_realloc (pData->aResults,
                                                nNew * sizeof (CypherResult));
          if (!aNew)
            {
              cypherResultDestroy (&result);
              return SQLITE_NOMEM;
            }
          pData->aResults = aNew;
          pData->nAllocated = nNew;
        }

      /* Copy result */
      pData->aResults[pData->nResults++] = result;
    }

  /* Sort results - simple bubble sort for now */
  for (int i = 0; i < pData->nResults - 1; i++)
    {
      for (int j = 0; j < pData->nResults - i - 1; j++)
        {
          /* Compare using first sort key only for simplicity */
          if (pData->nSortKeys > 0)
            {
              CypherValue v1, v2;
              cypherExpressionEvaluate (pData->apSortKeys[0],
                                        pIterator->pContext, &v1);
              cypherExpressionEvaluate (pData->apSortKeys[0],
                                        pIterator->pContext, &v2);

              if (cypherValueCompare (&v1, &v2) > 0)
                {
                  /* Swap */
                  CypherResult temp = pData->aResults[j];
                  pData->aResults[j] = pData->aResults[j + 1];
                  pData->aResults[j + 1] = temp;
                }

              cypherValueDestroy (&v1);
              cypherValueDestroy (&v2);
            }
        }
    }

  return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

static int
sortIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  SortIteratorData *pData = (SortIteratorData *)pIterator->pIterData;

  if (pData->iCurrent >= pData->nResults)
    {
      return SQLITE_DONE;
    }

  /* Return copy of current result */
  *pResult = pData->aResults[pData->iCurrent++];
  return SQLITE_OK;
}

static int
sortIteratorClose (CypherIterator *pIterator)
{
  SortIteratorData *pData = (SortIteratorData *)pIterator->pIterData;

  /* Free sorted results */
  for (int i = 0; i < pData->nResults; i++)
    {
      cypherResultDestroy (&pData->aResults[i]);
    }

  return pData->pSource->xClose (pData->pSource);
}

static void
sortIteratorDestroy (CypherIterator *pIterator)
{
  if (pIterator)
    {
      SortIteratorData *pData = (SortIteratorData *)pIterator->pIterData;
      if (pData)
        {
          if (pData->pSource)
            {
              cypherIteratorDestroy (pData->pSource);
            }
          sqlite3_free (pData->aResults);
          sqlite3_free (pData);
        }
      /* Don't free pIterator - cypherIteratorDestroy will do that */
    }
}

CypherIterator *
cypherSortCreate (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  SortIteratorData *pData;

  if (!pPlan || !pPlan->pChild)
    return NULL;

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;

  pData = sqlite3_malloc (sizeof (SortIteratorData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (SortIteratorData));

  /* Create source iterator */
  pData->pSource = cypherIteratorCreate (pPlan->pChild, pContext);
  if (!pData->pSource)
    {
      sqlite3_free (pData);
      sqlite3_free (pIterator);
      return NULL;
    }

  pData->apSortKeys = pPlan->apSortKeys;
  pData->nSortKeys = pPlan->nSortKeys;

  /* Set up iterator */
  pIterator->xOpen = sortIteratorOpen;
  pIterator->xNext = sortIteratorNext;
  pIterator->xClose = sortIteratorClose;
  pIterator->xDestroy = sortIteratorDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

/* Limit iterator implementation */
typedef struct LimitIteratorData
{
  CypherIterator *pSource; /* Source iterator */
  int nLimit;              /* Limit count */
  int nReturned;           /* Number returned so far */
} LimitIteratorData;

static int
limitIteratorOpen (CypherIterator *pIterator)
{
  LimitIteratorData *pData = (LimitIteratorData *)pIterator->pIterData;
  pData->nReturned = 0;
  return pData->pSource->xOpen (pData->pSource);
}

static int
limitIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  LimitIteratorData *pData = (LimitIteratorData *)pIterator->pIterData;

  /* Check if limit reached */
  if (pData->nReturned >= pData->nLimit)
    {
      return SQLITE_DONE;
    }

  /* Get next from source */
  int rc = pData->pSource->xNext (pData->pSource, pResult);
  if (rc == SQLITE_OK)
    {
      pData->nReturned++;
    }

  return rc;
}

static int
limitIteratorClose (CypherIterator *pIterator)
{
  LimitIteratorData *pData = (LimitIteratorData *)pIterator->pIterData;
  return pData->pSource->xClose (pData->pSource);
}

static void
limitIteratorDestroy (CypherIterator *pIterator)
{
  if (pIterator)
    {
      LimitIteratorData *pData = (LimitIteratorData *)pIterator->pIterData;
      if (pData)
        {
          if (pData->pSource)
            {
              cypherIteratorDestroy (pData->pSource);
            }
          sqlite3_free (pData);
        }
      /* Don't free pIterator - cypherIteratorDestroy will do that */
    }
}

CypherIterator *
cypherLimitCreate (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  LimitIteratorData *pData;

  if (!pPlan || !pPlan->pChild || pPlan->nLimit <= 0)
    return NULL;

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;

  pData = sqlite3_malloc (sizeof (LimitIteratorData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (LimitIteratorData));

  /* Create source iterator */
  pData->pSource = cypherIteratorCreate (pPlan->pChild, pContext);
  if (!pData->pSource)
    {
      sqlite3_free (pData);
      sqlite3_free (pIterator);
      return NULL;
    }

  pData->nLimit = pPlan->nLimit;
  pData->nReturned = 0;

  /* Set up iterator */
  pIterator->xOpen = limitIteratorOpen;
  pIterator->xNext = limitIteratorNext;
  pIterator->xClose = limitIteratorClose;
  pIterator->xDestroy = limitIteratorDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

/*
** Expand iterator implementation
** Traverses relationships from source nodes to target nodes
*/

typedef struct ExpandIteratorData
{
  CypherIterator *pSource;      /* Source iterator producing start nodes */
  sqlite3_stmt *pStmt;          /* Statement for relationship lookup */
  CypherResult *pCurrentRow;    /* Current source row being expanded */
  int bSourceExhausted;         /* True if source iterator is done */
  sqlite3_int64 iCurrentNodeId; /* Current source node ID being expanded */
} ExpandIteratorData;

static int
expandOpen (CypherIterator *pIterator)
{
  ExpandIteratorData *pData = (ExpandIteratorData *)pIterator->pIterData;
  int rc;

  /* Open source iterator if exists and not already opened */
  if (pData->pSource && !pData->pSource->bOpened)
    {
      rc = pData->pSource->xOpen (pData->pSource);
      if (rc != SQLITE_OK)
        return rc;
    }

  pIterator->bOpened = 1;
  pIterator->bEof = 0;
  pData->bSourceExhausted = 0;

  return SQLITE_OK;
}

static int
expandNext (CypherIterator *pIterator, CypherResult *pResult)
{
  ExpandIteratorData *pData = (ExpandIteratorData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  GraphVtab *pGraph = pIterator->pContext->pGraph;
  int rc;

  if (!pGraph)
    return SQLITE_ERROR;

  /* Loop until we find a relationship or exhaust all sources */
  while (1)
    {
      /* If we have a current statement, try to get next relationship */
      if (pData->pStmt)
        {
          rc = sqlite3_step (pData->pStmt);

          if (rc == SQLITE_ROW)
            {
              /* Found a relationship - build result */
              memset (pResult, 0, sizeof (CypherResult));

              /* Copy all columns from source row */
              if (pData->pCurrentRow)
                {
                  for (int i = 0; i < pData->pCurrentRow->nColumns; i++)
                    {
                      rc = cypherResultAddColumn (
                          pResult, pData->pCurrentRow->azColumnNames[i],
                          &pData->pCurrentRow->aValues[i]);
                      if (rc != SQLITE_OK)
                        {
                          cypherResultDestroy (pResult);
                          return rc;
                        }
                    }
                }

              /* Add target node from relationship */
              CypherValue targetValue;
              memset (&targetValue, 0, sizeof (targetValue));
              targetValue.type = CYPHER_VALUE_NODE;
              targetValue.u.iNodeId = sqlite3_column_int64 (pData->pStmt, 0);

              const char *zTargetAlias
                  = pPlan->zAlias ? pPlan->zAlias : "target";
              rc = cypherResultAddColumn (pResult, zTargetAlias, &targetValue);
              if (rc != SQLITE_OK)
                {
                  cypherResultDestroy (pResult);
                  return rc;
                }

              /* Add relationship with type information */
              CypherValue relValue;
              memset (&relValue, 0, sizeof (relValue));

              /* Get relationship ID and type from query results */
              sqlite3_int64 iRelId = sqlite3_column_int64 (pData->pStmt, 1);
              const unsigned char *zRelType
                  = sqlite3_column_text (pData->pStmt, 2);

              /* Store as string with type information for better serialization
               */
              relValue.type = CYPHER_VALUE_STRING;
              if (zRelType && zRelType[0])
                {
                  relValue.u.zString = sqlite3_mprintf (
                      "Relationship(id=%lld,type=%s)", iRelId, zRelType);
                }
              else
                {
                  relValue.u.zString
                      = sqlite3_mprintf ("Relationship(%lld)", iRelId);
                }

              const char *zRelAlias = pPlan->zLabel ? pPlan->zLabel : "r";
              rc = cypherResultAddColumn (pResult, zRelAlias, &relValue);
              if (rc != SQLITE_OK)
                {
                  cypherResultDestroy (pResult);
                  return rc;
                }

              return SQLITE_OK;
            }
          else if (rc == SQLITE_DONE)
            {
              /* No more relationships for current source node */
              sqlite3_finalize (pData->pStmt);
              pData->pStmt = NULL;
            }
          else
            {
              /* Error */
              return rc;
            }
        }

      /* Need to get next source node */
      if (pData->bSourceExhausted)
        {
          pIterator->bEof = 1;
          return SQLITE_DONE;
        }

      /* Clean up previous row */
      if (pData->pCurrentRow)
        {
          cypherResultDestroy (
              pData->pCurrentRow); /* This frees pCurrentRow itself */
          pData->pCurrentRow = NULL;
        }

      /* Allocate new result for current row */
      pData->pCurrentRow = sqlite3_malloc (sizeof (CypherResult));
      if (!pData->pCurrentRow)
        return SQLITE_NOMEM;
      memset (pData->pCurrentRow, 0, sizeof (CypherResult));

      /* Get next row from source */
      if (!pData->pSource)
        {
          /* No source iterator - this expand has no input, so we're done */
          pData->bSourceExhausted = 1;
          pIterator->bEof = 1;
          return SQLITE_DONE;
        }

      rc = pData->pSource->xNext (pData->pSource, pData->pCurrentRow);

      if (rc == SQLITE_DONE)
        {
          pData->bSourceExhausted = 1;
          pIterator->bEof = 1;
          return SQLITE_DONE;
        }
      else if (rc != SQLITE_OK)
        {
          return rc;
        }

      /* Extract source node ID from first child's alias */
      const char *zSourceAlias = NULL;
      if (pPlan->nChildren > 0 && pPlan->apChildren[0])
        {
          zSourceAlias = pPlan->apChildren[0]->zAlias;
        }
      if (!zSourceAlias)
        zSourceAlias = "node";

      /* Find the node value in the current row */
      CypherValue *pNodeValue = NULL;
      for (int i = 0; i < pData->pCurrentRow->nColumns; i++)
        {
          if (strcmp (pData->pCurrentRow->azColumnNames[i], zSourceAlias) == 0)
            {
              pNodeValue = &pData->pCurrentRow->aValues[i];
              break;
            }
        }

      if (!pNodeValue || pNodeValue->type != CYPHER_VALUE_NODE)
        {
          return SQLITE_ERROR;
        }

      pData->iCurrentNodeId = pNodeValue->u.iNodeId;

      /* Prepare query for relationships from this node */
      const char *zEdgeType = pPlan->zProperty ? pPlan->zProperty : "";
      const char *zTargetLabel = NULL;
      char *zSql;

      /* Check if there's a target node specification (second child) with a
       * label */
      if (pPlan->nChildren > 1 && pPlan->apChildren[1])
        {
          zTargetLabel = pPlan->apChildren[1]->zLabel;
        }

      /* Build SQL query with optional edge type and target label filtering */
      if (zTargetLabel && zTargetLabel[0])
        {
          /* Need to filter by target node label */
          if (zEdgeType && zEdgeType[0])
            {
              zSql = sqlite3_mprintf (
                  "SELECT e.target, e.id, e.edge_type FROM %s_edges e "
                  "JOIN %s_nodes n ON e.target = n.id "
                  "WHERE e.source = %lld AND e.edge_type = '%q' "
                  "AND EXISTS (SELECT 1 FROM json_each(n.labels) WHERE value "
                  "= %Q)",
                  pGraph->zTableName, pGraph->zTableName,
                  pData->iCurrentNodeId, zEdgeType, zTargetLabel);
            }
          else
            {
              zSql = sqlite3_mprintf (
                  "SELECT e.target, e.id, e.edge_type FROM %s_edges e "
                  "JOIN %s_nodes n ON e.target = n.id "
                  "WHERE e.source = %lld "
                  "AND EXISTS (SELECT 1 FROM json_each(n.labels) WHERE value "
                  "= %Q)",
                  pGraph->zTableName, pGraph->zTableName,
                  pData->iCurrentNodeId, zTargetLabel);
            }
        }
      else
        {
          /* No target label filtering */
          if (zEdgeType && zEdgeType[0])
            {
              zSql = sqlite3_mprintf (
                  "SELECT target, id, edge_type FROM %s_edges WHERE source = "
                  "%lld AND edge_type = '%q'",
                  pGraph->zTableName, pData->iCurrentNodeId, zEdgeType);
            }
          else
            {
              zSql = sqlite3_mprintf ("SELECT target, id, edge_type FROM "
                                      "%s_edges WHERE source = %lld",
                                      pGraph->zTableName,
                                      pData->iCurrentNodeId);
            }
        }

      if (!zSql)
        return SQLITE_NOMEM;

      rc = sqlite3_prepare_v2 (pGraph->pDb, zSql, -1, &pData->pStmt, 0);
      sqlite3_free (zSql);

      if (rc != SQLITE_OK)
        return rc;

      /* Loop back to try getting first relationship from this node */
    }
}

static int
expandClose (CypherIterator *pIterator)
{
  ExpandIteratorData *pData = (ExpandIteratorData *)pIterator->pIterData;

  if (pData->pStmt)
    {
      sqlite3_finalize (pData->pStmt);
      pData->pStmt = NULL;
    }

  if (pData->pSource)
    {
      pData->pSource->xClose (pData->pSource);
    }

  if (pData->pCurrentRow)
    {
      cypherResultDestroy (
          pData->pCurrentRow); /* This frees pCurrentRow itself */
      pData->pCurrentRow = NULL;
    }

  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void
expandDestroy (CypherIterator *pIterator)
{
  ExpandIteratorData *pData = (ExpandIteratorData *)pIterator->pIterData;

  if (pData)
    {
      if (pData->pStmt)
        {
          sqlite3_finalize (pData->pStmt);
        }

      if (pData->pSource)
        {
          cypherIteratorDestroy (pData->pSource);
        }

      if (pData->pCurrentRow)
        {
          cypherResultDestroy (
              pData->pCurrentRow); /* This frees pCurrentRow itself */
        }

      sqlite3_free (pData);
    }
}

static CypherIterator *
cypherExpandCreate (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  ExpandIteratorData *pData;
  CypherIterator *pSource = NULL;

  if (!pPlan || !pContext)
    return NULL;

  /* Create source iterator from first child */
  if (pPlan->nChildren > 0 && pPlan->apChildren[0])
    {
      pSource = cypherIteratorCreate (pPlan->apChildren[0], pContext);
      if (!pSource)
        return NULL;
    }

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    {
      if (pSource)
        cypherIteratorDestroy (pSource);
      return NULL;
    }

  pData = sqlite3_malloc (sizeof (ExpandIteratorData));
  if (!pData)
    {
      if (pSource)
        cypherIteratorDestroy (pSource);
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (ExpandIteratorData));

  pData->pSource = pSource;
  pData->pStmt = NULL;
  pData->pCurrentRow = NULL;
  pData->bSourceExhausted = 0;
  pData->iCurrentNodeId = 0;

  /* Set up iterator */
  pIterator->xOpen = expandOpen;
  pIterator->xNext = expandNext;
  pIterator->xClose = expandClose;
  pIterator->xDestroy = expandDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

/*
** Write Operation Wrapper Iterators
** Simplified version that directly inserts into database tables
*/

typedef struct WriteIteratorData
{
  sqlite3_stmt *pInsertStmt;
  int bExecuted;
  sqlite3_int64 iCreatedId;
  /* Support for driving iterators (MATCH ... CREATE) */
  CypherResult *apChildResults; /* Array of materialized child results */
  int nChildResults;             /* Number of child results */
  int iCurrentChild;             /* Current index in child results array */
  /* Persistent variable binding across CREATE clauses */
  struct {
    const char *zVarName;
    sqlite3_int64 iNodeId;
  } aVarBindings[50];
  int nVarBindings;
} WriteIteratorData;

typedef struct DeleteIteratorData
{
  CypherWriteContext *pWriteCtx; /* Write context for mutations */
  int bDetach;                   /* Whether to DETACH DELETE */
  char *zVariable;               /* Variable name to delete */
} DeleteIteratorData;

static int
createIteratorOpen (CypherIterator *pIterator)
{
  WriteIteratorData *pData = (WriteIteratorData *)pIterator->pIterData;
  ExecutionContext *pContext = pIterator->pContext;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  int rc;
  char *zSql;
  const char *zTableName;

  if (!pContext || !pContext->pDb || !pContext->pGraph)
    return SQLITE_ERROR;

  /* Get the correct table name from the graph virtual table */
  zTableName = pContext->pGraph->zNodeTableName;
  if (!zTableName)
    zTableName = "graph_nodes"; /* Fallback */

  /* Build INSERT statement for nodes table */
  zSql = sqlite3_mprintf ("INSERT INTO %s (labels, properties) VALUES (?, ?)",
                          zTableName);
  if (!zSql)
    return SQLITE_NOMEM;

  /* Prepare INSERT statement */
  rc = sqlite3_prepare_v2 (pContext->pDb, zSql, -1, &pData->pInsertStmt, NULL);

  sqlite3_free (zSql);

  if (rc != SQLITE_OK)
    {
      return rc;
    }

  pData->bExecuted = 0;
  pData->apChildResults = NULL;
  pData->nChildResults = 0;
  pData->iCurrentChild = 0;
  pData->nVarBindings = 0; /* Initialize persistent variable binding map */

  /* Check if we have a child iterator (e.g., from MATCH) */
  /* Materialize all child results to prevent seeing our own writes */
  if (pIterator->nChildren > 0)
    {
      CypherIterator *pChild = pIterator->apChildren[0];
      rc = pChild->xOpen (pChild);
      if (rc != SQLITE_OK)
        return rc;

      /* Materialize all child results into array */
      int nCapacity = 16; /* Initial capacity */
      pData->apChildResults = sqlite3_malloc (nCapacity * sizeof (CypherResult));
      if (!pData->apChildResults)
        return SQLITE_NOMEM;

      CypherResult tempResult;
      while (1)
        {
          memset (&tempResult, 0, sizeof (CypherResult));
          rc = pChild->xNext (pChild, &tempResult);
          if (rc != SQLITE_OK)
            break; /* Done or error */

          /* Grow array if needed */
          if (pData->nChildResults >= nCapacity)
            {
              nCapacity *= 2;
              CypherResult *pNew = sqlite3_realloc (
                  pData->apChildResults, nCapacity * sizeof (CypherResult));
              if (!pNew)
                {
                  cypherResultClear (&tempResult);
                  return SQLITE_NOMEM;
                }
              pData->apChildResults = pNew;
            }

          /* Deep copy result into array */
          memcpy (&pData->apChildResults[pData->nChildResults], &tempResult,
                  sizeof (CypherResult));
          pData->nChildResults++;
        }

      /* Close child iterator - we've consumed all its results */
      pChild->xClose (pChild);
    }

  pIterator->bOpened = 1;
  return SQLITE_OK;
}

/* Helper function to convert MAP AST to JSON string */
static char *
convertMapToJson (CypherAst *pMap)
{
  if (!pMap || !cypherAstIsType (pMap, CYPHER_AST_MAP))
    {
      return sqlite3_mprintf ("{}");
    }

  char *zJson = sqlite3_mprintf ("{");
  int first = 1;

  for (int k = 0; k < pMap->nChildren; k++)
    {
      CypherAst *pPair = pMap->apChildren[k];
      if (!cypherAstIsType (pPair, CYPHER_AST_PROPERTY_PAIR))
        continue;

      const char *zKey = cypherAstGetValue (pPair);
      if (!zKey)
        continue;

      const char *zValue = NULL;
      if (pPair->nChildren > 0)
        {
          zValue = cypherAstGetValue (pPair->apChildren[0]);
        }

      if (zValue)
        {
          if (!first)
            {
              char *zOld = zJson;
              zJson = sqlite3_mprintf (
                  "%s,", zOld); /* Fixed: removed space after comma */
              sqlite3_free (zOld);
            }

          char *zOld = zJson;
          /* Check if value looks like a number (digit or - sign) */
          int isNumeric = (zValue[0] >= '0' && zValue[0] <= '9')
                          || zValue[0] == '-' || strcmp (zValue, "true") == 0
                          || strcmp (zValue, "false") == 0
                          || strcmp (zValue, "null") == 0;

          if (isNumeric)
            {
              /* Numeric, boolean, or null - use as-is in JSON */
              zJson = sqlite3_mprintf ("%s\"%s\":%s", zOld, zKey, zValue);
            }
          else
            {
              /* String value - add JSON quotes */
              zJson = sqlite3_mprintf ("%s\"%s\":\"%s\"", zOld, zKey, zValue);
            }
          sqlite3_free (zOld);
          first = 0;
        }
    }

  char *zOld = zJson;
  zJson = sqlite3_mprintf ("%s}", zOld);
  sqlite3_free (zOld);

  return zJson;
}

static int
createIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  WriteIteratorData *pData = (WriteIteratorData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  ExecutionContext *pContext = pIterator->pContext;
  CypherAst *pPattern;
  int rc;

  /* If we have materialized child results, iterate over them */
  if (pData->apChildResults && pData->nChildResults > 0)
    {
      /* Check if we've exhausted all child rows */
      if (pData->iCurrentChild >= pData->nChildResults)
        {
          return SQLITE_DONE;
        }

      /* Bind all columns from current child result to execution context */
      CypherResult *pChildResult = &pData->apChildResults[pData->iCurrentChild];
      for (int i = 0; i < pChildResult->nColumns; i++)
        {
          const char *zColName = pChildResult->azColumnNames[i];
          CypherValue *pValue = &pChildResult->aValues[i];

          /* Bind the value in execution context so CREATE can access it */
          executionContextBind (pContext, zColName, pValue);
        }

      /* Move to next child result for next iteration */
      pData->iCurrentChild++;
    }
  else
    {
      /* No child results - CREATE only executes once */
      if (pData->bExecuted)
        {
          return SQLITE_DONE;
        }
      pData->bExecuted = 1;
    }

  /* Check if we have a pattern AST in pExecState (copied from logical plan's
   * pExtra) */
  pPattern = (CypherAst *)pPlan->pExecState;

  /* Handle both single patterns and pattern lists (e.g., CREATE (), () creates a PATTERN_LIST) */
  if (pPattern && cypherAstIsType (pPattern, CYPHER_AST_PATTERN_LIST))
    {
      /* Multiple patterns: CREATE (), (), () */
      /* Each child is a separate PATTERN to process */
      for (int patIdx = 0; patIdx < pPattern->nChildren; patIdx++)
        {
          CypherAst *pSinglePattern = pPattern->apChildren[patIdx];
          if (!cypherAstIsType (pSinglePattern, CYPHER_AST_PATTERN))
            continue;

          /* Process this pattern (similar to single pattern logic below) */
          sqlite3_int64 aiNodeIds[10];
          int nNodes = 0;
          int i;

          /* Count and create nodes first */
          for (i = 0; i < pSinglePattern->nChildren; i++)
            {
              if (cypherAstIsType (pSinglePattern->apChildren[i],
                                   CYPHER_AST_NODE_PATTERN))
                {
                  CypherAst *pNode = pSinglePattern->apChildren[i];

                  /* Extract variable name from IDENTIFIER child (if present) */
                  const char *zVarName = NULL;
                  for (int j = 0; j < pNode->nChildren; j++)
                    {
                      if (cypherAstIsType (pNode->apChildren[j], CYPHER_AST_IDENTIFIER))
                        {
                          zVarName = pNode->apChildren[j]->zValue;
                          break;
                        }
                    }

                  sqlite3_int64 iNodeId = 0;
                  int bNodeExists = 0;

                  /* Check if this variable already exists (from MATCH or earlier in pattern) */
                  if (zVarName)
                    {
                      /* First check execution context (for MATCH variables) */
                      CypherValue *pValue = executionContextGet (pContext, zVarName);
                      if (pValue && pValue->type == CYPHER_VALUE_NODE)
                        {
                          /* Variable bound from MATCH - reuse that node */
                          iNodeId = pValue->u.iNodeId;
                          bNodeExists = 1;
                        }
                      else
                        {
                          /* Check local binding map (for variables within this CREATE) */
                          for (int v = 0; v < pData->nVarBindings; v++)
                            {
                              if (strcmp (pData->aVarBindings[v].zVarName, zVarName) == 0)
                                {
                                  /* Variable already bound in this CREATE - reuse */
                                  iNodeId = pData->aVarBindings[v].iNodeId;
                                  bNodeExists = 1;
                                  break;
                                }
                            }
                        }
                    }

                  /* If node doesn't exist, create it */
                  if (!bNodeExists)
                    {
                      const char *zLabels = "[]";
                      const char *zProperties = "{}";
                      char *zJsonProps = NULL;
                      char *zJsonLabels = NULL;

                      /* Extract labels and properties from node pattern */
                      for (int j = 0; j < pNode->nChildren; j++)
                        {
                          if (cypherAstIsType (pNode->apChildren[j],
                                               CYPHER_AST_LABELS))
                            {
                              CypherAst *pLabels = pNode->apChildren[j];
                              if (pLabels->nChildren > 0)
                                {
                                  zJsonLabels = sqlite3_mprintf ("[");
                                  for (int k = 0; k < pLabels->nChildren; k++)
                                    {
                                      const char *zLabel = cypherAstGetValue (pLabels->apChildren[k]);
                                      if (zLabel)
                                        {
                                          char *zOld = zJsonLabels;
                                          zJsonLabels = sqlite3_mprintf ("%s%s\"%s\"",
                                                                         zOld,
                                                                         (k > 0 ? "," : ""),
                                                                         zLabel);
                                          sqlite3_free (zOld);
                                        }
                                    }
                                  char *zOld = zJsonLabels;
                                  zJsonLabels = sqlite3_mprintf ("%s]", zOld);
                                  sqlite3_free (zOld);
                                  zLabels = zJsonLabels;
                                }
                              else if (pLabels->zValue)
                                {
                                  zJsonLabels
                                      = sqlite3_mprintf ("[\"%s\"]", pLabels->zValue);
                                  if (zJsonLabels)
                                    zLabels = zJsonLabels;
                                }
                            }
                          else if (cypherAstIsType (pNode->apChildren[j],
                                                    CYPHER_AST_MAP))
                            {
                              zJsonProps = convertMapToJson (pNode->apChildren[j]);
                              if (zJsonProps)
                                zProperties = zJsonProps;
                            }
                        }

                      /* Insert node */
                      sqlite3_bind_text (pData->pInsertStmt, 1, zLabels, -1,
                                         SQLITE_TRANSIENT);
                      sqlite3_bind_text (pData->pInsertStmt, 2, zProperties, -1,
                                         SQLITE_TRANSIENT);

                      rc = sqlite3_step (pData->pInsertStmt);
                      if (zJsonProps)
                        sqlite3_free (zJsonProps);
                      if (zJsonLabels)
                        sqlite3_free (zJsonLabels);
                      if (rc != SQLITE_DONE)
                        return SQLITE_ERROR;

                      iNodeId = sqlite3_last_insert_rowid (pContext->pDb);
                      sqlite3_reset (pData->pInsertStmt);

                      /* Add to variable binding map if it has a name */
                      if (zVarName && pData->nVarBindings < 50)
                        {
                          pData->aVarBindings[pData->nVarBindings].zVarName = zVarName;
                          pData->aVarBindings[pData->nVarBindings].iNodeId = iNodeId;
                          pData->nVarBindings++;
                        }

                      /* Also bind to execution context for cross-clause variable sharing */
                      if (zVarName)
                        {
                          CypherValue nodeValue;
                          cypherValueInit (&nodeValue);
                          cypherValueSetNode (&nodeValue, iNodeId);
                          executionContextBind (pContext, zVarName, &nodeValue);
                          cypherValueDestroy (&nodeValue);
                        }
                    }

                  /* Store node ID for relationship creation */
                  aiNodeIds[nNodes++] = iNodeId;
                }
            }

          /* Now create relationships between nodes in this pattern */
          int nodeIndex = 0;
          for (i = 0; i < pSinglePattern->nChildren; i++)
            {
              if (cypherAstIsType (pSinglePattern->apChildren[i],
                                   CYPHER_AST_REL_PATTERN))
                {
                  CypherAst *pRel = pSinglePattern->apChildren[i];
                  const char *zRelType = "";
                  const char *zRelProps = "{}";
                  sqlite3_int64 iSource, iTarget;
                  char *zJsonRelProps = NULL;

                  for (int j = 0; j < pRel->nChildren; j++)
                    {
                      if (cypherAstIsType (pRel->apChildren[j], CYPHER_AST_LABELS))
                        {
                          CypherAst *pLabels = pRel->apChildren[j];
                          if (pLabels->zValue)
                            zRelType = pLabels->zValue;
                        }
                      else if (cypherAstIsType (pRel->apChildren[j],
                                                CYPHER_AST_MAP))
                        {
                          zJsonRelProps = convertMapToJson (pRel->apChildren[j]);
                          if (zJsonRelProps)
                            zRelProps = zJsonRelProps;
                        }
                    }

                  if (nodeIndex >= nNodes - 1)
                    continue;

                  /* Handle relationship direction: iFlags = 1 (->), -1 (<-), 0 (undirected) */
                  int iDirection = pRel->iFlags;
                  if (iDirection == -1) {
                    /* Reverse arrow: source and target are swapped */
                    iSource = aiNodeIds[nodeIndex + 1];
                    iTarget = aiNodeIds[nodeIndex];
                  } else {
                    /* Forward arrow or undirected */
                    iSource = aiNodeIds[nodeIndex];
                    iTarget = aiNodeIds[nodeIndex + 1];
                  }

                  const char *zEdgeTable = pContext->pGraph->zEdgeTableName
                                               ? pContext->pGraph->zEdgeTableName
                                               : "graph_edges";
                  char *zSql = sqlite3_mprintf (
                      "INSERT INTO %s (source, target, edge_type, properties) "
                      "VALUES (%lld, %lld, '%q', '%q')",
                      zEdgeTable, iSource, iTarget, zRelType, zRelProps);

                  if (zSql)
                    {
                      rc = sqlite3_exec (pContext->pDb, zSql, NULL, NULL, NULL);
                      sqlite3_free (zSql);
                      if (zJsonRelProps)
                        sqlite3_free (zJsonRelProps);
                      if (rc != SQLITE_OK)
                        return SQLITE_ERROR;
                    }

                  nodeIndex++;
                }
              else if (cypherAstIsType (pSinglePattern->apChildren[i],
                                        CYPHER_AST_NODE_PATTERN))
                {
                  /* Track which node we're on for relationships */
                }
            }
        }

      /* Return result for all created nodes */
      CypherValue nodeValue;
      memset (&nodeValue, 0, sizeof (nodeValue));
      nodeValue.type = CYPHER_VALUE_NODE;
      nodeValue.u.iNodeId = 1; /* Placeholder */
      rc = cypherResultAddColumn (
          pResult, pPlan->zAlias ? pPlan->zAlias : "node", &nodeValue);
      if (rc != SQLITE_OK)
        return rc;
    }
  else if (pPattern && cypherAstIsType (pPattern, CYPHER_AST_PATTERN))
    {
      /* Process full pattern: (a)-[r:TYPE]->(b) */
      sqlite3_int64 aiNodeIds[10]; /* Support up to 10 nodes in pattern */
      int nNodes = 0;
      int i;

      /* Count and create nodes first */
      for (i = 0; i < pPattern->nChildren; i++)
        {
          if (cypherAstIsType (pPattern->apChildren[i],
                               CYPHER_AST_NODE_PATTERN))
            {
              CypherAst *pNode = pPattern->apChildren[i];

              /* Extract variable name from IDENTIFIER child (if present) */
              const char *zVarName = NULL;
              for (int j = 0; j < pNode->nChildren; j++)
                {
                  if (cypherAstIsType (pNode->apChildren[j], CYPHER_AST_IDENTIFIER))
                    {
                      zVarName = pNode->apChildren[j]->zValue;
                      break;
                    }
                }

              sqlite3_int64 iNodeId = 0;
              int bNodeExists = 0;

              /* Check if this variable already exists (from MATCH or earlier in pattern) */
              if (zVarName)
                {
                  /* First check execution context (for MATCH variables) */
                  CypherValue *pValue = executionContextGet (pContext, zVarName);
                  if (pValue && pValue->type == CYPHER_VALUE_NODE)
                    {
                      /* Variable bound from MATCH - reuse that node */
                      iNodeId = pValue->u.iNodeId;
                      bNodeExists = 1;
                    }
                  else
                    {
                      /* Check local binding map (for variables within this CREATE) */
                      for (int v = 0; v < pData->nVarBindings; v++)
                        {
                          if (strcmp (pData->aVarBindings[v].zVarName, zVarName) == 0)
                            {
                              /* Variable already bound in this CREATE - reuse */
                              iNodeId = pData->aVarBindings[v].iNodeId;
                              bNodeExists = 1;
                              break;
                            }
                        }
                    }
                }

              /* If node doesn't exist, create it */
              if (!bNodeExists)
                {
                  const char *zLabels = "[]"; /* Default to empty JSON array */
                  const char *zProperties = "{}";

                  /* Extract labels and properties from node pattern */
                  char *zJsonProps = NULL;
                  char *zJsonLabels = NULL;
                  for (int j = 0; j < pNode->nChildren; j++)
                    {
                      if (cypherAstIsType (pNode->apChildren[j],
                                           CYPHER_AST_LABELS))
                        {
                          CypherAst *pLabels = pNode->apChildren[j];

                          /* Handle multiple labels - iterate through all children */
                          if (pLabels->nChildren > 0)
                            {
                              /* Build JSON array from all label children */
                              zJsonLabels = sqlite3_mprintf ("[");
                              for (int k = 0; k < pLabels->nChildren; k++)
                                {
                                  const char *zLabel = cypherAstGetValue (pLabels->apChildren[k]);
                                  if (zLabel)
                                    {
                                      char *zOld = zJsonLabels;
                                      zJsonLabels = sqlite3_mprintf ("%s%s\"%s\"",
                                                                     zOld,
                                                                     (k > 0 ? "," : ""),
                                                                     zLabel);
                                      sqlite3_free (zOld);
                                    }
                                }
                              char *zOld = zJsonLabels;
                              zJsonLabels = sqlite3_mprintf ("%s]", zOld);
                              sqlite3_free (zOld);
                              zLabels = zJsonLabels;
                            }
                          else if (pLabels->zValue)
                            {
                              /* Single label fallback */
                              zJsonLabels
                                  = sqlite3_mprintf ("[\"%s\"]", pLabels->zValue);
                              if (zJsonLabels)
                                zLabels = zJsonLabels;
                            }
                        }
                      else if (cypherAstIsType (pNode->apChildren[j],
                                                CYPHER_AST_MAP))
                        {
                          zJsonProps = convertMapToJson (pNode->apChildren[j]);
                          if (zJsonProps)
                            zProperties = zJsonProps;
                        }
                    }

                  /* Insert node */
                  sqlite3_bind_text (pData->pInsertStmt, 1, zLabels, -1,
                                     SQLITE_TRANSIENT);
                  sqlite3_bind_text (pData->pInsertStmt, 2, zProperties, -1,
                                     SQLITE_TRANSIENT);

                  rc = sqlite3_step (pData->pInsertStmt);
                  if (zJsonProps)
                    sqlite3_free (zJsonProps); /* Free allocated JSON string */
                  if (zJsonLabels)
                    sqlite3_free (
                        zJsonLabels); /* Free allocated JSON labels string */
                  if (rc != SQLITE_DONE)
                    return SQLITE_ERROR;

                  iNodeId = sqlite3_last_insert_rowid (pContext->pDb);
                  sqlite3_reset (pData->pInsertStmt);

                  /* Add to variable binding map if it has a name */
                  if (zVarName && pData->nVarBindings < 50)
                    {
                      pData->aVarBindings[pData->nVarBindings].zVarName = zVarName;
                      pData->aVarBindings[pData->nVarBindings].iNodeId = iNodeId;
                      pData->nVarBindings++;
                    }

                  /* Also bind to execution context for cross-clause variable sharing */
                  if (zVarName)
                    {
                      CypherValue nodeValue;
                      cypherValueInit (&nodeValue);
                      cypherValueSetNode (&nodeValue, iNodeId);
                      executionContextBind (pContext, zVarName, &nodeValue);
                      cypherValueDestroy (&nodeValue);
                    }
                }

              /* Store node ID for relationship creation */
              aiNodeIds[nNodes++] = iNodeId;
            }
        }

      /* Now create relationships between nodes */
      int nodeIndex = 0;
      for (i = 0; i < pPattern->nChildren; i++)
        {
          if (cypherAstIsType (pPattern->apChildren[i],
                               CYPHER_AST_REL_PATTERN))
            {
              CypherAst *pRel = pPattern->apChildren[i];
              const char *zRelType = "";
              const char *zRelProps = "{}";
              sqlite3_int64 iSource, iTarget;

              /* Extract relationship type and properties */
              char *zJsonRelProps = NULL;
              for (int j = 0; j < pRel->nChildren; j++)
                {
                  if (cypherAstIsType (pRel->apChildren[j], CYPHER_AST_LABELS))
                    {
                      CypherAst *pLabels = pRel->apChildren[j];
                      if (pLabels->zValue)
                        zRelType = pLabels->zValue;
                    }
                  else if (cypherAstIsType (pRel->apChildren[j],
                                            CYPHER_AST_MAP))
                    {
                      zJsonRelProps = convertMapToJson (pRel->apChildren[j]);
                      if (zJsonRelProps)
                        zRelProps = zJsonRelProps;
                    }
                }

              /* Source is the node before this rel, target is after */
              if (nodeIndex >= nNodes - 1)
                continue; /* Safety check */

              /* Handle relationship direction: iFlags = 1 (->), -1 (<-), 0 (undirected) */
              int iDirection = pRel->iFlags;
              if (iDirection == -1) {
                /* Reverse arrow: source and target are swapped */
                iSource = aiNodeIds[nodeIndex + 1];
                iTarget = aiNodeIds[nodeIndex];
              } else {
                /* Forward arrow or undirected */
                iSource = aiNodeIds[nodeIndex];
                iTarget = aiNodeIds[nodeIndex + 1];
              }

              /* Insert edge */
              const char *zEdgeTable = pContext->pGraph->zEdgeTableName
                                           ? pContext->pGraph->zEdgeTableName
                                           : "graph_edges";
              char *zSql = sqlite3_mprintf (
                  "INSERT INTO %s (source, target, edge_type, properties) "
                  "VALUES (%lld, %lld, '%q', '%q')",
                  zEdgeTable, iSource, iTarget, zRelType, zRelProps);

              if (zSql)
                {
                  rc = sqlite3_exec (pContext->pDb, zSql, NULL, NULL, NULL);
                  sqlite3_free (zSql);
                  if (zJsonRelProps)
                    sqlite3_free (
                        zJsonRelProps); /* Free allocated JSON string */
                  if (rc != SQLITE_OK)
                    return SQLITE_ERROR;
                }

              nodeIndex++;
            }
          else if (cypherAstIsType (pPattern->apChildren[i],
                                    CYPHER_AST_NODE_PATTERN))
            {
              /* Track which node we're on for relationships */
              /* nodeIndex incremented after processing rel */
            }
        }

      /* Bind all created variables to execution context for use in subsequent CREATE clauses */
      for (int v = 0; v < pData->nVarBindings; v++)
        {
          CypherValue varValue;
          memset (&varValue, 0, sizeof (varValue));
          varValue.type = CYPHER_VALUE_NODE;
          varValue.u.iNodeId = pData->aVarBindings[v].iNodeId;
          executionContextBind (pContext, pData->aVarBindings[v].zVarName, &varValue);
        }

      /* Return first created node (for compatibility) */
      if (nNodes > 0)
        {
          CypherValue nodeValue;
          memset (&nodeValue, 0, sizeof (nodeValue));
          nodeValue.type = CYPHER_VALUE_NODE;
          nodeValue.u.iNodeId = aiNodeIds[0];

          rc = cypherResultAddColumn (
              pResult, pPlan->zAlias ? pPlan->zAlias : "node", &nodeValue);
          if (rc != SQLITE_OK)
            return rc;
        }
    }
  else
    {
      /* Fallback: Simple single node creation (original behavior) */
      const char *zLabels
          = (pPlan->zLabel && pPlan->zLabel[0]) ? pPlan->zLabel : "";
      const char *zProperties = (pPlan->zProperty && pPlan->zProperty[0])
                                    ? pPlan->zProperty
                                    : "{}";
      CypherValue nodeValue;

      sqlite3_bind_text (pData->pInsertStmt, 1, zLabels, -1, SQLITE_TRANSIENT);
      sqlite3_bind_text (pData->pInsertStmt, 2, zProperties, -1,
                         SQLITE_TRANSIENT);

      rc = sqlite3_step (pData->pInsertStmt);
      if (rc != SQLITE_DONE)
        return SQLITE_ERROR;

      pData->iCreatedId = sqlite3_last_insert_rowid (pContext->pDb);
      sqlite3_reset (pData->pInsertStmt);

      memset (&nodeValue, 0, sizeof (nodeValue));
      nodeValue.type = CYPHER_VALUE_NODE;
      nodeValue.u.iNodeId = pData->iCreatedId;

      rc = cypherResultAddColumn (
          pResult, pPlan->zAlias ? pPlan->zAlias : "node", &nodeValue);
      if (rc != SQLITE_OK)
        return rc;
    }

  return SQLITE_OK;
}

static int
createIteratorClose (CypherIterator *pIterator)
{
  WriteIteratorData *pData = (WriteIteratorData *)pIterator->pIterData;

  if (pData && pData->pInsertStmt)
    {
      sqlite3_finalize (pData->pInsertStmt);
      pData->pInsertStmt = NULL;
    }

  /* Clean up materialized child results */
  if (pData && pData->apChildResults)
    {
      for (int i = 0; i < pData->nChildResults; i++)
        {
          cypherResultClear (&pData->apChildResults[i]);
        }
      sqlite3_free (pData->apChildResults);
      pData->apChildResults = NULL;
      pData->nChildResults = 0;
    }

  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void
createIteratorDestroy (CypherIterator *pIterator)
{
  if (pIterator && pIterator->pIterData)
    {
      WriteIteratorData *pData = (WriteIteratorData *)pIterator->pIterData;
      if (pData->pInsertStmt)
        {
          sqlite3_finalize (pData->pInsertStmt);
        }
      sqlite3_free (pData);
    }
}

static CypherIterator *
createWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                            ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  WriteIteratorData *pData;

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;

  pData = sqlite3_malloc (sizeof (WriteIteratorData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (WriteIteratorData));

  /* Set up iterator */
  pIterator->xOpen = createIteratorOpen;
  pIterator->xNext = createIteratorNext;
  pIterator->xClose = createIteratorClose;
  pIterator->xDestroy = createIteratorDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

static int
mergeIteratorOpen (CypherIterator *pIterator)
{
  pIterator->bOpened = 1;
  return SQLITE_OK;
}

static int
mergeIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  MergeIteratorData *pData = (MergeIteratorData *)pIterator->pIterData;
  ExecutionContext *pCtx = pIterator->pContext;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  CypherWriteContext *pWriteCtx = NULL;
  int rc = SQLITE_OK;

  if (pData->bExecuted)
    return SQLITE_DONE;
  if (!pData->pDetails)
    return SQLITE_MISUSE;

  pWriteCtx = cypherWriteContextCreate (pCtx->pDb, pCtx->pGraph, pCtx);
  if (!pWriteCtx)
    return SQLITE_NOMEM;

  pResult->nColumns = 0;

  if (pData->pDetails->bHasRelationship)
    {
      MergeRelOp *pRelOp = cypherMergeRelOpCreate ();
      if (!pRelOp)
        {
          cypherWriteContextDestroy (pWriteCtx);
          return SQLITE_NOMEM;
        }
      rc = mergePopulateRelOpFromDetails (pData->pDetails, pPlan, pRelOp);
      if (rc == SQLITE_OK)
        {
          rc = cypherMergeRelationship (pWriteCtx, pRelOp);
        }
      if (rc == SQLITE_OK)
        {
          CypherValue relValue;
          const char *zAlias
              = pRelOp->zRelVar
                    ? pRelOp->zRelVar
                    : (pPlan->zAlias ? pPlan->zAlias : "relationship");
          cypherValueInit (&relValue);
          cypherValueSetRelationship (&relValue, pRelOp->iRelId);
          rc = cypherResultAddColumn (pResult, zAlias, &relValue);
          cypherValueDestroy (&relValue);
        }
      cypherMergeRelOpDestroy (pRelOp);
    }
  else
    {
      MergeNodeOp *pNodeOp = cypherMergeNodeOpCreate ();
      if (!pNodeOp)
        {
          cypherWriteContextDestroy (pWriteCtx);
          return SQLITE_NOMEM;
        }
      rc = mergeInitNodeOpFromPattern (&pData->pDetails->targetNode,
                                       pPlan->zAlias, pPlan->zOnCreateJson,
                                       pPlan->zOnMatchJson, pNodeOp);
      fprintf (stderr,
               "DEBUG fallback check nodeMatch=%d detailsMatch=%d hasRel=%d\n",
               pNodeOp->nMatchProps, pData->pDetails->nMatchProps,
               pData->pDetails->bHasRelationship);
      if (rc == SQLITE_OK && !pData->pDetails->bHasRelationship
          && pNodeOp->nMatchProps == 0 && pData->pDetails->nMatchProps > 0)
        {
          fprintf (stderr, "DEBUG applying fallback match props count=%d\n",
                   pData->pDetails->nMatchProps);
          rc = mergeApplyMatchPropsFromDetails (pData->pDetails, pNodeOp,
                                                pCtx);
        }
      if (rc == SQLITE_OK)
        {
          rc = cypherMergeNode (pWriteCtx, pNodeOp);
        }
      if (rc == SQLITE_OK)
        {
          CypherValue nodeValue;
          const char *zAlias = pNodeOp->zVariable
                                   ? pNodeOp->zVariable
                                   : (pPlan->zAlias ? pPlan->zAlias : "node");
          cypherValueInit (&nodeValue);
          cypherValueSetNode (&nodeValue, pNodeOp->iNodeId);
          rc = cypherResultAddColumn (pResult, zAlias, &nodeValue);
          cypherValueDestroy (&nodeValue);
        }
      cypherMergeNodeOpDestroy (pNodeOp);
    }

  cypherWriteContextDestroy (pWriteCtx);
  if (rc != SQLITE_OK)
    return rc;

  pData->bExecuted = 1;
  return SQLITE_OK;
}

static int
mergeIteratorClose (CypherIterator *pIterator)
{
  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void
mergeIteratorDestroy (CypherIterator *pIterator)
{
  if (pIterator && pIterator->pIterData)
    {
      sqlite3_free (pIterator->pIterData);
    }
}

static CypherIterator *
mergeWriteIteratorWrapper (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  MergeClauseDetails *pDetails = (MergeClauseDetails *)pPlan->pExecState;
  CypherIterator *pIterator;
  MergeIteratorData *pData;

  fprintf (stderr, "DEBUG mergeWriteIteratorWrapper called, pDetails=%p\n",
           (void *)pDetails);
  if (!pDetails)
    {
      fprintf (stderr, "DEBUG mergeWriteIteratorWrapper: NO DETAILS! Falling "
                       "back to CREATE\n");
      return createWriteIteratorWrapper (pPlan, pContext);
    }
  fprintf (stderr,
           "DEBUG mergeWriteIteratorWrapper: has details, nMatchProps=%d "
           "hasRel=%d\n",
           pDetails->nMatchProps, pDetails->bHasRelationship);

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;
  pData = sqlite3_malloc (sizeof (MergeIteratorData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (MergeIteratorData));
  pData->pDetails = pDetails;

  pIterator->xOpen = mergeIteratorOpen;
  pIterator->xNext = mergeIteratorNext;
  pIterator->xClose = mergeIteratorClose;
  pIterator->xDestroy = mergeIteratorDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}

static CypherIterator *
setWriteIteratorWrapper (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  /* TODO: Implement SET iterator wrapper */
  return NULL;
}

static int
deleteIteratorOpen (CypherIterator *pIterator)
{
  DeleteIteratorData *pData = (DeleteIteratorData *)pIterator->pIterData;
  ExecutionContext *pContext = pIterator->pContext;

  if (!pContext || !pContext->pDb || !pContext->pGraph)
    return SQLITE_ERROR;

  /* Create write context for mutations */
  pData->pWriteCtx
      = cypherWriteContextCreate (pContext->pDb, pContext->pGraph, pContext);
  if (!pData->pWriteCtx)
    return SQLITE_NOMEM;

  pIterator->bOpened = 1;
  return SQLITE_OK;
}

static int
deleteIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  DeleteIteratorData *pData = (DeleteIteratorData *)pIterator->pIterData;
  ExecutionContext *pContext = pIterator->pContext;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  int rc;

  /* DELETE operations consume all input rows from child iterator */
  if (pIterator->nChildren == 0)
    {
      /* No child iterator - this is a standalone DELETE */
      /* This shouldn't happen in well-formed plans, but handle gracefully */
      return SQLITE_DONE;
    }

  CypherIterator *pChild = pIterator->apChildren[0];
  if (!pChild)
    return SQLITE_ERROR;

  /* Get next row from child */
  rc = pChild->xNext (pChild, pResult);
  if (rc != SQLITE_OK)
    return rc;

  /* For each variable to delete, execute the deletion */
  if (pData->zVariable)
    {
      /* Look up the variable in the result row */
      CypherValue *pValue = NULL;
      int i;

      for (i = 0; i < pResult->nColumns; i++)
        {
          if (strcmp (pResult->azColumnNames[i], pData->zVariable) == 0)
            {
              pValue = &pResult->aValues[i];
              break;
            }
        }

      if (pValue)
        {
          /* Create delete operation based on value type */
          DeleteOp *pDeleteOp = cypherDeleteOpCreate ();
          if (!pDeleteOp)
            return SQLITE_NOMEM;

          if (pValue->type == CYPHER_VALUE_NODE)
            {
              /* Delete node */
              pDeleteOp->bIsNode = 1;
              pDeleteOp->iNodeId = pValue->u.iNodeId;
              pDeleteOp->bDetach = pData->bDetach;
            }
          else if (pValue->type == CYPHER_VALUE_RELATIONSHIP)
            {
              /* Delete relationship */
              pDeleteOp->bIsNode = 0;
              pDeleteOp->iRelId = pValue->u.iRelId;
              /* DETACH doesn't apply to relationships */
              pDeleteOp->bDetach = 0;
            }
          else if (pValue->type == CYPHER_VALUE_LIST)
            {
              /* Delete from list - iterate through list elements */
              int i;
              for (i = 0; i < pValue->u.list.nValues; i++)
                {
                  CypherValue *pListItem = &pValue->u.list.apValues[i];
                  if (pListItem->type == CYPHER_VALUE_NODE)
                    {
                      pDeleteOp->bIsNode = 1;
                      pDeleteOp->iNodeId = pListItem->u.iNodeId;
                      pDeleteOp->bDetach = pData->bDetach;
                      rc = cypherDelete (pData->pWriteCtx, pDeleteOp);
                      if (rc != SQLITE_OK)
                        {
                          cypherDeleteOpDestroy (pDeleteOp);
                          return rc;
                        }
                    }
                  else if (pListItem->type == CYPHER_VALUE_RELATIONSHIP)
                    {
                      pDeleteOp->bIsNode = 0;
                      pDeleteOp->iRelId = pListItem->u.iRelId;
                      pDeleteOp->bDetach = 0;
                      rc = cypherDelete (pData->pWriteCtx, pDeleteOp);
                      if (rc != SQLITE_OK)
                        {
                          cypherDeleteOpDestroy (pDeleteOp);
                          return rc;
                        }
                    }
                }
              cypherDeleteOpDestroy (pDeleteOp);
              return SQLITE_OK; /* Return original row */
            }
          else if (pValue->type == CYPHER_VALUE_MAP)
            {
              /* Delete from map - iterate through map values */
              int i;
              for (i = 0; i < pValue->u.map.nPairs; i++)
                {
                  CypherValue *pMapValue = &pValue->u.map.apValues[i];
                  if (pMapValue->type == CYPHER_VALUE_NODE)
                    {
                      pDeleteOp->bIsNode = 1;
                      pDeleteOp->iNodeId = pMapValue->u.iNodeId;
                      pDeleteOp->bDetach = pData->bDetach;
                      rc = cypherDelete (pData->pWriteCtx, pDeleteOp);
                      if (rc != SQLITE_OK)
                        {
                          cypherDeleteOpDestroy (pDeleteOp);
                          return rc;
                        }
                    }
                  else if (pMapValue->type == CYPHER_VALUE_RELATIONSHIP)
                    {
                      pDeleteOp->bIsNode = 0;
                      pDeleteOp->iRelId = pMapValue->u.iRelId;
                      pDeleteOp->bDetach = 0;
                      rc = cypherDelete (pData->pWriteCtx, pDeleteOp);
                      if (rc != SQLITE_OK)
                        {
                          cypherDeleteOpDestroy (pDeleteOp);
                          return rc;
                        }
                    }
                }
              cypherDeleteOpDestroy (pDeleteOp);
              return SQLITE_OK; /* Return original row */
            }
          else if (pValue->type == CYPHER_VALUE_PATH)
            {
              /* Delete path - this expands to deleting all nodes and
               * relationships */
              /* For now, treat as unsupported */
              cypherDeleteOpDestroy (pDeleteOp);
              return SQLITE_ERROR; /* Path deletion not implemented */
            }
          else if (pValue->type == CYPHER_VALUE_NULL)
            {
              /* NULL values are ignored */
              cypherDeleteOpDestroy (pDeleteOp);
              return SQLITE_OK; /* Return original row */
            }
          else
            {
              /* Invalid type for DELETE */
              cypherDeleteOpDestroy (pDeleteOp);
              return SQLITE_ERROR; /* Invalid argument type */
            }

          /* Execute the delete operation */
          rc = cypherDelete (pData->pWriteCtx, pDeleteOp);
          cypherDeleteOpDestroy (pDeleteOp);
          if (rc != SQLITE_OK)
            return rc;
        }
    }

  /* Return the original row */
  return SQLITE_OK;
}

static int
deleteIteratorClose (CypherIterator *pIterator)
{
  DeleteIteratorData *pData = (DeleteIteratorData *)pIterator->pIterData;

  if (pData && pData->pWriteCtx)
    {
      /* Commit any pending operations */
      cypherWriteContextCommit (pData->pWriteCtx);
    }

  return SQLITE_OK;
}

static void
deleteIteratorDestroy (CypherIterator *pIterator)
{
  DeleteIteratorData *pData = (DeleteIteratorData *)pIterator->pIterData;

  if (pData)
    {
      if (pData->pWriteCtx)
        {
          cypherWriteContextDestroy (pData->pWriteCtx);
        }
      if (pData->zVariable)
        {
          sqlite3_free (pData->zVariable);
        }
      sqlite3_free (pData);
    }
}

static CypherIterator *
deleteWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                            ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  DeleteIteratorData *pData;

  pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;

  pData = sqlite3_malloc (sizeof (DeleteIteratorData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }

  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (DeleteIteratorData));

  /* Extract delete parameters from physical plan */
  pData->zVariable
      = pPlan->zAlias ? sqlite3_mprintf ("%s", pPlan->zAlias) : NULL;
  pData->bDetach = (pPlan->type == PHYSICAL_DETACH_DELETE);

  /* Set up iterator */
  pIterator->xOpen = deleteIteratorOpen;
  pIterator->xNext = deleteIteratorNext;
  pIterator->xClose = deleteIteratorClose;
  pIterator->xDestroy = deleteIteratorDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}
