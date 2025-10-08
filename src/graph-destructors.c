/*
** SQLite Graph Database Extension - Destructor Callbacks
**
** This file implements proper destructor callbacks required by SQLite
** for aggregates, virtual tables, and other extension objects.
** Prevents double-frees and ensures proper cleanup.
*/

#include "graph-memory.h"
#include "graph.h"

/*
** Aggregate function context with proper cleanup
*/
typedef struct GraphAggregateContext {
  GraphMemoryContext mem_ctx;
  void *pAggData;
  int is_finalized;
} GraphAggregateContext;

/*
** Initialize aggregate context with memory management
*/
static GraphAggregateContext* graph_aggregate_init(sqlite3_context *ctx) {
  GraphAggregateContext *agg_ctx = sqlite3_aggregate_context(ctx, sizeof(GraphAggregateContext));
  if (agg_ctx && !agg_ctx->is_finalized) {
    if (agg_ctx->pAggData == NULL) {
      graph_memory_context_init(&agg_ctx->mem_ctx);
      agg_ctx->is_finalized = 0;
    }
  }
  return agg_ctx;
}

/*
** Cleanup aggregate context - called automatically by SQLite
*/
static void graph_aggregate_cleanup(void *p) {
  GraphAggregateContext *agg_ctx = (GraphAggregateContext*)p;
  if (agg_ctx && !agg_ctx->is_finalized) {
    graph_memory_context_cleanup(&agg_ctx->mem_ctx);
    agg_ctx->is_finalized = 1;
  }
}

/*
** Path aggregation functions with proper destructors
*/
static void graph_path_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  GraphAggregateContext *agg_ctx;
  char *path_segment;
  
  if (argc != 1) {
    sqlite3_result_error(ctx, "path_agg() requires exactly one argument", -1);
    return;
  }
  
  agg_ctx = graph_aggregate_init(ctx);
  if (!agg_ctx) {
    sqlite3_result_error_nomem(ctx);
    return;
  }
  
  /* Get path segment */
  path_segment = (char*)sqlite3_value_text(argv[0]);
  if (!path_segment) return;
  
  /* Accumulate path segments in memory-managed context */
  if (!agg_ctx->pAggData) {
    agg_ctx->pAggData = graph_mprintf_safe(&agg_ctx->mem_ctx, "[%Q", path_segment);
  } else {
    char *old_data = (char*)agg_ctx->pAggData;
    agg_ctx->pAggData = graph_mprintf_safe(&agg_ctx->mem_ctx, "%s,%Q", old_data, path_segment);
  }
}

static void graph_path_final(sqlite3_context *ctx) {
  GraphAggregateContext *agg_ctx = (GraphAggregateContext*)sqlite3_aggregate_context(ctx, 0);
  char *result;
  
  if (!agg_ctx || !agg_ctx->pAggData) {
    sqlite3_result_text(ctx, "[]", -1, SQLITE_STATIC);
    return;
  }
  
  /* Finalize JSON array */
  result = graph_mprintf_safe(&agg_ctx->mem_ctx, "%s]", (char*)agg_ctx->pAggData);
  if (result) {
    sqlite3_result_text(ctx, result, -1, SQLITE_TRANSIENT);
  } else {
    sqlite3_result_error_nomem(ctx);
  }
  
  /* Cleanup will be called automatically by SQLite */
  graph_aggregate_cleanup(agg_ctx);
}

/*
** Node degree aggregation with proper memory management
*/
static void graph_degree_step(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
  GraphAggregateContext *agg_ctx;
  int *count;
  
  (void)argv; /* Suppress unused parameter warning */
  
  if (argc != 1) {
    sqlite3_result_error(ctx, "degree_agg() requires exactly one argument", -1);
    return;
  }
  
  agg_ctx = graph_aggregate_init(ctx);
  if (!agg_ctx) {
    sqlite3_result_error_nomem(ctx);
    return;
  }
  
  if (!agg_ctx->pAggData) {
    agg_ctx->pAggData = graph_malloc_safe(&agg_ctx->mem_ctx, sizeof(int));
    if (agg_ctx->pAggData) {
      *(int*)agg_ctx->pAggData = 0;
    }
  }
  
  count = (int*)agg_ctx->pAggData;
  if (count) {
    (*count)++;
  }
}

static void graph_degree_final(sqlite3_context *ctx) {
  GraphAggregateContext *agg_ctx = (GraphAggregateContext*)sqlite3_aggregate_context(ctx, 0);
  
  if (!agg_ctx || !agg_ctx->pAggData) {
    sqlite3_result_int(ctx, 0);
    return;
  }
  
  sqlite3_result_int(ctx, *(int*)agg_ctx->pAggData);
  graph_aggregate_cleanup(agg_ctx);
}

/*
** Register aggregate functions with proper destructors
*/
int graph_register_aggregates(sqlite3 *db) {
  int rc = SQLITE_OK;
  
  /* Path aggregation */
  rc = sqlite3_create_function_v2(db, "path_agg", 1,
                                  SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                                  NULL, NULL, graph_path_step, graph_path_final,
                                  graph_aggregate_cleanup);
  
  if (rc != SQLITE_OK) return rc;
  
  /* Degree aggregation */
  rc = sqlite3_create_function_v2(db, "degree_agg", 1,
                                  SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                                  NULL, NULL, graph_degree_step, graph_degree_final,
                                  graph_aggregate_cleanup);
  
  return rc;
}

/*
** Virtual table destructor with comprehensive cleanup
*/
static void graph_vtab_destructor(void *p) {
  GraphVtab *vtab = (GraphVtab*)p;
  if (vtab) {
    graph_vtab_destroy_safe(vtab);
  }
}

/*
** Register a user-defined function with proper destructor
*/
int graph_create_function_safe(sqlite3 *db, const char *zFunctionName, int nArg,
                               void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
                               void (*xStep)(sqlite3_context*,int,sqlite3_value**),
                               void (*xFinal)(sqlite3_context*),
                               void (*xDestroy)(void*)) {
  return sqlite3_create_function_v2(db, zFunctionName, nArg,
                                    SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                                    NULL, xFunc, xStep, xFinal, xDestroy);
}

/*
** Safe virtual table creation with destructor callback
*/
int graph_create_module_safe(sqlite3 *db, const char *zName, const sqlite3_module *pModule) {
  /* Register the module with automatic cleanup */
  int rc = sqlite3_create_module_v2(db, zName, pModule, NULL, graph_vtab_destructor);
  return rc;
}

/*
** Initialize all destructor callbacks for the extension
*/
int graph_init_destructors(sqlite3 *db) {
  int rc = SQLITE_OK;
  
  /* Register aggregates with destructors */
  rc = graph_register_aggregates(db);
  if (rc != SQLITE_OK) return rc;
  
  /* Register virtual table module with destructor */
  extern sqlite3_module graphModule;
  rc = graph_create_module_safe(db, "graph", &graphModule);
  
  return rc;
}
