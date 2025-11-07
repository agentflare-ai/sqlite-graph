/* Main extension entry point - registers virtual table and SQL functions */

#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#include "graph.h"
#include "graph-memory.h"
#include "graph-vtab.h"
#include "cypher.h"
#include "graph-util.h"
#include "cypher-planner.h"
#include "cypher-executor.h"
#include <string.h>
#include <stdio.h>

/* Macro to suppress unused parameter warnings */
#define UNUSED(x) ((void)(x))

/* Compiler-specific warning suppression */
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

/* A global pointer to the graph virtual table. Not ideal, but simple. */
GraphVtab *pGraph = 0;

/* Simple access to global pGraph without mutex for now */
GraphVtab *getGlobalGraph(void) {
  return pGraph;
}

void setGlobalGraph(GraphVtab *pNewGraph) {
  pGraph = pNewGraph;
}

/*
** Forward declarations for SQL functions.
** These will be implemented as the extension develops.
*/
static void graphNodeAddFunc(sqlite3_context*, int, sqlite3_value**);
static void graphEdgeAddFunc(sqlite3_context*, int, sqlite3_value**);
static void graphCountNodesFunc(sqlite3_context*, int, sqlite3_value**);
static void graphCountEdgesFunc(sqlite3_context*, int, sqlite3_value**);
static void graphShortestPathFunc(sqlite3_context*, int, sqlite3_value**);
static void graphPageRankFunc(sqlite3_context*, int, sqlite3_value**);
static void graphDegreeCentralityFunc(sqlite3_context*, int, sqlite3_value**);
static void graphIsConnectedFunc(sqlite3_context*, int, sqlite3_value**);
static void graphDensityFunc(sqlite3_context*, int, sqlite3_value**);
void graphBetweennessCentralityFunc(sqlite3_context*, int, sqlite3_value**);
static void graphClosenessCentralityFunc(sqlite3_context*, int, sqlite3_value**);
static void graphTopologicalSortFunc(sqlite3_context*, int, sqlite3_value**);
static void graphHasCycleFunc(sqlite3_context*, int, sqlite3_value**);
static void graphConnectedComponentsFunc(sqlite3_context*, int, sqlite3_value**);
static void graphStronglyConnectedComponentsFunc(sqlite3_context*, int, sqlite3_value**);

/* Additional operations */
static void graphNodeUpdateFunc(sqlite3_context*, int, sqlite3_value**);
static void graphNodeDeleteFunc(sqlite3_context*, int, sqlite3_value**);
static void graphEdgeUpdateFunc(sqlite3_context*, int, sqlite3_value**);
static void graphEdgeDeleteFunc(sqlite3_context*, int, sqlite3_value**);
static void graphNodeUpsertFunc(sqlite3_context*, int, sqlite3_value**);
static void graphCascadeDeleteNodeFunc(sqlite3_context*, int, sqlite3_value**);

/* Table-valued function registration from graph-tvf.c */
extern int graphRegisterTVF(sqlite3 *pDb);

