/*
** SQLite Graph Database Extension - Cypher Planner SQL Functions
**
** This file implements SQL functions that expose Cypher query planning
** capabilities to SQLite users. These functions allow users to analyze
** query plans, understand optimization decisions, and debug performance.
**
** Functions provided:
** - cypher_plan(query_text) - Generate and return execution plan
** - cypher_explain(query_text) - Detailed plan analysis with costs
** - cypher_logical_plan(query_text) - Show logical plan structure
** - cypher_optimize(query_text) - Show optimization decisions
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes or NULL on error
*/

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-planner.h"
#include <string.h>
#include <assert.h>

/*
** SQL function: cypher_plan(query_text)
**
** Parses a Cypher query and returns the physical execution plan.
** This shows the actual operators that will be used during execution.
**
** Usage: SELECT cypher_plan('MATCH (n:Person) RETURN n.name');
**
** Returns: String representation of the physical execution plan
*/
static void cypherPlanSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  const char *zQuery;
  CypherParser *pParser = NULL;
  CypherPlanner *pPlanner = NULL;
  CypherAst *pAst;
  PhysicalPlanNode *pPlan;
  char *zResult = NULL;
  int rc;
  
  /* Validate arguments */
  if( argc != 1 ) {
    sqlite3_result_error(context, "cypher_plan() requires exactly one argument", -1);
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
    if( zErrMsg ) sqlite3_free(zErrMsg);
    cypherParserDestroy(pParser);
    return;
  }
  
  /* Create planner and compile */
  pPlanner = cypherPlannerCreate(sqlite3_context_db_handle(context), NULL);
  if( !pPlanner ) {
    sqlite3_result_error_nomem(context);
    cypherParserDestroy(pParser);
    return;
  }
  
  rc = cypherPlannerCompile(pPlanner, pAst);
  if( rc != SQLITE_OK ) {
    const char *zError = cypherPlannerGetError(pPlanner);
    sqlite3_result_error(context, zError ? zError : "Compilation error", -1);
    cypherPlannerDestroy(pPlanner);
    cypherParserDestroy(pParser);
    return;
  }
  
  /* Optimize and get physical plan */
  rc = cypherPlannerOptimize(pPlanner);
  if( rc != SQLITE_OK ) {
    const char *zError = cypherPlannerGetError(pPlanner);
    sqlite3_result_error(context, zError ? zError : "Optimization error", -1);
    cypherPlannerDestroy(pPlanner);
    cypherParserDestroy(pParser);
    return;
  }
  
  pPlan = cypherPlannerGetPlan(pPlanner);
  if( pPlan ) {
    zResult = physicalPlanToString(pPlan);
    if( zResult ) {
      sqlite3_result_text(context, zResult, -1, sqlite3_free);
    } else {
      sqlite3_result_error_nomem(context);
    }
  } else {
    sqlite3_result_error(context, "No physical plan generated", -1);
  }
  
  cypherPlannerDestroy(pPlanner);
  cypherParserDestroy(pParser);
}

/*
** SQL function: cypher_logical_plan(query_text)
**
** Parses a Cypher query and returns the logical plan structure.
** This shows the intermediate representation before optimization.
**
** Usage: SELECT cypher_logical_plan('MATCH (n:Person) RETURN n.name');
**
** Returns: String representation of the logical plan
*/
static void cypherLogicalPlanSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  const char *zQuery;
  CypherParser *pParser = NULL;
  CypherPlanner *pPlanner = NULL;
  CypherAst *pAst;
  char *zResult = NULL;
  int rc;
  
  /* Validate arguments */
  if( argc != 1 ) {
    sqlite3_result_error(context, "cypher_logical_plan() requires exactly one argument", -1);
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
    if( zErrMsg ) sqlite3_free(zErrMsg);
    cypherParserDestroy(pParser);
    return;
  }
  
  /* Create planner and compile to logical plan */
  pPlanner = cypherPlannerCreate(sqlite3_context_db_handle(context), NULL);
  if( !pPlanner ) {
    sqlite3_result_error_nomem(context);
    cypherParserDestroy(pParser);
    return;
  }
  
  rc = cypherPlannerCompile(pPlanner, pAst);
  if( rc != SQLITE_OK ) {
    const char *zError = cypherPlannerGetError(pPlanner);
    sqlite3_result_error(context, zError ? zError : "Compilation error", -1);
    cypherPlannerDestroy(pPlanner);
    cypherParserDestroy(pParser);
    return;
  }
  
  /* Get logical plan string */
  if( pPlanner->pLogicalPlan ) {
    zResult = logicalPlanToString(pPlanner->pLogicalPlan);
    if( zResult ) {
      sqlite3_result_text(context, zResult, -1, sqlite3_free);
    } else {
      sqlite3_result_error_nomem(context);
    }
  } else {
    sqlite3_result_error(context, "No logical plan generated", -1);
  }
  
  cypherPlannerDestroy(pPlanner);
  cypherParserDestroy(pParser);
}

