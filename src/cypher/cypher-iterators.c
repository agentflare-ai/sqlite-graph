/* Iterator implementations using Volcano model (open/next/close) */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-executor.h"
#include "cypher-expressions.h"
#include "cypher-write.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "graph_debug.h"

/* Forward declarations for write operation wrapper iterators */
static CypherIterator *createWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                                                   ExecutionContext *pContext);
static CypherIterator *mergeWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                                                  ExecutionContext *pContext);
static CypherIterator *setWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                                                ExecutionContext *pContext);
static CypherIterator *deleteWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                                                   ExecutionContext *pContext);
static CypherIterator *cypherJoinCreate (PhysicalPlanNode *pPlan,
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

    case PHYSICAL_HASH_JOIN:
    case PHYSICAL_NESTED_LOOP_JOIN:
    case PHYSICAL_INDEX_NESTED_LOOP:
      return cypherJoinCreate (pPlan, pContext);

    case PHYSICAL_CREATE:
      return createWriteIteratorWrapper (pPlan, pContext);

    case PHYSICAL_MERGE:
      return mergeWriteIteratorWrapper (pPlan, pContext);

    case PHYSICAL_SET:
      return setWriteIteratorWrapper (pPlan, pContext);

    case PHYSICAL_DELETE:
      return deleteWriteIteratorWrapper (pPlan, pContext);

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

  /* Close iterator if still open */
  if (pIterator->bOpened)
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
  GraphVtab *pGraph = pIterator->pContext->pGraph;
  char *zSql;
  int rc;

  if (!pGraph)
    return SQLITE_ERROR;

  zSql = sqlite3_mprintf ("SELECT id FROM %s_nodes", pGraph->zTableName);
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

  /* Use %q to properly escape the label for SQL LIKE pattern */
  zSql = sqlite3_mprintf (
      "SELECT id FROM %s_nodes WHERE labels LIKE '%%%%\"%q\"%%%%'",
      pGraph->zTableName, pData->zLabel);
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

  char *zSql = NULL;
  if (pData->zValue && (pData->zValue[0] == '\'' || pData->zValue[0] == '"'))
    {
      int len = strlen (pData->zValue);
      char *zUnq = sqlite3_mprintf ("%.*s", len - 2, pData->zValue + 1);
      if (!zUnq)
        return SQLITE_NOMEM;
      zSql = sqlite3_mprintf (
          "SELECT id FROM %s_nodes WHERE json_extract(properties, '$.%s') = %Q",
          pGraph->zTableName, pData->zProperty, zUnq);
      sqlite3_free (zUnq);
    }
  else
    {
      zSql = sqlite3_mprintf (
          "SELECT id FROM %s_nodes WHERE json_extract(properties, '$.%s') = %s",
          pGraph->zTableName, pData->zProperty, pData->zValue);
    }

  if (!zSql)
    {
      return SQLITE_NOMEM;
    }

GDBG("PropertyIndexScan SQL: %s\n", zSql);
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

GDBG("PropertyIndexScan step...\n");
  rc = sqlite3_step (pData->pStmt);
GDBG("PropertyIndexScan rc=%d\n", rc);
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
  CypherValue filterResult;
  int rc;

  /* Keep fetching from source until we find a matching row */
  while ((rc = pData->pSource->xNext (pData->pSource, pResult)) == SQLITE_OK)
    {
      /* Evaluate filter expression */
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
              pData->pSource->xDestroy (pData->pSource);
            }
          sqlite3_free (pData);
        }
    }
}

CypherIterator *
cypherFilterCreate (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  CypherIterator *pIterator;
  FilterIteratorData *pData;

  if (!pPlan || !pPlan->pChild || !pPlan->pFilterExpr)
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
      return pData->pSource->xNext (pData->pSource, pResult);
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
  if (pIterator)
    {
      ProjectionIteratorData *pData
          = (ProjectionIteratorData *)pIterator->pIterData;
      if (pData)
        {
          if (pData->pSource)
            {
              pData->pSource->xDestroy (pData->pSource);
            }
          sqlite3_free (pData);
        }
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
              pData->pSource->xDestroy (pData->pSource);
            }
          sqlite3_free (pData->aResults);
          sqlite3_free (pData);
        }
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
              pData->pSource->xDestroy (pData->pSource);
            }
          sqlite3_free (pData);
        }
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
** Write Operation Wrapper Iterators
** Simplified version that directly inserts into database tables
*/

typedef struct WriteIteratorData
{
  sqlite3_stmt *pInsertStmt;
  int bExecuted;
  sqlite3_int64 iCreatedId;
  CypherIterator *pSource; /* Optional child iterator for chained CREATEs */
} WriteIteratorData;

static int
createIteratorOpen (CypherIterator *pIterator)
{
  WriteIteratorData *pData = (WriteIteratorData *)pIterator->pIterData;
  ExecutionContext *pContext = pIterator->pContext;
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

  /* Open child iterator if this CREATE has a child */
  if (pIterator->pPlan && pIterator->pPlan->pChild)
    {
      pData->pSource
          = cypherIteratorCreate (pIterator->pPlan->pChild, pIterator->pContext);
      if (pData->pSource)
        {
          rc = pData->pSource->xOpen (pData->pSource);
          if (rc != SQLITE_OK)
            return rc;
        }
    }

  pData->bExecuted = 0;
  pIterator->bOpened = 1;
  return SQLITE_OK;
}

