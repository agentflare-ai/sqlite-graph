/* Storage bridge between Cypher operations and graph virtual table */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-executor.h"
#include "graph-vtab.h"
#include "graph-memory.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

/*
** Forward declarations for internal functions
*/
int cypherStorageExecuteUpdate(GraphVtab *pGraph, const char *zSql, 
                               sqlite3_int64 *pRowId);
static char *cypherStorageEscapeString(const char *zStr);

/*
** Add a node to the graph storage.
** 
** Parameters:
**   pGraph - Graph virtual table instance
**   iNodeId - Node ID (0 for auto-generated)
**   azLabels - Array of label strings
**   nLabels - Number of labels
**   zProperties - JSON string of properties
**
** Returns: SQLITE_OK on success, error code on failure
*/
int cypherStorageAddNode(GraphVtab *pGraph, sqlite3_int64 iNodeId, 
                        const char **azLabels, int nLabels, 
                        const char *zProperties) {
    char *zSql = NULL;
    char *zLabelsJson = NULL;
    char *zEscapedProps = NULL;
    int rc = SQLITE_OK;
    sqlite3_int64 rowId = 0;
    
    if( !pGraph ) return SQLITE_MISUSE;
    
    /* Build labels JSON array */
    if( nLabels > 0 && azLabels ) {
        int nAlloc = 256;
        int nUsed = 0;
        zLabelsJson = sqlite3_malloc(nAlloc);
        if( !zLabelsJson ) return SQLITE_NOMEM;
        
        nUsed = snprintf(zLabelsJson, nAlloc, "[");
        
        for( int i = 0; i < nLabels; i++ ) {
            char *zEscapedLabel = cypherStorageEscapeString(azLabels[i]);
            if( !zEscapedLabel ) {
                sqlite3_free(zLabelsJson);
                return SQLITE_NOMEM;
            }
            
            int nNeeded = snprintf(NULL, 0, "%s\"%s\"", i > 0 ? "," : "", zEscapedLabel);
            if( nUsed + nNeeded + 2 >= nAlloc ) {
                nAlloc = (nUsed + nNeeded + 256) * 2;
                char *zNew = sqlite3_realloc(zLabelsJson, nAlloc);
                if( !zNew ) {
                    sqlite3_free(zLabelsJson);
                    sqlite3_free(zEscapedLabel);
                    return SQLITE_NOMEM;
                }
                zLabelsJson = zNew;
            }
            
            nUsed += snprintf(zLabelsJson + nUsed, nAlloc - nUsed, 
                             "%s\"%s\"", i > 0 ? "," : "", zEscapedLabel);
            sqlite3_free(zEscapedLabel);
        }
        
        if( nUsed + 2 < nAlloc ) {
            zLabelsJson[nUsed++] = ']';
            zLabelsJson[nUsed] = '\0';
        }
    } else {
        zLabelsJson = sqlite3_mprintf("[]");
        if( !zLabelsJson ) return SQLITE_NOMEM;
    }
    
    /* Escape properties JSON */
    if( zProperties ) {
        zEscapedProps = cypherStorageEscapeString(zProperties);
        if( !zEscapedProps ) {
            sqlite3_free(zLabelsJson);
            return SQLITE_NOMEM;
        }
    }
    
    /* Build INSERT SQL */
    if( iNodeId > 0 ) {
        /* Specific node ID requested */
        zSql = sqlite3_mprintf(
            "INSERT INTO graph_nodes (node_id, labels, properties) VALUES (%lld, '%s', %s)",
            iNodeId, zLabelsJson, 
            zEscapedProps ? sqlite3_mprintf("'%s'", zEscapedProps) : "NULL"
        );
    } else {
        /* Auto-generate node ID */
        zSql = sqlite3_mprintf(
            "INSERT INTO graph_nodes (labels, properties) VALUES ('%s', %s)",
            zLabelsJson,
            zEscapedProps ? sqlite3_mprintf("'%s'", zEscapedProps) : "NULL"
        );
    }
    
    sqlite3_free(zLabelsJson);
    sqlite3_free(zEscapedProps);
    
    if( !zSql ) return SQLITE_NOMEM;
    
    /* Execute the INSERT */
    rc = cypherStorageExecuteUpdate(pGraph, zSql, &rowId);
    sqlite3_free(zSql);
    
    return rc;
}

