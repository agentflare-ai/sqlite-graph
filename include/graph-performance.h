/*
** graph-performance.h - Performance optimization infrastructure
**
** This file defines structures and functions for query performance
** optimization, including pattern matching optimization, index
** utilization, memory management, and parallel execution support.
*/

#ifndef GRAPH_PERFORMANCE_H
#define GRAPH_PERFORMANCE_H

#include "graph.h"
#include "cypher-planner.h"
#include "graph-bulk.h"

/*
** Query Performance Optimization
*/

/* Selectivity estimation for pattern matching */
typedef struct SelectivityEstimate {
    double selectivity;          /* 0.0 to 1.0 */
    sqlite3_int64 estimatedRows; /* Estimated result size */
    int confidence;              /* 0-100 confidence level */
} SelectivityEstimate;

/* Join order optimizer */
typedef struct JoinOrderOptimizer {
    LogicalPlanNode **joins;     /* Array of join operations */
    int nJoins;                  /* Number of joins */
    double *costs;               /* Cost estimates for each order */
    int *bestOrder;              /* Optimal join order */
} JoinOrderOptimizer;

/* Pattern matching optimizer */
typedef struct PatternOptimizer {
    int eliminateCartesian;      /* Eliminate Cartesian products */
    int enablePruning;           /* Enable branch pruning */
    int cacheSubpatterns;        /* Cache repeated subpatterns */
    int maxCacheSize;            /* Maximum cache entries */
} PatternOptimizer;

/*
** Index Utilization Improvements
*/

/* Composite index structure */
typedef struct CompositeIndex {
    char *indexName;             /* Index name */
    char **properties;           /* Array of property names */
    int nProperties;             /* Number of properties */
    void *btree;                 /* B-tree implementation */
    sqlite3_int64 nEntries;      /* Number of entries */
} CompositeIndex;

/* Bitmap index for set operations */
typedef struct BitmapIndex {
    char *property;              /* Indexed property */
    unsigned char *bitmap;       /* Bitmap data */
    sqlite3_int64 nBits;         /* Number of bits */
    sqlite3_int64 nNodes;        /* Number of nodes */
} BitmapIndex;

/* Index statistics for query planning */
typedef struct IndexStatistics {
    char *indexName;             /* Index name */
    sqlite3_int64 cardinality;   /* Number of distinct values */
    double avgSelectivity;       /* Average selectivity */
    sqlite3_int64 nScans;        /* Number of scans performed */
    double avgScanTime;          /* Average scan time (ms) */
} IndexStatistics;

/*
** Memory Pool Optimization
*/

/* Per-query memory pool */
typedef struct QueryMemoryPool {
    void *basePtr;               /* Base memory pointer */
    size_t totalSize;            /* Total pool size */
    size_t usedSize;             /* Currently used size */
    void **allocations;          /* Array of allocations */
    int nAllocations;            /* Number of allocations */
    int recycleEnabled;          /* Enable memory recycling */
} QueryMemoryPool;

/* Tuple memory recycler */
typedef struct TupleRecycler {
    void **freeTuples;           /* Array of free tuples */
    int nFree;                   /* Number of free tuples */
    int tupleSize;               /* Size of each tuple */
    int maxFree;                 /* Maximum free list size */
} TupleRecycler;

/*
** Parallel Query Execution
*/

/* Task for parallel execution */
typedef struct ParallelTask {
    void (*execute)(void *arg);  /* Task function */
    void *arg;                   /* Task argument */
    int priority;                /* Task priority */
    struct ParallelTask *pNext;  /* Next task in queue */
} ParallelTask;

/* Work-stealing task scheduler */
typedef struct TaskScheduler {
    ParallelTask **queues;       /* Per-thread task queues */
    int nThreads;                /* Number of worker threads */
    int stealingEnabled;         /* Enable work stealing */
    sqlite3_mutex *mutex;        /* Synchronization mutex */
} TaskScheduler;

/*
** Storage Optimization
*/

/* Cache-friendly node layout */
typedef struct OptimizedNode {
    sqlite3_int64 nodeId;        /* Node ID */
    char *labels;                /* Packed label data */
    void *properties;            /* Compressed properties */
    sqlite3_int64 *outEdges;     /* Delta-encoded edge list */
    int nOutEdges;               /* Number of outgoing edges */
    int cacheLineAligned;        /* Alignment flag */
} OptimizedNode;

/* Compressed sparse row format for edges */
typedef struct CSRGraph {
    sqlite3_int64 *rowOffsets;   /* Row offset array */
    sqlite3_int64 *columnIndices;/* Column indices array */
    double *edgeWeights;         /* Edge weights array */
    sqlite3_int64 nNodes;        /* Number of nodes */
    sqlite3_int64 nEdges;        /* Number of edges */
} CSRGraph;

