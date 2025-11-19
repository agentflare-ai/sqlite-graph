# TCK Delete4 - Delete clause interoperation with other clauses

**Feature:** Delete4 - Delete clause interoperation with other clauses
**Total Scenarios:** 3
**Status:** 🔄 IN PROGRESS (1/3 passing, 33%)

***

## Implementation Summary

**Date:** 2025-01-15
**Result:** 1/3 passing (33%), 2 failing

### ✅ Passing Tests (1)

* Test 3: Create and delete in same query ✅

### ❌ Main Failures (2)

**Category 1: Advanced Pattern Matching (2 tests)** - Tests 1-2

* Issue: Undirected relationships and variable-length paths not supported
* Root cause: Graph traversal algorithms incomplete

### Key Findings

1. **Basic DELETE + CREATE interoperation works** ✅
2. **Undirected relationship matching missing** ❌
3. **Variable-length path expansion not implemented** ❌

***

## Scenarios

### ❌ \[1] Undirected expand followed by delete and count

**Status:** BLOCKED (undirected relationships not implemented)

* Query: `MATCH (a)-[r]-(b) DELETE r, a, b RETURN count(*) AS c`
* Expected: Delete relationship and both nodes, return count=2
* Issue: Undirected relationship syntax `(a)-[r]-(b)` not supported
* Required: Bidirectional relationship matching
* Expected side effects: `-nodes: 2, -relationships: 1`

### ❌ \[2] Undirected variable length expand followed by delete and count

**Status:** BLOCKED (variable-length paths not implemented)

* Query: `MATCH (a)-[*]-(b) DETACH DELETE a, b RETURN count(*) AS c`
* Expected: Delete all reachable nodes, return count=6
* Issue: Variable-length path syntax `(a)-[*]-(b)` not supported
* Required: Variable-length path expansion algorithms
* Expected side effects: `-nodes: 3, -relationships: 2`

### ✅ \[3] Create and delete in same query

**Status:** PASSING

* Query: `MATCH () CREATE (n) DELETE n`
* Expected: No net side effects (create then delete same node)
* Implementation: ✅ DELETE and CREATE interoperation working
* Result: Empty result with no side effects

***

## Implementation Plan

### Phase 1: Undirected Relationship Support (Required for Test 1)

**Priority:** HIGH
**Files:** `src/cypher/cypher-planner.c`, `src/cypher/cypher-executor.c`

* [ ] Implement undirected relationship matching `(a)-[r]-(b)`
* [ ] Support bidirectional edge traversal
* [ ] Handle relationship direction ambiguity
* [ ] Test with simple undirected patterns
* [ ] Verify correct node binding for both directions

### Phase 2: Variable-Length Path Expansion (Required for Test 2)

**Priority:** HIGH
**Files:** `src/cypher/cypher-planner.c`, `src/cypher/cypher-iterators.c`

* [ ] Add variable-length path syntax `(a)-[*]-(b)`
* [ ] Implement path expansion algorithms
* [ ] Support configurable path lengths `(a)-[*1..3]-(b)`
* [ ] Handle cyclic path detection
* [ ] Optimize for performance on large graphs

### Phase 3: Advanced Graph Traversal

**Priority:** MEDIUM
**Files:** `src/graph/graph-traversal.c` (new file)

* [ ] Implement BFS/DFS traversal algorithms
* [ ] Add path enumeration and deduplication
* [ ] Support shortest path finding
* [ ] Handle disconnected graph components
* [ ] Memory-efficient path storage

### Phase 4: DELETE + Aggregation Integration

**Priority:** MEDIUM
**Files:** `src/cypher/cypher-executor-sql.c`, `src/cypher/cypher-iterators.c`

* [ ] Ensure DELETE operations complete before aggregation
* [ ] Preserve side effects through aggregation pipeline
* [ ] Handle `count(*)` with DELETE side effects
* [ ] Test aggregation ordering with mutations
* [ ] Verify correct result counting

### Phase 5: Testing & Verification

**Priority:** MEDIUM

* [ ] Test undirected relationship patterns
* [ ] Verify variable-length path expansion
* [ ] Test complex DELETE + aggregation scenarios
* [ ] Performance testing for large graph traversals
* [ ] Validate side effect counting accuracy

***

## Technical Architecture

### Undirected Relationship Matching

For a pattern `(a)-[r]-(b)`:

1. Find all relationships where `a` is source OR target
2. For each relationship, bind the opposite node to `b`
3. Handle potential duplicate matches
4. Support both directed and undirected semantics

### Variable-Length Path Expansion

For a pattern `(a)-[*]-(b)`:

1. Perform BFS from node `a`
2. Enumerate all reachable nodes as `b`
3. Track path lengths and avoid cycles
4. Support length constraints `[*1..3]`
5. Deduplicate equivalent paths

### DELETE + Aggregation Pipeline

```
MATCH (a)-[*]-(b)     <- Find all reachable pairs
DETACH DELETE a, b    <- Delete nodes (side effects recorded)
RETURN count(*) AS c  <- Aggregate remaining rows
```

***

## Current Implementation Status

* **Directed relationship matching**: ✅ Working
* **Undirected relationship matching**: ❌ Not implemented
* **Variable-length paths**: ❌ Not implemented
* **DELETE + CREATE interoperation**: ✅ Working
* **DELETE + aggregation**: ✅ Working

***

## Dependencies

This feature depends on:

1. **Graph traversal algorithms** (for undirected and variable-length patterns)
2. **Advanced pattern matching** (for complex relationship patterns)
3. **Aggregation pipeline** (for count operations with DELETE)

***

## Next Steps

1. **Immediate:** Implement undirected relationship matching (unblocks Test 1)
2. **Short-term:** Add variable-length path expansion (unblocks Test 2)
3. **Medium-term:** Optimize traversal performance
4. **Long-term:** Advanced path constraints and filtering

***

## Notes

* **Basic DELETE + CREATE works perfectly** ✅
* **Aggregation integration functional** ✅
* **Graph traversal algorithms needed** for advanced patterns ⚠️
* **Test 3 validates core DELETE functionality** ✅

The interoperation between DELETE and other clauses is fundamentally working. The remaining tests require advanced graph traversal capabilities that would benefit the entire Cypher engine, not just DELETE operations.