/*
** Add an edge (relationship) to the graph storage.
**
** Parameters:
**   pGraph - Graph virtual table instance
**   iEdgeId - Edge ID (0 for auto-generated)
**   iFromId - Source node ID
**   iToId - Target node ID
**   zType - Relationship type
**   rWeight - Edge weight
**   zProperties - JSON string of properties
**
** Returns: SQLITE_OK on success, error code on failure
*/
int cypherStorageAddEdge(GraphVtab *pGraph, sqlite3_int64 iEdgeId,
                        sqlite3_int64 iFromId, sqlite3_int64 iToId,
                        const char *zType, double rWeight, 
                        const char *zProperties) {
    char *zSql = NULL;
    char *zEscapedType = NULL;
    char *zEscapedProps = NULL;
    int rc = SQLITE_OK;
    sqlite3_int64 rowId = 0;
    
    if( !pGraph || iFromId <= 0 || iToId <= 0 ) return SQLITE_MISUSE;
    
    /* Escape relationship type */
    if( zType ) {
        zEscapedType = cypherStorageEscapeString(zType);
        if( !zEscapedType ) return SQLITE_NOMEM;
    }
    
    /* Escape properties JSON */
    if( zProperties ) {
        zEscapedProps = cypherStorageEscapeString(zProperties);
        if( !zEscapedProps ) {
            sqlite3_free(zEscapedType);
            return SQLITE_NOMEM;
        }
    }
    
    /* Build INSERT SQL */
    if( iEdgeId > 0 ) {
        /* Specific edge ID requested */
        zSql = sqlite3_mprintf(
            "INSERT INTO graph_edges (edge_id, from_node, to_node, edge_type, weight, properties) "
            "VALUES (%lld, %lld, %lld, %s, %.15g, %s)",
            iEdgeId, iFromId, iToId,
            zEscapedType ? sqlite3_mprintf("'%s'", zEscapedType) : "NULL",
            rWeight,
            zEscapedProps ? sqlite3_mprintf("'%s'", zEscapedProps) : "NULL"
        );
    } else {
        /* Auto-generate edge ID */
        zSql = sqlite3_mprintf(
            "INSERT INTO graph_edges (from_node, to_node, edge_type, weight, properties) "
            "VALUES (%lld, %lld, %s, %.15g, %s)",
            iFromId, iToId,
            zEscapedType ? sqlite3_mprintf("'%s'", zEscapedType) : "NULL",
            rWeight,
            zEscapedProps ? sqlite3_mprintf("'%s'", zEscapedProps) : "NULL"
        );
    }
    
    sqlite3_free(zEscapedType);
    sqlite3_free(zEscapedProps);
    
    if( !zSql ) return SQLITE_NOMEM;
    
    /* Execute the INSERT */
    rc = cypherStorageExecuteUpdate(pGraph, zSql, &rowId);
    sqlite3_free(zSql);
    
    return rc;
}

/*
** Update properties of a node or relationship.
**
** Parameters:
**   pGraph - Graph virtual table instance
**   iNodeId - Node ID (if updating node, 0 if updating relationship)
**   iEdgeId - Edge ID (if updating relationship, 0 if updating node)
**   zProperty - Property name to update
**   pValue - New property value
**
** Returns: SQLITE_OK on success, error code on failure
*/
int cypherStorageUpdateProperties(GraphVtab *pGraph, sqlite3_int64 iNodeId,
                                 sqlite3_int64 iEdgeId, const char *zProperty, 
                                 const CypherValue *pValue) {
    char *zSql = NULL;
    char *zValueJson = NULL;
    char *zEscapedProp = NULL;
    char *zEscapedValue = NULL;
    int rc = SQLITE_OK;
    sqlite3_int64 rowId = 0;
    
    if( !pGraph || !zProperty || !pValue ) return SQLITE_MISUSE;
    if( (iNodeId > 0 && iEdgeId > 0) || (iNodeId <= 0 && iEdgeId <= 0) ) return SQLITE_MISUSE;
    
    /* Convert value to JSON */
    zValueJson = cypherValueToJson(pValue);
    if( !zValueJson ) return SQLITE_NOMEM;
    
    /* Escape property name and value */
    zEscapedProp = cypherStorageEscapeString(zProperty);
    zEscapedValue = cypherStorageEscapeString(zValueJson);
    
    sqlite3_free(zValueJson);
    
    if( !zEscapedProp || !zEscapedValue ) {
        sqlite3_free(zEscapedProp);
        sqlite3_free(zEscapedValue);
        return SQLITE_NOMEM;
    }
    
    /* Build UPDATE SQL using JSON functions */
    if( iNodeId > 0 ) {
        /* Update node property */
        zSql = sqlite3_mprintf(
            "UPDATE graph_nodes SET properties = json_set("
            "COALESCE(properties, '{}'), '$.%s', json('%s')) "
            "WHERE node_id = %lld",
            zEscapedProp, zEscapedValue, iNodeId
        );
    } else {
        /* Update edge property */
        zSql = sqlite3_mprintf(
            "UPDATE graph_edges SET properties = json_set("
            "COALESCE(properties, '{}'), '$.%s', json('%s')) "
            "WHERE edge_id = %lld",
            zEscapedProp, zEscapedValue, iEdgeId
        );
    }
    
    sqlite3_free(zEscapedProp);
    sqlite3_free(zEscapedValue);
    
    if( !zSql ) return SQLITE_NOMEM;
    
    /* Execute the UPDATE */
    rc = cypherStorageExecuteUpdate(pGraph, zSql, &rowId);
    sqlite3_free(zSql);
    
    return rc;
}

