/*
** SQLite Graph Database Extension - Cypher Query Planner
**
** This file contains structures and functions for compiling Cypher ASTs
** into optimized logical and physical query execution plans. The planner
** follows standard database query optimization techniques.
**
** Features:
** - AST to logical plan compilation
** - Pattern optimization and rewriting
** - Cost-based physical plan selection
** - Index utilization planning
** - Join ordering optimization
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes
*/
#ifndef CYPHER_PLANNER_H
#define CYPHER_PLANNER_H

#include "cypher.h"
#include "graph-vtab.h"


/*
** Logical plan node types representing different query operations.
** These form the intermediate representation between AST and physical operators.
*/
typedef enum {
  /* Scan Operations */
  LOGICAL_NODE_SCAN = 1,        /* Scan all nodes */
  LOGICAL_LABEL_SCAN,           /* Scan nodes by label */
  LOGICAL_INDEX_SCAN,           /* Scan using property index */
  LOGICAL_RELATIONSHIP_SCAN,    /* Scan all relationships */
  LOGICAL_TYPE_SCAN,           /* Scan relationships by type */
  
  /* Pattern Operations */
  LOGICAL_EXPAND,              /* Expand from node along relationships */
  LOGICAL_VAR_LENGTH_EXPAND,   /* Variable-length path expansion */
  LOGICAL_OPTIONAL_EXPAND,     /* Optional pattern matching */
  
  /* Filter Operations */
  LOGICAL_FILTER,              /* WHERE clause filtering */
  LOGICAL_PROPERTY_FILTER,     /* Property-based filtering */
  LOGICAL_LABEL_FILTER,        /* Label-based filtering */
  
  /* Join Operations */
  LOGICAL_HASH_JOIN,           /* Hash-based join */
  LOGICAL_NESTED_LOOP_JOIN,    /* Nested loop join */
  LOGICAL_CARTESIAN_PRODUCT,   /* Cartesian product */
  
  /* Projection Operations */
  LOGICAL_PROJECTION,          /* SELECT/RETURN columns */
  LOGICAL_DISTINCT,            /* DISTINCT modifier */
  LOGICAL_AGGREGATION,         /* GROUP BY and aggregates */
  
  /* Ordering Operations */
  LOGICAL_SORT,                /* ORDER BY */
  LOGICAL_LIMIT,               /* LIMIT clause */
  LOGICAL_SKIP,                /* SKIP clause */
  
  /* Mutation Operations */
  LOGICAL_CREATE,              /* CREATE nodes/relationships */
  LOGICAL_MERGE,               /* MERGE operation */
  LOGICAL_SET,                 /* SET properties */
  LOGICAL_DELETE,              /* DELETE nodes/relationships */
  LOGICAL_DETACH_DELETE        /* DETACH DELETE with cascade */
} LogicalPlanNodeType;

/*
** Physical operator types for actual execution.
** Each logical operator can map to multiple physical implementations.
*/
typedef enum {
  /* Scan Operators */
  PHYSICAL_ALL_NODES_SCAN = 1, /* Sequential scan of all nodes */
  PHYSICAL_LABEL_INDEX_SCAN,   /* Use label index for scanning */
  PHYSICAL_PROPERTY_INDEX_SCAN, /* Use property index for scanning */
  PHYSICAL_ALL_RELS_SCAN,      /* Sequential scan of all relationships */
  PHYSICAL_TYPE_INDEX_SCAN,    /* Use type index for relationships */

  /* Join Operators */
  PHYSICAL_HASH_JOIN,          /* In-memory hash join */
  PHYSICAL_NESTED_LOOP_JOIN,   /* Nested loop with outer/inner tables */
  PHYSICAL_INDEX_NESTED_LOOP,  /* Index-assisted nested loop */

  /* Other Operators */
  PHYSICAL_FILTER,             /* Predicate evaluation */
  PHYSICAL_PROJECTION,         /* Column projection */
  PHYSICAL_SORT,               /* External sorting */
  PHYSICAL_LIMIT,              /* Result limiting */
  PHYSICAL_AGGREGATION,        /* Grouping and aggregation */

  /* Mutation Operators */
  PHYSICAL_CREATE,             /* Create nodes/relationships */
  PHYSICAL_MERGE,              /* Merge (match or create) */
  PHYSICAL_SET,                /* Set properties */
  PHYSICAL_DELETE,             /* Delete nodes/relationships */
  PHYSICAL_REMOVE              /* Remove properties/labels */
} PhysicalOperatorType;

