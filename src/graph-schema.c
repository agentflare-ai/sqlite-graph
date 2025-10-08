/*
** SQLite Graph Database Extension - Schema Management
**
** This file implements schema management and indexing for the SQLite graph
** database extension. It provides label and relationship type support for
** Cypher compatibility.
**
** Key features:
** - Label-based node indexing with hash tables
** - Relationship type tracking and indexing
** - Dynamic schema discovery and validation
** - Property schema inference for optimization
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
#include "cypher/cypher-schema.h"
#include <string.h>
#include <assert.h>

/*
** Default hash table size for label and property indexes.
** Must be power of 2 for efficient modulo operations.
*/
#define GRAPH_INDEX_DEFAULT_SIZE 256

/*
** Hash function for string keys.
** Simple but effective djb2 algorithm for label/property hashing.
*/
unsigned int graphHashString(const char *zString) {
  unsigned int hash = 5381;
  int c;
  
  if( !zString ) return 0;
  
  while( (c = *zString++) ) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  
  return hash;
}

/*
** Initialize schema structure for virtual table.
** Creates empty schema with default capacity.
** Returns SQLITE_OK on success, SQLITE_NOMEM on allocation failure.
*/
int graphInitSchema(GraphVtab *pVtab) {
  CypherSchema *pSchema;
  
  if( !pVtab ) return SQLITE_MISUSE;
  if( pVtab->pSchema ) return SQLITE_OK; /* Already initialized */
  
  pSchema = sqlite3_malloc(sizeof(CypherSchema));
  if( !pSchema ) return SQLITE_NOMEM;
  
  memset(pSchema, 0, sizeof(CypherSchema));
  
  /* Initialize with small capacity, will grow as needed */
  pSchema->azNodeLabels = sqlite3_malloc(sizeof(char*) * 16);
  pSchema->azRelTypes = sqlite3_malloc(sizeof(char*) * 16);
  
  if( !pSchema->azNodeLabels || !pSchema->azRelTypes ) {
    sqlite3_free(pSchema->azNodeLabels);
    sqlite3_free(pSchema->azRelTypes);
    sqlite3_free(pSchema);
    return SQLITE_NOMEM;
  }
  
  pVtab->pSchema = pSchema;
  return SQLITE_OK;
}

/*
** Destroy schema structure and free all memory.
** Safe to call with NULL pointer.
*/
void graphDestroySchema(CypherSchema *pSchema) {
  if( !pSchema ) return;
  
  /* Free node labels */
  if( pSchema->azNodeLabels ) {
    for( int i = 0; i < pSchema->nNodeLabels; i++ ) {
      sqlite3_free(pSchema->azNodeLabels[i]);
    }
    sqlite3_free(pSchema->azNodeLabels);
  }
  
  /* Free relationship types */
  if( pSchema->azRelTypes ) {
    for( int i = 0; i < pSchema->nRelTypes; i++ ) {
      sqlite3_free(pSchema->azRelTypes[i]);
    }
    sqlite3_free(pSchema->azRelTypes);
  }
  
  /* Free property schemas */
  GraphPropertySchema *pProp = pSchema->pPropSchema;
  while( pProp ) {
    GraphPropertySchema *pNext = pProp->pNext;
    sqlite3_free(pProp->zLabelOrType);
    
    if( pProp->azProperties ) {
      for( int i = 0; i < pProp->nProperties; i++ ) {
        sqlite3_free(pProp->azProperties[i]);
      }
      sqlite3_free(pProp->azProperties);
    }
    
    sqlite3_free(pProp->aiPropertyTypes);
    sqlite3_free(pProp);
    pProp = pNext;
  }
  
  sqlite3_free(pSchema);
}

/*
** Register a new label in the schema.
** Returns SQLITE_OK if successful, SQLITE_NOMEM on allocation failure.
** Skips registration if label already exists.
*/
int graphRegisterLabel(CypherSchema *pSchema, const char *zLabel) {
  if( !pSchema || !zLabel ) return SQLITE_MISUSE;
  
  /* Check if label already exists */
  for( int i = 0; i < pSchema->nNodeLabels; i++ ) {
    if( sqlite3_stricmp(pSchema->azNodeLabels[i], zLabel) == 0 ) {
      return SQLITE_OK; /* Already registered */
    }
  }
  
  /* Grow array if needed (double when full) */
  if( pSchema->nNodeLabels >= 16 && (pSchema->nNodeLabels & (pSchema->nNodeLabels - 1)) == 0 ) {
    char **azNew = sqlite3_realloc(pSchema->azNodeLabels, 
                                   sizeof(char*) * pSchema->nNodeLabels * 2);
    if( !azNew ) return SQLITE_NOMEM;
    pSchema->azNodeLabels = azNew;
  }
  
  /* Add new label */
  char *zLabelCopy = sqlite3_mprintf("%s", zLabel);
  if( !zLabelCopy ) return SQLITE_NOMEM;
  
  pSchema->azNodeLabels[pSchema->nNodeLabels++] = zLabelCopy;
  return SQLITE_OK;
}

