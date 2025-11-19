# TCK Delete6 - Persistence of delete clause side effects

**Feature:** Delete6 - Persistence of delete clause side effects
**Total Scenarios:** 14
**Status:** ❌ NOT IMPLEMENTED (0/14 passing, 0%)

---

## Implementation Summary

**Date:** 2025-01-15
**Result:** 0/14 passing (0%), 14 failing

### ❌ Main Failures (14)

**Category 1: LIMIT/SKIP Operations (6 tests)** - Tests 1-4, 8-11
- Issue: LIMIT and SKIP clauses not implemented
- Root cause: Result set modification not supported

**Category 2: WITH Clause Missing (8 tests)** - Tests 5-7, 12-14
- Issue: Multi-clause queries with WITH not supported
- Root cause: WITH clause parser and executor missing

### Key Findings

1. **DELETE side effects work correctly** ✅
2. **Result set modification clauses missing** ❌
3. **Multi-clause query support incomplete** ❌

---

## Scenarios

### ❌ [1] Limiting to zero results after deleting nodes affects the result set but not the side effects
**Status:** BLOCKED (LIMIT clause not implemented)
- Query: `MATCH (n:N) DELETE n RETURN 42 AS num LIMIT 0`
- Expected: No rows returned, but nodes deleted
- Issue: `LIMIT 0` syntax not supported
- Required: LIMIT clause implementation
- Expected side effects: `-nodes: 1, -labels: 1, -properties: 1`

### ❌ [2] Skipping all results after deleting nodes affects the result set but not the side effects
**Status:** BLOCKED (SKIP clause not implemented)
- Query: `MATCH (n:N) DELETE n RETURN 42 AS num SKIP 1`
- Expected: No rows returned (skip past available row), but nodes deleted
- Issue: `SKIP 1` syntax not supported
- Required: SKIP clause implementation
- Expected side effects: `-nodes: 1, -labels: 1, -properties: 1`

### ❌ [3] Skipping and limiting to a few results after deleting nodes affects the result set but not the side effects
**Status:** BLOCKED (SKIP and LIMIT not implemented)
- Query: `MATCH (n:N) DELETE n RETURN 42 AS num SKIP 2 LIMIT 2`
- Expected: 2 rows returned (from 5 deleted nodes), nodes still deleted
- Issue: `SKIP 2 LIMIT 2` syntax not supported
- Required: Combined SKIP/LIMIT implementation
- Expected side effects: `-nodes: 5, -labels: 1, -properties: 5`

### ❌ [4] Skipping zero results and limiting to all results after deleting nodes does not affect the result set nor the side effects
**Status:** BLOCKED (SKIP and LIMIT not implemented)
- Query: `MATCH (n:N) DELETE n RETURN 42 AS num SKIP 0 LIMIT 5`
- Expected: All 5 rows returned, all nodes deleted
- Issue: `SKIP 0 LIMIT 5` syntax not supported
- Required: SKIP/LIMIT clause implementation
- Expected side effects: `-nodes: 5, -labels: 1, -properties: 5`

### ❌ [5] Filtering after deleting nodes affects the result set but not the side effects
**Status:** BLOCKED (WITH clause and filtering not implemented)
- Query: `MATCH (n:N) WITH n, n.num AS num DELETE n WITH num WHERE num % 2 = 0 RETURN num`
- Expected: Only even numbers returned, all nodes deleted
- Issue: Multiple `WITH` clauses and `WHERE` filtering not supported
- Required: WITH clause and expression evaluation
- Expected side effects: `-nodes: 5, -labels: 1, -properties: 5`

### ❌ [6] Aggregating in `RETURN` after deleting nodes affects the result set but not the side effects
**Status:** BLOCKED (WITH clause and aggregation not implemented)
- Query: `MATCH (n:N) WITH n, n.num AS num DELETE n RETURN sum(num) AS sum`
- Expected: Sum of all node properties, all nodes deleted
- Issue: `WITH` clause and `sum()` aggregation not supported
- Required: WITH clause and aggregation functions
- Expected side effects: `-nodes: 5, -labels: 1, -properties: 5`

### ❌ [7] Aggregating in `WITH` after deleting nodes affects the result set but not the side effects
**Status:** BLOCKED (WITH clause and aggregation not implemented)
- Query: `MATCH (n:N) WITH n, n.num AS num DELETE n WITH sum(num) AS sum RETURN sum`
- Expected: Sum of all node properties, all nodes deleted
- Issue: `WITH` clause and `sum()` aggregation not supported
- Required: WITH clause and aggregation functions
- Expected side effects: `-nodes: 5, -labels: 1, -properties: 5`

