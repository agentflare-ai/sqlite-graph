/*
** SQLite Graph Database Extension - Cypher Query Planner
**
** This file implements the main query planner that compiles Cypher ASTs
** into optimized logical and physical execution plans. The planner handles
** pattern recognition, cost estimation, and operator selection.
**
** Features:
** - AST to logical plan compilation
** - Pattern optimization and rewriting
** - Cost-based physical plan generation
** - Index utilization planning
** - Join ordering optimization
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes
*/

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-planner.h"
#include <string.h>
#include <assert.h>

/* Forward declarations for optimization functions */
static double calculateJoinCost(LogicalPlanNode *pLeft, LogicalPlanNode *pRight, int joinType);
static int optimizeIndexUsage(LogicalPlanNode *pNode, PlanContext *pContext);

/*
** Create a new Cypher query planner.
** Returns NULL on allocation failure.
*/
CypherPlanner *cypherPlannerCreate(sqlite3 *pDb, GraphVtab *pGraph) {
  CypherPlanner *pPlanner;
  
  pPlanner = sqlite3_malloc(sizeof(CypherPlanner));
  if( !pPlanner ) return NULL;
  
  memset(pPlanner, 0, sizeof(CypherPlanner));
  pPlanner->pDb = pDb;
  
  /* Create planning context */
  pPlanner->pContext = sqlite3_malloc(sizeof(PlanContext));
  if( !pPlanner->pContext ) {
    sqlite3_free(pPlanner);
    return NULL;
  }
  
  memset(pPlanner->pContext, 0, sizeof(PlanContext));
  pPlanner->pContext->pDb = pDb;
  pPlanner->pContext->pGraph = pGraph;
  
  /* Set default optimization settings */
  pPlanner->pContext->bUseIndexes = 1;
  pPlanner->pContext->bReorderJoins = 1;
  pPlanner->pContext->rIndexCostFactor = 0.1;
  
  return pPlanner;
}

/*
** Destroy a Cypher planner and free all associated memory.
** Safe to call with NULL pointer.
*/
void cypherPlannerDestroy(CypherPlanner *pPlanner) {
  int i;
  
  if( !pPlanner ) return;
  
  /* Free context */
  if( pPlanner->pContext ) {
    /* Free variable arrays */
    for( i = 0; i < pPlanner->pContext->nVariables; i++ ) {
      sqlite3_free(pPlanner->pContext->azVariables[i]);
    }
    sqlite3_free(pPlanner->pContext->azVariables);
    sqlite3_free(pPlanner->pContext->apVarNodes);
    
    /* Free index arrays */
    for( i = 0; i < pPlanner->pContext->nLabelIndexes; i++ ) {
      sqlite3_free(pPlanner->pContext->azLabelIndexes[i]);
    }
    sqlite3_free(pPlanner->pContext->azLabelIndexes);
    
    for( i = 0; i < pPlanner->pContext->nPropertyIndexes; i++ ) {
      sqlite3_free(pPlanner->pContext->azPropertyIndexes[i]);
    }
    sqlite3_free(pPlanner->pContext->azPropertyIndexes);
    
    sqlite3_free(pPlanner->pContext->zErrorMsg);
    sqlite3_free(pPlanner->pContext);
  }
  
  /* Free plans */
  logicalPlanNodeDestroy(pPlanner->pLogicalPlan);
  physicalPlanNodeDestroy(pPlanner->pPhysicalPlan);
  
  sqlite3_free(pPlanner->zErrorMsg);
  sqlite3_free(pPlanner);
}

/*
** Add a variable to the planning context.
** Returns SQLITE_OK on success, SQLITE_NOMEM on allocation failure.
*/
static int planContextAddVariable(PlanContext *pContext, const char *zVar, LogicalPlanNode *pNode) {
  char **azNew;
  LogicalPlanNode **apNew;
  
  if( !pContext || !zVar ) return SQLITE_MISUSE;
  
  /* Resize arrays if needed */
  if( pContext->nVariables >= pContext->nVariablesAlloc ) {
    int nNew = pContext->nVariablesAlloc ? pContext->nVariablesAlloc * 2 : 8;
    
    azNew = sqlite3_realloc(pContext->azVariables, nNew * sizeof(char*));
    if( !azNew ) return SQLITE_NOMEM;
    pContext->azVariables = azNew;
    
    apNew = sqlite3_realloc(pContext->apVarNodes, nNew * sizeof(LogicalPlanNode*));
    if( !apNew ) return SQLITE_NOMEM;
    pContext->apVarNodes = apNew;
    
    pContext->nVariablesAlloc = nNew;
  }
  
  /* Add variable */
  pContext->azVariables[pContext->nVariables] = sqlite3_mprintf("%s", zVar);
  pContext->apVarNodes[pContext->nVariables] = pNode;
  
  if( !pContext->azVariables[pContext->nVariables] ) {
    return SQLITE_NOMEM;
  }
  
  pContext->nVariables++;
  return SQLITE_OK;
}


