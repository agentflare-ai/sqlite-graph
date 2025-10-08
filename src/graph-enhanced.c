/*
** SQLite Graph Database Extension - Enhanced Storage Functions
**
** This file implements enhanced storage functions with label and relationship
** type support for Cypher compatibility. These functions extend the basic
** graph storage with schema-aware operations.
**
** Key features:
** - Node operations with multi-label support
** - Edge operations with relationship types
** - Label management (add, remove, query)
** - Automatic schema registration and indexing
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes (SQLITE_OK, etc.)
** Thread safety: Extension supports SQLite threading modes
*/

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "graph.h"
#include "graph-memory.h"
#include "cypher/cypher-api.h"
#include <string.h>
#include <assert.h>

int graphAddNodeWithLabels(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                           const char **azLabels, int nLabels,
                           const char *zProperties) {
  char *zSql;
  int rc;
  char *zLabelsJson = 0;

  if( azLabels && nLabels>0 ){
    zLabelsJson = sqlite3_mprintf("[");
    for(int i=0; i<nLabels; i++){
      zLabelsJson = sqlite3_mprintf("%s%s\"%s\"", zLabelsJson, i>0?",":"", azLabels[i]);
    }
    zLabelsJson = sqlite3_mprintf("%s]", zLabelsJson);
  }

  zSql = sqlite3_mprintf("INSERT INTO %s_nodes(id, properties, labels) VALUES(%lld, %Q, %Q)", pVtab->zTableName, iNodeId, zProperties, zLabelsJson);
  sqlite3_free(zLabelsJson);
  rc = sqlite3_exec(pVtab->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  return rc;
}

int graphAddEdgeWithType(GraphVtab *pVtab, sqlite3_int64 iFromId,
                         sqlite3_int64 iToId, const char *zType,
                         double rWeight, const char *zProperties) {
  char *zSql;
  int rc;

  zSql = sqlite3_mprintf("INSERT INTO %s_edges(from_id, to_id, weight, properties, rel_type) VALUES(%lld, %lld, %f, %Q, %Q)", pVtab->zTableName, iFromId, iToId, rWeight, zProperties, zType);
  rc = sqlite3_exec(pVtab->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  return rc;
}

int graphSetNodeLabels(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                       const char **azLabels, int nLabels) {
  char *zSql;
  int rc;
  char *zLabelsJson = 0;

  if( azLabels && nLabels>0 ){
    zLabelsJson = sqlite3_mprintf("[");
    for(int i=0; i<nLabels; i++){
      zLabelsJson = sqlite3_mprintf("%s%s\"%s\"", zLabelsJson, i>0?",":"", azLabels[i]);
    }
    zLabelsJson = sqlite3_mprintf("%s]", zLabelsJson);
  }

  zSql = sqlite3_mprintf("UPDATE %s_nodes SET labels = %Q WHERE id = %lld", pVtab->zTableName, zLabelsJson, iNodeId);
  sqlite3_free(zLabelsJson);
  rc = sqlite3_exec(pVtab->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  return rc;
}

int graphAddNodeLabel(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                      const char *zLabel) {
  char *zSql, *zExistingLabels = NULL, *zNewLabels = NULL;
  sqlite3_stmt *pStmt;
  int rc;
  
  // Input validation
  if (!pVtab || !zLabel || iNodeId <= 0) {
    return SQLITE_MISUSE;
  }
  
  // Step 1: Get existing labels
  zSql = sqlite3_mprintf("SELECT labels FROM %s_nodes WHERE id = %lld", 
                        pVtab->zTableName, iNodeId);
  if (!zSql) return SQLITE_NOMEM;
  
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, NULL);
  sqlite3_free(zSql);
  if (rc != SQLITE_OK) return rc;
  
  if (sqlite3_step(pStmt) == SQLITE_ROW) {
    const char *zLabels = (const char*)sqlite3_column_text(pStmt, 0);
    if (zLabels) {
      zExistingLabels = sqlite3_mprintf("%s", zLabels);
    }
  }
  sqlite3_finalize(pStmt);
  
  // Step 2: Check if label already exists
  if (zExistingLabels) {
    char *zSearchPattern = sqlite3_mprintf("\"%s\"", zLabel);
    if (strstr(zExistingLabels, zSearchPattern)) {
      // Label already exists
      sqlite3_free(zExistingLabels);
      sqlite3_free(zSearchPattern);
      return SQLITE_OK;
    }
    sqlite3_free(zSearchPattern);
    
    // Step 3: Add new label to existing array
    // Remove closing bracket and add new label
    size_t len = strlen(zExistingLabels);
    if (len > 0 && zExistingLabels[len-1] == ']') {
      zExistingLabels[len-1] = '\0'; // Remove closing bracket
      zNewLabels = sqlite3_mprintf("%s,\"%s\"]", zExistingLabels, zLabel);
    } else {
      // Malformed JSON, rebuild array
      zNewLabels = sqlite3_mprintf("[\"%s\"]", zLabel);
    }
  } else {
    // No existing labels, create new array
    zNewLabels = sqlite3_mprintf("[\"%s\"]", zLabel);
  }
  
  if (!zNewLabels) {
    sqlite3_free(zExistingLabels);
    return SQLITE_NOMEM;
  }
  
  // Step 4: Update database
  zSql = sqlite3_mprintf("UPDATE %s_nodes SET labels = %Q WHERE id = %lld", 
                        pVtab->zTableName, zNewLabels, iNodeId);
  
  if (!zSql) {
    sqlite3_free(zExistingLabels);
    sqlite3_free(zNewLabels);
    return SQLITE_NOMEM;
  }
  
  rc = sqlite3_exec(pVtab->pDb, zSql, NULL, NULL, NULL);
  
  // Cleanup
  sqlite3_free(zExistingLabels);
  sqlite3_free(zNewLabels);
  sqlite3_free(zSql);
  
  return rc;
}

int graphRemoveNodeLabel(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                         const char *zLabel) {
  char *zSql, *zExistingLabels = NULL, *zNewLabels = NULL;
  sqlite3_stmt *pStmt;
  int rc;
  
  // Input validation
  if (!pVtab || !zLabel || iNodeId <= 0) {
    return SQLITE_MISUSE;
  }
  
  // Step 1: Get existing labels
  zSql = sqlite3_mprintf("SELECT labels FROM %s_nodes WHERE id = %lld", 
                        pVtab->zTableName, iNodeId);
  if (!zSql) return SQLITE_NOMEM;
  
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, NULL);
  sqlite3_free(zSql);
  if (rc != SQLITE_OK) return rc;
  
  if (sqlite3_step(pStmt) == SQLITE_ROW) {
    const char *zLabels = (const char*)sqlite3_column_text(pStmt, 0);
    if (zLabels) {
      zExistingLabels = sqlite3_mprintf("%s", zLabels);
    }
  }
  sqlite3_finalize(pStmt);
  
  if (!zExistingLabels) {
    return SQLITE_OK; // No labels to remove
  }
  
  // Step 2: Build new labels array without the target label
  // Parse JSON array and rebuild without target label
  char *zNewArray = sqlite3_mprintf("[");
  if (!zNewArray) {
    sqlite3_free(zExistingLabels);
    return SQLITE_NOMEM;
  }
  
  // Simple JSON parsing for label removal
  char *zCurrent = zExistingLabels;
  int bFirst = 1;
  
  while (*zCurrent) {
    if (*zCurrent == '"') {
      // Start of label
      char *zLabelStart = ++zCurrent;
      while (*zCurrent && *zCurrent != '"') zCurrent++;
      if (*zCurrent == '"') {
        size_t labelLen = zCurrent - zLabelStart;
        if (labelLen != strlen(zLabel) || strncmp(zLabelStart, zLabel, labelLen) != 0) {
          // Keep this label
          char *zTemp = sqlite3_mprintf("%s%s\"%.*s\"", 
                                       zNewArray, bFirst ? "" : ",", 
                                       (int)labelLen, zLabelStart);
          if (!zTemp) {
            sqlite3_free(zExistingLabels);
            sqlite3_free(zNewArray);
            return SQLITE_NOMEM;
          }
          sqlite3_free(zNewArray);
          zNewArray = zTemp;
          bFirst = 0;
        }
        zCurrent++;
      }
    } else {
      zCurrent++;
    }
  }
  
  // Close array
  zNewLabels = sqlite3_mprintf("%s]", zNewArray);
  sqlite3_free(zNewArray);
  
  if (!zNewLabels) {
    sqlite3_free(zExistingLabels);
    return SQLITE_NOMEM;
  }
  
  // Step 3: Update database
  zSql = sqlite3_mprintf("UPDATE %s_nodes SET labels = %Q WHERE id = %lld", 
                        pVtab->zTableName, zNewLabels, iNodeId);
  
  if (!zSql) {
    sqlite3_free(zExistingLabels);
    sqlite3_free(zNewLabels);
    return SQLITE_NOMEM;
  }
  
  rc = sqlite3_exec(pVtab->pDb, zSql, NULL, NULL, NULL);
  
  // Cleanup
  sqlite3_free(zExistingLabels);
  sqlite3_free(zNewLabels);
  sqlite3_free(zSql);
  
  return rc;
}

int graphGetNodeLabels(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                       char **pzLabels) {
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;

  zSql = sqlite3_mprintf("SELECT labels FROM %s_nodes WHERE id = %lld", pVtab->zTableName, iNodeId);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ) return rc;

  rc = sqlite3_step(pStmt);
  if( rc==SQLITE_ROW ){
    *pzLabels = sqlite3_mprintf("%s", sqlite3_column_text(pStmt, 0));
    rc = SQLITE_OK;
  } else {
    *pzLabels = 0;
    rc = SQLITE_NOTFOUND;
  }
  sqlite3_finalize(pStmt);
  return rc;
}

int graphNodeHasLabel(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                      const char *zLabel) {
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc, bHasLabel = 0;
  
  // Input validation
  if (!pVtab || !zLabel || iNodeId <= 0) {
    return 0; // Return false for invalid input
  }
  
  // Query for node labels
  zSql = sqlite3_mprintf("SELECT labels FROM %s_nodes WHERE id = %lld", 
                        pVtab->zTableName, iNodeId);
  if (!zSql) return 0;
  
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, NULL);
  sqlite3_free(zSql);
  if (rc != SQLITE_OK) return 0;
  
  if (sqlite3_step(pStmt) == SQLITE_ROW) {
    const char *zLabels = (const char*)sqlite3_column_text(pStmt, 0);
    if (zLabels) {
      // Search for label in JSON array
      char *zSearchPattern = sqlite3_mprintf("\"%s\"", zLabel);
      if (zSearchPattern) {
        bHasLabel = (strstr(zLabels, zSearchPattern) != NULL);
        sqlite3_free(zSearchPattern);
      }
    }
  }
  
  sqlite3_finalize(pStmt);
  return bHasLabel;
}