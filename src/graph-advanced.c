#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
#include "graph.h"
#include "graph-memory.h"
#include <float.h>
#include <string.h>
#include <stdlib.h>

/*
** Stack entry for Tarjan's algorithm.
*/
typedef struct TarjanNode TarjanNode;
struct TarjanNode {
  int iIndex;
  TarjanNode *pNext;
};

/*
** Node index mapping for Brandes' algorithm.
** Maps node IDs to array indices for O(1) access.
*/
typedef struct NodeIndexMap NodeIndexMap;
struct NodeIndexMap {
  sqlite3_int64 *aNodeIds;        /* Array of node IDs */
  int nNodes;                     /* Number of nodes */
};

/*
** Tarjan's algorithm state.
*/
typedef struct TarjanState TarjanState;
struct TarjanState {
  int *aIndex;      /* DFS index for each node */
  int *aLowLink;    /* Lowest index reachable */
  int *aOnStack;    /* Is node on stack? */
  TarjanNode *pStack;  /* Stack of nodes */
  int nIndex;       /* Current DFS index */
  int nSCC;         /* Number of SCCs found */
  char **azSCC;     /* Array of SCC strings */
  NodeIndexMap *pMap;
};

static void tarjanStrongConnect(GraphVtab *pVtab, TarjanState *pState, 
                               int iNodeIdx);
NodeIndexMap *createNodeIndexMap(GraphVtab *pVtab);
int getNodeIndex(NodeIndexMap *pMap, sqlite3_int64 iNodeId);
void freeNodeIndexMap(NodeIndexMap *pMap);

/*
** Create a node index map from the backing table.
** This provides O(1) lookup for node indices in algorithms.
*/
NodeIndexMap *createNodeIndexMap(GraphVtab *pVtab) {
  NodeIndexMap *pMap = sqlite3_malloc(sizeof(NodeIndexMap));
  if (!pMap) return NULL;
  
  pMap->aNodeIds = NULL;
  pMap->nNodes = 0;
  
  char *zSql = sqlite3_mprintf("SELECT id FROM %s_nodes ORDER BY id", pVtab->zTableName);
  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, NULL);
  sqlite3_free(zSql);
  
  if (rc != SQLITE_OK) {
    sqlite3_free(pMap);
    return NULL;
  }
  
  /* Count nodes first */
  while (sqlite3_step(pStmt) == SQLITE_ROW) {
    pMap->nNodes++;
  }
  
  sqlite3_finalize(pStmt);
  
  if (pMap->nNodes == 0) {
    sqlite3_free(pMap);
    return NULL;
  }
  
  /* Allocate array for node IDs */
  pMap->aNodeIds = sqlite3_malloc(pMap->nNodes * sizeof(sqlite3_int64));
  if (!pMap->aNodeIds) {
    sqlite3_free(pMap);
    return NULL;
  }
  
  /* Fill array with node IDs */
  zSql = sqlite3_mprintf("SELECT id FROM %s_nodes ORDER BY id", pVtab->zTableName);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, NULL);
  sqlite3_free(zSql);
  
  if (rc != SQLITE_OK) {
    sqlite3_free(pMap->aNodeIds);
    sqlite3_free(pMap);
    return NULL;
  }
  
  int i = 0;
  while (sqlite3_step(pStmt) == SQLITE_ROW && i < pMap->nNodes) {
    pMap->aNodeIds[i] = sqlite3_column_int64(pStmt, 0);
    i++;
  }
  
  sqlite3_finalize(pStmt);
  return pMap;
}