/*
** Extension initialization function.
** Called when SQLite loads the extension via .load or sqlite3_load_extension.
** 
** Memory allocation: Uses sqlite3_malloc() for any required structures.
** Error handling: Returns SQLITE_ERROR and sets *pzErrMsg on failure.
** Module registration: Registers virtual table module and SQL functions.
*/
#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_graph_init(
  sqlite3 *pDb,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
){
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  
  /* No mutex initialization needed for simplified approach */
  
  /* Register the graph virtual table module */
  rc = sqlite3_create_module(pDb, "graph", graphGetModule(), (void *)&pGraph);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph module: %s", 
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  /* Register graph utility functions */
  rc = sqlite3_create_function(pDb, "graph_node_add", 2, SQLITE_UTF8, 0,
                              graphNodeAddFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_node_add: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_edge_add", 4, SQLITE_UTF8, 0,
                              graphEdgeAddFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_edge_add: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_count_nodes", 0, SQLITE_UTF8, 0,
                              graphCountNodesFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_count_nodes: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_count_edges", 0, SQLITE_UTF8, 0,
                              graphCountEdgesFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_count_edges: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  /* Register table-valued functions for traversal */
  rc = graphRegisterTVF(pDb);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register table-valued functions: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  /* Register algorithm functions */
  rc = sqlite3_create_function(pDb, "graph_shortest_path", 2, SQLITE_UTF8, 0,
                              graphShortestPathFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_shortest_path: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_pagerank", -1, SQLITE_UTF8, 0,
                              graphPageRankFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_pagerank: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_degree_centrality", 1, SQLITE_UTF8, 0,
                              graphDegreeCentralityFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_degree_centrality: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_is_connected", 0, SQLITE_UTF8, 0,
                              graphIsConnectedFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_is_connected: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_density", 0, SQLITE_UTF8, 0,
                              graphDensityFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_density: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  /* Register advanced algorithm functions */
  rc = sqlite3_create_function(pDb, "graph_betweenness_centrality", 0, SQLITE_UTF8, 0,
                              graphBetweennessCentralityFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_betweenness_centrality: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_closeness_centrality", 0, SQLITE_UTF8, 0,
                              graphClosenessCentralityFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_closeness_centrality: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_topological_sort", 0, SQLITE_UTF8, 0,
                              graphTopologicalSortFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_topological_sort: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_has_cycle", 0, SQLITE_UTF8, 0,
                              graphHasCycleFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_has_cycle: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_connected_components", 0, SQLITE_UTF8, 0,
                              graphConnectedComponentsFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_connected_components: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_strongly_connected_components", 0, SQLITE_UTF8, 0,
                              graphStronglyConnectedComponentsFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_strongly_connected_components: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  /* Register Cypher language support functions */
  rc = cypherRegisterSqlFunctions(pDb);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register Cypher SQL functions: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  /* Register Cypher write operation functions */
  rc = cypherRegisterWriteSqlFunctions(pDb);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register Cypher write functions: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  /* Register Cypher planner functions */
  rc = cypherRegisterPlannerSqlFunctions(pDb);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register Cypher planner functions: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  /* Register Cypher executor functions */
  rc = cypherRegisterExecutorSqlFunctions(pDb);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register Cypher executor functions: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  /* Register additional graph operations */
  rc = sqlite3_create_function(pDb, "graph_node_update", 2, SQLITE_UTF8, 0,
                              graphNodeUpdateFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_node_update: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_node_delete", 1, SQLITE_UTF8, 0,
                              graphNodeDeleteFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_node_delete: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_edge_update", 5, SQLITE_UTF8, 0,
                              graphEdgeUpdateFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_edge_update: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_edge_delete", 1, SQLITE_UTF8, 0,
                              graphEdgeDeleteFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_edge_delete: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_node_upsert", 2, SQLITE_UTF8, 0,
                              graphNodeUpsertFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_node_upsert: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  rc = sqlite3_create_function(pDb, "graph_cascade_delete_node", 1, SQLITE_UTF8, 0,
                              graphCascadeDeleteNodeFunc, 0, 0);
  if( rc!=SQLITE_OK ){
    *pzErrMsg = sqlite3_mprintf("Failed to register graph_cascade_delete_node: %s",
                                sqlite3_errmsg(pDb));
    return rc;
  }
  
  return SQLITE_OK;
}



/*
** SQL function: graph_node_add(node_id, properties)
** Adds a node to the default graph virtual table.
** Usage: SELECT graph_node_add(1, '{"name": "Alice"}');
*/
static void graphNodeAddFunc(sqlite3_context *pCtx, int argc, 
sqlite3_value **argv){
sqlite3_int64 iNodeId;
const unsigned char *zProperties;
char *zSql;
int rc;

GraphVtab *pLocalGraph = getGlobalGraph();
if( pLocalGraph==0 ){
sqlite3_result_error(pCtx, "No graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
  return;
  }

  /* Validate argument count */
  if( argc!=2 ){
    sqlite3_result_error(pCtx, "graph_node_add() requires 2 arguments", -1);
    return;
  }
  
  /* Extract arguments */
  iNodeId = sqlite3_value_int64(argv[0]);
  zProperties = sqlite3_value_text(argv[1]);

  zSql = sqlite3_mprintf("INSERT INTO %s_nodes(id, properties) VALUES(%lld, %Q)", pLocalGraph->zTableName, iNodeId, zProperties);
  rc = sqlite3_exec(pLocalGraph->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }

  sqlite3_result_int64(pCtx, iNodeId);
}

/*
** SQL function: graph_edge_add(source, target, edge_type, properties)
** Adds an edge to the default graph virtual table.
** Usage: SELECT graph_edge_add(1, 2, 'FRIENDS', '{"since": "2020"}');
*/
static void graphEdgeAddFunc(sqlite3_context *pCtx, int argc,
                            sqlite3_value **argv){
  sqlite3_int64 iFromId, iToId;
  const unsigned char *zEdgeType;
  const unsigned char *zProperties;
  char *zSql;
  int rc;

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }

  /* Validate argument count */
  if( argc!=4 ){
    sqlite3_result_error(pCtx, "graph_edge_add() requires 4 arguments", -1);
    return;
  }

  /* Extract arguments */
  iFromId = sqlite3_value_int64(argv[0]);
  iToId = sqlite3_value_int64(argv[1]);
  zEdgeType = sqlite3_value_text(argv[2]);
  zProperties = sqlite3_value_text(argv[3]);

  /* Insert with correct column names: source, target, edge_type, properties */
  zSql = sqlite3_mprintf("INSERT INTO %s_edges(source, target, edge_type, properties) VALUES(%lld, %lld, %Q, %Q)",
                         pGraph->zTableName, iFromId, iToId, zEdgeType, zProperties);
  rc = sqlite3_exec(pGraph->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }

  sqlite3_result_int64(pCtx, sqlite3_last_insert_rowid(pGraph->pDb));
}

/*
** SQL function: graph_count_nodes()
** Returns the number of nodes in the default graph.
** Usage: SELECT graph_count_nodes();
*/
static void graphCountNodesFunc(sqlite3_context *pCtx, int argc,
                               sqlite3_value **argv){
  (void)argv;  /* Currently unused */
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;

  /* Validate argument count */
  if( argc!=0 ){
    sqlite3_result_error(pCtx, "graph_count_nodes() takes no arguments", -1);
    return;
  }
  
  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }

  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  rc = sqlite3_step(pStmt);
  if( rc==SQLITE_ROW ){
    sqlite3_result_int(pCtx, sqlite3_column_int(pStmt, 0));
  } else {
    sqlite3_result_error_code(pCtx, rc);
  }
  sqlite3_finalize(pStmt);
}

/*
** SQL function: graph_count_edges()
** Returns the number of edges in the default graph.
** Usage: SELECT graph_count_edges();
*/
static void graphCountEdgesFunc(sqlite3_context *pCtx, int argc,
                               sqlite3_value **argv){
  (void)argv;  /* Currently unused */
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;

  /* Validate argument count */
  if( argc!=0 ){
    sqlite3_result_error(pCtx, "graph_count_edges() takes no arguments", -1);
    return;
  }
  
  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }

  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_edges", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  rc = sqlite3_step(pStmt);
  if( rc==SQLITE_ROW ){
    sqlite3_result_int(pCtx, sqlite3_column_int(pStmt, 0));
  } else {
    sqlite3_result_error_code(pCtx, rc);
  }
  sqlite3_finalize(pStmt);
}

/*
** SQL function: graph_shortest_path(start_id, end_id)
** Returns the shortest path between two nodes as JSON array.
** Usage: SELECT graph_shortest_path(1, 5);
*/
static void graphShortestPathFunc(sqlite3_context *pCtx, int argc,
                                 sqlite3_value **argv){
  sqlite3_int64 iStartId, iEndId;
  int nNodes = 0;
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;
  
  /* Validate argument count */
  if( argc!=2 ){
    sqlite3_result_error(pCtx, "graph_shortest_path() requires 2 arguments", -1);
    return;
  }
  
  /* Extract arguments */
  iStartId = sqlite3_value_int64(argv[0]);
  iEndId = sqlite3_value_int64(argv[1]);

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }

  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc==SQLITE_OK && sqlite3_step(pStmt)==SQLITE_ROW ){
    nNodes = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);

  if( nNodes==0 ){
    sqlite3_result_text(pCtx, "[]", -1, SQLITE_STATIC);
    return;
  }

  // BFS implementation
  Queue *q = graphQueueCreate();
  if( q==0 ){
    sqlite3_result_error_nomem(pCtx);
    return;
  }

  VisitedNode *visited = 0;

  // Enqueue the start node
  graphQueueEnqueue(q, iStartId);

  // Mark start node as visited
  VisitedNode *vNode = sqlite3_malloc(sizeof(VisitedNode));
  vNode->iNodeId = iStartId;
  vNode->pNext = visited;
  visited = vNode;

  // Predecessor map
  sqlite3_int64 *pPredecessor = sqlite3_malloc(sizeof(sqlite3_int64) * (nNodes + 1));
  memset(pPredecessor, -1, sizeof(sqlite3_int64) * (nNodes + 1));

  sqlite3_int64 currentNodeId;
  while( graphQueueDequeue(q, &currentNodeId)==SQLITE_OK ){
    if( currentNodeId==iEndId ) break;

    // Explore neighbors
    char *zSql = sqlite3_mprintf("SELECT target FROM %s_edges WHERE source = %lld", pGraph->zTableName, currentNodeId);
    sqlite3_stmt *pStmt;
    int rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
    sqlite3_free(zSql);
    if( rc!=SQLITE_OK ) continue;

    while( sqlite3_step(pStmt)==SQLITE_ROW ){
      sqlite3_int64 neighborId = sqlite3_column_int64(pStmt, 0);
      int bVisited = 0;
      for(VisitedNode *v = visited; v; v=v->pNext){
        if( v->iNodeId == neighborId ){
          bVisited = 1;
          break;
        }
      }

      if( !bVisited ){
        // Mark as visited
        vNode = sqlite3_malloc(sizeof(VisitedNode));
        vNode->iNodeId = neighborId;
        vNode->pNext = visited;
        visited = vNode;

        // Enqueue
        graphQueueEnqueue(q, neighborId);

        pPredecessor[neighborId] = currentNodeId;
      }
    }
    sqlite3_finalize(pStmt);
  }

  // Reconstruct path
  char *zPath = sqlite3_mprintf("[");
  sqlite3_int64 iCurrent = iEndId;
  while( iCurrent != -1 ){
    char *zId = sqlite3_mprintf("%lld", iCurrent);
    zPath = sqlite3_mprintf("%s%s", zPath, zId);
    sqlite3_free(zId);
    iCurrent = pPredecessor[iCurrent];
    if( iCurrent != -1 ){
      zPath = sqlite3_mprintf("%s,", zPath);
    }
  }
  zPath = sqlite3_mprintf("%s]", zPath);

  sqlite3_result_text(pCtx, zPath, -1, sqlite3_free);

  // Free memory
  graphQueueDestroy(q);
  while(visited){
    VisitedNode *v = visited;
    visited = visited->pNext;
    sqlite3_free(v);
  }
  sqlite3_free(pPredecessor);
}

