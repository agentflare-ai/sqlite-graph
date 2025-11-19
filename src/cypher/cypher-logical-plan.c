/* Logical query plan - AST compilation and optimization */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-planner.h"
#include <string.h>
#include <assert.h>

/*
** Create a new logical plan node.
** Returns NULL on allocation failure.
*/
LogicalPlanNode *logicalPlanNodeCreate(LogicalPlanNodeType type) {
  LogicalPlanNode *pNode;
  
  pNode = sqlite3_malloc(sizeof(LogicalPlanNode));
  if( !pNode ) return NULL;
  
  memset(pNode, 0, sizeof(LogicalPlanNode));
  pNode->type = type;
  pNode->rEstimatedCost = 0.0;
  pNode->iEstimatedRows = 0;
  
  return pNode;
}

/*
** Destroy a logical plan node and all children recursively.
** Safe to call with NULL pointer.
*/
void logicalPlanNodeDestroy(LogicalPlanNode *pNode) {
  int i;
  
  if( !pNode ) return;
  
  /* Free child nodes recursively */
  for( i = 0; i < pNode->nChildren; i++ ) {
    logicalPlanNodeDestroy(pNode->apChildren[i]);
  }
  
  /* Free allocated memory */
  sqlite3_free(pNode->apChildren);
  sqlite3_free(pNode->zAlias);
  sqlite3_free(pNode->zPathVar);
  sqlite3_free(pNode->zLabel);
  sqlite3_free(pNode->zProperty);
  sqlite3_free(pNode->zValue);
  if( pNode->type == LOGICAL_MERGE && pNode->pExtra ) {
    mergeClauseDetailsDestroy((MergeClauseDetails*)pNode->pExtra);
    pNode->pExtra = NULL;
  }
  /* Remaining pExtra pointers reference parser-owned AST nodes. */
  sqlite3_free(pNode);
}

/*
** Add a child node to a logical plan node.
** Returns SQLITE_OK on success, SQLITE_NOMEM on allocation failure.
*/
int logicalPlanNodeAddChild(LogicalPlanNode *pParent, LogicalPlanNode *pChild) {
  LogicalPlanNode **apNew;
  
  if( !pParent || !pChild ) return SQLITE_MISUSE;
  
  /* Resize children array if needed */
  if( pParent->nChildren >= pParent->nChildrenAlloc ) {
    int nNew = pParent->nChildrenAlloc ? pParent->nChildrenAlloc * 2 : 4;
    apNew = sqlite3_realloc(pParent->apChildren, 
                           nNew * sizeof(LogicalPlanNode*));
    if( !apNew ) return SQLITE_NOMEM;
    
    pParent->apChildren = apNew;
    pParent->nChildrenAlloc = nNew;
  }
  
  /* Add child and set parent reference */
  pParent->apChildren[pParent->nChildren++] = pChild;
  pChild->pParent = pParent;
  
  return SQLITE_OK;
}

/*
** Set string properties of a logical plan node.
** Makes a copy of the string using sqlite3_malloc().
*/
int logicalPlanNodeSetAlias(LogicalPlanNode *pNode, const char *zAlias) {
  char *zNew;
  
  if( !pNode ) return SQLITE_MISUSE;
  if( !zAlias ) {
    sqlite3_free(pNode->zAlias);
    pNode->zAlias = NULL;
    return SQLITE_OK;
  }
  
  zNew = sqlite3_mprintf("%s", zAlias);
  if( !zNew ) return SQLITE_NOMEM;
  
  sqlite3_free(pNode->zAlias);
  pNode->zAlias = zNew;
  return SQLITE_OK;
}

int logicalPlanNodeSetLabel(LogicalPlanNode *pNode, const char *zLabel) {
  char *zNew;
  
  if( !pNode ) return SQLITE_MISUSE;
  if( !zLabel ) {
    sqlite3_free(pNode->zLabel);
    pNode->zLabel = NULL;
    return SQLITE_OK;
  }
  
  zNew = sqlite3_mprintf("%s", zLabel);
  if( !zNew ) return SQLITE_NOMEM;
  
  sqlite3_free(pNode->zLabel);
  pNode->zLabel = zNew;
  return SQLITE_OK;
}

