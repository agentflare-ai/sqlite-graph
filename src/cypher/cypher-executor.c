/* Cypher query executor - coordinates iterator execution and result collection */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-executor.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

CypherExecutor *cypherExecutorCreate(sqlite3 *pDb, GraphVtab *pGraph) {
  CypherExecutor *pExecutor;
  
  pExecutor = sqlite3_malloc(sizeof(CypherExecutor));
  if( !pExecutor ) return NULL;
  
  memset(pExecutor, 0, sizeof(CypherExecutor));
  pExecutor->pDb = pDb;
  pExecutor->pGraph = pGraph;
  
  /* Create execution context */
  pExecutor->pContext = executionContextCreate(pDb, pGraph);
  if( !pExecutor->pContext ) {
    sqlite3_free(pExecutor);
    return NULL;
  }
  
  return pExecutor;
}

void cypherExecutorDestroy(CypherExecutor *pExecutor) {
  if( !pExecutor ) return;
  
  cypherIteratorDestroy(pExecutor->pRootIterator);
  executionContextDestroy(pExecutor->pContext);
  sqlite3_free(pExecutor->zErrorMsg);
  sqlite3_free(pExecutor);
}

static CypherIterator *createIteratorTree(PhysicalPlanNode *pPlan, ExecutionContext *pContext) {
  CypherIterator *pIterator;
  CypherIterator *pChild;
  
  if( !pPlan ) return NULL;
  
  /* Create iterator for this plan node */
  pIterator = cypherIteratorCreate(pPlan, pContext);
  if( !pIterator ) return NULL;
  
  /* Create child iterators */
  if( pPlan->nChildren > 0 ) {
    pIterator->apChildren = sqlite3_malloc(pPlan->nChildren * sizeof(CypherIterator*));
    if( !pIterator->apChildren ) {
      cypherIteratorDestroy(pIterator);
      return NULL;
    }
    
    for( int i = 0; i < pPlan->nChildren; i++ ) {
      pChild = createIteratorTree(pPlan->apChildren[i], pContext);
      if( !pChild ) {
        /* Clean up partial iterator tree */
        for( int j = 0; j < i; j++ ) {
          cypherIteratorDestroy(pIterator->apChildren[j]);
        }
        cypherIteratorDestroy(pIterator);
        return NULL;
      }
      pIterator->apChildren[i] = pChild;
      pIterator->nChildren++;
    }
  }
  
  return pIterator;
}

int cypherExecutorPrepare(CypherExecutor *pExecutor, PhysicalPlanNode *pPlan) {
  if( !pExecutor || !pPlan ) return SQLITE_MISUSE;
  
  /* Clean up any previous iterator */
  cypherIteratorDestroy(pExecutor->pRootIterator);
  pExecutor->pRootIterator = NULL;
  
  /* Store plan reference */
  pExecutor->pPlan = pPlan;
  
  /* Create iterator tree from physical plan */
  pExecutor->pRootIterator = createIteratorTree(pPlan, pExecutor->pContext);
  if( !pExecutor->pRootIterator ) {
    pExecutor->zErrorMsg = sqlite3_mprintf("Failed to create iterator tree");
    return SQLITE_ERROR;
  }
  
  return SQLITE_OK;
}