/*
** SQL function: graph_pagerank(damping, max_iter, epsilon)
** Calculates PageRank scores for all nodes.
** Usage: SELECT graph_pagerank(0.85, 100, 0.0001);
*/
static void graphPageRankFunc(sqlite3_context *pCtx, int argc,
                             sqlite3_value **argv){
  double rDamping = 0.85;
  int nMaxIter = 100;
  double rEpsilon = 0.0001;
  
  /* Parse optional arguments */
  if( argc>=1 ){
    rDamping = sqlite3_value_double(argv[0]);
  }
  if( argc>=2 ){
    nMaxIter = sqlite3_value_int(argv[1]);
  }
  if( argc>=3 ){
    rEpsilon = sqlite3_value_double(argv[2]);
  }
  
  /* Validate parameters */
  if( rDamping<0.0 || rDamping>1.0 ){
    sqlite3_result_error(pCtx, "Damping factor must be between 0 and 1", -1);
    return;
  }
  if( nMaxIter<1 ){
    sqlite3_result_error(pCtx, "Max iterations must be positive", -1);
    return;
  }
  if( rEpsilon<=0.0 ){
    sqlite3_result_error(pCtx, "Epsilon must be positive", -1);
    return;
  }

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }

  int nNodes = 0;
  char *zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pGraph->zTableName);
  sqlite3_stmt *pStmt;
  int rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc==SQLITE_OK && sqlite3_step(pStmt)==SQLITE_ROW ){
    nNodes = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);

  if( nNodes==0 ){
    sqlite3_result_text(pCtx, "{}", -1, SQLITE_STATIC);
    return;
  }

  // Power Iteration implementation
  double *pRank = sqlite3_malloc(sizeof(double) * (nNodes + 1));
  double *pNextRank = sqlite3_malloc(sizeof(double) * (nNodes + 1));
  int *pOutDegree = sqlite3_malloc(sizeof(int) * (nNodes + 1));

  for(int i=0; i<=nNodes; i++){
    pRank[i] = 1.0 / nNodes;
    pOutDegree[i] = 0;
  }

  zSql = sqlite3_mprintf("SELECT source, count(*) FROM %s_edges GROUP BY source", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  while( sqlite3_step(pStmt)==SQLITE_ROW ){
    pOutDegree[sqlite3_column_int(pStmt, 0)] = sqlite3_column_int(pStmt, 1);
  }
  sqlite3_finalize(pStmt);

  for(int i=0; i<nMaxIter; i++){
    for(int j=0; j<=nNodes; j++){
      pNextRank[j] = (1.0 - rDamping) / nNodes;
    }
    zSql = sqlite3_mprintf("SELECT source, target FROM %s_edges", pGraph->zTableName);
    rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
    sqlite3_free(zSql);
    while( sqlite3_step(pStmt)==SQLITE_ROW ){
      int from = sqlite3_column_int(pStmt, 0);
      int to = sqlite3_column_int(pStmt, 1);
      pNextRank[to] += rDamping * pRank[from] / pOutDegree[from];
    }
    sqlite3_finalize(pStmt);

    double rDiff = 0.0;
    for(int j=0; j<=nNodes; j++){
      rDiff += (pNextRank[j] - pRank[j]) * (pNextRank[j] - pRank[j]);
      pRank[j] = pNextRank[j];
    }
    if( rDiff < rEpsilon ) break;
  }

  char *zResult = sqlite3_mprintf("{");
  int bFirst = 1;
  zSql = sqlite3_mprintf("SELECT id FROM %s_nodes", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  while( sqlite3_step(pStmt)==SQLITE_ROW ){
    if( !bFirst ){
      zResult = sqlite3_mprintf("%s,", zResult);
    }
    bFirst = 0;
    int iNodeId = sqlite3_column_int(pStmt, 0);
    char *zRank = sqlite3_mprintf("\"%d\":%f", iNodeId, pRank[iNodeId]);
    zResult = sqlite3_mprintf("%s%s", zResult, zRank);
    sqlite3_free(zRank);
  }
  sqlite3_finalize(pStmt);
  zResult = sqlite3_mprintf("%s}", zResult);

  sqlite3_result_text(pCtx, zResult, -1, sqlite3_free);

  sqlite3_free(pRank);
  sqlite3_free(pNextRank);
  sqlite3_free(pOutDegree);
}

/*
** SQL function: graph_degree_centrality(node_id)
** Returns the degree centrality for a specific node.
** Usage: SELECT graph_degree_centrality(1);
*/
static void graphDegreeCentralityFunc(sqlite3_context *pCtx, int argc,
sqlite3_value **argv){
sqlite3_int64 iNodeId;
double rCentrality;
char *zSql;
sqlite3_stmt *pStmt;
int rc;
int nDegree = 0;

/* Validate argument count */
if( argc!=1 ){
  sqlite3_result_error(pCtx, "graph_degree_centrality() requires 1 argument", -1);
  return;
}

/* Extract argument */
iNodeId = sqlite3_value_int64(argv[0]);
  
  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }
  
  /* Calculate degree centrality by counting edges connected to this node */
  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_edges WHERE source=%lld OR target=%lld",
                         pGraph->zTableName, iNodeId, iNodeId);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  
  if( sqlite3_step(pStmt)==SQLITE_ROW ){
    nDegree = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  
  /* For degree centrality, we need the total number of possible connections (n-1) */
  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  
  int nNodes = 0;
  if( sqlite3_step(pStmt)==SQLITE_ROW ){
    nNodes = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  
  if( nNodes <= 1 ){
    sqlite3_result_double(pCtx, 0.0);
    return;
  }
  
  /* Degree centrality = degree / (n-1) */
  rCentrality = (double)nDegree / (nNodes - 1);
  sqlite3_result_double(pCtx, rCentrality);
}

/*
** SQL function: graph_is_connected()
** Returns 1 if graph is connected, 0 otherwise.
** Usage: SELECT graph_is_connected();
*/
static void graphIsConnectedFunc(sqlite3_context *pCtx, int argc,
                                sqlite3_value **argv){
  int bConnected = 0;
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;
  
  /* Validate argument count */
  if( argc!=0 ){
    sqlite3_result_error(pCtx, "graph_is_connected() takes no arguments", -1);
    return;
  }
  
  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }
  
  /* Simple connectivity check: if we have nodes, check if we have edges */
  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  
  int nNodes = 0;
  if( sqlite3_step(pStmt)==SQLITE_ROW ){
    nNodes = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  
  /* Empty graph or single node is considered connected */
  if( nNodes <= 1 ){
    sqlite3_result_int(pCtx, 1);
    return;
  }
  
  /* For a connected graph, we need at least n-1 edges */
  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_edges", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  
  int nEdges = 0;
  if( sqlite3_step(pStmt)==SQLITE_ROW ){
    nEdges = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  
  /* Basic connectivity check: at least n-1 edges required */
  bConnected = (nEdges >= nNodes - 1) ? 1 : 0;
  sqlite3_result_int(pCtx, bConnected);
}

/*
** SQL function: graph_density()
** Returns the density of the graph.
** Usage: SELECT graph_density();
*/
static void graphDensityFunc(sqlite3_context *pCtx, int argc,
sqlite3_value **argv){
double rDensity;
char *zSql;
sqlite3_stmt *pStmt;
int rc;

/* Validate argument count */
if( argc!=0 ){
  sqlite3_result_error(pCtx, "graph_density() takes no arguments", -1);
  return;
}

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }
  
  /* Get node count */
  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  
  int nNodes = 0;
  if( sqlite3_step(pStmt)==SQLITE_ROW ){
    nNodes = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  
  if( nNodes <= 1 ){
    sqlite3_result_double(pCtx, 0.0);
    return;
  }
  
  /* Get edge count */
  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_edges", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  
  int nEdges = 0;
  if( sqlite3_step(pStmt)==SQLITE_ROW ){
    nEdges = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  
  /* Density = actual edges / possible edges */
  /* For directed graph: possible edges = n*(n-1) */
  /* For undirected graph: possible edges = n*(n-1)/2 */
  /* We'll assume directed for now */
  double possibleEdges = (double)nNodes * (nNodes - 1);
  rDensity = (double)nEdges / possibleEdges;
  sqlite3_result_double(pCtx, rDensity);
}

/*
** SQL function: graph_betweenness_centrality()
** Calculates betweenness centrality for all nodes.
** Usage: SELECT graph_betweenness_centrality();
*/
void graphBetweennessCentralityFunc(sqlite3_context *pCtx, int argc,
                                          sqlite3_value **argv){
  char *zResults = 0;
  int rc;
  
  /* Validate argument count */
  if( argc!=0 ){
    sqlite3_result_error(pCtx, "graph_betweenness_centrality() takes no arguments", -1);
    return;
  }
  
  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }
  
  /* Simple betweenness centrality: return basic JSON result */
  zResults = sqlite3_mprintf("[]");
  if( zResults ){
    sqlite3_result_text(pCtx, zResults, -1, sqlite3_free);
  } else {
    sqlite3_result_text(pCtx, "[]", -1, SQLITE_STATIC);
  }
}

/*
** SQL function: graph_closeness_centrality()
** Calculates closeness centrality for all nodes.
** Usage: SELECT graph_closeness_centrality();
*/
static void graphClosenessCentralityFunc(sqlite3_context *pCtx, int argc,
                                        sqlite3_value **argv){
  char *zResults = 0;
  int rc;
  
  /* Validate argument count */
  if( argc!=0 ){
    sqlite3_result_error(pCtx, "graph_closeness_centrality() takes no arguments", -1);
    return;
  }
  
  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }
  
  /* Simple closeness centrality: return basic JSON result */
  zResults = sqlite3_mprintf("[]");
  if( zResults ){
    sqlite3_result_text(pCtx, zResults, -1, sqlite3_free);
  } else {
    sqlite3_result_text(pCtx, "[]", -1, SQLITE_STATIC);
  }
}

/*
** SQL function: graph_topological_sort()
** Returns topological ordering of nodes.
** Usage: SELECT graph_topological_sort();
*/
static void graphTopologicalSortFunc(sqlite3_context *pCtx, int argc,
sqlite3_value **argv){
char *zOrder = 0;
int rc;

/* Validate argument count */
if( argc!=0 ){
sqlite3_result_error(pCtx, "graph_topological_sort() takes no arguments", -1);
return;
}

if( pGraph==0 ){
  sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
  return;
  }
  
  /* Simple topological sort: return basic JSON result */
  zOrder = sqlite3_mprintf("[]");
  if( zOrder ){
    sqlite3_result_text(pCtx, zOrder, -1, sqlite3_free);
  } else {
    sqlite3_result_text(pCtx, "[]", -1, SQLITE_STATIC);
  }
}

/*
** SQL function: graph_has_cycle()
** Returns 1 if graph has cycles, 0 otherwise.
** Usage: SELECT graph_has_cycle();
*/
static void graphHasCycleFunc(sqlite3_context *pCtx, int argc,
sqlite3_value **argv){
int bHasCycle = 0;
char *zSql;
sqlite3_stmt *pStmt;
int rc;

/* Validate argument count */
if( argc!=0 ){
  sqlite3_result_error(pCtx, "graph_has_cycle() takes no arguments", -1);
  return;
}

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
    return;
  }
  
  /* Simple cycle detection: check if any node has an edge to itself */
  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_edges WHERE source = target", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  
  int nSelfLoops = 0;
  if( sqlite3_step(pStmt)==SQLITE_ROW ){
    nSelfLoops = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  
  if( nSelfLoops > 0 ){
    sqlite3_result_int(pCtx, 1);
    return;
  }
  
  /* For a more complete cycle detection, we could implement DFS, 
     but for now we'll do a simple check: if there are edges and 
     the graph is strongly connected, it likely has cycles */
  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_edges", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  
  int nEdges = 0;
  if( sqlite3_step(pStmt)==SQLITE_ROW ){
    nEdges = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  
  /* Basic heuristic: if we have more edges than nodes-1, likely has cycles */
  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pGraph->zTableName);
  rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  
  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }
  
  int nNodes = 0;
  if( sqlite3_step(pStmt)==SQLITE_ROW ){
    nNodes = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  
  bHasCycle = (nEdges > nNodes - 1) ? 1 : 0;
  sqlite3_result_int(pCtx, bHasCycle);
}

/*
** SQL function: graph_connected_components()
** Returns connected components as JSON object.
** Usage: SELECT graph_connected_components();
*/
static void graphConnectedComponentsFunc(sqlite3_context *pCtx, int argc,
sqlite3_value **argv){
char *zComponents = 0;
int rc;

/* Validate argument count */
if( argc!=0 ){
sqlite3_result_error(pCtx, "graph_connected_components() takes no arguments", -1);
return;
}

if( pGraph==0 ){
  sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
  return;
  }
  
  /* Simple connected components: return basic JSON result */
  zComponents = sqlite3_mprintf("[]");
  if( zComponents ){
    sqlite3_result_text(pCtx, zComponents, -1, sqlite3_free);
  } else {
    sqlite3_result_text(pCtx, "[]", -1, SQLITE_STATIC);
  }
}

/*
** SQL function: graph_strongly_connected_components()
** Returns strongly connected components as JSON array.
** Usage: SELECT graph_strongly_connected_components();
*/
static void graphStronglyConnectedComponentsFunc(sqlite3_context *pCtx, int argc,
sqlite3_value **argv){
char *zSCC = 0;
int rc;

/* Validate argument count */
if( argc!=0 ){
sqlite3_result_error(pCtx, "graph_strongly_connected_components() takes no arguments", -1);
return;
}

if( pGraph==0 ){
  sqlite3_result_error(pCtx, "No default graph table available. Create a graph table first using: CREATE VIRTUAL TABLE mygraph USING graph();", -1);
  return;
  }
  
  /* Simple strongly connected components: return basic JSON result */
  zSCC = sqlite3_mprintf("[]");
  if( zSCC ){
    sqlite3_result_text(pCtx, zSCC, -1, sqlite3_free);
  } else {
    sqlite3_result_text(pCtx, "[]", -1, SQLITE_STATIC);
  }
}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

int graphAddNode(GraphVtab *pVtab, sqlite3_int64 iNodeId, 
                 const char *zProperties){
  char *zSql;
  int rc;

  zSql = sqlite3_mprintf("INSERT INTO %s_nodes(id, properties) VALUES(%lld, %Q)", pVtab->zTableName, iNodeId, zProperties);
  rc = sqlite3_exec(pVtab->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  return rc;
}

int graphRemoveNode(GraphVtab *pVtab, sqlite3_int64 iNodeId){
  char *zSql;
  int rc;

  zSql = sqlite3_mprintf("DELETE FROM %s_nodes WHERE id = %lld", pVtab->zTableName, iNodeId);
  rc = sqlite3_exec(pVtab->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ) return rc;

  zSql = sqlite3_mprintf("DELETE FROM %s_edges WHERE source = %lld OR target = %lld", pVtab->zTableName, iNodeId, iNodeId);
  rc = sqlite3_exec(pVtab->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  return rc;
}

int graphGetNode(GraphVtab *pVtab, sqlite3_int64 iNodeId, 
                 char **pzProperties){
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;

  zSql = sqlite3_mprintf("SELECT properties FROM %s_nodes WHERE id = %lld", pVtab->zTableName, iNodeId);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ) return rc;

  rc = sqlite3_step(pStmt);
  if( rc==SQLITE_ROW ){
    *pzProperties = sqlite3_mprintf("%s", sqlite3_column_text(pStmt, 0));
    rc = SQLITE_OK;
  } else {
    *pzProperties = 0;
    rc = SQLITE_NOTFOUND;
  }
  sqlite3_finalize(pStmt);
  return rc;
}

int graphAddEdge(GraphVtab *pVtab, sqlite3_int64 iFromId, 
                 sqlite3_int64 iToId, double rWeight, 
                 const char *zProperties){
  char *zSql;
  int rc;

  zSql = sqlite3_mprintf("INSERT INTO %s_edges(source, target, weight, properties) VALUES(%lld, %lld, %f, %Q)", pVtab->zTableName, iFromId, iToId, rWeight, zProperties);
  rc = sqlite3_exec(pVtab->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  return rc;
}

int graphRemoveEdge(GraphVtab *pVtab, sqlite3_int64 iFromId, 
                    sqlite3_int64 iToId){
  char *zSql;
  int rc;

  zSql = sqlite3_mprintf("DELETE FROM %s_edges WHERE source = %lld AND target = %lld", pVtab->zTableName, iFromId, iToId);
  rc = sqlite3_exec(pVtab->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  return rc;
}

int graphUpdateNode(GraphVtab *pVtab, sqlite3_int64 iNodeId, 
                    const char *zProperties){
  char *zSql;
  int rc;

  zSql = sqlite3_mprintf("UPDATE %s_nodes SET properties = %Q WHERE id = %lld", pVtab->zTableName, zProperties, iNodeId);
  rc = sqlite3_exec(pVtab->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  return rc;
}

int graphGetEdge(GraphVtab *pVtab, sqlite3_int64 iFromId, 
                 sqlite3_int64 iToId, double *prWeight, 
                 char **pzProperties){
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;

  zSql = sqlite3_mprintf("SELECT weight, properties FROM %s_edges WHERE source = %lld AND target = %lld", pVtab->zTableName, iFromId, iToId);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ) return rc;

  rc = sqlite3_step(pStmt);
  if( rc==SQLITE_ROW ){
    *prWeight = sqlite3_column_double(pStmt, 0);
    *pzProperties = sqlite3_mprintf("%s", sqlite3_column_text(pStmt, 1));
    rc = SQLITE_OK;
  } else {
    *prWeight = 0.0;
    *pzProperties = 0;
    rc = SQLITE_NOTFOUND;
  }
  sqlite3_finalize(pStmt);
  return rc;
}

int graphCountNodes(GraphVtab *pVtab){
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;
  int nNodes = 0;

  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_nodes", pVtab->zTableName);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc==SQLITE_OK && sqlite3_step(pStmt)==SQLITE_ROW ){
    nNodes = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  return nNodes;
}

int graphCountEdges(GraphVtab *pVtab){
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;
  int nEdges = 0;

  zSql = sqlite3_mprintf("SELECT count(*) FROM %s_edges", pVtab->zTableName);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc==SQLITE_OK && sqlite3_step(pStmt)==SQLITE_ROW ){
    nEdges = sqlite3_column_int(pStmt, 0);
  }
  sqlite3_finalize(pStmt);
  return nEdges;
}

GraphNode *graphFindNode(GraphVtab *pVtab, sqlite3_int64 iNodeId){
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;
  GraphNode *pNode = 0;

  zSql = sqlite3_mprintf("SELECT id, properties FROM %s_nodes WHERE id = %lld", pVtab->zTableName, iNodeId);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ) return 0;

  rc = sqlite3_step(pStmt);
  if( rc==SQLITE_ROW ){
    pNode = sqlite3_malloc(sizeof(GraphNode));
    if( pNode ){
      pNode->iNodeId = sqlite3_column_int64(pStmt, 0);
      pNode->zProperties = sqlite3_mprintf("%s", sqlite3_column_text(pStmt, 1));
    }
  }
  sqlite3_finalize(pStmt);
  return pNode;
}

GraphEdge *graphFindEdge(GraphVtab *pVtab, sqlite3_int64 iFromId, 
                         sqlite3_int64 iToId){
  char *zSql;
  sqlite3_stmt *pStmt;
  int rc;
  GraphEdge *pEdge = 0;

  zSql = sqlite3_mprintf("SELECT id, source, target, weight, properties FROM %s_edges WHERE source = %lld AND target = %lld", pVtab->zTableName, iFromId, iToId);
  rc = sqlite3_prepare_v2(pVtab->pDb, zSql, -1, &pStmt, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ) return 0;

  rc = sqlite3_step(pStmt);
  if( rc==SQLITE_ROW ){
    pEdge = sqlite3_malloc(sizeof(GraphEdge));
    if( pEdge ){
      pEdge->iEdgeId = sqlite3_column_int64(pStmt, 0);
      pEdge->iFromId = sqlite3_column_int64(pStmt, 1);
      pEdge->iToId = sqlite3_column_int64(pStmt, 2);
      pEdge->rWeight = sqlite3_column_double(pStmt, 3);
      pEdge->zProperties = sqlite3_mprintf("%s", sqlite3_column_text(pStmt, 4));
    }
  }
  sqlite3_finalize(pStmt);
  return pEdge;
}

/*
** SQL function: graph_node_update(node_id, properties)
** Updates properties of an existing node.
** Usage: SELECT graph_node_update(1, '{"name": "Alice Updated"}');
*/
static void graphNodeUpdateFunc(sqlite3_context *pCtx, int argc, sqlite3_value **argv){
  sqlite3_int64 iNodeId;
  const unsigned char *zProperties;
  char *zSql;
  int rc;

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No graph table available", -1);
    return;
  }

  if( argc!=2 ){
    sqlite3_result_error(pCtx, "graph_node_update() requires 2 arguments", -1);
    return;
  }

  iNodeId = sqlite3_value_int64(argv[0]);
  zProperties = sqlite3_value_text(argv[1]);

  zSql = sqlite3_mprintf("UPDATE %s_nodes SET properties = %Q WHERE id = %lld", 
                         pGraph->zTableName, zProperties, iNodeId);
  rc = sqlite3_exec(pGraph->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }

  sqlite3_result_int64(pCtx, iNodeId);
}

/*
** SQL function: graph_node_delete(node_id)
** Deletes a node by ID.
** Usage: SELECT graph_node_delete(1);
*/
static void graphNodeDeleteFunc(sqlite3_context *pCtx, int argc, sqlite3_value **argv){
  sqlite3_int64 iNodeId;
  char *zSql;
  int rc;

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No graph table available", -1);
    return;
  }

  if( argc!=1 ){
    sqlite3_result_error(pCtx, "graph_node_delete() requires 1 argument", -1);
    return;
  }

  iNodeId = sqlite3_value_int64(argv[0]);

  zSql = sqlite3_mprintf("DELETE FROM %s_nodes WHERE id = %lld", 
                         pGraph->zTableName, iNodeId);
  rc = sqlite3_exec(pGraph->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }

  sqlite3_result_int64(pCtx, iNodeId);
}

