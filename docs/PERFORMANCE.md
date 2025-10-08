# Performance Optimization Guide

**Part of the AgentFlare AI ecosystem**

This document describes the performance optimization features implemented in the SQLite Graph Extension.

## Overview

The SQLite Graph Extension includes comprehensive performance optimizations for handling large-scale graph workloads efficiently. These optimizations span query execution, storage, memory management, and parallel processing.

## Query Performance Features

### 1. Query Plan Caching

The extension implements an LRU cache for compiled query plans to avoid repeated parsing and planning overhead.

```sql
-- Enable plan caching (done automatically)
SELECT graph_plan_cache_stats();
-- Returns: {"hits":150,"misses":20,"entries":50,"memory_bytes":102400,"hit_rate":88.2}

-- Clear the cache if needed
SELECT graph_plan_cache_clear();
```

### 2. Selectivity Estimation

The query planner uses statistical information to estimate the selectivity of patterns and optimize join order:

- Label-based selectivity from index statistics
- Property filter selectivity estimation
- Relationship type frequency analysis

### 3. Parallel Query Execution

Multi-threaded query processing with work-stealing scheduler:

```c
// Parallel pattern matching example
TaskScheduler *scheduler = graphCreateTaskScheduler(4); // 4 threads
graphParallelPatternMatch(pGraph, pattern, &results, &nResults);
```

## Storage Optimizations

### 1. Property Compression

Dictionary encoding for frequently occurring string values:

```sql
-- View compression statistics
SELECT graph_compression_stats();
-- Returns: {"dict_entries":1000,"dict_memory":50000,"saved_bytes":150000,"compression_ratio":300.0}
```

Properties are automatically compressed when:
- String length > 10 characters
- String appears multiple times
- Large properties use zlib compression

### 2. Bulk Loading

High-performance data import with memory mapping:

```sql
-- Bulk load nodes from CSV
SELECT graph_bulk_load('my_graph', '/path/to/nodes.csv', 
  '{"batch_size":10000,"defer_indexing":true,"compress_properties":true}');
```

CSV format:
```csv
id,label,properties
1,Person,{"name":"Alice","age":30}
2,Person,{"name":"Bob","age":25}
```

### 3. Compressed Sparse Row (CSR) Format

Convert graph to CSR format for efficient traversal:

```c
CSRGraph *csr = graphConvertToCSR(pGraph);
// Provides O(1) neighbor access
```

## Memory Management

### 1. Per-Query Memory Pools

Reduce allocation overhead with query-specific memory pools:

```c
QueryMemoryPool *pool = graphCreateMemoryPool(1024 * 1024); // 1MB pool
void *data = graphPoolAlloc(pool, size);
// No need to free individual allocations
graphDestroyMemoryPool(pool); // Frees all at once
```

### 2. Tuple Recycling

Reuse memory for intermediate results during query execution.

## Performance Monitoring

### 1. Query Metrics

Track execution statistics:

```sql
SELECT * FROM cypher_execute_explain('MATCH (n:Person) RETURN n');
-- Returns execution plan with timing and resource usage
```

### 2. LDBC Benchmark Suite

Run industry-standard benchmarks:

```sql
SELECT graph_benchmark(10); -- Scale factor 10
```

Benchmark includes:
- Interactive workload queries
- Graph algorithm performance
- Aggregation operations

### 3. Regression Testing

Automated performance regression detection:

```bash
# Create baseline
./scripts/perf_regression.sh baseline

# Run tests and compare
./scripts/perf_regression.sh test
```

## Configuration Tuning

### Recommended Settings

1. **Memory Allocation**
   ```sql
   -- Set memory pool size
   PRAGMA graph_memory_pool_size = 10485760; -- 10MB
   ```

2. **Cache Sizes**
   ```sql
   -- Set plan cache size
   PRAGMA graph_plan_cache_entries = 200;
   PRAGMA graph_plan_cache_memory = 20971520; -- 20MB
   ```

3. **Parallel Execution**
   ```sql
   -- Set thread count (0 = auto-detect)
   PRAGMA graph_worker_threads = 0;
   ```

## Best Practices

### 1. Index Usage

Create appropriate indexes for your workload:
```sql
SELECT graph_create_label_index('my_graph', 'Person');
SELECT graph_create_property_index('my_graph', 'Person', 'name');
```

### 2. Query Patterns

Write efficient Cypher queries:
```sql
-- Good: Specific label and property filters
MATCH (p:Person {name: "Alice"})-[:KNOWS]->(friend)
RETURN friend;

-- Avoid: Unbounded patterns without filters
MATCH (n)-[*]->(m)
RETURN n, m;
```

### 3. Bulk Operations

Use bulk loading for initial data import:
```sql
-- Disable auto-commit for bulk operations
BEGIN;
-- Load data with deferred indexing
SELECT graph_bulk_load(...);
-- Re-enable indexes
COMMIT;
```

## Performance Characteristics

### Time Complexity

| Operation | Complexity | Notes |
|-----------|------------|-------|
| Node lookup by ID | O(1) | Hash index |
| Label index scan | O(k) | k = nodes with label |
| Pattern matching | O(V + E) | With pruning |
| Shortest path | O((V + E) log V) | Dijkstra's algorithm |
| PageRank | O(k(V + E)) | k = iterations |

### Space Complexity

| Structure | Memory Usage |
|-----------|--------------|
| Node | 48 bytes + properties |
| Edge | 40 bytes + properties |
| Label index | O(n) entries |
| Property index | O(n) entries |
| Dictionary | ~50% compression |

## Troubleshooting

### High Memory Usage

1. Check dictionary size:
   ```sql
   SELECT graph_compression_stats();
   ```

2. Clear plan cache:
   ```sql
   SELECT graph_plan_cache_clear();
   ```

3. Reduce batch sizes for bulk operations

### Slow Queries

1. Check query plan:
   ```sql
   SELECT cypher_explain('your query');
   ```

2. Verify indexes are being used

3. Consider parallel execution for large scans

### Regression Detection

Run performance tests regularly:
```bash
# In CI/CD pipeline
./scripts/perf_regression.sh test
```

This will detect any performance degradation > 10%.