/*
** Compile a Cypher AST node into a logical plan node.
** Returns the compiled logical plan node, or NULL on error.
*/
static LogicalPlanNode *compileAstNode(CypherAst *pAst, PlanContext *pContext) {
  LogicalPlanNode *pLogical = NULL;
  LogicalPlanNode *pChild;
  const char *zAlias;
  int i;
  
  if( !pAst ) return NULL;
  
  switch( pAst->type ) {
    case CYPHER_AST_QUERY:
    case CYPHER_AST_SINGLE_QUERY:
      /* Compile children and combine them */
      if( pAst->nChildren > 0 ) {
        pLogical = compileAstNode(pAst->apChildren[0], pContext);
        
        for( i = 1; i < pAst->nChildren; i++ ) {
          pChild = compileAstNode(pAst->apChildren[i], pContext);
          if( pChild && pLogical ) {
            /* Create a join or sequence node */
            LogicalPlanNode *pJoin = logicalPlanNodeCreate(LOGICAL_HASH_JOIN);
            if( pJoin ) {
              logicalPlanNodeAddChild(pJoin, pLogical);
              logicalPlanNodeAddChild(pJoin, pChild);
              pLogical = pJoin;
            }
          }
        }
      }
      break;
      
    case CYPHER_AST_MATCH:
      /* Compile MATCH clause */
      if( pAst->nChildren > 0 ) {
        pLogical = compileAstNode(pAst->apChildren[0], pContext);
      }
      break;
      
    case CYPHER_AST_NODE_PATTERN:
      /* Node pattern becomes a scan operation */
      if( pAst->nChildren > 0 && cypherAstIsType(pAst->apChildren[0], CYPHER_AST_IDENTIFIER) ) {
        zAlias = cypherAstGetValue(pAst->apChildren[0]);
        
        /* Check if this is a labeled node */
        if( pAst->nChildren > 1 && cypherAstIsType(pAst->apChildren[1], CYPHER_AST_LABELS) ) {
          /* Label scan */
          pLogical = logicalPlanNodeCreate(LOGICAL_LABEL_SCAN);
          if( pLogical ) {
            logicalPlanNodeSetAlias(pLogical, zAlias);
            
            /* Get first label */
            if( pAst->apChildren[1]->nChildren > 0 ) {
              const char *zLabel = cypherAstGetValue(pAst->apChildren[1]->apChildren[0]);
              logicalPlanNodeSetLabel(pLogical, zLabel);
            }
            
            /* Add variable to context */
            planContextAddVariable(pContext, zAlias, pLogical);
          }
        } else {
          /* Full node scan */
          pLogical = logicalPlanNodeCreate(LOGICAL_NODE_SCAN);
          if( pLogical ) {
            logicalPlanNodeSetAlias(pLogical, zAlias);
            planContextAddVariable(pContext, zAlias, pLogical);
          }
        }
      }
      break;
      
    case CYPHER_AST_WHERE:
      /* WHERE clause becomes a filter */
      if( pAst->nChildren > 0 ) {
        CypherAst *pExpr = pAst->apChildren[0];
        
        if( cypherAstIsType(pExpr, CYPHER_AST_BINARY_OP) && 
            strcmp(cypherAstGetValue(pExpr), "=") == 0 &&
            pExpr->nChildren >= 2 ) {
          
          /* Property filter: n.prop = value */
          if( cypherAstIsType(pExpr->apChildren[0], CYPHER_AST_PROPERTY) ) {
            pLogical = logicalPlanNodeCreate(LOGICAL_PROPERTY_FILTER);
            if( pLogical ) {
              CypherAst *pProp = pExpr->apChildren[0];
              if( pProp->nChildren >= 2 ) {
                const char *zVar = cypherAstGetValue(pProp->apChildren[0]);
                const char *zProp = cypherAstGetValue(pProp->apChildren[1]);
                const char *zValue = cypherAstGetValue(pExpr->apChildren[1]);
                
                logicalPlanNodeSetAlias(pLogical, zVar);
                logicalPlanNodeSetProperty(pLogical, zProp);
                logicalPlanNodeSetValue(pLogical, zValue);
              }
            }
          }
        }
        
        if( !pLogical ) {
          /* Generic filter */
          pLogical = logicalPlanNodeCreate(LOGICAL_FILTER);
        }
      }
      break;
      
    case CYPHER_AST_RETURN:
      /* RETURN clause becomes a projection */
      pLogical = logicalPlanNodeCreate(LOGICAL_PROJECTION);
      if( pLogical && pAst->nChildren > 0 ) {
        /* Process projection list */
        CypherAst *pProjList = pAst->apChildren[0];
        if( cypherAstIsType(pProjList, CYPHER_AST_PROJECTION_LIST) && 
            pProjList->nChildren > 0 ) {
          CypherAst *pItem = pProjList->apChildren[0];
          if( cypherAstIsType(pItem, CYPHER_AST_PROJECTION_ITEM) && 
              pItem->nChildren > 0 ) {
            CypherAst *pExpr = pItem->apChildren[0];
            
            if( cypherAstIsType(pExpr, CYPHER_AST_IDENTIFIER) ) {
              logicalPlanNodeSetAlias(pLogical, cypherAstGetValue(pExpr));
            } else if( cypherAstIsType(pExpr, CYPHER_AST_PROPERTY) && 
                       pExpr->nChildren >= 2 ) {
              const char *zVar = cypherAstGetValue(pExpr->apChildren[0]);
              const char *zProp = cypherAstGetValue(pExpr->apChildren[1]);
              
              logicalPlanNodeSetAlias(pLogical, zVar);
              logicalPlanNodeSetProperty(pLogical, zProp);
            }
          }
        }
      }
      break;
      
    default:
      /* Unsupported AST node type */
      pContext->zErrorMsg = sqlite3_mprintf("Unsupported AST node type: %d", pAst->type);
      pContext->nErrors++;
      break;
  }
  
  return pLogical;
}