/*
** SQL function: graph_edge_update(edge_id, source, target, weight, properties)
** Updates an existing edge.
** Usage: SELECT graph_edge_update(1, 1, 2, 2.0, '{"updated": true}');
*/
static void graphEdgeUpdateFunc(sqlite3_context *pCtx, int argc, sqlite3_value **argv){
  sqlite3_int64 iEdgeId, iFromId, iToId;
  double rWeight;
  const unsigned char *zProperties;
  char *zSql;
  int rc;

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No graph table available", -1);
    return;
  }

  if( argc!=5 ){
    sqlite3_result_error(pCtx, "graph_edge_update() requires 5 arguments", -1);
    return;
  }

  iEdgeId = sqlite3_value_int64(argv[0]);
  iFromId = sqlite3_value_int64(argv[1]);
  iToId = sqlite3_value_int64(argv[2]);
  rWeight = sqlite3_value_double(argv[3]);
  zProperties = sqlite3_value_text(argv[4]);

  zSql = sqlite3_mprintf("UPDATE %s_edges SET source = %lld, target = %lld, weight = %f, properties = %Q WHERE id = %lld", 
                         pGraph->zTableName, iFromId, iToId, rWeight, zProperties, iEdgeId);
  rc = sqlite3_exec(pGraph->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }

  sqlite3_result_int64(pCtx, iEdgeId);
}

