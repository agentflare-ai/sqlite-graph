/*
** SQLite Graph Database Extension - Cypher Write Operations
**
** This file contains the structures and function declarations for
** Cypher write operations (CREATE, MERGE, SET, DELETE). Includes
** transaction management and graph mutation support.
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes (SQLITE_OK, etc.)
** Transaction safety: All operations support SQLite transaction semantics
*/
#ifndef CYPHER_WRITE_H
#define CYPHER_WRITE_H

#include "graph.h"
#include "cypher.h"
#include "cypher-executor.h"

/*
** Write operation types for mutation tracking and rollback support.
*/
typedef enum {
    CYPHER_WRITE_CREATE_NODE = 1,     /* CREATE (n:Label {props}) */
    CYPHER_WRITE_CREATE_RELATIONSHIP, /* CREATE (a)-[r:TYPE]->(b) */
    CYPHER_WRITE_MERGE_NODE,          /* MERGE (n:Label {props}) */
    CYPHER_WRITE_MERGE_RELATIONSHIP,  /* MERGE (a)-[r:TYPE]->(b) */
    CYPHER_WRITE_SET_PROPERTY,        /* SET n.prop = value */
    CYPHER_WRITE_SET_LABEL,           /* SET n:Label */
    CYPHER_WRITE_REMOVE_PROPERTY,     /* REMOVE n.prop */
    CYPHER_WRITE_REMOVE_LABEL,        /* REMOVE n:Label */
    CYPHER_WRITE_DELETE_NODE,         /* DELETE n */
    CYPHER_WRITE_DELETE_RELATIONSHIP, /* DELETE r */
    CYPHER_WRITE_DETACH_DELETE_NODE   /* DETACH DELETE n */
} CypherWriteOpType;

/*
** Write operation record for transaction logging and rollback.
** Tracks all mutations performed during query execution.
*/
typedef struct CypherWriteOp CypherWriteOp;
struct CypherWriteOp {
    CypherWriteOpType type;           /* Operation type */
    sqlite3_int64 iNodeId;            /* Node ID (for node operations) */
    sqlite3_int64 iFromId;            /* Source node ID (for relationship ops) */
    sqlite3_int64 iToId;              /* Target node ID (for relationship ops) */
    sqlite3_int64 iRelId;             /* Relationship ID */
    char *zProperty;                  /* Property name */
    char *zLabel;                     /* Label name */
    char *zRelType;                   /* Relationship type */
    CypherValue *pOldValue;           /* Previous value (for rollback) */
    CypherValue *pNewValue;           /* New value */
    char *zOldLabels;                 /* Previous labels JSON (for rollback) */
    char *zNewLabels;                 /* New labels JSON */
    CypherWriteOp *pNext;             /* Next operation in transaction */
};

/*
** Write transaction context for managing mutations and rollback.
** Maintains operation log and provides transaction semantics.
*/
typedef struct CypherWriteContext CypherWriteContext;
struct CypherWriteContext {
    sqlite3 *pDb;                     /* Database connection */
    GraphVtab *pGraph;                /* Graph virtual table */
    ExecutionContext *pExecContext;   /* Execution context */
    CypherWriteOp *pOperations;       /* Linked list of operations */
    CypherWriteOp *pLastOp;           /* Last operation for efficient appending */
    int nOperations;                  /* Number of operations */
    int bInTransaction;               /* Whether in explicit transaction */
    int bAutoCommit;                  /* Whether to auto-commit changes */
    char *zErrorMsg;                  /* Error message */
    sqlite3_int64 iNextNodeId;        /* Next available node ID */
    sqlite3_int64 iNextRelId;         /* Next available relationship ID */
};

/*
** CREATE operation structures for node and relationship creation.
*/
typedef struct CreateNodeOp CreateNodeOp;
struct CreateNodeOp {
    char *zVariable;                  /* Variable name for created node */
    char **azLabels;                  /* Array of label names */
    int nLabels;                      /* Number of labels */
    char **azPropNames;               /* Property names */
    CypherValue **apPropValues;       /* Property values */
    int nProperties;                  /* Number of properties */
    sqlite3_int64 iCreatedNodeId;     /* ID of created node (output) */
};

