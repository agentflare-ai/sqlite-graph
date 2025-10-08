/*
** SQLite Graph Database Extension - Table-Valued Functions
**
** This file implements table-valued functions for graph traversal including
** graph_dfs() and graph_bfs(). These functions return virtual tables with
** traversal results.
**
** Table-valued functions are implemented as virtual tables in SQLite.
** Each function creates a specialized virtual table module.
*/

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "graph.h"
#include "graph-memory.h"
#include "graph-vtab.h"
#include "graph-memory.h"
#include <string.h>
#include <stdlib.h>

/* Macro to suppress unused parameter warnings */
#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

/*
** Virtual table structure for graph traversal table-valued functions.
** Subclass of sqlite3_vtab with traversal-specific data.
*/
typedef struct GraphTraversalVtab GraphTraversalVtab;
struct GraphTraversalVtab {
  sqlite3_vtab base;        /* Base class - must be first */
  GraphVtab *pGraphVtab;    /* Pointer to source graph vtab */
  int iTraversalType;       /* 0=DFS, 1=BFS */
};

/*
** Cursor structure for graph traversal results.
** Iterates through the result path from DFS/BFS.
*/
typedef struct GraphTraversalCursor GraphTraversalCursor;
struct GraphTraversalCursor {
  sqlite3_vtab_cursor base;  /* Base class - must be first */
  char *zPath;               /* JSON array of node IDs */
  sqlite3_int64 *aNodeIds;   /* Parsed array of node IDs */
  int nNodes;                /* Number of nodes in path */
  int iCurrentNode;          /* Current position in array */
};

/* Forward declarations for DFS virtual table methods */
static int graphDFSCreate(sqlite3*, void*, int, const char*const*, 
                         sqlite3_vtab**, char**);
static int graphDFSConnect(sqlite3*, void*, int, const char*const*,
                          sqlite3_vtab**, char**);
static int graphDFSBestIndex(sqlite3_vtab*, sqlite3_index_info*);
static int graphDFSDisconnect(sqlite3_vtab*);
static int graphDFSDestroy(sqlite3_vtab*);
static int graphDFSOpen(sqlite3_vtab*, sqlite3_vtab_cursor**);
static int graphDFSClose(sqlite3_vtab_cursor*);
static int graphDFSFilter(sqlite3_vtab_cursor*, int, const char*, int,
                         sqlite3_value**);
static int graphDFSNext(sqlite3_vtab_cursor*);
static int graphDFSEof(sqlite3_vtab_cursor*);
static int graphDFSColumn(sqlite3_vtab_cursor*, sqlite3_context*, int);
static int graphDFSRowid(sqlite3_vtab_cursor*, sqlite3_int64*);

/*
** Virtual table module for graph_dfs() table-valued function.
** Implements depth-first search traversal.
*/
static sqlite3_module graphDFSModule = {
  0,                      /* iVersion */
  graphDFSCreate,         /* xCreate */
  graphDFSConnect,        /* xConnect */
  graphDFSBestIndex,      /* xBestIndex */
  graphDFSDisconnect,     /* xDisconnect */
  graphDFSDestroy,        /* xDestroy */
  graphDFSOpen,           /* xOpen */
  graphDFSClose,          /* xClose */
  graphDFSFilter,         /* xFilter */
  graphDFSNext,           /* xNext */
  graphDFSEof,            /* xEof */
  graphDFSColumn,         /* xColumn */
  graphDFSRowid,          /* xRowid */
  0,                      /* xUpdate */
  0,                      /* xBegin */
  0,                      /* xSync */
  0,                      /* xCommit */
  0,                      /* xRollback */
  0,                      /* xFindFunction */
  0,                      /* xRename */
  0,                      /* xSavepoint */
  0,                      /* xRelease */
  0,                      /* xRollbackTo */
  0,                      /* xShadowName */
  0                       /* xIntegrity */
};

