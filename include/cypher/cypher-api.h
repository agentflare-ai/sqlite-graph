/*
** SQLite Graph Database Extension - Cypher API
**
** This file contains the public API for Cypher query functionality.
** Includes functions for enhanced storage with labels and types,
** as well as utility functions for Cypher operations.
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes (SQLITE_OK, etc.)
** Thread safety: Extension supports SQLite threading modes
*/
#ifndef CYPHER_API_H
#define CYPHER_API_H

#include "sqlite3.h"

/*
** Forward declarations
*/
typedef struct GraphVtab GraphVtab;
typedef struct GraphNode GraphNode;
typedef struct GraphEdge GraphEdge;
typedef struct CypherAST CypherAST;

/*
** Enhanced storage functions with label and type support.
** These extend the existing API with Cypher graph concepts.
*/

/*
** Add a node with labels to the graph.
** azLabels: Array of label strings (can be NULL for no labels)
** nLabels: Number of labels in array
*/
int cypherAddNodeWithLabels(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                            const char **azLabels, int nLabels,
                            const char *zProperties);

/*
** Add an edge with relationship type to the graph.
** zType: Relationship type string (can be NULL for untyped edge)
*/
int cypherAddEdgeWithType(GraphVtab *pVtab, sqlite3_int64 iFromId,
                          sqlite3_int64 iToId, const char *zType,
                          double rWeight, const char *zProperties);

/*
** Set or update labels for an existing node.
** Replaces existing labels with new ones.
*/
int cypherSetNodeLabels(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                        const char **azLabels, int nLabels);

/*
** Add a single label to an existing node.
** Does not replace existing labels.
*/
int cypherAddNodeLabel(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                       const char *zLabel);

/*
** Remove a specific label from a node.
*/
int cypherRemoveNodeLabel(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                          const char *zLabel);

/*
** Get all labels for a node as a JSON array.
** Caller must sqlite3_free() the returned string.
*/
int cypherGetNodeLabels(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                        char **pzLabels);

/*
** Check if a node has a specific label.
** Returns 1 if node has label, 0 if not, -1 on error.
*/
int cypherNodeHasLabel(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                       const char *zLabel);

/*
** Cypher query execution functions.
*/

/*
** Parse a Cypher query string into an AST.
** Returns SQLITE_OK on success, error code on failure.
** Caller must free the returned AST with cypherFreeAST().
*/
int cypherParseQuery(const char *zQuery, CypherAST **ppAST);

/*
** Execute a parsed Cypher query.
** Returns SQLITE_OK on success, error code on failure.
** Results are returned through the callback function.
*/
int cypherExecuteAST(GraphVtab *pVtab, CypherAST *pAST,
                     int (*xCallback)(void*, int, char**, char**),
                     void *pArg);

/*
** Execute a Cypher query string directly.
** Combines parsing and execution in one call.
*/
int cypherExecuteQuery(GraphVtab *pVtab, const char *zQuery,
                       int (*xCallback)(void*, int, char**, char**),
                       void *pArg);

/*
** Free an AST allocated by cypherParseQuery().
*/
void cypherFreeAST(CypherAST *pAST);

/*
** Cypher transaction support.
*/

/*
** Begin a write transaction for Cypher operations.
** Required for CREATE, SET, DELETE, MERGE operations.
*/
int cypherBeginWrite(GraphVtab *pVtab);

/*
** Commit a write transaction.
*/
int cypherCommitWrite(GraphVtab *pVtab);

/*
** Rollback a write transaction.
*/
int cypherRollbackWrite(GraphVtab *pVtab);

/*
** Check if currently in a write transaction.
** Returns 1 if in write transaction, 0 otherwise.
*/
int cypherInWriteTransaction(GraphVtab *pVtab);

/*
** Cypher query validation and planning.
*/

/*
** Validate a Cypher query without executing it.
** Returns SQLITE_OK if valid, error code with details if invalid.
*/
int cypherValidateQuery(const char *zQuery, char **pzError);

/*
** Get query execution plan for a Cypher query.
** Returns plan as JSON string, caller must sqlite3_free().
*/
int cypherGetQueryPlan(GraphVtab *pVtab, const char *zQuery, char **pzPlan);

/*
** Estimate query cost for optimization.
** Returns estimated execution cost as double.
*/
double cypherEstimateQueryCost(GraphVtab *pVtab, const char *zQuery);

/*
** Cypher built-in functions.
*/

/*
** Get the ID of a node.
*/
sqlite3_int64 cypherNodeId(GraphNode *pNode);

/*
** Get the type of a relationship.
*/
const char *cypherRelationshipType(GraphEdge *pEdge);

/*
** Get node degree (total number of connections).
*/
int cypherNodeDegree(GraphVtab *pVtab, sqlite3_int64 iNodeId);

/*
** Get node in-degree (number of incoming connections).
*/
int cypherNodeInDegree(GraphVtab *pVtab, sqlite3_int64 iNodeId);

/*
** Get node out-degree (number of outgoing connections).
*/
int cypherNodeOutDegree(GraphVtab *pVtab, sqlite3_int64 iNodeId);

/*
** Path and traversal functions.
*/

/*
** Find shortest path between two nodes.
** Returns path as JSON array, caller must sqlite3_free().
*/
int cypherShortestPath(GraphVtab *pVtab, sqlite3_int64 iFromId,
                       sqlite3_int64 iToId, char **pzPath);

/*
** Find all paths between two nodes up to maximum length.
** Returns paths as JSON array, caller must sqlite3_free().
*/
int cypherAllPaths(GraphVtab *pVtab, sqlite3_int64 iFromId,
                   sqlite3_int64 iToId, int nMaxLength, char **pzPaths);

/*
** Expand from a node following relationships.
** Returns expanded nodes as JSON array, caller must sqlite3_free().
*/
int cypherExpand(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                 const char *zRelType, int nMaxHops, char **pzNodes);

/*
** Pattern matching functions.
*/

/*
** Match nodes by label and properties.
** Returns matching nodes as JSON array, caller must sqlite3_free().
*/
int cypherMatchNodes(GraphVtab *pVtab, const char *zLabel,
                     const char *zProperties, char **pzNodes);

/*
** Match relationships by type and properties.
** Returns matching relationships as JSON array, caller must sqlite3_free().
*/
int cypherMatchRelationships(GraphVtab *pVtab, const char *zType,
                             const char *zProperties, char **pzRels);

/*
** Match complex patterns.
** Returns matching patterns as JSON array, caller must sqlite3_free().
*/
int cypherMatchPattern(GraphVtab *pVtab, const char *zPattern, char **pzResults);

/*
** Utility functions.
*/

/*
** Convert Cypher value to JSON string.
** Caller must sqlite3_free() the returned string.
*/
char *cypherValueToJSON(const char *zValue);

/*
** Convert JSON string to Cypher value.
** Caller must sqlite3_free() the returned string.
*/
char *cypherJSONToValue(const char *zJSON);

/*
** Escape string for use in Cypher queries.
** Caller must sqlite3_free() the returned string.
*/
char *cypherEscapeString(const char *zString);

/*
** Format error message for Cypher operations.
** Caller must sqlite3_free() the returned string.
*/
char *cypherFormatError(int iErrorCode, const char *zContext);

#endif /* CYPHER_API_H */

/*
** Validate a Cypher query without executing it.
** Returns SQLITE_OK if valid, error code with details if invalid.
** This is needed for TCK compliance testing.
*/
int cypherValidateQuery(const char *zQuery, char **pzError);