typedef struct CreateRelOp CreateRelOp;
struct CreateRelOp {
    char *zFromVar;                   /* Source node variable */
    char *zToVar;                     /* Target node variable */
    char *zRelVar;                    /* Relationship variable */
    char *zRelType;                   /* Relationship type */
    char **azPropNames;               /* Property names */
    CypherValue **apPropValues;       /* Property values */
    int nProperties;                  /* Number of properties */
    sqlite3_int64 iFromNodeId;        /* Source node ID */
    sqlite3_int64 iToNodeId;          /* Target node ID */
    sqlite3_int64 iCreatedRelId;      /* ID of created relationship (output) */
};

/*
** MERGE operation structures for conditional creation.
*/
typedef struct MergeNodeOp MergeNodeOp;
struct MergeNodeOp {
    char *zVariable;                  /* Variable name */
    char **azLabels;                  /* Labels to match/create */
    int nLabels;                      /* Number of labels */
    char **azMatchProps;              /* Properties to match on */
    CypherValue **apMatchValues;      /* Values to match */
    int nMatchProps;                  /* Number of match properties */
    char **azOnCreateProps;           /* Properties to set on creation */
    CypherValue **apOnCreateValues;   /* Values to set on creation */
    int nOnCreateProps;               /* Number of creation properties */
    char **azOnMatchProps;            /* Properties to set on match */
    CypherValue **apOnMatchValues;    /* Values to set on match */
    int nOnMatchProps;                /* Number of match update properties */
    sqlite3_int64 iNodeId;            /* Matched/created node ID (output) */
    int bWasCreated;                  /* Whether node was created (output) */
};

/*
** SET operation structures for property and label updates.
*/
typedef struct SetPropertyOp SetPropertyOp;
struct SetPropertyOp {
    char *zVariable;                  /* Variable name */
    char *zProperty;                  /* Property name */
    CypherValue *pValue;              /* New value */
    sqlite3_int64 iNodeId;            /* Target node ID */
};

typedef struct SetLabelOp SetLabelOp;
struct SetLabelOp {
    char *zVariable;                  /* Variable name */
    char **azLabels;                  /* Labels to add */
    int nLabels;                      /* Number of labels */
    sqlite3_int64 iNodeId;            /* Target node ID */
};

/*
** DELETE operation structures for node and relationship deletion.
*/
typedef struct DeleteOp DeleteOp;
struct DeleteOp {
    char *zVariable;                  /* Variable name */
    int bDetach;                      /* Whether to detach delete */
    sqlite3_int64 iNodeId;            /* Node ID (if deleting node) */
    sqlite3_int64 iRelId;             /* Relationship ID (if deleting rel) */
    int bIsNode;                      /* Whether deleting node vs relationship */
};

/*
** Write operation iterator for executing mutations.
** Extends the base iterator model for write operations.
*/
typedef struct CypherWriteIterator CypherWriteIterator;
struct CypherWriteIterator {
    CypherIterator base;              /* Base iterator interface */
    CypherWriteContext *pWriteCtx;    /* Write transaction context */
    void *pOperationData;             /* Operation-specific data */
    int (*xExecute)(CypherWriteIterator*, CypherResult*); /* Execute function */
};

/*
** Write context management functions.
*/

/*
** Create a new write context for mutation operations.
** Returns NULL on allocation failure.
*/
CypherWriteContext *cypherWriteContextCreate(sqlite3 *pDb, GraphVtab *pGraph,
                                           ExecutionContext *pExecContext);

/*
** Destroy a write context and free all associated memory.
** Automatically rolls back any uncommitted operations.
*/
void cypherWriteContextDestroy(CypherWriteContext *pCtx);

/*
** Begin a write transaction in the context.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherWriteContextBegin(CypherWriteContext *pCtx);

/*
** Commit all operations in the write context.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherWriteContextCommit(CypherWriteContext *pCtx);

/*
** Rollback all operations in the write context.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherWriteContextRollback(CypherWriteContext *pCtx);

/*
** Add a write operation to the transaction log.
** Returns SQLITE_OK on success, SQLITE_NOMEM on allocation failure.
*/
int cypherWriteContextAddOperation(CypherWriteContext *pCtx, CypherWriteOp *pOp);

