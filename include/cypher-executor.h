/*
** SQLite Graph Database Extension - Cypher Execution Engine
**
** This file contains structures and functions for executing Cypher queries
** using the Volcano iterator model. The execution engine takes optimized
** physical plans and executes them against graph data to produce results.
**
** Features:
** - Volcano iterator model for streaming execution
** - Physical operator implementations
** - Execution context and variable binding
** - Memory-efficient result processing
** - Integration with graph storage and indexing
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes
*/
#ifndef CYPHER_EXECUTOR_H
#define CYPHER_EXECUTOR_H

#include "cypher-planner.h"

/*
** Forward declarations for execution structures.
*/
typedef struct CypherExecutor CypherExecutor;
typedef struct CypherIterator CypherIterator;
typedef struct ExecutionContext ExecutionContext;
typedef struct CypherResult CypherResult;
typedef struct CypherValue CypherValue;

/*
** Cypher value types for runtime values.
** These represent the actual data types that can flow through operators.
*/
typedef enum {
  CYPHER_VALUE_NULL = 0,        /* NULL value */
  CYPHER_VALUE_BOOLEAN,         /* true/false */
  CYPHER_VALUE_INTEGER,         /* 64-bit integer */
  CYPHER_VALUE_FLOAT,           /* Double precision float */
  CYPHER_VALUE_STRING,          /* UTF-8 string */
  CYPHER_VALUE_NODE,            /* Graph node reference */
  CYPHER_VALUE_RELATIONSHIP,    /* Graph relationship reference */
  CYPHER_VALUE_PATH,            /* Graph path (sequence of nodes/rels) */
  CYPHER_VALUE_LIST,            /* List of values */
  CYPHER_VALUE_MAP              /* Map/object of key-value pairs */
} CypherValueType;

/*
** Runtime value structure.
** Represents a single value flowing through the execution pipeline.
*/
struct CypherValue {
  CypherValueType type;         /* Type of the value */
  union {
    int bBoolean;               /* Boolean value */
    sqlite3_int64 iInteger;     /* Integer value */
    double rFloat;              /* Float value */
    char *zString;              /* String value (null-terminated) */
    sqlite3_int64 iNodeId;      /* Node ID reference */
    sqlite3_int64 iRelId;       /* Relationship ID reference */
    struct {                    /* List value */
      CypherValue *apValues;
      int nValues;
    } list;
    struct {                    /* Map value */
      char **azKeys;
      CypherValue *apValues;
      int nPairs;
    } map;
  } u;
};

/*
** Result row structure.
** Represents a single row of query results with named columns.
*/
struct CypherResult {
  char **azColumnNames;         /* Column names */
  CypherValue *aValues;         /* Column values */
  int nColumns;                 /* Number of columns */
  int nColumnsAlloc;            /* Allocated column space */
};

/*
** Execution context structure.
** Manages state during query execution including variable bindings.
*/
struct ExecutionContext {
  sqlite3 *pDb;                 /* Database connection */
  GraphVtab *pGraph;            /* Graph virtual table */
  
  /* Variable bindings */
  char **azVariables;           /* Variable names */
  CypherValue *aBindings;       /* Variable values */
  int nVariables;               /* Number of variables */
  int nVariablesAlloc;          /* Allocated variable space */
  
  /* Execution state */
  int nRowsProduced;            /* Total rows produced */
  int nRowsProcessed;           /* Total rows processed */
  char *zErrorMsg;              /* Error message */
  int iErrorCode;               /* Error code */
  
  /* Memory management */
  void **apAllocated;           /* Allocated memory blocks */
  int nAllocated;               /* Number of allocated blocks */
  int nAllocatedMax;            /* Maximum allocated blocks */
};

/*
** Base iterator interface (Volcano model).
** All physical operators implement this interface.
*/
struct CypherIterator {
  /* Virtual function table */
  int (*xOpen)(CypherIterator*);                    /* Initialize iterator */
  int (*xNext)(CypherIterator*, CypherResult*);     /* Get next result row */
  int (*xClose)(CypherIterator*);                   /* Clean up iterator */
  void (*xDestroy)(CypherIterator*);                /* Destroy iterator */
  
  /* Iterator state */
  ExecutionContext *pContext;   /* Execution context */
  PhysicalPlanNode *pPlan;      /* Physical plan node */
  CypherIterator **apChildren;  /* Child iterators */
  int nChildren;                /* Number of children */
  
  /* Iterator-specific data */
  void *pIterData;              /* Iterator implementation data */
  int bOpened;                  /* Whether iterator is open */
  int bEof;                     /* Whether at end of results */
  