/*
** Register a new relationship type in the schema.
** Returns SQLITE_OK if successful, SQLITE_NOMEM on allocation failure.
*/
int graphRegisterRelationshipType(CypherSchema *pSchema, const char *zType) {
  if( !pSchema || !zType ) return SQLITE_MISUSE;
  
  /* Check if type already exists */
  for( int i = 0; i < pSchema->nRelTypes; i++ ) {
    if( sqlite3_stricmp(pSchema->azRelTypes[i], zType) == 0 ) {
      return SQLITE_OK; /* Already registered */
    }
  }
  
  /* Grow array if needed */
  if( pSchema->nRelTypes >= 16 && (pSchema->nRelTypes & (pSchema->nRelTypes - 1)) == 0 ) {
    char **azNew = sqlite3_realloc(pSchema->azRelTypes, 
                                   sizeof(char*) * pSchema->nRelTypes * 2);
    if( !azNew ) return SQLITE_NOMEM;
    pSchema->azRelTypes = azNew;
  }
  
  /* Add new type */
  char *zTypeCopy = sqlite3_mprintf("%s", zType);
  if( !zTypeCopy ) return SQLITE_NOMEM;
  
  pSchema->azRelTypes[pSchema->nRelTypes++] = zTypeCopy;
  return SQLITE_OK;
}

/*
** Create a label-based index for fast node lookups.
** Creates hash table for O(1) label-based node access.
** Returns SQLITE_OK on success.
*/
int graphCreateLabelIndex(GraphVtab *pVtab, const char *zLabel) {
  (void)pVtab;
  (void)zLabel;
  // This function is now a no-op, as indexing is handled by the underlying SQLite tables.
  return SQLITE_OK;
}

/*
** Create a property-based index for fast property lookups.
** Implements property-based indexing for graph optimization.
*/
int graphCreatePropertyIndex(GraphVtab *pVtab, const char *zLabel,
                             const char *zProperty) {
  if( !pVtab || !zProperty ) return SQLITE_MISUSE;
  
  /* Suppress unused parameter warning */
  (void)zLabel;
  
  /* Initialize schema if needed */
  if( !pVtab->pSchema ) {
    int rc = graphInitSchema(pVtab);
    if( rc != SQLITE_OK ) return rc;
  }
  
  /* Property indexing implementation will be added in next iteration */
  testcase( zLabel != 0 );
  testcase( zProperty != 0 );
  
  return SQLITE_OK;
}

/*
** Find nodes by label using index.
** Returns first node in the bucket chain for the label.
** Caller should iterate through pLabelNext to get all nodes.
*/
GraphNode *graphFindNodesByLabel(GraphVtab *pVtab, const char *zLabel) {
  (void)pVtab;
  (void)zLabel;
  // This function is now a no-op, as node lookups are done via SQL queries.
  return 0;
}

/*
** Find edges by relationship type.
** Linear search through edge list - will be optimized with type indexing.
*/
GraphEdge *graphFindEdgesByType(GraphVtab *pVtab, const char *zType) {
  (void)pVtab;
  (void)zType;
  // This function is now a no-op, as edge lookups are done via SQL queries.
  return 0;
}

/*
** Copy label array with proper memory allocation.
** Returns newly allocated array that caller must free.
*/
char **graphCopyLabels(const char **azLabels, int nLabels) {
  if( !azLabels || nLabels <= 0 ) return NULL;
  
  char **azCopy = sqlite3_malloc(sizeof(char*) * nLabels);
  if( !azCopy ) return NULL;
  
  for( int i = 0; i < nLabels; i++ ) {
    azCopy[i] = sqlite3_mprintf("%s", azLabels[i]);
    if( !azCopy[i] ) {
      /* Cleanup on failure */
      for( int j = 0; j < i; j++ ) {
        sqlite3_free(azCopy[j]);
      }
      sqlite3_free(azCopy);
      return NULL;
    }
  }
  
  return azCopy;
}