/*
** Get the index of a node ID in the index map.
** Returns -1 if not found.
*/
int getNodeIndex(NodeIndexMap *pMap, sqlite3_int64 iNodeId) {
  if (!pMap || !pMap->aNodeIds) return -1;
  
  /* Binary search since nodes are ordered by ID */
  int left = 0;
  int right = pMap->nNodes - 1;
  
  while (left <= right) {
    int mid = (left + right) / 2;
    if (pMap->aNodeIds[mid] == iNodeId) {
      return mid;
    } else if (pMap->aNodeIds[mid] < iNodeId) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  
  return -1;
}

/*
** Free a node index map.
*/
void freeNodeIndexMap(NodeIndexMap *pMap) {
  if (pMap) {
    sqlite3_free(pMap->aNodeIds);
    sqlite3_free(pMap);
  }
}

static void tarjanStrongConnect(GraphVtab *pVtab, TarjanState *pState, 
                               int iNodeIdx){
  TarjanNode *pStackNode;
  sqlite3_int64 iNodeId = pState->pMap->aNodeIds[iNodeIdx];
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;
  
  pState->aIndex[iNodeIdx] = pState->nIndex;
  pState->aLowLink[iNodeIdx] = pState->nIndex;
  pState->nIndex++;
  
  pStackNode = sqlite3_malloc(sizeof(*pStackNode));
  if( pStackNode ){
    pStackNode->iIndex = iNodeIdx;
    pStackNode->pNext = pState->pStack;
    pState->pStack = pStackNode;
    pState->aOnStack[iNodeIdx] = 1;
  }
  
  zSql = sqlite3_mprintf("SELECT to_id FROM %s_edges WHERE from_id = %lld", pVtab->zTableName, iNodeId);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ) return;

  while( sqlite3_step(pStmt)==SQLITE_ROW ){
    int iToIdx = getNodeIndex(pState->pMap, sqlite3_column_int64(pStmt, 0));
    if( iToIdx >= 0 ){
      if( pState->aIndex[iToIdx] == -1 ){
        tarjanStrongConnect(pVtab, pState, iToIdx);
        if( pState->aLowLink[iToIdx] < pState->aLowLink[iNodeIdx] ){
          pState->aLowLink[iNodeIdx] = pState->aLowLink[iToIdx];
        }
      } else if( pState->aOnStack[iToIdx] ){
        if( pState->aIndex[iToIdx] < pState->aLowLink[iNodeIdx] ){
          pState->aLowLink[iNodeIdx] = pState->aIndex[iToIdx];
        }
      }
    }
  }
  sqlite3_finalize(pStmt);
  
  if( pState->aLowLink[iNodeIdx] == pState->aIndex[iNodeIdx] ){
    char *zSCC = sqlite3_mprintf("[");
    int first = 1;
    
    while( pState->pStack ){
      TarjanNode *pNext = pState->pStack->pNext;
      int iIdx = pState->pStack->iIndex;
      
      if( zSCC ){
        char *zOld = zSCC;
        zSCC = sqlite3_mprintf("%s%s%lld", zOld,
                              first ? "" : ",",
                              pState->pMap->aNodeIds[iIdx]);
        sqlite3_free(zOld);
        first = 0;
      }
      
      pState->aOnStack[iIdx] = 0;
      sqlite3_free(pState->pStack);
      pState->pStack = pNext;
      
      if( iIdx == iNodeIdx ) break;
    }
    
    if( zSCC ){
      char *zOld = zSCC;
      zSCC = sqlite3_mprintf("%s]", zOld);
      sqlite3_free(zOld);
      
      char **azNew = sqlite3_realloc(pState->azSCC, 
                                    sizeof(char*) * (pState->nSCC + 1));
      if( azNew ){
        pState->azSCC = azNew;
        pState->azSCC[pState->nSCC++] = zSCC;
      } else {
        sqlite3_free(zSCC);
      }
    }
  }
}

int graphStronglyConnectedComponents(GraphVtab *pVtab, char **pzSCC){
  TarjanState state;
  char *zResult = 0;
  int rc = SQLITE_OK;
  int i;
  int nNodes = 0;
  char *zSql;
  sqlite3_stmt *pStmt;

  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pVtab->zTableName);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc==SQLITE_OK && sqlite3_step(pStmt)==SQLITE_ROW ){
    nNodes = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);

  if( nNodes == 0 ){
    *pzSCC = sqlite3_mprintf("[]");
    return *pzSCC ? SQLITE_OK : SQLITE_NOMEM;
  }
  
  memset(&state, 0, sizeof(state));
  state.pMap = createNodeIndexMap(pVtab);
  if( state.pMap==0 ) return SQLITE_NOMEM;
  
  state.aIndex = sqlite3_malloc(sizeof(int) * nNodes);
  state.aLowLink = sqlite3_malloc(sizeof(int) * nNodes);
  state.aOnStack = sqlite3_malloc(sizeof(int) * nNodes);
  
  if( !state.aIndex || !state.aLowLink || !state.aOnStack ){
    rc = SQLITE_NOMEM;
    goto scc_cleanup;
  }
  
  for( i = 0; i < nNodes; i++ ){
    state.aIndex[i] = -1;
    state.aLowLink[i] = -1;
    state.aOnStack[i] = 0;
  }
  
  for( i = 0; i < nNodes; i++ ){
    if( state.aIndex[i] == -1 ){
      tarjanStrongConnect(pVtab, &state, i);
    }
  }
  
  zResult = sqlite3_mprintf("[");
  for( i = 0; i < state.nSCC && zResult; i++ ){
    char *zOld = zResult;
    zResult = sqlite3_mprintf("%s%s%s", zOld,
                             i > 0 ? "," : "",
                             state.azSCC[i]);
    sqlite3_free(zOld);
  }
  
  if( zResult ){
    char *zOld = zResult;
    zResult = sqlite3_mprintf("%s]", zOld);
    sqlite3_free(zOld);
  }
  
  if( zResult==0 ){
    rc = SQLITE_NOMEM;
  } else {
    *pzSCC = zResult;
  }
  
scc_cleanup:
  for( i = 0; i < state.nSCC; i++ ){
    sqlite3_free(state.azSCC[i]);
  }
  sqlite3_free(state.azSCC);
  
  while( state.pStack ){
    TarjanNode *pNext = state.pStack->pNext;
    sqlite3_free(state.pStack);
    state.pStack = pNext;
  }
  
  sqlite3_free(state.aIndex);
  sqlite3_free(state.aLowLink);
  sqlite3_free(state.aOnStack);
  freeNodeIndexMap(state.pMap);
  
  return rc;
}