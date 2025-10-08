/*
** SQLite Graph Database Extension - Iterator Implementations
**
** This file implements the core iterators for the Cypher execution engine
** using the Volcano iterator model. Each iterator implements the standard
** open/next/close interface for streaming query execution.
**
** Features:
** - AllNodesScan iterator for full table scans
** - LabelIndexScan iterator for label-based filtering
** - PropertyIndexScan iterator for property-based filtering
** - Filter iterator for predicate evaluation
** - Projection iterator for column selection
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes
*/

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-executor.h"
#include "cypher-expressions.h"
#include <string.h>
#include <assert.h>



/*
** Base iterator functions.
*/

/*
** Create an iterator from a physical plan node.
** Returns NULL on allocation failure or unsupported operator.
*/
CypherIterator *cypherIteratorCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  if( !pPlan || !pContext ) return NULL;
  
  switch( pPlan->type ) {
    case PHYSICAL_ALL_NODES_SCAN:
      return cypherAllNodesScanCreate(pPlan, pContext);
      
    case PHYSICAL_LABEL_INDEX_SCAN:
      return cypherLabelIndexScanCreate(pPlan, pContext);
      
    case PHYSICAL_PROPERTY_INDEX_SCAN:
      return cypherPropertyIndexScanCreate(pPlan, pContext);
      
    case PHYSICAL_FILTER:
      return cypherFilterCreate(pPlan, pContext);
      
    case PHYSICAL_PROJECTION:
      return cypherProjectionCreate(pPlan, pContext);
      
    case PHYSICAL_SORT:
      return cypherSortCreate(pPlan, pContext);
      
    case PHYSICAL_LIMIT:
      return cypherLimitCreate(pPlan, pContext);
      
    default:
      /* Unsupported operator type */
      return NULL;
  }
}

/*
** Destroy an iterator and free all associated memory.
** Safe to call with NULL pointer.
*/
void cypherIteratorDestroy(CypherIterator *pIterator) {
  int i;
  
  if( !pIterator ) return;
  
  /* Close iterator if still open */
  if( pIterator->bOpened ) {
    pIterator->xClose(pIterator);
  }
  
  /* Destroy child iterators */
  for( i = 0; i < pIterator->nChildren; i++ ) {
    cypherIteratorDestroy(pIterator->apChildren[i]);
  }
  sqlite3_free(pIterator->apChildren);
  
  /* Call iterator-specific destroy function */
  if( pIterator->xDestroy ) {
    pIterator->xDestroy(pIterator);
  }
  
  sqlite3_free(pIterator);
}

/*
** AllNodesScan iterator implementation.
** Scans all nodes in the graph sequentially.
*/

typedef struct AllNodesScanData {
  sqlite3_stmt *pStmt;          /* SQL statement for node iteration */
} AllNodesScanData;

static int allNodesScanOpen(CypherIterator *pIterator) {
  AllNodesScanData *pData = (AllNodesScanData*)pIterator->pIterData;
  GraphVtab *pGraph = pIterator->pContext->pGraph;
  char *zSql;
  int rc;
  
  if( !pGraph ) return SQLITE_ERROR;
  
  zSql = sqlite3_mprintf("SELECT id FROM %s_nodes", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pData->pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ) return rc;

  pIterator->bOpened = 1;
  pIterator->bEof = 0;
  
  return SQLITE_OK;
}

static int allNodesScanNext(CypherIterator *pIterator, CypherResult *pResult) {
  AllNodesScanData *pData = (AllNodesScanData*)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  CypherValue nodeValue;
  int rc;
  
  if( pIterator->bEof ) return SQLITE_DONE;
  
  rc = sqlite3_step(pData->pStmt);
  if( rc!=SQLITE_ROW ){
    pIterator->bEof = 1;
    return SQLITE_DONE;
  }
  
  memset(&nodeValue, 0, sizeof(nodeValue));
  nodeValue.type = CYPHER_VALUE_NODE;
  nodeValue.u.iNodeId = sqlite3_column_int64(pData->pStmt, 0);
  
  rc = cypherResultAddColumn(pResult, pPlan->zAlias ? pPlan->zAlias : "node", &nodeValue);
  if( rc != SQLITE_OK ) return rc;
  
  pIterator->nRowsProduced++;
  
  return SQLITE_OK;
}

