# sqlite-graph

[![Release](https://img.shields.io/github/v/release/agentflare-ai/sqlite-graph?include_prereleases)](https://github.com/agentflare-ai/sqlite-graph/releases)
[![Build Status](https://img.shields.io/github/actions/workflow/status/agentflare-ai/sqlite-graph/ci.yml?branch=main)](https://github.com/agentflare-ai/sqlite-graph/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-linux-lightgrey)](https://github.com/agentflare-ai/sqlite-graph)
[![C99](https://img.shields.io/badge/std-C99-blue)](https://en.wikipedia.org/wiki/C99)

A SQLite extension that adds graph database capabilities with Cypher query support.

> **⚠️ ALPHA RELEASE**: Version 0.1.0-alpha.0 is for testing and feedback. The API may change. Not recommended for production use.

## Table of Contents

- [What is sqlite-graph?](#what-is-sqlite-graph)
- [Quick Start](#quick-start)
- [Features](#features)
- [Installation](#installation)
- [Usage Examples](#usage-examples)
- [API Reference](#api-reference)
- [Performance](#performance)
- [Platform Support](#platform-support)
- [Troubleshooting](#troubleshooting)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [License](#license)

## What is sqlite-graph?

`sqlite-graph` extends SQLite with graph database capabilities, allowing you to:

- **Store and query graph data** using nodes and edges with properties
- **Execute Cypher queries** directly in SQLite for graph pattern matching
- **Run graph algorithms** like connectivity checks and centrality measures
- **Combine SQL and graphs** - use standard SQL alongside graph operations
- **Zero dependencies** - pure C99 extension with no external requirements

**Why use it?** Get the power of graph databases without leaving SQLite. Perfect for:
- Applications already using SQLite that need graph capabilities
- Prototyping graph features before committing to a dedicated graph DB
- Embedded graph analytics in edge devices or mobile apps
- Learning graph databases with the simplicity of SQLite

## Quick Start

### Installation (2 minutes)

**Linux (x86_64) - Download pre-built:**

```bash
wget https://github.com/agentflare-ai/sqlite-graph/releases/latest/download/libgraph.so
```

**Or build from source:**

```bash
git clone https://github.com/agentflare-ai/sqlite-graph.git
cd sqlite-graph
make
# Extension built as build/libgraph.so
```

See [Installation Guide](INSTALL.md) for detailed instructions.

### First Graph in 30 Seconds

```python
import sqlite3
import json

# Load the extension
conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)
conn.load_extension("./build/libgraph.so")

# Create a graph
conn.execute("CREATE VIRTUAL TABLE graph USING graph()")

# Use Cypher to create nodes and relationships
conn.execute("SELECT cypher_execute('CREATE (p:Person {name: \"Alice\", age: 30})')")
conn.execute("SELECT cypher_execute('CREATE (p:Person {name: \"Bob\", age: 25})')")
conn.execute("SELECT cypher_execute(\"CREATE (a:Person {name: \\\"Alice\\\"})-[:KNOWS]->(b:Person {name: \\\"Bob\\\"})\")") 

# Query with pattern matching
cursor = conn.execute("SELECT cypher_execute('MATCH (p:Person) WHERE p.age > 25 RETURN p')")
results = json.loads(cursor.fetchone()[0])
print(results)  # Returns Alice (age 30)
```

**That's it!** See [Usage Examples](#usage-examples) below for more.

## Features

**Current Status (Alpha v0.1.0):**

### ✅ What Works Now

**Cypher Query Language:**
- ✅ `CREATE` - nodes and relationships with labels and properties (70/70 TCK tests ✓)
- ✅ `MATCH` - pattern matching with labels and relationships
- ✅ `WHERE` - filtering with comparison operators (`=`, `>`, `<`, `>=`, `<=`, `<>`)
- ✅ `RETURN` - return nodes, relationships, and multiple items

**Graph Operations:**
- ✅ Add/query nodes and edges via SQL functions
- ✅ Graph algorithms: connectivity, density, degree centrality
- ✅ JSON property support for nodes and relationships
- ✅ Virtual table interface for SQL integration

**Performance:**
- ✅ 300K+ nodes/second, 390K+ edges/second
- ✅ Thread-safe for concurrent access
- ✅ Python 3.6+ bindings included

### 🚧 Coming Soon (v0.2.0 - Q1 2026)

- Bidirectional relationships (`<-[r]-`, `-[r]-`)
- Variable-length paths (`[r*1..3]`)
- Complex WHERE expressions (AND, OR, NOT)
- Aggregations (COUNT, SUM, AVG, etc.)
- Property projection in RETURN (`n.property`)
- ORDER BY, SKIP, LIMIT

See [FEATURES.md](FEATURES.md) for complete feature details and [ROADMAP.md](ROADMAP.md) for the development timeline.

## Installation

### Requirements

- **Platform**: Linux (x86_64), macOS, or Windows with WSL
- **SQLite**: Version 3.8.0+ with extension loading enabled
- **Compiler**: GCC 4.8+ or Clang 3.8+ (for building from source)

### Method 1: Download Pre-built Binary (Linux)

```bash
# Get the latest release
wget https://github.com/agentflare-ai/sqlite-graph/releases/latest/download/libgraph.so

# Verify integrity (recommended)
wget https://github.com/agentflare-ai/sqlite-graph/releases/latest/download/checksums.txt
sha256sum -c checksums.txt
```

### Method 2: Build from Source

```bash
# Clone repository
git clone https://github.com/agentflare-ai/sqlite-graph.git
cd sqlite-graph

# Build (creates build/libgraph.so)
make

# Optional: Run tests
make test
```

### Loading the Extension

**Python:**
```python
import sqlite3
conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)
conn.load_extension("./build/libgraph.so")  # or path to libgraph.so
```

**SQLite CLI:**
```sql
.load ./build/libgraph.so
-- Extension loaded, ready to use
```

**C/C++:**
```c
sqlite3 *db;
sqlite3_open(":memory:", &db);
sqlite3_enable_load_extension(db, 1);
sqlite3_load_extension(db, "./build/libgraph.so", 0, 0);
```

For platform-specific instructions and troubleshooting, see [INSTALL.md](INSTALL.md).

## Usage Examples

### Example 1: Using Cypher Queries

```python

```

### Example 1: Using Cypher Queries

```python
import sqlite3
import json

conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)
conn.load_extension("./build/libgraph.so")
conn.execute("CREATE VIRTUAL TABLE graph USING graph()")

# Create a social network
conn.execute("SELECT cypher_execute('CREATE (p:Person {name: \"Alice\", age: 30})')")
conn.execute("SELECT cypher_execute('CREATE (p:Person {name: \"Bob\", age: 25})')")
conn.execute("SELECT cypher_execute(\"CREATE (a:Person {name: \\\"Alice\\\"})-[:KNOWS {since: 2020}]->(b:Person {name: \\\"Bob\\\"})\")") 

# Query with pattern matching
cursor = conn.execute("SELECT cypher_execute('MATCH (p:Person) WHERE p.age > 25 RETURN p')")
results = json.loads(cursor.fetchone()[0])
print(results)  # Returns Alice (age 30)

# Find relationships
cursor = conn.execute("SELECT cypher_execute('MATCH (a)-[r:KNOWS]->(b) RETURN a, r, b')")
results = json.loads(cursor.fetchone()[0])
```

### Example 2: Using SQL Functions

```python
import sqlite3
import json

conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)
conn.load_extension("./build/libgraph.so")
conn.execute("CREATE VIRTUAL TABLE graph USING graph()")

# Add nodes
conn.execute("SELECT graph_node_add(1, ?)", (json.dumps({"name": "Alice", "age": 30}),))
conn.execute("SELECT graph_node_add(2, ?)", (json.dumps({"name": "Bob", "age": 25}),))

# Add edges
conn.execute("SELECT graph_edge_add(1, 2, 'KNOWS', ?)", (json.dumps({"since": "2020"}),))

# Query graph
node_count = conn.execute("SELECT graph_count_nodes()").fetchone()[0]
edge_count = conn.execute("SELECT graph_count_edges()").fetchone()[0]
print(f"Nodes: {node_count}, Edges: {edge_count}")

# Run algorithms
is_connected = conn.execute("SELECT graph_is_connected()").fetchone()[0]
density = conn.execute("SELECT graph_density()").fetchone()[0]
centrality = conn.execute("SELECT graph_degree_centrality(1)").fetchone()[0]
print(f"Connected: {bool(is_connected)}, Density: {density:.3f}, Centrality: {centrality:.3f}")
```

### Example 3: Social Network Analysis

```python
import sqlite3
import json

conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)
conn.load_extension("./build/libgraph.so")
conn.execute("CREATE VIRTUAL TABLE graph USING graph()")

# Create person nodes
people = [
    (1, {"name": "Alice", "city": "NYC"}),
    (2, {"name": "Bob", "city": "LA"}),
    (3, {"name": "Carol", "city": "NYC"})
]

for person_id, props in people:
    conn.execute("SELECT graph_node_add(?, ?)", (person_id, json.dumps(props)))

# Add friendships
conn.execute("SELECT graph_edge_add(1, 2, 'FRIENDS', ?)", (json.dumps({}),))
conn.execute("SELECT graph_edge_add(1, 3, 'FRIENDS', ?)", (json.dumps({}),))

# Find mutual friends using SQL
mutual_friends = conn.execute("""
    SELECT DISTINCT n.id, n.properties
    FROM graph_nodes n
    JOIN graph_edges e1 ON e1.target = n.id
    JOIN graph_edges e2 ON e2.target = n.id
    WHERE e1.source = ? AND e2.source = ?
      AND e1.edge_type = 'FRIENDS' AND e2.edge_type = 'FRIENDS'
""", (1, 2)).fetchall()
```

**More Examples:** See `examples/` directory for complete, tested code:
- `simple_graph_example.py` - Basic operations walkthrough
- `python_examples.py` - 6 comprehensive examples
- `cypher_demo.py` - Cypher query examples

## API Reference

### Cypher Functions

| Function | Description | Example |
|----------|-------------|---------|
| `cypher_execute(query)` | Execute Cypher query | `SELECT cypher_execute('MATCH (n) RETURN n')` |

**Supported Cypher:**
- `CREATE` - Create nodes and relationships with labels/properties
- `MATCH` - Pattern matching with labels and relationships  
- `WHERE` - Filter with `=`, `>`, `<`, `>=`, `<=`, `<>`
- `RETURN` - Return nodes, relationships, multiple items

### SQL Functions

| Function | Description | Return Type |
|----------|-------------|-------------|
| `graph_node_add(id, properties_json)` | Add node with properties | INTEGER (node id) |
| `graph_edge_add(src, tgt, type, props_json)` | Add typed edge | INTEGER (edge id) |
| `graph_count_nodes()` | Count all nodes | INTEGER |
| `graph_count_edges()` | Count all edges | INTEGER |
| `graph_is_connected()` | Check if graph is connected | BOOLEAN (0/1) |
| `graph_density()` | Calculate graph density | REAL (0.0-1.0) |
| `graph_degree_centrality(node_id)` | Node degree centrality | REAL (0.0-1.0) |

### Virtual Tables

```sql
CREATE VIRTUAL TABLE graph USING graph();

-- Backing tables created automatically:
-- graph_nodes(id INTEGER, properties TEXT, labels TEXT)
-- graph_edges(id INTEGER, source INTEGER, target INTEGER, edge_type TEXT, properties TEXT)
```

**Direct SQL queries:**
```sql
-- Query nodes by label
SELECT * FROM graph_nodes WHERE labels LIKE '%Person%';

-- Query edges by type  
SELECT * FROM graph_edges WHERE edge_type = 'KNOWS';

-- Complex joins
SELECT a.properties, e.edge_type, b.properties
FROM graph_nodes a
JOIN graph_edges e ON e.source = a.id
JOIN graph_nodes b ON e.target = b.id;
```

## Performance

**Benchmarks (Alpha v0.1.0):**

| Operation | Throughput | Notes |
|-----------|------------|-------|
| Node creation | 300K+ nodes/sec | Tested up to 1,000 nodes |
| Edge creation | 390K+ edges/sec | Tested up to 999 edges |
| Connectivity check | <1ms | For 1,000 node graphs |
| Pattern matching | 180K nodes/sec | With WHERE filtering |

**Scalability:** Currently tested up to 1,000 nodes/edges. Larger scale testing planned for v0.2.0.

**Storage:** In-memory with SQLite persistence. Performance depends on available memory and disk I/O.

## Platform Support

### Supported Platforms

| Platform | Architecture | Status | Extension File |
|----------|--------------|--------|----------------|
| Linux | x86_64 | ✅ Tested | `libgraph.so` |
| macOS | x86_64, ARM64 | ⚠️ Builds, limited testing | `libgraph.dylib` |
| Windows | x86_64 | ⚠️ WSL only | `libgraph.so` |

### Requirements

- **SQLite**: 3.8.0+ compiled with `SQLITE_ENABLE_LOAD_EXTENSION`
- **glibc**: 2.27+ (Ubuntu 18.04+, Debian 10+)

### Platform-Specific Notes

**macOS:**
```bash
# May need to remove quarantine flag
xattr -d com.apple.quarantine libgraph.dylib
```

**Windows:**
- Use WSL (Windows Subsystem for Linux)
- Native Windows DLL support planned for v0.2.0

## Troubleshooting

### Extension Won't Load

**Problem:** `Error loading extension`

**Solutions:**
```python
# Check extension loading is enabled
conn.enable_load_extension(True)  # Must call before load_extension()

# Use absolute path
conn.load_extension("/full/path/to/libgraph.so")

# Check file permissions
# $ chmod +x libgraph.so
```

**Linux:** Ensure `libsqlite3.so` was compiled with `SQLITE_ENABLE_LOAD_EXTENSION`:
```bash
sqlite3 -version  # Should show extension support
```

### Cypher Query Errors

**Problem:** `Parse error in Cypher query`

**Common Issues:**
```python
# ❌ Wrong: Single quotes in property values
"CREATE (p:Person {name: 'Alice'})"  # Fails due to SQL escaping

# ✅ Correct: Double quotes in property values  
"CREATE (p:Person {name: \"Alice\"})"

# ✅ Alternative: Escape quotes
'CREATE (p:Person {name: "Alice"})'
```

### Build Failures

**Problem:** `make: *** No such file or directory`

**Solution:**
```bash
# Install build tools
sudo apt-get update
sudo apt-get install build-essential sqlite3 libsqlite3-dev

# Clone and build
git clone https://github.com/agentflare-ai/sqlite-graph.git
cd sqlite-graph
make
```

### Performance Issues

**Issue:** Slow queries with many nodes

**Tips:**
- Use labels in MATCH for better performance: `MATCH (p:Person)` vs `MATCH (p)`
- Add WHERE filters to reduce result sets
- Consider indexing for future versions (v0.2.0+)

**Need more help?** [Open an issue](https://github.com/agentflare-ai/sqlite-graph/issues)

## Documentation

### Essential Reading
- **[FEATURES.md](FEATURES.md)** - ⭐ Complete feature status and API details
- **[INSTALL.md](INSTALL.md)** - Detailed installation guide with platform-specific instructions
- **[ROADMAP.md](ROADMAP.md)** - Development timeline and planned features
- **[CHANGELOG.md](CHANGELOG.md)** - Version history and changes

### Examples & Tutorials
- [examples/](examples/) - Working Python code examples
  - `simple_graph_example.py` - Getting started
  - `python_examples.py` - Comprehensive examples
  - `cypher_demo.py` - Cypher query examples
  - `recommendation_system.py` - Real-world use case

## Contributing

Contributions are welcome! Here's how to get started:

1. **Check existing work:**
   - [FEATURES.md](FEATURES.md) - What's implemented
   - [ROADMAP.md](ROADMAP.md) - What's planned
   - [Issues](https://github.com/agentflare-ai/sqlite-graph/issues) - Known bugs and feature requests

2. **Report bugs:**
   - Include SQLite version, OS, and extension version
   - Provide minimal reproduction example
   - Check if issue already exists

3. **Request features:**
   - Check [ROADMAP.md](ROADMAP.md) first
   - Open issue with `[Feature Request]` tag
   - Describe use case and expected behavior

4. **Submit code:**
   - Fork repository
   - Create feature branch
   - Add tests for new functionality
   - Submit pull request with clear description

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

**Part of the AgentFlare AI ecosystem** | Built with SQLite | Version 0.1.0-alpha.0
