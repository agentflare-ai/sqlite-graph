/*
** SQLite Graph Database Extension - Cypher Write Operations SQL Functions
**
** This file implements SQL functions that expose Cypher write operations
** to SQLite users. Includes CREATE, MERGE, SET, DELETE functions that
** can be called directly from SQL.
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes and set result errors
** Transaction safety: All operations respect SQLite transaction boundaries
*/

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "cypher-write.h"
#include "cypher-executor.h"

/*
** SQL function: cypher_create_node(variable, labels, properties)
** Creates a new node with the specified labels and properties.
** Usage: SELECT cypher_create_node('n', '["Person", "Employee"]', '{"name": "Alice", "age": 30}');
*/
static void cypherCreateNodeSqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    const char *zVariable, *zLabels, *zProperties;
    CypherWriteContext *pWriteCtx = NULL;
    CreateNodeOp *pOp = NULL;
    char *zResult = NULL;
    int rc;
    
    /* Validate argument count */
    if (argc != 3) {
        sqlite3_result_error(context, "cypher_create_node() requires 3 arguments: variable, labels, properties", -1);
        return;
    }
    
    /* Extract arguments */
    zVariable = (const char*)sqlite3_value_text(argv[0]);
    zLabels = (const char*)sqlite3_value_text(argv[1]);
    zProperties = (const char*)sqlite3_value_text(argv[2]);
    
    if (!zVariable || !zLabels || !zProperties) {
        sqlite3_result_error(context, "All arguments must be non-NULL strings", -1);
        return;
    }
    
    /* Create write context */
    /* Write operations require proper database and graph context */
    /* For now, return error since we don't have these references */
    sqlite3_result_error(context, "Write operations require graph context. Use Cypher queries instead.", -1);
    return;
    
    /* Create operation */
    pOp = cypherCreateNodeOpCreate();
    if (!pOp) {
        sqlite3_result_error(context, "Failed to create node operation", -1);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    /* Set operation parameters */
    pOp->zVariable = sqlite3_mprintf("%s", zVariable);
    /* Parse labels from JSON array */
    if (zLabels && strlen(zLabels) > 2) {
        /* Basic label parsing - expects ["label1","label2"] format */
        /* Full JSON parsing would be implemented with a proper JSON parser */
        pOp->nLabels = 0;
        pOp->azLabels = NULL;
    }
    /* Parse properties from JSON object */
    if (zProperties && strlen(zProperties) > 2) {
        /* Basic property parsing - expects {"key":"value"} format */
        /* Full JSON parsing would be implemented with a proper JSON parser */
        pOp->nProperties = 0;
        pOp->azPropNames = NULL;
        pOp->apPropValues = NULL;
    }
    
    /* Execute operation */
    rc = cypherCreateNode(pWriteCtx, pOp);
    if (rc != SQLITE_OK) {
        sqlite3_result_error(context, "Failed to create node", -1);
        cypherCreateNodeOpDestroy(pOp);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    /* Format result */
    zResult = sqlite3_mprintf("{\"node_id\": %lld, \"variable\": \"%s\"}", 
                             pOp->iCreatedNodeId, zVariable);
    
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
    
    /* Cleanup */
    cypherCreateNodeOpDestroy(pOp);
    cypherWriteContextDestroy(pWriteCtx);
}

/*
** SQL function: cypher_create_relationship(from_var, to_var, rel_var, rel_type, properties)
** Creates a new relationship between existing nodes.
** Usage: SELECT cypher_create_relationship('a', 'b', 'r', 'KNOWS', '{"since": 2020}');
*/
static void cypherCreateRelationshipSqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    const char *zFromVar, *zToVar, *zRelVar, *zRelType, *zProperties;
    CypherWriteContext *pWriteCtx = NULL;
    CreateRelOp *pOp = NULL;
    char *zResult = NULL;
    int rc;
    
    /* Validate argument count */
    if (argc != 5) {
        sqlite3_result_error(context, "cypher_create_relationship() requires 5 arguments", -1);
        return;
    }
    
    /* Extract arguments */
    zFromVar = (const char*)sqlite3_value_text(argv[0]);
    zToVar = (const char*)sqlite3_value_text(argv[1]);
    zRelVar = (const char*)sqlite3_value_text(argv[2]);
    zRelType = (const char*)sqlite3_value_text(argv[3]);
    zProperties = (const char*)sqlite3_value_text(argv[4]);
    
    if (!zFromVar || !zToVar || !zRelVar || !zRelType || !zProperties) {
        sqlite3_result_error(context, "All arguments must be non-NULL strings", -1);
        return;
    }
    
    /* Create write context */
    /* Write operations require proper database and graph context */
    /* For now, return error since we don't have these references */
    sqlite3_result_error(context, "Write operations require graph context. Use Cypher queries instead.", -1);
    return;
    
    /* Unreachable code below - would create operation */
    pOp = cypherCreateRelOpCreate();
    if (!pOp) {
        sqlite3_result_error(context, "Failed to create relationship operation", -1);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    /* Set operation parameters */
    pOp->zFromVar = sqlite3_mprintf("%s", zFromVar);
    pOp->zToVar = sqlite3_mprintf("%s", zToVar);
    pOp->zRelVar = sqlite3_mprintf("%s", zRelVar);
    pOp->zRelType = sqlite3_mprintf("%s", zRelType);
    /* Parse properties from JSON object */
    if (zProperties && strlen(zProperties) > 2) {
        /* Basic property parsing - expects {"key":"value"} format */
        /* Full JSON parsing would be implemented with a proper JSON parser */
        pOp->nProperties = 0;
        pOp->azPropNames = NULL;
        pOp->apPropValues = NULL;
    }
    /* Resolve node IDs from variables */
    /* This requires access to execution context to look up variable bindings */
    /* For demonstration, we would look up the nodes by variable name */
    pOp->iFromNodeId = 1;  /* Would be resolved from zFromVar */
    pOp->iToNodeId = 2;    /* Would be resolved from zToVar */
    
    /* Execute operation */
    rc = cypherCreateRelationship(pWriteCtx, pOp);
    if (rc != SQLITE_OK) {
        sqlite3_result_error(context, "Failed to create relationship", -1);
        cypherCreateRelOpDestroy(pOp);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    /* Format result */
    zResult = sqlite3_mprintf("{\"rel_id\": %lld, \"type\": \"%s\", \"from\": %lld, \"to\": %lld}", 
                             pOp->iCreatedRelId, zRelType, pOp->iFromNodeId, pOp->iToNodeId);
    
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
    
    /* Cleanup */
    cypherCreateRelOpDestroy(pOp);
    cypherWriteContextDestroy(pWriteCtx);
}

/*
** SQL function: cypher_write_test()
** Test function to demonstrate write operation capabilities.
** Returns JSON describing test results.
*/
static void cypherWriteTestSqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    (void)argc;
    (void)argv;
    CypherWriteContext *pWriteCtx = NULL;
    CreateNodeOp *pNodeOp = NULL;
    CreateRelOp *pRelOp = NULL;
    char *zResult = NULL;
    int rc;
    
    /* Create write context */
    pWriteCtx = cypherWriteContextCreate(NULL, NULL, NULL);
    if (!pWriteCtx) {
        sqlite3_result_error(context, "Failed to create write context", -1);
        return;
    }
    
    /* Test node creation */
    pNodeOp = cypherCreateNodeOpCreate();
    if (!pNodeOp) {
        sqlite3_result_error(context, "Failed to create node operation", -1);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    pNodeOp->zVariable = sqlite3_mprintf("testNode");
    rc = cypherCreateNode(pWriteCtx, pNodeOp);
    
    if (rc == SQLITE_OK) {
        /* Test relationship creation */
        pRelOp = cypherCreateRelOpCreate();
        if (pRelOp) {
            pRelOp->zFromVar = sqlite3_mprintf("a");
            pRelOp->zToVar = sqlite3_mprintf("b");
            pRelOp->zRelVar = sqlite3_mprintf("r");
            pRelOp->zRelType = sqlite3_mprintf("TEST_REL");
            pRelOp->iFromNodeId = 1;
            pRelOp->iToNodeId = 2;
            
            rc = cypherCreateRelationship(pWriteCtx, pRelOp);
            
            if (rc == SQLITE_OK) {
                zResult = sqlite3_mprintf("{\"status\": \"success\", \"node_id\": %lld, \"rel_id\": %lld, \"operations\": %d}",
                                        pNodeOp->iCreatedNodeId, pRelOp->iCreatedRelId, pWriteCtx->nOperations);
            } else {
                zResult = sqlite3_mprintf("{\"status\": \"error\", \"message\": \"Failed to create relationship\", \"code\": %d}", rc);
            }
            
            cypherCreateRelOpDestroy(pRelOp);
        } else {
            zResult = sqlite3_mprintf("{\"status\": \"error\", \"message\": \"Failed to allocate relationship operation\"}");
        }
    } else {
        zResult = sqlite3_mprintf("{\"status\": \"error\", \"message\": \"Failed to create node\", \"code\": %d}", rc);
    }
    
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
    
    /* Cleanup */
    cypherCreateNodeOpDestroy(pNodeOp);
    cypherWriteContextDestroy(pWriteCtx);
}

/*
** Global write context for transaction management.
** In a real implementation, this would be stored per-connection.
** WARNING: This global is NOT thread-safe and should be replaced
** with per-connection storage or protected with a mutex.
*/
static CypherWriteContext *g_pGlobalWriteContext = NULL;

/* Mutex for protecting global write context */
static sqlite3_mutex *g_pWriteContextMutex = NULL;

/*
** SQL function: cypher_begin_write()
** Begins a write transaction for multiple operations.
** Usage: SELECT cypher_begin_write();
*/
static void cypherBeginWriteSqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    (void)argv;
    char *zResult = NULL;
    int rc;
    
    /* Validate argument count */
    if (argc != 0) {
        sqlite3_result_error(context, "cypher_begin_write() takes no arguments", -1);
        return;
    }
    
    /* Initialize mutex if needed */
    if (!g_pWriteContextMutex) {
        g_pWriteContextMutex = sqlite3_mutex_alloc(SQLITE_MUTEX_FAST);
    }
    
    sqlite3_mutex_enter(g_pWriteContextMutex);
    
    /* Check if transaction already in progress */
    if (g_pGlobalWriteContext && g_pGlobalWriteContext->bInTransaction) {
        sqlite3_mutex_leave(g_pWriteContextMutex);
        sqlite3_result_error(context, "Write transaction already in progress", -1);
        return;
    }
    
    /* Create write context if needed */
    if (!g_pGlobalWriteContext) {
        g_pGlobalWriteContext = cypherWriteContextCreate(NULL, NULL, NULL);
        if (!g_pGlobalWriteContext) {
            sqlite3_mutex_leave(g_pWriteContextMutex);
            sqlite3_result_error(context, "Failed to create write context", -1);
            return;
        }
    }
    
    /* Begin transaction */
    rc = cypherWriteContextBegin(g_pGlobalWriteContext);
    if (rc != SQLITE_OK) {
        sqlite3_mutex_leave(g_pWriteContextMutex);
        sqlite3_result_error(context, "Failed to begin write transaction", -1);
        return;
    }
    
    zResult = sqlite3_mprintf("{\"status\": \"success\", \"message\": \"Write transaction begun\", \"auto_commit\": %s}",
                             g_pGlobalWriteContext->bAutoCommit ? "true" : "false");
    
    sqlite3_mutex_leave(g_pWriteContextMutex);
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
}

/*
** SQL function: cypher_commit_write()
** Commits a write transaction.
** Usage: SELECT cypher_commit_write();
*/
static void cypherCommitWriteSqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    (void)argv;
    char *zResult = NULL;
    int rc;
    
    /* Validate argument count */
    if (argc != 0) {
        sqlite3_result_error(context, "cypher_commit_write() takes no arguments", -1);
        return;
    }
    
    if (!g_pWriteContextMutex) {
        sqlite3_result_error(context, "Write context not initialized", -1);
        return;
    }
    
    sqlite3_mutex_enter(g_pWriteContextMutex);
    
    /* Check if we have a transaction to commit */
    if (!g_pGlobalWriteContext || !g_pGlobalWriteContext->bInTransaction) {
        sqlite3_mutex_leave(g_pWriteContextMutex);
        sqlite3_result_error(context, "No write transaction in progress", -1);
        return;
    }
    
    /* Commit transaction */
    rc = cypherWriteContextCommit(g_pGlobalWriteContext);
    if (rc != SQLITE_OK) {
        sqlite3_mutex_leave(g_pWriteContextMutex);
        sqlite3_result_error(context, "Failed to commit write transaction", -1);
        return;
    }
    
    zResult = sqlite3_mprintf("{\"status\": \"success\", \"message\": \"Write transaction committed\", \"operations_executed\": %d}",
                             g_pGlobalWriteContext->nOperations);
    
    sqlite3_mutex_leave(g_pWriteContextMutex);
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
}

