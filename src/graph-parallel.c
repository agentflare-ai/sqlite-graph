/*
** graph-parallel.c - Parallel query execution implementation
**
** This file implements multi-threaded query processing with a
** work-stealing task scheduler for the SQLite Graph Extension.
*/

#include <sqlite3.h>
#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include "graph.h"
#include "graph-memory.h"
#include "graph-performance.h"
#include "graph-memory.h"

/* Thread-local storage for worker threads */
typedef struct WorkerContext {
    int threadId;                /* Worker thread ID */
    TaskScheduler *scheduler;    /* Parent scheduler */
    ParallelTask *localQueue;    /* Thread-local task queue */
    int localQueueSize;          /* Number of tasks in local queue */
    pthread_t thread;            /* Thread handle */
    int shouldStop;              /* Stop flag */
    sqlite3_int64 tasksExecuted; /* Statistics */
    sqlite3_int64 tasksStolen;   /* Work stealing statistics */
} WorkerContext;

/* Global thread pool state */
static struct {
    WorkerContext *workers;      /* Array of worker contexts */
    int nWorkers;                /* Number of worker threads */
    pthread_mutex_t globalMutex; /* Global synchronization */
    pthread_cond_t workAvailable;/* Work available condition */
    int initialized;             /* Initialization flag */
} g_threadPool = {0};

/*
** Worker thread main function
*/
static void* workerThreadMain(void *arg) {
    WorkerContext *ctx = (WorkerContext*)arg;
    
    while (!ctx->shouldStop) {
        ParallelTask *task = NULL;
        
        /* Try to get task from local queue */
        pthread_mutex_lock(&g_threadPool.globalMutex);
        if (ctx->localQueue) {
            task = ctx->localQueue;
            ctx->localQueue = task->pNext;
            ctx->localQueueSize--;
        }
        pthread_mutex_unlock(&g_threadPool.globalMutex);
        
        /* If no local task, try work stealing */
        if (!task && ctx->scheduler->stealingEnabled) {
            /* Try to steal from other workers */
            for (int i = 0; i < g_threadPool.nWorkers; i++) {
                if (i == ctx->threadId) continue;
                
                WorkerContext *victim = &g_threadPool.workers[i];
                pthread_mutex_lock(&g_threadPool.globalMutex);
                
                if (victim->localQueueSize > 1) {
                    /* Steal half of victim's tasks */
                    int stealCount = victim->localQueueSize / 2;
                    ParallelTask **pCurrent = &victim->localQueue;
                    
                    /* Find the middle of the queue */
                    for (int j = 0; j < victim->localQueueSize - stealCount; j++) {
                        pCurrent = &(*pCurrent)->pNext;
                    }
                    
                    /* Steal tasks */
                    task = *pCurrent;
                    *pCurrent = NULL;
                    victim->localQueueSize -= stealCount;
                    ctx->tasksStolen += stealCount;
                }
                
                pthread_mutex_unlock(&g_threadPool.globalMutex);
                if (task) break;
            }
        }
        
        /* Execute task if found */
        if (task) {
            task->execute(task->arg);
            ctx->tasksExecuted++;
            sqlite3_free(task);
        } else {
            /* Wait for work */
            pthread_mutex_lock(&g_threadPool.globalMutex);
            if (!ctx->shouldStop && !ctx->localQueue) {
                pthread_cond_wait(&g_threadPool.workAvailable, 
                                 &g_threadPool.globalMutex);
            }
            pthread_mutex_unlock(&g_threadPool.globalMutex);
        }
    }
    
    return NULL;
}

/*
** Create task scheduler
*/
TaskScheduler* graphCreateTaskScheduler(int nThreads) {
    TaskScheduler *scheduler = sqlite3_malloc(sizeof(TaskScheduler));
    if (!scheduler) return NULL;
    
    /* Limit threads to available cores */
    int nCores = sysconf(_SC_NPROCESSORS_ONLN);
    if (nThreads <= 0 || nThreads > nCores * 2) {
        nThreads = nCores;
    }
    
    scheduler->nThreads = nThreads;
    scheduler->stealingEnabled = 1;
    scheduler->queues = sqlite3_malloc(nThreads * sizeof(ParallelTask*));
    
    if (!scheduler->queues) {
        sqlite3_free(scheduler);
        return NULL;
    }
    
    /* Initialize global thread pool if needed */
    if (!g_threadPool.initialized) {
        pthread_mutex_init(&g_threadPool.globalMutex, NULL);
        pthread_cond_init(&g_threadPool.workAvailable, NULL);
        
        g_threadPool.workers = sqlite3_malloc(nThreads * sizeof(WorkerContext));
        if (!g_threadPool.workers) {
            sqlite3_free(scheduler->queues);
            sqlite3_free(scheduler);
            return NULL;
        }
        
        g_threadPool.nWorkers = nThreads;
        
        /* Create worker threads */
        for (int i = 0; i < nThreads; i++) {
            WorkerContext *worker = &g_threadPool.workers[i];
            memset(worker, 0, sizeof(WorkerContext));
            worker->threadId = i;
            worker->scheduler = scheduler;
            worker->shouldStop = 0;
            
            pthread_create(&worker->thread, NULL, workerThreadMain, worker);
        }
        
        g_threadPool.initialized = 1;
    }
    
    return scheduler;
}