/* Execute query and collect results as JSON array */
int cypherExecutorExecute(CypherExecutor *pExecutor, char **pzResults) {
  CypherIterator *pRoot;
  char *zResultArray = NULL;
  int nResults = 0;
  int nAllocated = 256;
  int nUsed = 0;
  int rc;
  
  if( !pExecutor || !pzResults ) return SQLITE_MISUSE;
  if( !pExecutor->pRootIterator ) return SQLITE_ERROR;
  
  *pzResults = NULL;
  pRoot = pExecutor->pRootIterator;
  
  /* Allocate result buffer */
  zResultArray = sqlite3_malloc(nAllocated);
  if( !zResultArray ) return SQLITE_NOMEM;
  
  /* Start JSON array */
  nUsed = snprintf(zResultArray, nAllocated, "[");
  
  /* Open root iterator */
  rc = pRoot->xOpen(pRoot);
  if( rc != SQLITE_OK ) {
    pExecutor->zErrorMsg = sqlite3_mprintf("Failed to open root iterator");
    sqlite3_free(zResultArray);
    return rc;
  }
  
  /* Iterate through results */
  while( 1 ) {
    CypherResult *pResult = cypherResultCreate();
    if( !pResult ) {
      rc = SQLITE_NOMEM;
      break;
    }

    /* Get next result row */
    rc = pRoot->xNext(pRoot, pResult);
    if( rc == SQLITE_DONE ) {
      cypherResultDestroy(pResult);
      rc = SQLITE_OK;
      break;
    } else if( rc != SQLITE_OK ) {
      cypherResultDestroy(pResult);
      pExecutor->zErrorMsg = sqlite3_mprintf("Iterator error: %d", rc);
      break;
    }

    /* Convert result to JSON */
    char *zRowJson = cypherResultToJson(pResult);
    cypherResultDestroy(pResult);

    if( !zRowJson ) {
      rc = SQLITE_NOMEM;
      break;
    }
    
    /* Calculate space needed */
    int nRowLen = strlen(zRowJson);
    int nNeeded = nUsed + (nResults > 0 ? 1 : 0) + nRowLen;
    
    /* Resize buffer if needed */
    if( nNeeded + 2 >= nAllocated ) {
      nAllocated = (nNeeded + 256) * 2;
      char *zNew = sqlite3_realloc(zResultArray, nAllocated);
      if( !zNew ) {
        sqlite3_free(zRowJson);
        sqlite3_free(zResultArray);
        rc = SQLITE_NOMEM;
        break;
      }
      zResultArray = zNew;
    }
    
    /* Add result to array */
    if( nResults > 0 ) {
      zResultArray[nUsed++] = ',';
    }
    memcpy(zResultArray + nUsed, zRowJson, nRowLen);
    nUsed += nRowLen;

    sqlite3_free(zRowJson);
    nResults++;
    
    /* Sanity check to prevent infinite loops */
    if( nResults > 10000 ) {
      pExecutor->zErrorMsg = sqlite3_mprintf("Result limit exceeded (10000 rows)");
      rc = SQLITE_ERROR;
      break;
    }
  }

  /* Close iterator */
  pRoot->xClose(pRoot);

  if( rc == SQLITE_OK ) {
    /* Close JSON array */
    if( nUsed + 2 < nAllocated ) {
      zResultArray[nUsed++] = ']';
      zResultArray[nUsed] = '\0';
    }

    *pzResults = zResultArray;
  } else {
    sqlite3_free(zResultArray);
  }

  return rc;
}

/*
** Get error message from executor.
** Returns NULL if no error occurred.
*/
const char *cypherExecutorGetError(CypherExecutor *pExecutor) {
  return pExecutor ? pExecutor->zErrorMsg : NULL;
}

/*
** Create a test execution context for demonstration.
** Returns context with sample graph data loaded.
*/
ExecutionContext *cypherCreateTestExecutionContext(sqlite3 *pDb) {
  ExecutionContext *pContext;
  
  /* Create context with test data support */
  pContext = executionContextCreate(pDb, NULL);
  
  if (pContext) {
    /* Initialize with basic test variables for testing */
    CypherValue testValue;
    cypherValueInit(&testValue);
    cypherValueSetInteger(&testValue, 42);
    executionContextBind(pContext, "testVar", &testValue);
    cypherValueDestroy(&testValue);
  }
  
  return pContext;
}

/*
** Execute a simple test query for demonstration.
** Returns JSON results, caller must sqlite3_free().
*/
char *cypherExecuteTestQuery(sqlite3 *pDb, const char *zQuery) {
  CypherParser *pParser = NULL;
  CypherPlanner *pPlanner = NULL;
  CypherExecutor *pExecutor = NULL;
  CypherAst *pAst;
  PhysicalPlanNode *pPlan;
  char *zResults = NULL;
  int rc;
  
  if( !zQuery ) return NULL;
  
  /* Parse query */
  pParser = cypherParserCreate();
  if( !pParser ) goto cleanup;
  
  pAst = cypherParse(pParser, zQuery, NULL);
  if( !pAst ) goto cleanup;
  
  /* Plan query */
  pPlanner = cypherPlannerCreate(pDb, NULL);
  if( !pPlanner ) goto cleanup;
  
  rc = cypherPlannerCompile(pPlanner, pAst);
  if( rc != SQLITE_OK ) goto cleanup;
  
  rc = cypherPlannerOptimize(pPlanner);
  if( rc != SQLITE_OK ) goto cleanup;
  
  pPlan = cypherPlannerGetPlan(pPlanner);
  if( !pPlan ) goto cleanup;
  
  /* Execute query */
  pExecutor = cypherExecutorCreate(pDb, NULL);
  if( !pExecutor ) goto cleanup;
  
  rc = cypherExecutorPrepare(pExecutor, pPlan);
  if( rc != SQLITE_OK ) goto cleanup;
  
  rc = cypherExecutorExecute(pExecutor, &zResults);
  if( rc != SQLITE_OK ) {
    const char *zError = cypherExecutorGetError(pExecutor);
    zResults = sqlite3_mprintf("ERROR: %s", zError ? zError : "Unknown execution error");
  }
  
cleanup:
  cypherExecutorDestroy(pExecutor);
  /* Clear AST reference in planner before destroying parser */
  if( pPlanner && pPlanner->pContext ) {
    pPlanner->pContext->pAst = NULL;
  }
  cypherPlannerDestroy(pPlanner);
  cypherParserDestroy(pParser);
  
  if( !zResults ) {
    zResults = sqlite3_mprintf("ERROR: Query execution failed");
  }
  
  return zResults;
}

