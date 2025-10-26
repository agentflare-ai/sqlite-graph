/* SQL functions for executing Cypher queries and returning JSON results */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-executor.h"
#include "graph.h"  /* For GraphVtab and extern pGraph */
#include <string.h>
#include <assert.h>
#include <stdio.h>

/* External reference to global graph virtual table */
extern GraphVtab *pGraph;

/*
** SQL function: cypher_execute(query_text)
**
** Executes a Cypher query and returns the results as JSON.
** This is the main function for running Cypher queries.
**
** Usage: SELECT cypher_execute('MATCH (n:Person) RETURN n.name');
**
** Returns: JSON array of result rows
*/
static void cypherExecuteSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  const char *zQuery;
  CypherParser *pParser = NULL;
  CypherPlanner *pPlanner = NULL;
  CypherExecutor *pExecutor = NULL;
  CypherAst *pAst;
  PhysicalPlanNode *pPlan;
  char *zResults = NULL;
  int rc;
  
  /* Validate arguments */
  if( argc != 1 ) {
    sqlite3_result_error(context, "cypher_execute() requires exactly one argument", -1);
    return;
  }
  
  zQuery = (const char*)sqlite3_value_text(argv[0]);
  if( !zQuery ) {
    sqlite3_result_null(context);
    return;
  }
  
  /* Parse the query */
  pParser = cypherParserCreate();
  if( !pParser ) {
    sqlite3_result_error_nomem(context);
    return;
  }
  
  char *zErrMsg = NULL;
  pAst = cypherParse(pParser, zQuery, &zErrMsg);
  if( !pAst ) {
    sqlite3_result_error(context, zErrMsg ? zErrMsg : "Parse error", -1);
    if (zErrMsg) sqlite3_free(zErrMsg);
    cypherParserDestroy(pParser);
    return;
  }
  if( !pAst ) {
    sqlite3_result_error(context, "No AST generated", -1);
    cypherParserDestroy(pParser);
    return;
  }
  
  /* Plan the query */
  pPlanner = cypherPlannerCreate(sqlite3_context_db_handle(context), pGraph);
  if( !pPlanner ) {
    sqlite3_result_error_nomem(context);
    cypherParserDestroy(pParser);
    return;
  }

  rc = cypherPlannerCompile(pPlanner, pAst);
  if( rc != SQLITE_OK ) {
    const char *zError = cypherPlannerGetError(pPlanner);
    sqlite3_result_error(context, zError ? zError : "Planning error", -1);
    cypherPlannerDestroy(pPlanner);
    cypherParserDestroy(pParser);
    return;
  }

  rc = cypherPlannerOptimize(pPlanner);
  if( rc != SQLITE_OK ) {
    const char *zError = cypherPlannerGetError(pPlanner);
    sqlite3_result_error(context, zError ? zError : "Optimization error", -1);
    cypherPlannerDestroy(pPlanner);
    cypherParserDestroy(pParser);
    return;
  }

  pPlan = cypherPlannerGetPlan(pPlanner);
  if( !pPlan ) {
    sqlite3_result_error(context, "No execution plan generated", -1);
    cypherPlannerDestroy(pPlanner);
    cypherParserDestroy(pParser);
    return;
  }

  /* Execute the query */
  pExecutor = cypherExecutorCreate(sqlite3_context_db_handle(context), pGraph);
  if( !pExecutor ) {
    sqlite3_result_error_nomem(context);
    cypherPlannerDestroy(pPlanner);
    cypherParserDestroy(pParser);
    return;
  }
  
  rc = cypherExecutorPrepare(pExecutor, pPlan);
  if( rc != SQLITE_OK ) {
    const char *zError = cypherExecutorGetError(pExecutor);
    sqlite3_result_error(context, zError ? zError : "Executor prepare error", -1);
    cypherExecutorDestroy(pExecutor);
    cypherPlannerDestroy(pPlanner);
    cypherParserDestroy(pParser);
    return;
  }
  
  rc = cypherExecutorExecute(pExecutor, &zResults);

  if( rc != SQLITE_OK ) {
    const char *zError = cypherExecutorGetError(pExecutor);
    sqlite3_result_error(context, zError ? zError : "Execution error", -1);
  } else if( zResults ) {
    sqlite3_result_text(context, zResults, -1, sqlite3_free);
  } else {
    sqlite3_result_text(context, "[]", -1, SQLITE_STATIC);
  }

  cypherExecutorDestroy(pExecutor);
  cypherPlannerDestroy(pPlanner);
  cypherParserDestroy(pParser);
}

