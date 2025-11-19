# TCK Create2 - Creating Relationships

**Feature:** Create2 - Creating relationships
**Total Scenarios:** 24
**Status:** ✅ COMPLETE (24/24 passing, 100%)

---

## Implementation Summary

**Date:** 2025-11-16 (Updated - Final)
**Result:** 24/24 passing (100%), 0 failing

**Note:** ALL TESTS PASSING! Error validation implemented for undirected relationships, bidirectional relationships, and relationship variable rebinding.

### ✅ Passing Tests (24/24 - 100%)
- Test 1: Create two nodes and a single relationship in a single pattern
- Test 2: Create two nodes and a single relationship in separate patterns
- Test 3: Create two nodes and a single relationship in separate clauses
- Test 4: Create two nodes and a single relationship in the reverse direction
- Test 5: Create a single relationship between two existing nodes
- Test 6: Create a single relationship between two existing nodes in the reverse direction
- Test 7: Create a single node and a single self loop in a single pattern
- Test 8: Create a single node and a single self loop in separate patterns
- Test 9: Create a single node and a single self loop in separate clauses
- Test 10: Create a single self loop on an existing node
- Test 11: Create a single relationship and an end node on an existing starting node
- Test 12: Create a single relationship and a starting node on an existing end node
- Test 13: Create a single relationship with a property
- Test 14: Create a single relationship with a property and return it
- Test 15: Create a single relationship with two properties
- Test 16: Create a single relationship with two properties and return them
- Test 17: Create a single relationship with null properties
- Test 18: Fail when creating a relationship without a type
- Test 19: Fail when creating a relationship without a direction ✨ **NEWLY FIXED 2025-11-16**
- Test 20: Fail when creating a relationship with two directions ✨ **NEWLY FIXED 2025-11-16**
- Test 21: Fail when creating a relationship with more than one type
- Test 22: Fail when creating a variable-length relationship
- Test 23: Fail when creating a relationship variable already bound ✨ **NEWLY FIXED 2025-11-16**
- Test 24: Fail when creating a relationship using undefined variable in pattern

### ✅ Previously Failing - Now Fixed (3 tests)

**Error Validation (3 tests)** - Tests 19, 20, 23
- Test 19: Undirected relationship validation - FIXED
- Test 20: Bidirectional relationship validation - FIXED
- Test 23: Relationship variable rebinding validation - FIXED

### Key Findings (Updated 2025-11-16 - FINAL)

1. **Basic relationship creation works**: `CREATE ()-[:R]->()` ✅
2. **Properties on relationships work**: Including null handling ✅
3. **Error detection COMPLETE**: All validation tests passing ✅ **NEWLY FIXED**
4. **Reverse arrows IMPLEMENTED**: `<-` syntax now fully working in CREATE ✅
5. **MATCH integration works**: Can create relationships on matched nodes in both directions ✅
6. **Variable binding FULLY IMPLEMENTED**: Works in both single patterns and pattern lists ✅
7. **Self-loops work**: Both within single pattern and across patterns ✅
8. **Variable binding across clauses works**: Tests 3, 9 passing ✅
9. **Direction validation implemented**: Undirected and bidirectional relationships properly rejected ✅ **NEWLY FIXED**
10. **Relationship variable rebinding prevented**: Cannot reuse relationship variables from MATCH ✅ **NEWLY FIXED**

---

## Scenarios

### ✅ [1] Create two nodes and a single relationship in a single pattern
**Status:** PASSING
- Query: `CREATE ()-[:R]->()`
- Expected: 2 nodes, 1 relationship
- Result: ✅ Works correctly

### ✅ [2] Create two nodes and a single relationship in separate patterns
**Status:** PASSING ✨ **FIXED 2025-01-16**
- Query: `CREATE (a), (b), (a)-[:R]->(b)`
- Expected: 2 nodes, 1 relationship
- Result: ✅ Works correctly
- Fix: Implemented variable binding map in PATTERN_LIST iterator to track node variables across patterns

