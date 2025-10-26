/* Iterator implementations using Volcano model (open/next/close) */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-executor.h"
#include "cypher-expressions.h"
#include "cypher-write.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Forward declarations for write operation wrapper iterators */
static CypherIterator *createWriteIteratorWrapper(PhysicalPlanNode *pPlan, ExecutionContext *pContext);
static CypherIterator *mergeWriteIteratorWrapper(PhysicalPlanNode *pPlan, ExecutionContext *pContext);
static CypherIterator *setWriteIteratorWrapper(PhysicalPlanNode *pPlan, ExecutionContext *pContext);
static CypherIterator *deleteWriteIteratorWrapper(PhysicalPlanNode *pPlan, ExecutionContext *pContext);
static CypherIterator *cypherExpandCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext);

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

    case PHYSICAL_CREATE:
      return createWriteIteratorWrapper(pPlan, pContext);

    case PHYSICAL_MERGE:
      return mergeWriteIteratorWrapper(pPlan, pContext);

    case PHYSICAL_SET:
      return setWriteIteratorWrapper(pPlan, pContext);

    case PHYSICAL_DELETE:
      return deleteWriteIteratorWrapper(pPlan, pContext);

    case PHYSICAL_EXPAND:
      return cypherExpandCreate(pPlan, pContext);

    default:
      /* Unsupported operator type */
      return NULL;
  }
}

void cypherIteratorDestroy(CypherIterator *pIterator) {
  int i;

  if( !pIterator ) return;

  /* Safety check for invalid iterator */
  if( pIterator->xOpen == NULL && pIterator->xNext == NULL &&
      pIterator->xClose == NULL && pIterator->xDestroy == NULL ) {
    /* Invalid iterator with all NULL function pointers - skip */
    return;
  }

  /* Close iterator if still open */
  if( pIterator->bOpened && pIterator->xClose ) {
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

  /* Reset result for new row */
  pResult->nColumns = 0;

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

  /* Use %q to properly escape the label for SQL LIKE pattern */
  zSql = sqlite3_mprintf("SELECT id FROM %s_nodes WHERE labels LIKE '%%%%\"%q\"%%%%'", pGraph->zTableName, pData->zLabel);
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

  /* Reset result for new row */
  pResult->nColumns = 0;

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

/* Helper to get a property value from a node */
static char *cypherValueGetProperty(CypherValue *pValue, const char *zProperty, ExecutionContext *pContext) {
  if (!pValue || pValue->type != CYPHER_VALUE_NODE || !zProperty || !pContext) {
    return NULL;
  }

  /* Query the properties column from the nodes table */
  sqlite3_stmt *stmt;
  char *zSql = sqlite3_mprintf(
    "SELECT json_extract(properties, '$.%q') FROM %s WHERE id = %lld",
    zProperty,
    pContext->pGraph->zNodeTableName ? pContext->pGraph->zNodeTableName : "graph_nodes",
    pValue->u.iNodeId
  );

  if (!zSql) return NULL;

  int rc = sqlite3_prepare_v2(pContext->pDb, zSql, -1, &stmt, NULL);
  sqlite3_free(zSql);

  if (rc != SQLITE_OK) return NULL;

  char *zResult = NULL;
  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *zValue = (const char*)sqlite3_column_text(stmt, 0);
    if (zValue) {
      zResult = sqlite3_mprintf("%s", zValue);
    }
  }

  sqlite3_finalize(stmt);
  return zResult;
}

static int filterIteratorOpen(CypherIterator *pIterator) {
  FilterIteratorData *pData = (FilterIteratorData*)pIterator->pIterData;
  return pData->pSource->xOpen(pData->pSource);
}

static int filterIteratorNext(CypherIterator *pIterator, CypherResult *pResult) {
  FilterIteratorData *pData = (FilterIteratorData*)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  CypherValue filterResult;
  int rc;

  /* Keep fetching from source until we find a matching row */
  while ((rc = pData->pSource->xNext(pData->pSource, pResult)) == SQLITE_OK) {
    /* Use simple property comparison if available (faster path) */
    if (!pData->pFilter && pPlan->zProperty && pPlan->zValue) {
      /* Find the column with the variable name */
      const char *zVarName = pPlan->zAlias;
      CypherValue *pValue = NULL;

      for (int i = 0; i < pResult->nColumns; i++) {
        if (zVarName && pResult->azColumnNames[i] && strcmp(pResult->azColumnNames[i], zVarName) == 0) {
          pValue = &pResult->aValues[i];
          break;
        }
      }

      if (pValue && pValue->type == CYPHER_VALUE_NODE) {
        /* Get property from node */
        char *zPropValue = cypherValueGetProperty(pValue, pPlan->zProperty, pIterator->pContext);
        if (zPropValue) {
          int passes = 0;
          /* Compare based on operator in iFlags */
          /* 1=Equal, 2=Greater, 3=Less, 4=GreaterEqual, 5=LessEqual, 6=NotEqual */
          int cmp = strcmp(zPropValue, pPlan->zValue);
          if (pPlan->iFlags == 1) passes = (cmp == 0);
          else if (pPlan->iFlags == 2) passes = (atoi(zPropValue) > atoi(pPlan->zValue));
          else if (pPlan->iFlags == 3) passes = (atoi(zPropValue) < atoi(pPlan->zValue));
          else if (pPlan->iFlags == 4) passes = (atoi(zPropValue) >= atoi(pPlan->zValue));
          else if (pPlan->iFlags == 5) passes = (atoi(zPropValue) <= atoi(pPlan->zValue));
          else if (pPlan->iFlags == 6) passes = (cmp != 0);

          sqlite3_free(zPropValue);
          if (passes) {
            return SQLITE_OK;
          }
        }
      }
    } else if (pData->pFilter) {
      /* Evaluate full filter expression */
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
        cypherIteratorDestroy(pData->pSource);
      }
      sqlite3_free(pData);
    }
    /* Don't free pIterator - cypherIteratorDestroy will do that */
  }
}

