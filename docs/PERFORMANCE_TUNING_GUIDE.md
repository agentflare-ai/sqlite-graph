# SQLite Graph Extension Performance Tuning Guide

**Part of the AgentFlare AI ecosystem**

## Table of Contents

1. [Introduction](#introduction)
2. [Performance Fundamentals](#performance-fundamentals)
3. [Configuration Options](#configuration-options)
4. [Indexing Strategies](#indexing-strategies)
5. [Query Optimization](#query-optimization)
6. [Memory Management](#memory-management)
7. [Parallel Processing](#parallel-processing)
8. [Bulk Operations](#bulk-operations)
9. [Monitoring and Profiling](#monitoring-and-profiling)
10. [Troubleshooting](#troubleshooting)
11. [Best Practices](#best-practices)

## Introduction

This guide provides detailed information on optimizing the performance of the SQLite Graph Extension. By following these recommendations, you can achieve:

- Query response times < 100ms for graphs with 10K nodes
- Bulk loading speeds > 10K nodes/second
- Efficient memory usage < 100MB overhead for 10K nodes
- Scalability to millions of nodes and edges

## Performance Fundamentals

### Understanding Graph Performance

Graph operations have different performance characteristics than traditional relational queries:

1. **Traversal Complexity**: O(V + E) for breadth-first search
2. **Path Finding**: O((V + E) log V) for Dijkstra's algorithm
3. **Pattern Matching**: Can be exponential without proper optimization
4. **Memory Access**: Random access patterns reduce cache efficiency

### Key Performance Metrics

Monitor these metrics to understand performance:

```sql
-- Enable performance monitoring
PRAGMA graph.performance_monitor = ON;

-- View performance statistics
SELECT * FROM graph_performance_stats();
```

Metrics include:
- Query execution time
- Memory usage
- Cache hit rates
- I/O operations
- Thread utilization

## Configuration Options

### Core Settings

```sql
-- Set cache size (in pages, default: 1000)
PRAGMA graph.cache_size = 10000;

-- Enable query plan caching
PRAGMA graph.plan_cache = ON;
PRAGMA graph.plan_cache_size = 100;

-- Set maximum traversal depth
PRAGMA graph.max_depth = 15;

-- Enable parallel execution
PRAGMA graph.parallel_execution = ON;
PRAGMA graph.thread_pool_size = 8;

-- Set memory pool size (in MB)
PRAGMA graph.memory_pool_size = 256;

-- Enable compression
PRAGMA graph.compression = ON;
PRAGMA graph.compression_level = 6;
```

### Recommended Configurations

#### Small Graphs (< 10K nodes)
```sql
PRAGMA graph.cache_size = 2000;
PRAGMA graph.thread_pool_size = 2;
PRAGMA graph.memory_pool_size = 64;
```

#### Medium Graphs (10K - 1M nodes)
```sql
PRAGMA graph.cache_size = 10000;
PRAGMA graph.thread_pool_size = 4;
PRAGMA graph.memory_pool_size = 256;
PRAGMA graph.compression = ON;
```

#### Large Graphs (> 1M nodes)
```sql
PRAGMA graph.cache_size = 50000;
PRAGMA graph.thread_pool_size = 8;
PRAGMA graph.memory_pool_size = 1024;
PRAGMA graph.compression = ON;
PRAGMA graph.compression_level = 9;
```

## Indexing Strategies

### Property Indexes

Create indexes on frequently queried properties:

```sql
-- Index on node properties
CREATE INDEX idx_person_name ON my_graph(
    json_extract(properties, '$.name')
) WHERE edge_id IS NULL;

-- Index on edge properties
CREATE INDEX idx_edge_weight ON my_graph(weight) 
WHERE edge_id IS NOT NULL;

-- Composite index for complex queries
CREATE INDEX idx_person_age_city ON my_graph(
    json_extract(properties, '$.age'),
    json_extract(properties, '$.city')
) WHERE edge_id IS NULL;
```

### Label Indexes

Optimize label-based queries:

```sql
-- Index on primary label
CREATE INDEX idx_node_label ON my_graph(
    json_extract(labels, '$[0]')
) WHERE edge_id IS NULL;

-- Index for multi-label queries
CREATE INDEX idx_node_labels ON my_graph(labels) 
WHERE edge_id IS NULL;
```

### Relationship Indexes

Speed up traversals:

```sql
-- Index on edge type
CREATE INDEX idx_edge_type ON my_graph(edge_type) 
WHERE edge_id IS NOT NULL;

-- Index for outgoing edges
CREATE INDEX idx_from_node ON my_graph(from_id, edge_type) 
WHERE edge_id IS NOT NULL;

-- Index for incoming edges
CREATE INDEX idx_to_node ON my_graph(to_id, edge_type) 
WHERE edge_id IS NOT NULL;
```

### Index Maintenance

```sql
-- Analyze table statistics
ANALYZE my_graph;

-- Rebuild indexes
REINDEX my_graph;

-- View index usage
SELECT * FROM graph_index_stats();
```

## Query Optimization

### Cypher Query Optimization

#### Use Specific Patterns
```cypher
-- Good: Specific pattern
MATCH (p:Person {name: 'John'})-[:KNOWS]->(friend:Person)
RETURN friend;

-- Bad: Generic pattern
MATCH (p)-[r]->(friend)
WHERE p.name = 'John' AND type(r) = 'KNOWS'
RETURN friend;
```

#### Filter Early
```cypher
-- Good: Filter in pattern
MATCH (p:Person {age: 30})-[:WORKS_AT]->(c:Company)
RETURN c;

-- Bad: Filter in WHERE
MATCH (p:Person)-[:WORKS_AT]->(c:Company)
WHERE p.age = 30
RETURN c;
```

#### Use Index Hints
```cypher
-- Force index usage
MATCH (p:Person)
USING INDEX p:Person(name)
WHERE p.name = 'John'
RETURN p;
```

### SQL Query Optimization

#### Use Prepared Statements
```c
sqlite3_stmt *stmt;
sqlite3_prepare_v2(db, 
    "SELECT * FROM graph_dijkstra(?, ?, ?)", 
    -1, &stmt, NULL);

// Reuse statement multiple times
for (int i = 0; i < n; i++) {
    sqlite3_bind_int64(stmt, 1, start_id);
    sqlite3_bind_int64(stmt, 2, end_id);
    sqlite3_step(stmt);
    sqlite3_reset(stmt);
}
sqlite3_finalize(stmt);
```

#### Batch Operations
```sql
-- Good: Batch insert
BEGIN TRANSACTION;
INSERT INTO my_graph (node_id, labels, properties) VALUES
    (1, '["Person"]', '{"name": "User1"}'),
    (2, '["Person"]', '{"name": "User2"}'),
    -- ... 1000s more
    (10000, '["Person"]', '{"name": "User10000"}');
COMMIT;

-- Bad: Individual inserts
INSERT INTO my_graph VALUES (...);
INSERT INTO my_graph VALUES (...);
-- ... repeated
```

### Query Plan Analysis

```sql
-- Analyze query plan
EXPLAIN QUERY PLAN
MATCH (p:Person)-[:KNOWS*1..3]->(friend:Person)
WHERE p.name = 'John'
RETURN friend;

-- View execution statistics
.stats on
MATCH (p:Person) RETURN COUNT(p);
```

## Memory Management

### Memory Pool Configuration

```sql
-- Configure memory pools
PRAGMA graph.memory_pool_size = 512;  -- MB
PRAGMA graph.tuple_pool_size = 10000; -- Number of tuples
PRAGMA graph.string_pool_size = 100;  -- MB for strings
```

### Cache Management

```sql
-- Configure caches
PRAGMA graph.node_cache_size = 5000;
PRAGMA graph.edge_cache_size = 10000;
PRAGMA graph.path_cache_size = 1000;

-- Monitor cache performance
SELECT * FROM graph_cache_stats();

-- Clear caches
SELECT graph_clear_cache('all');
```

### Memory-Efficient Queries

```cypher
-- Use LIMIT to control result size
MATCH (p:Person)-[:KNOWS]->(friend)
RETURN friend
LIMIT 100;

-- Stream results instead of collecting
MATCH (p:Person)
WITH p
ORDER BY p.created_at
RETURN p
SKIP 0 LIMIT 1000;  -- Paginate results
```

## Parallel Processing

### Configuring Parallelism

```sql
-- Set thread pool size based on CPU cores
PRAGMA graph.thread_pool_size = 8;

-- Configure work stealing
PRAGMA graph.work_stealing = ON;
PRAGMA graph.task_granularity = 1000;  -- Nodes per task

-- Set parallel thresholds
PRAGMA graph.parallel_threshold = 10000;  -- Min nodes for parallel
```

### Parallel Query Patterns

```sql
-- Parallel graph algorithms
SELECT * FROM graph_pagerank('my_graph') 
WITH parallel_iterations = 4;

-- Parallel pattern matching
SELECT * FROM graph_parallel_match('my_graph', 
    'MATCH (p:Person)-[:KNOWS]->(f) RETURN p, f'
);
```

### Thread Safety

```c
// Use thread-local storage for performance
__thread GraphContext *local_ctx = NULL;

// Minimize lock contention
pthread_mutex_t *locks = create_lock_array(1024);
int lock_idx = node_id % 1024;
pthread_mutex_lock(&locks[lock_idx]);
// ... critical section
pthread_mutex_unlock(&locks[lock_idx]);
```

## Bulk Operations

### CSV Import

```sql
-- Configure bulk loader
PRAGMA graph.bulk_buffer_size = 1048576;  -- 1MB
PRAGMA graph.bulk_commit_interval = 10000;  -- Commit every 10K rows

-- Import nodes
SELECT graph_import_nodes('my_graph', 
    'nodes.csv',
    'node_id,labels,properties'
);

-- Import edges
SELECT graph_import_edges('my_graph',
    'edges.csv', 
    'edge_id,from_id,to_id,type,weight,properties'
);
```

### Optimized Bulk Loading

```c
// Disable indexes during bulk load
sqlite3_exec(db, "PRAGMA graph.defer_indexes = ON", NULL, NULL, NULL);

// Load data
graph_bulk_load(graph, "data.csv");

// Rebuild indexes
sqlite3_exec(db, "PRAGMA graph.rebuild_indexes = ON", NULL, NULL, NULL);
```

### Batch Updates

```sql
-- Batch property updates
UPDATE my_graph 
SET properties = json_set(properties, '$.processed', 'true')
WHERE node_id IN (
    SELECT node_id FROM my_graph 
    WHERE json_extract(properties, '$.status') = 'pending'
    LIMIT 1000
);
```

## Monitoring and Profiling

### Performance Monitoring

```sql
-- Enable detailed monitoring
PRAGMA graph.monitor_level = 'detailed';

-- Query performance metrics
SELECT * FROM graph_query_stats 
ORDER BY total_time DESC 
LIMIT 10;

-- Monitor resource usage
SELECT * FROM graph_resource_usage();
```

### Query Profiling

```sql
-- Enable query profiler
PRAGMA graph.profiler = ON;

-- Run query
MATCH (p:Person)-[:KNOWS*1..3]->(friend)
WHERE p.name = 'John'
RETURN friend;

-- View profile
SELECT * FROM graph_last_query_profile();
```

### Performance Dashboards

```sql
-- Create monitoring view
CREATE VIEW graph_performance_dashboard AS
SELECT 
    'Cache Hit Rate' as metric,
    ROUND(cache_hits * 100.0 / (cache_hits + cache_misses), 2) as value
FROM graph_cache_stats()
UNION ALL
SELECT 
    'Avg Query Time (ms)',
    ROUND(AVG(execution_time), 2)
FROM graph_query_stats()
UNION ALL
SELECT 
    'Memory Usage (MB)',
    ROUND(memory_used / 1048576.0, 2)
FROM graph_resource_usage();
```

## Troubleshooting

### Common Performance Issues

#### Slow Traversals
```sql
-- Check traversal depth
SELECT MAX(json_array_length(path)) as max_depth
FROM graph_path('my_graph', 1, NULL);

-- Limit depth
MATCH (p:Person)-[:KNOWS*1..2]->(friend)
RETURN friend;
```

#### High Memory Usage
```sql
-- Check memory consumers
SELECT * FROM graph_memory_breakdown();

-- Reduce cache sizes
PRAGMA graph.cache_size = 1000;
PRAGMA graph.memory_pool_size = 128;
```

#### Lock Contention
```sql
-- Check lock statistics
SELECT * FROM graph_lock_stats();

-- Increase lock granularity
PRAGMA graph.lock_granularity = 'fine';
```

### Diagnostic Queries

```sql
-- Find expensive queries
SELECT 
    query,
    execution_count,
    total_time,
    avg_time,
    max_time
FROM graph_query_stats()
WHERE avg_time > 100  -- ms
ORDER BY total_time DESC;

-- Identify missing indexes
SELECT 
    'CREATE INDEX idx_' || 
    REPLACE(property_path, '$.', '') || 
    ' ON my_graph(' || property_path || ')' as suggested_index,
    access_count,
    avg_scan_rows
FROM graph_missing_indexes()
WHERE avg_scan_rows > 1000
ORDER BY access_count DESC;
```

## Best Practices

### Design Patterns

1. **Denormalize for Performance**
   ```cypher
   -- Store computed values
   CREATE (p:Person {
       name: 'John',
       friend_count: 150,  -- Denormalized
       last_updated: timestamp()
   });
   ```

2. **Use Appropriate Data Types**
   ```sql
   -- Use integers for IDs
   CREATE (p:Person {id: 12345});  -- Not "12345"
   
   -- Use proper date formats
   CREATE (e:Event {date: '2024-01-01'});  -- ISO format
   ```

3. **Partition Large Graphs**
   ```sql
   -- Create separate graphs by time period
   CREATE VIRTUAL TABLE graph_2024_q1 USING graph();
   CREATE VIRTUAL TABLE graph_2024_q2 USING graph();
   ```

### Query Patterns

1. **Avoid Cartesian Products**
   ```cypher
   -- Bad: Cartesian product
   MATCH (a:Person), (b:Person)
   WHERE a.name = 'John' AND b.name = 'Jane';
   
   -- Good: Direct relationship
   MATCH (a:Person {name: 'John'}), (b:Person {name: 'Jane'})
   CREATE (a)-[:KNOWS]->(b);
   ```

2. **Use WITH for Pipeline Processing**
   ```cypher
   MATCH (p:Person)-[:KNOWS]->(friend)
   WITH p, COUNT(friend) as friend_count
   WHERE friend_count > 10
   RETURN p.name, friend_count
   ORDER BY friend_count DESC;
   ```

3. **Optimize Variable-Length Paths**
   ```cypher
   -- Specify min and max lengths
   MATCH path = (p:Person)-[:KNOWS*2..4]->(target)
   WHERE p.name = 'John'
   RETURN path
   LIMIT 10;
   ```

### Maintenance Tasks

1. **Regular Statistics Updates**
   ```sql
   -- Schedule daily
   ANALYZE my_graph;
   ```

2. **Index Maintenance**
   ```sql
   -- Weekly maintenance
   REINDEX my_graph;
   VACUUM my_graph;
   ```

3. **Performance Baseline**
   ```sql
   -- Create baseline
   CREATE TABLE performance_baseline AS
   SELECT * FROM graph_performance_stats();
   
   -- Compare current performance
   SELECT 
       current.metric,
       baseline.value as baseline_value,
       current.value as current_value,
       ROUND((current.value - baseline.value) * 100.0 / baseline.value, 2) as pct_change
   FROM graph_performance_stats() current
   JOIN performance_baseline baseline USING (metric);
   ```

## Performance Targets

### Expected Performance Metrics

| Operation | 10K Nodes | 100K Nodes | 1M Nodes |
|-----------|-----------|------------|----------|
| Single Node Lookup | < 1ms | < 1ms | < 2ms |
| 1-hop Traversal | < 5ms | < 10ms | < 50ms |
| 3-hop Traversal | < 50ms | < 200ms | < 2s |
| Shortest Path | < 100ms | < 1s | < 10s |
| PageRank (full) | < 1s | < 10s | < 2min |
| Bulk Load | > 10K/s | > 8K/s | > 5K/s |

### Optimization Checklist

- [ ] Appropriate indexes created
- [ ] Cache sizes tuned for workload
- [ ] Parallel execution enabled
- [ ] Query plans analyzed and optimized
- [ ] Memory pools configured
- [ ] Statistics regularly updated
- [ ] Monitoring enabled
- [ ] Performance baseline established

By following this guide, you should achieve optimal performance from the SQLite Graph Extension for your specific use case.