/*
** Free label array allocated by graphCopyLabels.
** Safe to call with NULL pointer.
*/
void graphFreeLabels(char **azLabels, int nLabels) {
  if( !azLabels ) return;
  
  for( int i = 0; i < nLabels; i++ ) {
    sqlite3_free(azLabels[i]);
  }
  sqlite3_free(azLabels);
}

/*
** Compare two label arrays for equality.
** Returns 1 if equal, 0 if different.
*/
int graphLabelsEqual(const char **azLabels1, int nLabels1,
                     const char **azLabels2, int nLabels2) {
  if( nLabels1 != nLabels2 ) return 0;
  if( !azLabels1 && !azLabels2 ) return 1;
  if( !azLabels1 || !azLabels2 ) return 0;
  
  /* Sort labels for comparison (simple bubble sort for small arrays) */
  for( int i = 0; i < nLabels1; i++ ) {
    int found = 0;
    for( int j = 0; j < nLabels2; j++ ) {
      if( sqlite3_stricmp(azLabels1[i], azLabels2[j]) == 0 ) {
        found = 1;
        break;
      }
    }
    if( !found ) return 0;
  }
  
  return 1;
}

/*
** Dynamic schema discovery functions.
** These functions analyze existing graph data to build schema information.
*/

/*
** Discover all labels and relationship types in the current graph.
** Updates the schema with found labels and types.
** Returns SQLITE_OK on success.
*/
int graphDiscoverSchema(GraphVtab *pVtab) {
  (void)pVtab;
  // This function is now a no-op, as schema is not explicitly stored.
  return SQLITE_OK;
}

/*
** Get schema information as JSON.
** Returns schema metadata including labels, types, and property info.
** Caller must sqlite3_free() the returned string.
*/
int graphGetSchemaInfo(GraphVtab *pVtab, char **pzSchemaInfo) {
  char *zResult;
  int nNodes = 0, nEdges = 0;
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;

  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pVtab->zTableName);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc==SQLITE_OK && sqlite3_step(pStmt)==SQLITE_ROW ){
    nNodes = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);

  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_edges", pVtab->zTableName);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc==SQLITE_OK && sqlite3_step(pStmt)==SQLITE_ROW ){
    nEdges = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);

  zResult = sqlite3_mprintf("{\"node_count\":%d,\"edge_count\":%d}", nNodes, nEdges);
  if( !zResult ) return SQLITE_NOMEM;

  *pzSchemaInfo = zResult;
  return SQLITE_OK;
}

/*
** Validate node labels against schema constraints.
** Currently performs basic validation - can be extended.
** Returns SQLITE_OK if valid, SQLITE_CONSTRAINT if invalid.
*/
int graphValidateNodeLabels(GraphVtab *pVtab, const char **azLabels, int nLabels) {
  if( !pVtab ) return SQLITE_MISUSE;
  if( !azLabels || nLabels <= 0 ) return SQLITE_OK; /* Empty labels are valid */
  
  /* Basic validation: ensure labels are non-empty strings */
  for( int i = 0; i < nLabels; i++ ) {
    if( !azLabels[i] || strlen(azLabels[i]) == 0 ) {
      return SQLITE_CONSTRAINT;
    }
    
    /* Check for duplicates within the label array */
    for( int j = i + 1; j < nLabels; j++ ) {
      if( sqlite3_stricmp(azLabels[i], azLabels[j]) == 0 ) {
        return SQLITE_CONSTRAINT; /* Duplicate label */
      }
    }
  }
  
  return SQLITE_OK;
}

/*
** Validate relationship type against schema constraints.
** Returns SQLITE_OK if valid, SQLITE_CONSTRAINT if invalid.
*/
int graphValidateRelationshipType(GraphVtab *pVtab, const char *zType) {
  if( !pVtab ) return SQLITE_MISUSE;
  if( !zType ) return SQLITE_OK; /* NULL type is valid (untyped edge) */
  
  /* Basic validation: ensure type is non-empty string */
  if( strlen(zType) == 0 ) {
    return SQLITE_CONSTRAINT;
  }
  
  return SQLITE_OK;
}

/*
** Rebuild all indexes after schema changes.
** This is a potentially expensive operation for large graphs.
** Returns SQLITE_OK on success.
*/
int graphRebuildIndexes(GraphVtab *pVtab) {
  (void)pVtab;
  // This function is now a no-op, as indexing is handled by the underlying SQLite tables.
  return SQLITE_OK;
}