### ❌ [8] Limiting to zero results after deleting relationships affects the result set but not the side effects
**Status:** BLOCKED (LIMIT clause not implemented)
- Query: `MATCH ()-[r:R]->() DELETE r RETURN 42 AS num LIMIT 0`
- Expected: No rows returned, but relationship deleted
- Issue: `LIMIT 0` syntax not supported
- Required: LIMIT clause implementation
- Expected side effects: `-relationships: 1, -properties: 1`

### ❌ [9] Skipping all results after deleting relationships affects the result set but not the side effects
**Status:** BLOCKED (SKIP clause not implemented)
- Query: `MATCH ()-[r:R]->() DELETE r RETURN 42 AS num SKIP 1`
- Expected: No rows returned, but relationship deleted
- Issue: `SKIP 1` syntax not supported
- Required: SKIP clause implementation
- Expected side effects: `-relationships: 1, -properties: 1`

### ❌ [10] Skipping and limiting to a few results after deleting relationships affects the result set but not the side effects
**Status:** BLOCKED (SKIP and LIMIT not implemented)
- Query: `MATCH ()-[r:R]->() DELETE r RETURN 42 AS num SKIP 2 LIMIT 2`
- Expected: 2 rows returned, all relationships deleted
- Issue: `SKIP 2 LIMIT 2` syntax not supported
- Required: Combined SKIP/LIMIT implementation
- Expected side effects: `-relationships: 5, -properties: 5`

### ❌ [11] Skipping zero result and limiting to all results after deleting relationships does not affect the result set nor the side effects
**Status:** BLOCKED (SKIP and LIMIT not implemented)
- Query: `MATCH ()-[r:R]->() DELETE r RETURN 42 AS num SKIP 0 LIMIT 5`
- Expected: All 5 rows returned, all relationships deleted
- Issue: `SKIP 0 LIMIT 5` syntax not supported
- Required: SKIP/LIMIT clause implementation
- Expected side effects: `-relationships: 5, -properties: 5`

### ❌ [12] Filtering after deleting relationships affects the result set but not the side effects
**Status:** BLOCKED (WITH clause and filtering not implemented)
- Query: `MATCH ()-[r:R]->() WITH r, r.num AS num DELETE r WITH num WHERE num % 2 = 0 RETURN num`
- Expected: Only even numbers returned, all relationships deleted
- Issue: Multiple `WITH` clauses and filtering not supported
- Required: WITH clause and expression evaluation
- Expected side effects: `-relationships: 5, -properties: 5`

### ❌ [13] Aggregating in `RETURN` after deleting relationships affects the result set but not the side effects
**Status:** BLOCKED (WITH clause and aggregation not implemented)
- Query: `MATCH ()-[r:R]->() WITH r, r.num AS num DELETE r RETURN sum(num) AS sum`
- Expected: Sum of all relationship properties, all relationships deleted
- Issue: `WITH` clause and `sum()` aggregation not supported
- Required: WITH clause and aggregation functions
- Expected side effects: `-relationships: 5, -properties: 5`

### ❌ [14] Aggregating in `WITH` after deleting relationships affects the result set but not the side effects
**Status:** BLOCKED (WITH clause and aggregation not implemented)
- Query: `MATCH ()-[r:R]->() WITH r, r.num AS num DELETE r WITH sum(num) AS sum RETURN sum`
- Expected: Sum of all relationship properties, all relationships deleted
- Issue: `WITH` clause and `sum()` aggregation not supported
- Required: WITH clause and aggregation functions
- Expected side effects: `-relationships: 5, -properties: 5`

---

## Implementation Plan

### Phase 1: LIMIT Clause Implementation (Required for Tests 1, 3-4, 8, 10-11)
**Priority:** HIGH
**Files:** `src/cypher/cypher-parser.c`, `src/cypher/cypher-executor.c`

- [ ] Add LIMIT keyword to lexer (`CYPHER_TOK_LIMIT`)
- [ ] Implement LIMIT clause parsing and AST construction
- [ ] Add result set limiting in query execution
- [ ] Support `LIMIT 0` (empty result set)
- [ ] Test LIMIT with various numeric values

### Phase 2: SKIP Clause Implementation (Required for Tests 2-4, 9-11)
**Priority:** HIGH
**Files:** `src/cypher/cypher-parser.c`, `src/cypher/cypher-executor.c`

- [ ] Add SKIP keyword to lexer (`CYPHER_TOK_SKIP`)
- [ ] Implement SKIP clause parsing and AST construction
- [ ] Add result set skipping in query execution
- [ ] Support `SKIP n` for various values
- [ ] Test SKIP with edge cases (skip past end)