/*
** Compile an AST into a logical plan.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherPlannerCompile(CypherPlanner *pPlanner, CypherAst *pAst) {
  LogicalPlanNode *pRoot;
  
  if( !pPlanner || !pAst ) return SQLITE_MISUSE;
  
  /* Clean up any previous plan */
  logicalPlanNodeDestroy(pPlanner->pLogicalPlan);
  pPlanner->pLogicalPlan = NULL;
  physicalPlanNodeDestroy(pPlanner->pPhysicalPlan);
  pPlanner->pPhysicalPlan = NULL;
  
  sqlite3_free(pPlanner->zErrorMsg);
  pPlanner->zErrorMsg = NULL;
  
  /* Reset context */
  pPlanner->pContext->pAst = pAst;
  pPlanner->pContext->nErrors = 0;
  sqlite3_free(pPlanner->pContext->zErrorMsg);
  pPlanner->pContext->zErrorMsg = NULL;
  
  /* Compile AST to logical plan */
  pRoot = compileAstNode(pAst, pPlanner->pContext);
  if( !pRoot ) {
    if( pPlanner->pContext->zErrorMsg ) {
      pPlanner->zErrorMsg = sqlite3_mprintf("Compilation failed: %s", 
                                           pPlanner->pContext->zErrorMsg);
    } else {
      pPlanner->zErrorMsg = sqlite3_mprintf("Failed to compile AST to logical plan");
    }
    return SQLITE_ERROR;
  }
  
  pPlanner->pLogicalPlan = pRoot;
  
  /* Estimate costs */
  logicalPlanEstimateCost(pRoot, pPlanner->pContext);
  logicalPlanEstimateRows(pRoot, pPlanner->pContext);
  
  return SQLITE_OK;
}

/*
** Optimize the logical plan and generate physical plan.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherPlannerOptimize(CypherPlanner *pPlanner) {
  PhysicalPlanNode *pPhysical;
  
  if( !pPlanner || !pPlanner->pLogicalPlan ) return SQLITE_MISUSE;
  
  /* Clean up any previous physical plan */
  physicalPlanNodeDestroy(pPlanner->pPhysicalPlan);
  pPlanner->pPhysicalPlan = NULL;
  
  /* Join reordering optimization */
  /* For Phase 1/2, basic join ordering is preserved from the query */
  /* Advanced join reordering based on cardinality would be added later */
  if( pPlanner->pContext->bReorderJoins ) {
    /* The logicalPlanOptimizeJoins function would analyze join predicates */
    /* and reorder based on estimated cardinalities */
    logicalPlanOptimizeJoins(pPlanner->pLogicalPlan, pPlanner->pContext);
  }
  
  /* Index usage optimization */
  optimizeIndexUsage(pPlanner->pLogicalPlan, pPlanner->pContext);
  
  /* Convert logical plan to physical plan */
  pPhysical = logicalPlanToPhysical(pPlanner->pLogicalPlan, pPlanner->pContext);
  if( !pPhysical ) {
    pPlanner->zErrorMsg = sqlite3_mprintf("Failed to generate physical plan");
    return SQLITE_ERROR;
  }
  
  pPlanner->pPhysicalPlan = pPhysical;
  return SQLITE_OK;
}