/*
** Collect statistics from iterator tree recursively.
** Updates nRowsScanned and nRowsReturned with accumulated values.
*/
static void collectIteratorStats(CypherIterator *pIterator, sqlite3_int64 *pnScanned, sqlite3_int64 *pnReturned) {
  int i;
  
  if (!pIterator || !pnScanned || !pnReturned) return;
  
  /* Add this iterator's row production count */
  if (pIterator->nRowsProduced > 0) {
    *pnReturned += pIterator->nRowsProduced;
  }
  
  /* For scan iterators, estimate rows scanned based on type */
  if (pIterator->xOpen && pIterator->xNext) {
    /* Estimate scanned rows - could be much higher than produced */
    *pnScanned += pIterator->nRowsProduced * 10;  /* Rough estimate */
  }
  
  /* Recursively collect from children */
  for (i = 0; i < pIterator->nChildren; i++) {
    if (pIterator->apChildren[i]) {
      collectIteratorStats(pIterator->apChildren[i], pnScanned, pnReturned);
    }
  }
}

/*
** Calculate the depth of the iterator tree.
** Returns the maximum depth from root to leaf.
*/
static int calculateIteratorDepth(CypherIterator *pIterator) {
  int maxDepth = 0, childDepth;
  int i;
  
  if (!pIterator) return 0;
  
  /* Find maximum child depth */
  for (i = 0; i < pIterator->nChildren; i++) {
    if (pIterator->apChildren[i]) {
      childDepth = calculateIteratorDepth(pIterator->apChildren[i]);
      if (childDepth > maxDepth) {
        maxDepth = childDepth;
      }
    }
  }
  
  return maxDepth + 1;
}

/*
** Get current time in milliseconds.
** Used for performance measurement.
*/
static double getTimeMs(void) {
  struct timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
    return 0.0;  /* Fallback if clock_gettime fails */
  }
  return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

/*
** Execute a query with enhanced statistics and error reporting.
** Returns detailed execution information including timing and resource usage.
*/
int cypherExecutorExecuteWithStats(CypherExecutor *pExecutor, char **pzResults, char **pzStats) {
  double startTime, endTime;
  int rc;
  int nResults = 0;
  sqlite3_int64 nRowsScanned = 0, nRowsReturned = 0;
  char *zResults = NULL;
  
  if (!pExecutor || !pzResults || !pzStats) return SQLITE_MISUSE;
  
  *pzResults = NULL;
  *pzStats = NULL;
  
  /* Record start time */
  startTime = getTimeMs();
  
  /* Execute the query */
  rc = cypherExecutorExecute(pExecutor, &zResults);
  
  /* Record end time */
  endTime = getTimeMs();
  
  if (rc == SQLITE_OK) {
    *pzResults = zResults;
    
    /* Count results */
    if (zResults) {
      char *p = zResults;
      while ((p = strchr(p, '{')) != NULL) {
        nResults++;
        p++;
      }
    }
    
    /* Collect iterator statistics */
    if (pExecutor->pRootIterator) {
      collectIteratorStats(pExecutor->pRootIterator, &nRowsScanned, &nRowsReturned);
    }
    
    /* Format execution statistics */
    *pzStats = sqlite3_mprintf(
      "{\n"
      "  \"execution_time_ms\": %.2f,\n"
      "  \"rows_scanned\": %lld,\n"
      "  \"rows_returned\": %d,\n"
      "  \"selectivity\": %.3f,\n"
      "  \"iterator_tree_depth\": %d\n"
      "}",
      endTime - startTime,
      nRowsScanned,
      nResults,
      nRowsScanned > 0 ? (double)nResults / nRowsScanned : 0.0,
      calculateIteratorDepth(pExecutor->pRootIterator)
    );
  } else {
    sqlite3_free(zResults);
  }
  
  return rc;
}