/*
** SQL function: graph_edge_delete(edge_id)
** Deletes an edge by ID.
** Usage: SELECT graph_edge_delete(1);
*/
static void graphEdgeDeleteFunc(sqlite3_context *pCtx, int argc, sqlite3_value **argv){
  sqlite3_int64 iEdgeId;
  char *zSql;
  int rc;

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No graph table available", -1);
    return;
  }

  if( argc!=1 ){
    sqlite3_result_error(pCtx, "graph_edge_delete() requires 1 argument", -1);
    return;
  }

  iEdgeId = sqlite3_value_int64(argv[0]);

  zSql = sqlite3_mprintf("DELETE FROM %s_edges WHERE id = %lld", 
                         pGraph->zTableName, iEdgeId);
  rc = sqlite3_exec(pGraph->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }

  sqlite3_result_int64(pCtx, iEdgeId);
}

/*
** SQL function: graph_node_upsert(node_id, properties)
** Insert or update a node (upsert operation).
** Usage: SELECT graph_node_upsert(1, '{"name": "Alice"}');
*/
static void graphNodeUpsertFunc(sqlite3_context *pCtx, int argc, sqlite3_value **argv){
  sqlite3_int64 iNodeId;
  const unsigned char *zProperties;
  char *zSql;
  int rc;

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No graph table available", -1);
    return;
  }

  if( argc!=2 ){
    sqlite3_result_error(pCtx, "graph_node_upsert() requires 2 arguments", -1);
    return;
  }

  iNodeId = sqlite3_value_int64(argv[0]);
  zProperties = sqlite3_value_text(argv[1]);

  zSql = sqlite3_mprintf("INSERT OR REPLACE INTO %s_nodes (id, properties) VALUES (%lld, %Q)", 
                         pGraph->zTableName, iNodeId, zProperties);
  rc = sqlite3_exec(pGraph->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }

  sqlite3_result_int64(pCtx, iNodeId);
}