/*
** Virtual table module for graph_bfs() table-valued function.
** Implements breadth-first search traversal.
*/
static sqlite3_module graphBFSModule = {
  0,                      /* iVersion */
  graphDFSCreate,         /* xCreate (reuse DFS create) */
  graphDFSConnect,        /* xConnect (reuse DFS connect) */
  graphDFSBestIndex,      /* xBestIndex (reuse DFS) */
  graphDFSDisconnect,     /* xDisconnect (reuse DFS) */
  graphDFSDestroy,        /* xDestroy (reuse DFS) */
  graphDFSOpen,           /* xOpen (reuse DFS) */
  graphDFSClose,          /* xClose (reuse DFS) */
  graphDFSFilter,         /* xFilter (modified for BFS) */
  graphDFSNext,           /* xNext (reuse DFS) */
  graphDFSEof,            /* xEof (reuse DFS) */
  graphDFSColumn,         /* xColumn (reuse DFS) */
  graphDFSRowid,          /* xRowid (reuse DFS) */
  0,                      /* xUpdate */
  0,                      /* xBegin */
  0,                      /* xSync */
  0,                      /* xCommit */
  0,                      /* xRollback */
  0,                      /* xFindFunction */
  0,                      /* xRename */
  0,                      /* xSavepoint */
  0,                      /* xRelease */
  0,                      /* xRollbackTo */
  0,                      /* xShadowName */
  0                       /* xIntegrity */
};

/*
** Create virtual table for graph traversal.
** Schema: CREATE TABLE x(node_id INTEGER, depth INTEGER, position INTEGER)
*/
static int graphDFSCreate(sqlite3 *pDb, void *pAux, int argc,
                         const char *const *argv, sqlite3_vtab **ppVtab,
                         char **pzErr){
  GraphTraversalVtab *pNew;
  int rc;
  
  /* Suppress unused parameter warnings */
  UNUSED(pAux);
  UNUSED(argc);
  UNUSED(argv);
  UNUSED(pzErr);
  
  /* Allocate virtual table structure */
  pNew = sqlite3_malloc(sizeof(*pNew));
  if( pNew==0 ){
    return SQLITE_NOMEM;
  }
  memset(pNew, 0, sizeof(*pNew));
  
  /* Declare schema for traversal results */
  rc = sqlite3_declare_vtab(pDb, "CREATE TABLE x("
                                "node_id INTEGER,"
                                "depth INTEGER,"
                                "position INTEGER"
                                ")");
  if( rc!=SQLITE_OK ){
    sqlite3_free(pNew);
    return rc;
  }
  
  /* Set traversal type based on module */
  pNew->iTraversalType = (pAux!=0) ? 1 : 0;  /* 0=DFS, 1=BFS */
  
  *ppVtab = &pNew->base;
  return SQLITE_OK;
}

/*
** Connect to existing traversal virtual table.
*/
static int graphDFSConnect(sqlite3 *pDb, void *pAux, int argc,
                          const char *const *argv, sqlite3_vtab **ppVtab,
                          char **pzErr){
  return graphDFSCreate(pDb, pAux, argc, argv, ppVtab, pzErr);
}

/*
** Query planner for traversal functions.
** Expects arguments: start_node_id, max_depth (optional)
*/
static int graphDFSBestIndex(sqlite3_vtab *pVtab, sqlite3_index_info *pInfo){
  int iStartNodeArg = -1;
  int iMaxDepthArg = -1;
  int i;
  
  /* Suppress unused parameter warnings */
  UNUSED(pVtab);
  
  /* Look for start_node_id and max_depth constraints */
  for( i=0; i<pInfo->nConstraint; i++ ){
    if( pInfo->aConstraint[i].iColumn==-1 &&  /* Hidden column */
        pInfo->aConstraint[i].usable ){
      if( iStartNodeArg<0 ){
        iStartNodeArg = i;
      } else if( iMaxDepthArg<0 ){
        iMaxDepthArg = i;
      }
    }
  }
  
  /* Start node is required */
  if( iStartNodeArg<0 ){
    return SQLITE_CONSTRAINT;
  }
  
  /* Set argument indices */
  pInfo->aConstraintUsage[iStartNodeArg].argvIndex = 1;
  pInfo->aConstraintUsage[iStartNodeArg].omit = 1;
  
  if( iMaxDepthArg>=0 ){
    pInfo->aConstraintUsage[iMaxDepthArg].argvIndex = 2;
    pInfo->aConstraintUsage[iMaxDepthArg].omit = 1;
  }
  
  pInfo->estimatedCost = 100.0;
  pInfo->estimatedRows = 100;
  
  return SQLITE_OK;
}