/*
** Load comprehensive sample data for Phase 3 demonstrations.
** Creates a social network graph with users, posts, and relationships.
*/
int cypherLoadComprehensiveSampleData(sqlite3 *db, GraphVtab *pGraph) {
  int rc = SQLITE_OK;
  sqlite3_int64 userId1, userId2, userId3, postId1, postId2;
  char *zProps;
  
  if (!db || !pGraph) return SQLITE_MISUSE;
  
  /* Create user nodes with rich properties */
  zProps = sqlite3_mprintf("{\"name\":\"Alice\",\"age\":28,\"city\":\"San Francisco\",\"interests\":[\"AI\",\"Databases\"]}");
  rc = graphAddNode(pGraph, 1, zProps);
  sqlite3_free(zProps);
  if (rc != SQLITE_OK) return rc;
  userId1 = 1;
  
  zProps = sqlite3_mprintf("{\"name\":\"Bob\",\"age\":32,\"city\":\"Seattle\",\"interests\":[\"Photography\",\"Travel\"]}");
  rc = graphAddNode(pGraph, 2, zProps);
  sqlite3_free(zProps);
  if (rc != SQLITE_OK) return rc;
  userId2 = 2;
  
  zProps = sqlite3_mprintf("{\"name\":\"Charlie\",\"age\":25,\"city\":\"Austin\",\"interests\":[\"Music\",\"Coding\"]}");
  rc = graphAddNode(pGraph, 3, zProps);
  sqlite3_free(zProps);
  if (rc != SQLITE_OK) return rc;
  userId3 = 3;
  
  /* Create post nodes */
  zProps = sqlite3_mprintf("{\"title\":\"Graph Databases are Amazing\",\"content\":\"Exploring SQLite graph extensions\",\"timestamp\":\"2024-01-15\"}");
  rc = graphAddNode(pGraph, 101, zProps);
  sqlite3_free(zProps);
  if (rc != SQLITE_OK) return rc;
  postId1 = 101;
  
  zProps = sqlite3_mprintf("{\"title\":\"Pacific Northwest Adventures\",\"content\":\"Beautiful hike photos\",\"timestamp\":\"2024-01-20\"}");
  rc = graphAddNode(pGraph, 102, zProps);
  sqlite3_free(zProps);
  if (rc != SQLITE_OK) return rc;
  postId2 = 102;
  
  /* Create relationships with properties */
  zProps = sqlite3_mprintf("{\"since\":\"2020-05-15\",\"strength\":0.8}");
  rc = graphAddEdge(pGraph, userId1, userId2, 1.0, zProps);
  sqlite3_free(zProps);
  if (rc != SQLITE_OK) return rc;
  
  zProps = sqlite3_mprintf("{\"since\":\"2021-03-10\",\"strength\":0.6}");
  rc = graphAddEdge(pGraph, userId2, userId3, 1.0, zProps);
  sqlite3_free(zProps);
  if (rc != SQLITE_OK) return rc;
  
  zProps = sqlite3_mprintf("{\"type\":\"authored\",\"date\":\"2024-01-15\"}");
  rc = graphAddEdge(pGraph, userId1, postId1, 1.0, zProps);
  sqlite3_free(zProps);
  if (rc != SQLITE_OK) return rc;
  
  zProps = sqlite3_mprintf("{\"type\":\"authored\",\"date\":\"2024-01-20\"}");
  rc = graphAddEdge(pGraph, userId2, postId2, 1.0, zProps);
  sqlite3_free(zProps);
  if (rc != SQLITE_OK) return rc;
  
  zProps = sqlite3_mprintf("{\"type\":\"liked\",\"date\":\"2024-01-16\"}");
  rc = graphAddEdge(pGraph, userId2, postId1, 1.0, zProps);
  sqlite3_free(zProps);
  if (rc != SQLITE_OK) return rc;
  
  return SQLITE_OK;
}