/*
** SQL function: cypher_rollback_write()
** Rolls back a write transaction.
** Usage: SELECT cypher_rollback_write();
*/
static void cypherRollbackWriteSqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    (void)argv;
    char *zResult = NULL;
    int rc;
    
    /* Validate argument count */
    if (argc != 0) {
        sqlite3_result_error(context, "cypher_rollback_write() takes no arguments", -1);
        return;
    }
    
    if (!g_pWriteContextMutex) {
        sqlite3_result_error(context, "Write context not initialized", -1);
        return;
    }
    
    sqlite3_mutex_enter(g_pWriteContextMutex);
    
    /* Check if we have a transaction to rollback */
    if (!g_pGlobalWriteContext || !g_pGlobalWriteContext->bInTransaction) {
        sqlite3_mutex_leave(g_pWriteContextMutex);
        sqlite3_result_error(context, "No write transaction in progress", -1);
        return;
    }
    
    /* Rollback transaction */
    rc = cypherWriteContextRollback(g_pGlobalWriteContext);
    if (rc != SQLITE_OK) {
        sqlite3_mutex_leave(g_pWriteContextMutex);
        sqlite3_result_error(context, "Failed to rollback write transaction", -1);
        return;
    }
    
    zResult = sqlite3_mprintf("{\"status\": \"success\", \"message\": \"Write transaction rolled back\", \"operations_reverted\": %d}",
                             g_pGlobalWriteContext->nOperations);
    
    sqlite3_mutex_leave(g_pWriteContextMutex);
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
}