/*
** Get the final physical execution plan.
** Returns NULL if planning failed or not yet completed.
*/
PhysicalPlanNode *cypherPlannerGetPlan(CypherPlanner *pPlanner) {
  return pPlanner ? pPlanner->pPhysicalPlan : NULL;
}

/*
** Get error message from planner.
** Returns NULL if no error occurred.
*/
const char *cypherPlannerGetError(CypherPlanner *pPlanner) {
  return pPlanner ? pPlanner->zErrorMsg : NULL;
}

/*
** Optimize join ordering using simple heuristics.
** This is a simplified version - full implementation would use dynamic programming.
*/
int logicalPlanOptimizeJoins(LogicalPlanNode *pNode, PlanContext *pContext) {
  int i;
  
  if( !pNode ) return SQLITE_OK;
  
  /* Recursively optimize children first */
  for( i = 0; i < pNode->nChildren; i++ ) {
    logicalPlanOptimizeJoins(pNode->apChildren[i], pContext);
  }
  
  /* For join nodes, perform cost-based optimization */
  if( pNode->type == LOGICAL_HASH_JOIN || 
      pNode->type == LOGICAL_NESTED_LOOP_JOIN ) {
    if( pNode->nChildren >= 2 ) {
      LogicalPlanNode *pLeft = pNode->apChildren[0];
      LogicalPlanNode *pRight = pNode->apChildren[1];
      
      /* Calculate join costs for both orderings */
      double leftFirstCost = calculateJoinCost(pLeft, pRight, pNode->type);
      double rightFirstCost = calculateJoinCost(pRight, pLeft, pNode->type);
      
      /* Choose the more efficient ordering */
      if( rightFirstCost < leftFirstCost ) {
        pNode->apChildren[0] = pRight;
        pNode->apChildren[1] = pLeft;
      }
      
      /* Update estimated rows for the join */
      pNode->iEstimatedRows = (pLeft->iEstimatedRows * pRight->iEstimatedRows) / 10;
    }
  }
  
  return SQLITE_OK;
}

/*
** Calculate the estimated cost of a join operation.
** Uses cardinality estimates and join type to compute relative cost.
*/
static double calculateJoinCost(LogicalPlanNode *pLeft, LogicalPlanNode *pRight, int joinType) {
  double leftRows = pLeft->iEstimatedRows > 0 ? pLeft->iEstimatedRows : 1000.0;
  double rightRows = pRight->iEstimatedRows > 0 ? pRight->iEstimatedRows : 1000.0;
  double cost = 0.0;
  
  switch (joinType) {
    case LOGICAL_HASH_JOIN:
      /* Hash join: O(m + n) where m, n are input sizes */
      /* Cost includes hash table build (right side) + probe (left side) */
      cost = rightRows * 1.2 + leftRows * 1.0;  /* Build cost > probe cost */
      break;
      
    case LOGICAL_NESTED_LOOP_JOIN:
      /* Nested loop: O(m * n) - very expensive for large inputs */
      cost = leftRows * rightRows * 0.001;  /* Scale down to compare with hash */
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
static int optimizeIndexUsage(LogicalPlanNode *pNode, PlanContext *pContext) {
  int i;
  
  if (!pNode) return SQLITE_OK;
  
  /* Recursively optimize children first */
  for (i = 0; i < pNode->nChildren; i++) {
    optimizeIndexUsage(pNode->apChildren[i], pContext);
  }
  
  /* Optimize node scan operations */
  if (pNode->type == LOGICAL_NODE_SCAN) {
    /* Check if we have a label filter that can use label index */
    if (pNode->zLabel && strlen(pNode->zLabel) > 0) {
      /* Convert to label index scan - much more efficient */
      pNode->type = LOGICAL_LABEL_SCAN;
      pNode->iEstimatedRows = pNode->iEstimatedRows / 10;  /* Assume 10x improvement */
    }
    
    /* Check if we have property filters that can use property index */
    if (pNode->zProperty && pNode->zValue) {
      /* Convert to property index scan - highly selective */
      pNode->type = LOGICAL_INDEX_SCAN;
      pNode->iEstimatedRows = pNode->iEstimatedRows / 100;  /* Assume 100x improvement */
    }
  }
  
  return SQLITE_OK;
}