static int allNodesScanClose(CypherIterator *pIterator) {
  AllNodesScanData *pData = (AllNodesScanData*)pIterator->pIterData;
  sqlite3_finalize(pData->pStmt);
  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void allNodesScanDestroy(CypherIterator *pIterator) {
  sqlite3_free(pIterator->pIterData);
}

CypherIterator *cypherAllNodesScanCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  AllNodesScanData *pData;
  
  pIterator = sqlite3_malloc(sizeof(CypherIterator));
  if( !pIterator ) return NULL;
  
  pData = sqlite3_malloc(sizeof(AllNodesScanData));
  if( !pData ) {
    sqlite3_free(pIterator);
    return NULL;
  }
  
  memset(pIterator, 0, sizeof(CypherIterator));
  memset(pData, 0, sizeof(AllNodesScanData));
  
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

typedef struct LabelIndexScanData {
  sqlite3_stmt *pStmt;          /* SQL statement for node iteration */
  const char *zLabel;           /* Label to filter by */
} LabelIndexScanData;

static int labelIndexScanOpen(CypherIterator *pIterator) {
  LabelIndexScanData *pData = (LabelIndexScanData*)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  GraphVtab *pGraph = pIterator->pContext->pGraph;
  char *zSql;
  int rc;
  
  if( !pGraph || !pPlan->zLabel ) return SQLITE_ERROR;
  
  pData->zLabel = pPlan->zLabel;
  
  zSql = sqlite3_mprintf("SELECT id FROM %s_nodes WHERE labels LIKE '%%%%\"%s\"%%%%'", pGraph->zTableName, pData->zLabel);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pData->pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ) return rc;

  pIterator->bOpened = 1;
  pIterator->bEof = 0;
  
  return SQLITE_OK;
}

static int labelIndexScanNext(CypherIterator *pIterator, CypherResult *pResult) {
  LabelIndexScanData *pData = (LabelIndexScanData*)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  CypherValue nodeValue;
  int rc;
  
  if( pIterator->bEof ) return SQLITE_DONE;
  
  rc = sqlite3_step(pData->pStmt);
  if( rc!=SQLITE_ROW ){
    pIterator->bEof = 1;
    return SQLITE_DONE;
  }
  
  memset(&nodeValue, 0, sizeof(nodeValue));
  nodeValue.type = CYPHER_VALUE_NODE;
  nodeValue.u.iNodeId = sqlite3_column_int64(pData->pStmt, 0);
  
  rc = cypherResultAddColumn(pResult, pPlan->zAlias ? pPlan->zAlias : "node", &nodeValue);
  if( rc != SQLITE_OK ) return rc;
  
  pIterator->nRowsProduced++;
  
  return SQLITE_OK;
}