/*
** Schedule a task for execution
*/
int graphScheduleTask(TaskScheduler *scheduler, ParallelTask *task) {
    if (!scheduler || !task) return SQLITE_MISUSE;
    
    /* Find worker with smallest queue */
    int minQueueSize = INT_MAX;
    int targetWorker = 0;
    
    pthread_mutex_lock(&g_threadPool.globalMutex);
    
    for (int i = 0; i < g_threadPool.nWorkers; i++) {
        if (g_threadPool.workers[i].localQueueSize < minQueueSize) {
            minQueueSize = g_threadPool.workers[i].localQueueSize;
            targetWorker = i;
        }
    }
    
    /* Add task to target worker's queue */
    WorkerContext *worker = &g_threadPool.workers[targetWorker];
    task->pNext = worker->localQueue;
    worker->localQueue = task;
    worker->localQueueSize++;
    
    /* Signal work available */
    pthread_cond_broadcast(&g_threadPool.workAvailable);
    pthread_mutex_unlock(&g_threadPool.globalMutex);
    
    return SQLITE_OK;
}

/*
** Execute multiple tasks in parallel
*/
int graphExecuteParallel(TaskScheduler *scheduler,
                        void (*taskFunc)(void*),
                        void **args, int nTasks) {
    if (!scheduler || !taskFunc || !args || nTasks <= 0) {
        return SQLITE_MISUSE;
    }
    
    /* Create and schedule tasks */
    for (int i = 0; i < nTasks; i++) {
        ParallelTask *task = sqlite3_malloc(sizeof(ParallelTask));
        if (!task) return SQLITE_NOMEM;
        
        task->execute = taskFunc;
        task->arg = args[i];
        task->priority = 0;
        task->pNext = NULL;
        
        int rc = graphScheduleTask(scheduler, task);
        if (rc != SQLITE_OK) {
            sqlite3_free(task);
            return rc;
        }
    }
    
    /* Wait for all tasks to complete */
    int allDone = 0;
    while (!allDone) {
        pthread_mutex_lock(&g_threadPool.globalMutex);
        
        allDone = 1;
        for (int i = 0; i < g_threadPool.nWorkers; i++) {
            if (g_threadPool.workers[i].localQueueSize > 0) {
                allDone = 0;
                break;
            }
        }
        
        pthread_mutex_unlock(&g_threadPool.globalMutex);
        
        if (!allDone) {
            usleep(1000); /* 1ms sleep */
        }
    }
    
    return SQLITE_OK;
}

/*
** Destroy task scheduler
*/
void graphDestroyTaskScheduler(TaskScheduler *scheduler) {
    if (!scheduler) return;
    
    /* Stop all worker threads */
    if (g_threadPool.initialized) {
        pthread_mutex_lock(&g_threadPool.globalMutex);
        
        for (int i = 0; i < g_threadPool.nWorkers; i++) {
            g_threadPool.workers[i].shouldStop = 1;
        }
        
        pthread_cond_broadcast(&g_threadPool.workAvailable);
        pthread_mutex_unlock(&g_threadPool.globalMutex);
        
        /* Wait for threads to finish */
        for (int i = 0; i < g_threadPool.nWorkers; i++) {
            pthread_join(g_threadPool.workers[i].thread, NULL);
        }
        
        /* Cleanup */
        sqlite3_free(g_threadPool.workers);
        pthread_mutex_destroy(&g_threadPool.globalMutex);
        pthread_cond_destroy(&g_threadPool.workAvailable);
        g_threadPool.initialized = 0;
    }
    
    sqlite3_free(scheduler->queues);
    sqlite3_free(scheduler);
}

/*
** Parallel pattern matching implementation
*/
typedef struct ParallelPatternMatch {
    GraphVtab *pGraph;
    CypherAst *pattern;
    sqlite3_int64 startNode;
    sqlite3_int64 endNode;
    sqlite3_int64 *results;
    int nResults;
    pthread_mutex_t resultMutex;
} ParallelPatternMatch;

