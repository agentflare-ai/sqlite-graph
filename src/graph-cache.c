/*
** graph-cache.c - Query plan caching implementation
**
** This file implements a cache for compiled query plans to avoid
** repeated parsing and planning overhead for prepared statements.
*/

#include <sqlite3.h>
#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "graph.h"
#include "graph-memory.h"
#include "graph-performance.h"
#include "graph-memory.h"
#include "cypher-planner.h"

/* Cache entry for compiled query plan */
typedef struct PlanCacheEntry {
    char *zQueryHash;            /* Hash of query text */
    PhysicalPlanNode *pPlan;   /* Cached physical plan */
    sqlite3_int64 lastUsed;      /* Last access timestamp */
    sqlite3_int64 useCount;      /* Number of times used */
    double avgExecutionTime;     /* Average execution time */
    size_t memorySize;           /* Memory used by plan */
    struct PlanCacheEntry *pNext;/* Next entry in hash chain */
    struct PlanCacheEntry *pLRU; /* LRU list linkage */
} PlanCacheEntry;

/* Query plan cache structure */
typedef struct PlanCache {
    PlanCacheEntry **buckets;    /* Hash table buckets */
    int nBuckets;                /* Number of buckets */
    int nEntries;                /* Current number of entries */
    int maxEntries;              /* Maximum entries allowed */
    size_t maxMemory;            /* Maximum memory usage */
    size_t currentMemory;        /* Current memory usage */
    PlanCacheEntry *pLRUHead;    /* LRU list head (most recent) */
    PlanCacheEntry *pLRUTail;    /* LRU list tail (least recent) */
    sqlite3_mutex *mutex;        /* Thread safety */
    
    /* Statistics */
    sqlite3_int64 hits;          /* Cache hits */
    sqlite3_int64 misses;        /* Cache misses */
    sqlite3_int64 evictions;     /* Entries evicted */
} PlanCache;

/* Global plan cache instance */
static PlanCache *g_planCache = NULL;

/*
** Hash function for query text
*/
static unsigned int planCacheHash(const char *zQuery) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *zQuery++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/*
** Calculate memory size of a physical plan
*/
static size_t calculatePlanSize(PhysicalPlanNode *pPlan) {
    if (!pPlan) return 0;
    
    size_t size = sizeof(PhysicalPlanNode);
    
    /* Add size of operator-specific data */
    switch (pPlan->type) {
        case PHYSICAL_ALL_NODES_SCAN:
        case PHYSICAL_ALL_RELS_SCAN:
        case PHYSICAL_TYPE_INDEX_SCAN:
            /* Basic scan operators */
            size += 50; /* Estimate */
            break;
        case PHYSICAL_LABEL_INDEX_SCAN:
        case PHYSICAL_PROPERTY_INDEX_SCAN:
            if (pPlan->zLabel) {
                size += strlen(pPlan->zLabel) + 1;
            }
            if (pPlan->zProperty) {
                size += strlen(pPlan->zProperty) + 1;
            }
            break;
        case PHYSICAL_HASH_JOIN:
        case PHYSICAL_NESTED_LOOP_JOIN:
        case PHYSICAL_INDEX_NESTED_LOOP:
            /* Join operators */
            size += 200; /* Estimate */
            break;
        case PHYSICAL_FILTER:
            /* Add expression size */
            size += 100; /* Estimate */
            break;
        case PHYSICAL_PROJECTION:
        case PHYSICAL_SORT:
        case PHYSICAL_LIMIT:
        case PHYSICAL_AGGREGATION:
            /* Other operators */
            size += 100; /* Estimate */
            break;
    }
    
    /* Recursively add child plans */
    for (int i = 0; i < pPlan->nChildren; i++) {
        size += calculatePlanSize(pPlan->apChildren[i]);
    }
    
    return size;
}

/*
** Initialize the global plan cache
*/
int graphInitPlanCache(int maxEntries, size_t maxMemory) {
    if (g_planCache) {
        return SQLITE_MISUSE; /* Already initialized */
    }
    
    g_planCache = sqlite3_malloc(sizeof(PlanCache));
    if (!g_planCache) return SQLITE_NOMEM;
    
    memset(g_planCache, 0, sizeof(PlanCache));
    
    /* Default settings */
    if (maxEntries <= 0) maxEntries = 100;
    if (maxMemory <= 0) maxMemory = 10 * 1024 * 1024; /* 10MB */
    
    g_planCache->maxEntries = maxEntries;
    g_planCache->maxMemory = maxMemory;
    g_planCache->nBuckets = maxEntries * 2; /* Load factor 0.5 */
    
    /* Allocate hash table */
    g_planCache->buckets = sqlite3_malloc(
        g_planCache->nBuckets * sizeof(PlanCacheEntry*));
    if (!g_planCache->buckets) {
        sqlite3_free(g_planCache);
        g_planCache = NULL;
        return SQLITE_NOMEM;
    }
    
    memset(g_planCache->buckets, 0, 
           g_planCache->nBuckets * sizeof(PlanCacheEntry*));
    
    /* Create mutex for thread safety */
    g_planCache->mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_FAST);
    
    return SQLITE_OK;
}

