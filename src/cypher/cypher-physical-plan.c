/* Physical execution plan - converts logical plan to executable operators */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-planner.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

PhysicalPlanNode *physicalPlanNodeCreate(PhysicalOperatorType type) {
  PhysicalPlanNode *pNode;
  
  pNode = sqlite3_malloc(sizeof(PhysicalPlanNode));
  if( !pNode ) return NULL;
  
  memset(pNode, 0, sizeof(PhysicalPlanNode));
  pNode->type = type;
  pNode->rCost = 0.0;
  pNode->iRows = 0;
  pNode->rSelectivity = 1.0;
  
  return pNode;
}

/*
** Destroy a physical plan node and all children recursively.
** Safe to call with NULL pointer.
*/
void physicalPlanNodeDestroy(PhysicalPlanNode *pNode) {
  int i;
  
  if( !pNode ) return;
  
  /* Free child nodes recursively */
  for( i = 0; i < pNode->nChildren; i++ ) {
    physicalPlanNodeDestroy(pNode->apChildren[i]);
  }
  
  /* Free allocated memory */
  sqlite3_free(pNode->apChildren);
  sqlite3_free(pNode->zAlias);
  sqlite3_free(pNode->zIndexName);
  sqlite3_free(pNode->zLabel);
  sqlite3_free(pNode->zProperty);
  sqlite3_free(pNode->zValue);
  /* Don't free pExecState - it points to AST owned by parser */
  /* sqlite3_free(pNode->pExecState); */
  sqlite3_free(pNode);
}

/*
** Add a child node to a physical plan node.
** Returns SQLITE_OK on success, SQLITE_NOMEM on allocation failure.
*/
int physicalPlanNodeAddChild(PhysicalPlanNode *pParent, PhysicalPlanNode *pChild) {
  PhysicalPlanNode **apNew;

  if( !pParent || !pChild ) return SQLITE_MISUSE;

  /* Resize children array if needed */
  if( pParent->nChildren >= pParent->nChildrenAlloc ) {
    int nNew = pParent->nChildrenAlloc ? pParent->nChildrenAlloc * 2 : 4;
    apNew = sqlite3_realloc(pParent->apChildren,
                           nNew * sizeof(PhysicalPlanNode*));
    if( !apNew ) return SQLITE_NOMEM;

    pParent->apChildren = apNew;
    pParent->nChildrenAlloc = nNew;
  }

  /* Add child */
  pParent->apChildren[pParent->nChildren++] = pChild;

  /* For single-child operators (Projection, Filter, Sort, Limit),
     also set pChild pointer for convenience */
  if( pParent->nChildren == 1 ) {
    pParent->pChild = pChild;
  }

  return SQLITE_OK;
}

/*
** Get string representation of physical operator type.
** Returns static string, do not free.
*/
const char *physicalOperatorTypeName(PhysicalOperatorType type) {
  switch( type ) {
    case PHYSICAL_ALL_NODES_SCAN:     return "AllNodesScan";
    case PHYSICAL_LABEL_INDEX_SCAN:   return "LabelIndexScan";
    case PHYSICAL_PROPERTY_INDEX_SCAN: return "PropertyIndexScan";
    case PHYSICAL_ALL_RELS_SCAN:      return "AllRelsScan";
    case PHYSICAL_TYPE_INDEX_SCAN:    return "TypeIndexScan";
    case PHYSICAL_HASH_JOIN:          return "HashJoin";
    case PHYSICAL_NESTED_LOOP_JOIN:   return "NestedLoopJoin";
    case PHYSICAL_INDEX_NESTED_LOOP:  return "IndexNestedLoop";
    case PHYSICAL_EXPAND:             return "Expand";
    case PHYSICAL_FILTER:             return "Filter";
    case PHYSICAL_PROJECTION:         return "Projection";
    case PHYSICAL_SORT:               return "Sort";
    case PHYSICAL_LIMIT:              return "Limit";
    case PHYSICAL_AGGREGATION:        return "Aggregation";
    case PHYSICAL_CREATE:             return "Create";
    case PHYSICAL_MERGE:              return "Merge";
    case PHYSICAL_SET:                return "Set";
    case PHYSICAL_DELETE:             return "Delete";
    case PHYSICAL_REMOVE:             return "Remove";
    default:                          return "Unknown";
  }
}