static int labelIndexScanClose(CypherIterator *pIterator) {
  LabelIndexScanData *pData = (LabelIndexScanData*)pIterator->pIterData;
  sqlite3_finalize(pData->pStmt);
  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void labelIndexScanDestroy(CypherIterator *pIterator) {
  sqlite3_free(pIterator->pIterData);
}

CypherIterator *cypherLabelIndexScanCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  LabelIndexScanData *pData;
  
  pIterator = sqlite3_malloc(sizeof(CypherIterator));
  if( !pIterator ) return NULL;
  
  pData = sqlite3_malloc(sizeof(LabelIndexScanData));
  if( !pData ) {
    sqlite3_free(pIterator);
    return NULL;
  }
  
  memset(pIterator, 0, sizeof(CypherIterator));
  memset(pData, 0, sizeof(LabelIndexScanData));
  
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
** Scans nodes/relationships with specific property values using property indexes.
*/

typedef struct PropertyIndexScanData {
  const char *zProperty;        /* Property to filter by */
  const char *zValue;           /* Value to match */
  sqlite3_stmt *pStmt;          /* SQL statement for property lookup */
} PropertyIndexScanData;

static int propertyIndexScanOpen(CypherIterator *pIterator) {
  PropertyIndexScanData *pData = (PropertyIndexScanData*)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  GraphVtab *pGraph = pIterator->pContext->pGraph;
  
  if( !pGraph || !pPlan->zProperty ) return SQLITE_ERROR;
  
  pData->zProperty = pPlan->zProperty;
  pData->zValue = pPlan->zValue;
  
  char *zSql = sqlite3_mprintf(
    "SELECT id FROM %s_nodes WHERE "
    "json_extract(properties, '$.%s') = %Q",
    pGraph->zTableName, pData->zProperty, pData->zValue);
  
  if (!zSql) {
    return SQLITE_NOMEM;
  }
  
  int rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pData->pStmt, NULL);
  sqlite3_free(zSql);
  
  if (rc != SQLITE_OK) {
    return rc;
  }
  
  pIterator->bOpened = 1;
  pIterator->bEof = 0;
  
  return SQLITE_OK;
}

static int propertyIndexScanNext(CypherIterator *pIterator, CypherResult *pResult) {
  PropertyIndexScanData *pData = (PropertyIndexScanData*)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  CypherValue nodeValue;
  int rc;
  
  if( pIterator->bEof ) return SQLITE_DONE;
  
  rc = sqlite3_step(pData->pStmt);
  if( rc!=SQLITE_ROW ){
    pIterator->bEof = 1;
    return SQLITE_DONE;
  }
  
  memset(&nodeValue, 0, sizeof(nodeValue));
  nodeValue.type = CYPHER_VALUE_NODE;
  nodeValue.u.iNodeId = sqlite3_column_int64(pData->pStmt, 0);
  
  rc = cypherResultAddColumn(pResult, pPlan->zAlias ? pPlan->zAlias : "node", &nodeValue);
  if( rc != SQLITE_OK ) return rc;
  
  pIterator->nRowsProduced++;
  
  return SQLITE_OK;
}