/*
** Delete a node from the graph storage.
**
** Parameters:
**   pGraph - Graph virtual table instance
**   iNodeId - Node ID to delete
**   bDetach - If true, delete connected relationships first
**
** Returns: SQLITE_OK on success, error code on failure
*/
int cypherStorageDeleteNode(GraphVtab *pGraph, sqlite3_int64 iNodeId, int bDetach) {
    char *zSql = NULL;
    int rc = SQLITE_OK;
    sqlite3_int64 rowId = 0;
    
    if( !pGraph || iNodeId <= 0 ) return SQLITE_MISUSE;
    
    if( bDetach ) {
        /* First delete all connected relationships */
        zSql = sqlite3_mprintf(
            "DELETE FROM graph_edges WHERE from_node = %lld OR to_node = %lld",
            iNodeId, iNodeId
        );
        
        if( !zSql ) return SQLITE_NOMEM;
        
        rc = cypherStorageExecuteUpdate(pGraph, zSql, &rowId);
        sqlite3_free(zSql);
        
        if( rc != SQLITE_OK ) return rc;
    }
    
    /* Delete the node */
    zSql = sqlite3_mprintf("DELETE FROM graph_nodes WHERE node_id = %lld", iNodeId);
    if( !zSql ) return SQLITE_NOMEM;
    
    rc = cypherStorageExecuteUpdate(pGraph, zSql, &rowId);
    sqlite3_free(zSql);
    
    return rc;
}

/*
** Delete a relationship from the graph storage.
**
** Parameters:
**   pGraph - Graph virtual table instance
**   iEdgeId - Edge ID to delete
**
** Returns: SQLITE_OK on success, error code on failure
*/
int cypherStorageDeleteEdge(GraphVtab *pGraph, sqlite3_int64 iEdgeId) {
    char *zSql = NULL;
    int rc = SQLITE_OK;
    sqlite3_int64 rowId = 0;
    
    if( !pGraph || iEdgeId <= 0 ) return SQLITE_MISUSE;
    
    /* Delete the edge */
    zSql = sqlite3_mprintf("DELETE FROM graph_edges WHERE edge_id = %lld", iEdgeId);
    if( !zSql ) return SQLITE_NOMEM;
    
    rc = cypherStorageExecuteUpdate(pGraph, zSql, &rowId);
    sqlite3_free(zSql);
    
    return rc;
}

/*
** Check if a node exists in the graph storage.
**
** Parameters:
**   pGraph - Graph virtual table instance
**   iNodeId - Node ID to check
**
** Returns: 1 if node exists, 0 if not, negative value on error
*/
int cypherStorageNodeExists(GraphVtab *pGraph, sqlite3_int64 iNodeId) {
    char *zSql = NULL;
    sqlite3_stmt *pStmt = NULL;
    int rc = SQLITE_OK;
    int bExists = 0;
    
    if( !pGraph || iNodeId <= 0 ) return -1;
    
    zSql = sqlite3_mprintf("SELECT 1 FROM graph_nodes WHERE node_id = %lld LIMIT 1", iNodeId);
    if( !zSql ) return -1;
    
    rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, NULL);
    sqlite3_free(zSql);
    
    if( rc != SQLITE_OK ) return -1;
    
    rc = sqlite3_step(pStmt);
    if( rc == SQLITE_ROW ) {
        bExists = 1;
    }
    
    sqlite3_finalize(pStmt);
    return bExists;
}

