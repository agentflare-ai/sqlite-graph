# SQLite Graph Extension - Python Examples

This directory contains comprehensive Python examples demonstrating how to use the AgentFlare AI SQLite Graph Database Extension from Python applications.

## Prerequisites

1. **Python 3.6+** with sqlite3 module (built-in)
2. **Compiled graph extension** (libgraph.so)
3. **Optional**: Jupyter notebook for interactive examples

## Quick Start

### 1. Simple Example

Start with the basic example to verify everything works:

```bash
cd examples
python simple_graph_example.py
```

This example shows:
- Loading the extension
- Creating nodes and edges
- Running basic graph algorithms
- Testing Cypher query parsing

### 2. Comprehensive Examples

Run the full feature demonstration:

```bash
python python_examples.py
```

This covers:
- All SQL functions available
- Graph algorithms and analysis
- Cypher query operations  
- Write operations (CREATE, MERGE, SET, DELETE)
- Performance testing
- Social network modeling

### 3. Practical Application

See a real-world use case with the recommendation system:

```bash
python recommendation_system.py
```

This demonstrates:
- Building a movie recommendation graph
- User similarity analysis
- Movie recommendations based on graph algorithms
- Popularity analysis

### 4. Interactive Tutorial

Launch the Jupyter notebook for an interactive experience:

```bash
jupyter notebook graph_database_tutorial.ipynb
```

## Available Examples

| File | Description | Complexity |
|------|-------------|------------|
| `simple_graph_example.py` | Basic operations and setup | Beginner |
| `python_examples.py` | Comprehensive feature demo | Intermediate |
| `recommendation_system.py` | Real-world application | Advanced |
| `graph_database_tutorial.ipynb` | Interactive tutorial | All levels |

## Extension Functions

The SQLite Graph Extension provides these SQL functions:

### Core Graph Operations
```sql
-- Node and edge management
SELECT graph_node_add(node_id, properties_json);
SELECT graph_edge_add(from_id, to_id, type, properties_json);
SELECT graph_count_nodes();
SELECT graph_count_edges();
```

### Graph Algorithms
```sql
-- Path finding and analysis
SELECT graph_shortest_path(from_id, to_id);
SELECT graph_degree_centrality(node_id);
SELECT graph_is_connected();
SELECT graph_density();
SELECT graph_has_cycle();
```

### Cypher Support
```sql
-- Query parsing and validation
SELECT cypher_parse(query_string);
SELECT cypher_validate(query_string);
SELECT cypher_execute(query_string);
```

### Write Operations
```sql
-- Transaction management
SELECT cypher_begin_write();
SELECT cypher_commit_write();
SELECT cypher_rollback_write();

-- Node and relationship operations
SELECT cypher_create_node(id, labels, properties);
SELECT cypher_merge_node(id, labels, match_props, create_props);
SELECT cypher_set_property(type, id, property, value);
SELECT cypher_delete_node(type, id, properties);
```

## Python Usage Patterns

### Basic Connection Setup

```python
import sqlite3
import json
import os

# Connect to database
conn = sqlite3.connect(":memory:")  # or file path
conn.row_factory = sqlite3.Row

# Load extension
if os.path.exists("build/libgraph.so"):
    conn.enable_load_extension(True)
    conn.load_extension("build/libgraph.so")

cursor = conn.cursor()
```

### Creating Nodes

```python
# Create a node with JSON properties
node_data = {"name": "Alice", "age": 30, "city": "NYC"}
cursor.execute("SELECT graph_node_add(?, ?) as result", 
               (1, json.dumps(node_data)))
result = cursor.fetchone()
print(f"Node created: {result['result']}")
```

### Creating Edges

```python
# Create relationship between nodes
edge_data = {"relationship": "FRIENDS", "since": "2020"}
cursor.execute("SELECT graph_edge_add(?, ?, ?, ?) as result",
               (1, 2, "FRIENDS", json.dumps(edge_data)))
result = cursor.fetchone()
print(f"Edge created: {result['result']}")
```

### Running Algorithms

```python
# Check if graph is connected
cursor.execute("SELECT graph_is_connected() as connected")
result = cursor.fetchone()
print(f"Graph connected: {bool(result['connected'])}")

# Find shortest path
cursor.execute("SELECT graph_shortest_path(?, ?) as path", (1, 2))
result = cursor.fetchone()
print(f"Shortest path: {result['path']}")

# Calculate centrality
cursor.execute("SELECT graph_degree_centrality(?) as centrality", (1,))
result = cursor.fetchone()
print(f"Node centrality: {result['centrality']}")
```

### Error Handling

```python
try:
    cursor.execute("SELECT graph_some_function() as result")
    result = cursor.fetchone()
    print(f"Success: {result['result']}")
except sqlite3.Error as e:
    print(f"Error: {e}")
```

## Common Use Cases

### 1. Social Networks
- Model users, friendships, interactions
- Find mutual friends, influence analysis
- Recommend connections, detect communities

### 2. Recommendation Systems  
- User-item relationships with ratings
- Collaborative filtering using graph algorithms
- Content-based recommendations

### 3. Knowledge Graphs
- Entities and relationships modeling
- Semantic queries and inference
- Knowledge discovery

### 4. Network Analysis
- Infrastructure modeling (servers, connections)
- Dependency analysis
- Failure impact assessment

### 5. Fraud Detection
- Transaction networks
- Pattern recognition
- Anomaly detection using graph metrics

## Performance Tips

1. **Batch Operations**: Create multiple nodes/edges in transactions
2. **Index Usage**: Leverage SQLite indexes for large graphs
3. **Memory Management**: Use file-based databases for large datasets
4. **Algorithm Selection**: Choose appropriate algorithms for your use case

## Troubleshooting

### Extension Not Loading
```python
# Check if extension file exists
import os
extension_path = "build/libgraph.so"
if not os.path.exists(extension_path):
    print(f"Extension not found: {extension_path}")
    print("Please compile the extension first")
```

### Function Not Found
```python
# Test if extension loaded correctly
try:
    cursor.execute("SELECT graph_count_nodes()")
    print("Extension loaded successfully")
except sqlite3.Error as e:
    print(f"Extension error: {e}")
```

### Memory Issues
```python
# Use file database for large graphs
conn = sqlite3.connect("large_graph.db")  # instead of ":memory:"
```

## Next Steps

1. **Experiment** with the provided examples
2. **Modify** the sample data for your use case
3. **Integrate** with your existing applications
4. **Scale** to larger datasets
5. **Contribute** improvements to the project

## Support

- Check the main project documentation
- Review test files for additional examples
- Submit issues for bugs or feature requests
- Contribute improvements via pull requests

## License

These examples are provided under the same license as the main AgentFlare AI SQLite Graph Extension project.

---

**Part of the AgentFlare AI ecosystem**