/*
** Move entry to front of LRU list
*/
static void planCacheTouchLRU(PlanCacheEntry *entry) {
    if (!g_planCache || !entry) return;
    
    /* Already at front */
    if (g_planCache->pLRUHead == entry) return;
    
    /* Remove from current position */
    if (entry->pLRU) {
        entry->pLRU->pLRU = entry->pLRU;
    }
    if (g_planCache->pLRUTail == entry) {
        g_planCache->pLRUTail = entry->pLRU;
    }
    
    /* Add to front */
    entry->pLRU = g_planCache->pLRUHead;
    g_planCache->pLRUHead = entry;
    
    if (!g_planCache->pLRUTail) {
        g_planCache->pLRUTail = entry;
    }
}

/*
** Evict least recently used entry
*/
static void planCacheEvictLRU(void) {
    if (!g_planCache || !g_planCache->pLRUTail) return;
    
    PlanCacheEntry *victim = g_planCache->pLRUTail;
    
    /* Remove from hash table */
    unsigned int bucket = planCacheHash(victim->zQueryHash) % g_planCache->nBuckets;
    PlanCacheEntry **pp = &g_planCache->buckets[bucket];
    
    while (*pp) {
        if (*pp == victim) {
            *pp = victim->pNext;
            break;
        }
        pp = &(*pp)->pNext;
    }
    
    /* Remove from LRU list */
    g_planCache->pLRUTail = victim->pLRU;
    if (!g_planCache->pLRUTail) {
        g_planCache->pLRUHead = NULL;
    }
    
    /* Update statistics */
    g_planCache->nEntries--;
    g_planCache->currentMemory -= victim->memorySize;
    g_planCache->evictions++;
    
    /* Free memory */
    sqlite3_free(victim->zQueryHash);
    physicalPlanNodeDestroy(victim->pPlan);
    sqlite3_free(victim);
}

/*
** Look up a plan in the cache
*/
PhysicalPlanNode* graphPlanCacheLookup(const char *zQuery) {
    if (!g_planCache || !zQuery) return NULL;
    
    sqlite3_mutex_enter(g_planCache->mutex);
    
    unsigned int bucket = planCacheHash(zQuery) % g_planCache->nBuckets;
    PlanCacheEntry *entry = g_planCache->buckets[bucket];
    
    while (entry) {
        if (strcmp(entry->zQueryHash, zQuery) == 0) {
            /* Cache hit */
            g_planCache->hits++;
            entry->useCount++;
            entry->lastUsed = (sqlite3_int64)time(NULL);
            
            /* Move to front of LRU */
            planCacheTouchLRU(entry);
            
            sqlite3_mutex_leave(g_planCache->mutex);
            return entry->pPlan;
        }
        entry = entry->pNext;
    }
    
    /* Cache miss */
    g_planCache->misses++;
    sqlite3_mutex_leave(g_planCache->mutex);
    return NULL;
}

/*
** Insert a plan into the cache
*/
int graphPlanCacheInsert(const char *zQuery, PhysicalPlanNode *pPlan) {
    if (!g_planCache || !zQuery || !pPlan) return SQLITE_MISUSE;
    
    sqlite3_mutex_enter(g_planCache->mutex);
    
    /* Check if already exists */
    unsigned int bucket = planCacheHash(zQuery) % g_planCache->nBuckets;
    PlanCacheEntry *existing = g_planCache->buckets[bucket];
    
    while (existing) {
        if (strcmp(existing->zQueryHash, zQuery) == 0) {
            /* Update existing entry */
            physicalPlanNodeDestroy(existing->pPlan);
            existing->pPlan = pPlan;
            existing->memorySize = calculatePlanSize(pPlan);
            existing->lastUsed = (sqlite3_int64)time(NULL);
            
            planCacheTouchLRU(existing);
            sqlite3_mutex_leave(g_planCache->mutex);
            return SQLITE_OK;
        }
        existing = existing->pNext;
    }
    
    /* Create new entry */
    PlanCacheEntry *entry = sqlite3_malloc(sizeof(PlanCacheEntry));
    if (!entry) {
        sqlite3_mutex_leave(g_planCache->mutex);
        return SQLITE_NOMEM;
    }
    
    memset(entry, 0, sizeof(PlanCacheEntry));
    entry->zQueryHash = sqlite3_mprintf("%s", zQuery);
    entry->pPlan = pPlan;
    entry->memorySize = calculatePlanSize(pPlan);
    entry->lastUsed = (sqlite3_int64)time(NULL);
    entry->useCount = 1;
    
    /* Check cache limits */
    while ((g_planCache->nEntries >= g_planCache->maxEntries) ||
           (g_planCache->currentMemory + entry->memorySize > g_planCache->maxMemory)) {
        planCacheEvictLRU();
        
        if (g_planCache->nEntries == 0) break;
    }
    
    /* Insert into hash table */
    entry->pNext = g_planCache->buckets[bucket];
    g_planCache->buckets[bucket] = entry;
    
    /* Add to LRU list */
    entry->pLRU = g_planCache->pLRUHead;
    g_planCache->pLRUHead = entry;
    if (!g_planCache->pLRUTail) {
        g_planCache->pLRUTail = entry;
    }
    
    /* Update statistics */
    g_planCache->nEntries++;
    g_planCache->currentMemory += entry->memorySize;
    
    sqlite3_mutex_leave(g_planCache->mutex);
    return SQLITE_OK;
}