/*
** Logical plan node structure.
** Forms a tree representing the logical query structure.
*/
typedef struct LogicalPlanNode {
  LogicalPlanNodeType type;     /* Type of logical operation */
  char *zAlias;                 /* Variable name/alias */
  char *zLabel;                 /* Node label (for scans) */
  char *zProperty;              /* Property name (for filters/indexes) */
  char *zValue;                 /* Literal value (for filters) */
  
  /* Child operations */
  struct LogicalPlanNode **apChildren;
  int nChildren;
  int nChildrenAlloc;
  
  /* Parent operation (for upward traversal) */
  struct LogicalPlanNode *pParent;
  
  /* Cost estimation */
  double rEstimatedCost;        /* Estimated execution cost */
  sqlite3_int64 iEstimatedRows; /* Estimated result rows */
  
  /* Additional metadata */
  int iFlags;                   /* Operation flags */
  void *pExtra;                 /* Type-specific extra data */
} LogicalPlanNode;

/*
** Physical execution plan node.
** Contains specific operator implementation details.
*/
typedef struct PhysicalPlanNode {
  PhysicalOperatorType type;    /* Physical operator type */
  char *zAlias;                 /* Variable name/alias */
  
  /* Operator-specific parameters */
  char *zIndexName;             /* Index to use (if any) */
  char *zLabel;                 /* Label for scans */
  char *zProperty;              /* Property for filters/indexes */
  char *zValue;                 /* Filter value */
  
  /* Child operators */
  struct PhysicalPlanNode **apChildren;
  struct PhysicalPlanNode *pChild;     /* Primary child (for single-child operators) */
  int nChildren;
  int nChildrenAlloc;
  
  /* Filter and projection expressions */
  struct CypherExpression *pFilterExpr;      /* Filter expression */
  struct CypherExpression **apProjections;   /* Projection expressions */
  int nProjections;                          /* Number of projections */
  
  /* Sort and limit parameters */
  struct CypherExpression **apSortKeys;      /* Sort key expressions */
  int nSortKeys;                             /* Number of sort keys */
  int nLimit;                                /* LIMIT value */
  
  /* Cost and statistics */
  double rCost;                 /* Actual estimated cost */
  sqlite3_int64 iRows;          /* Estimated output rows */
  double rSelectivity;          /* Filter selectivity */
  
  /* Execution state (used during execution) */
  void *pExecState;             /* Operator execution state */
  int iFlags;                   /* Execution flags */
} PhysicalPlanNode;

/*
** Query plan compilation context.
** Tracks state during plan compilation and optimization.
*/
typedef struct PlanContext {
  sqlite3 *pDb;                 /* Database connection */
  GraphVtab *pGraph;            /* Graph virtual table */
  CypherAst *pAst;              /* Original AST */
  
  /* Symbol table for variables */
  char **azVariables;           /* Variable names */
  LogicalPlanNode **apVarNodes; /* Nodes producing variables */
  int nVariables;
  int nVariablesAlloc;
  
  /* Available indexes */
  char **azLabelIndexes;        /* Available label indexes */
  char **azPropertyIndexes;     /* Available property indexes */
  int nLabelIndexes;
  int nPropertyIndexes;
  
  /* Optimization settings */
  int bUseIndexes;              /* Enable index usage */
  int bReorderJoins;            /* Enable join reordering */
  double rIndexCostFactor;      /* Index vs scan cost factor */
  
  /* Error tracking */
  char *zErrorMsg;              /* Error message */
  int nErrors;                  /* Error count */
} PlanContext;

/*
** Main query planner structure.
** Manages the compilation from AST to physical plan.
*/
typedef struct CypherPlanner {
  sqlite3 *pDb;                 /* Database connection */
  PlanContext *pContext;        /* Current planning context */
  LogicalPlanNode *pLogicalPlan; /* Compiled logical plan */
  PhysicalPlanNode *pPhysicalPlan; /* Optimized physical plan */
  char *zErrorMsg;              /* Error message */
} CypherPlanner;

/*
** Planner creation and management functions.
*/

/*
** Create a new Cypher query planner.
** Returns NULL on allocation failure.
*/
CypherPlanner *cypherPlannerCreate(sqlite3 *pDb, GraphVtab *pGraph);

/*
** Destroy a Cypher planner and free all associated memory.
** Safe to call with NULL pointer.
*/
void cypherPlannerDestroy(CypherPlanner *pPlanner);