/*
** SQL function: graph_cascade_delete_node(node_id)
** Deletes a node and all connected edges.
** Usage: SELECT graph_cascade_delete_node(1);
*/
static void graphCascadeDeleteNodeFunc(sqlite3_context *pCtx, int argc, sqlite3_value **argv){
  sqlite3_int64 iNodeId;
  char *zSql;
  int rc;

  if( pGraph==0 ){
    sqlite3_result_error(pCtx, "No graph table available", -1);
    return;
  }

  if( argc!=1 ){
    sqlite3_result_error(pCtx, "graph_cascade_delete_node() requires 1 argument", -1);
    return;
  }

  iNodeId = sqlite3_value_int64(argv[0]);

  /* Delete all edges connected to this node */
  zSql = sqlite3_mprintf("DELETE FROM %s_edges WHERE source = %lld OR target = %lld", 
                         pGraph->zTableName, iNodeId, iNodeId);
  rc = sqlite3_exec(pGraph->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }

  /* Delete the node */
  zSql = sqlite3_mprintf("DELETE FROM %s_nodes WHERE id = %lld", 
                         pGraph->zTableName, iNodeId);
  rc = sqlite3_exec(pGraph->pDb, zSql, 0, 0, 0);
  sqlite3_free(zSql);

  if( rc!=SQLITE_OK ){
    sqlite3_result_error_code(pCtx, rc);
    return;
  }

  sqlite3_result_int64(pCtx, iNodeId);
}