static int
createIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  WriteIteratorData *pData = (WriteIteratorData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  ExecutionContext *pContext = pIterator->pContext;
  CypherValue nodeValue;
  int rc;
  const char *zLabels;
  const char *zProperties;

  /* Detect relationship CREATE by presence of $from/$to in property JSON */
  int isRelCreate = 0;
  const char *zRelType = NULL;
  char *zFromVarCopy = NULL, *zToVarCopy = NULL;
  if (pPlan->zProperty && pPlan->zProperty[0]) {
    CypherValue map;
    cypherValueInit(&map);
    if (cypherParseJsonProperties(pPlan->zProperty, &map) == SQLITE_OK &&
        map.type == CYPHER_VALUE_MAP) {
      for (int i = 0; i < map.u.map.nPairs; i++) {
        const char *k = map.u.map.azKeys[i];
        if (!k) continue;
        if (strcmp(k, "$from") == 0) {
          const char *z = cypherValueGetString(&map.u.map.apValues[i]);
          if (z) zFromVarCopy = sqlite3_mprintf("%s", z);
          isRelCreate = 1;
        } else if (strcmp(k, "$to") == 0) {
          const char *z = cypherValueGetString(&map.u.map.apValues[i]);
          if (z) zToVarCopy = sqlite3_mprintf("%s", z);
          isRelCreate = 1;
        }
      }
    }
    cypherValueDestroy(&map);
  }
  if (pPlan->zLabel && pPlan->zLabel[0]) {
    zRelType = pPlan->zLabel;
  }

  if (isRelCreate) {
    /* Pull a child row to bind endpoint variables, if any */
    if (pData->pSource) {
      CypherResult childRow;
      memset(&childRow, 0, sizeof(childRow));
      rc = pData->pSource->xNext(pData->pSource, &childRow);
      if (rc == SQLITE_DONE) {
        return SQLITE_DONE;
      } else if (rc != SQLITE_OK) {
        cypherResultClear(&childRow);
        return rc;
      }
      for (int i = 0; i < childRow.nColumns; i++) {
        CypherValue *pVal = &childRow.aValues[i];
        const char *zName = childRow.azColumnNames[i];
        if (zName && pVal) executionContextBind(pContext, zName, pVal);
      }
      cypherResultClear(&childRow);
    }
    /* Resolve endpoints */
    if (!zFromVarCopy || !zToVarCopy) { sqlite3_free(zFromVarCopy); sqlite3_free(zToVarCopy); return SQLITE_MISUSE; }
    CypherValue *vFrom = executionContextGet(pContext, zFromVarCopy);
    CypherValue *vTo = executionContextGet(pContext, zToVarCopy);
    if (!vFrom || !vTo || vFrom->type != CYPHER_VALUE_NODE || vTo->type != CYPHER_VALUE_NODE) {
      sqlite3_free(zFromVarCopy); sqlite3_free(zToVarCopy);
      return SQLITE_MISUSE;
    }

    /* Build CreateRelOp with properties excluding special keys */
    CreateRelOp *pOp = cypherCreateRelOpCreate();
    if (!pOp) return SQLITE_NOMEM;
    pOp->zRelType = zRelType ? sqlite3_mprintf("%s", zRelType) : NULL;
    pOp->iFromNodeId = vFrom->u.iNodeId;
    pOp->iToNodeId = vTo->u.iNodeId;

    /* Extract non-$ properties from zProperty JSON */
    if (pPlan->zProperty && pPlan->zProperty[0]) {
      CypherValue map2; cypherValueInit(&map2);
      if (cypherParseJsonProperties(pPlan->zProperty, &map2) == SQLITE_OK &&
          map2.type == CYPHER_VALUE_MAP && map2.u.map.nPairs > 0) {
        int count = 0;
        for (int i = 0; i < map2.u.map.nPairs; i++) {
          const char *k = map2.u.map.azKeys[i];
          if (k && k[0] != '$') count++;
        }
        if (count > 0) {
          pOp->azPropNames = (char**)sqlite3_malloc(sizeof(char*) * count);
          pOp->apPropValues = (CypherValue**)sqlite3_malloc(sizeof(CypherValue*) * count);
          if (!pOp->azPropNames || !pOp->apPropValues) {
            cypherCreateRelOpDestroy(pOp);
            cypherValueDestroy(&map2);
            return SQLITE_NOMEM;
          }
          int idx = 0;
          for (int i = 0; i < map2.u.map.nPairs; i++) {
            const char *k = map2.u.map.azKeys[i];
            if (!k || k[0] == '$') continue;
            pOp->azPropNames[idx] = sqlite3_mprintf("%s", k);
            pOp->apPropValues[idx] = (CypherValue*)sqlite3_malloc(sizeof(CypherValue));
            if (!pOp->apPropValues[idx]) { cypherCreateRelOpDestroy(pOp); cypherValueDestroy(&map2); return SQLITE_NOMEM; }
            memcpy(pOp->apPropValues[idx], &map2.u.map.apValues[i], sizeof(CypherValue));
            idx++;
          }
          pOp->nProperties = count;
        }
      }
      cypherValueDestroy(&map2);
    }

    /* Execute relationship create */
    CypherWriteContext *pWriteCtx = cypherWriteContextCreate(pContext->pDb, pContext->pGraph, pContext);
    if (!pWriteCtx) { cypherCreateRelOpDestroy(pOp); return SQLITE_NOMEM; }
    rc = cypherCreateRelationship(pWriteCtx, pOp);
    cypherWriteContextDestroy(pWriteCtx);
    if (rc != SQLITE_OK) { cypherCreateRelOpDestroy(pOp); sqlite3_free(zFromVarCopy); sqlite3_free(zToVarCopy); return rc; }

    /* Emit created relationship id */
    CypherValue relVal; memset(&relVal, 0, sizeof(relVal));
    relVal.type = CYPHER_VALUE_RELATIONSHIP; relVal.u.iRelId = pOp->iCreatedRelId;
    rc = cypherResultAddColumn(pResult, pPlan->zAlias ? pPlan->zAlias : "rel", &relVal);
    cypherCreateRelOpDestroy(pOp);
    sqlite3_free(zFromVarCopy); sqlite3_free(zToVarCopy);
    if (rc != SQLITE_OK) return rc;
    pData->bExecuted = 1;
    return SQLITE_OK;
  }

  /* If we've executed this CREATE already, delegate to child chain (if any) */
  if (pData->bExecuted)
    {
      if (pData->pSource)
        {
          return pData->pSource->xNext (pData->pSource, pResult);
        }
      return SQLITE_DONE;
    }

  /* Extract label and properties from plan */
  zLabels = (pPlan->zLabel && pPlan->zLabel[0]) ? pPlan->zLabel : "";
  zProperties
      = (pPlan->zProperty && pPlan->zProperty[0]) ? pPlan->zProperty : "{}";

  /* Bind parameters */
  sqlite3_bind_text (pData->pInsertStmt, 1, zLabels, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text (pData->pInsertStmt, 2, zProperties, -1, SQLITE_TRANSIENT);

  /* Execute INSERT */
  rc = sqlite3_step (pData->pInsertStmt);
  if (rc != SQLITE_DONE)
    {
      return SQLITE_ERROR;
    }

  /* Get the inserted row ID */
  pData->iCreatedId = sqlite3_last_insert_rowid (pContext->pDb);

  /* Reset statement for potential reuse */
  sqlite3_reset (pData->pInsertStmt);

GDBG("CREATE node labels=%s props=%s id=%lld\n", zLabels, zProperties, (long long)pData->iCreatedId);
  /* Return the created node ID as result */
  memset (&nodeValue, 0, sizeof (nodeValue));
  nodeValue.type = CYPHER_VALUE_NODE;
  nodeValue.u.iNodeId = pData->iCreatedId;

  rc = cypherResultAddColumn (pResult, pPlan->zAlias ? pPlan->zAlias : "node",
                              &nodeValue);
  if (rc != SQLITE_OK)
    return rc;

  pData->bExecuted = 1;
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
  if (pData && pData->pSource)
    {
      pData->pSource->xClose (pData->pSource);
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
      if (pData->pSource)
        {
          pData->pSource->xDestroy (pData->pSource);
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

/*
** Stub implementations for other mutation iterators
** TODO: Implement full functionality
*/

typedef struct MergeIteratorData
{
  int bExecuted;
  sqlite3_int64 iNodeId;
  CypherIterator *pSource; /* Child iterator producing bindings */
} MergeIteratorData;

static int
mergeIteratorOpen (CypherIterator *pIterator)
{
  MergeIteratorData *pData = (MergeIteratorData *)pIterator->pIterData;
  /* Open child if present */
  if (pIterator->pPlan && pIterator->pPlan->pChild)
    {
      if (!pData->pSource)
        {
          pData->pSource = cypherIteratorCreate (pIterator->pPlan->pChild,
                                                 pIterator->pContext);
          if (!pData->pSource)
            return SQLITE_NOMEM;
        }
      int rc = pData->pSource->xOpen (pData->pSource);
      if (rc != SQLITE_OK)
        return rc;
    }
  pIterator->bOpened = 1;
  return SQLITE_OK;
}

static int
mergeIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  MergeIteratorData *pData = (MergeIteratorData *)pIterator->pIterData;
  PhysicalPlanNode *pPlan = pIterator->pPlan;
  ExecutionContext *pCtx = pIterator->pContext;
  int rc = SQLITE_OK;

GDBG("MERGE iterator: start label=%s zProperty=%s\n",
          pPlan->zLabel ? pPlan->zLabel : "(null)",
          pPlan->zProperty ? pPlan->zProperty : "(null)");

  /* Pull a row from child to populate variable bindings if child exists */
  if (pData->pSource)
    {
      CypherResult childRow;
      memset (&childRow, 0, sizeof (childRow));
  rc = pData->pSource->xNext (pData->pSource, &childRow);
      if (rc == SQLITE_DONE)
        {
GDBG("MERGE iterator: child produced no rows\n");
          return SQLITE_DONE;
        }
      if (rc != SQLITE_OK)
        {
GDBG("MERGE iterator: child error rc=%d\n", rc);
          cypherResultClear (&childRow);
          return rc;
        }
GDBG("MERGE iterator: child row with %d columns\n", childRow.nColumns);
      /* Bind all columns from childRow into execution context */
      for (int i = 0; i < childRow.nColumns; i++)
        {
          CypherValue *pVal = &childRow.aValues[i];
          const char *zName = childRow.azColumnNames[i];
          if (zName && pVal)
            {
              executionContextBind (pCtx, zName, pVal);
            }
        }
      cypherResultClear (&childRow);
    }
  else
    {
      /* Fallback to single execution if no child pipeline */
      if (pData->bExecuted)
        return SQLITE_DONE;
    }

  CypherWriteContext *pWriteCtx
      = cypherWriteContextCreate (pCtx->pDb, pCtx->pGraph, pCtx);
  if (!pWriteCtx)
    return SQLITE_NOMEM;
  /* Determine if this is a relationship MERGE by looking for special keys in
   * zProperty */
  if (pPlan->zProperty || pPlan->zLabel)
    {
GDBG("MERGE plan label=%s zProperty=%s onMatch=%s onCreate=%s\n",
               pPlan->zLabel ? pPlan->zLabel : "(null)",
               pPlan->zProperty ? pPlan->zProperty : "(null)",
               pPlan->zIndexName ? pPlan->zIndexName : "(null)",
               pPlan->zValue ? pPlan->zValue : "(null)");
    }
  int isRelMerge = 0;
  const char *zFromVar = NULL;
  const char *zToVar = NULL;
  int bUndirected = 0;
  CypherValue propMapTmp;
  cypherValueInit (&propMapTmp);
  if (pPlan->zProperty && pPlan->zProperty[0])
    {
      if (cypherParseJsonProperties (pPlan->zProperty, &propMapTmp)
              == SQLITE_OK
          && propMapTmp.type == CYPHER_VALUE_MAP
          && propMapTmp.u.map.nPairs > 0)
        {
          for (int k = 0; k < propMapTmp.u.map.nPairs; k++)
            {
              const char *kname = propMapTmp.u.map.azKeys[k];
              if (kname && strcmp (kname, "$from") == 0)
                {
                  isRelMerge = 1;
                  zFromVar
                      = cypherValueGetString (&propMapTmp.u.map.apValues[k]);
                }
              if (kname && strcmp (kname, "$to") == 0)
                {
                  isRelMerge = 1;
                  zToVar
                      = cypherValueGetString (&propMapTmp.u.map.apValues[k]);
                }
              if (kname && strcmp (kname, "$undirected") == 0)
                {
                  bUndirected
                      = cypherValueGetBoolean (&propMapTmp.u.map.apValues[k]);
                }
            }
        }
    }

  if (isRelMerge)
    {
      /* Relationship MERGE path */
      MergeRelOp ropZero;
      memset (&ropZero, 0, sizeof (ropZero));
      MergeRelOp *pRop = (MergeRelOp *)sqlite3_malloc (sizeof (MergeRelOp));
      if (!pRop)
        {
          cypherValueDestroy (&propMapTmp);
          cypherWriteContextDestroy (pWriteCtx);
          return SQLITE_NOMEM;
        }
      memcpy (pRop, &ropZero, sizeof (ropZero));
      if (zFromVar)
        pRop->zFromVar = sqlite3_mprintf ("%s", zFromVar);
      if (zToVar)
        pRop->zToVar = sqlite3_mprintf ("%s", zToVar);
      GDBG("MERGE rel fromVar=%s toVar=%s type=%s undirected=%d\n",
               zFromVar ? zFromVar : "(null)",
               zToVar ? zToVar : "(null)",
               pPlan->zLabel ? pPlan->zLabel : "(null)", bUndirected);
      if (pPlan->zAlias && pPlan->zAlias[0])
        pRop->zRelVar = sqlite3_mprintf ("%s", pPlan->zAlias);
      if (pPlan->zLabel && pPlan->zLabel[0])
        pRop->zRelType = sqlite3_mprintf ("%s", pPlan->zLabel);
      /* Propagate undirected flag */
      pRop->bUndirected = bUndirected;

      /* Build match props excluding special keys */
      if (propMapTmp.type == CYPHER_VALUE_MAP && propMapTmp.u.map.nPairs > 0)
        {
          int count = 0;
          for (int m = 0; m < propMapTmp.u.map.nPairs; m++)
            {
              const char *kname = propMapTmp.u.map.azKeys[m];
              if (!kname || kname[0] == '$')
                continue;
              count++;
            }
          if (count > 0)
            {
              pRop->azMatchProps
                  = (char **)sqlite3_malloc (sizeof (char *) * count);
              pRop->apMatchValues = (CypherValue **)sqlite3_malloc (
                  sizeof (CypherValue *) * count);
              if (!pRop->azMatchProps || !pRop->apMatchValues)
                {
                  cypherValueDestroy (&propMapTmp);
                  sqlite3_free (pRop->azMatchProps);
                  sqlite3_free (pRop->apMatchValues);
                  sqlite3_free (pRop);
                  cypherWriteContextDestroy (pWriteCtx);
                  return SQLITE_NOMEM;
                }
              int idx = 0;
              for (int m = 0; m < propMapTmp.u.map.nPairs; m++)
                {
                  const char *kname = propMapTmp.u.map.azKeys[m];
                  if (!kname || kname[0] == '$')
                    continue;
                  pRop->azMatchProps[idx] = sqlite3_mprintf ("%s", kname);
                  pRop->apMatchValues[idx]
                      = (CypherValue *)sqlite3_malloc (sizeof (CypherValue));
                  if (!pRop->apMatchValues[idx])
                    {
                      cypherValueDestroy (
                          &propMapTmp); /* leak a bit on OOM acceptable */
                      sqlite3_free (pRop);
                      cypherWriteContextDestroy (pWriteCtx);
                      return SQLITE_NOMEM;
                    }
                  memcpy (pRop->apMatchValues[idx],
                          &propMapTmp.u.map.apValues[m], sizeof (CypherValue));
                  idx++;
                }
              pRop->nMatchProps = count;
            }
        }
      cypherValueDestroy (&propMapTmp);

      /* ON CREATE props from zValue */
      if (pPlan->zValue && pPlan->zValue[0])
        {
          CypherValue map2;
          cypherValueInit (&map2);
          rc = cypherParseJsonProperties (pPlan->zValue, &map2);
          if (rc == SQLITE_OK && map2.type == CYPHER_VALUE_MAP
              && map2.u.map.nPairs > 0)
            {
              int n2 = map2.u.map.nPairs;
              pRop->azOnCreateProps
                  = (char **)sqlite3_malloc (sizeof (char *) * n2);
              pRop->apOnCreateValues = (CypherValue **)sqlite3_malloc (
                  sizeof (CypherValue *) * n2);
              if (!pRop->azOnCreateProps || !pRop->apOnCreateValues)
                {
                  if (pRop->azOnCreateProps)
                    sqlite3_free (pRop->azOnCreateProps);
                  if (pRop->apOnCreateValues)
                    sqlite3_free (pRop->apOnCreateValues);
                  cypherValueDestroy (&map2);
                  sqlite3_free (pRop);
                  cypherWriteContextDestroy (pWriteCtx);
                  return SQLITE_NOMEM;
                }
              for (int i2 = 0; i2 < n2; i2++)
                {
                  pRop->azOnCreateProps[i2]
                      = sqlite3_mprintf ("%s", map2.u.map.azKeys[i2]);
                  pRop->apOnCreateValues[i2]
                      = (CypherValue *)sqlite3_malloc (sizeof (CypherValue));
                  if (!pRop->apOnCreateValues[i2])
                    {
                      rc = SQLITE_NOMEM;
                      break;
                    }
                  CypherValue *pCopy = cypherValueCopy(&map2.u.map.apValues[i2]);
                  if (!pCopy)
                    {
                      rc = SQLITE_NOMEM;
                      break;
                    }
                  *(pRop->apOnCreateValues[i2]) = *pCopy;
                  sqlite3_free(pCopy);
                }
              pRop->nOnCreateProps = n2;
            }
          cypherValueDestroy (&map2);
          if (rc != SQLITE_OK)
            {
              sqlite3_free (pRop);
              cypherWriteContextDestroy (pWriteCtx);
              return rc;
            }
        }
      /* ON MATCH props from zIndexName */
      if (pPlan->zIndexName && pPlan->zIndexName[0])
        {
GDBG("MERGE rel ON MATCH JSON: %s\n", pPlan->zIndexName);
          CypherValue map3;
          cypherValueInit (&map3);
          rc = cypherParseJsonProperties (pPlan->zIndexName, &map3);
          if (rc == SQLITE_OK && map3.type == CYPHER_VALUE_MAP
              && map3.u.map.nPairs > 0)
            {
              int n3 = map3.u.map.nPairs;
              pRop->azOnMatchProps
                  = (char **)sqlite3_malloc (sizeof (char *) * n3);
              pRop->apOnMatchValues = (CypherValue **)sqlite3_malloc (
                  sizeof (CypherValue *) * n3);
              if (!pRop->azOnMatchProps || !pRop->apOnMatchValues)
                {
                  if (pRop->azOnMatchProps)
                    sqlite3_free (pRop->azOnMatchProps);
                  if (pRop->apOnMatchValues)
                    sqlite3_free (pRop->apOnMatchValues);
                  cypherValueDestroy (&map3);
                  sqlite3_free (pRop);
                  cypherWriteContextDestroy (pWriteCtx);
                  return SQLITE_NOMEM;
                }
              for (int i3 = 0; i3 < n3; i3++)
                {
                  pRop->azOnMatchProps[i3]
                      = sqlite3_mprintf ("%s", map3.u.map.azKeys[i3]);
                  pRop->apOnMatchValues[i3]
                      = (CypherValue *)sqlite3_malloc (sizeof (CypherValue));
                  if (!pRop->apOnMatchValues[i3])
                    {
                      rc = SQLITE_NOMEM;
                      break;
                    }
                  CypherValue *pCopy3 = cypherValueCopy(&map3.u.map.apValues[i3]);
                  if (!pCopy3)
                    {
                      rc = SQLITE_NOMEM;
                      break;
                    }
                  *(pRop->apOnMatchValues[i3]) = *pCopy3;
                  sqlite3_free(pCopy3);
                }
              pRop->nOnMatchProps = n3;
            }
          cypherValueDestroy (&map3);
          if (rc != SQLITE_OK)
            {
              sqlite3_free (pRop);
              cypherWriteContextDestroy (pWriteCtx);
              return rc;
            }
        }

      /* Execute relationship MERGE */
      rc = cypherMergeRelationship (pWriteCtx, pRop);
      if (rc != SQLITE_OK)
        {
          sqlite3_free (pRop);
          cypherWriteContextDestroy (pWriteCtx);
          return rc;
        }

      /* Emit relationship id */
      CypherValue relVal;
      memset (&relVal, 0, sizeof (relVal));
      relVal.type = CYPHER_VALUE_RELATIONSHIP;
      relVal.u.iRelId = pRop->iRelId;
      rc = cypherResultAddColumn (
          pResult, pPlan->zAlias ? pPlan->zAlias : "rel", &relVal);
      sqlite3_free (pRop);
      if (rc != SQLITE_OK)
        {
          cypherWriteContextDestroy (pWriteCtx);
          return rc;
        }
      pData->bExecuted = 1;
      cypherWriteContextDestroy (pWriteCtx);
      return SQLITE_OK;
    }

  /* Node MERGE fallback */
  MergeNodeOp *pOp = cypherMergeNodeOpCreate ();
  if (!pOp)
    {
      cypherWriteContextDestroy (pWriteCtx);
      return SQLITE_NOMEM;
    }
  if (pPlan->zAlias && pPlan->zAlias[0])
    {
      pOp->zVariable = sqlite3_mprintf ("%s", pPlan->zAlias);
    }
  if (pPlan->zLabel && pPlan->zLabel[0])
    {
      pOp->azLabels = (char **)sqlite3_malloc (sizeof (char *));
      if (!pOp->azLabels)
        {
          cypherMergeNodeOpDestroy (pOp);
          cypherWriteContextDestroy (pWriteCtx);
          return SQLITE_NOMEM;
        }
      pOp->azLabels[0] = sqlite3_mprintf ("%s", pPlan->zLabel);
      pOp->nLabels = 1;
    }
  if (pPlan->zProperty && pPlan->zProperty[0])
    {
GDBG("MERGE node zProperty: %s\n", pPlan->zProperty);
      CypherValue map;
      cypherValueInit (&map);
      rc = cypherParseJsonProperties (pPlan->zProperty, &map);
      if (rc == SQLITE_OK && map.type == CYPHER_VALUE_MAP
          && map.u.map.nPairs > 0)
        {
          int n = map.u.map.nPairs;
          pOp->azMatchProps = (char **)sqlite3_malloc (sizeof (char *) * n);
          pOp->apMatchValues
              = (CypherValue **)sqlite3_malloc (sizeof (CypherValue *) * n);
          if (!pOp->azMatchProps || !pOp->apMatchValues)
            {
              if (pOp->azMatchProps)
                sqlite3_free (pOp->azMatchProps);
              if (pOp->apMatchValues)
                sqlite3_free (pOp->apMatchValues);
              cypherValueDestroy (&map);
              cypherMergeNodeOpDestroy (pOp);
              cypherWriteContextDestroy (pWriteCtx);
              return SQLITE_NOMEM;
            }
          for (int i = 0; i < n; i++)
            {
              pOp->azMatchProps[i]
                  = sqlite3_mprintf ("%s", map.u.map.azKeys[i]);
              pOp->apMatchValues[i]
                  = (CypherValue *)sqlite3_malloc (sizeof (CypherValue));
              if (!pOp->apMatchValues[i])
                {
                  rc = SQLITE_NOMEM;
                  break;
                }
              memcpy (pOp->apMatchValues[i], &map.u.map.apValues[i],
                      sizeof (CypherValue));
            }
          pOp->nMatchProps = n;
        }
      cypherValueDestroy (&map);
      if (rc != SQLITE_OK)
        {
          cypherMergeNodeOpDestroy (pOp);
          cypherWriteContextDestroy (pWriteCtx);
          return rc;
        }
    }
  if (pPlan->zValue && pPlan->zValue[0])
    {
      CypherValue map2;
      cypherValueInit (&map2);
      rc = cypherParseJsonProperties (pPlan->zValue, &map2);
      if (rc == SQLITE_OK && map2.type == CYPHER_VALUE_MAP
          && map2.u.map.nPairs > 0)
        {
          int n2 = map2.u.map.nPairs;
          pOp->azOnCreateProps
              = (char **)sqlite3_malloc (sizeof (char *) * n2);
          pOp->apOnCreateValues
              = (CypherValue **)sqlite3_malloc (sizeof (CypherValue *) * n2);
          if (!pOp->azOnCreateProps || !pOp->apOnCreateValues)
            {
              if (pOp->azOnCreateProps)
                sqlite3_free (pOp->azOnCreateProps);
              if (pOp->apOnCreateValues)
                sqlite3_free (pOp->apOnCreateValues);
              cypherValueDestroy (&map2);
              cypherMergeNodeOpDestroy (pOp);
              cypherWriteContextDestroy (pWriteCtx);
              return SQLITE_NOMEM;
            }
          for (int i2 = 0; i2 < n2; i2++)
            {
              pOp->azOnCreateProps[i2]
                  = sqlite3_mprintf ("%s", map2.u.map.azKeys[i2]);
              pOp->apOnCreateValues[i2]
                  = (CypherValue *)sqlite3_malloc (sizeof (CypherValue));
              if (!pOp->apOnCreateValues[i2])
                {
                  rc = SQLITE_NOMEM;
                  break;
                }
              memcpy (pOp->apOnCreateValues[i2], &map2.u.map.apValues[i2],
                      sizeof (CypherValue));
            }
          pOp->nOnCreateProps = n2;
        }
      cypherValueDestroy (&map2);
      if (rc != SQLITE_OK)
        {
          cypherMergeNodeOpDestroy (pOp);
          cypherWriteContextDestroy (pWriteCtx);
          return rc;
        }
    }
  if (pPlan->zIndexName && pPlan->zIndexName[0])
    {
      CypherValue map3;
      cypherValueInit (&map3);
      rc = cypherParseJsonProperties (pPlan->zIndexName, &map3);
      if (rc == SQLITE_OK && map3.type == CYPHER_VALUE_MAP
          && map3.u.map.nPairs > 0)
        {
          int n3 = map3.u.map.nPairs;
          pOp->azOnMatchProps = (char **)sqlite3_malloc (sizeof (char *) * n3);
          pOp->apOnMatchValues
              = (CypherValue **)sqlite3_malloc (sizeof (CypherValue *) * n3);
          if (!pOp->azOnMatchProps || !pOp->apOnMatchValues)
            {
              if (pOp->azOnMatchProps)
                sqlite3_free (pOp->azOnMatchProps);
              if (pOp->apOnMatchValues)
                sqlite3_free (pOp->apOnMatchValues);
              cypherValueDestroy (&map3);
              cypherMergeNodeOpDestroy (pOp);
              cypherWriteContextDestroy (pWriteCtx);
              return SQLITE_NOMEM;
            }
          for (int i3 = 0; i3 < n3; i3++)
            {
              pOp->azOnMatchProps[i3]
                  = sqlite3_mprintf ("%s", map3.u.map.azKeys[i3]);
              pOp->apOnMatchValues[i3]
                  = (CypherValue *)sqlite3_malloc (sizeof (CypherValue));
              if (!pOp->apOnMatchValues[i3])
                {
                  rc = SQLITE_NOMEM;
                  break;
                }
              memcpy (pOp->apOnMatchValues[i3], &map3.u.map.apValues[i3],
                      sizeof (CypherValue));
            }
          pOp->nOnMatchProps = n3;
        }
      cypherValueDestroy (&map3);
      if (rc != SQLITE_OK)
        {
          cypherMergeNodeOpDestroy (pOp);
          cypherWriteContextDestroy (pWriteCtx);
          return rc;
        }
    }

  rc = cypherMergeNode (pWriteCtx, pOp);
  if (rc != SQLITE_OK)
    {
      cypherMergeNodeOpDestroy (pOp);
      cypherWriteContextDestroy (pWriteCtx);
      return rc;
    }
  pData->iNodeId = pOp->iNodeId;
  CypherValue nodeValue;
  memset (&nodeValue, 0, sizeof (nodeValue));
  nodeValue.type = CYPHER_VALUE_NODE;
  nodeValue.u.iNodeId = pData->iNodeId;
  rc = cypherResultAddColumn (pResult, pPlan->zAlias ? pPlan->zAlias : "node",
                              &nodeValue);
  if (rc != SQLITE_OK)
    {
      cypherMergeNodeOpDestroy (pOp);
      cypherWriteContextDestroy (pWriteCtx);
      return rc;
    }
  pData->bExecuted = (pData->pSource == NULL); /* Only mark done if no child */
  cypherMergeNodeOpDestroy (pOp);
  cypherWriteContextDestroy (pWriteCtx);
  return SQLITE_OK;
}

static int
mergeIteratorClose (CypherIterator *pIterator)
{
  MergeIteratorData *pData = (MergeIteratorData *)pIterator->pIterData;
  if (pData && pData->pSource)
    {
      pData->pSource->xClose (pData->pSource);
    }
  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void
mergeIteratorDestroy (CypherIterator *pIterator)
{
  if (pIterator && pIterator->pIterData)
    {
      MergeIteratorData *pData = (MergeIteratorData *)pIterator->pIterData;
      if (pData->pSource)
        {
          pData->pSource->xDestroy (pData->pSource);
        }
      sqlite3_free (pIterator->pIterData);
    }
}

static CypherIterator *
mergeWriteIteratorWrapper (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  CypherIterator *pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;
  MergeIteratorData *pData = sqlite3_malloc (sizeof (MergeIteratorData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }
  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (MergeIteratorData));
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

static CypherIterator *
deleteWriteIteratorWrapper (PhysicalPlanNode *pPlan,
                            ExecutionContext *pContext)
{
  /* TODO: Implement DELETE iterator wrapper */
  return NULL;
}

/*
** Join iterator implementation (nested-loop join fallback).
** Produces Cartesian product of left and right child rows and merges columns.
*/
typedef struct JoinIteratorData
{
  CypherIterator *pLeft;  /* Left child iterator */
  CypherIterator *pRight; /* Right child iterator */
  int bLeftOpen;          /* Left child open */
  int bRightOpen;         /* Right child open */
  int bLeftHasRow;        /* Whether leftRow is valid */
  CypherResult leftRow;   /* Current left row */
} JoinIteratorData;

static int
joinIteratorOpen (CypherIterator *pIterator)
{
  JoinIteratorData *pData = (JoinIteratorData *)pIterator->pIterData;
  int rc;
  memset (&pData->leftRow, 0, sizeof (pData->leftRow));

  if (!pData->pLeft || !pData->pRight)
    return SQLITE_MISUSE;

  rc = pData->pLeft->xOpen (pData->pLeft);
  if (rc != SQLITE_OK)
    return rc;
  pData->bLeftOpen = 1;

  /* Prime the first left row */
  rc = pData->pLeft->xNext (pData->pLeft, &pData->leftRow);
  if (rc == SQLITE_DONE)
    {
      pData->bLeftHasRow = 0;
      pIterator->bOpened = 1;
      return SQLITE_OK; /* No rows available */
    }
  else if (rc != SQLITE_OK)
    {
      return rc;
    }
  pData->bLeftHasRow = 1;

  rc = pData->pRight->xOpen (pData->pRight);
  if (rc != SQLITE_OK)
    return rc;
  pData->bRightOpen = 1;

  pIterator->bOpened = 1;
  return SQLITE_OK;
}

static int
joinIteratorNext (CypherIterator *pIterator, CypherResult *pResult)
{
  JoinIteratorData *pData = (JoinIteratorData *)pIterator->pIterData;
  int rc;

  if (!pData->bLeftHasRow)
    return SQLITE_DONE;

  while (1)
    {
      CypherResult rightRow;
      memset (&rightRow, 0, sizeof (rightRow));

      rc = pData->pRight->xNext (pData->pRight, &rightRow);
      if (rc == SQLITE_OK)
        {
          /* Combine leftRow and rightRow into pResult */
          memset (pResult, 0, sizeof (CypherResult));

          /* Add left columns */
          for (int i = 0; i < pData->leftRow.nColumns; i++)
            {
              CypherValue *pVal = &pData->leftRow.aValues[i];
              const char *zName = pData->leftRow.azColumnNames[i];
              rc = cypherResultAddColumn (pResult, zName ? zName : "col",
                                          pVal);
              if (rc != SQLITE_OK)
                {
      cypherResultClear (&rightRow);
                  return rc;
                }
            }
          /* Add right columns */
          for (int j = 0; j < rightRow.nColumns; j++)
            {
              CypherValue *pValR = &rightRow.aValues[j];
              const char *zNameR = rightRow.azColumnNames[j];
              rc = cypherResultAddColumn (pResult, zNameR ? zNameR : "col",
                                          pValR);
              if (rc != SQLITE_OK)
                {
                  cypherResultDestroy (&rightRow);
                  return rc;
                }
            }
          cypherResultClear (&rightRow);
          pIterator->nRowsProduced++;
          return SQLITE_OK;
        }

      if (rc != SQLITE_DONE)
        {
          /* Propagate errors */
          return rc;
        }

      /* Advance left row and reset right */
      cypherResultClear (&pData->leftRow);
      memset (&pData->leftRow, 0, sizeof (pData->leftRow));

      rc = pData->pLeft->xNext (pData->pLeft, &pData->leftRow);
      if (rc == SQLITE_DONE)
        {
          pData->bLeftHasRow = 0;
          return SQLITE_DONE;
        }
      else if (rc != SQLITE_OK)
        {
          return rc;
        }
      /* Reset right iterator */
      pData->pRight->xClose (pData->pRight);
      rc = pData->pRight->xOpen (pData->pRight);
      if (rc != SQLITE_OK)
        return rc;
    }
}

static int
joinIteratorClose (CypherIterator *pIterator)
{
  JoinIteratorData *pData = (JoinIteratorData *)pIterator->pIterData;
  if (pData->bRightOpen)
    {
      pData->pRight->xClose (pData->pRight);
      pData->bRightOpen = 0;
    }
  if (pData->bLeftOpen)
    {
      pData->pLeft->xClose (pData->pLeft);
      pData->bLeftOpen = 0;
    }
  if (pData->bLeftHasRow)
    {
      cypherResultClear (&pData->leftRow);
      pData->bLeftHasRow = 0;
    }
  pIterator->bOpened = 0;
  return SQLITE_OK;
}

static void
joinIteratorDestroy (CypherIterator *pIterator)
{
  if (!pIterator)
    return;
  JoinIteratorData *pData = (JoinIteratorData *)pIterator->pIterData;
  if (pData)
    {
      if (pData->pLeft)
        pData->pLeft->xDestroy (pData->pLeft);
      if (pData->pRight)
        pData->pRight->xDestroy (pData->pRight);
      if (pData->bLeftHasRow)
        cypherResultClear (&pData->leftRow);
      sqlite3_free (pData);
    }
}

static CypherIterator *
cypherJoinCreate (PhysicalPlanNode *pPlan, ExecutionContext *pContext)
{
  if (!pPlan || pPlan->nChildren < 2)
    return NULL;
  CypherIterator *pIterator = sqlite3_malloc (sizeof (CypherIterator));
  if (!pIterator)
    return NULL;
  JoinIteratorData *pData = sqlite3_malloc (sizeof (JoinIteratorData));
  if (!pData)
    {
      sqlite3_free (pIterator);
      return NULL;
    }
  memset (pIterator, 0, sizeof (CypherIterator));
  memset (pData, 0, sizeof (JoinIteratorData));

  /* Create child iterators */
  pData->pLeft = cypherIteratorCreate (pPlan->apChildren[0], pContext);
  if (!pData->pLeft)
    {
      sqlite3_free (pData);
      sqlite3_free (pIterator);
      return NULL;
    }
  pData->pRight = cypherIteratorCreate (pPlan->apChildren[1], pContext);
  if (!pData->pRight)
    {
      pData->pLeft->xDestroy (pData->pLeft);
      sqlite3_free (pData);
      sqlite3_free (pIterator);
      return NULL;
    }

  pIterator->xOpen = joinIteratorOpen;
  pIterator->xNext = joinIteratorNext;
  pIterator->xClose = joinIteratorClose;
  pIterator->xDestroy = joinIteratorDestroy;
  pIterator->pContext = pContext;
  pIterator->pPlan = pPlan;
  pIterator->pIterData = pData;

  return pIterator;
}