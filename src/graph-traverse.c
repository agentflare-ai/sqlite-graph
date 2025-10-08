/*
** SQLite Graph Database Extension - Graph Traversal
**
** This file implements graph traversal algorithms including DFS and BFS.
** Supports cycle detection, depth limits, and path tracking.
** All functions follow SQLite patterns with proper memory management.
**
** Traversal modes: Depth-first search, Breadth-first search
** Features: Visited tracking, path reconstruction, cycle detection
*/

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "graph.h"
#include "graph-memory.h"
#include "graph-util.h"
#include "graph-memory.h"
#include <assert.h>

/*
** Visited set structure for tracking nodes during traversal.
** Uses dynamic array for O(1) insertion and O(n) lookup.
** SQLite-style memory management with sqlite3_malloc/free.
*/
typedef struct GraphVisitedSet GraphVisitedSet;
struct GraphVisitedSet {
  sqlite3_int64 *aNodes;    /* Array of visited node IDs */
  int nUsed;                /* Number of nodes currently in set */
  int nAlloc;               /* Allocated size of array */
};

/* Forward declarations for internal functions */
static int graphDFSRecursive(GraphVtab *pVtab, sqlite3_int64 iNodeId, 
                            int nMaxDepth, int nCurrentDepth,
                            GraphVisitedSet *pVisited, char **pzPath);




/*
** Create a new visited set for tracking traversal state.
** Memory allocation: Uses sqlite3_malloc() for initial capacity.
** Returns pointer to set or NULL on OOM.
*/
static GraphVisitedSet *graphVisitedSetCreate(void){
  GraphVisitedSet *pSet;
  
  pSet = sqlite3_malloc(sizeof(*pSet));
  if( pSet==0 ){
    testcase( pSet==0 );  /* Out of memory */
    return 0;
  }
  
  /* Initialize with small initial capacity */
  pSet->nAlloc = 16;
  pSet->nUsed = 0;
  pSet->aNodes = sqlite3_malloc(pSet->nAlloc * sizeof(sqlite3_int64));
  if( pSet->aNodes==0 ){
    testcase( pSet->aNodes==0 );  /* Out of memory */
    sqlite3_free(pSet);
    return 0;
  }
  
  return pSet;
}

/*
** Add a node to the visited set.
** Returns SQLITE_OK on success, SQLITE_NOMEM on allocation failure.
** Grows array as needed using sqlite3_realloc().
*/
static int graphVisitedSetAdd(GraphVisitedSet *pSet, sqlite3_int64 iNodeId){
  sqlite3_int64 *aNew;
  
  assert( pSet!=0 );
  
  /* Grow array if needed */
  if( pSet->nUsed>=pSet->nAlloc ){
    int nNewAlloc = pSet->nAlloc * 2;
    aNew = sqlite3_realloc(pSet->aNodes, nNewAlloc * sizeof(sqlite3_int64));
    if( aNew==0 ){
      testcase( aNew==0 );  /* Realloc failed */
      return SQLITE_NOMEM;
    }
    pSet->aNodes = aNew;
    pSet->nAlloc = nNewAlloc;
  }
  
  /* Add node to set */
  pSet->aNodes[pSet->nUsed++] = iNodeId;
  return SQLITE_OK;
}

/*
** Check if a node is in the visited set.
** Returns non-zero if node is visited, zero otherwise.
** Time complexity: O(n) linear search.
*/
static int graphVisitedSetContains(GraphVisitedSet *pSet, 
                                   sqlite3_int64 iNodeId){
  int i;
  
  assert( pSet!=0 );
  
  for( i=0; i<pSet->nUsed; i++ ){
    if( pSet->aNodes[i]==iNodeId ){
      testcase( pSet->aNodes[i]==iNodeId );  /* Node found in set */
      return 1;
    }
  }
  
  testcase( i==pSet->nUsed );  /* Node not found in set */
  return 0;
}

/*
** Destroy visited set and free all memory.
** Memory management: Calls sqlite3_free() on all allocated memory.
*/
static void graphVisitedSetDestroy(GraphVisitedSet *pSet){
  if( pSet ){
    sqlite3_free(pSet->aNodes);
    sqlite3_free(pSet);
  }
}