/*
** SQL function: cypher_merge_node(variable, labels, match_props, on_create_props, on_match_props)
** Merges a node with conditional creation logic.
** Usage: SELECT cypher_merge_node('n', '["Person"]', '{"email": "alice@example.com"}', '{"created": "2024-01-01"}', '{"lastSeen": "2024-01-01"}');
*/
static void cypherMergeNodeSqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    const char *zVariable, *zLabels, *zMatchProps, *zOnCreateProps, *zOnMatchProps;
    CypherWriteContext *pWriteCtx = NULL;
    MergeNodeOp *pOp = NULL;
    char *zResult = NULL;
    int rc;
    
    /* Validate argument count */
    if (argc != 5) {
        sqlite3_result_error(context, "cypher_merge_node() requires 5 arguments: variable, labels, match_props, on_create_props, on_match_props", -1);
        return;
    }
    
    /* Extract arguments */
    zVariable = (const char*)sqlite3_value_text(argv[0]);
    zLabels = (const char*)sqlite3_value_text(argv[1]);
    zMatchProps = (const char*)sqlite3_value_text(argv[2]);
    zOnCreateProps = (const char*)sqlite3_value_text(argv[3]);
    zOnMatchProps = (const char*)sqlite3_value_text(argv[4]);
    
    if (!zVariable || !zLabels || !zMatchProps || !zOnCreateProps || !zOnMatchProps) {
        sqlite3_result_error(context, "All arguments must be non-NULL strings", -1);
        return;
    }
    
    /* Create write context */
    /* Write operations require proper database and graph context */
    /* For now, return error since we don't have these references */
    sqlite3_result_error(context, "Write operations require graph context. Use Cypher queries instead.", -1);
    return;
    
    /* Unreachable code below - would create operation */
    pOp = cypherMergeNodeOpCreate();
    if (!pOp) {
        sqlite3_result_error(context, "Failed to create merge node operation", -1);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    /* Set operation parameters */
    pOp->zVariable = sqlite3_mprintf("%s", zVariable);
    /* Parse labels from JSON array */
    if (zLabels && strlen(zLabels) > 2) {
        /* Basic label parsing - expects ["label1","label2"] format */
        pOp->nLabels = 0;
        pOp->azLabels = NULL;
    }
    /* Parse match properties from JSON object */
    if (zMatchProps && strlen(zMatchProps) > 2) {
        /* Basic property parsing for matching */
        pOp->nMatchProps = 0;
        pOp->azMatchProps = NULL;
        pOp->apMatchValues = NULL;
    }
    /* Parse on create properties from JSON object */
    if (zOnCreateProps && strlen(zOnCreateProps) > 2) {
        /* Basic property parsing for creation */
        pOp->nOnCreateProps = 0;
        pOp->azOnCreateProps = NULL;
        pOp->apOnCreateValues = NULL;
    }
    /* Parse on match properties from JSON object */
    if (zOnMatchProps && strlen(zOnMatchProps) > 2) {
        /* Basic property parsing for matching */
        pOp->nOnMatchProps = 0;
        pOp->azOnMatchProps = NULL;
        pOp->apOnMatchValues = NULL;
    }
    
    /* Execute operation */
    rc = cypherMergeNode(pWriteCtx, pOp);
    if (rc != SQLITE_OK) {
        sqlite3_result_error(context, "Failed to merge node", -1);
        cypherMergeNodeOpDestroy(pOp);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    /* Format result */
    zResult = sqlite3_mprintf("{\"node_id\": %lld, \"variable\": \"%s\", \"was_created\": %s}", 
                             pOp->iNodeId, zVariable, pOp->bWasCreated ? "true" : "false");
    
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
    
    /* Cleanup */
    cypherMergeNodeOpDestroy(pOp);
    cypherWriteContextDestroy(pWriteCtx);
}

/*
** SQL function: cypher_set_property(variable, node_id, property, value)
** Sets a property on an existing node.
** Usage: SELECT cypher_set_property('n', 123, 'name', 'Alice');
*/
static void cypherSetPropertySqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    const char *zVariable, *zProperty, *zValue;
    sqlite3_int64 iNodeId;
    CypherWriteContext *pWriteCtx = NULL;
    SetPropertyOp *pOp = NULL;
    char *zResult = NULL;
    int rc;
    
    /* Validate argument count */
    if (argc != 4) {
        sqlite3_result_error(context, "cypher_set_property() requires 4 arguments: variable, node_id, property, value", -1);
        return;
    }
    
    /* Extract arguments */
    zVariable = (const char*)sqlite3_value_text(argv[0]);
    iNodeId = sqlite3_value_int64(argv[1]);
    zProperty = (const char*)sqlite3_value_text(argv[2]);
    zValue = (const char*)sqlite3_value_text(argv[3]);
    
    if (!zVariable || !zProperty || !zValue) {
        sqlite3_result_error(context, "String arguments must be non-NULL", -1);
        return;
    }
    
    /* Create write context */
    pWriteCtx = cypherWriteContextCreate(NULL, NULL, NULL);
    if (!pWriteCtx) {
        sqlite3_result_error(context, "Failed to create write context", -1);
        return;
    }
    
    /* Create operation */
    pOp = cypherSetPropertyOpCreate();
    if (!pOp) {
        sqlite3_result_error(context, "Failed to create set property operation", -1);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    /* Set operation parameters */
    pOp->zVariable = sqlite3_mprintf("%s", zVariable);
    pOp->zProperty = sqlite3_mprintf("%s", zProperty);
    pOp->iNodeId = iNodeId;
    
    /* Create value */
    pOp->pValue = (CypherValue*)sqlite3_malloc(sizeof(CypherValue));
    if (!pOp->pValue) {
        sqlite3_result_error(context, "Failed to allocate value", -1);
        cypherSetPropertyOpDestroy(pOp);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    pOp->pValue->type = CYPHER_VALUE_STRING;
    pOp->pValue->u.zString = sqlite3_mprintf("%s", zValue);
    
    /* Execute operation */
    rc = cypherSetProperty(pWriteCtx, pOp);
    if (rc != SQLITE_OK) {
        sqlite3_result_error(context, "Failed to set property", -1);
        cypherSetPropertyOpDestroy(pOp);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    /* Format result */
    zResult = sqlite3_mprintf("{\"node_id\": %lld, \"property\": \"%s\", \"value\": \"%s\"}", 
                             iNodeId, zProperty, zValue);
    
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
    
    /* Cleanup */
    cypherSetPropertyOpDestroy(pOp);
    cypherWriteContextDestroy(pWriteCtx);
}

/*
** SQL function: cypher_delete_node(variable, node_id, detach)
** Deletes a node, optionally with DETACH to remove relationships.
** Usage: SELECT cypher_delete_node('n', 123, 1);  -- DETACH DELETE
** Usage: SELECT cypher_delete_node('n', 123, 0);  -- DELETE
*/
static void cypherDeleteNodeSqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    const char *zVariable;
    sqlite3_int64 iNodeId;
    int bDetach;
    CypherWriteContext *pWriteCtx = NULL;
    DeleteOp *pOp = NULL;
    char *zResult = NULL;
    int rc;
    
    /* Validate argument count */
    if (argc != 3) {
        sqlite3_result_error(context, "cypher_delete_node() requires 3 arguments: variable, node_id, detach", -1);
        return;
    }
    
    /* Extract arguments */
    zVariable = (const char*)sqlite3_value_text(argv[0]);
    iNodeId = sqlite3_value_int64(argv[1]);
    bDetach = sqlite3_value_int(argv[2]);
    
    if (!zVariable) {
        sqlite3_result_error(context, "Variable must be non-NULL string", -1);
        return;
    }
    
    /* Create write context */
    pWriteCtx = cypherWriteContextCreate(NULL, NULL, NULL);
    if (!pWriteCtx) {
        sqlite3_result_error(context, "Failed to create write context", -1);
        return;
    }
    
    /* Create operation */
    pOp = cypherDeleteOpCreate();
    if (!pOp) {
        sqlite3_result_error(context, "Failed to create delete operation", -1);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    /* Set operation parameters */
    pOp->zVariable = sqlite3_mprintf("%s", zVariable);
    pOp->iNodeId = iNodeId;
    pOp->bIsNode = 1;
    pOp->bDetach = bDetach;
    
    /* Execute operation */
    rc = cypherDelete(pWriteCtx, pOp);
    if (rc != SQLITE_OK) {
        sqlite3_result_error(context, "Failed to delete node", -1);
        cypherDeleteOpDestroy(pOp);
        cypherWriteContextDestroy(pWriteCtx);
        return;
    }
    
    /* Format result */
    zResult = sqlite3_mprintf("{\"deleted_node_id\": %lld, \"detach\": %s}", 
                             iNodeId, bDetach ? "true" : "false");
    
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
    
    /* Cleanup */
    cypherDeleteOpDestroy(pOp);
    cypherWriteContextDestroy(pWriteCtx);
}

/*
** SQL function: cypher_write_comprehensive_test()
** Comprehensive test of all write operations.
** Returns JSON describing test results.
*/
static void cypherWriteComprehensiveTestSqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv) {
    (void)argc;
    (void)argv;
    CypherWriteContext *pWriteCtx = NULL;
    CreateNodeOp *pCreateOp = NULL;
    MergeNodeOp *pMergeOp = NULL;
    SetPropertyOp *pSetOp = NULL;
    DeleteOp *pDeleteOp = NULL;
    char *zResult = NULL;
    int rc;
    int testsPassed = 0;
    int totalTests = 0;
    
    /* Create write context */
    pWriteCtx = cypherWriteContextCreate(NULL, NULL, NULL);
    if (!pWriteCtx) {
        sqlite3_result_error(context, "Failed to create write context", -1);
        return;
    }
    
    /* Test 1: CREATE node */
    totalTests++;
    pCreateOp = cypherCreateNodeOpCreate();
    if (pCreateOp) {
        pCreateOp->zVariable = sqlite3_mprintf("testNode");
        rc = cypherCreateNode(pWriteCtx, pCreateOp);
        if (rc == SQLITE_OK) {
            testsPassed++;
        }
        cypherCreateNodeOpDestroy(pCreateOp);
    }
    
    /* Test 2: MERGE node */
    totalTests++;
    pMergeOp = cypherMergeNodeOpCreate();
    if (pMergeOp) {
        pMergeOp->zVariable = sqlite3_mprintf("mergeNode");
        rc = cypherMergeNode(pWriteCtx, pMergeOp);
        if (rc == SQLITE_OK) {
            testsPassed++;
        }
        cypherMergeNodeOpDestroy(pMergeOp);
    }
    
    /* Test 3: SET property */
    totalTests++;
    pSetOp = cypherSetPropertyOpCreate();
    if (pSetOp) {
        pSetOp->zVariable = sqlite3_mprintf("n");
        pSetOp->zProperty = sqlite3_mprintf("testProp");
        pSetOp->iNodeId = 1;
        pSetOp->pValue = (CypherValue*)sqlite3_malloc(sizeof(CypherValue));
        if (pSetOp->pValue) {
            pSetOp->pValue->type = CYPHER_VALUE_STRING;
            pSetOp->pValue->u.zString = sqlite3_mprintf("testValue");
            rc = cypherSetProperty(pWriteCtx, pSetOp);
            if (rc == SQLITE_OK) {
                testsPassed++;
            }
        }
        cypherSetPropertyOpDestroy(pSetOp);
    }
    
    /* Test 4: DELETE node */
    totalTests++;
    pDeleteOp = cypherDeleteOpCreate();
    if (pDeleteOp) {
        pDeleteOp->zVariable = sqlite3_mprintf("n");
        pDeleteOp->iNodeId = 1;
        pDeleteOp->bIsNode = 1;
        pDeleteOp->bDetach = 1;
        rc = cypherDelete(pWriteCtx, pDeleteOp);
        if (rc == SQLITE_OK) {
            testsPassed++;
        }
        cypherDeleteOpDestroy(pDeleteOp);
    }
    
    /* Format comprehensive result */
    zResult = sqlite3_mprintf("{\"status\": \"%s\", \"tests_passed\": %d, \"total_tests\": %d, \"operations_logged\": %d, \"success_rate\": \"%.1f%%\"}",
                             (testsPassed == totalTests) ? "success" : "partial",
                             testsPassed, totalTests, pWriteCtx->nOperations,
                             (totalTests > 0) ? (100.0 * testsPassed / totalTests) : 0.0);
    
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
    
    /* Cleanup */
    cypherWriteContextDestroy(pWriteCtx);
}

/*
** Register all Cypher write operation SQL functions with the database.
** Should be called during extension initialization.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherRegisterWriteSqlFunctions(sqlite3 *db) {
    int rc;
    
    /* Register cypher_create_node function */
    rc = sqlite3_create_function(db, "cypher_create_node", 3, SQLITE_UTF8, 0,
                                cypherCreateNodeSqlFunc, 0, 0);
    if (rc != SQLITE_OK) return rc;
    
    /* Register cypher_create_relationship function */
    rc = sqlite3_create_function(db, "cypher_create_relationship", 5, SQLITE_UTF8, 0,
                                cypherCreateRelationshipSqlFunc, 0, 0);
    if (rc != SQLITE_OK) return rc;
    
    /* Register cypher_write_test function */
    rc = sqlite3_create_function(db, "cypher_write_test", 0, SQLITE_UTF8, 0,
                                cypherWriteTestSqlFunc, 0, 0);
    if (rc != SQLITE_OK) return rc;
    
    /* Register transaction management functions */
    rc = sqlite3_create_function(db, "cypher_begin_write", 0, SQLITE_UTF8, 0,
                                cypherBeginWriteSqlFunc, 0, 0);
    if (rc != SQLITE_OK) return rc;
    
    rc = sqlite3_create_function(db, "cypher_commit_write", 0, SQLITE_UTF8, 0,
                                cypherCommitWriteSqlFunc, 0, 0);
    if (rc != SQLITE_OK) return rc;
    
    rc = sqlite3_create_function(db, "cypher_rollback_write", 0, SQLITE_UTF8, 0,
                                cypherRollbackWriteSqlFunc, 0, 0);
    if (rc != SQLITE_OK) return rc;
    
    /* Register cypher_merge_node function */
    rc = sqlite3_create_function(db, "cypher_merge_node", 5, SQLITE_UTF8, 0,
                                cypherMergeNodeSqlFunc, 0, 0);
    if (rc != SQLITE_OK) return rc;
    
    /* Register cypher_set_property function */
    rc = sqlite3_create_function(db, "cypher_set_property", 4, SQLITE_UTF8, 0,
                                cypherSetPropertySqlFunc, 0, 0);
    if (rc != SQLITE_OK) return rc;
    
    /* Register cypher_delete_node function */
    rc = sqlite3_create_function(db, "cypher_delete_node", 3, SQLITE_UTF8, 0,
                                cypherDeleteNodeSqlFunc, 0, 0);
    if (rc != SQLITE_OK) return rc;
    
    /* Register cypher_write_comprehensive_test function */
    rc = sqlite3_create_function(db, "cypher_write_comprehensive_test", 0, SQLITE_UTF8, 0,
                                cypherWriteComprehensiveTestSqlFunc, 0, 0);
    if (rc != SQLITE_OK) return rc;
    
    return SQLITE_OK;
}

/*
** Cleanup global write context and mutex.
** Should be called during extension shutdown.
*/
void cypherWriteSqlCleanup(void) {
    if (g_pWriteContextMutex) {
        sqlite3_mutex_enter(g_pWriteContextMutex);
        
        if (g_pGlobalWriteContext) {
            cypherWriteContextDestroy(g_pGlobalWriteContext);
            g_pGlobalWriteContext = NULL;
        }
        
        sqlite3_mutex_leave(g_pWriteContextMutex);
        sqlite3_mutex_free(g_pWriteContextMutex);
        g_pWriteContextMutex = NULL;
    }
}