  /* Statistics */
  int nRowsProduced;            /* Rows produced by this iterator */
  double rCost;                 /* Actual execution cost */
};

/*
** Main Cypher executor structure.
** Manages query execution from physical plan to results.
*/
struct CypherExecutor {
  sqlite3 *pDb;                 /* Database connection */
  GraphVtab *pGraph;            /* Graph virtual table */
  ExecutionContext *pContext;   /* Execution context */
  CypherIterator *pRootIterator; /* Root iterator */
  PhysicalPlanNode *pPlan;      /* Physical execution plan */
  char *zErrorMsg;              /* Error message */
};

/*
** Executor creation and management functions.
*/

/*
** Create a new Cypher executor.
** Returns NULL on allocation failure.
*/
CypherExecutor *cypherExecutorCreate(sqlite3 *pDb, GraphVtab *pGraph);

/*
** Destroy a Cypher executor and free all associated memory.
** Safe to call with NULL pointer.
*/
void cypherExecutorDestroy(CypherExecutor *pExecutor);

/*
** Prepare an executor with a physical execution plan.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherExecutorPrepare(CypherExecutor *pExecutor, PhysicalPlanNode *pPlan);

/*
** Execute the prepared query and collect all results.
** Returns SQLITE_OK on success, error code on failure.
** Results are returned as a JSON array string.
*/
int cypherExecutorExecute(CypherExecutor *pExecutor, char **pzResults);

/*
** Get error message from executor.
** Returns NULL if no error occurred.
*/
const char *cypherExecutorGetError(CypherExecutor *pExecutor);

/*
** Execution context management functions.
*/

/*
** Create a new execution context.
** Returns NULL on allocation failure.
*/
ExecutionContext *executionContextCreate(sqlite3 *pDb, GraphVtab *pGraph);

/*
** Destroy an execution context and free all associated memory.
** Safe to call with NULL pointer.
*/
void executionContextDestroy(ExecutionContext *pContext);

/*
** Bind a variable to a value in the execution context.
** Returns SQLITE_OK on success, error code on failure.
*/
int executionContextBind(ExecutionContext *pContext, const char *zVar, CypherValue *pValue);

/*
** Get the value of a variable from the execution context.
** Returns NULL if variable not found.
*/
CypherValue *executionContextGet(ExecutionContext *pContext, const char *zVar);

/*
** Iterator creation functions.
*/

/*
** Create an iterator from a physical plan node.
** Returns NULL on allocation failure or unsupported operator.
*/
CypherIterator *cypherIteratorCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext);

/*
** Destroy an iterator and free all associated memory.
** Safe to call with NULL pointer.
*/
void cypherIteratorDestroy(CypherIterator *pIterator);

/*
** Specific iterator implementations.
*/

/*
** Create an AllNodesScan iterator.
** Scans all nodes in the graph sequentially.
*/
CypherIterator *cypherAllNodesScanCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext);

/*
** Create a LabelIndexScan iterator.
** Scans nodes with a specific label using the label index.
*/
CypherIterator *cypherLabelIndexScanCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext);

/*
** Create a PropertyIndexScan iterator.
** Scans nodes/relationships with specific property values using property indexes.
*/
CypherIterator *cypherPropertyIndexScanCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext);

/*
** Create a Filter iterator.
** Filters input rows based on predicate expressions.
*/
CypherIterator *cypherFilterCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext);

/*
** Create a Projection iterator.
** Projects (selects) specific columns from input rows.
*/
CypherIterator *cypherProjectionCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext);

/*
** Create a Sort iterator.
** Sorts input rows based on specified criteria.
*/
CypherIterator *cypherSortCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext);

/*
** Create a Limit iterator.
** Limits the number of output rows.
*/
CypherIterator *cypherLimitCreate(PhysicalPlanNode *pPlan, ExecutionContext *pContext);

/*
** Value manipulation functions.
*/

/*
** Create a new Cypher value.
** Returns NULL on allocation failure.
*/
CypherValue *cypherValueCreate(CypherValueType type);

/*
** Destroy a Cypher value and free associated memory.
** Safe to call with NULL pointer.
*/
void cypherValueDestroy(CypherValue *pValue);

/*
** Copy a Cypher value.
** Returns NULL on allocation failure.
*/
CypherValue *cypherValueCopy(CypherValue *pValue);

/*
** Initialize a CypherValue to NULL.
*/
void cypherValueInit(CypherValue *pValue);

/*
** Set a CypherValue to NULL.
*/
void cypherValueSetNull(CypherValue *pValue);