CypherIterator *cypherFilterCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  FilterIteratorData *pData;

  /* Filter requires either pFilterExpr/pExecState OR (zProperty + zValue) for simple filters */
  if (!pPlan || !pPlan->pChild) return NULL;
  if (!pPlan->pFilterExpr && !pPlan->pExecState && (!pPlan->zProperty || !pPlan->zValue)) return NULL;

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
  int rc, i;

  /* Pass-through mode: if no projections, just pass source result directly */
  if (pData->nProjections == 0 || !pData->apProjections) {
    rc = pData->pSource->xNext(pData->pSource, pResult);
    return rc;
  }

  /* Projection mode: evaluate projection expressions */
  CypherResult sourceResult;

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
  ProjectionIteratorData *pData = (ProjectionIteratorData*)pIterator->pIterData;

  if (pData) {
    if (pData->pSource) {
      cypherIteratorDestroy(pData->pSource);
    }
    sqlite3_free(pData);
  }
}

CypherIterator *cypherProjectionCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  ProjectionIteratorData *pData;

  /* Allow projection without explicit projection list (pass-through mode) */
  if (!pPlan || !pPlan->pChild) return NULL;

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
        cypherIteratorDestroy(pData->pSource);
      }
      sqlite3_free(pData->aResults);
      sqlite3_free(pData);
    }
    /* Don't free pIterator - cypherIteratorDestroy will do that */
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
        cypherIteratorDestroy(pData->pSource);
      }
      sqlite3_free(pData);
    }
    /* Don't free pIterator - cypherIteratorDestroy will do that */
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

/*
** Expand iterator implementation
** Traverses relationships from source nodes to target nodes
*/