### ✅ [3] Create two nodes and a single relationship in separate clauses
**Status:** PASSING ✨ **FIXED 2025-01-16**
- Query: `CREATE (a) CREATE (b) CREATE (a)-[:R]->(b)`
- Expected: 2 nodes, 1 relationship
- Result: ✅ Works correctly
- Note: Variable binding across CREATE clauses was already working from previous implementation

### ✅ [4] Create two nodes and a single relationship in the reverse direction
**Status:** PASSING ✨ **NEWLY FIXED 2025-01-16**
- Query: `CREATE (:A)<-[:R]-(:B)`
- Expected: 2 nodes, 1 relationship
- Result: ✅ Works correctly
- Fix: Implemented reverse arrow parsing and execution support

### ✅ [5] Create a single relationship between two existing nodes
**Status:** PASSING ✨ **FIXED 2025-01-16**
- Query: `MATCH (x:X), (y:Y) CREATE (x)-[:R]->(y)`
- Expected: Relationship created between existing nodes
- Result: ✅ Works correctly
- Note: MATCH variable binding into CREATE works for forward arrows!

### ✅ [6] Create a single relationship between two existing nodes in the reverse direction
**Status:** PASSING ✨ **NEWLY FIXED 2025-01-16**
- Query: `MATCH (x:X), (y:Y) CREATE (x)<-[:R]-(y)`
- Expected: Relationship created
- Result: ✅ Works correctly
- Fix: Reverse arrow support enables MATCH + reverse arrow combination

### ✅ [7] Create a single node and a single self loop in a single pattern
**Status:** PASSING ✨ **FIXED 2025-01-16**
- Query: `CREATE (root)-[:LINK]->(root)`
- Expected: 1 node, 1 self-loop
- Result: ✅ Works correctly
- Fix: Extended variable binding map to single PATTERN (not just PATTERN_LIST)

### ✅ [8] Create a single node and a single self loop in separate patterns
**Status:** PASSING ✨ **FIXED 2025-01-16**
- Query: `CREATE (root), (root)-[:LINK]->(root)`
- Expected: 1 node, 1 self-loop
- Result: ✅ Works correctly
- Fix: Variable binding map allows reusing the same node variable in different patterns

### ✅ [9] Create a single node and a single self loop in separate clauses
**Status:** PASSING ✨ **FIXED 2025-01-16**
- Query: `CREATE (root) CREATE (root)-[:LINK]->(root)`
- Expected: 1 node, 1 self-loop
- Result: ✅ Works correctly
- Note: Variable binding across CREATE clauses was already working from previous implementation

### ✅ [10] Create a single self loop on an existing node
**Status:** PASSING ✨ **FIXED 2025-01-16**
- Query: `MATCH (root:Root) CREATE (root)-[:LINK]->(root)`
- Expected: Self-loop on existing node
- Result: ✅ Works correctly
- Fix: Driving iterator pattern - CREATE iterates over MATCH results

### ✅ [11] Create a single relationship and an end node on an existing starting node
**Status:** PASSING ✨ **FIXED 2025-01-16**
- Query: `MATCH (x:Begin) CREATE (x)-[:TYPE]->(:End)`
- Expected: 1 new node, 1 relationship
- Result: ✅ Works correctly
- Fix: Driving iterator pattern - MATCH variables bound before CREATE executes

### ✅ [12] Create a single relationship and a starting node on an existing end node
**Status:** PASSING ✨ **FIXED 2025-01-16**
- Query: `MATCH (x:End) CREATE (:Begin)-[:TYPE]->(x)`
- Expected: 1 new node, 1 relationship
- Result: ✅ Works correctly
- Fix: Driving iterator pattern enables proper variable binding

### ✅ [13] Create a single relationship with a property
**Status:** PASSING
- Query: `CREATE ()-[:R {num: 42}]->()`
- Expected: Relationship with property
- Result: ✅ Works correctly

### ✅ [14] Create a single relationship with a property and return it
**Status:** PASSING
- Query: `CREATE ()-[r:R {num: 42}]->() RETURN r.num AS num`
- Expected: Returns num=42
- Result: ✅ Works correctly

### ✅ [15] Create a single relationship with two properties
**Status:** PASSING
- Query: `CREATE ()-[:R {id: 12, name: 'foo'}]->()`
- Expected: Relationship with 2 properties
- Result: ✅ Works correctly