int logicalPlanNodeSetProperty(LogicalPlanNode *pNode, const char *zProperty) {
  char *zNew;
  
  if( !pNode ) return SQLITE_MISUSE;
  if( !zProperty ) {
    sqlite3_free(pNode->zProperty);
    pNode->zProperty = NULL;
    return SQLITE_OK;
  }
  
  zNew = sqlite3_mprintf("%s", zProperty);
  if( !zNew ) return SQLITE_NOMEM;
  
  sqlite3_free(pNode->zProperty);
  pNode->zProperty = zNew;
  return SQLITE_OK;
}

int logicalPlanNodeSetValue(LogicalPlanNode *pNode, const char *zValue) {
  char *zNew;
  
  if( !pNode ) return SQLITE_MISUSE;
  if( !zValue ) {
    sqlite3_free(pNode->zValue);
    pNode->zValue = NULL;
    return SQLITE_OK;
  }
  
  zNew = sqlite3_mprintf("%s", zValue);
  if( !zNew ) return SQLITE_NOMEM;
  
  sqlite3_free(pNode->zValue);
  pNode->zValue = zNew;
  return SQLITE_OK;
}

/*
** Get string representation of logical plan node type.
** Returns static string, do not free.
*/
const char *logicalPlanNodeTypeName(LogicalPlanNodeType type) {
  switch( type ) {
    case LOGICAL_NODE_SCAN:         return "NODE_SCAN";
    case LOGICAL_LABEL_SCAN:        return "LABEL_SCAN";
    case LOGICAL_INDEX_SCAN:        return "INDEX_SCAN";
    case LOGICAL_RELATIONSHIP_SCAN: return "RELATIONSHIP_SCAN";
    case LOGICAL_TYPE_SCAN:         return "TYPE_SCAN";
    case LOGICAL_EXPAND:            return "EXPAND";
    case LOGICAL_VAR_LENGTH_EXPAND: return "VAR_LENGTH_EXPAND";
    case LOGICAL_OPTIONAL_EXPAND:   return "OPTIONAL_EXPAND";
    case LOGICAL_FILTER:            return "FILTER";
    case LOGICAL_PROPERTY_FILTER:   return "PROPERTY_FILTER";
    case LOGICAL_LABEL_FILTER:      return "LABEL_FILTER";
    case LOGICAL_HASH_JOIN:         return "HASH_JOIN";
    case LOGICAL_NESTED_LOOP_JOIN:  return "NESTED_LOOP_JOIN";
    case LOGICAL_CARTESIAN_PRODUCT: return "CARTESIAN_PRODUCT";
    case LOGICAL_PROJECTION:        return "PROJECTION";
    case LOGICAL_WITH:              return "WITH";
    case LOGICAL_DISTINCT:          return "DISTINCT";
    case LOGICAL_AGGREGATION:       return "AGGREGATION";
    case LOGICAL_SORT:              return "SORT";
    case LOGICAL_LIMIT:             return "LIMIT";
    case LOGICAL_SKIP:              return "SKIP";
    case LOGICAL_CREATE:            return "CREATE";
    case LOGICAL_MERGE:             return "MERGE";
    case LOGICAL_SET:               return "SET";
    case LOGICAL_DELETE:            return "DELETE";
    case LOGICAL_DETACH_DELETE:     return "DETACH_DELETE";
    default:                        return "UNKNOWN";
  }
}