/*
** Benchmarking Infrastructure
*/

/* Performance metrics collector */
typedef struct PerfMetrics {
    double queryStartTime;       /* Query start timestamp */
    double queryEndTime;         /* Query end timestamp */
    sqlite3_int64 nodesScanned;  /* Nodes scanned */
    sqlite3_int64 edgesTraversed;/* Edges traversed */
    sqlite3_int64 bytesRead;     /* Bytes read from storage */
    sqlite3_int64 bytesWritten;  /* Bytes written to storage */
    int cacheHits;               /* Cache hit count */
    int cacheMisses;             /* Cache miss count */
} PerfMetrics;

/*
** Function Declarations
*/

/* Query optimization */
SelectivityEstimate graphEstimateSelectivity(GraphVtab *pGraph, 
                                           CypherAst *pattern);
int graphOptimizeJoinOrder(JoinOrderOptimizer *optimizer);
int graphEliminateCartesianProduct(PhysicalPlanNode *plan);

/* Index operations */
CompositeIndex* graphCreateCompositeIndex(GraphVtab *pGraph,
                                         const char **properties,
                                         int nProperties);
BitmapIndex* graphCreateBitmapIndex(GraphVtab *pGraph,
                                   const char *property);
int graphIntersectBitmaps(BitmapIndex *idx1, BitmapIndex *idx2,
                         unsigned char *result);

/* Memory management */
QueryMemoryPool* graphCreateMemoryPool(size_t initialSize);
void* graphPoolAlloc(QueryMemoryPool *pool, size_t size);
void graphPoolFree(QueryMemoryPool *pool, void *ptr);
void graphDestroyMemoryPool(QueryMemoryPool *pool);

/* Parallel execution */
TaskScheduler* graphCreateTaskScheduler(int nThreads);
int graphScheduleTask(TaskScheduler *scheduler, ParallelTask *task);
int graphExecuteParallel(TaskScheduler *scheduler,
                        void (*taskFunc)(void*),
                        void **args, int nTasks);
void graphDestroyTaskScheduler(TaskScheduler *scheduler);
int graphParallelPatternMatch(GraphVtab *pGraph, CypherAst *pattern,
                             sqlite3_int64 **pResults, int *pnResults);

/* Storage optimization */
CSRGraph* graphConvertToCSR(GraphVtab *pGraph);
char* graphCompressProperties(const char *zProperties);
int graphDeltaEncodeEdges(sqlite3_int64 *edges, int nEdges);

/* Compression system */
int graphInitStringDictionary(int initialBuckets);
char* graphDecompressProperties(const char *zCompressed);
char* graphCompressLarge(const char *zData, size_t *pCompressedSize);
char* graphDecompressLarge(const char *zCompressed, size_t compressedSize);
void graphCompressionStats(sqlite3_int64 *pDictEntries,
                          size_t *pDictMemory,
                          size_t *pSavedBytes);
void graphCompressionShutdown(void);
int graphRegisterCompressionFunctions(sqlite3 *db);

/* Bulk loading functions */
int graphBulkLoadNodesCSV(GraphVtab *pGraph, const char *csvData,
                         size_t dataSize, BulkLoaderConfig *config,
                         BulkLoadStats *stats);
int graphBulkLoadMapped(GraphVtab *pGraph, const char *filename,
                       BulkLoaderConfig *config, BulkLoadStats *stats);
int graphRegisterBulkLoadFunctions(sqlite3 *db);

/* Performance monitoring */
PerfMetrics* graphStartMetrics(void);
void graphUpdateMetrics(PerfMetrics *metrics, const char *event);
char* graphFormatMetrics(PerfMetrics *metrics);
void graphEndMetrics(PerfMetrics *metrics);

/* Plan cache operations */
int graphInitPlanCache(int maxEntries, size_t maxMemory);
PhysicalPlanNode* graphPlanCacheLookup(const char *zQuery);
int graphPlanCacheInsert(const char *zQuery, PhysicalPlanNode *pPlan);
int graphPlanCacheInvalidate(const char *pattern);
void graphPlanCacheStats(sqlite3_int64 *hits, sqlite3_int64 *misses,
                        int *nEntries, size_t *memoryUsed);
void graphPlanCacheClear(void);
void graphPlanCacheShutdown(void);
int graphRegisterPlanCacheFunctions(sqlite3 *db);
int graphRegisterBenchmarkFunctions(sqlite3 *db);

#endif /* GRAPH_PERFORMANCE_H */