### ✅ [16] Create a single relationship with two properties and return them
**Status:** PASSING
- Query: `CREATE ()-[r:R {id: 12, name: 'foo'}]->() RETURN r.id, r.name`
- Expected: Returns both properties
- Result: ✅ Works correctly

### ✅ [17] Create a single relationship with null properties
**Status:** PASSING
- Query: `CREATE ()-[r:X {id: 12, name: null}]->() RETURN r.id, r.name`
- Expected: Only id property stored
- Result: ✅ Works correctly

### ✅ [18] Fail when creating a relationship without a type
**Status:** PASSING
- Query: `CREATE ()-->()`
- Expected: SyntaxError: NoSingleRelationshipType
- Result: ✅ Correctly rejected

### ✅ [19] Fail when creating a relationship without a direction
**Status:** PASSING ✨ **NEWLY FIXED 2025-11-16**
- Query: `CREATE (a)-[:FOO]-(b)`
- Expected: SyntaxError: RequiresDirectedRelationship
- Result: ✅ Correctly rejected
- Fix: Added direction validation in `validateCreatePattern` - undirected relationships (iDirection == 0) now properly rejected

### ✅ [20] Fail when creating a relationship with two directions
**Status:** PASSING ✨ **NEWLY FIXED 2025-11-16**
- Query: `CREATE (a)<-[:FOO]->(b)`
- Expected: SyntaxError: RequiresDirectedRelationship
- Result: ✅ Correctly rejected
- Fix: Bidirectional patterns (both `<-` and `->`) result in iDirection == 0, caught by same validation as Test 19

### ✅ [21] Fail when creating a relationship with more than one type
**Status:** PASSING
- Query: `CREATE ()-[:A|:B]->()`
- Expected: SyntaxError: NoSingleRelationshipType
- Result: ✅ Correctly rejected

### ✅ [22] Fail when creating a variable-length relationship
**Status:** PASSING
- Query: `CREATE ()-[:FOO*2]->()`
- Expected: SyntaxError: CreatingVarLength
- Result: ✅ Correctly rejected

### ✅ [23] Fail when creating a relationship that is already bound
**Status:** PASSING ✨ **NEWLY FIXED 2025-11-16**
- Query: `MATCH ()-[r]->() CREATE ()-[r]->()`
- Expected: SyntaxError: VariableAlreadyBound
- Result: ✅ Correctly rejected
- Fix: Added relationship variable rebinding check in `validateCreatePattern` - validates that relationship variables are not already bound from previous clauses

### ✅ [24] Fail when creating a relationship using undefined variable in pattern
**Status:** PASSING
- Query: `MATCH (a) CREATE (a)-[:KNOWS]->(b {name: missing})`
- Expected: SyntaxError: UndefinedVariable
- Result: ✅ Correctly rejected
- Note: Undefined variable detection already implemented

---

## Implementation Plan

### Phase 1: MATCH Variable Binding in CREATE
**Priority:** CRITICAL
**Files:** `src/cypher/cypher-write.c`, variable binding logic

- [ ] Fix MATCH-CREATE variable passing
- [ ] Ensure matched nodes can be referenced in CREATE
- [ ] Test with all MATCH+CREATE scenarios
- [ ] Verify relationship creation on existing nodes

### Phase 2: Reverse Arrow Support
**Priority:** HIGH
**Files:** `src/cypher/cypher-parser.c`

- [ ] Add `<-` parsing support in CREATE patterns
- [ ] Ensure correct direction handling
- [ ] Test bidirectional scenarios
- [ ] Update relationship direction logic

### Phase 3: Self-Loop and Variable Reuse
**Priority:** HIGH
**Files:** `src/cypher/cypher-write.c`

- [ ] Detect when same variable used in pattern
- [ ] Create self-loop instead of duplicate node
- [ ] Handle `CREATE (a)-[:R]->(a)` correctly
- [ ] Test all self-loop scenarios

### Phase 4: Multiple Patterns/Clauses
**Priority:** HIGH
**Files:** Parser and executor