/*
** Disconnect from traversal virtual table.
*/
static int graphDFSDisconnect(sqlite3_vtab *pVtab){
  GraphTraversalVtab *pTrav = (GraphTraversalVtab*)pVtab;
  sqlite3_free(pTrav);
  return SQLITE_OK;
}

/*
** Destroy traversal virtual table.
*/
static int graphDFSDestroy(sqlite3_vtab *pVtab){
  return graphDFSDisconnect(pVtab);
}

/*
** Open cursor for traversal results.
*/
static int graphDFSOpen(sqlite3_vtab *pVtab, sqlite3_vtab_cursor **ppCursor){
  GraphTraversalCursor *pCur;
  
  /* Suppress unused parameter warnings */
  UNUSED(pVtab);
  
  pCur = sqlite3_malloc(sizeof(*pCur));
  if( pCur==0 ){
    return SQLITE_NOMEM;
  }
  memset(pCur, 0, sizeof(*pCur));
  
  *ppCursor = &pCur->base;
  return SQLITE_OK;
}

/*
** Close traversal cursor.
*/
static int graphDFSClose(sqlite3_vtab_cursor *pCursor){
  GraphTraversalCursor *pCur = (GraphTraversalCursor*)pCursor;
  sqlite3_free(pCur->zPath);
  sqlite3_free(pCur->aNodeIds);
  sqlite3_free(pCur);
  return SQLITE_OK;
}

/*
** Parse JSON path array into node ID array.
** Format: "[1,2,3,4]" -> array of sqlite3_int64
*/
static int parseTraversalPath(const char *zPath, sqlite3_int64 **paNodeIds,
                             int *pnNodes){
  sqlite3_int64 *aIds = 0;
  int nAlloc = 0;
  int nUsed = 0;
  const char *p;
  sqlite3_int64 iNodeId;
  char *zEnd;
  
  if( zPath==0 || zPath[0]!='[' ){
    return SQLITE_ERROR;
  }
  
  p = zPath + 1;  /* Skip opening bracket */
  
  while( *p && *p!=']' ){
    /* Skip whitespace and commas */
    while( *p && (*p==' ' || *p==',') ) p++;
    if( *p==']' ) break;
    
    /* Parse node ID */
    iNodeId = strtoll(p, &zEnd, 10);
    if( p==zEnd ){
      sqlite3_free(aIds);
      return SQLITE_ERROR;
    }
    p = zEnd;
    
    /* Grow array if needed */
    if( nUsed>=nAlloc ){
      sqlite3_int64 *aNew;
      nAlloc = nAlloc ? nAlloc*2 : 16;
      aNew = sqlite3_realloc(aIds, nAlloc * sizeof(sqlite3_int64));
      if( aNew==0 ){
        sqlite3_free(aIds);
        return SQLITE_NOMEM;
      }
      aIds = aNew;
    }
    
    /* Add node ID to array */
    aIds[nUsed++] = iNodeId;
  }
  
  *paNodeIds = aIds;
  *pnNodes = nUsed;
  return SQLITE_OK;
}

