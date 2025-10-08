# sqlite-utils-sqlite-graph

A [sqlite-utils](https://sqlite-utils.datasette.io/) plugin that adds graph database capabilities to SQLite using the AgentFlare AI SQLite Graph extension.

## Installation

First, make sure you have the SQLite Graph extension built and available. Then install this plugin:

```bash
sqlite-utils install sqlite-utils-sqlite-graph
```

Or install from source:

```bash
git clone https://github.com/agentflare-ai/sqlite-graph.git
cd sqlite-graph/sqlite-utils-plugin
sqlite-utils install -e .
```

## Usage

### Basic Graph Operations

```bash
# Create a graph and execute Cypher queries
sqlite-utils graph mydb.db --cypher "CREATE (p:Person {name: 'Alice', age: 30})"

# Query the graph
sqlite-utils graph mydb.db --cypher "MATCH (p:Person) RETURN p.name, p.age"

# Output as JSON
sqlite-utils graph mydb.db --cypher "MATCH (p:Person) RETURN p.name, p.age" --output json

# Output as CSV
sqlite-utils graph mydb.db --cypher "MATCH (p:Person) RETURN p.name, p.age" --output csv
```

### Graph Information

```bash
# Show graph statistics
sqlite-utils graph-info mydb.db

# Create sample data for testing
sqlite-utils graph-sample mydb.db --nodes 100 --edges 200
```

### Complex Queries

```bash
# Find shortest paths
sqlite-utils graph mydb.db --cypher "MATCH path = shortestPath((a:Person {name: 'Alice'})-[*]-(b:Person {name: 'Bob'})) RETURN path"

# PageRank algorithm
sqlite-utils graph mydb.db --cypher "CALL graph.pageRank() YIELD nodeId, score RETURN nodeId, score ORDER BY score DESC LIMIT 10"

# Pattern matching
sqlite-utils graph mydb.db --cypher "MATCH (p1:Person)-[:KNOWS]->(p2:Person) WHERE p1.age > 25 RETURN p1.name, p2.name"
```

## Commands

### `graph`

Execute Cypher queries on a SQLite graph database.

```bash
sqlite-utils graph DATABASE [OPTIONS]
```

Options:
- `--cypher`, `-c`: Execute a Cypher query
- `--output`, `-o`: Output format (table, json, csv)

### `graph-info`

Show information about the graph database including node/edge counts and label distributions.

```bash
sqlite-utils graph-info DATABASE
```

### `graph-sample`

Create sample graph data for testing.

```bash
sqlite-utils graph-sample DATABASE [OPTIONS]
```

Options:
- `--nodes`, `-n`: Number of sample nodes to create
- `--edges`, `-e`: Number of sample edges to create

## Extension Loading

The plugin automatically tries to load the SQLite Graph extension from these locations:
- `./build/libgraph.so` (local build)
- `./libgraph.so` (current directory)
- `/usr/local/lib/libgraph.so` (system-wide install)
- `/usr/lib/libgraph.so` (alternative system location)

Make sure the extension is built and available in one of these paths.

## Requirements

- Python 3.8+
- sqlite-utils 3.0+
- SQLite Graph extension (libgraph.so)

## License

MIT License

---

**Part of the AgentFlare AI ecosystem**