/*
** Depth-first search with cycle detection.
** Recursive implementation with configurable depth limits.
** Memory allocation: Uses sqlite3_malloc() for visited set and path.
** Returns: SQLITE_OK on success, error codes on failure.
*/
int graphDFS(GraphVtab *pVtab, sqlite3_int64 iStartId, int nMaxDepth,
             char **pzPath){
  GraphVisitedSet *pVisited = 0;
  GraphNode *pStartNode;
  int rc = SQLITE_OK;
  
  assert( pVtab!=0 );
  assert( pzPath!=0 );
  
  *pzPath = 0;
  
  /* Validate start node exists */
  pStartNode = graphFindNode(pVtab, iStartId);
  if( pStartNode==0 ){
    testcase( pStartNode==0 );  /* Start node not found */
    return SQLITE_NOTFOUND;
  }
  
  /* Create visited set */
  pVisited = graphVisitedSetCreate();
  if( pVisited==0 ){
    return SQLITE_NOMEM;
  }
  
  /* Perform DFS traversal */
  rc = graphDFSRecursive(pVtab, iStartId, nMaxDepth, 0, pVisited, pzPath);
  
  /* Close JSON array */
  if( rc==SQLITE_OK && *pzPath ){
    char *zNewPath = sqlite3_mprintf("%s]", *pzPath);
    sqlite3_free(*pzPath);
    *pzPath = zNewPath;
    if( *pzPath==0 ){
      testcase( *pzPath==0 );  /* Final path allocation failed */
      rc = SQLITE_NOMEM;
    }
  }
  
  /* Cleanup */
  graphVisitedSetDestroy(pVisited);
  
  return rc;
}

/*
** Recursive DFS implementation.
** Explores graph depth-first with cycle detection and depth limiting.
** Path tracking: Builds JSON array of visited node IDs.
*/
static int graphDFSRecursive(GraphVtab *pVtab, sqlite3_int64 iNodeId, 
                            int nMaxDepth, int nCurrentDepth,
                            GraphVisitedSet *pVisited, char **pzPath){
  char *zNewPath = 0;
  int rc;
  char *zSql;
  sqlite3_stmt *pStmt;

  assert( pVtab!=0 );
  assert( pVisited!=0 );
  assert( pzPath!=0 );
  
  if( nMaxDepth>=0 && nCurrentDepth>=nMaxDepth ){
    return SQLITE_OK;
  }
  
  if( graphVisitedSetContains(pVisited, iNodeId) ){
    return SQLITE_OK;
  }
  
  rc = graphVisitedSetAdd(pVisited, iNodeId);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  
  if( *pzPath==0 ){
    *pzPath = sqlite3_mprintf("[%lld", iNodeId);
  } else {
    zNewPath = sqlite3_mprintf("%s,%lld", *pzPath, iNodeId);
    sqlite3_free(*pzPath);
    *pzPath = zNewPath;
  }
  
  if( *pzPath==0 ){
    return SQLITE_NOMEM;
  }
  
  zSql = sqlite3_mprintf("SELECT to_id FROM %s_edges WHERE from_id = %lld", pVtab->zTableName, iNodeId);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ) return rc;

  while( sqlite3_step(pStmt)==SQLITE_ROW ){
    sqlite3_int64 to_id = sqlite3_column_int64(pStmt, 0);
    rc = graphDFSRecursive(pVtab, to_id, nMaxDepth, 
                          nCurrentDepth + 1, pVisited, pzPath);
    if( rc!=SQLITE_OK ){
      sqlite3_finalize(pStmt);
      return rc;
    }
  }
  sqlite3_finalize(pStmt);
  
  return SQLITE_OK;
}

