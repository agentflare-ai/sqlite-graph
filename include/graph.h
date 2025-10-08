/*
** SQLite Graph Database Extension - Core Definitions
**
** This file contains the core data structures and function declarations
** for the SQLite graph database extension. All structures follow SQLite
** naming conventions with Hungarian notation.
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes (SQLITE_OK, etc.)
** Thread safety: Extension supports SQLite threading modes
*/
#ifndef GRAPH_H
#define GRAPH_H

#include "sqlite3.h"

/*
** SQLite-style testcase macro for coverage tracking.
** Marks boundary conditions and error paths for test coverage.
*/
#ifndef testcase
# define testcase(X)  /* No-op in release builds */
#endif

/*
** Enhanced graph node structure with label support for Cypher compatibility.
** Each node has a unique identifier, multiple labels, and optional JSON properties.
** Nodes are stored in a linked list for O(1) insertion with label indexing.
*/
typedef struct GraphNode GraphNode;
struct GraphNode {
  sqlite3_int64 iNodeId;  /* Unique node identifier */
  char **azLabels;        /* Array of node labels (Person, Company, etc.) */
  int nLabels;            /* Number of labels */
  char *zProperties;      /* JSON properties string (sqlite3_malloc'd) */
  GraphNode *pNext;       /* Next node in linked list */
  GraphNode *pLabelNext;  /* Next node with same label (for indexing) */
};

/*
** Enhanced graph edge structure with relationship type support for Cypher.
** Edges connect two nodes with type, optional weight and properties.
** Stored as linked list with adjacency list optimization.
*/
typedef struct GraphEdge GraphEdge;
struct GraphEdge {
  sqlite3_int64 iEdgeId;  /* Unique edge identifier */
  sqlite3_int64 iFromId;  /* Source node ID */
  sqlite3_int64 iToId;    /* Target node ID */
  char *zType;            /* Relationship type (KNOWS, WORKS_AT, etc.) */
  double rWeight;         /* Edge weight (default 1.0) */
  char *zProperties;      /* JSON properties string (sqlite3_malloc'd) */
  GraphEdge *pNext;       /* Next edge in linked list */
  GraphEdge *pFromNext;   /* Next edge from same source node */
  GraphEdge *pToNext;     /* Next edge to same target node */
};

/*
** Forward declarations for schema structures
*/
typedef struct CypherSchema CypherSchema;

/*
** Enhanced graph virtual table structure with schema and indexing support.
** All graph operations are performed through this interface.
** Memory management: Reference counted with proper cleanup.
*/
typedef struct GraphVtab GraphVtab;
struct GraphVtab {
  sqlite3_vtab base;      /* Base class - must be first */
  sqlite3 *pDb;           /* Database connection */
  char *zDbName;          /* Database name ("main", "temp", etc.) */
  char *zTableName;       /* Name of the virtual table */
  char *zNodeTableName;   /* Name of the backing nodes table */
  char *zEdgeTableName;   /* Name of the backing edges table */
  int nRef;               /* Reference count */
  void *pLabelIndex;  /* Label-based node index */
  void *pPropertyIndex; /* Property-based index */
  CypherSchema *pSchema;  /* Schema information for labels/types */
};

/* A global pointer to the graph virtual table. Not ideal, but simple. */
extern GraphVtab *pGraph;

/*
** Graph cursor structure for virtual table iteration.
** Subclass of sqlite3_vtab_cursor following SQLite patterns.
*/
typedef struct GraphCursor GraphCursor;
struct GraphCursor {
  sqlite3_vtab_cursor base;  /* Base class - must be first */
  GraphVtab *pVtab;         /* Pointer to virtual table */
  int iRowid;               /* Current row ID */
  sqlite3_stmt *pNodeStmt;  /* Statement for node iteration */
  sqlite3_stmt *pEdgeStmt;  /* Statement for edge iteration */
  int iIterMode;            /* 0=nodes, 1=edges */
};

/*
** Visited node structure for BFS
*/
typedef struct VisitedNode VisitedNode;
struct VisitedNode {
  sqlite3_int64 iNodeId;
  VisitedNode *pNext;
};

/*
** Depth info for BFS
*/
typedef struct GraphDepthInfo GraphDepthInfo;
struct GraphDepthInfo {
  sqlite3_int64 iNodeId;    /* Node ID */
  int nDepth;               /* Depth from start node */
  GraphDepthInfo *pNext;    /* Next in linked list */
};

/*
** Core storage function declarations.
** All functions return SQLite error codes and follow SQLite patterns.
*/