typedef struct ExpandIteratorData {
  CypherIterator *pSource;          /* Source iterator producing start nodes */
  sqlite3_stmt *pStmt;              /* Statement for relationship lookup */
  CypherResult *pCurrentRow;        /* Current source row being expanded */
  int bSourceExhausted;             /* True if source iterator is done */
  sqlite3_int64 iCurrentNodeId;     /* Current source node ID being expanded */
} ExpandIteratorData;

static int expandOpen(CypherIterator *pIterator) {
  ExpandIteratorData *pData = (ExpandIteratorData*)pIterator->pIterData;
  int rc;

  /* Open source iterator if exists and not already opened */
  if (pData->pSource && !pData->pSource->bOpened) {
    rc = pData->pSource->xOpen(pData->pSource);
    if (rc != SQLITE_OK) return rc;
  }

  pIterator->bOpened = 1;
  pIterator->bEof = 0;
  pData->bSourceExhausted = 0;

  return SQLITE_OK;
}

static int expandNext(CypherIterator *pIterator, CypherResult *pResult) {
  ExpandIteratorData *pData = (ExpandIteratorData*)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  GraphVtab *pGraph = pIterator->pContext->pGraph;
  int rc;

  if (!pGraph) return SQLITE_ERROR;

  /* Loop until we find a relationship or exhaust all sources */
  while (1) {
    /* If we have a current statement, try to get next relationship */
    if (pData->pStmt) {
      rc = sqlite3_step(pData->pStmt);

      if (rc == SQLITE_ROW) {
        /* Found a relationship - build result */
        memset(pResult, 0, sizeof(CypherResult));

        /* Copy all columns from source row */
        if (pData->pCurrentRow) {
          for (int i = 0; i < pData->pCurrentRow->nColumns; i++) {
            rc = cypherResultAddColumn(pResult,
                                      pData->pCurrentRow->azColumnNames[i],
                                      &pData->pCurrentRow->aValues[i]);
            if (rc != SQLITE_OK) {
              cypherResultDestroy(pResult);
              return rc;
            }
          }
        }

        /* Add target node from relationship */
        CypherValue targetValue;
        memset(&targetValue, 0, sizeof(targetValue));
        targetValue.type = CYPHER_VALUE_NODE;
        targetValue.u.iNodeId = sqlite3_column_int64(pData->pStmt, 0);

        const char *zTargetAlias = pPlan->zAlias ? pPlan->zAlias : "target";
        rc = cypherResultAddColumn(pResult, zTargetAlias, &targetValue);
        if (rc != SQLITE_OK) {
          cypherResultDestroy(pResult);
          return rc;
        }

        /* Add relationship */
        CypherValue relValue;
        memset(&relValue, 0, sizeof(relValue));
        relValue.type = CYPHER_VALUE_RELATIONSHIP;
        relValue.u.iRelId = sqlite3_column_int64(pData->pStmt, 1);

        const char *zRelAlias = pPlan->zLabel ? pPlan->zLabel : "r";
        rc = cypherResultAddColumn(pResult, zRelAlias, &relValue);
        if (rc != SQLITE_OK) {
          cypherResultDestroy(pResult);
          return rc;
        }

        return SQLITE_OK;
      } else if (rc == SQLITE_DONE) {
        /* No more relationships for current source node */
        sqlite3_finalize(pData->pStmt);
        pData->pStmt = NULL;
      } else {
        /* Error */
        return rc;
      }
    }

    /* Need to get next source node */
    if (pData->bSourceExhausted) {
      pIterator->bEof = 1;
      return SQLITE_DONE;
    }

    /* Clean up previous row */
    if (pData->pCurrentRow) {
      cypherResultDestroy(pData->pCurrentRow);  /* This frees pCurrentRow itself */
      pData->pCurrentRow = NULL;
    }

    /* Allocate new result for current row */
    pData->pCurrentRow = sqlite3_malloc(sizeof(CypherResult));
    if (!pData->pCurrentRow) return SQLITE_NOMEM;
    memset(pData->pCurrentRow, 0, sizeof(CypherResult));

    /* Get next row from source */
    rc = pData->pSource->xNext(pData->pSource, pData->pCurrentRow);

    if (rc == SQLITE_DONE) {
      pData->bSourceExhausted = 1;
      pIterator->bEof = 1;
      return SQLITE_DONE;
    } else if (rc != SQLITE_OK) {
      return rc;
    }

    /* Extract source node ID from first child's alias */
    const char *zSourceAlias = NULL;
    if (pPlan->nChildren > 0 && pPlan->apChildren[0]) {
      zSourceAlias = pPlan->apChildren[0]->zAlias;
    }
    if (!zSourceAlias) zSourceAlias = "node";

    /* Find the node value in the current row */
    CypherValue *pNodeValue = NULL;
    for (int i = 0; i < pData->pCurrentRow->nColumns; i++) {
      if (strcmp(pData->pCurrentRow->azColumnNames[i], zSourceAlias) == 0) {
        pNodeValue = &pData->pCurrentRow->aValues[i];
        break;
      }
    }

    if (!pNodeValue || pNodeValue->type != CYPHER_VALUE_NODE) {
      return SQLITE_ERROR;
    }

    pData->iCurrentNodeId = pNodeValue->u.iNodeId;

    /* Prepare query for relationships from this node */
    const char *zEdgeType = pPlan->zProperty ? pPlan->zProperty : "";
    char *zSql;

    if (zEdgeType && zEdgeType[0]) {
      zSql = sqlite3_mprintf(
        "SELECT target, id FROM %s_edges WHERE source = %lld AND edge_type = '%q'",
        pGraph->zTableName, pData->iCurrentNodeId, zEdgeType
      );
    } else {
      zSql = sqlite3_mprintf(
        "SELECT target, id FROM %s_edges WHERE source = %lld",
        pGraph->zTableName, pData->iCurrentNodeId
      );
    }

    if (!zSql) return SQLITE_NOMEM;

    rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pData->pStmt, 0);
    sqlite3_free(zSql);

    if (rc != SQLITE_OK) return rc;

    /* Loop back to try getting first relationship from this node */
  }
}

