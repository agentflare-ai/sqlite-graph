# TCK Delete3 - Deleting named paths

**Feature:** Delete3 - Deleting named paths
**Total Scenarios:** 2
**Status:** ❌ NOT IMPLEMENTED (0/2 passing, 0%)

***

## Implementation Summary

**Date:** 2025-01-15
**Result:** 0/2 passing (0%), 2 failing

### ❌ Main Failures (2)

**Category 1: Path Support Missing (2 tests)** - Tests 1-2

* Issue: No support for path expressions or path deletion
* Root cause: Path data type and path operations not implemented

### Key Findings

1. **DELETE iterator doesn't support paths** ❌
2. **Path expressions not implemented** ❌
3. **No path data type in value system** ❌

***

## Scenarios

### ❌ \[1] Detach deleting paths

**Status:** BLOCKED (path expressions not implemented)

* Query: `MATCH p = (:X)-->()-->()-->() DETACH DELETE p`
* Expected: Delete entire path (4 nodes, 3 relationships, 1 label)
* Issue: Path variable `p` and path matching not supported
* Required: Complete path expression implementation
* Expected side effects: `-nodes: 4, -relationships: 3, -labels: 1`

### ❌ \[2] Delete on null path

**Status:** BLOCKED (path expressions and OPTIONAL MATCH not implemented)

* Query: `OPTIONAL MATCH p = ()-->() DETACH DELETE p`
* Expected: No side effects when no path found
* Issue: Both path expressions and OPTIONAL MATCH missing
* Required: Path expressions + OPTIONAL MATCH implementation
* Expected: Empty result with no side effects

***

## Implementation Plan

### Phase 1: Path Data Type Implementation

**Priority:** HIGH
**Files:** `include/cypher.h`, `src/cypher/cypher-value.c`

* [ ] Define `CYPHER_VALUE_PATH` enum value
* [ ] Implement `CypherPath` structure to store node/relationship sequences
* [ ] Add path construction functions
* [ ] Implement path serialization/deserialization
* [ ] Add path memory management

### Phase 2: Path Expression Parsing

**Priority:** HIGH
**Files:** `src/cypher/cypher-parser.c`, `src/cypher/cypher-ast.c`

* [ ] Add path assignment syntax `p = (start)-->(middle)-->(end)`
* [ ] Implement path pattern matching
* [ ] Support variable-length paths `()-->()-->()`
* [ ] Add path variable binding in execution context
* [ ] Handle path concatenation and traversal

### Phase 3: Path Deletion Logic

**Priority:** MEDIUM
**Files:** `src/cypher/cypher-iterators.c`, `src/cypher/cypher-write.c`

* [ ] Extend DELETE iterator to handle `CYPHER_VALUE_PATH`
* [ ] Implement path expansion to individual elements
* [ ] Add DETACH DELETE logic for path elements
* [ ] Handle cyclic path deletion (avoid double-deletion)
* [ ] Implement path element ordering for safe deletion

### Phase 4: Path Matching Integration

**Priority:** MEDIUM
**Files:** `src/cypher/cypher-planner.c`, `src/cypher/cypher-executor.c`

* [ ] Integrate path matching with existing graph traversal
* [ ] Support path constraints and filtering
* [ ] Implement shortest path algorithms
* [ ] Add path result formatting
* [ ] Optimize path query execution

### Phase 5: Testing & Validation

**Priority:** MEDIUM

* [ ] Test basic path creation and matching
* [ ] Verify path deletion side effects
* [ ] Test complex path patterns
* [ ] Validate null path handling
* [ ] Performance testing for large graphs

***

## Technical Architecture

### Path Data Structure

```c
typedef struct CypherPath {
  int nElements;           /* Number of elements in path */
  CypherValue **aElements; /* Alternating nodes and relationships */
  int *aDirections;        /* Direction of each relationship */
} CypherPath;
```

### Path Deletion Algorithm

```
For a path p = (n1)-[r1]->(n2)-[r2]->(n3):
1. DETACH DELETE n1 (deletes n1 and its relationships)
2. DETACH DELETE n2 (deletes n2 and its remaining relationships)
3. DETACH DELETE n3 (deletes n3 and its remaining relationships)
4. Clean up any duplicate relationship deletions
```

### Current Implementation Status

* **Path data type**: Not implemented ❌
* **Path parsing**: Not implemented ❌
* **Path matching**: Not implemented ❌
* **Path deletion**: Not implemented ❌
* **Path serialization**: Not implemented ❌

***

## Dependencies

This feature depends on:

1. **Basic path support** (needed for path expressions)
2. **OPTIONAL MATCH** (needed for null path handling)
3. **Variable-length path matching** (needed for complex patterns)
4. **Graph traversal algorithms** (needed for path finding)

***

## Next Steps

1. **Immediate:** Implement basic path data type
2. **Short-term:** Add path expression parsing
3. **Medium-term:** Integrate with graph matching engine
4. **Long-term:** Optimize path operations for large graphs

***

## Notes

* **Major architectural work required** for path support ⚠️
* **No existing path infrastructure** in codebase ❌
* **Complex feature with many dependencies** ⚠️
* **Both tests completely blocked** by missing functionality ❌

Path deletion represents a significant architectural extension to the Cypher engine. The current implementation has no path support whatsoever, making this one of the most complex DELETE features to implement.