/*
** Estimate the cost of executing a logical plan node.
** Uses simple heuristics for cost calculation.
*/
double logicalPlanEstimateCost(LogicalPlanNode *pNode, PlanContext *pContext) {
  double rCost = 0.0;
  int i;
  
  if( !pNode ) return 0.0;
  
  /* Base cost depends on operation type */
  switch( pNode->type ) {
    case LOGICAL_NODE_SCAN:
      /* Full table scan - expensive */
      rCost = 1000.0;
      break;
      
    case LOGICAL_LABEL_SCAN:
      /* Label index scan - much cheaper */
      rCost = 10.0;
      break;
      
    case LOGICAL_INDEX_SCAN:
      /* Property index scan - very cheap */
      rCost = 1.0;
      break;
      
    case LOGICAL_FILTER:
      /* Filter cost depends on selectivity */
      rCost = 1.0;
      break;
      
    case LOGICAL_EXPAND:
      /* Relationship traversal */
      rCost = 5.0;
      break;
      
    case LOGICAL_HASH_JOIN:
      /* Hash join cost */
      rCost = 10.0;
      break;
      
    case LOGICAL_NESTED_LOOP_JOIN:
      /* Nested loop - expensive */
      rCost = 100.0;
      break;
      
    case LOGICAL_PROJECTION:
      /* Projection is cheap */
      rCost = 0.1;
      break;
      
    case LOGICAL_SORT:
      /* Sorting cost */
      rCost = 50.0;
      break;
      
    default:
      rCost = 1.0;
      break;
  }
  
  /* Add costs of children */
  for( i = 0; i < pNode->nChildren; i++ ) {
    rCost += logicalPlanEstimateCost(pNode->apChildren[i], pContext);
  }
  
  pNode->rEstimatedCost = rCost;
  return rCost;
}

/*
** Estimate the number of rows produced by a logical plan node.
** Uses simple cardinality estimation.
*/
sqlite3_int64 logicalPlanEstimateRows(LogicalPlanNode *pNode, PlanContext *pContext) {
  sqlite3_int64 iRows = 0;
  
  if( !pNode ) return 0;
  
  /* Estimate based on operation type */
  switch( pNode->type ) {
    case LOGICAL_NODE_SCAN:
      /* Assume reasonable graph size */
      iRows = 10000;
      break;
      
    case LOGICAL_LABEL_SCAN:
      /* Labels are selective */
      iRows = 1000;
      break;
      
    case LOGICAL_INDEX_SCAN:
      /* Property indexes are very selective */
      iRows = 100;
      break;
      
    case LOGICAL_FILTER:
      /* Filters reduce cardinality */
      if( pNode->nChildren > 0 ) {
        iRows = logicalPlanEstimateRows(pNode->apChildren[0], pContext) / 10;
      } else {
        iRows = 100;
      }
      break;
      
    case LOGICAL_EXPAND:
      /* Relationship expansion multiplies rows */
      if( pNode->nChildren > 0 ) {
        iRows = logicalPlanEstimateRows(pNode->apChildren[0], pContext) * 5;
      } else {
        iRows = 500;
      }
      break;
      
    case LOGICAL_HASH_JOIN:
    case LOGICAL_NESTED_LOOP_JOIN:
      /* Join multiplies cardinalities */
      if( pNode->nChildren >= 2 ) {
        sqlite3_int64 iLeft = logicalPlanEstimateRows(pNode->apChildren[0], pContext);
        sqlite3_int64 iRight = logicalPlanEstimateRows(pNode->apChildren[1], pContext);
        iRows = (iLeft * iRight) / 100; /* Assume some selectivity */
      } else {
        iRows = 1000;
      }
      break;
      
    case LOGICAL_PROJECTION:
    case LOGICAL_DISTINCT:
      /* Projection doesn't change cardinality much */
      if( pNode->nChildren > 0 ) {
        iRows = logicalPlanEstimateRows(pNode->apChildren[0], pContext);
      } else {
        iRows = 100;
      }
      break;
      
    case LOGICAL_LIMIT:
      /* Limit reduces cardinality */
      iRows = 10; /* Assume small limit */
      break;
      
    default:
      /* Default estimate */
      if( pNode->nChildren > 0 ) {
        iRows = logicalPlanEstimateRows(pNode->apChildren[0], pContext);
      } else {
        iRows = 100;
      }
      break;
  }
  
  pNode->iEstimatedRows = iRows;
  return iRows;
}

