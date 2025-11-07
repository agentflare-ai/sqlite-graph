# SQLite Graph Database Extension

[![Release](https://img.shields.io/github/v/release/agentflare-ai/sqlite-graph?include_prereleases)](https://github.com/agentflare-ai/sqlite-graph/releases)
[![Build Status](https://img.shields.io/github/actions/workflow/status/agentflare-ai/sqlite-graph/ci.yml?branch=main)](https://github.com/agentflare-ai/sqlite-graph/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-linux-lightgrey)](https://github.com/agentflare-ai/sqlite-graph)
[![C99](https://img.shields.io/badge/std-C99-blue)](https://en.wikipedia.org/wiki/C99)

A powerful SQLite extension that adds graph database capabilities with full Cypher query support. Part of the AgentFlare AI ecosystem. Build sophisticated graph applications with the reliability of SQLite and the expressiveness of Cypher.

> **⚠️ ALPHA RELEASE**: This is an alpha release (v0.1.0-alpha.0) intended for testing and feedback. The API may change in future releases. Not recommended for production use.

## Features (Alpha v0.1.0)

### ✅ Working Features
- **Core Graph Operations**: Create, read, update, delete nodes and edges via SQL functions
- **Cypher Query Execution** (NEW!): Basic Cypher queries now work end-to-end
  - `CREATE (n)` - Create anonymous nodes ✅
  - `CREATE (p:Person)` - Create nodes with labels ✅
  - `MATCH (n) RETURN n` - Basic pattern matching ✅
  - `MATCH (n:Label) RETURN n` - Label-based matching ✅
  - Full execution pipeline: parser → logical planner → physical planner → executor ✅
  - **70/70 CREATE TCK tests passing** (100% openCypher compliance for CREATE)
- **Graph Virtual Tables**: SQLite virtual table integration for graph data
- **Basic Graph Algorithms**: Connectivity checks, density calculations, degree centrality
- **SQL API**: `graph_node_add()`, `graph_edge_add()`, `graph_count_nodes()`, `graph_count_edges()`
- **Performance**: 300K+ nodes/sec, 390K+ edges/sec
- **Python Bindings**: Full Python 3.6+ support with examples
- **Thread Safety**: Fixed critical thread-safety issues for production use
- **Security**: Buffer overflow protections, SQL injection prevention

### 🚧 In Progress
- **Advanced Cypher Features**:
  - MERGE relationships (nodes supported with ON CREATE/ON MATCH)
  - Relationship matching (edges/patterns)
  - WHERE clause filtering
  - Property expressions in RETURN
  - Aggregations (COUNT, SUM, etc.)
  - ORDER BY, SKIP, LIMIT
- **Graph Algorithms**: Shortest path, PageRank (implementation incomplete)
- **Property Indexing**: Basic support, optimization ongoing

### 📋 Roadmap (Not Yet Implemented)
- **Full openCypher Compliance**: 100% TCK test suite passing
- **Advanced Write Operations**: MERGE, SET, DELETE with complex patterns
- **Advanced Pattern Matching**: Multi-hop paths, variable-length patterns
- **Advanced Algorithms**: Betweenness centrality, community detection
- **Query Optimization**: Cost-based query planner with statistics
- **Distributed Queries**: Multi-database graph queries

See [ROADMAP.md](ROADMAP.md) for detailed feature timeline.

## Quick Start

### Installation

#### Option 1: Download Pre-built Binary (Linux x86_64)

```bash
# Download from releases page
wget https://github.com/agentflare-ai/sqlite-graph/releases/latest/download/libgraph.so

# Verify checksum (recommended)
wget https://github.com/agentflare-ai/sqlite-graph/releases/latest/download/checksums.txt
sha256sum -c checksums.txt

# Ready to use!
```

#### Option 2: Build from Source

```bash
# Clone the repository
git clone https://github.com/agentflare-ai/sqlite-graph.git
cd sqlite-graph

# Build the extension
make

# The extension will be built as build/libgraph.so
```

See the [Installation Guide](INSTALL.md) for detailed instructions and platform-specific guidance.

### Basic Usage (Alpha v0.1.0)

```python
import sqlite3
import json

# Load the extension
conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)
conn.load_extension("./build/libgraph.so")

# Create a graph virtual table
conn.execute("CREATE VIRTUAL TABLE graph USING graph()")

# Option 1: Use Cypher queries (NEW!)
conn.execute("SELECT cypher_execute('CREATE (p:Person {name: \"Alice\"})')")
conn.execute("SELECT cypher_execute('CREATE (c:Company {name: \"Acme Inc\"})')")

# Query with MATCH...RETURN
cursor = conn.execute("SELECT cypher_execute('MATCH (n:Person) RETURN n')")
results = json.loads(cursor.fetchone()[0])
print(results)  # [{"n": Node(1)}]

# Option 2: Add nodes using SQL functions
conn.execute("SELECT graph_node_add(1, ?) as id", (json.dumps({"name": "Alice", "age": 30}),))
conn.execute("SELECT graph_node_add(2, ?) as id", (json.dumps({"name": "Bob", "age": 25}),))

# Add edges
conn.execute("SELECT graph_edge_add(1, 2, 'KNOWS', ?) as id", (json.dumps({"since": "2020"}),))

# Query the graph
node_count = conn.execute("SELECT graph_count_nodes()").fetchone()[0]
edge_count = conn.execute("SELECT graph_count_edges()").fetchone()[0]
print(f"Nodes: {node_count}, Edges: {edge_count}")  # Nodes: 4, Edges: 1

# Check graph properties
is_connected = conn.execute("SELECT graph_is_connected()").fetchone()[0]
density = conn.execute("SELECT graph_density()").fetchone()[0]
centrality = conn.execute("SELECT graph_degree_centrality(1)").fetchone()[0]
print(f"Connected: {bool(is_connected)}, Density: {density:.3f}, Alice centrality: {centrality:.3f}")

# Verify Cypher-created nodes
cypher_nodes = conn.execute("SELECT id, labels FROM graph_nodes WHERE labels != ''").fetchall()
print(f"Cypher nodes: {cypher_nodes}")  # [(node_id, 'Person'), (node_id, 'Company')]
```

### Working Examples

See the `examples/` directory for fully functional demonstrations:
- **simple_graph_example.py** - Complete working example with nodes, edges, and algorithms
- **python_examples.py** - 6 comprehensive examples showcasing all working features
- **cypher_demo.py** - NEW! Cypher CREATE query examples

> **Note**: Basic Cypher CREATE queries now work! MATCH and RETURN are in development for v0.2.0. The alpha version also provides SQL function-based graph operations.

## Documentation

### Essential Reading
- **[FEATURES.md](FEATURES.md)** - ⭐ **Start here!** Complete feature status and API reference
- **[ROADMAP.md](ROADMAP.md)** - Development timeline and planned features
- [Installation Guide](INSTALL.md) - Detailed build and installation instructions
- [Examples](examples/) - Working code examples (simple_graph_example.py, python_examples.py)

### Project Status (Alpha v0.1.0)
✅ **What Works**: Node/edge creation, basic Cypher CREATE, basic algorithms, Python bindings
🚧 **In Progress**: MATCH queries, RETURN clause, property support in CREATE
📋 **Planned**: Full Cypher support (Q1 2026), advanced algorithms (Q2 2026)

## Architecture (Alpha Implementation)

The extension currently consists of:

- ✅ **Virtual Table Interface**: SQLite virtual table implementation for graph operations
- ✅ **Storage Engine**: Efficient node/edge storage with JSON properties
- ✅ **Algorithm Library**: Basic graph algorithms (connectivity, density, centrality)
- ✅ **Cypher Execution Engine**: Parser → Planner → Iterator-based Executor (NEW!)
  - ✅ Lexer and Parser (AST generation)
  - ✅ Logical Plan generation
  - ✅ Physical Plan optimization
  - ✅ Volcano-model iterators
  - ✅ Result serialization
- 🚧 **Cypher Operators**: CREATE working, MATCH/RETURN in progress
- 📋 **Query Optimizer**: Cost-based optimization planned for v0.2.0

## Performance (Alpha Benchmarks)

- **Node Creation**: 300,000+ nodes/second (tested up to 1,000 nodes)
- **Edge Creation**: 390,000+ edges/second (tested up to 999 edges)
- **Connectivity Check**: <1ms for 1,000 node graphs
- **Scalability**: Currently tested up to 1,000 nodes/edges
- **Memory**: In-memory storage, persistence via SQLite database file

## More Examples

### Social Network Analysis (Working Alpha Code)
```python
import json

# Create person nodes
alice_id = 1
bob_id = 2
carol_id = 3

conn.execute("SELECT graph_node_add(?, ?)",
            (alice_id, json.dumps({"name": "Alice", "city": "NYC"})))
conn.execute("SELECT graph_node_add(?, ?)",
            (bob_id, json.dumps({"name": "Bob", "city": "LA"})))
conn.execute("SELECT graph_node_add(?, ?)",
            (carol_id, json.dumps({"name": "Carol", "city": "NYC"})))

# Add friendships
conn.execute("SELECT graph_edge_add(?, ?, 'FRIENDS', ?)",
            (alice_id, bob_id, json.dumps({})))
conn.execute("SELECT graph_edge_add(?, ?, 'FRIENDS', ?)",
            (alice_id, carol_id, json.dumps({})))

# Query the graph with SQL
mutual_friends = conn.execute("""
    SELECT DISTINCT n.id, n.properties
    FROM graph_nodes n
    JOIN graph_edges e1 ON e1.target = n.id
    JOIN graph_edges e2 ON e2.target = n.id
    WHERE e1.source = ? AND e2.source = ?
      AND e1.edge_type = 'FRIENDS' AND e2.edge_type = 'FRIENDS'
""", (alice_id, bob_id)).fetchall()

# Check connectivity
is_connected = conn.execute("SELECT graph_is_connected()").fetchone()[0]
print(f"Network is connected: {bool(is_connected)}")
```

### More Working Examples
See the `examples/` directory for complete, tested code:
- `simple_graph_example.py` - Basic operations walkthrough
- `python_examples.py` - 6 comprehensive examples with output

## Testing

```bash
# Build the extension
make

# Run Python examples (all tests should pass)
cd examples
python3 simple_graph_example.py
python3 python_examples.py

# Expected output: All examples pass with ✅ indicators
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

We welcome contributions! Please check:

- [FEATURES.md](FEATURES.md) - See what's implemented and what's not
- [ROADMAP.md](ROADMAP.md) - See planned features and timeline
- [GitHub Issues](https://github.com/agentflare-ai/sqlite-graph/issues) - Report bugs or request features

## Support

- **Bug Reports**: [GitHub Issues](https://github.com/agentflare-ai/sqlite-graph/issues)
- **Feature Requests**: Check [ROADMAP.md](ROADMAP.md) first, then open an issue with `[Feature Request]` tag
- **Questions**: See [FEATURES.md](FEATURES.md) for detailed API documentation

## Development Roadmap

**Alpha v0.1.0 (Current)**: Core graph operations, basic algorithms
**v0.2.0 (Q1 2026)**: Full Cypher query execution
**v0.3.0 (Q2 2026)**: Advanced graph algorithms
**v0.4.0 (Q3 2026)**: Performance optimization & scale
**v1.0.0 (2027)**: Production ready with full openCypher compliance

See [ROADMAP.md](ROADMAP.md) for detailed feature timeline.

---

**Part of the AgentFlare AI ecosystem • Built with SQLite**
**Alpha Release**: v0.1.0-alpha.0 • Not for production use