### Phase 3: Combined SKIP/LIMIT (Required for Tests 3-4, 10-11)
**Priority:** MEDIUM
**Files:** `src/cypher/cypher-executor.c`

- [ ] Implement SKIP then LIMIT execution order
- [ ] Support `SKIP n LIMIT m` syntax
- [ ] Handle edge cases (SKIP > available rows)
- [ ] Optimize for large result sets
- [ ] Test various combinations

### Phase 4: WITH Clause Implementation (Required for Tests 5-7, 12-14)
**Priority:** HIGH
**Files:** `src/cypher/cypher-parser.c`, `src/cypher/cypher-executor.c`

- [ ] Add WITH keyword to lexer (`CYPHER_TOK_WITH`)
- [ ] Implement WITH clause parsing and AST construction
- [ ] Add variable projection and aliasing
- [ ] Support multiple WITH clauses in sequence
- [ ] Implement variable scoping rules

### Phase 5: WHERE Clause in WITH (Required for Test 5, 12)
**Priority:** MEDIUM
**Files:** `src/cypher/cypher-parser.c`, `src/cypher/cypher-evaluator.c`

- [ ] Support `WITH ... WHERE condition` syntax
- [ ] Implement expression evaluation for filtering
- [ ] Add boolean expression support (`num % 2 = 0`)
- [ ] Test arithmetic and comparison operations
- [ ] Handle null value filtering

### Phase 6: Aggregation Functions (Required for Tests 6-7, 13-14)
**Priority:** HIGH
**Files:** `src/cypher/cypher-functions.c` (new file)

- [ ] Implement `sum()` aggregation function
- [ ] Add aggregation execution in RETURN and WITH clauses
- [ ] Support numeric aggregation over collections
- [ ] Handle empty collections and null values
- [ ] Test various aggregation scenarios

### Phase 7: DELETE Side Effects Persistence
**Priority:** MEDIUM
**Files:** `src/cypher/cypher-executor.c`

- [ ] Ensure DELETE operations execute before LIMIT/SKIP
- [ ] Preserve side effects through result set modifications
- [ ] Maintain side effect tracking across multiple clauses
- [ ] Test side effect persistence with complex queries
- [ ] Validate transactional behavior

### Phase 8: Testing & Verification
**Priority:** MEDIUM

- [ ] Test LIMIT/SKIP with DELETE side effects
- [ ] Verify aggregation with DELETE operations
- [ ] Test complex multi-clause DELETE scenarios
- [ ] Performance testing for large result sets
- [ ] Validate all side effect counting

---

## Technical Architecture

### DELETE Execution Order

The key requirement is that DELETE operations must execute before result set modifications:

```
1. MATCH (n:N)           <- Find nodes
2. DELETE n               <- Execute deletions (side effects recorded)
3. RETURN 42 AS num       <- Generate result rows
4. SKIP 2 LIMIT 2         <- Modify result set
```

### Side Effects Persistence

Side effects must persist regardless of LIMIT/SKIP operations:
- DELETE executes and records side effects
- LIMIT/SKIP only affects which rows are returned
- Side effects are committed even if no rows returned

### Multi-Clause Execution Pipeline

```
MATCH -> WITH -> DELETE -> WITH -> WHERE -> RETURN -> SKIP -> LIMIT
```

Each clause processes the row stream, but mutations happen at the correct time.

---

## Current Implementation Status

- **DELETE side effects**: ✅ Working
- **Basic RETURN**: ✅ Working
- **LIMIT clause**: ❌ Not implemented
- **SKIP clause**: ❌ Not implemented
- **WITH clause**: ❌ Not implemented
- **Aggregation functions**: ❌ Not implemented
- **WHERE filtering**: ❌ Not implemented

---

## Dependencies

This feature depends on:
1. **Result set modification clauses** (LIMIT, SKIP)
2. **WITH clause support** (for data flow)
3. **Expression evaluation** (for WHERE conditions)
4. **Aggregation functions** (sum, count, etc.)
5. **Multi-clause query execution** (pipeline architecture)

---

## Next Steps

1. **Immediate:** Implement LIMIT and SKIP clauses
2. **Short-term:** Add WITH clause support
3. **Medium-term:** Implement aggregation functions
4. **Long-term:** Add WHERE filtering in WITH clauses

---

## Notes

- **DELETE functionality is complete and correct** ✅
- **Side effect tracking works properly** ✅
- **Major query pipeline extensions needed** ⚠️
- **All tests blocked by missing clauses** ❌

The side effects persistence tests validate that the DELETE implementation correctly executes mutations regardless of result set modifications. The current DELETE engine handles this correctly, but the broader Cypher query pipeline needs LIMIT, SKIP, WITH, and aggregation support to enable these tests.