- [ ] Support multiple CREATE patterns in one statement
- [ ] Support variable binding across CREATE clauses
- [ ] Ensure proper scoping
- [ ] Test complex multi-pattern scenarios

### Phase 5: Error Validation
**Priority:** MEDIUM
**Files:** Semantic validator

- [ ] Add direction requirement validation
- [ ] Add variable rebinding checks
- [ ] Add undefined variable detection
- [ ] Return appropriate error codes

---

## Next Steps

1. **Critical:** Fix MATCH-CREATE variable binding (Tests 5, 6, 10-12)
2. **High:** Add reverse arrow support (Tests 4, 6)
3. **High:** Fix self-loop creation (Tests 7-10)
4. **Medium:** Implement error validation (Tests 19, 20, 23, 24)

---

## Notes

- Core relationship creation with properties works well
- Variable binding across patterns now working (Tests 2, 8 fixed)
- MATCH+CREATE integration partially working (Test 5 passes, others need investigation)
- Reverse arrow syntax needs parser work
- Self-loop detection within single pattern still needs work (Test 7)

---

## Recent Implementation (2025-01-16)

### Variable Binding Map for PATTERN_LIST

**Problem:** `CREATE (a), (b), (a)-[:R]->(b)` was creating 4 nodes instead of 2 because each pattern created new nodes without checking if the variable was already used.

**Solution:** Implemented a variable binding map in `src/cypher/cypher-iterators.c`:

```c
typedef struct VarBinding {
    const char *zVarName;
    sqlite3_int64 iNodeId;
} VarBinding;
VarBinding aVarBindings[50]; /* Support up to 50 unique variables */
int nVarBindings = 0;
```

**Implementation Details:**
1. Extract variable name from IDENTIFIER child of NODE_PATTERN AST node
2. Before creating a node, check if variable exists in binding map
3. If found, reuse existing node_id instead of creating new node
4. If not found, create node and add to binding map
5. Binding map persists across all patterns in a single PATTERN_LIST

**Files Modified:**
- `src/cypher/cypher-iterators.c` (lines 2550-2690)

**Tests Fixed:**
- Create2_02: `CREATE (a), (b), (a)-[:R]->(b)` - 2 nodes, 1 relationship ✅
- Create2_08: `CREATE (root), (root)-[:LINK]->(root)` - 1 node, 1 self-loop ✅

**Limitations:**
- Only works within a single CREATE clause
- Multiple CREATE clauses don't share variable bindings (Tests 3, 9 still fail)

---

### Self-Loop Support in Single Patterns

**Problem:** `CREATE (root)-[:LINK]->(root)` was creating 2 nodes (1 and 2) with edge 1->2 instead of creating 1 node with self-loop edge 1->1.

**Solution:** Extended the variable binding map to also work for single PATTERN (not just PATTERN_LIST).

**Implementation:**
- Applied the same variable binding logic from PATTERN_LIST to single PATTERN handling
- Before creating each node in a pattern, check if the variable name already exists
- If found, reuse the existing node ID instead of creating a new node
- This allows self-loops like `(root)-[:LINK]->(root)` to work correctly

**Files Modified:**
- `src/cypher/cypher-iterators.c` (lines 2768-2900)

**Tests Fixed:**
- Create2_07: `CREATE (root)-[:LINK]->(root)` - 1 node with self-loop ✅

**Impact:**
- Create2 improved from 11/24 (46%) to 12/24 (50%) passing

---

### Driving Iterator Pattern for MATCH+CREATE

**Problem:** `MATCH (x) CREATE (x)->()` was creating new nodes instead of reusing matched nodes. CREATE only executed once regardless of how many rows MATCH returned.

**Root Cause:** CREATE iterator had `bExecuted` flag that prevented it from executing more than once, even when driven by a child iterator (MATCH).

**Solution:** Implemented "eager evaluation" pattern used by Neo4j:
1. **Materialize child results**: When CREATE has a child iterator, fully materialize all results before CREATE executes
2. **Iterate over materialized results**: For each child result row, bind variables to execution context and execute CREATE
3. **Prevent feedback loops**: Materialization prevents CREATE from seeing its own writes

