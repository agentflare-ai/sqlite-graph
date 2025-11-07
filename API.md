# API Reference

**Part of the AgentFlare AI ecosystem**

This document provides comprehensive documentation for the SQLite Graph Database Extension API, including the Cypher query language implementation and native SQLite functions.

## Table of Contents

- [Virtual Table Interface](#virtual-table-interface)
- [Cypher Query Language](#cypher-query-language)
- [Graph Functions](#graph-functions)
- [Node Operations](#node-operations)
- [Edge Operations](#edge-operations)
- [Graph Algorithms](#graph-algorithms)
- [Schema Management](#schema-management)
- [Error Handling](#error-handling)

## Virtual Table Interface

### Creating a Graph Table

```sql
CREATE VIRTUAL TABLE graph_name USING graph();
```

Creates a new graph virtual table that supports all graph operations.

### Basic Table Operations

```sql
-- Insert Cypher commands
INSERT INTO graph_name (command) VALUES ('CREATE (n:Person {name: "Alice"})');

-- Query with Cypher
SELECT * FROM graph_name WHERE command = 'MATCH (n:Person) RETURN n.name';

-- Get schema information
SELECT * FROM graph_name WHERE command = 'CALL db.schema()';
```

## Cypher Query Language

### Node Operations

#### Creating Nodes

```cypher
-- Create a node with label
CREATE (n:Person)

-- Create a node with properties
CREATE (n:Person {name: "Alice", age: 30})

-- Create multiple nodes
CREATE (alice:Person {name: "Alice"}), (bob:Person {name: "Bob"})

-- Create node with multiple labels
CREATE (n:Person:Employee {name: "Alice", department: "Engineering"})
```

#### Matching Nodes

```cypher
-- Match all nodes
MATCH (n) RETURN n

-- Match nodes by label
MATCH (n:Person) RETURN n

-- Match nodes by property
MATCH (n:Person {name: "Alice"}) RETURN n

-- Match with WHERE clause
MATCH (n:Person) WHERE n.age > 25 RETURN n.name, n.age

-- Match with multiple conditions
MATCH (n:Person) WHERE n.age > 25 AND n.department = "Engineering" RETURN n
```

#### Updating Nodes

```cypher
-- Set properties
MATCH (n:Person {name: "Alice"}) SET n.age = 31

-- Add labels
MATCH (n:Person {name: "Alice"}) SET n:Employee

-- Remove properties
MATCH (n:Person {name: "Alice"}) REMOVE n.age

-- Remove labels
MATCH (n:Person:Employee {name: "Alice"}) REMOVE n:Employee
```

#### Deleting Nodes
### MERGE

#### MERGE Nodes

```cypher
-- Merge node by label and properties (creates if not found)
MERGE (n:Person {name: "Alice"})
ON CREATE SET n.createdAt = 1234567890
ON MATCH SET n.lastSeen = 1234567890
```

Notes:
- Node MERGE matches by equality on provided properties and first label.
- ON CREATE and ON MATCH property sets are supported for nodes.
- Relationship MERGE is planned; relationship patterns in MERGE are not yet implemented.


```cypher
-- Delete a node and its relationships
MATCH (n:Person {name: "Alice"}) DELETE n

-- Delete all nodes (dangerous!)
MATCH (n) DELETE n
```

### Edge Operations

#### Creating Relationships

```cypher
-- Create relationship between existing nodes
MATCH (a:Person {name: "Alice"}), (b:Person {name: "Bob"})
CREATE (a)-[:KNOWS]->(b)

-- Create relationship with properties
MATCH (a:Person {name: "Alice"}), (b:Person {name: "Bob"})
CREATE (a)-[:KNOWS {since: 2020, strength: 0.8}]->(b)

-- Create nodes and relationship in one statement
CREATE (alice:Person {name: "Alice"})-[:KNOWS]->(bob:Person {name: "Bob"})
```

#### Matching Relationships

```cypher
-- Match any relationship
MATCH (a)-[r]-(b) RETURN a, r, b

-- Match specific relationship type
MATCH (a)-[r:KNOWS]->(b) RETURN a, r, b

-- Match with relationship properties
MATCH (a)-[r:KNOWS {since: 2020}]->(b) RETURN a, r, b

-- Match multiple relationship types
MATCH (a)-[r:KNOWS|FRIENDS]->(b) RETURN a, r, b

-- Match variable-length paths
MATCH (a)-[r*1..3]->(b) RETURN a, r, b

-- Match shortest path
MATCH path = shortestPath((a:Person {name: "Alice"})-[*]-(b:Person {name: "Bob"}))
RETURN path
```

#### Updating Relationships

```cypher
-- Set relationship properties
MATCH (a:Person {name: "Alice"})-[r:KNOWS]->(b:Person {name: "Bob"})
SET r.strength = 0.9

-- Remove relationship properties
MATCH (a:Person {name: "Alice"})-[r:KNOWS]->(b:Person {name: "Bob"})
REMOVE r.since
```

#### Deleting Relationships

```cypher
-- Delete specific relationship
MATCH (a:Person {name: "Alice"})-[r:KNOWS]->(b:Person {name: "Bob"})
DELETE r

-- Delete all relationships of a type
MATCH ()-[r:KNOWS]-() DELETE r
```

### Advanced Query Patterns

#### Aggregation

```cypher
-- Count nodes
MATCH (n:Person) RETURN COUNT(n)

-- Group by property
MATCH (n:Person) RETURN n.department, COUNT(n)

-- Calculate averages
MATCH (n:Person) RETURN AVG(n.age)

-- Find min/max
MATCH (n:Person) RETURN MIN(n.age), MAX(n.age)
```

#### Sorting and Limiting

```cypher
-- Sort results
MATCH (n:Person) RETURN n.name, n.age ORDER BY n.age DESC

-- Limit results
MATCH (n:Person) RETURN n.name LIMIT 10

-- Skip and limit (pagination)
MATCH (n:Person) RETURN n.name ORDER BY n.name SKIP 10 LIMIT 10
```

#### Conditional Logic

```cypher
-- CASE expressions
MATCH (n:Person)
RETURN n.name,
       CASE 
         WHEN n.age < 18 THEN "Minor"
         WHEN n.age < 65 THEN "Adult"
         ELSE "Senior"
       END AS age_group

-- OPTIONAL MATCH
MATCH (n:Person)
OPTIONAL MATCH (n)-[r:KNOWS]->(friend)
RETURN n.name, friend.name
```

#### Subqueries

```cypher
-- EXISTS clause
MATCH (n:Person)
WHERE EXISTS((n)-[:KNOWS]->())
RETURN n.name

-- NOT EXISTS
MATCH (n:Person)
WHERE NOT EXISTS((n)-[:KNOWS]->())
RETURN n.name AS loner
```

## Graph Functions

### Built-in Functions

#### String Functions

```cypher
-- String operations
MATCH (n:Person) WHERE n.name STARTS WITH "A" RETURN n.name
MATCH (n:Person) WHERE n.name ENDS WITH "e" RETURN n.name
MATCH (n:Person) WHERE n.name CONTAINS "lic" RETURN n.name

-- String manipulation
RETURN toUpper("hello") AS upper_case
RETURN toLower("WORLD") AS lower_case
RETURN substring("Alice", 1, 3) AS substr
```

#### Math Functions

```cypher
-- Numeric operations
RETURN abs(-5) AS absolute
RETURN ceil(3.14) AS ceiling
RETURN floor(3.14) AS floor
RETURN round(3.14159, 2) AS rounded
RETURN sqrt(16) AS square_root
```

#### Collection Functions

```cypher
-- Work with lists
RETURN size([1, 2, 3]) AS list_size
RETURN head([1, 2, 3]) AS first_element
RETURN tail([1, 2, 3]) AS rest_elements
RETURN [x IN [1, 2, 3] WHERE x > 1] AS filtered
```

### Graph-Specific Functions

#### Path Functions

```cypher
-- Path analysis
MATCH path = (a:Person {name: "Alice"})-[:KNOWS*]->(b:Person {name: "Bob"})
RETURN length(path) AS path_length

-- Extract nodes from path
MATCH path = (a)-[:KNOWS*]->(b)
RETURN nodes(path) AS path_nodes

-- Extract relationships from path
MATCH path = (a)-[:KNOWS*]->(b)
RETURN relationships(path) AS path_rels
```

#### Degree Functions

```cypher
-- Get node degree
MATCH (n:Person {name: "Alice"})
RETURN graph.degree(n) AS total_degree

-- Get in-degree
MATCH (n:Person {name: "Alice"})
RETURN graph.inDegree(n) AS in_degree

-- Get out-degree
MATCH (n:Person {name: "Alice"})
RETURN graph.outDegree(n) AS out_degree
```

## Graph Algorithms

### Shortest Path

```cypher
-- Find shortest path between two nodes
MATCH path = shortestPath((a:Person {name: "Alice"})-[*]-(b:Person {name: "Bob"}))
RETURN path

-- Find all shortest paths
MATCH path = allShortestPaths((a:Person {name: "Alice"})-[*]-(b:Person {name: "Bob"}))
RETURN path

-- Dijkstra's algorithm (weighted)
CALL graph.dijkstra({start: "Alice", end: "Bob", weightProperty: "distance"})
YIELD path, distance
RETURN path, distance
```

### Centrality Measures

```cypher
-- PageRank
CALL graph.pageRank({maxIterations: 20, dampingFactor: 0.85})
YIELD nodeId, score
RETURN nodeId, score ORDER BY score DESC

-- Betweenness centrality
CALL graph.betweennessCentrality()
YIELD nodeId, centrality
RETURN nodeId, centrality ORDER BY centrality DESC

-- Closeness centrality
CALL graph.closenessCentrality()
YIELD nodeId, centrality
RETURN nodeId, centrality ORDER BY centrality DESC
```

### Community Detection

```cypher
-- Connected components
CALL graph.connectedComponents()
YIELD nodeId, componentId
RETURN componentId, collect(nodeId) AS nodes

-- Strongly connected components
CALL graph.stronglyConnectedComponents()
YIELD nodeId, componentId
RETURN componentId, collect(nodeId) AS nodes
```

### Graph Properties

```cypher
-- Check if graph is connected
CALL graph.isConnected()
YIELD connected
RETURN connected

-- Calculate graph density
CALL graph.density()
YIELD density
RETURN density

-- Check for cycles
CALL graph.hasCycle()
YIELD hasCycle
RETURN hasCycle
```

## Schema Management

### Schema Information

```cypher
-- Get all node labels
CALL db.labels()
YIELD label
RETURN label

-- Get all relationship types
CALL db.relationshipTypes()
YIELD relationshipType
RETURN relationshipType

-- Get property keys
CALL db.propertyKeys()
YIELD propertyKey
RETURN propertyKey

-- Get schema overview
CALL db.schema()
YIELD description
RETURN description
```

### Constraints and Indexes

```cypher
-- Create uniqueness constraint
CREATE CONSTRAINT ON (n:Person) ASSERT n.email IS UNIQUE

-- Create property index
CREATE INDEX ON :Person(name)

-- Create composite index
CREATE INDEX ON :Person(name, age)

-- List constraints
CALL db.constraints()
YIELD description
RETURN description

-- List indexes
CALL db.indexes()
YIELD description
RETURN description
```

## Error Handling

### Common Error Codes

- `SQLITE_OK` (0): Success
- `SQLITE_ERROR` (1): Generic error
- `SQLITE_CONSTRAINT` (19): Constraint violation
- `SQLITE_NOTFOUND` (12): Node/edge not found
- `SQLITE_NOMEM` (7): Out of memory

### Error Messages

```cypher
-- Syntax error example
CREATE (n:Person {name: "Alice"  -- Missing closing brace
-- Error: Syntax error at line 1, column 30: Expected '}'

-- Constraint violation example
CREATE (n:Person {email: "alice@example.com"})
CREATE (m:Person {email: "alice@example.com"})
-- Error: Constraint violation: email must be unique

-- Type error example
MATCH (n:Person) WHERE n.age = "thirty"
-- Error: Type mismatch: cannot compare integer with string
```

### Best Practices

1. **Use transactions** for multiple operations:
```sql
BEGIN;
INSERT INTO graph (command) VALUES ('CREATE (n:Person {name: "Alice"})');
INSERT INTO graph (command) VALUES ('CREATE (n:Person {name: "Bob"})');
COMMIT;
```

2. **Check for existence** before operations:
```cypher
MERGE (n:Person {name: "Alice"})
ON CREATE SET n.created = timestamp()
ON MATCH SET n.updated = timestamp()
```

3. **Use parameterized queries** in applications:
```python
conn.execute("INSERT INTO graph (command) VALUES (?)", 
            ["CREATE (n:Person {name: $name})"])
```

4. **Handle large result sets** with pagination:
```cypher
MATCH (n:Person)
RETURN n.name
ORDER BY n.name
SKIP $offset LIMIT $limit
```

## Performance Tips

1. **Use indexes** for frequently queried properties
2. **Limit result sets** with `LIMIT` clauses
3. **Use `EXPLAIN`** to analyze query plans
4. **Batch operations** in transactions
5. **Use `MERGE`** instead of `CREATE` when appropriate

## Examples

See the [examples/](examples/) directory for complete working examples including:

- Social network analysis
- Recommendation systems
- Fraud detection
- Knowledge graphs
- Network topology analysis

---

**For more information**, see the [README](README.md) and [Installation Guide](INSTALL.md).