static int expandClose(CypherIterator *pIterator) {
  ExpandIteratorData *pData = (ExpandIteratorData*)pIterator->pIterData;

  if (pData->pStmt) {
    sqlite3_finalize(pData->pStmt);
    pData->pStmt = NULL;
  }

  if (pData->pSource) {
    pData->pSource->xClose(pData->pSource);
  }

  if (pData->pCurrentRow) {
    cypherResultDestroy(pData->pCurrentRow);  /* This frees pCurrentRow itself */
    pData->pCurrentRow = NULL;
  }

  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void expandDestroy(CypherIterator *pIterator) {
  ExpandIteratorData *pData = (ExpandIteratorData*)pIterator->pIterData;

  if (pData) {
    if (pData->pStmt) {
      sqlite3_finalize(pData->pStmt);
    }

    if (pData->pSource) {
      cypherIteratorDestroy(pData->pSource);
    }

    if (pData->pCurrentRow) {
      cypherResultDestroy(pData->pCurrentRow);  /* This frees pCurrentRow itself */
    }

    sqlite3_free(pData);
  }
}

static CypherIterator *cypherExpandCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  ExpandIteratorData *pData;
  CypherIterator *pSource = NULL;

  if (!pPlan || !pContext) return NULL;

  /* Create source iterator from first child */
  if (pPlan->nChildren > 0 && pPlan->apChildren[0]) {
    pSource = cypherIteratorCreate(pPlan->apChildren[0], pContext);
    if (!pSource) return NULL;
  }

  pIterator = sqlite3_malloc(sizeof(CypherIterator));
  if (!pIterator) {
    if (pSource) cypherIteratorDestroy(pSource);
    return NULL;
  }

  pData = sqlite3_malloc(sizeof(ExpandIteratorData));
  if (!pData) {
    if (pSource) cypherIteratorDestroy(pSource);
    sqlite3_free(pIterator);
    return NULL;
  }

  memset(pIterator, 0, sizeof(CypherIterator));
  memset(pData, 0, sizeof(ExpandIteratorData));

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

typedef struct WriteIteratorData {
  sqlite3_stmt *pInsertStmt;
  int bExecuted;
  sqlite3_int64 iCreatedId;
} WriteIteratorData;

static int createIteratorOpen(CypherIterator *pIterator) {
  WriteIteratorData *pData = (WriteIteratorData*)pIterator->pIterData;
  ExecutionContext *pContext = pIterator->pContext;
  int rc;
  char *zSql;
  const char *zTableName;

  if (!pContext || !pContext->pDb || !pContext->pGraph) return SQLITE_ERROR;

  /* Get the correct table name from the graph virtual table */
  zTableName = pContext->pGraph->zNodeTableName;
  if (!zTableName) zTableName = "graph_nodes";  /* Fallback */

  /* Build INSERT statement for nodes table */
  zSql = sqlite3_mprintf("INSERT INTO %s (labels, properties) VALUES (?, ?)", zTableName);
  if (!zSql) return SQLITE_NOMEM;

  /* Prepare INSERT statement */
  rc = sqlite3_prepare_v2(
    pContext->pDb,
    zSql,
    -1,
    &pData->pInsertStmt,
    NULL
  );

  sqlite3_free(zSql);

  if (rc != SQLITE_OK) {
    return rc;
  }

  pData->bExecuted = 0;
  pIterator->bOpened = 1;
  return SQLITE_OK;
}

/* Helper function to convert MAP AST to JSON string */
static char *convertMapToJson(CypherAst *pMap) {
  if (!pMap || !cypherAstIsType(pMap, CYPHER_AST_MAP)) {
    return sqlite3_mprintf("{}");
  }

  char *zJson = sqlite3_mprintf("{");
  int first = 1;

  for (int k = 0; k < pMap->nChildren; k++) {
    CypherAst *pPair = pMap->apChildren[k];
    if (!cypherAstIsType(pPair, CYPHER_AST_PROPERTY_PAIR)) continue;

    const char *zKey = cypherAstGetValue(pPair);
    if (!zKey) continue;

    const char *zValue = NULL;
    if (pPair->nChildren > 0) {
      zValue = cypherAstGetValue(pPair->apChildren[0]);
    }

    if (zValue) {
      if (!first) {
        char *zOld = zJson;
        zJson = sqlite3_mprintf("%s,", zOld);  /* Fixed: removed space after comma */
        sqlite3_free(zOld);
      }

      char *zOld = zJson;
      /* Check if value looks like a number (digit or - sign) */
      int isNumeric = (zValue[0] >= '0' && zValue[0] <= '9') || zValue[0] == '-' ||
                      strcmp(zValue, "true") == 0 || strcmp(zValue, "false") == 0 || strcmp(zValue, "null") == 0;

      if (isNumeric) {
        /* Numeric, boolean, or null - use as-is in JSON */
        zJson = sqlite3_mprintf("%s\"%s\":%s", zOld, zKey, zValue);
      } else {
        /* String value - add JSON quotes */
        zJson = sqlite3_mprintf("%s\"%s\":\"%s\"", zOld, zKey, zValue);
      }
      sqlite3_free(zOld);
      first = 0;
    }
  }

  char *zOld = zJson;
  zJson = sqlite3_mprintf("%s}", zOld);
  sqlite3_free(zOld);

  return zJson;
}

static int createIteratorNext(CypherIterator *pIterator, CypherResult *pResult) {
  WriteIteratorData *pData = (WriteIteratorData*)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  ExecutionContext *pContext = pIterator->pContext;
  CypherAst *pPattern;
  int rc;

  /* CREATE only executes once */
  if (pData->bExecuted) {
    return SQLITE_DONE;
  }

  /* Check if we have a pattern AST in pExecState (copied from logical plan's pExtra) */
  pPattern = (CypherAst*)pPlan->pExecState;

  if (pPattern && cypherAstIsType(pPattern, CYPHER_AST_PATTERN)) {
    /* Process full pattern: (a)-[r:TYPE]->(b) */
    sqlite3_int64 aiNodeIds[10];  /* Support up to 10 nodes in pattern */
    int nNodes = 0;
    int i;

    /* Count and create nodes first */
    for (i = 0; i < pPattern->nChildren; i++) {
      if (cypherAstIsType(pPattern->apChildren[i], CYPHER_AST_NODE_PATTERN)) {
        CypherAst *pNode = pPattern->apChildren[i];
        const char *zLabels = "";
        const char *zProperties = "{}";

        /* Extract labels and properties from node pattern */
        char *zJsonProps = NULL;
        for (int j = 0; j < pNode->nChildren; j++) {
          if (cypherAstIsType(pNode->apChildren[j], CYPHER_AST_LABELS)) {
            CypherAst *pLabels = pNode->apChildren[j];
            if (pLabels->zValue) zLabels = pLabels->zValue;
          } else if (cypherAstIsType(pNode->apChildren[j], CYPHER_AST_MAP)) {
            zJsonProps = convertMapToJson(pNode->apChildren[j]);
            if (zJsonProps) zProperties = zJsonProps;
          }
        }

        /* Insert node */
        sqlite3_bind_text(pData->pInsertStmt, 1, zLabels, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(pData->pInsertStmt, 2, zProperties, -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(pData->pInsertStmt);
        if (zJsonProps) sqlite3_free(zJsonProps);  /* Free allocated JSON string */
        if (rc != SQLITE_DONE) return SQLITE_ERROR;

        aiNodeIds[nNodes++] = sqlite3_last_insert_rowid(pContext->pDb);
        sqlite3_reset(pData->pInsertStmt);
      }
    }

    /* Now create relationships between nodes */
    int nodeIndex = 0;
    for (i = 0; i < pPattern->nChildren; i++) {
      if (cypherAstIsType(pPattern->apChildren[i], CYPHER_AST_REL_PATTERN)) {
        CypherAst *pRel = pPattern->apChildren[i];
        const char *zRelType = "";
        const char *zRelProps = "{}";
        sqlite3_int64 iSource, iTarget;

        /* Extract relationship type and properties */
        char *zJsonRelProps = NULL;
        for (int j = 0; j < pRel->nChildren; j++) {
          if (cypherAstIsType(pRel->apChildren[j], CYPHER_AST_LABELS)) {
            CypherAst *pLabels = pRel->apChildren[j];
            if (pLabels->zValue) zRelType = pLabels->zValue;
          } else if (cypherAstIsType(pRel->apChildren[j], CYPHER_AST_MAP)) {
            zJsonRelProps = convertMapToJson(pRel->apChildren[j]);
            if (zJsonRelProps) zRelProps = zJsonRelProps;
          }
        }

        /* Source is the node before this rel, target is after */
        if (nodeIndex >= nNodes - 1) continue;  /* Safety check */
        iSource = aiNodeIds[nodeIndex];
        iTarget = aiNodeIds[nodeIndex + 1];

        /* Insert edge */
        const char *zEdgeTable = pContext->pGraph->zEdgeTableName ?
                                  pContext->pGraph->zEdgeTableName : "graph_edges";
        char *zSql = sqlite3_mprintf(
          "INSERT INTO %s (source, target, edge_type, properties) VALUES (%lld, %lld, '%q', '%q')",
          zEdgeTable, iSource, iTarget, zRelType, zRelProps
        );

        if (zSql) {
          rc = sqlite3_exec(pContext->pDb, zSql, NULL, NULL, NULL);
          sqlite3_free(zSql);
          if (zJsonRelProps) sqlite3_free(zJsonRelProps);  /* Free allocated JSON string */
          if (rc != SQLITE_OK) return SQLITE_ERROR;
        }

        nodeIndex++;
      } else if (cypherAstIsType(pPattern->apChildren[i], CYPHER_AST_NODE_PATTERN)) {
        /* Track which node we're on for relationships */
        /* nodeIndex incremented after processing rel */
      }
    }

    /* Return first created node (for compatibility) */
    if (nNodes > 0) {
      CypherValue nodeValue;
      memset(&nodeValue, 0, sizeof(nodeValue));
      nodeValue.type = CYPHER_VALUE_NODE;
      nodeValue.u.iNodeId = aiNodeIds[0];

      rc = cypherResultAddColumn(pResult, pPlan->zAlias ? pPlan->zAlias : "node", &nodeValue);
      if (rc != SQLITE_OK) return rc;
    }

  } else {
    /* Fallback: Simple single node creation (original behavior) */
    const char *zLabels = (pPlan->zLabel && pPlan->zLabel[0]) ? pPlan->zLabel : "";
    const char *zProperties = (pPlan->zProperty && pPlan->zProperty[0]) ? pPlan->zProperty : "{}";
    CypherValue nodeValue;

    sqlite3_bind_text(pData->pInsertStmt, 1, zLabels, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(pData->pInsertStmt, 2, zProperties, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(pData->pInsertStmt);
    if (rc != SQLITE_DONE) return SQLITE_ERROR;

    pData->iCreatedId = sqlite3_last_insert_rowid(pContext->pDb);
    sqlite3_reset(pData->pInsertStmt);

    memset(&nodeValue, 0, sizeof(nodeValue));
    nodeValue.type = CYPHER_VALUE_NODE;
    nodeValue.u.iNodeId = pData->iCreatedId;

    rc = cypherResultAddColumn(pResult, pPlan->zAlias ? pPlan->zAlias : "node", &nodeValue);
    if (rc != SQLITE_OK) return rc;
  }

  pData->bExecuted = 1;
  return SQLITE_OK;
}

static int createIteratorClose(CypherIterator *pIterator) {
  WriteIteratorData *pData = (WriteIteratorData*)pIterator->pIterData;

  if (pData && pData->pInsertStmt) {
    sqlite3_finalize(pData->pInsertStmt);
    pData->pInsertStmt = NULL;
  }

  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void createIteratorDestroy(CypherIterator *pIterator) {
  if (pIterator && pIterator->pIterData) {
    WriteIteratorData *pData = (WriteIteratorData*)pIterator->pIterData;
    if (pData->pInsertStmt) {
      sqlite3_finalize(pData->pInsertStmt);
    }
    sqlite3_free(pData);
  }
}

static CypherIterator *createWriteIteratorWrapper(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  WriteIteratorData *pData;

  pIterator = sqlite3_malloc(sizeof(CypherIterator));
  if (!pIterator) return NULL;

  pData = sqlite3_malloc(sizeof(WriteIteratorData));
  if (!pData) {
    sqlite3_free(pIterator);
    return NULL;
  }

  memset(pIterator, 0, sizeof(CypherIterator));
  memset(pData, 0, sizeof(WriteIteratorData));

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

/*
** Stub implementations for other mutation iterators
** TODO: Implement full functionality
*/

static CypherIterator *mergeWriteIteratorWrapper(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  /* For now, MERGE uses same logic as CREATE */
  return createWriteIteratorWrapper(pPlan, pContext);
}

static CypherIterator *setWriteIteratorWrapper(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  /* TODO: Implement SET iterator wrapper */
  return NULL;
}

static CypherIterator *deleteWriteIteratorWrapper(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  /* TODO: Implement DELETE iterator wrapper */
  return NULL;
}