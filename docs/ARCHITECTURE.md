# SQLite Graph Database Extension - Architecture

This document provides a technical deep-dive into the architecture, design decisions, and implementation details of the SQLite Graph Database Extension.

## Table of Contents

- [Overview](#overview)
- [Component Architecture](#component-architecture)
- [Data Structures](#data-structures)
- [Query Processing Pipeline](#query-processing-pipeline)
- [Storage Layer](#storage-layer)
- [Memory Management](#memory-management)
- [Threading Model](#threading-model)
- [Performance Considerations](#performance-considerations)
- [Extension Points](#extension-points)

---

## Overview

The SQLite Graph Database Extension is a loadable extension that adds graph database capabilities to SQLite using the Virtual Table interface. It implements the openCypher query language and provides graph algorithms while maintaining compatibility with SQLite's ACID properties and SQL capabilities.

### Design Goals

1. **Seamless Integration**: Works as a standard SQLite extension, no forking required
2. **Performance**: Optimized for common graph operations and queries
3. **Memory Efficiency**: Minimal overhead beyond the graph data itself
4. **Compatibility**: Works with any SQLite-compatible application
5. **Standard Compliance**: Implements openCypher specification

---

## Component Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     SQLite Core                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │         Virtual Table Interface                      │   │
│  └───────────────────┬──────────────────────────────────┘   │
└────────────────────┬─┴───────────────────────────────────────┘
                     │
     ┌───────────────┴───────────────┐
     │   Graph Extension Entry Point  │
     │      (sqlite3_graph_init)      │
     └───────────────┬───────────────┘
                     │
     ┌───────────────┴───────────────────────────────────┐
     │                                                    │
┌────▼─────┐  ┌──────▼──────┐  ┌────────▼────────┐    │
│  Virtual  │  │   Cypher    │  │     Graph       │    │
│   Table   │  │   Query     │  │   Algorithms    │    │
│  Manager  │  │   Engine    │  │                 │    │
└────┬─────┘  └──────┬──────┘  └────────┬────────┘    │
     │               │                  │              │
     │        ┌──────▼──────┐           │              │
     │        │   Cypher    │           │              │
     │        │   Parser    │           │              │
     │        │   (Lexer/   │           │              │
     │        │   AST)      │           │              │
     │        └──────┬──────┘           │              │
     │               │                  │              │
     │        ┌──────▼──────┐           │              │
     │        │   Logical   │           │              │
     │        │   Planner   │           │              │
     │        └──────┬──────┘           │              │
     │               │                  │              │
     │        ┌──────▼──────┐           │              │
     │        │  Physical   │           │              │
     │        │   Planner   │           │              │
     │        └──────┬──────┘           │              │
     │               │                  │              │
     └───────────────┼──────────────────┘              │
                     │                                 │
              ┌──────▼──────────────────────────┐      │
              │     Graph Storage Layer         │      │
              │  ┌───────────┬──────────────┐  │      │
              │  │   Nodes   │    Edges     │  │      │
              │  ├───────────┼──────────────┤  │      │
              │  │  Indexes  │  Properties  │  │      │
              │  └───────────┴──────────────┘  │      │
              └─────────────────────────────────┘      │
                                                       │
                          ┌────────────────────────────┘
                          │
                    ┌─────▼──────┐
                    │   Memory   │
                    │ Management │
                    └────────────┘
```

### Key Components

#### 1. Extension Entry Point (`src/graph.c`)

- **Purpose**: SQLite extension initialization
- **Responsibilities**:
  - Register virtual table module
  - Register SQL functions
  - Initialize extension state
  - Handle extension unloading

#### 2. Virtual Table Manager (`src/graph-vtab.c`)

- **Purpose**: Implements SQLite virtual table interface
- **Key Methods**:
  - `graphCreate()` / `graphConnect()`: Table creation
  - `graphBestIndex()`: Query planning hints to SQLite
  - `graphOpen()`: Create cursor for iteration
  - `graphFilter()` / `graphNext()`: Row iteration
  - `graphUpdate()`: Handle INSERT/UPDATE/DELETE
  - `graphDisconnect()` / `graphDestroy()`: Cleanup

#### 3. Cypher Query Engine (`src/cypher/`)

##### Lexer (`cypher-lexer.c`)
- Tokenizes Cypher query strings
- Handles keywords, identifiers, literals, operators
- Position tracking for error reporting

##### Parser (`cypher-parser.c`)
- Recursive descent parser
- Builds Abstract Syntax Tree (AST)
- Syntax validation and error reporting

##### AST (`cypher-ast.c`)
- Tree representation of Cypher queries
- Node types for each Cypher construct
- Visitor pattern for tree traversal

##### Logical Planner (`cypher-logical-plan.c`)
- Converts AST to logical query plan
- Query optimization (predicate pushdown, join reordering)
- Cost estimation

##### Physical Planner (`cypher-physical-plan.c`)
- Converts logical plan to execution plan
- Chooses execution strategies (index scans vs. full scans)
- Generates iterators

##### Executor (`cypher-executor.c`)
- Executes physical plan
- Manages execution context
- Result materialization

#### 4. Graph Storage Layer (`src/graph-storage.c`)

- **In-Memory Data Structures**:
  - Hash tables for node/edge lookup by ID
  - B-trees for property indexes
  - Adjacency lists for graph traversal

- **Schema Management**:
  - Label registry
  - Relationship type registry
  - Property key registry

#### 5. Graph Algorithms (`src/graph-algo.c`)

- Shortest path (Dijkstra, BFS)
- Centrality measures (PageRank, betweenness)
- Graph traversal (DFS, BFS)
- Connected components
- And more...

---

## Data Structures

### Node Structure

```c
typedef struct GraphNode {
  sqlite3_int64 id;           // Unique node ID
  int label_count;            // Number of labels
  char **labels;              // Array of label strings
  char *properties_json;      // JSON-encoded properties
  struct GraphNode *next;     // Linked list pointer

  // Index support
  struct GraphNode *hash_next; // Hash table collision chain
  int ref_count;              // Reference counting for memory management
} GraphNode;
```

### Edge Structure

```c
typedef struct GraphEdge {
  sqlite3_int64 id;           // Unique edge ID
  sqlite3_int64 from_id;      // Source node ID
  sqlite3_int64 to_id;        // Target node ID
  char *type;                 // Relationship type
  char *properties_json;      // JSON-encoded properties
  struct GraphEdge *next;     // Linked list pointer

  // For adjacency list
  struct GraphEdge *from_next; // Next outgoing edge
  struct GraphEdge *to_next;   // Next incoming edge
  int ref_count;              // Reference counting
} GraphEdge;
```

### Graph Structure

```c
typedef struct GraphVtab {
  sqlite3_vtab base;          // SQLite virtual table base
  sqlite3 *db;                // SQLite database connection

  // Storage
  GraphNode **node_hash;      // Hash table of nodes
  GraphEdge **edge_hash;      // Hash table of edges
  int hash_size;              // Size of hash tables

  // Metadata
  sqlite3_int64 next_node_id; // ID counter for nodes
  sqlite3_int64 next_edge_id; // ID counter for edges
  int node_count;             // Total node count
  int edge_count;             // Total edge count

  // Indexes
  struct LabelIndex *label_indexes;     // Label-based indexes
  struct PropertyIndex *prop_indexes;   // Property indexes

  // Schema
  struct SchemaInfo *schema;  // Schema metadata
} GraphVtab;
```

### AST Node Structure (Example)

```c
typedef struct CypherASTNode {
  CypherASTNodeType type;     // Node type (MATCH, CREATE, etc.)
  int line, column;           // Source location

  union {
    struct {
      struct CypherPattern *pattern;
      struct CypherASTNode *where;
      struct CypherASTNode *return_clause;
    } match_clause;

    struct {
      struct CypherPattern *pattern;
    } create_clause;

    struct {
      struct CypherReturnItem *items;
      int item_count;
      struct CypherASTNode *order_by;
      struct CypherASTNode *limit;
    } return_clause;

    // ... other clause types
  } data;

  struct CypherASTNode *next; // For linked lists
} CypherASTNode;
```

---

## Query Processing Pipeline

### Example: `MATCH (p:Person {age: 30}) RETURN p.name`

```
1. Lexer
   Input:  "MATCH (p:Person {age: 30}) RETURN p.name"
   Output: [MATCH, LPAREN, IDENT("p"), COLON, IDENT("Person"), ...]

2. Parser
   Input:  Token stream
   Output: AST with MATCH clause, pattern, RETURN clause

3. Logical Planner
   - Identify pattern: node with label "Person" and property filter
   - Optimization: use label index for "Person"
   - Optimization: push down property filter
   - Generate logical plan: LabelScan(Person) -> Filter(age=30) -> Return(name)

4. Physical Planner
   - Choose index scan strategy
   - Generate iterators: IndexScan -> FilterIterator -> ProjectIterator

5. Executor
   - Open index scan on "Person" label
   - For each matching node:
     - Apply filter: age = 30
     - Project property: name
     - Add to result set
   - Return results to SQLite

6. Virtual Table Interface
   - SQLite calls graphOpen() to create cursor
   - SQLite calls graphFilter() to start iteration
   - SQLite calls graphNext() for each row
   - Results returned via graphColumn()
```

---

## Storage Layer

### Node Storage

Nodes are stored in a hash table for O(1) lookup by ID, with a linked list for iteration:

```
node_hash[hash(id)] -> Node1 -> Node2 -> Node3 -> ...
```

### Edge Storage

Edges are stored similarly, but also maintain adjacency lists for graph traversal:

```
Outgoing edges from node N:
  node_outgoing[N] -> Edge1 -> Edge2 -> ...

Incoming edges to node N:
  node_incoming[N] -> Edge1 -> Edge2 -> ...
```

### Indexes

#### Label Index
- B-tree mapping label -> list of node IDs
- Enables fast `MATCH (n:Label)` queries

#### Property Index
- B-tree mapping (property_key, property_value) -> list of node/edge IDs
- Enables fast property-based lookups
- Created automatically for frequently accessed properties

### JSON Properties

Properties are stored as JSON strings using SQLite's JSON functions for:
- Flexible schema
- Complex nested structures
- Efficient serialization

---

## Memory Management

### Allocation Strategy

The extension uses SQLite's memory allocator (`sqlite3_malloc`, `sqlite3_free`) for all allocations to:
- Integrate with SQLite's memory accounting
- Support custom memory allocators
- Enable memory limit enforcement

### Reference Counting

Nodes and edges use reference counting for lifecycle management:

```c
void graphNodeRetain(GraphNode *node) {
  if (node) node->ref_count++;
}

void graphNodeRelease(GraphNode *node) {
  if (node && --node->ref_count == 0) {
    graphNodeDestroy(node);
  }
}
```

### Memory Pools

For frequently allocated small objects (e.g., iterators, execution context), memory pools reduce fragmentation and improve performance.

---

## Threading Model

### SQLite Threading Modes

The extension supports all SQLite threading modes:
- **Single-thread**: No locking
- **Multi-thread**: Database-level locking (handled by SQLite)
- **Serialized**: Connection-level locking (handled by SQLite)

### Read-Write Locks

For future multi-threaded query execution, the storage layer includes read-write locks:
- Multiple concurrent reads
- Exclusive writes
- Currently unused in v0.1.0 (single-threaded execution)

---

## Performance Considerations

### Query Optimization

1. **Index Selection**: Chooses best index based on selectivity estimates
2. **Join Ordering**: Reorders joins to minimize intermediate results
3. **Predicate Pushdown**: Applies filters as early as possible
4. **Projection Pushdown**: Avoids materializing unused properties

### Caching

- **Schema Cache**: Labels, relationship types, property keys cached
- **Query Plan Cache**: Compiled plans cached for repeated queries (future)

### Memory Efficiency

- Lazy evaluation: iterators don't materialize full results
- Streaming: results returned incrementally to SQLite
- Compact structures: careful struct packing reduces memory overhead

---

## Extension Points

### Adding New Graph Algorithms

1. Implement algorithm in `src/graph-algo.c`
2. Register as SQL function in `src/graph.c`
3. Add Cypher procedure call syntax (optional)

Example:
```c
void graphRegisterAlgorithms(sqlite3 *db) {
  sqlite3_create_function(db, "shortest_path", 2,
    SQLITE_UTF8 | SQLITE_DETERMINISTIC,
    NULL, shortestPathFunc, NULL, NULL);
}
```

### Adding Cypher Language Features

1. Add tokens to lexer (`src/cypher/cypher-lexer.c`)
2. Add grammar rules to parser (`src/cypher/cypher-parser.c`)
3. Add AST node types (`include/cypher/cypher-ast.h`)
4. Update logical planner to handle new construct
5. Update physical planner for execution strategy
6. Add tests

---

## Future Enhancements

### Planned for v0.2.0+

- **Persistence**: Write-ahead log (WAL) for durability
- **Query Parallelization**: Multi-threaded query execution
- **Distributed Queries**: Sharding and remote query execution
- **Advanced Indexes**: Spatial indexes, full-text search
- **Query Optimization**: Cost-based optimizer improvements
- **Streaming Ingestion**: Efficient bulk loading

---

## References

- [SQLite Virtual Table Documentation](https://www.sqlite.org/vtab.html)
- [openCypher Specification](https://opencypher.org/)
- [Graph Database Internals](https://www.graphdatabasesinternals.com/)
- [SQLite Extension Loading](https://www.sqlite.org/loadext.html)

---

**Last Updated**: 2025-10-24
**Version**: 0.1.0-alpha.0