/*
** Convert logical plan to physical plan with operator selection.
** Chooses the best physical operator for each logical operation.
*/
PhysicalPlanNode *logicalPlanToPhysical(LogicalPlanNode *pLogical, PlanContext *pContext) {
  PhysicalPlanNode *pPhysical = NULL;
  PhysicalPlanNode *pChild;
  int i, rc;
  
  if( !pLogical ) return NULL;
  
  /* Select physical operator based on logical operation */
  switch( pLogical->type ) {
    case LOGICAL_NODE_SCAN:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_ALL_NODES_SCAN);
      break;
      
    case LOGICAL_LABEL_SCAN:
      /* Choose between full scan and label index scan */
      if( pContext && pContext->bUseIndexes && pLogical->zLabel ) {
        pPhysical = physicalPlanNodeCreate(PHYSICAL_LABEL_INDEX_SCAN);
        if( pPhysical ) {
          pPhysical->zLabel = sqlite3_mprintf("%s", pLogical->zLabel);
          pPhysical->rCost = pLogical->rEstimatedCost * pContext->rIndexCostFactor;
        }
      } else {
        pPhysical = physicalPlanNodeCreate(PHYSICAL_ALL_NODES_SCAN);
        if( pPhysical ) {
          /* Preserve label for filtering even when not using index */
          if( pLogical->zLabel ) {
            pPhysical->zLabel = sqlite3_mprintf("%s", pLogical->zLabel);
          }
          pPhysical->rCost = pLogical->rEstimatedCost;
        }
      }
      break;
      
    case LOGICAL_INDEX_SCAN:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_PROPERTY_INDEX_SCAN);
      if( pPhysical && pLogical->zProperty ) {
        pPhysical->zProperty = sqlite3_mprintf("%s", pLogical->zProperty);
        pPhysical->zValue = sqlite3_mprintf("%s", pLogical->zValue ? pLogical->zValue : "");
        pPhysical->rCost = pLogical->rEstimatedCost * 0.1; /* Index is much faster */
      }
      break;
      
    case LOGICAL_FILTER:
    case LOGICAL_PROPERTY_FILTER:
    case LOGICAL_LABEL_FILTER:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_FILTER);
      if( pPhysical ) {
        if( pLogical->zProperty ) {
          pPhysical->zProperty = sqlite3_mprintf("%s", pLogical->zProperty);
        }
        if( pLogical->zValue ) {
          pPhysical->zValue = sqlite3_mprintf("%s", pLogical->zValue);
        }
        /* Copy operator from iFlags */
        pPhysical->iFlags = pLogical->iFlags;
        /* Copy filter AST from logical plan's pExtra to physical plan's pExecState */
        if( pLogical->pExtra ) {
          pPhysical->pExecState = pLogical->pExtra;
        }
        pPhysical->rSelectivity = 0.1; /* Assume 10% selectivity */
      }
      break;
      
    case LOGICAL_EXPAND:
      /* Relationship expansion/traversal */
      pPhysical = physicalPlanNodeCreate(PHYSICAL_EXPAND);
      break;

    case LOGICAL_HASH_JOIN:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_HASH_JOIN);
      break;

    case LOGICAL_NESTED_LOOP_JOIN:
      /* Choose between nested loop and index nested loop */
      if( pContext && pContext->bUseIndexes ) {
        pPhysical = physicalPlanNodeCreate(PHYSICAL_INDEX_NESTED_LOOP);
      } else {
        pPhysical = physicalPlanNodeCreate(PHYSICAL_NESTED_LOOP_JOIN);
      }
      break;
      
    case LOGICAL_PROJECTION:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_PROJECTION);
      if( pPhysical && pLogical->zProperty ) {
        pPhysical->zProperty = sqlite3_mprintf("%s", pLogical->zProperty);
      }
      break;
      
    case LOGICAL_SORT:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_SORT);
      break;

    case LOGICAL_SKIP:
      /* SKIP is implemented as LIMIT with offset */
      /* For now, map to LIMIT - full implementation would need separate SKIP physical operator */
      pPhysical = physicalPlanNodeCreate(PHYSICAL_LIMIT);
      if( pPhysical && pLogical->zValue ) {
        /* Mark this as a SKIP by storing negative limit? Or use flags */
        pPhysical->iFlags |= 0x01; /* SKIP flag */
      }
      break;

    case LOGICAL_LIMIT:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_LIMIT);
      break;

    case LOGICAL_AGGREGATION:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_AGGREGATION);
      break;

    case LOGICAL_CREATE:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_CREATE);
      break;

    case LOGICAL_MERGE:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_MERGE);
      break;

    case LOGICAL_SET:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_SET);
      break;

    case LOGICAL_DELETE:
    case LOGICAL_DETACH_DELETE:
      pPhysical = physicalPlanNodeCreate(PHYSICAL_DELETE);
      break;

    default:
      /* Default to filter for unknown operations */
      pPhysical = physicalPlanNodeCreate(PHYSICAL_FILTER);
      break;
  }
  
  if( !pPhysical ) return NULL;
  
  /* Copy common properties */
  if( pLogical->zAlias ) {
    pPhysical->zAlias = sqlite3_mprintf("%s", pLogical->zAlias);
  }
  if( pLogical->zLabel ) {
    pPhysical->zLabel = sqlite3_mprintf("%s", pLogical->zLabel);
  }
  if( pLogical->zProperty ) {
    pPhysical->zProperty = sqlite3_mprintf("%s", pLogical->zProperty);
  }
  if( pLogical->zValue ) {
    pPhysical->zValue = sqlite3_mprintf("%s", pLogical->zValue);
  }

  /* Set cost and row estimates */
  pPhysical->rCost = pLogical->rEstimatedCost;
  pPhysical->iRows = pLogical->iEstimatedRows;

  /* Copy extra data (e.g., pattern AST for CREATE) */
  pPhysical->pExecState = pLogical->pExtra;
  pPhysical->iFlags = pLogical->iFlags;

  /* Convert children recursively */
  for( i = 0; i < pLogical->nChildren; i++ ) {
    pChild = logicalPlanToPhysical(pLogical->apChildren[i], pContext);
    if( pChild ) {
      rc = physicalPlanNodeAddChild(pPhysical, pChild);
      if( rc != SQLITE_OK ) {
        physicalPlanNodeDestroy(pChild);
        physicalPlanNodeDestroy(pPhysical);
        return NULL;
      }
    }
  }
  
  return pPhysical;
}

