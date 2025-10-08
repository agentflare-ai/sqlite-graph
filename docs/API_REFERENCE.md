# SQLite Graph Extension API Reference

**Part of the AgentFlare AI ecosystem**

## Table of Contents

1. [Overview](#overview)
2. [Graph Virtual Table](#graph-virtual-table)
3. [Cypher Query Language](#cypher-query-language)
4. [Table-Valued Functions](#table-valued-functions)
5. [Graph Algorithms](#graph-algorithms)
6. [Performance Features](#performance-features)
7. [Error Handling](#error-handling)
8. [C API Reference](#c-api-reference)

## Overview

The SQLite Graph Extension provides graph database functionality within SQLite, supporting the Cypher query language and various graph algorithms. This document provides a comprehensive reference for all public APIs.

### Loading the Extension

```sql
-- Load the extension
.load ./graph_extension

-- Verify installation
SELECT sqlite_version();
SELECT * FROM pragma_function_list WHERE name LIKE 'graph%';
```

## Graph Virtual Table

### Creating a Graph Table

```sql
CREATE VIRTUAL TABLE my_graph USING graph(
    [options]
);
```

**Options:**
- `cache_size`: LRU cache size (default: 1000)
- `max_depth`: Maximum traversal depth (default: 10)
- `thread_pool_size`: Number of worker threads (default: 4)

### Node Operations

#### Adding Nodes

```sql
-- Add a node with properties
INSERT INTO my_graph (
    node_id, labels, properties
) VALUES (
    1,
    '["Person", "Employee"]',
    '{"name": "John Doe", "age": 30}'
);
```

#### Querying Nodes

```sql
-- Find all nodes
SELECT * FROM my_graph WHERE edge_id IS NULL;

-- Find nodes by label
SELECT * FROM my_graph 
WHERE edge_id IS NULL 
  AND json_extract(labels, '$[0]') = 'Person';

-- Find nodes by property
SELECT * FROM my_graph 
WHERE edge_id IS NULL 
  AND json_extract(properties, '$.name') = 'John Doe';
```

#### Updating Nodes

```sql
-- Update node properties
UPDATE my_graph 
SET properties = json_set(properties, '$.age', 31)
WHERE node_id = 1;

-- Add a label
UPDATE my_graph 
SET labels = json_insert(labels, '$[#]', 'Manager')
WHERE node_id = 1;
```

#### Deleting Nodes

```sql
-- Delete a node (must have no edges)
DELETE FROM my_graph WHERE node_id = 1;
```

### Edge Operations

#### Adding Edges

```sql
-- Add an edge between nodes
INSERT INTO my_graph (
    edge_id, from_id, to_id, edge_type, weight, properties
) VALUES (
    1001, 1, 2, 'KNOWS', 1.0,
    '{"since": "2020-01-01"}'
);
```

#### Querying Edges

```sql
-- Find all edges
SELECT * FROM my_graph WHERE edge_id IS NOT NULL;

-- Find edges by type
SELECT * FROM my_graph 
WHERE edge_id IS NOT NULL 
  AND edge_type = 'KNOWS';

-- Find edges from a specific node
SELECT * FROM my_graph 
WHERE edge_id IS NOT NULL 
  AND from_id = 1;
```

#### Updating Edges

```sql
-- Update edge properties
UPDATE my_graph 
SET properties = json_set(properties, '$.strength', 'strong')
WHERE edge_id = 1001;

-- Update edge weight
UPDATE my_graph 
SET weight = 0.8
WHERE edge_id = 1001;
```

#### Deleting Edges

```sql
DELETE FROM my_graph WHERE edge_id = 1001;
```

## Cypher Query Language

The extension supports a substantial subset of the Cypher query language for graph queries.

### Basic Patterns

```cypher
-- Find all nodes
MATCH (n) RETURN n;

-- Find nodes with a specific label
MATCH (n:Person) RETURN n;

-- Find nodes with properties
MATCH (n:Person {name: 'John Doe'}) RETURN n;

-- Pattern matching
MATCH (a:Person)-[:KNOWS]->(b:Person) 
RETURN a.name, b.name;
```

### CREATE Operations

```cypher
-- Create a node
CREATE (n:Person {name: 'Jane Doe', age: 28});

-- Create nodes and relationships
CREATE (a:Person {name: 'Alice'}),
       (b:Person {name: 'Bob'}),
       (a)-[:KNOWS {since: 2020}]->(b);
```

### MERGE Operations

```cypher
-- Create node if it doesn't exist
MERGE (n:Person {name: 'Charlie'});

-- Merge with ON CREATE and ON MATCH
MERGE (n:Person {name: 'David'})
ON CREATE SET n.created = timestamp()
ON MATCH SET n.updated = timestamp();
```

### SET Operations

```cypher
-- Set properties
MATCH (n:Person {name: 'John'})
SET n.age = 31, n.city = 'New York';

-- Set labels
MATCH (n:Person {name: 'Jane'})
SET n:Employee:Manager;
```

### DELETE Operations

```cypher
-- Delete a node and its relationships
MATCH (n:Person {name: 'Old User'})
DETACH DELETE n;

-- Delete only relationships
MATCH (a)-[r:OLD_RELATIONSHIP]->(b)
DELETE r;
```

### WHERE Clauses

```cypher
-- Filter by properties
MATCH (n:Person)
WHERE n.age > 25 AND n.city = 'New York'
RETURN n;

-- Filter by pattern existence
MATCH (a:Person)
WHERE EXISTS((a)-[:MANAGES]->(:Department))
RETURN a;
```

### RETURN and Aggregation

```cypher
-- Return specific properties
MATCH (n:Person)
RETURN n.name, n.age;

-- Aggregation
MATCH (n:Person)
RETURN COUNT(n) as personCount, AVG(n.age) as avgAge;

-- ORDER BY and LIMIT
MATCH (n:Person)
RETURN n
ORDER BY n.age DESC
LIMIT 10;
```

## Table-Valued Functions

### graph_neighbors()

Find immediate neighbors of a node.

```sql
SELECT * FROM graph_neighbors('my_graph', 1)
WHERE direction = 'outgoing';
```

**Parameters:**
- `graph_name`: Name of the graph virtual table
- `node_id`: Starting node ID
- `direction` (optional): 'incoming', 'outgoing', or 'both' (default: 'both')

**Returns:**
- `neighbor_id`: ID of the neighbor node
- `edge_id`: ID of the connecting edge
- `edge_type`: Type of the edge
- `weight`: Edge weight
- `direction`: Direction of the edge

### graph_path()

Find paths between nodes.

```sql
SELECT * FROM graph_path('my_graph', 1, 10)
WHERE length <= 3;
```

**Parameters:**
- `graph_name`: Name of the graph virtual table
- `start_id`: Starting node ID
- `end_id`: Ending node ID
- `max_depth` (optional): Maximum path length (default: 10)

**Returns:**
- `path`: JSON array of node IDs in the path
- `edges`: JSON array of edge IDs in the path
- `length`: Number of edges in the path
- `total_weight`: Sum of edge weights

### graph_subgraph()

Extract a subgraph around a node.

```sql
SELECT * FROM graph_subgraph('my_graph', 1, 2);
```

**Parameters:**
- `graph_name`: Name of the graph virtual table
- `center_id`: Center node ID
- `radius`: Maximum distance from center

**Returns:**
- `node_id`: Node ID in the subgraph
- `distance`: Distance from center node
- `properties`: Node properties

## Graph Algorithms

### Dijkstra's Shortest Path

```sql
SELECT * FROM graph_dijkstra('my_graph', 1, 10);
```

**Parameters:**
- `graph_name`: Name of the graph virtual table
- `start_id`: Starting node ID
- `end_id`: Target node ID (optional, NULL for all nodes)

**Returns:**
- `node_id`: Destination node ID
- `distance`: Shortest distance from start
- `path`: JSON array of node IDs in shortest path
- `previous_node`: Previous node in shortest path

### PageRank

```sql
SELECT * FROM graph_pagerank('my_graph')
ORDER BY rank DESC
LIMIT 10;
```

**Parameters:**
- `graph_name`: Name of the graph virtual table
- `damping_factor` (optional): PageRank damping factor (default: 0.85)
- `iterations` (optional): Number of iterations (default: 100)
- `tolerance` (optional): Convergence tolerance (default: 1e-6)

**Returns:**
- `node_id`: Node ID
- `rank`: PageRank score
- `outgoing_edges`: Number of outgoing edges

### Connected Components

```sql
SELECT * FROM graph_components('my_graph');
```

**Parameters:**
- `graph_name`: Name of the graph virtual table

**Returns:**
- `node_id`: Node ID
- `component_id`: Component identifier
- `component_size`: Number of nodes in the component

### Community Detection (Louvain)

```sql
SELECT * FROM graph_louvain('my_graph');
```

**Parameters:**
- `graph_name`: Name of the graph virtual table
- `resolution` (optional): Resolution parameter (default: 1.0)

**Returns:**
- `node_id`: Node ID
- `community_id`: Community identifier
- `modularity`: Modularity score of the partition

### Centrality Measures

```sql
-- Degree centrality
SELECT * FROM graph_centrality('my_graph', 'degree');

-- Betweenness centrality
SELECT * FROM graph_centrality('my_graph', 'betweenness');

-- Closeness centrality
SELECT * FROM graph_centrality('my_graph', 'closeness');
```

**Parameters:**
- `graph_name`: Name of the graph virtual table
- `centrality_type`: 'degree', 'betweenness', or 'closeness'

**Returns:**
- `node_id`: Node ID
- `centrality`: Centrality score
- `normalized`: Normalized centrality (0-1)

## Performance Features

### Index Creation

```sql
-- Create property index
CREATE INDEX idx_person_name ON my_graph(
    json_extract(properties, '$.name')
) WHERE edge_id IS NULL;

-- Create label index
CREATE INDEX idx_person_label ON my_graph(
    json_extract(labels, '$[0]')
) WHERE edge_id IS NULL;
```

### Query Optimization

```sql
-- Analyze graph statistics
ANALYZE my_graph;

-- View query plan
EXPLAIN QUERY PLAN
SELECT * FROM graph_dijkstra('my_graph', 1, 100);
```

### Bulk Operations

```sql
-- Bulk insert nodes
BEGIN TRANSACTION;
INSERT INTO my_graph (node_id, labels, properties) VALUES
    (1, '["Person"]', '{"name": "User1"}'),
    (2, '["Person"]', '{"name": "User2"}'),
    -- ... more nodes
    (1000, '["Person"]', '{"name": "User1000"}');
COMMIT;

-- Bulk load from CSV
SELECT graph_bulk_load('my_graph', 'nodes.csv', 'edges.csv');
```

### Performance Configuration

```sql
-- Set cache size
PRAGMA graph.cache_size = 10000;

-- Enable parallel execution
PRAGMA graph.parallel_execution = ON;

-- Set thread pool size
PRAGMA graph.thread_pool_size = 8;

-- Enable query plan caching
PRAGMA graph.plan_cache = ON;
```

## Error Handling

### Error Codes

| Code | Name | Description |
|------|------|-------------|
| SQLITE_GRAPH_INVALID_NODE | Invalid Node | Node ID does not exist |
| SQLITE_GRAPH_INVALID_EDGE | Invalid Edge | Edge ID does not exist |
| SQLITE_GRAPH_CYCLE_DETECTED | Cycle Detected | Operation would create a cycle |
| SQLITE_GRAPH_DEPTH_EXCEEDED | Depth Exceeded | Traversal depth limit reached |
| SQLITE_GRAPH_MEMORY_ERROR | Memory Error | Out of memory |
| SQLITE_GRAPH_PARSE_ERROR | Parse Error | Cypher query parse error |

### Error Handling Example

```sql
-- Check for errors
SELECT CASE
    WHEN error_code = 0 THEN 'Success'
    ELSE error_message
END as status
FROM (
    SELECT graph_validate('my_graph') as (error_code, error_message)
);
```

## C API Reference

### Initialization

```c
#include "graph.h"

// Initialize the extension
int sqlite3_graph_init(
    sqlite3 *db,
    char **pzErrMsg,
    const sqlite3_api_routines *pApi
);
```

### Graph Operations

```c
// Create a graph
int graph_create(
    sqlite3 *db,
    const char *zName,
    GraphOptions *pOptions
);

// Open existing graph
int graph_open(
    sqlite3 *db,
    const char *zName,
    Graph **ppGraph
);

// Close graph
int graph_close(Graph *pGraph);
```

### Node Operations

```c
// Add node
int graph_add_node(
    Graph *pGraph,
    sqlite3_int64 iNodeId,
    const char **azLabels,
    int nLabels,
    const char *zProperties
);

// Find node
int graph_find_node(
    Graph *pGraph,
    sqlite3_int64 iNodeId,
    GraphNode **ppNode
);

// Update node
int graph_update_node(
    Graph *pGraph,
    sqlite3_int64 iNodeId,
    const char *zProperties
);

// Delete node
int graph_delete_node(
    Graph *pGraph,
    sqlite3_int64 iNodeId,
    int bDetach
);
```

### Edge Operations

```c
// Add edge
int graph_add_edge(
    Graph *pGraph,
    sqlite3_int64 iEdgeId,
    sqlite3_int64 iFromId,
    sqlite3_int64 iToId,
    const char *zType,
    double rWeight,
    const char *zProperties
);

// Find edge
int graph_find_edge(
    Graph *pGraph,
    sqlite3_int64 iEdgeId,
    GraphEdge **ppEdge
);

// Update edge
int graph_update_edge(
    Graph *pGraph,
    sqlite3_int64 iEdgeId,
    double rWeight,
    const char *zProperties
);

// Delete edge
int graph_delete_edge(
    Graph *pGraph,
    sqlite3_int64 iEdgeId
);
```

### Traversal Operations

```c
// Breadth-first search
int graph_bfs(
    Graph *pGraph,
    sqlite3_int64 iStartId,
    GraphVisitor *pVisitor,
    void *pContext
);

// Depth-first search
int graph_dfs(
    Graph *pGraph,
    sqlite3_int64 iStartId,
    GraphVisitor *pVisitor,
    void *pContext
);

// Find shortest path
int graph_shortest_path(
    Graph *pGraph,
    sqlite3_int64 iStartId,
    sqlite3_int64 iEndId,
    GraphPath **ppPath
);
```

### Memory Management

```c
// Free node
void graph_node_free(GraphNode *pNode);

// Free edge
void graph_edge_free(GraphEdge *pEdge);

// Free path
void graph_path_free(GraphPath *pPath);

// Free result set
void graph_result_free(GraphResult *pResult);
```

### Error Handling

```c
// Get last error
const char *graph_errmsg(Graph *pGraph);

// Get error code
int graph_errcode(Graph *pGraph);

// Clear error
void graph_clear_error(Graph *pGraph);
```

## Examples

### Social Network Analysis

```sql
-- Create social network graph
CREATE VIRTUAL TABLE social_network USING graph();

-- Add users
INSERT INTO social_network (node_id, labels, properties) VALUES
    (1, '["User"]', '{"name": "Alice", "age": 28}'),
    (2, '["User"]', '{"name": "Bob", "age": 32}'),
    (3, '["User"]', '{"name": "Charlie", "age": 25}');

-- Add friendships
INSERT INTO social_network (edge_id, from_id, to_id, edge_type, weight) VALUES
    (101, 1, 2, 'FRIEND', 1.0),
    (102, 2, 3, 'FRIEND', 1.0),
    (103, 1, 3, 'FRIEND', 0.5);

-- Find friends of friends
WITH RECURSIVE friends_of_friends AS (
    SELECT 1 as user_id, 0 as depth
    UNION ALL
    SELECT 
        CASE 
            WHEN e.from_id = f.user_id THEN e.to_id
            ELSE e.from_id
        END as user_id,
        f.depth + 1
    FROM friends_of_friends f
    JOIN social_network e ON (
        e.edge_type = 'FRIEND' AND 
        (e.from_id = f.user_id OR e.to_id = f.user_id)
    )
    WHERE f.depth < 2
)
SELECT DISTINCT 
    n.node_id,
    json_extract(n.properties, '$.name') as name
FROM friends_of_friends f
JOIN social_network n ON n.node_id = f.user_id
WHERE f.depth = 2;

-- Find most influential users (PageRank)
SELECT 
    n.node_id,
    json_extract(n.properties, '$.name') as name,
    p.rank
FROM graph_pagerank('social_network') p
JOIN social_network n ON n.node_id = p.node_id
ORDER BY p.rank DESC
LIMIT 10;
```

### Knowledge Graph

```cypher
-- Create knowledge graph
CREATE (python:Language {name: 'Python', paradigm: 'multi-paradigm'}),
       (java:Language {name: 'Java', paradigm: 'object-oriented'}),
       (ml:Field {name: 'Machine Learning'}),
       (web:Field {name: 'Web Development'}),
       (python)-[:USED_FOR]->(ml),
       (python)-[:USED_FOR]->(web),
       (java)-[:USED_FOR]->(web);

-- Query relationships
MATCH (lang:Language)-[:USED_FOR]->(field:Field)
WHERE field.name = 'Web Development'
RETURN lang.name as language, field.name as field;

-- Find languages by paradigm
MATCH (lang:Language)
WHERE lang.paradigm CONTAINS 'object'
RETURN lang;
```

### Route Planning

```sql
-- Create transportation network
CREATE VIRTUAL TABLE transport_network USING graph();

-- Add cities (nodes)
INSERT INTO transport_network (node_id, labels, properties) VALUES
    (1, '["City"]', '{"name": "New York", "lat": 40.7128, "lon": -74.0060}'),
    (2, '["City"]', '{"name": "Boston", "lat": 42.3601, "lon": -71.0589}'),
    (3, '["City"]', '{"name": "Philadelphia", "lat": 39.9526, "lon": -75.1652}');

-- Add routes (edges) with distances as weights
INSERT INTO transport_network (edge_id, from_id, to_id, edge_type, weight, properties) VALUES
    (101, 1, 2, 'ROUTE', 215, '{"mode": "highway", "time": 4.5}'),
    (102, 1, 3, 'ROUTE', 95, '{"mode": "highway", "time": 2.0}'),
    (103, 2, 3, 'ROUTE', 310, '{"mode": "highway", "time": 5.5}');

-- Find shortest route
SELECT 
    d.*,
    (SELECT GROUP_CONCAT(
        json_extract(n.properties, '$.name'), ' -> '
    ) 
    FROM json_each(d.path) p
    JOIN transport_network n ON n.node_id = p.value
    ) as route_names
FROM graph_dijkstra('transport_network', 1, 2) d
WHERE d.node_id = 2;
```

## Best Practices

1. **Indexing**: Always create indexes on frequently queried properties
2. **Batch Operations**: Use transactions for bulk inserts/updates
3. **Memory Management**: Set appropriate cache sizes for your workload
4. **Query Optimization**: Use EXPLAIN QUERY PLAN to understand performance
5. **Error Handling**: Always check return codes and handle errors appropriately
6. **Resource Cleanup**: Properly close graphs and free resources
7. **Thread Safety**: Use appropriate locking when accessing from multiple threads

## Limitations

1. Maximum graph size limited by available memory and SQLite database size limits
2. Cypher support is a subset of the full specification
3. Some advanced graph algorithms may require custom implementation
4. Performance degrades with very deep traversals (configurable limit)

## Version Information

- Extension Version: 1.0.0
- SQLite Minimum Version: 3.35.0
- Cypher Compatibility: Subset of Cypher 9

For more information and updates, visit the project repository.