/*
** Invalidate cache entries matching a pattern
*/
int graphPlanCacheInvalidate(const char *pattern) {
    if (!g_planCache) return SQLITE_OK;
    
    sqlite3_mutex_enter(g_planCache->mutex);
    
    int invalidated = 0;
    
    /* Scan all buckets */
    for (int i = 0; i < g_planCache->nBuckets; i++) {
        PlanCacheEntry **pp = &g_planCache->buckets[i];
        
        while (*pp) {
            PlanCacheEntry *entry = *pp;
            
            /* Check if query matches pattern */
            int matches = 0;
            if (!pattern || pattern[0] == '\0') {
                matches = 1; /* Invalidate all */
            } else if (strstr(entry->zQueryHash, pattern)) {
                matches = 1;
            }
            
            if (matches) {
                /* Remove entry */
                *pp = entry->pNext;
                
                /* Remove from LRU */
                if (entry == g_planCache->pLRUHead) {
                    g_planCache->pLRUHead = entry->pLRU;
                }
                if (entry == g_planCache->pLRUTail) {
                    g_planCache->pLRUTail = NULL;
                }
                
                /* Update statistics */
                g_planCache->nEntries--;
                g_planCache->currentMemory -= entry->memorySize;
                
                /* Free memory */
                sqlite3_free(entry->zQueryHash);
                physicalPlanNodeDestroy(entry->pPlan);
                sqlite3_free(entry);
                
                invalidated++;
            } else {
                pp = &entry->pNext;
            }
        }
    }
    
    sqlite3_mutex_leave(g_planCache->mutex);
    return invalidated;
}

/*
** Get cache statistics
*/
void graphPlanCacheStats(sqlite3_int64 *hits, sqlite3_int64 *misses,
                        int *nEntries, size_t *memoryUsed) {
    if (!g_planCache) return;
    
    sqlite3_mutex_enter(g_planCache->mutex);
    
    if (hits) *hits = g_planCache->hits;
    if (misses) *misses = g_planCache->misses;
    if (nEntries) *nEntries = g_planCache->nEntries;
    if (memoryUsed) *memoryUsed = g_planCache->currentMemory;
    
    sqlite3_mutex_leave(g_planCache->mutex);
}

/*
** Clear all entries from cache
*/
void graphPlanCacheClear(void) {
    if (!g_planCache) return;
    
    graphPlanCacheInvalidate(NULL); /* Invalidate all */
    
    /* Reset statistics */
    sqlite3_mutex_enter(g_planCache->mutex);
    g_planCache->hits = 0;
    g_planCache->misses = 0;
    g_planCache->evictions = 0;
    sqlite3_mutex_leave(g_planCache->mutex);
}

/*
** Shutdown plan cache
*/
void graphPlanCacheShutdown(void) {
    if (!g_planCache) return;
    
    /* Clear all entries */
    graphPlanCacheClear();
    
    /* Free structures */
    sqlite3_mutex_free(g_planCache->mutex);
    sqlite3_free(g_planCache->buckets);
    sqlite3_free(g_planCache);
    g_planCache = NULL;
}

/*
** SQL function to get cache statistics
*/
static void planCacheStatsFunc(
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
) {
    (void)argc;
    (void)argv;
    sqlite3_int64 hits, misses;
    int nEntries;
    size_t memoryUsed;
    
    graphPlanCacheStats(&hits, &misses, &nEntries, &memoryUsed);
    
    double hitRate = 0.0;
    if (hits + misses > 0) {
        hitRate = (double)hits / (hits + misses) * 100.0;
    }
    
    char *result = sqlite3_mprintf(
        "{\"hits\":%lld,\"misses\":%lld,\"entries\":%d,"
        "\"memory_bytes\":%zu,\"hit_rate\":%.1f}",
        hits, misses, nEntries, memoryUsed, hitRate
    );
    
    sqlite3_result_text(context, result, -1, sqlite3_free);
}

/*
** SQL function to clear cache
*/
static void planCacheClearFunc(
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
) {
    (void)argc;
    (void)argv;
    graphPlanCacheClear();
    sqlite3_result_text(context, "Plan cache cleared", -1, SQLITE_STATIC);
}

/*
** Register plan cache SQL functions
*/
int graphRegisterPlanCacheFunctions(sqlite3 *db) {
    int rc = SQLITE_OK;
    
    rc = sqlite3_create_function(db, "graph_plan_cache_stats", 0,
                                SQLITE_UTF8, NULL,
                                planCacheStatsFunc, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
    
    rc = sqlite3_create_function(db, "graph_plan_cache_clear", 0,
                                SQLITE_UTF8, NULL,
                                planCacheClearFunc, NULL, NULL);
    
    return rc;
}