/*
** Compile an AST into a logical plan.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherPlannerCompile(CypherPlanner *pPlanner, CypherAst *pAst);

/*
** Optimize the logical plan and generate physical plan.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherPlannerOptimize(CypherPlanner *pPlanner);

/*
** Get the final physical execution plan.
** Returns NULL if planning failed or not yet completed.
*/
PhysicalPlanNode *cypherPlannerGetPlan(CypherPlanner *pPlanner);

/*
** Get error message from planner.
** Returns NULL if no error occurred.
*/
const char *cypherPlannerGetError(CypherPlanner *pPlanner);

/*
** Logical plan construction functions.
*/

/*
** Create a new logical plan node.
** Returns NULL on allocation failure.
*/
LogicalPlanNode *logicalPlanNodeCreate(LogicalPlanNodeType type);

/*
** Destroy a logical plan node and all children.
** Safe to call with NULL pointer.
*/
void logicalPlanNodeDestroy(LogicalPlanNode *pNode);

/*
** Add a child node to a logical plan node.
** Returns SQLITE_OK on success, SQLITE_NOMEM on allocation failure.
*/
int logicalPlanNodeAddChild(LogicalPlanNode *pParent, LogicalPlanNode *pChild);

/*
** Set string properties of a logical plan node.
** Makes a copy of the string using sqlite3_malloc().
*/
int logicalPlanNodeSetAlias(LogicalPlanNode *pNode, const char *zAlias);
int logicalPlanNodeSetLabel(LogicalPlanNode *pNode, const char *zLabel);
int logicalPlanNodeSetProperty(LogicalPlanNode *pNode, const char *zProperty);
int logicalPlanNodeSetValue(LogicalPlanNode *pNode, const char *zValue);

/*
** Physical plan construction functions.
*/

/*
** Create a new physical plan node.
** Returns NULL on allocation failure.
*/
PhysicalPlanNode *physicalPlanNodeCreate(PhysicalOperatorType type);

/*
** Destroy a physical plan node and all children.
** Safe to call with NULL pointer.
*/
void physicalPlanNodeDestroy(PhysicalPlanNode *pNode);

/*
** Add a child node to a physical plan node.
** Returns SQLITE_OK on success, SQLITE_NOMEM on allocation failure.
*/
int physicalPlanNodeAddChild(PhysicalPlanNode *pParent, PhysicalPlanNode *pChild);

/*
** Optimization and cost estimation functions.
*/

/*
** Estimate the cost of executing a logical plan node.
** Uses statistics and heuristics for cost calculation.
*/
double logicalPlanEstimateCost(LogicalPlanNode *pNode, PlanContext *pContext);

/*
** Estimate the number of rows produced by a logical plan node.
** Uses cardinality estimation techniques.
*/
sqlite3_int64 logicalPlanEstimateRows(LogicalPlanNode *pNode, PlanContext *pContext);

/*
** Optimize join ordering using dynamic programming.
** Modifies the logical plan tree for better execution order.
*/
int logicalPlanOptimizeJoins(LogicalPlanNode *pNode, PlanContext *pContext);

/*
** Convert logical plan to physical plan with operator selection.
** Chooses the best physical operator for each logical operation.
*/
PhysicalPlanNode *logicalPlanToPhysical(LogicalPlanNode *pLogical, PlanContext *pContext);

/*
** Utility and debugging functions.
*/

/*
** Get string representation of logical plan node type.
** Returns static string, do not free.
*/
const char *logicalPlanNodeTypeName(LogicalPlanNodeType type);

/*
** Get string representation of physical operator type.
** Returns static string, do not free.
*/
const char *physicalOperatorTypeName(PhysicalOperatorType type);

/*
** Generate string representation of logical plan tree.
** Caller must sqlite3_free() the returned string.
*/
char *logicalPlanToString(LogicalPlanNode *pNode);

/*
** Generate string representation of physical plan tree.
** Caller must sqlite3_free() the returned string.
*/
char *physicalPlanToString(PhysicalPlanNode *pNode);

/*
** Test and demo functions.
*/

/*
** Create a simple test logical plan for demonstration.
** Represents: MATCH (n:Person) WHERE n.age > 30 RETURN n.name
*/
LogicalPlanNode *cypherCreateTestLogicalPlan(void);

/*
** Create a simple test physical plan for demonstration.
** Shows optimized physical operators with index usage.
*/
PhysicalPlanNode *cypherCreateTestPhysicalPlan(void);

#endif /* CYPHER_PLANNER_H */