static void parallelPatternWorker(void *arg) {
    ParallelPatternMatch *match = (ParallelPatternMatch*)arg;
    char *zSql;
    sqlite3_stmt *pStmt;
    int rc;
    
    zSql = sqlite3_mprintf("SELECT id, labels, properties FROM %s_nodes LIMIT %lld OFFSET %lld", 
                           match->pGraph->zTableName, 
                           match->endNode - match->startNode, 
                           match->startNode);
    rc = sqlite3_prepare_v2(match->pGraph->pDb, zSql, -1, &pStmt, 0);
    sqlite3_free(zSql);
    if( rc!=SQLITE_OK ) return;

    while( sqlite3_step(pStmt)==SQLITE_ROW ){
      sqlite3_int64 nodeId = sqlite3_column_int64(pStmt, 0);
      const unsigned char *labels = sqlite3_column_text(pStmt, 1);

      int matches = 1;
      if (match->pattern->zValue && labels) {
          matches = 0;
          if( strstr((const char*)labels, match->pattern->zValue) ){
            matches = 1;
          }
      }
      
      if (matches) {
          pthread_mutex_lock(&match->resultMutex);
          if (match->nResults < 1000) { /* Limit results */
              match->results[match->nResults++] = nodeId;
          }
          pthread_mutex_unlock(&match->resultMutex);
      }
    }
    sqlite3_finalize(pStmt);
}

int graphParallelPatternMatch(GraphVtab *pGraph, CypherAst *pattern,
                             sqlite3_int64 **pResults, int *pnResults) {
    if (!pGraph || !pattern || !pResults || !pnResults) {
        return SQLITE_MISUSE;
    }
    
    TaskScheduler *scheduler = graphCreateTaskScheduler(0);
    if (!scheduler) return SQLITE_NOMEM;
    
    sqlite3_int64 *results = sqlite3_malloc(1000 * sizeof(sqlite3_int64));
    if (!results) {
        graphDestroyTaskScheduler(scheduler);
        return SQLITE_NOMEM;
    }
    
    int nThreads = scheduler->nThreads;
    int nNodes = 0;
    char *zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pGraph->zTableName);
    sqlite3_stmt *pStmt;
    int rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
    sqlite3_free(zSql);
    if( rc==SQLITE_OK && sqlite3_step(pStmt)==SQLITE_ROW ){
      nNodes = sqlite3_column_int(pStmt, 0);
    }
    sqlite3_finalize(pStmt);

    sqlite3_int64 nodesPerThread = nNodes / nThreads;
    
    ParallelPatternMatch *matches = sqlite3_malloc(
        nThreads * sizeof(ParallelPatternMatch));
    if (!matches) {
        sqlite3_free(results);
        graphDestroyTaskScheduler(scheduler);
        return SQLITE_NOMEM;
    }
    
    pthread_mutex_t resultMutex;
    pthread_mutex_init(&resultMutex, NULL);
    
    void **args = sqlite3_malloc(nThreads * sizeof(void*));
    if (!args) {
        sqlite3_free(matches);
        sqlite3_free(results);
        graphDestroyTaskScheduler(scheduler);
        return SQLITE_NOMEM;
    }
    
    int totalResults = 0;
    for (int i = 0; i < nThreads; i++) {
        matches[i].pGraph = pGraph;
        matches[i].pattern = pattern;
        matches[i].startNode = i * nodesPerThread;
        matches[i].endNode = (i == nThreads - 1) ? 
            nNodes : (i + 1) * nodesPerThread;
        matches[i].results = results;
        matches[i].nResults = 0;
        matches[i].resultMutex = resultMutex;
        
        args[i] = &matches[i];
    }
    
    rc = graphExecuteParallel(scheduler, parallelPatternWorker, 
                                 args, nThreads);
    
    if (rc == SQLITE_OK) {
        for (int i = 0; i < nThreads; i++) {
            totalResults += matches[i].nResults;
        }
        
        *pResults = results;
        *pnResults = totalResults;
    } else {
        sqlite3_free(results);
    }
    
    pthread_mutex_destroy(&resultMutex);
    sqlite3_free(args);
    sqlite3_free(matches);
    graphDestroyTaskScheduler(scheduler);
    
    return rc;
}

/*
** Parallel aggregation implementation
*/
typedef struct ParallelAggregation {
    GraphVtab *pGraph;
    const char *property;
    double sum;
    sqlite3_int64 count;
    double min;
    double max;
    pthread_mutex_t mutex;
} ParallelAggregation;