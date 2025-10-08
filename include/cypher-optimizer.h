/*
 * Cypher Query Optimizer
 * Advanced pattern matching and query optimization for performance
 */

#ifndef CYPHER_OPTIMIZER_H
#define CYPHER_OPTIMIZER_H

#include "cypher-planner.h"
#include "cypher-executor.h"

/* Optimization levels */
typedef enum {
    CYPHER_OPT_NONE = 0,     /* No optimization */
    CYPHER_OPT_BASIC = 1,    /* Basic rule-based optimization */
    CYPHER_OPT_ADVANCED = 2, /* Advanced cost-based optimization */
    CYPHER_OPT_AGGRESSIVE = 3 /* Aggressive optimization with heuristics */
} CypherOptimizationLevel;

/* Pattern matching strategies */
typedef enum {
    CYPHER_PATTERN_NAIVE,      /* Simple nested loop joins */
    CYPHER_PATTERN_HASH_JOIN,  /* Hash-based joins for equality predicates */
    CYPHER_PATTERN_INDEX_SCAN, /* Index-based pattern matching */
    CYPHER_PATTERN_ADAPTIVE    /* Adaptive strategy based on data characteristics */
} CypherPatternStrategy;

/* Query statistics for optimization */
typedef struct {
    sqlite3_int64 iNodeCount;
    sqlite3_int64 iEdgeCount;
    sqlite3_int64 iLabelCount;
    sqlite3_int64 iPropertyCount;
    double rSelectivity;
    double rJoinSelectivity;
    int nPatternComplexity;
    int bHasIndexes;
} CypherQueryStats;

/* Optimization context */
typedef struct CypherOptimizer {
    CypherOptimizationLevel level;
    CypherQueryStats stats;
    sqlite3 *pDb;
    char *zErrorMsg;
    
    /* Plan cache */
    struct PlanCacheEntry **apPlanCache;
    int nCacheSize;
    int nCacheCapacity;
    
    /* Optimization flags */
    int bEnablePushdown;
    int bEnableJoinReorder;
    int bEnableIndexSelection;
    int bEnableParallelization;
    
    /* Cost model parameters */
    double rSeqScanCost;
    double rIndexScanCost;
    double rHashJoinCost;
    double rNestedLoopCost;
} CypherOptimizer;

/* Plan cache entry */
typedef struct PlanCacheEntry {
    char *zQueryPattern;
    char *zQueryHash;
    PhysicalPlanNode *pPlan;
    CypherQueryStats stats;
    sqlite3_int64 iAccessTime;
    int nAccessCount;
    struct PlanCacheEntry *pNext;
} PlanCacheEntry;

/* Join ordering optimization */
typedef struct JoinNode {
    LogicalPlanNode *pPattern;
    double rCost;
    double rSelectivity;
    sqlite3_int64 iCardinality;
    int *aiJoinable; /* Bit vector of joinable patterns */
    struct JoinNode *pNext;
} JoinNode;

/* Index selection optimization */
typedef struct IndexCandidate {
    char *zIndexName;
    char *zLabelName;
    char *zPropertyName;
    double rSelectivity;
    double rCost;
    int bCovering;
    int nKeyColumns;
} IndexCandidate;

/* Optimizer interface */
int cypherOptimizerCreate(CypherOptimizer **ppOptimizer, sqlite3 *pDb);
int cypherOptimizerDestroy(CypherOptimizer *pOptimizer);
int cypherOptimizerSetLevel(CypherOptimizer *pOptimizer, CypherOptimizationLevel level);

/* Main optimization entry points */
int cypherOptimizeLogicalPlan(CypherOptimizer *pOptimizer, 
                             LogicalPlanNode **ppPlan,
                             CypherQueryStats *pStats);

int cypherOptimizePhysicalPlan(CypherOptimizer *pOptimizer,
                              PhysicalPlanNode **ppPlan,
                              CypherQueryStats *pStats);

/* Pattern matching optimization */
int cypherOptimizePatternMatching(CypherOptimizer *pOptimizer,
                                 LogicalPlanNode *pPattern,
                                 CypherPatternStrategy *pStrategy);

