# SQLite Graph Extension Migration Guide

**Part of the AgentFlare AI ecosystem**

## Table of Contents

1. [Overview](#overview)
2. [Before You Migrate](#before-you-migrate)
3. [Migration Strategy](#migration-strategy)
4. [Step-by-Step Migration](#step-by-step-migration)
5. [Schema Changes](#schema-changes)
6. [API Changes](#api-changes)
7. [Performance Considerations](#performance-considerations)
8. [Testing Migration](#testing-migration)
9. [Rollback Strategy](#rollback-strategy)
10. [Common Issues](#common-issues)

## Overview

This guide helps you migrate from the SQLite Graph Extension prototype to the production-ready version. The migration addresses:

- **Infrastructure Changes**: Improved storage format and indexing
- **API Evolution**: Enhanced function signatures and new capabilities
- **Performance Optimizations**: Better algorithms and memory management
- **Schema Updates**: More efficient internal representation
- **Feature Additions**: New algorithms and Cypher support

### What Changed

| Component | Prototype | Production | Impact |
|-----------|-----------|------------|---------|
| Storage Format | Basic adjacency lists | Optimized compressed format | Schema migration required |
| Cypher Support | Limited subset | Full CRUD operations | New capabilities |
| Memory Management | Basic allocation | Pooled memory with compression | Better performance |
| Thread Safety | Limited | Full thread safety | Concurrent operations |
| Performance | Basic algorithms | Optimized with caching | Significant speed improvements |

## Before You Migrate

### Backup Your Data

```bash
# Create a complete backup of your database
cp your_database.db your_database_backup.db

# Export graph data to JSON for safety
sqlite3 your_database.db << EOF
.load ./old_graph_extension
.mode json
.output graph_backup.json
SELECT * FROM your_graph_table;
.quit
EOF
```

### Assess Your Current Usage

1. **Inventory Your Graphs**
   ```sql
   -- List all graph virtual tables
   SELECT name FROM sqlite_master 
   WHERE type = 'table' AND sql LIKE '%USING graph%';
   ```

2. **Document Custom Functions**
   ```sql
   -- Check for custom graph functions in use
   .schema
   -- Look for any custom graph-related functions or triggers
   ```

3. **Analyze Query Patterns**
   ```sql
   -- Review your application code for:
   -- - Direct graph table queries
   -- - Custom traversal logic
   -- - Performance-critical operations
   ```

### Version Compatibility Check

```sql
-- Check current extension version
SELECT graph_version();

-- Verify minimum SQLite version
SELECT sqlite_version();
-- Must be >= 3.35.0 for production extension
```

## Migration Strategy

### Option 1: In-Place Migration (Recommended)

Best for small to medium graphs (< 1M nodes):

1. Backup data
2. Load new extension
3. Run migration utility
4. Test functionality
5. Update application code

### Option 2: Side-by-Side Migration

Best for large graphs or critical systems:

1. Create new database with production extension
2. Export data from old format
3. Import data to new format
4. Validate data integrity
5. Switch application to new database

### Option 3: Gradual Migration

Best for complex applications:

1. Keep both versions temporarily
2. Migrate non-critical graphs first
3. Update application incrementally
4. Migrate critical graphs last

## Step-by-Step Migration

### Step 1: Prepare Environment

```bash
# Ensure you have the latest production extension
make clean
make production

# Verify new extension
sqlite3 test.db << EOF
.load ./graph_extension
SELECT graph_version();
-- Should show production version (1.0.0+)
.quit
EOF
```

### Step 2: Migrate Schema

```sql
-- Load the migration utility
.load ./graph_extension

-- Run schema migration for each graph table
SELECT graph_migrate_schema('your_graph_table');

-- Verify migration
PRAGMA table_info(your_graph_table);
```

### Step 3: Update Data Format

The production version uses an optimized storage format:

```sql
-- Migrate existing data
SELECT graph_migrate_data('your_graph_table');

-- Rebuild indexes for performance
REINDEX your_graph_table;
ANALYZE your_graph_table;

-- Compress data (optional, for large graphs)
SELECT graph_compress('your_graph_table');
```

### Step 4: Application Code Updates

#### Update Extension Loading

```c
// Old prototype loading
if (sqlite3_load_extension(db, "./graph_prototype", NULL, &zErrMsg) != SQLITE_OK) {
    // error handling
}

// New production loading
if (sqlite3_load_extension(db, "./graph_extension", NULL, &zErrMsg) != SQLITE_OK) {
    // error handling
}
```

#### Update Function Calls

```c
// Old prototype API
int graph_add_node_old(sqlite3 *db, int node_id, const char *data);

// New production API  
int graph_add_node(Graph *pGraph, sqlite3_int64 iNodeId, 
                   const char **azLabels, int nLabels, 
                   const char *zProperties);
```

#### Update SQL Queries

```sql
-- Old prototype queries
INSERT INTO graph_table (id, data) VALUES (1, 'node_data');

-- New production queries  
INSERT INTO graph_table (node_id, labels, properties) VALUES 
    (1, '["Person"]', '{"name": "John", "age": 30}');
```

## Schema Changes

### Node Storage Format

#### Prototype Format
```sql
CREATE TABLE nodes (
    id INTEGER PRIMARY KEY,
    data TEXT
);
```

#### Production Format
```sql
-- Automatically created by virtual table
CREATE VIRTUAL TABLE my_graph USING graph();
-- Columns: node_id, labels, properties, edge_id, from_id, to_id, edge_type, weight
```

### Migration Script

```sql
-- Migrate nodes from old format
WITH old_nodes AS (
    SELECT id, data FROM old_graph_nodes
)
INSERT INTO new_graph (node_id, labels, properties)
SELECT 
    id,
    '["Node"]',  -- Default label
    CASE 
        WHEN json_valid(data) THEN data
        ELSE json_object('data', data)
    END
FROM old_nodes;

-- Migrate edges from old format
WITH old_edges AS (
    SELECT from_id, to_id, weight, data FROM old_graph_edges
)
INSERT INTO new_graph (edge_id, from_id, to_id, edge_type, weight, properties)
SELECT 
    rowid,
    from_id,
    to_id,
    'CONNECTED',  -- Default edge type
    COALESCE(weight, 1.0),
    CASE 
        WHEN json_valid(data) THEN data
        ELSE json_object('data', data)
    END
FROM old_edges;
```

## API Changes

### Function Signature Updates

#### Graph Creation

```c
// Old prototype
int create_graph_table(sqlite3 *db, const char *table_name);

// New production
int graph_create(sqlite3 *db, const char *zName, GraphOptions *pOptions);
```

#### Node Operations

```c
// Old prototype
int add_node(sqlite3 *db, int node_id, const char *data);
int get_node(sqlite3 *db, int node_id, char **data);

// New production
int graph_add_node(Graph *pGraph, sqlite3_int64 iNodeId, 
                   const char **azLabels, int nLabels, 
                   const char *zProperties);
int graph_find_node(Graph *pGraph, sqlite3_int64 iNodeId, GraphNode **ppNode);
```

#### Traversal Operations

```c
// Old prototype
int find_path(sqlite3 *db, int start, int end, int **path, int *path_len);

// New production
int graph_shortest_path(Graph *pGraph, sqlite3_int64 iStartId, 
                       sqlite3_int64 iEndId, GraphPath **ppPath);
```

### New Cypher Support

```c
// New in production version
int cypher_execute(Graph *pGraph, const char *zQuery, 
                   CypherResult **ppResult);

// Example usage
CypherResult *pResult;
int rc = cypher_execute(pGraph, 
    "MATCH (n:Person)-[:KNOWS]->(friend) RETURN friend.name", 
    &pResult);
```

### Error Handling Improvements

```c
// Old prototype - basic error codes
if (rc != SQLITE_OK) {
    printf("Error: %s\n", sqlite3_errmsg(db));
}

// New production - detailed error information
if (rc != GRAPH_OK) {
    printf("Error %d: %s\n", graph_errcode(pGraph), graph_errmsg(pGraph));
    // Additional context available
    const char *context = graph_error_context(pGraph);
    if (context) {
        printf("Context: %s\n", context);
    }
}
```

## Performance Considerations

### Memory Usage

```c
// Configure memory pools for better performance
GraphOptions options;
options.cache_size = 10000;        // LRU cache size
options.memory_pool_size = 256;    // MB
options.thread_pool_size = 4;      // Worker threads

graph_create_with_options(db, "my_graph", &options);
```

### Index Configuration

```sql
-- Create performance indexes
CREATE INDEX idx_node_labels ON my_graph(labels) WHERE edge_id IS NULL;
CREATE INDEX idx_edge_types ON my_graph(edge_type) WHERE edge_id IS NOT NULL;
CREATE INDEX idx_from_to ON my_graph(from_id, to_id) WHERE edge_id IS NOT NULL;

-- Property indexes for frequent queries
CREATE INDEX idx_name ON my_graph(
    json_extract(properties, '$.name')
) WHERE edge_id IS NULL;
```

### Query Optimization

```sql
-- Old prototype style
SELECT * FROM nodes WHERE data LIKE '%John%';

-- New production style with proper indexing
SELECT * FROM my_graph 
WHERE edge_id IS NULL 
  AND json_extract(properties, '$.name') = 'John';
```

## Testing Migration

### Data Integrity Tests

```sql
-- Verify node count
SELECT 
    'Nodes' as type,
    (SELECT COUNT(*) FROM old_graph_nodes) as old_count,
    (SELECT COUNT(*) FROM my_graph WHERE edge_id IS NULL) as new_count;

-- Verify edge count  
SELECT 
    'Edges' as type,
    (SELECT COUNT(*) FROM old_graph_edges) as old_count,
    (SELECT COUNT(*) FROM my_graph WHERE edge_id IS NOT NULL) as new_count;

-- Verify specific data
SELECT 
    old.id,
    old.data as old_data,
    new.properties as new_properties
FROM old_graph_nodes old
JOIN my_graph new ON new.node_id = old.id
WHERE new.edge_id IS NULL
LIMIT 10;
```

### Functionality Tests

```c
// Test basic operations
void test_migration_functionality() {
    Graph *pGraph;
    graph_open(db, "my_graph", &pGraph);
    
    // Test node operations
    GraphNode *pNode;
    int rc = graph_find_node(pGraph, 1, &pNode);
    assert(rc == GRAPH_OK);
    assert(pNode != NULL);
    
    // Test traversal
    GraphPath *pPath;
    rc = graph_shortest_path(pGraph, 1, 10, &pPath);
    assert(rc == GRAPH_OK);
    
    // Test Cypher
    CypherResult *pResult;
    rc = cypher_execute(pGraph, "MATCH (n) RETURN COUNT(n)", &pResult);
    assert(rc == GRAPH_OK);
    
    // Cleanup
    graph_path_free(pPath);
    cypher_result_free(pResult);
    graph_node_free(pNode);
    graph_close(pGraph);
}
```

### Performance Tests

```c
// Benchmark migration performance
void benchmark_migration() {
    clock_t start, end;
    
    // Test traversal performance
    start = clock();
    GraphPath *pPath;
    graph_shortest_path(pGraph, 1, 1000, &pPath);
    end = clock();
    
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Shortest path took %f seconds\n", time_taken);
    
    graph_path_free(pPath);
}
```

## Rollback Strategy

### Quick Rollback

```bash
# If issues occur, quickly restore from backup
cp your_database_backup.db your_database.db

# Use old extension temporarily
sqlite3 your_database.db << EOF
.load ./old_graph_extension
-- Your existing application should work
.quit
EOF
```

### Partial Rollback

```sql
-- Rollback specific tables if needed
DROP TABLE migrated_graph;
ALTER TABLE old_graph_backup RENAME TO your_graph;
```

### Data Export for Rollback

```sql
-- Export data in old format for rollback
CREATE TABLE nodes_rollback AS
SELECT 
    node_id as id,
    CASE 
        WHEN json_valid(properties) THEN properties
        ELSE json_object('data', properties)
    END as data
FROM my_graph 
WHERE edge_id IS NULL;

CREATE TABLE edges_rollback AS
SELECT 
    from_id,
    to_id,
    weight,
    CASE 
        WHEN json_valid(properties) THEN properties
        ELSE json_object('data', properties)
    END as data
FROM my_graph 
WHERE edge_id IS NOT NULL;
```

## Common Issues

### Issue 1: Extension Loading Fails

**Problem**: New extension won't load
```
Error: Could not load extension
```

**Solution**:
```bash
# Check extension file exists and is correct architecture
file ./graph_extension.so
# Should match your system (x86_64, ARM, etc.)

# Check dependencies
ldd ./graph_extension.so
# Ensure all libraries are available

# Check SQLite version
sqlite3 --version
# Must be >= 3.35.0
```

### Issue 2: Schema Migration Fails

**Problem**: 
```
Error: table my_graph already exists
```

**Solution**:
```sql
-- Drop old table first
DROP TABLE IF EXISTS my_graph;

-- Recreate with new schema
CREATE VIRTUAL TABLE my_graph USING graph();

-- Re-import data
-- ... run migration scripts
```

### Issue 3: Performance Degradation

**Problem**: Queries are slower after migration

**Solution**:
```sql
-- Analyze table statistics
ANALYZE my_graph;

-- Create missing indexes
CREATE INDEX idx_performance ON my_graph(...);

-- Configure caching
PRAGMA graph.cache_size = 10000;

-- Enable parallel processing
PRAGMA graph.parallel_execution = ON;
```

### Issue 4: Data Type Mismatches

**Problem**: 
```
Error: JSON property value type mismatch
```

**Solution**:
```sql
-- Check and fix JSON formatting
UPDATE my_graph 
SET properties = json(properties)
WHERE NOT json_valid(properties);

-- Convert numeric strings to numbers
UPDATE my_graph 
SET properties = json_set(properties, '$.age', 
    CAST(json_extract(properties, '$.age') AS INTEGER))
WHERE json_type(properties, '$.age') = 'text';
```

### Issue 5: Memory Issues

**Problem**: Out of memory during migration

**Solution**:
```c
// Migrate in batches
int batch_size = 1000;
for (int offset = 0; offset < total_nodes; offset += batch_size) {
    // Migrate batch
    migrate_node_batch(offset, batch_size);
    
    // Clear caches
    graph_clear_cache(pGraph);
}
```

## Migration Checklist

### Pre-Migration
- [ ] Backup all databases
- [ ] Test new extension in development
- [ ] Document current API usage
- [ ] Plan rollback strategy
- [ ] Notify stakeholders

### During Migration
- [ ] Load new extension
- [ ] Run schema migration
- [ ] Migrate data format
- [ ] Create performance indexes
- [ ] Update application code
- [ ] Run integrity tests

### Post-Migration
- [ ] Verify data integrity
- [ ] Test all functionality
- [ ] Monitor performance
- [ ] Update documentation
- [ ] Train team on new features

### Final Validation
- [ ] All tests pass
- [ ] Performance meets expectations
- [ ] No data loss
- [ ] Application works correctly
- [ ] Backup strategy updated

## Getting Help

### Resources
- API Reference: `docs/API_REFERENCE.md`
- Performance Guide: `docs/PERFORMANCE_TUNING_GUIDE.md`
- Tutorials: `docs/TUTORIALS.md`

### Support Channels
- GitHub Issues: Report migration problems
- Documentation: Check for latest updates
- Community Forum: Get help from other users

### Professional Services
For complex migrations or custom requirements, consider professional migration services to ensure smooth transition with minimal downtime.

---

**Remember**: Migration is a critical process. Always test thoroughly in a development environment before migrating production systems.