int graphBFS(GraphVtab *pVtab, sqlite3_int64 iStartId, int nMaxDepth,
             char **pzPath){
  Queue *pQueue = 0;
  GraphVisitedSet *pVisited = 0;
  GraphDepthInfo *pDepthList = 0;
  GraphDepthInfo *pDepthInfo;
  sqlite3_int64 iCurrentId;
  int nCurrentDepth;
  char *zNewPath = 0;
  int rc = SQLITE_OK;
  char *zSql;
  sqlite3_stmt *pStmt;

  assert( pVtab!=0 );
  assert( pzPath!=0 );
  
  *pzPath = 0;
  
  pQueue = graphQueueCreate();
  if( pQueue==0 ){
    return SQLITE_NOMEM;
  }
  
  pVisited = graphVisitedSetCreate();
  if( pVisited==0 ){
    graphQueueDestroy(pQueue);
    return SQLITE_NOMEM;
  }
  
  rc = graphQueueEnqueue(pQueue, iStartId);
  if( rc!=SQLITE_OK ){
    goto bfs_cleanup;
  }
  
  rc = graphVisitedSetAdd(pVisited, iStartId);
  if( rc!=SQLITE_OK ){
    goto bfs_cleanup;
  }
  
  pDepthInfo = sqlite3_malloc(sizeof(*pDepthInfo));
  if( pDepthInfo==0 ){
    rc = SQLITE_NOMEM;
    goto bfs_cleanup;
  }
  pDepthInfo->iNodeId = iStartId;
  pDepthInfo->nDepth = 0;
  pDepthInfo->pNext = pDepthList;
  pDepthList = pDepthInfo;
  
  *pzPath = sqlite3_mprintf("[%lld", iStartId);
  if( *pzPath==0 ){
    rc = SQLITE_NOMEM;
    goto bfs_cleanup;
  }
  
  while( graphQueueDequeue(pQueue, &iCurrentId)==SQLITE_OK ){
    nCurrentDepth = 0;
    for( pDepthInfo=pDepthList; pDepthInfo; pDepthInfo=pDepthInfo->pNext ){
      if( pDepthInfo->iNodeId==iCurrentId ){
        nCurrentDepth = pDepthInfo->nDepth;
        break;
      }
    }
    
    if( nMaxDepth>=0 && nCurrentDepth>=nMaxDepth ){
      continue;
    }
    
    zSql = sqlite3_mprintf("SELECT to_id FROM %s_edges WHERE from_id = %lld", pVtab->zTableName, iCurrentId);
    rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
    sqlite3_free(zSql);
    if( rc!=SQLITE_OK ) continue;

    while( sqlite3_step(pStmt)==SQLITE_ROW ){
      sqlite3_int64 to_id = sqlite3_column_int64(pStmt, 0);
      if( !graphVisitedSetContains(pVisited, to_id) ){
        rc = graphVisitedSetAdd(pVisited, to_id);
        if( rc!=SQLITE_OK ){
          sqlite3_finalize(pStmt);
          goto bfs_cleanup;
        }
        
        rc = graphQueueEnqueue(pQueue, to_id);
        if( rc!=SQLITE_OK ){
          sqlite3_finalize(pStmt);
          goto bfs_cleanup;
        }
        
        pDepthInfo = sqlite3_malloc(sizeof(*pDepthInfo));
        if( pDepthInfo==0 ){
          rc = SQLITE_NOMEM;
          sqlite3_finalize(pStmt);
          goto bfs_cleanup;
        }
        pDepthInfo->iNodeId = to_id;
        pDepthInfo->nDepth = nCurrentDepth + 1;
        pDepthInfo->pNext = pDepthList;
        pDepthList = pDepthInfo;
        
        zNewPath = sqlite3_mprintf("%s,%lld", *pzPath, to_id);
        sqlite3_free(*pzPath);
        *pzPath = zNewPath;
        if( *pzPath==0 ){
          rc = SQLITE_NOMEM;
          sqlite3_finalize(pStmt);
          goto bfs_cleanup;
        }
      }
    }
    sqlite3_finalize(pStmt);
  }
  
  if( rc==SQLITE_OK && *pzPath ){
    zNewPath = sqlite3_mprintf("%s]", *pzPath);
    sqlite3_free(*pzPath);
    *pzPath = zNewPath;
    if( *pzPath==0 ){
      rc = SQLITE_NOMEM;
    }
  }
  
bfs_cleanup:
  while( pDepthList ){
    pDepthInfo = pDepthList;
    pDepthList = pDepthList->pNext;
    sqlite3_free(pDepthInfo);
  }
  
  graphQueueDestroy(pQueue);
  graphVisitedSetDestroy(pVisited);
  
  return rc;
}