**Architecture Changes:**

Modified `WriteIteratorData` struct:
```c
typedef struct WriteIteratorData {
  sqlite3_stmt *pInsertStmt;
  int bExecuted;                    // Still used for non-driven CREATE
  sqlite3_int64 iCreatedId;
  CypherResult *apChildResults;     // NEW: Materialized child results
  int nChildResults;                // NEW: Count of materialized results
  int iCurrentChild;                // NEW: Current position in iteration
} WriteIteratorData;
```

Modified `createIteratorOpen`:
- Detects child iterator presence
- Materializes all child results into array
- Closes child iterator (prevents seeing CREATE's writes)

Modified `createIteratorNext`:
- If materialized results exist, iterate over them
- For each result, bind all columns to execution context via `executionContextBind`
- Execute CREATE pattern with those bindings
- Return one result per call
- Falls back to single execution for standalone CREATE

Modified `createIteratorClose`:
- Cleans up materialized results array
- Frees allocated memory

**Files Modified:**
- `src/cypher/cypher-iterators.c` (lines 2412-3147)

**Tests Fixed:**
- Create2_10: `MATCH (root:Root) CREATE (root)-[:LINK]->(root)` ✅
- Create2_11: `MATCH (x:Begin) CREATE (x)-[:TYPE]->(:End)` ✅
- Create2_12: `MATCH (x:End) CREATE (:Begin)-[:TYPE]->(x)` ✅

**Architectural Benefits:**
1. **Specification Compliance**: Matches Cypher/Neo4j eager evaluation semantics
2. **Prevents Infinite Loops**: MATCH can't see CREATE's writes
3. **Deterministic Behavior**: Results independent of execution order
4. **Extensible Pattern**: Can be applied to other write operations (DELETE, MERGE)

**Impact:**
- Create2 improved from 12/24 (50%) to 16/24 (66.7%) passing
- +4 tests fixed with single architectural change

---

### Reverse Arrow Support for CREATE (2025-01-16 Evening)

**Problem:** Queries with reverse arrows like `CREATE (:A)<-[:R]-(:B)` and `MATCH (x:X), (y:Y) CREATE (x)<-[:R]-(y)` were failing with parser errors: "Expected node pattern after relationship near '-'".

**Root Cause Analysis:**
1. **Parser Issue**: After consuming `]-` in a reverse arrow pattern `<-[:R]-`, the code checked for ARROW_RIGHT but didn't consume the trailing dash when iDirection == -1
2. **Execution Issue**: The relationship creation code always used `aiNodeIds[nodeIndex]` as source and `aiNodeIds[nodeIndex + 1]` as target, ignoring the direction flag

**Solution Implemented:**

**1. Parser Fix** (`src/cypher/cypher-parser.c` line 842):
```c
// Before: Only consumed trailing dash for undirected (iDirection == 0)
} else if (iDirection == 0) {
    parserConsumeToken(pLexer, CYPHER_TOK_DASH);
}

// After: Also consume for reverse arrow (iDirection == -1)
} else if (iDirection == 0 || iDirection == -1) {
    /* Consume trailing dash if no right arrow (for undirected or left-arrow patterns) */
    parserConsumeToken(pLexer, CYPHER_TOK_DASH);
}
```

**2. Execution Fix** (`src/cypher/cypher-iterators.c` lines 2832, 3073):
Added direction handling in both PATTERN and PATTERN_LIST relationship creation:
```c
/* Handle relationship direction: iFlags = 1 (->), -1 (<-), 0 (undirected) */
int iDirection = pRel->iFlags;
if (iDirection == -1) {
  /* Reverse arrow: source and target are swapped */
  iSource = aiNodeIds[nodeIndex + 1];
  iTarget = aiNodeIds[nodeIndex];
} else {
  /* Forward arrow or undirected */
  iSource = aiNodeIds[nodeIndex];
  iTarget = aiNodeIds[nodeIndex + 1];
}
```

**Files Modified:**
- `src/cypher/cypher-parser.c` (line 842-844)
- `src/cypher/cypher-iterators.c` (lines 2832-2842, 3072-3082)

**Tests Fixed:**
- Create2_04: `CREATE (:A)<-[:R]-(:B)` - 2 nodes, 1 relationship in reverse direction ✅
- Create2_06: `MATCH (x:X), (y:Y) CREATE (x)<-[:R]-(y)` - Create reverse relationship on existing nodes ✅
- Create5_02, Create5_03, Create5_04: Additional reverse arrow scenarios ✅

**Impact:**
- Create2 improved from 19/24 (79.2%) to 21/24 (87.5%) passing
- +2 tests fixed directly in Create2, +3 tests fixed in Create5
- Only 3 error validation tests remaining in Create2

---

### Error Validation for CREATE Relationships (2025-11-16 - FINAL)

**Problem:** Three error validation tests were failing because invalid CREATE queries were not being rejected:
1. Test 19: Undirected relationships `CREATE (a)-[:FOO]-(b)` should fail
2. Test 20: Bidirectional relationships `CREATE (a)<-[:FOO]->(b)` should fail
3. Test 23: Relationship variable rebinding `MATCH ()-[r]->() CREATE ()-[r]->()` should fail

**Root Cause:** The `validateCreatePattern` function in the planner only validated node patterns but did not validate relationship patterns.

**Solution Implemented:**

Added relationship pattern validation to `validateCreatePattern` function in `src/cypher/cypher-planner.c`:

```c
} else if( cypherAstIsType(pElement, CYPHER_AST_REL_PATTERN) ) {
  /* Validate relationship patterns */
  int iDirection = pElement->iFlags;

  /* Check 4: Relationship must have a direction in CREATE (not undirected) */
  /* iDirection: 0 = undirected, 1 = right arrow (->), -1 = left arrow (<-) */
  if( iDirection == 0 ) {
    pContext->zErrorMsg = sqlite3_mprintf(
      "SyntaxError: RequiresDirectedRelationship - Only directed relationships are supported in CREATE");
    rc = CYPHER_ERROR_SYNTAX_INVALID_PATTERN;
    goto cleanup;
  }

  /* Check 5: Relationship variable cannot be already bound */
  /* Get relationship variable name if present */
  if( pElement->nChildren > 0 &&
      cypherAstIsType(pElement->apChildren[0], CYPHER_AST_IDENTIFIER) ) {
    const char *zRelVar = cypherAstGetValue(pElement->apChildren[0]);

    /* Check if variable is already bound from previous clauses */
    for( k = 0; k < pContext->nVariables; k++ ) {
      if( pContext->azVariables[k] &&
          strcmp(pContext->azVariables[k], zRelVar) == 0 ) {
        pContext->zErrorMsg = sqlite3_mprintf(
          "SyntaxError: VariableAlreadyBound - Variable `%s` already bound and cannot be redefined in CREATE",
          zRelVar);
        rc = CYPHER_ERROR_SEMANTIC_VARIABLE_REDEFINED;
        goto cleanup;
      }
    }
  }
}
```

**Implementation Details:**

1. **Direction Validation (Tests 19 & 20):**
   - Check `pElement->iFlags` which stores the relationship direction
   - `iDirection == 0` means undirected (catches `-[:FOO]-`) or bidirectional (catches `<-[:FOO]->`)
   - Both cases are invalid for CREATE and now properly rejected
   - Error: `SyntaxError: RequiresDirectedRelationship`

2. **Variable Rebinding Validation (Test 23):**
   - Extract relationship variable from first child (IDENTIFIER node)
   - Check if variable exists in `pContext->nVariables` (bound from previous clauses)
   - If already bound, reject with error
   - Error: `SyntaxError: VariableAlreadyBound`

**Files Modified:**
- `src/cypher/cypher-planner.c` (lines 314-345)

**Tests Fixed:**
- Create2_19: Undirected relationships properly rejected ✅
- Create2_20: Bidirectional relationships properly rejected ✅
- Create2_23: Relationship variable rebinding properly rejected ✅

**Impact:**
- Create2 improved from 21/24 (87.5%) to 24/24 (100%) passing
- +3 tests fixed with comprehensive relationship validation
- **Create2 test suite COMPLETE** - all 24 tests passing