/*
** Get the next available node ID from the context.
** Updates the context's next ID counter.
*/
sqlite3_int64 cypherWriteContextNextNodeId(CypherWriteContext *pCtx);

/*
** Get the next available relationship ID from the context.
** Updates the context's next ID counter.
*/
sqlite3_int64 cypherWriteContextNextRelId(CypherWriteContext *pCtx);

/*
** Execute all pending write operations in the context.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherExecuteOperations(CypherWriteContext *pCtx);

/*
** Rollback all pending write operations in the context.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherRollbackOperations(CypherWriteContext *pCtx);

/*
** CREATE operation functions.
*/

/*
** Execute a CREATE node operation.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherCreateNode(CypherWriteContext *pCtx, CreateNodeOp *pOp);

/*
** Execute a CREATE relationship operation.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherCreateRelationship(CypherWriteContext *pCtx, CreateRelOp *pOp);

/*
** Create a CREATE node iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherCreateNodeIteratorCreate(CypherWriteContext *pCtx,
                                                  CreateNodeOp *pOp);

/*
** Create a CREATE relationship iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherCreateRelIteratorCreate(CypherWriteContext *pCtx,
                                                 CreateRelOp *pOp);

/*
** MERGE operation functions.
*/

/*
** Execute a MERGE node operation.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherMergeNode(CypherWriteContext *pCtx, MergeNodeOp *pOp);

/*
** Create a MERGE node iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherMergeNodeIteratorCreate(CypherWriteContext *pCtx,
                                                 MergeNodeOp *pOp);

/*
** SET operation functions.
*/

/*
** Execute a SET property operation.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherSetProperty(CypherWriteContext *pCtx, SetPropertyOp *pOp);

/*
** Execute a SET label operation.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherSetLabel(CypherWriteContext *pCtx, SetLabelOp *pOp);

/*
** Create a SET property iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherSetPropertyIteratorCreate(CypherWriteContext *pCtx,
                                                   SetPropertyOp *pOp);

/*
** Create a SET label iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherSetLabelIteratorCreate(CypherWriteContext *pCtx,
                                                SetLabelOp *pOp);

/*
** DELETE operation functions.
*/

/*
** Execute a DELETE operation.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherDelete(CypherWriteContext *pCtx, DeleteOp *pOp);

/*
** Create a DELETE iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherDeleteIteratorCreate(CypherWriteContext *pCtx,
                                              DeleteOp *pOp);

/*
** Utility functions for write operations.
*/

/*
** Validate that a node exists before creating relationships.
** Returns SQLITE_OK if node exists, SQLITE_ERROR if not found.
*/
int cypherValidateNodeExists(CypherWriteContext *pCtx, sqlite3_int64 iNodeId);

/*
** Check if a node matches the given labels and properties.
** Returns 1 if match, 0 if no match, -1 on error.
*/
int cypherNodeMatches(CypherWriteContext *pCtx, sqlite3_int64 iNodeId,
                     char **azLabels, int nLabels,
                     char **azProps, CypherValue **apValues, int nProps);

/*
** Find a node that matches the given criteria.
** Returns node ID if found, 0 if not found, -1 on error.
*/
sqlite3_int64 cypherFindMatchingNode(CypherWriteContext *pCtx,
                                    char **azLabels, int nLabels,
                                    char **azProps, CypherValue **apValues, int nProps);

/*
** Get all relationships connected to a node (for DETACH DELETE).
** Returns JSON array of relationship IDs, caller must sqlite3_free().
*/
char *cypherGetNodeRelationships(CypherWriteContext *pCtx, sqlite3_int64 iNodeId);

/*
** Storage bridge functions - connect Cypher operations to graph storage.
*/

