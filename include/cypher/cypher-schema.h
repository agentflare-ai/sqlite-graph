/*
** SQLite Graph Database Extension - Cypher Schema Definitions
**
** This file contains schema-related structures and functions for the
** Cypher query engine. Handles label tracking, relationship types,
** and property schemas.
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes (SQLITE_OK, etc.)
** Thread safety: Extension supports SQLite threading modes
*/
#ifndef CYPHER_SCHEMA_H
#define CYPHER_SCHEMA_H

#include "sqlite3.h"

/*
** Forward declarations
*/
typedef struct CypherSchema CypherSchema;
typedef struct GraphIndex GraphIndex;
typedef struct GraphPropertySchema GraphPropertySchema;

/*
** Cypher schema tracking for labels and relationship types.
** Maintains metadata about graph structure for optimization.
*/
struct CypherSchema {
  char **azNodeLabels;    /* Array of known node labels */
  int nNodeLabels;        /* Number of node labels */
  char **azRelTypes;      /* Array of known relationship types */
  int nRelTypes;          /* Number of relationship types */
  GraphPropertySchema *pPropSchema; /* Property schemas by label/type */
};

/*
** Index structures for efficient pattern matching.
** Hash-based indexing for O(1) label and property lookups.
*/
struct GraphIndex {
  char *zIndexName;       /* Index name */
  int iIndexType;         /* 0=label, 1=property, 2=composite */
  void **apNodes;         /* Hash table of nodes */
  int nBuckets;           /* Number of hash buckets */
  int nEntries;           /* Number of indexed entries */
};

/*
** Property schema information for type inference and optimization.
** Tracks property types and frequency for each label/relationship type.
*/
struct GraphPropertySchema {
  char *zLabelOrType;     /* Label or relationship type name */
  char **azProperties;    /* Array of property names */
  int *aiPropertyTypes;   /* Array of SQLite type codes */
  int nProperties;        /* Number of properties */
  GraphPropertySchema *pNext; /* Next schema in linked list */
};

/*
** Schema management functions.
*/

/*
** Create or get schema structure for virtual table.
** Initializes schema tracking if not already present.
*/
int cypherInitSchema(struct GraphVtab *pVtab);

/*
** Destroy schema structure and free all memory.
*/
void cypherDestroySchema(CypherSchema *pSchema);

/*
** Register a new label in the schema.
** Returns SQLITE_OK if successful.
*/
int cypherRegisterLabel(CypherSchema *pSchema, const char *zLabel);

/*
** Register a new relationship type in the schema.
*/
int cypherRegisterRelationshipType(CypherSchema *pSchema, const char *zType);

/*
** Index management functions.
*/

/*
** Create a label-based index for fast node lookups.
** zLabel: Label to index (NULL for all labels)
*/
int cypherCreateLabelIndex(struct GraphVtab *pVtab, const char *zLabel);

/*
** Create a property-based index for fast property lookups.
** zLabel: Label to index properties for (NULL for all nodes)
** zProperty: Property name to index
*/
int cypherCreatePropertyIndex(struct GraphVtab *pVtab, const char *zLabel,
                              const char *zProperty);

/*
** Find nodes by label using index.
** Returns linked list of nodes with specified label.
*/
struct GraphNode *cypherFindNodesByLabel(struct GraphVtab *pVtab, const char *zLabel);

/*
** Find edges by relationship type.
** Returns linked list of edges with specified type.
*/
struct GraphEdge *cypherFindEdgesByType(struct GraphVtab *pVtab, const char *zType);

/*
** Utility functions for label and type operations.
*/

/*
** Hash function for label and property indexing.
** Simple but effective hash for string keys.
*/
unsigned int cypherHashString(const char *zString);

/*
** Compare two label arrays for equality.
** Returns 1 if equal, 0 if different.
*/
int cypherLabelsEqual(const char **azLabels1, int nLabels1,
                      const char **azLabels2, int nLabels2);

/*
** Copy label array with proper memory allocation.
** Caller must free returned array and strings.
*/
char **cypherCopyLabels(const char **azLabels, int nLabels);

/*
** Free label array allocated by cypherCopyLabels.
*/
void cypherFreeLabels(char **azLabels, int nLabels);

/*
** Dynamic schema discovery and validation functions.
*/

/*
** Discover all labels and relationship types in the current graph.
** Updates the schema with found labels and types automatically.
*/
int cypherDiscoverSchema(struct GraphVtab *pVtab);

/*
** Get schema information as JSON.
** Returns schema metadata including labels, types, and statistics.
** Caller must sqlite3_free() the returned string.
*/
int cypherGetSchemaInfo(struct GraphVtab *pVtab, char **pzSchemaInfo);

/*
** Validate node labels against schema constraints.
** Returns SQLITE_OK if valid, SQLITE_CONSTRAINT if invalid.
*/
int cypherValidateNodeLabels(struct GraphVtab *pVtab, const char **azLabels, int nLabels);

/*
** Validate relationship type against schema constraints.
** Returns SQLITE_OK if valid, SQLITE_CONSTRAINT if invalid.
*/
int cypherValidateRelationshipType(struct GraphVtab *pVtab, const char *zType);

/*
** Rebuild all indexes after schema changes.
** This is a potentially expensive operation for large graphs.
*/
int cypherRebuildIndexes(struct GraphVtab *pVtab);

#endif /* CYPHER_SCHEMA_H */