static int propertyIndexScanClose(CypherIterator *pIterator) {
  PropertyIndexScanData *pData = (PropertyIndexScanData*)pIterator->pIterData;
  sqlite3_finalize(pData->pStmt);
  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void propertyIndexScanDestroy(CypherIterator *pIterator) {
  sqlite3_free(pIterator->pIterData);
}

CypherIterator *cypherPropertyIndexScanCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  PropertyIndexScanData *pData;
  
  pIterator = sqlite3_malloc(sizeof(CypherIterator));
  if( !pIterator ) return NULL;
  
  pData = sqlite3_malloc(sizeof(PropertyIndexScanData));
  if( !pData ) {
    sqlite3_free(pIterator);
    return NULL;
  }
  
  memset(pIterator, 0, sizeof(CypherIterator));
  memset(pData, 0, sizeof(PropertyIndexScanData));
  
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
typedef struct FilterIteratorData {
  CypherIterator *pSource;     /* Source iterator */
  CypherExpression *pFilter;   /* Filter expression */
} FilterIteratorData;

static int filterIteratorOpen(CypherIterator *pIterator) {
  FilterIteratorData *pData = (FilterIteratorData*)pIterator->pIterData;
  return pData->pSource->xOpen(pData->pSource);
}

static int filterIteratorNext(CypherIterator *pIterator, CypherResult *pResult) {
  FilterIteratorData *pData = (FilterIteratorData*)pIterator->pIterData;
  CypherValue filterResult;
  int rc;
  
  /* Keep fetching from source until we find a matching row */
  while ((rc = pData->pSource->xNext(pData->pSource, pResult)) == SQLITE_OK) {
    /* Evaluate filter expression */
    rc = cypherExpressionEvaluate(pData->pFilter, pIterator->pContext, &filterResult);
    if (rc != SQLITE_OK) return rc;
    
    /* Check if filter passes (truthy value) */
    if (!cypherValueIsNull(&filterResult) && 
        (filterResult.type != CYPHER_VALUE_BOOLEAN || filterResult.u.bBoolean)) {
      cypherValueDestroy(&filterResult);
      return SQLITE_OK;
    }
    
    cypherValueDestroy(&filterResult);
  }
  
  return rc;
}

static int filterIteratorClose(CypherIterator *pIterator) {
  FilterIteratorData *pData = (FilterIteratorData*)pIterator->pIterData;
  return pData->pSource->xClose(pData->pSource);
}

static void filterIteratorDestroy(CypherIterator *pIterator) {
  if (pIterator) {
    FilterIteratorData *pData = (FilterIteratorData*)pIterator->pIterData;
    if (pData) {
      if (pData->pSource) {
        pData->pSource->xDestroy(pData->pSource);
      }
      sqlite3_free(pData);
    }
    sqlite3_free(pIterator);
  }
}

CypherIterator *cypherFilterCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  FilterIteratorData *pData;
  
  if (!pPlan || !pPlan->pChild || !pPlan->pFilterExpr) return NULL;
  
  pIterator = sqlite3_malloc(sizeof(CypherIterator));
  if (!pIterator) return NULL;
  
  pData = sqlite3_malloc(sizeof(FilterIteratorData));
  if (!pData) {
    sqlite3_free(pIterator);
    return NULL;
  }
  
  memset(pIterator, 0, sizeof(CypherIterator));
  memset(pData, 0, sizeof(FilterIteratorData));
  
  /* Create source iterator */
  pData->pSource = cypherIteratorCreate(pPlan->pChild, pContext);
  if (!pData->pSource) {
    sqlite3_free(pData);
    sqlite3_free(pIterator);
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

/* Projection iterator implementation */
typedef struct ProjectionIteratorData {
  CypherIterator *pSource;          /* Source iterator */
  CypherExpression **apProjections; /* Projection expressions */
  int nProjections;                 /* Number of projections */
} ProjectionIteratorData;

static int projectionIteratorOpen(CypherIterator *pIterator) {
  ProjectionIteratorData *pData = (ProjectionIteratorData*)pIterator->pIterData;
  return pData->pSource->xOpen(pData->pSource);
}

static int projectionIteratorNext(CypherIterator *pIterator, CypherResult *pResult) {
  ProjectionIteratorData *pData = (ProjectionIteratorData*)pIterator->pIterData;
  CypherResult sourceResult;
  int rc, i;
  
  /* Get next row from source */
  rc = pData->pSource->xNext(pData->pSource, &sourceResult);
  if (rc != SQLITE_OK) return rc;
  
  /* Create new result with projections */
  memset(pResult, 0, sizeof(CypherResult));
  
  for (i = 0; i < pData->nProjections; i++) {
    CypherValue projValue;
    
    /* Evaluate projection expression */
    rc = cypherExpressionEvaluate(pData->apProjections[i], pIterator->pContext, &projValue);
    if (rc != SQLITE_OK) {
      cypherResultDestroy(&sourceResult);
      return rc;
    }
    
    /* Add to result */
    char *zColName = sqlite3_mprintf("col%d", i);
    if (!zColName) {
      cypherValueDestroy(&projValue);
      cypherResultDestroy(&sourceResult);
      return SQLITE_NOMEM;
    }
    
    rc = cypherResultAddColumn(pResult, zColName, &projValue);
    sqlite3_free(zColName);
    cypherValueDestroy(&projValue);
    
    if (rc != SQLITE_OK) {
      cypherResultDestroy(&sourceResult);
      return rc;
    }
  }
  
  cypherResultDestroy(&sourceResult);
  return SQLITE_OK;
}

static int projectionIteratorClose(CypherIterator *pIterator) {
  ProjectionIteratorData *pData = (ProjectionIteratorData*)pIterator->pIterData;
  return pData->pSource->xClose(pData->pSource);
}

static void projectionIteratorDestroy(CypherIterator *pIterator) {
  if (pIterator) {
    ProjectionIteratorData *pData = (ProjectionIteratorData*)pIterator->pIterData;
    if (pData) {
      if (pData->pSource) {
        pData->pSource->xDestroy(pData->pSource);
      }
      sqlite3_free(pData);
    }
    sqlite3_free(pIterator);
  }
}

CypherIterator *cypherProjectionCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  ProjectionIteratorData *pData;
  
  if (!pPlan || !pPlan->pChild || !pPlan->apProjections || pPlan->nProjections <= 0) return NULL;
  
  pIterator = sqlite3_malloc(sizeof(CypherIterator));
  if (!pIterator) return NULL;
  
  pData = sqlite3_malloc(sizeof(ProjectionIteratorData));
  if (!pData) {
    sqlite3_free(pIterator);
    return NULL;
  }
  
  memset(pIterator, 0, sizeof(CypherIterator));
  memset(pData, 0, sizeof(ProjectionIteratorData));
  
  /* Create source iterator */
  pData->pSource = cypherIteratorCreate(pPlan->pChild, pContext);
  if (!pData->pSource) {
    sqlite3_free(pData);
    sqlite3_free(pIterator);
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
typedef struct SortIteratorData {
  CypherIterator *pSource;     /* Source iterator */
  CypherResult *aResults;      /* Array of results to sort */
  int nResults;                /* Number of results */
  int nAllocated;              /* Allocated size */
  int iCurrent;                /* Current position */
  CypherExpression **apSortKeys; /* Sort key expressions */
  int nSortKeys;               /* Number of sort keys */
} SortIteratorData;

static int sortIteratorOpen(CypherIterator *pIterator) {
  SortIteratorData *pData = (SortIteratorData*)pIterator->pIterData;
  CypherResult result;
  int rc;
  
  /* Open source iterator */
  rc = pData->pSource->xOpen(pData->pSource);
  if (rc != SQLITE_OK) return rc;
  
  /* Collect all results */
  pData->nResults = 0;
  pData->iCurrent = 0;
  
  while ((rc = pData->pSource->xNext(pData->pSource, &result)) == SQLITE_OK) {
    /* Resize array if needed */
    if (pData->nResults >= pData->nAllocated) {
      int nNew = pData->nAllocated ? pData->nAllocated * 2 : 16;
      CypherResult *aNew = sqlite3_realloc(pData->aResults, nNew * sizeof(CypherResult));
      if (!aNew) {
        cypherResultDestroy(&result);
        return SQLITE_NOMEM;
      }
      pData->aResults = aNew;
      pData->nAllocated = nNew;
    }
    
    /* Copy result */
    pData->aResults[pData->nResults++] = result;
  }
  
  /* Sort results - simple bubble sort for now */
  for (int i = 0; i < pData->nResults - 1; i++) {
    for (int j = 0; j < pData->nResults - i - 1; j++) {
      /* Compare using first sort key only for simplicity */
      if (pData->nSortKeys > 0) {
        CypherValue v1, v2;
        cypherExpressionEvaluate(pData->apSortKeys[0], pIterator->pContext, &v1);
        cypherExpressionEvaluate(pData->apSortKeys[0], pIterator->pContext, &v2);
        
        if (cypherValueCompare(&v1, &v2) > 0) {
          /* Swap */
          CypherResult temp = pData->aResults[j];
          pData->aResults[j] = pData->aResults[j + 1];
          pData->aResults[j + 1] = temp;
        }
        
        cypherValueDestroy(&v1);
        cypherValueDestroy(&v2);
      }
    }
  }
  
  return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

static int sortIteratorNext(CypherIterator *pIterator, CypherResult *pResult) {
  SortIteratorData *pData = (SortIteratorData*)pIterator->pIterData;
  
  if (pData->iCurrent >= pData->nResults) {
    return SQLITE_DONE;
  }
  
  /* Return copy of current result */
  *pResult = pData->aResults[pData->iCurrent++];
  return SQLITE_OK;
}

static int sortIteratorClose(CypherIterator *pIterator) {
  SortIteratorData *pData = (SortIteratorData*)pIterator->pIterData;
  
  /* Free sorted results */
  for (int i = 0; i < pData->nResults; i++) {
    cypherResultDestroy(&pData->aResults[i]);
  }
  
  return pData->pSource->xClose(pData->pSource);
}

static void sortIteratorDestroy(CypherIterator *pIterator) {
  if (pIterator) {
    SortIteratorData *pData = (SortIteratorData*)pIterator->pIterData;
    if (pData) {
      if (pData->pSource) {
        pData->pSource->xDestroy(pData->pSource);
      }
      sqlite3_free(pData->aResults);
      sqlite3_free(pData);
    }
    sqlite3_free(pIterator);
  }
}

CypherIterator *cypherSortCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  SortIteratorData *pData;
  
  if (!pPlan || !pPlan->pChild) return NULL;
  
  pIterator = sqlite3_malloc(sizeof(CypherIterator));
  if (!pIterator) return NULL;
  
  pData = sqlite3_malloc(sizeof(SortIteratorData));
  if (!pData) {
    sqlite3_free(pIterator);
    return NULL;
  }
  
  memset(pIterator, 0, sizeof(CypherIterator));
  memset(pData, 0, sizeof(SortIteratorData));
  
  /* Create source iterator */
  pData->pSource = cypherIteratorCreate(pPlan->pChild, pContext);
  if (!pData->pSource) {
    sqlite3_free(pData);
    sqlite3_free(pIterator);
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
typedef struct LimitIteratorData {
  CypherIterator *pSource;  /* Source iterator */
  int nLimit;               /* Limit count */
  int nReturned;            /* Number returned so far */
} LimitIteratorData;

static int limitIteratorOpen(CypherIterator *pIterator) {
  LimitIteratorData *pData = (LimitIteratorData*)pIterator->pIterData;
  pData->nReturned = 0;
  return pData->pSource->xOpen(pData->pSource);
}

static int limitIteratorNext(CypherIterator *pIterator, CypherResult *pResult) {
  LimitIteratorData *pData = (LimitIteratorData*)pIterator->pIterData;
  
  /* Check if limit reached */
  if (pData->nReturned >= pData->nLimit) {
    return SQLITE_DONE;
  }
  
  /* Get next from source */
  int rc = pData->pSource->xNext(pData->pSource, pResult);
  if (rc == SQLITE_OK) {
    pData->nReturned++;
  }
  
  return rc;
}

static int limitIteratorClose(CypherIterator *pIterator) {
  LimitIteratorData *pData = (LimitIteratorData*)pIterator->pIterData;
  return pData->pSource->xClose(pData->pSource);
}

static void limitIteratorDestroy(CypherIterator *pIterator) {
  if (pIterator) {
    LimitIteratorData *pData = (LimitIteratorData*)pIterator->pIterData;
    if (pData) {
      if (pData->pSource) {
        pData->pSource->xDestroy(pData->pSource);
      }
      sqlite3_free(pData);
    }
    sqlite3_free(pIterator);
  }
}

CypherIterator *cypherLimitCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  LimitIteratorData *pData;
  
  if (!pPlan || !pPlan->pChild || pPlan->nLimit <= 0) return NULL;
  
  pIterator = sqlite3_malloc(sizeof(CypherIterator));
  if (!pIterator) return NULL;
  
  pData = sqlite3_malloc(sizeof(LimitIteratorData));
  if (!pData) {
    sqlite3_free(pIterator);
    return NULL;
  }
  
  memset(pIterator, 0, sizeof(CypherIterator));
  memset(pData, 0, sizeof(LimitIteratorData));
  
  /* Create source iterator */
  pData->pSource = cypherIteratorCreate(pPlan->pChild, pContext);
  if (!pData->pSource) {
    sqlite3_free(pData);
    sqlite3_free(pIterator);
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