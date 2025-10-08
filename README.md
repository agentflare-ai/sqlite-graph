# SQLite Graph Database Extension

A powerful SQLite extension that adds graph database capabilities with full Cypher query support. Part of the AgentFlare AI ecosystem. Build sophisticated graph applications with the reliability of SQLite and the expressiveness of Cypher.

## Features

- **Full Graph Database Support**: Nodes, edges, properties, labels, and relationship types
- **Cypher Query Language**: Complete implementation of openCypher standard
- **SQLite Integration**: Seamless integration with existing SQLite databases
- **Advanced Algorithms**: Shortest path, PageRank, centrality measures, and more
- **Performance Optimized**: Efficient indexing and query planning
- **Thread Safe**: Supports SQLite's threading modes
- **Python Bindings**: Easy-to-use Python interface

## Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/agentflare-ai/sqlite-graph.git
cd sqlite-graph

# Build the extension
make

# The extension will be built as build/libgraph.so
```

### Basic Usage

```python
import sqlite3

# Load the extension
conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)
conn.load_extension("./build/libgraph.so")

# Create a graph virtual table
conn.execute("CREATE VIRTUAL TABLE graph USING graph()")

# Add nodes and edges using Cypher
conn.execute("CYPHER 'CREATE (p:Person {name: \"Alice\", age: 30})'")
conn.execute("CYPHER 'CREATE (p:Person {name: \"Bob\", age: 25})'")
conn.execute("CYPHER 'MATCH (a:Person {name: \"Alice\"}), (b:Person {name: \"Bob\"}) CREATE (a)-[:KNOWS]->(b)'")

# Query the graph
result = conn.execute("CYPHER 'MATCH (p:Person) RETURN p.name, p.age'").fetchall()
print(result)  # [('Alice', 30), ('Bob', 25)]
```

### Advanced Features

```python
# Find shortest path
path = conn.execute("""
    CYPHER 'MATCH path = shortestPath((a:Person {name: "Alice"})-[*]-(b:Person {name: "Bob"}))
            RETURN path'
""").fetchall()

# PageRank algorithm
pagerank = conn.execute("""
    CYPHER 'CALL graph.pageRank() YIELD nodeId, score RETURN nodeId, score'
""").fetchall()

# Pattern matching with complex relationships
results = conn.execute("""
    CYPHER 'MATCH (p1:Person)-[:KNOWS]->(p2:Person)-[:WORKS_AT]->(c:Company)
            WHERE p1.age > 25
            RETURN p1.name, p2.name, c.name'
""").fetchall()
```

## Documentation

- [Installation Guide](INSTALL.md) - Detailed build and installation instructions
- [API Reference](API.md) - Complete Cypher query language documentation
- [Contributing Guide](CONTRIBUTING.md) - Development setup and guidelines
- [Examples](examples/) - Sample applications and tutorials

## Architecture

The extension consists of several key components:

- **Virtual Table Interface**: SQLite virtual table implementation for graph operations
- **Cypher Parser**: Full openCypher query parser and AST generator
- **Query Planner**: Optimizes Cypher queries for efficient execution
- **Storage Engine**: Efficient in-memory graph storage with persistence
- **Algorithm Library**: Graph algorithms (shortest path, centrality, etc.)

## Performance

- **Scalability**: Handles graphs with millions of nodes and edges
- **Indexing**: Automatic label and property indexing for fast queries
- **Memory Efficient**: Optimized data structures with minimal overhead
- **Query Optimization**: Cost-based query planning for complex patterns

## Examples

### Social Network Analysis
```python
# Create a social network
conn.execute("CYPHER 'CREATE (alice:Person {name: \"Alice\", city: \"NYC\"})'")
conn.execute("CYPHER 'CREATE (bob:Person {name: \"Bob\", city: \"LA\"})'")
conn.execute("CYPHER 'CREATE (carol:Person {name: \"Carol\", city: \"NYC\"})'")

# Add friendships
conn.execute("CYPHER 'MATCH (a:Person {name: \"Alice\"}), (b:Person {name: \"Bob\"}) CREATE (a)-[:FRIENDS]->(b)'")
conn.execute("CYPHER 'MATCH (a:Person {name: \"Alice\"}), (c:Person {name: \"Carol\"}) CREATE (a)-[:FRIENDS]->(c)'")

# Find mutual friends
mutual_friends = conn.execute("""
    CYPHER 'MATCH (a:Person {name: \"Alice\"})-[:FRIENDS]->(mutual)<-[:FRIENDS]-(b:Person {name: \"Bob\"})
            RETURN mutual.name as mutual_friend'
""").fetchall()
```

### Recommendation System
```python
# Find people Alice might know (friends of friends)
recommendations = conn.execute("""
    CYPHER 'MATCH (alice:Person {name: \"Alice\"})-[:FRIENDS]->(friend)-[:FRIENDS]->(recommendation)
            WHERE NOT (alice)-[:FRIENDS]->(recommendation) AND alice <> recommendation
            RETURN recommendation.name, COUNT(*) as mutual_friends
            ORDER BY mutual_friends DESC'
""").fetchall()
```

## Testing

```bash
# Run all tests
make test

# Run specific test
cd build/tests && ./test_cypher_basic

# Run with coverage
make coverage
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details on:

- Setting up the development environment
- Code style and conventions
- Testing requirements
- Submitting pull requests

## Support

- **Issues**: [GitHub Issues](https://github.com/agentflare-ai/sqlite-graph/issues)
- **Discussions**: [GitHub Discussions](https://github.com/agentflare-ai/sqlite-graph/discussions)
- **Documentation**: [Wiki](https://github.com/agentflare-ai/sqlite-graph/wiki)

## Roadmap

- [ ] Distributed graph processing
- [ ] GraphQL API layer
- [ ] More graph algorithms (community detection, graph neural networks)
- [ ] Performance optimizations for large graphs
- [ ] Integration with popular data science tools

---

**Part of the AgentFlare AI ecosystem • Built with SQLite and openCypher**