/*
** Generate string representation of physical plan tree.
** Caller must sqlite3_free() the returned string.
*/
char *physicalPlanToString(PhysicalPlanNode *pNode) {
  char *zResult;
  char *zChildren = NULL;
  char *zDetails = NULL;
  int i;
  
  if( !pNode ) return sqlite3_mprintf("(null)");
  
  /* Build children string */
  if( pNode->nChildren > 0 ) {
    char *zChild;
    
    for( i = 0; i < pNode->nChildren; i++ ) {
      zChild = physicalPlanToString(pNode->apChildren[i]);
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
  
  /* Build details string */
  if( pNode->zIndexName ) {
    zDetails = sqlite3_mprintf("index=%s", pNode->zIndexName);
  } else if( pNode->zLabel ) {
    zDetails = sqlite3_mprintf("label=%s", pNode->zLabel);
  } else if( pNode->zProperty ) {
    if( pNode->zValue ) {
      zDetails = sqlite3_mprintf("prop=%s val=%s", pNode->zProperty, pNode->zValue);
    } else {
      zDetails = sqlite3_mprintf("prop=%s", pNode->zProperty);
    }
  }
  
  /* Build node string */
  if( pNode->zAlias && zDetails ) {
    zResult = sqlite3_mprintf("%s(%s %s cost=%.1f rows=%lld%s%s)",
                             physicalOperatorTypeName(pNode->type),
                             pNode->zAlias,
                             zDetails,
                             pNode->rCost,
                             pNode->iRows,
                             zChildren ? " [" : "",
                             zChildren ? zChildren : "");
  } else if( pNode->zAlias ) {
    zResult = sqlite3_mprintf("%s(%s cost=%.1f rows=%lld%s%s)",
                             physicalOperatorTypeName(pNode->type),
                             pNode->zAlias,
                             pNode->rCost,
                             pNode->iRows,
                             zChildren ? " [" : "",
                             zChildren ? zChildren : "");
  } else if( zDetails ) {
    zResult = sqlite3_mprintf("%s(%s cost=%.1f rows=%lld%s%s)",
                             physicalOperatorTypeName(pNode->type),
                             zDetails,
                             pNode->rCost,
                             pNode->iRows,
                             zChildren ? " [" : "",
                             zChildren ? zChildren : "");
  } else {
    zResult = sqlite3_mprintf("%s(cost=%.1f rows=%lld%s%s)",
                             physicalOperatorTypeName(pNode->type),
                             pNode->rCost,
                             pNode->iRows,
                             zChildren ? " [" : "",
                             zChildren ? zChildren : "");
  }
  
  if( zChildren && zResult ) {
    char *zFinal = sqlite3_mprintf("%s]", zResult);
    sqlite3_free(zResult);
    zResult = zFinal;
  }
  
  sqlite3_free(zChildren);
  sqlite3_free(zDetails);
  return zResult;
}

/*
** Create a simple test physical plan for demonstration.
** Shows optimized physical operators with index usage.
*/
PhysicalPlanNode *cypherCreateTestPhysicalPlan(void) {
  PhysicalPlanNode *pProjection, *pFilter, *pScan;
  int rc;
  
  /* Create physical nodes with optimized operators */
  pProjection = physicalPlanNodeCreate(PHYSICAL_PROJECTION);
  pFilter = physicalPlanNodeCreate(PHYSICAL_FILTER);
  pScan = physicalPlanNodeCreate(PHYSICAL_LABEL_INDEX_SCAN);
  
  if( !pProjection || !pFilter || !pScan ) {
    physicalPlanNodeDestroy(pProjection);
    physicalPlanNodeDestroy(pFilter);
    physicalPlanNodeDestroy(pScan);
    return NULL;
  }
  
  /* Set operator-specific properties */
  pScan->zAlias = sqlite3_mprintf("n");
  pScan->zLabel = sqlite3_mprintf("Person");
  pScan->rCost = 10.0;
  pScan->iRows = 1000;
  
  pFilter->zAlias = sqlite3_mprintf("n");
  pFilter->zProperty = sqlite3_mprintf("age");
  pFilter->zValue = sqlite3_mprintf("30");
  pFilter->rCost = 1.0;
  pFilter->iRows = 100;
  pFilter->rSelectivity = 0.1;
  
  pProjection->zAlias = sqlite3_mprintf("n");
  pProjection->zProperty = sqlite3_mprintf("name");
  pProjection->rCost = 0.1;
  pProjection->iRows = 100;
  
  /* Build tree structure */
  rc = physicalPlanNodeAddChild(pFilter, pScan);
  if( rc == SQLITE_OK ) {
    rc = physicalPlanNodeAddChild(pProjection, pFilter);
  }
  
  if( rc != SQLITE_OK ) {
    physicalPlanNodeDestroy(pProjection);
    return NULL;
  }
  
  return pProjection;
}