/*
** Add a node to the graph. Return SQLITE_OK on success.
** Memory allocation: Uses sqlite3_malloc() for internal structures.
** Error conditions: Returns SQLITE_NOMEM on OOM, SQLITE_CONSTRAINT on 
**                   duplicate ID.
** I/O errors: Returns SQLITE_IOERR on database write failure.
*/
int graphAddNode(GraphVtab *pVtab, sqlite3_int64 iNodeId, 
                 const char *zProperties);

/*
** Remove a node from the graph along with all connected edges.
** Return SQLITE_OK on success, SQLITE_NOTFOUND if node doesn't exist.
** Memory management: Frees all associated memory with sqlite3_free().
*/
int graphRemoveNode(GraphVtab *pVtab, sqlite3_int64 iNodeId);

/*
** Retrieve node properties. Caller must sqlite3_free() the returned string.
** Returns SQLITE_OK and sets *pzProperties to allocated string.
** Returns SQLITE_NOTFOUND if node doesn't exist.
** Memory allocation: Allocates new string with sqlite3_malloc().
*/
int graphGetNode(GraphVtab *pVtab, sqlite3_int64 iNodeId, 
                 char **pzProperties);

/*
** Add edge between two nodes. Return SQLITE_OK on success.
** Both nodes must exist before adding edge.
** Error conditions: SQLITE_CONSTRAINT if nodes don't exist.
** Note: This is the legacy interface - use graphAddEdgeWithType for new code.
*/
int graphAddEdge(GraphVtab *pVtab, sqlite3_int64 iFromId, 
                 sqlite3_int64 iToId, double rWeight, 
                 const char *zProperties);

/*
** Remove specific edge between two nodes.
** Returns SQLITE_OK on success, SQLITE_NOTFOUND if edge doesn't exist.
*/
int graphRemoveEdge(GraphVtab *pVtab, sqlite3_int64 iFromId, 
                    sqlite3_int64 iToId);

/*
** Update existing node properties.
** Returns SQLITE_OK on success, SQLITE_NOTFOUND if node doesn't exist.
** Memory management: Frees old properties, allocates new ones.
*/
int graphUpdateNode(GraphVtab *pVtab, sqlite3_int64 iNodeId, 
                    const char *zProperties);

/*
** Retrieve edge properties and weight.
** Caller must sqlite3_free() the returned properties string.
** Returns SQLITE_OK and sets outputs, SQLITE_NOTFOUND if edge doesn't exist.
*/
int graphGetEdge(GraphVtab *pVtab, sqlite3_int64 iFromId, 
                 sqlite3_int64 iToId, double *prWeight, 
                 char **pzProperties);

/*
** Utility functions for graph properties.
** These provide O(1) access to cached counts.
*/
int graphCountNodes(GraphVtab *pVtab);
int graphCountEdges(GraphVtab *pVtab);

/*
** Find a node by ID. Returns pointer to node or NULL if not found.
** Internal function for efficient node lookup.
*/
GraphNode *graphFindNode(GraphVtab *pVtab, sqlite3_int64 iNodeId);

/*
** Find an edge by source and target IDs.
** Returns pointer to edge or NULL if not found.
*/
GraphEdge *graphFindEdge(GraphVtab *pVtab, sqlite3_int64 iFromId, 
                         sqlite3_int64 iToId);

/*
** Graph traversal algorithms.
** These implement standard graph search with SQLite error handling.
*/

/*
** Depth-first search with cycle detection and depth limiting.
** Returns SQLITE_OK and sets *pzPath to JSON array of node IDs.
** Caller must sqlite3_free() the returned path string.
** nMaxDepth: Maximum depth to search (-1 for unlimited)
*/
int graphDFS(GraphVtab *pVtab, sqlite3_int64 iStartId, int nMaxDepth,
             char **pzPath);

/*
** Breadth-first search with level-order traversal.
** Returns SQLITE_OK and sets *pzPath to JSON array of node IDs.
** Caller must sqlite3_free() the returned path string.
** nMaxDepth: Maximum depth to search (-1 for unlimited)
*/
int graphBFS(GraphVtab *pVtab, sqlite3_int64 iStartId, int nMaxDepth,
             char **pzPath);

/*
** Graph algorithms.
** These implement advanced graph analysis algorithms.
*/