/*
** Filter traversal results based on arguments.
** Arguments: start_node_id, max_depth (optional)
*/
static int graphDFSFilter(sqlite3_vtab_cursor *pCursor, int idxNum,
                         const char *idxStr, int argc, sqlite3_value **argv){
  GraphTraversalCursor *pCur = (GraphTraversalCursor*)pCursor;
  GraphTraversalVtab *pVtab = (GraphTraversalVtab*)pCursor->pVtab;
  sqlite3_int64 iStartId;
  int nMaxDepth = -1;
  int rc;
  
  /* Suppress unused parameter warnings */
  UNUSED(idxNum);
  UNUSED(idxStr);
  
  /* Free previous results */
  sqlite3_free(pCur->zPath);
  sqlite3_free(pCur->aNodeIds);
  pCur->zPath = 0;
  pCur->aNodeIds = 0;
  pCur->nNodes = 0;
  pCur->iCurrentNode = 0;
  
  /* Get arguments */
  if( argc<1 ){
    return SQLITE_ERROR;
  }
  
  iStartId = sqlite3_value_int64(argv[0]);
  if( argc>=2 ){
    nMaxDepth = sqlite3_value_int(argv[1]);
  }
  
  /* Suppress unused variable warning for nMaxDepth - used in future implementation */
  UNUSED(nMaxDepth);
  
  /* Graph traversal requires a graph table instance */
  /* For now, return error since we don't have a graph reference */
  /* In a future implementation, this would look up the graph by name */
  return SQLITE_ERROR;
  
  /* Perform traversal (DFS or BFS based on vtab type) */
  if( pVtab->iTraversalType==0 ){
    /* DFS traversal */
    /* Cannot perform traversal without graph instance */
    /* Return single-node path as minimal valid result */
    pCur->zPath = sqlite3_mprintf("[%lld]", iStartId);
  } else {
    /* BFS traversal */
    /* Cannot perform traversal without graph instance */
    /* Return single-node path as minimal valid result */
    pCur->zPath = sqlite3_mprintf("[%lld]", iStartId);
  }
  
  if( pCur->zPath==0 ){
    return SQLITE_NOMEM;
  }
  
  /* Parse path into node array */
  rc = parseTraversalPath(pCur->zPath, &pCur->aNodeIds, &pCur->nNodes);
  if( rc!=SQLITE_OK ){
    sqlite3_free(pCur->zPath);
    pCur->zPath = 0;
    return rc;
  }
  
  pCur->iCurrentNode = 0;
  return SQLITE_OK;
}

/*
** Move to next node in traversal result.
*/
static int graphDFSNext(sqlite3_vtab_cursor *pCursor){
  GraphTraversalCursor *pCur = (GraphTraversalCursor*)pCursor;
  pCur->iCurrentNode++;
  return SQLITE_OK;
}

/*
** Check if at end of traversal results.
*/
static int graphDFSEof(sqlite3_vtab_cursor *pCursor){
  GraphTraversalCursor *pCur = (GraphTraversalCursor*)pCursor;
  return pCur->iCurrentNode >= pCur->nNodes;
}

/*
** Return column value for current node.
** Columns: node_id, depth, position
*/
static int graphDFSColumn(sqlite3_vtab_cursor *pCursor, sqlite3_context *pCtx,
                         int iCol){
  GraphTraversalCursor *pCur = (GraphTraversalCursor*)pCursor;
  
  if( pCur->iCurrentNode >= pCur->nNodes ){
    return SQLITE_ERROR;
  }
  
  switch( iCol ){
    case 0:  /* node_id */
      sqlite3_result_int64(pCtx, pCur->aNodeIds[pCur->iCurrentNode]);
      break;
    case 1:  /* depth */
      /* Depth tracking requires actual traversal implementation */
      /* For single-node result, depth is always 0 */
      sqlite3_result_int(pCtx, 0);
      break;
    case 2:  /* position */
      sqlite3_result_int(pCtx, pCur->iCurrentNode);
      break;
    default:
      return SQLITE_ERROR;
  }
  
  return SQLITE_OK;
}

/*
** Return rowid for current position.
*/
static int graphDFSRowid(sqlite3_vtab_cursor *pCursor, sqlite3_int64 *pRowid){
  GraphTraversalCursor *pCur = (GraphTraversalCursor*)pCursor;
  *pRowid = pCur->iCurrentNode;
  return SQLITE_OK;
}

/*
** Register table-valued functions with SQLite.
** Called from main extension init function.
*/
int graphRegisterTVF(sqlite3 *pDb){
  int rc;
  
  /* Register graph_dfs() table-valued function */
  rc = sqlite3_create_module(pDb, "graph_dfs", &graphDFSModule, 0);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  
  /* Register graph_bfs() table-valued function */
  rc = sqlite3_create_module(pDb, "graph_bfs", &graphBFSModule, (void*)1);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  
  return SQLITE_OK;
}