/*
** Set a CypherValue to an integer.
*/
void cypherValueSetInteger(CypherValue *pValue, sqlite3_int64 iValue);

/*
** Set a CypherValue to a float.
*/
void cypherValueSetFloat(CypherValue *pValue, double rValue);

/*
** Set string value.
** Makes a copy of the string using sqlite3_malloc().
*/
int cypherValueSetString(CypherValue *pValue, const char *zString);

/*
** Get string representation of a Cypher value.
** Caller must sqlite3_free() the returned string.
*/
char *cypherValueToString(CypherValue *pValue);

/*
** Set a CypherValue to a boolean.
*/
void cypherValueSetBoolean(CypherValue *pValue, int bValue);

/*
** Get boolean value from a CypherValue.
*/
int cypherValueGetBoolean(const CypherValue *pValue);

/*
** Get integer value from a CypherValue.
*/
sqlite3_int64 cypherValueGetInteger(const CypherValue *pValue);

/*
** Get float value from a CypherValue.
*/
double cypherValueGetFloat(const CypherValue *pValue);

/*
** Get string value from a CypherValue.
*/
const char *cypherValueGetString(const CypherValue *pValue);

/*
** Compare two CypherValues.
** Returns -1, 0, 1 for less, equal, greater or SQLITE_MISMATCH for incompatible types.
*/
int cypherValueCompare(const CypherValue *pLeft, const CypherValue *pRight);

/*
** Set a CypherValue to a list.
*/
void cypherValueSetList(CypherValue *pValue, CypherValue *apValues, int nValues);

/*
** Set a CypherValue to a map.
*/
void cypherValueSetMap(CypherValue *pValue, char **azKeys, CypherValue *apValues, int nPairs);

/*
** Check if a CypherValue is NULL.
*/
int cypherValueIsNull(const CypherValue *pValue);

/*
** Check if a CypherValue is a list.
*/
int cypherValueIsList(const CypherValue *pValue);

/*
** Check if a CypherValue is a map.
*/
int cypherValueIsMap(const CypherValue *pValue);

/*
** Set a CypherValue to a node ID.
*/
void cypherValueSetNode(CypherValue *pValue, sqlite3_int64 iNodeId);

/*
** Set a CypherValue to a relationship ID.
*/
void cypherValueSetRelationship(CypherValue *pValue, sqlite3_int64 iRelId);

/*
** Parse JSON properties string and populate a CypherValue map.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherParseJsonProperties(const char *zJson, CypherValue *pResult);

/*
** Convert a CypherValue to JSON string representation.
** Caller must sqlite3_free() the returned string.
*/
char *cypherValueToJson(const CypherValue *pValue);

/*
** Result management functions.
*/

/*
** Create a new result row.
** Returns NULL on allocation failure.
*/
CypherResult *cypherResultCreate(void);

/*
** Destroy a result row and free all associated memory.
** Safe to call with NULL pointer.
*/
void cypherResultDestroy(CypherResult *pResult);
void cypherResultClear(CypherResult *pResult);

/*
** Add a column to a result row.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherResultAddColumn(CypherResult *pResult, const char *zName, CypherValue *pValue);

/*
** Get JSON representation of a result row.
** Caller must sqlite3_free() the returned string.
*/
char *cypherResultToJson(CypherResult *pResult);

/*
** Get formatted JSON representation of a result row with indentation.
** Caller must sqlite3_free() the returned string.
*/
char *cypherResultToFormattedJson(CypherResult *pResult, int nIndent);

/*
** Convert a Cypher value to formatted JSON string with indentation.
** Caller must sqlite3_free() the returned string.
*/
char *cypherValueToFormattedJson(const CypherValue *pValue, int nIndent);

/*
** Enhanced execution functions with statistics.
*/
int cypherExecutorExecuteWithStats(CypherExecutor *pExecutor, char **pzResults, char **pzStats);

/*
** Load comprehensive sample data for Phase 3 demonstrations.
*/
int cypherLoadComprehensiveSampleData(sqlite3 *db, GraphVtab *pGraph);

/*
** Utility and debugging functions.
*/

/*
** Get string representation of value type.
** Returns static string, do not free.
*/
const char *cypherValueTypeName(CypherValueType type);

/*
** Create a test execution context for demonstration.
** Returns context with sample graph data loaded.
*/
ExecutionContext *cypherCreateTestExecutionContext(sqlite3 *pDb);

/*
** Execute a simple test query for demonstration.
** Returns JSON results, caller must sqlite3_free().
*/
char *cypherExecuteTestQuery(sqlite3 *pDb, const char *zQuery);

#endif /* CYPHER_EXECUTOR_H */