/*
** Dijkstra's shortest path algorithm.
** Returns SQLITE_OK and sets *pzPath to JSON array of node IDs.
** If prDistance is not NULL, sets it to the total path distance.
** iEndId: Target node (-1 for distances to all nodes)
*/
int graphDijkstra(GraphVtab *pVtab, sqlite3_int64 iStartId, 
                  sqlite3_int64 iEndId, char **pzPath, double *prDistance);

/*
** Shortest path for unweighted graphs using BFS.
** More efficient than Dijkstra for unweighted graphs.
*/
int graphShortestPathUnweighted(GraphVtab *pVtab, sqlite3_int64 iStartId,
                                sqlite3_int64 iEndId, char **pzPath);

/*
** PageRank algorithm implementation.
** Returns SQLITE_OK and sets *pzResults to JSON object with scores.
** rDamping: Damping factor (typically 0.85)
** nMaxIter: Maximum iterations
** rEpsilon: Convergence threshold
*/
int graphPageRank(GraphVtab *pVtab, double rDamping, int nMaxIter, 
                  double rEpsilon, char **pzResults);

/*
** Degree calculations.
*/
int graphInDegree(GraphVtab *pVtab, sqlite3_int64 iNodeId);
int graphOutDegree(GraphVtab *pVtab, sqlite3_int64 iNodeId);
int graphTotalDegree(GraphVtab *pVtab, sqlite3_int64 iNodeId);
double graphDegreeCentrality(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                            int bDirected);

/*
** Graph properties.
*/
int graphIsConnected(GraphVtab *pVtab);
double graphDensity(GraphVtab *pVtab, int bDirected);

/*
** Advanced graph algorithms.
*/

/*
** Betweenness centrality using Brandes' algorithm.
** Returns SQLITE_OK and sets *pzResults to JSON object with scores.
** Algorithm complexity: O(V*E) for unweighted graphs.
*/
int graphBetweennessCentrality(GraphVtab *pVtab, char **pzResults);

/*
** Closeness centrality calculation.
** Returns SQLITE_OK and sets *pzResults to JSON object with scores.
** Closeness = (n-1) / sum of shortest path distances.
*/
int graphClosenessCentrality(GraphVtab *pVtab, char **pzResults);

/*
** Topological sort using Kahn's algorithm.
** Returns SQLITE_OK and sets *pzOrder to JSON array of node IDs.
** Returns SQLITE_CONSTRAINT if graph contains cycles.
*/
int graphTopologicalSort(GraphVtab *pVtab, char **pzOrder);

/*
** Check if directed graph has cycles.
** Returns 1 if cycles exist, 0 otherwise.
*/
int graphHasCycle(GraphVtab *pVtab);

/*
** Find connected components (for undirected view).
** Returns SQLITE_OK and sets *pzComponents to JSON object.
** Format: {"component_id": [node_ids...], ...}
*/
int graphConnectedComponents(GraphVtab *pVtab, char **pzComponents);

/*
** Find strongly connected components using Tarjan's algorithm.
** Returns SQLITE_OK and sets *pzSCC to JSON array of components.
** Each component is an array of node IDs.
*/
int graphStronglyConnectedComponents(GraphVtab *pVtab, char **pzSCC);

/*
** Enhanced storage functions with label and type support.
** These extend the existing API with Cypher graph concepts.
** For full Cypher functionality, include cypher/cypher-api.h
*/

/*
** Add a node with labels to the graph.
** azLabels: Array of label strings (can be NULL for no labels)
** nLabels: Number of labels in array
*/
int graphAddNodeWithLabels(GraphVtab *pVtab, sqlite3_int64 iNodeId,
                           const char **azLabels, int nLabels,
                           const char *zProperties);

/*
** Add an edge with relationship type to the graph.
** zType: Relationship type string (can be NULL for untyped edge)
*/
int graphAddEdgeWithType(GraphVtab *pVtab, sqlite3_int64 iFromId,
                         sqlite3_int64 iToId, const char *zType,
                         double rWeight, const char *zProperties);

/*
** Find nodes by label using index.
** Returns linked list of nodes with specified label.
*/
GraphNode *graphFindNodesByLabel(GraphVtab *pVtab, const char *zLabel);

/*
** Find edges by relationship type.
** Returns linked list of edges with specified type.
*/
GraphEdge *graphFindEdgesByType(GraphVtab *pVtab, const char *zType);

/*
** Thread-safe global graph access functions.
** Used to manage the global graph variable in a thread-safe manner.
*/
void setGlobalGraph(GraphVtab *pNewGraph);
GraphVtab *getGlobalGraph(void);

#endif /* GRAPH_H */