/*
** SQL function: cypher_explain(query_text)
**
** Provides detailed analysis of a Cypher query execution plan.
** Shows both logical and physical plans with cost estimates.
**
** Usage: SELECT cypher_explain('MATCH (n:Person) WHERE n.age > 30 RETURN n.name');
**
** Returns: Detailed plan analysis with costs and optimization decisions
*/
static void cypherExplainSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  const char *zQuery;
  CypherParser *pParser = NULL;
  CypherPlanner *pPlanner = NULL;
  CypherAst *pAst;
  PhysicalPlanNode *pPhysical = NULL;
  char *zLogical = NULL;
  char *zPhysical = NULL;
  char *zResult = NULL;
  int rc;
  
  /* Validate arguments */
  if( argc != 1 ) {
    sqlite3_result_error(context, "cypher_explain() requires exactly one argument", -1);
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
    if( zErrMsg ) sqlite3_free(zErrMsg);
    cypherParserDestroy(pParser);
    return;
  }
  
  /* Create planner and compile */
  pPlanner = cypherPlannerCreate(sqlite3_context_db_handle(context), NULL);
  if( !pPlanner ) {
    sqlite3_result_error_nomem(context);
    cypherParserDestroy(pParser);
    return;
  }
  
  rc = cypherPlannerCompile(pPlanner, pAst);
  if( rc != SQLITE_OK ) {
    const char *zError = cypherPlannerGetError(pPlanner);
    sqlite3_result_error(context, zError ? zError : "Compilation error", -1);
    cypherPlannerDestroy(pPlanner);
    cypherParserDestroy(pParser);
    return;
  }
  
  /* Get logical plan */
  if( pPlanner->pLogicalPlan ) {
    zLogical = logicalPlanToString(pPlanner->pLogicalPlan);
  }
  
  /* Optimize and get physical plan */
  rc = cypherPlannerOptimize(pPlanner);
  if( rc == SQLITE_OK ) {
    pPhysical = cypherPlannerGetPlan(pPlanner);
    if( pPhysical ) {
      zPhysical = physicalPlanToString(pPhysical);
    }
  }
  
  /* Build comprehensive result */
  zResult = sqlite3_mprintf(
    "Cypher Query Plan Analysis\n"
    "==========================\n"
    "Query: %s\n\n"
    "Logical Plan:\n"
    "%s\n\n"
    "Physical Plan:\n"
    "%s\n\n"
    "Optimization Notes:\n"
    "- Index usage: %s\n"
    "- Join reordering: %s\n"
    "- Estimated total cost: %.1f\n",
    zQuery,
    zLogical ? zLogical : "(failed to generate)",
    zPhysical ? zPhysical : "(failed to generate)",
    pPlanner->pContext->bUseIndexes ? "enabled" : "disabled",
    pPlanner->pContext->bReorderJoins ? "enabled" : "disabled",
    pPhysical ? pPhysical->rCost : 0.0
  );
  
  if( zResult ) {
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
  } else {
    sqlite3_result_error_nomem(context);
  }
  
  sqlite3_free(zLogical);
  sqlite3_free(zPhysical);
  cypherPlannerDestroy(pPlanner);
  cypherParserDestroy(pParser);
}

/*
** SQL function: cypher_test_plans()
**
** Creates and returns test logical and physical plans for demonstration.
** This is useful for testing and understanding plan structures.
**
** Usage: SELECT cypher_test_plans();
**
** Returns: String showing example logical and physical plans
*/
static void cypherTestPlansSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  (void)argc;
  (void)argv;
  LogicalPlanNode *pLogical;
  PhysicalPlanNode *pPhysical;
  char *zLogical = NULL;
  char *zPhysical = NULL;
  char *zResult;
  
  /* Validate arguments */
  if( argc != 0 ) {
    sqlite3_result_error(context, "cypher_test_plans() takes no arguments", -1);
    return;
  }
  
  /* Create test plans */
  pLogical = cypherCreateTestLogicalPlan();
  pPhysical = cypherCreateTestPhysicalPlan();
  
  if( pLogical ) {
    zLogical = logicalPlanToString(pLogical);
  }
  
  if( pPhysical ) {
    zPhysical = physicalPlanToString(pPhysical);
  }
  
  /* Build result */
  zResult = sqlite3_mprintf(
    "Test Cypher Query Plans\n"
    "=======================\n"
    "Example Query: MATCH (n:Person) WHERE n.age > 30 RETURN n.name\n\n"
    "Logical Plan:\n"
    "%s\n\n"
    "Physical Plan:\n"
    "%s\n\n"
    "Notes:\n"
    "- Logical plans represent the high-level query structure\n"
    "- Physical plans show specific operator implementations\n"
    "- Cost estimates guide optimization decisions\n"
    "- Index usage can dramatically improve performance\n",
    zLogical ? zLogical : "(failed to generate)",
    zPhysical ? zPhysical : "(failed to generate)"
  );
  
  if( zResult ) {
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
  } else {
    sqlite3_result_error_nomem(context);
  }
  
  sqlite3_free(zLogical);
  sqlite3_free(zPhysical);
  logicalPlanNodeDestroy(pLogical);
  physicalPlanNodeDestroy(pPhysical);
}

/*
** Register all Cypher planner SQL functions with the database.
** This should be called during extension initialization.
*/
int cypherRegisterPlannerSqlFunctions(sqlite3 *db) {
  int rc = SQLITE_OK;
  
  /* Register cypher_plan function */
  rc = sqlite3_create_function(db, "cypher_plan", 1, 
                              SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                              0, cypherPlanSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  /* Register cypher_logical_plan function */
  rc = sqlite3_create_function(db, "cypher_logical_plan", 1,
                              SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                              0, cypherLogicalPlanSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  /* Register cypher_explain function */
  rc = sqlite3_create_function(db, "cypher_explain", 1,
                              SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                              0, cypherExplainSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  /* Register cypher_test_plans function */
  rc = sqlite3_create_function(db, "cypher_test_plans", 0,
                              SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                              0, cypherTestPlansSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  return SQLITE_OK;
}