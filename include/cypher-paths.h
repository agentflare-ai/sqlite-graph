/*
** cypher-paths.h - Variable-length path support for Cypher
**
** This file defines structures and functions for handling
** variable-length paths and optional patterns in Cypher queries.
*/

#ifndef CYPHER_PATHS_H
#define CYPHER_PATHS_H

#include "cypher.h"

/* Path length bounds for variable-length patterns */
typedef struct PathBounds {
    int minLength;      /* Minimum path length (default 1) */
    int maxLength;      /* Maximum path length (-1 for unbounded) */
    int isOptional;     /* 1 if path is optional (0 length allowed) */
} PathBounds;

/* Variable-length path pattern */
typedef struct VariableLengthPath {
    CypherAst *relationshipPattern;  /* Base relationship pattern */
    PathBounds bounds;               /* Length bounds */
    char *pathVariable;              /* Optional path variable name */
} VariableLengthPath;

/* Path matching context */
typedef struct PathMatchContext {
    GraphVtab *pGraph;               /* Graph being searched */
    PathBounds bounds;               /* Current path bounds */
    int currentDepth;                /* Current traversal depth */
    sqlite3_int64 *visitedNodes;     /* Array of visited node IDs */
    int nVisited;                    /* Number of visited nodes */
    int nAllocated;                  /* Allocated size of visitedNodes */
} PathMatchContext;

/* Path result structure */
typedef struct PathResult {
    sqlite3_int64 *nodeIds;          /* Array of node IDs in path */
    sqlite3_int64 *edgeIds;          /* Array of edge IDs in path */
    int pathLength;                  /* Number of edges in path */
    double totalWeight;              /* Sum of edge weights */
    struct PathResult *pNext;        /* Next path in result set */
} PathResult;

/* Function declarations */

/* Parse path bounds from pattern (e.g., "*1..3", "*", "*..5") */
PathBounds cypherParsePathBounds(const char *pattern);

/* Create variable-length path AST node */
CypherAst* cypherCreateVariableLengthPath(CypherAst *relPattern, 
                                         const char *boundsStr);

/* Match variable-length paths in graph */
PathResult* cypherMatchVariableLengthPaths(GraphVtab *pGraph,
                                          sqlite3_int64 startNode,
                                          sqlite3_int64 endNode,
                                          const char *relType,
                                          PathBounds bounds);

/* Find all shortest paths between nodes */
PathResult* cypherFindAllShortestPaths(GraphVtab *pGraph,
                                       sqlite3_int64 startNode,
                                       sqlite3_int64 endNode,
                                       const char *relType);

/* Find single shortest path between nodes */
PathResult* cypherFindShortestPath(GraphVtab *pGraph,
                                   sqlite3_int64 startNode,
                                   sqlite3_int64 endNode,
                                   const char *relType);

/* Check if path exists with given constraints */
int cypherPathExists(GraphVtab *pGraph,
                    sqlite3_int64 startNode,
                    sqlite3_int64 endNode,
                    const char *relType,
                    PathBounds bounds);

/* Free path result */
void cypherPathResultFree(PathResult *path);

/* Free all paths in result set */
void cypherPathResultsFreeAll(PathResult *paths);

/* Convert path to JSON representation */
char* cypherPathToJson(PathResult *path, GraphVtab *pGraph);

/* Optional pattern matching */
typedef struct OptionalPattern {
    CypherAst *pattern;              /* Pattern to match */
    int isOptional;                  /* 1 if OPTIONAL MATCH */
    CypherAst *defaultValue;         /* Default value if no match */
} OptionalPattern;

/* Create optional pattern */
OptionalPattern* cypherCreateOptionalPattern(CypherAst *pattern);

/* Execute optional pattern matching */
int cypherExecuteOptionalPattern(OptionalPattern *optional,
                                GraphVtab *pGraph,
                                void *context);

#endif /* CYPHER_PATHS_H */