/*
** Add a node to the graph storage.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherStorageAddNode(GraphVtab *pGraph, sqlite3_int64 iNodeId, 
                        const char **azLabels, int nLabels, 
                        const char *zProperties);

/*
** Execute an SQL update statement on the graph storage.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherStorageExecuteUpdate(GraphVtab *pGraph, const char *zSql, 
                               sqlite3_int64 *pRowId);

/*
** Add an edge (relationship) to the graph storage.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherStorageAddEdge(GraphVtab *pGraph, sqlite3_int64 iEdgeId,
                        sqlite3_int64 iFromId, sqlite3_int64 iToId,
                        const char *zType, double rWeight, 
                        const char *zProperties);

/*
** Update properties of a node or relationship.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherStorageUpdateProperties(GraphVtab *pGraph, sqlite3_int64 iNodeId,
                                 sqlite3_int64 iEdgeId, const char *zProperty, 
                                 const CypherValue *pValue);

/*
** Delete a node from the graph storage.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherStorageDeleteNode(GraphVtab *pGraph, sqlite3_int64 iNodeId, int bDetach);

/*
** Delete a relationship from the graph storage.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherStorageDeleteEdge(GraphVtab *pGraph, sqlite3_int64 iEdgeId);

/*
** Check if a node exists in the graph storage.
** Returns 1 if exists, 0 if not, negative on error.
*/
int cypherStorageNodeExists(GraphVtab *pGraph, sqlite3_int64 iNodeId);

/*
** Get the next available node/edge IDs.
** Returns next ID or negative on error.
*/
sqlite3_int64 cypherStorageGetNextNodeId(GraphVtab *pGraph);
sqlite3_int64 cypherStorageGetNextEdgeId(GraphVtab *pGraph);

/*
** Write operation memory management.
*/

/*
** Create a write operation record.
** Returns NULL on allocation failure.
*/
CypherWriteOp *cypherWriteOpCreate(CypherWriteOpType type);

/*
** Destroy a write operation and free all associated memory.
** Safe to call with NULL pointer.
*/
void cypherWriteOpDestroy(CypherWriteOp *pOp);

/*
** Create a CREATE node operation structure.
** Returns NULL on allocation failure.
*/
CreateNodeOp *cypherCreateNodeOpCreate(void);

/*
** Destroy a CREATE node operation.
** Safe to call with NULL pointer.
*/
void cypherCreateNodeOpDestroy(CreateNodeOp *pOp);

/*
** Create a CREATE relationship operation structure.
** Returns NULL on allocation failure.
*/
CreateRelOp *cypherCreateRelOpCreate(void);

/*
** Destroy a CREATE relationship operation.
** Safe to call with NULL pointer.
*/
void cypherCreateRelOpDestroy(CreateRelOp *pOp);

/*
** Create a MERGE node operation structure.
** Returns NULL on allocation failure.
*/
MergeNodeOp *cypherMergeNodeOpCreate(void);

/*
** Destroy a MERGE node operation.
** Safe to call with NULL pointer.
*/
void cypherMergeNodeOpDestroy(MergeNodeOp *pOp);

/*
** Create a SET property operation structure.
** Returns NULL on allocation failure.
*/
SetPropertyOp *cypherSetPropertyOpCreate(void);

/*
** Destroy a SET property operation.
** Safe to call with NULL pointer.
*/
void cypherSetPropertyOpDestroy(SetPropertyOp *pOp);

/*
** Create a SET label operation structure.
** Returns NULL on allocation failure.
*/
SetLabelOp *cypherSetLabelOpCreate(void);

/*
** Destroy a SET label operation.
** Safe to call with NULL pointer.
*/
void cypherSetLabelOpDestroy(SetLabelOp *pOp);

/*
** Create a DELETE operation structure.
** Returns NULL on allocation failure.
*/
DeleteOp *cypherDeleteOpCreate(void);

/*
** Destroy a DELETE operation.
** Safe to call with NULL pointer.
*/
void cypherDeleteOpDestroy(DeleteOp *pOp);

/*
** SQL Function Interface for Write Operations
**
** These functions register SQL functions that expose Cypher write
** capabilities to SQLite users through function calls.
*/

/*
** Register all Cypher write operation SQL functions with the database.
** Should be called during extension initialization.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherRegisterWriteSqlFunctions(sqlite3 *db);

#endif /* CYPHER_WRITE_H */