/*
** Generate string representation of logical plan tree.
** Caller must sqlite3_free() the returned string.
*/
char *logicalPlanToString(LogicalPlanNode *pNode) {
  char *zResult;
  char *zChildren = NULL;
  int i;
  
  if( !pNode ) return sqlite3_mprintf("(null)");
  
  /* Build children string */
  if( pNode->nChildren > 0 ) {
    char *zChild;
    
    for( i = 0; i < pNode->nChildren; i++ ) {
      zChild = logicalPlanToString(pNode->apChildren[i]);
      if( zChild ) {
        if( zChildren ) {
          char *zNew = sqlite3_mprintf("%s, %s", zChildren, zChild);
          sqlite3_free(zChildren);
          sqlite3_free(zChild);
          zChildren = zNew;
        } else {
          zChildren = zChild;
        }
      }
    }
  }
  
  /* Build node string */
  if( pNode->zAlias && pNode->zLabel ) {
    /* Both alias and label */
    zResult = sqlite3_mprintf("%s(%s:%s cost=%.1f rows=%lld%s%s)",
                             logicalPlanNodeTypeName(pNode->type),
                             pNode->zAlias,
                             pNode->zLabel,
                             pNode->rEstimatedCost,
                             pNode->iEstimatedRows,
                             zChildren ? " [" : "",
                             zChildren ? zChildren : "");
  } else if( pNode->zAlias ) {
    /* Just alias */
    zResult = sqlite3_mprintf("%s(%s cost=%.1f rows=%lld%s%s)",
                             logicalPlanNodeTypeName(pNode->type),
                             pNode->zAlias,
                             pNode->rEstimatedCost,
                             pNode->iEstimatedRows,
                             zChildren ? " [" : "",
                             zChildren ? zChildren : "");
  } else if( pNode->zLabel ) {
    /* Just label */
    zResult = sqlite3_mprintf("%s(:%s cost=%.1f rows=%lld%s%s)",
                             logicalPlanNodeTypeName(pNode->type),
                             pNode->zLabel,
                             pNode->rEstimatedCost,
                             pNode->iEstimatedRows,
                             zChildren ? " [" : "",
                             zChildren ? zChildren : "");
  } else {
    /* Neither */
    zResult = sqlite3_mprintf("%s(cost=%.1f rows=%lld%s%s)",
                             logicalPlanNodeTypeName(pNode->type),
                             pNode->rEstimatedCost,
                             pNode->iEstimatedRows,
                             zChildren ? " [" : "",
                             zChildren ? zChildren : "");
  }
  
  if( zChildren && zResult ) {
    char *zFinal = sqlite3_mprintf("%s]", zResult);
    sqlite3_free(zResult);
    zResult = zFinal;
  }
  
  sqlite3_free(zChildren);
  return zResult;
}

/*
** Create a simple test logical plan for demonstration.
** Represents: MATCH (n:Person) WHERE n.age > 30 RETURN n.name
*/
LogicalPlanNode *cypherCreateTestLogicalPlan(void) {
  LogicalPlanNode *pProjection, *pFilter, *pScan;
  int rc;
  
  /* Create nodes */
  pProjection = logicalPlanNodeCreate(LOGICAL_PROJECTION);
  pFilter = logicalPlanNodeCreate(LOGICAL_PROPERTY_FILTER);
  pScan = logicalPlanNodeCreate(LOGICAL_LABEL_SCAN);
  
  if( !pProjection || !pFilter || !pScan ) {
    logicalPlanNodeDestroy(pProjection);
    logicalPlanNodeDestroy(pFilter);
    logicalPlanNodeDestroy(pScan);
    return NULL;
  }
  
  /* Set properties */
  logicalPlanNodeSetAlias(pScan, "n");
  logicalPlanNodeSetLabel(pScan, "Person");
  
  logicalPlanNodeSetAlias(pFilter, "n");
  logicalPlanNodeSetProperty(pFilter, "age");
  logicalPlanNodeSetValue(pFilter, "30");
  
  logicalPlanNodeSetAlias(pProjection, "n");
  logicalPlanNodeSetProperty(pProjection, "name");
  
  /* Build tree structure */
  rc = logicalPlanNodeAddChild(pFilter, pScan);
  if( rc == SQLITE_OK ) {
    rc = logicalPlanNodeAddChild(pProjection, pFilter);
  }
  
  if( rc != SQLITE_OK ) {
    logicalPlanNodeDestroy(pProjection);
    return NULL;
  }
  
  /* Update cost estimates */
  logicalPlanEstimateCost(pProjection, NULL);
  logicalPlanEstimateRows(pProjection, NULL);
  
  return pProjection;
}