/*
** Get the next available node ID.
**
** Parameters:
**   pGraph - Graph virtual table instance
**
** Returns: Next available node ID, or negative value on error
*/
sqlite3_int64 cypherStorageGetNextNodeId(GraphVtab *pGraph) {
    char *zSql = NULL;
    sqlite3_stmt *pStmt = NULL;
    int rc = SQLITE_OK;
    sqlite3_int64 iNextId = 1;
    
    if( !pGraph ) return -1;
    
    zSql = sqlite3_mprintf("SELECT COALESCE(MAX(node_id), 0) + 1 FROM graph_nodes");
    if( !zSql ) return -1;
    
    rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, NULL);
    sqlite3_free(zSql);
    
    if( rc != SQLITE_OK ) return -1;
    
    rc = sqlite3_step(pStmt);
    if( rc == SQLITE_ROW ) {
        iNextId = sqlite3_column_int64(pStmt, 0);
    }
    
    sqlite3_finalize(pStmt);
    return iNextId;
}

/*
** Get the next available edge ID.
**
** Parameters:
**   pGraph - Graph virtual table instance
**
** Returns: Next available edge ID, or negative value on error
*/
sqlite3_int64 cypherStorageGetNextEdgeId(GraphVtab *pGraph) {
    char *zSql = NULL;
    sqlite3_stmt *pStmt = NULL;
    int rc = SQLITE_OK;
    sqlite3_int64 iNextId = 1;
    
    if( !pGraph ) return -1;
    
    zSql = sqlite3_mprintf("SELECT COALESCE(MAX(edge_id), 0) + 1 FROM graph_edges");
    if( !zSql ) return -1;
    
    rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, NULL);
    sqlite3_free(zSql);
    
    if( rc != SQLITE_OK ) return -1;
    
    rc = sqlite3_step(pStmt);
    if( rc == SQLITE_ROW ) {
        iNextId = sqlite3_column_int64(pStmt, 0);
    }
    
    sqlite3_finalize(pStmt);
    return iNextId;
}

/*
** Execute an SQL update statement.
** Internal helper function.
*/
int cypherStorageExecuteUpdate(GraphVtab *pGraph, const char *zSql, 
                               sqlite3_int64 *pRowId) {
    sqlite3_stmt *pStmt = NULL;
    int rc = SQLITE_OK;
    
    if( !pGraph || !zSql ) return SQLITE_MISUSE;
    
    rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, NULL);
    if( rc != SQLITE_OK ) return rc;
    
    rc = sqlite3_step(pStmt);
    if( rc == SQLITE_DONE ) {
        if( pRowId ) {
            *pRowId = sqlite3_last_insert_rowid(pGraph->pDb);
        }
        rc = SQLITE_OK;
    } else if( rc == SQLITE_ROW ) {
        /* Should not happen for UPDATE/INSERT/DELETE */
        rc = SQLITE_OK;
    }
    
    sqlite3_finalize(pStmt);
    return rc;
}

/*
** Escape a string for SQL usage.
** Internal helper function.
** Caller must sqlite3_free() the returned string.
*/
static char *cypherStorageEscapeString(const char *zStr) {
    if( !zStr ) return NULL;
    
    int nLen = strlen(zStr);
    int nAlloc = nLen * 2 + 1; /* Worst case: every char escaped */
    char *zResult = sqlite3_malloc(nAlloc);
    if( !zResult ) return NULL;
    
    int nResult = 0;
    for( int i = 0; i < nLen && nResult < nAlloc - 1; i++ ) {
        char c = zStr[i];
        if( c == '\'' ) {
            if( nResult < nAlloc - 2 ) {
                zResult[nResult++] = '\'';
                zResult[nResult++] = '\'';
            }
        } else {
            zResult[nResult++] = c;
        }
    }
    zResult[nResult] = '\0';
    
    return zResult;
}