int cypherEstimatePatternSelectivity(CypherOptimizer *pOptimizer,
                                   LogicalPlanNode *pPattern,
                                   double *pSelectivity);

/* Join ordering optimization */
int cypherOptimizeJoinOrder(CypherOptimizer *pOptimizer,
                           LogicalPlanNode **ppPlan,
                           JoinNode **apJoins,
                           int nJoins);

int cypherEnumerateJoinOrders(JoinNode **apJoins, int nJoins,
                            JoinNode ***papBestOrder, double *pBestCost);

/* Index selection */
int cypherSelectOptimalIndexes(CypherOptimizer *pOptimizer,
                              LogicalPlanNode *pPlan,
                              IndexCandidate **papIndexes,
                              int *pnIndexes);

int cypherEvaluateIndexCandidate(CypherOptimizer *pOptimizer,
                               IndexCandidate *pIndex,
                               LogicalPlanNode *pPattern,
                               double *pCost);

/* Predicate pushdown optimization */
int cypherPushdownPredicates(CypherOptimizer *pOptimizer,
                           LogicalPlanNode **ppPlan);

int cypherIdentifyPushdownCandidates(LogicalPlanNode *pPlan,
                                   LogicalPlanNode ***papCandidates,
                                   int *pnCandidates);

/* Plan caching */
int cypherPlanCacheGet(CypherOptimizer *pOptimizer,
                      const char *zQueryPattern,
                      PhysicalPlanNode **ppPlan);

int cypherPlanCachePut(CypherOptimizer *pOptimizer,
                      const char *zQueryPattern,
                      PhysicalPlanNode *pPlan,
                      CypherQueryStats *pStats);

int cypherPlanCacheEvict(CypherOptimizer *pOptimizer, int nTargetSize);

/* Statistics collection */
int cypherCollectQueryStats(CypherOptimizer *pOptimizer,
                          LogicalPlanNode *pPlan,
                          CypherQueryStats *pStats);

int cypherUpdateTableStats(CypherOptimizer *pOptimizer);

/* Cost model */
double cypherEstimateOperatorCost(CypherOptimizer *pOptimizer,
                                LogicalPlanNode *pOp,
                                CypherQueryStats *pStats);

double cypherEstimateJoinCost(CypherOptimizer *pOptimizer,
                            LogicalPlanNode *pLeft,
                            LogicalPlanNode *pRight,
                            CypherQueryStats *pStats);

/* Memory optimization */
int cypherOptimizeMemoryUsage(CypherOptimizer *pOptimizer,
                            PhysicalPlanNode **ppPlan);

int cypherEstimateMemoryRequirement(PhysicalPlanNode *pPlan,
                                  sqlite3_int64 *piMemory);

/* Parallelization optimization */
int cypherIdentifyParallelizationOpportunities(CypherOptimizer *pOptimizer,
                                              PhysicalPlanNode *pPlan,
                                              PhysicalPlanNode ***papParallel,
                                              int *pnParallel);

/* Query complexity analysis */
int cypherAnalyzeQueryComplexity(LogicalPlanNode *pPlan, int *pnComplexity);
int cypherEstimateExecutionTime(CypherOptimizer *pOptimizer,
                              PhysicalPlanNode *pPlan,
                              double *pEstimatedTime);

/* Optimization rules */
int cypherApplyOptimizationRules(CypherOptimizer *pOptimizer,
                               LogicalPlanNode **ppPlan);

/* Debug and profiling */
char *cypherOptimizerExplainPlan(CypherOptimizer *pOptimizer,
                               PhysicalPlanNode *pPlan);

int cypherOptimizerGetStats(CypherOptimizer *pOptimizer,
                          char **pzStatsJson);

/* Utility functions */
char *cypherGenerateQueryPattern(LogicalPlanNode *pPlan);
char *cypherHashQuery(const char *zQuery);
int cypherIsEquivalentPlan(LogicalPlanNode *pPlan1, LogicalPlanNode *pPlan2);

#endif /* CYPHER_OPTIMIZER_H */