/*
** SQL function: cypher_execute_explain(query_text)
**
** Executes a Cypher query and returns detailed execution statistics
** along with the results. Useful for performance analysis.
**
** Usage: SELECT cypher_execute_explain('MATCH (n:Person) RETURN n.name');
**
** Returns: JSON object with results and execution statistics
*/
static void cypherExecuteExplainSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  const char *zQuery;
  CypherParser *pParser = NULL;
  CypherPlanner *pPlanner = NULL;
  CypherExecutor *pExecutor = NULL;
  CypherAst *pAst;
  PhysicalPlanNode *pPlan;
  char *zResults = NULL;
  char *zPlanString = NULL;
  char *zFinalResult = NULL;
  int rc;
  
  /* Validate arguments */
  if( argc != 1 ) {
    sqlite3_result_error(context, "cypher_execute_explain() requires exactly one argument", -1);
    return;
  }
  
  zQuery = (const char*)sqlite3_value_text(argv[0]);
  if( !zQuery ) {
    sqlite3_result_null(context);
    return;
  }
  
  /* Parse and plan the query (same as cypher_execute) */
  pParser = cypherParserCreate();
  if( !pParser ) {
    sqlite3_result_error_nomem(context);
    return;
  }
  
  char *zErrMsg = NULL;
  pAst = cypherParse(pParser, zQuery, &zErrMsg);
  if( !pAst ) {
    sqlite3_result_error(context, zErrMsg ? zErrMsg : "Parse error", -1);
    if (zErrMsg) sqlite3_free(zErrMsg);
    cypherParserDestroy(pParser);
    return;
  }
  if( !pAst ) {
    sqlite3_result_error(context, "No AST generated", -1);
    cypherParserDestroy(pParser);
    return;
  }
  
  pPlanner = cypherPlannerCreate(sqlite3_context_db_handle(context), NULL);
  if( !pPlanner ) {
    sqlite3_result_error_nomem(context);
    cypherParserDestroy(pParser);
    return;
  }
  
  rc = cypherPlannerCompile(pPlanner, pAst);
  if( rc == SQLITE_OK ) {
    rc = cypherPlannerOptimize(pPlanner);
  }
  
  if( rc != SQLITE_OK ) {
    const char *zError = cypherPlannerGetError(pPlanner);
    sqlite3_result_error(context, zError ? zError : "Planning error", -1);
    cypherPlannerDestroy(pPlanner);
    cypherParserDestroy(pParser);
    return;
  }
  
  pPlan = cypherPlannerGetPlan(pPlanner);
  if( pPlan ) {
    zPlanString = physicalPlanToString(pPlan);
  }
  
  /* Execute the query */
  pExecutor = cypherExecutorCreate(sqlite3_context_db_handle(context), NULL);
  if( pExecutor ) {
    rc = cypherExecutorPrepare(pExecutor, pPlan);
    if( rc == SQLITE_OK ) {
      rc = cypherExecutorExecute(pExecutor, &zResults);
    }
  }
  
  /* Build comprehensive result with execution stats */
  zFinalResult = sqlite3_mprintf(
    "{\n"
    "  \"query\": \"%s\",\n"
    "  \"execution_plan\": \"%s\",\n"
    "  \"execution_status\": \"%s\",\n"
    "  \"results\": %s\n"
    "}",
    zQuery,
    zPlanString ? zPlanString : "No plan generated",
    rc == SQLITE_OK ? "SUCCESS" : "ERROR",
    zResults ? zResults : "[]"
  );
  
  if( zFinalResult ) {
    sqlite3_result_text(context, zFinalResult, -1, sqlite3_free);
  } else {
    sqlite3_result_error_nomem(context);
  }
  
  sqlite3_free(zPlanString);
  sqlite3_free(zResults);
  cypherExecutorDestroy(pExecutor);
  cypherPlannerDestroy(pPlanner);
  cypherParserDestroy(pParser);
}

/*
** SQL function: cypher_test_execute()
**
** Executes test Cypher queries for demonstration purposes.
** Shows the complete execution pipeline in action.
**
** Usage: SELECT cypher_test_execute();
**
** Returns: JSON with test query results and execution information
*/
static void cypherTestExecuteSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  char *zResult;
  (void)argv;  /* Unused parameter */
  
  /* Validate arguments */
  if( argc != 0 ) {
    sqlite3_result_error(context, "cypher_test_execute() takes no arguments", -1);
    return;
  }
  
  /* Execute a simple test query */
  zResult = cypherExecuteTestQuery(sqlite3_context_db_handle(context), 
                                  "MATCH (n) RETURN n");
  
  if( zResult ) {
    char *zFinalResult = sqlite3_mprintf(
      "{\n"
      "  \"test_query\": \"MATCH (n) RETURN n\",\n"
      "  \"description\": \"Basic node scan test query\",\n"
      "  \"results\": %s,\n"
      "  \"notes\": [\n"
      "    \"This query scans all nodes in the graph\",\n"
      "    \"Results depend on available graph data\",\n"
      "    \"Empty results indicate no graph data loaded\"\n"
      "  ]\n"
      "}",
      zResult
    );
    
    sqlite3_free(zResult);
    
    if( zFinalResult ) {
      sqlite3_result_text(context, zFinalResult, -1, sqlite3_free);
    } else {
      sqlite3_result_error_nomem(context);
    }
  } else {
    sqlite3_result_text(context, 
      "{\"error\": \"Failed to execute test query\"}", 
      -1, SQLITE_STATIC);
  }
}

/*
** Register all Cypher executor SQL functions with the database.
** This should be called during extension initialization.
*/
int cypherRegisterExecutorSqlFunctions(sqlite3 *db) {
  int rc = SQLITE_OK;
  
  /* Register cypher_execute function */
  rc = sqlite3_create_function(db, "cypher_execute", 1, 
                              SQLITE_UTF8,
                              0, cypherExecuteSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  /* Register cypher_execute_explain function */
  rc = sqlite3_create_function(db, "cypher_execute_explain", 1,
                              SQLITE_UTF8,
                              0, cypherExecuteExplainSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  /* Register cypher_test_execute function */
  rc = sqlite3_create_function(db, "cypher_test_execute", 0,
                              SQLITE_UTF8,
                              0, cypherTestExecuteSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  return SQLITE_OK;
}