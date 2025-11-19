# TCK Create3 - Interoperation with Other Clauses

**Feature:** Create3 - Interoperation with other clauses
**Total Scenarios:** 13
**Status:** 🟡 PARTIAL (4/13 passing, 31%)

---

## Implementation Summary

**Date:** 2025-01-16 (Updated - WITH clause implemented)
**Result:** 4/13 passing (31%), 9 failing

### ✅ Tests Passing (4) - UP FROM 2!

**Test 1: MATCH-CREATE** - Basic MATCH+CREATE interaction
- Query: `MATCH () CREATE ()`
- Expected: Creates 1 node per match result
- Status: PASSING ✓
- Note: Prerequisites from Create2 are now met!

**Test 2: WITH-CREATE** - WITH clause pass-through ✓ NEW!
- Query: `MATCH () CREATE () WITH * CREATE ()`
- Expected: 6 nodes total (2 initial + 2 from first CREATE + 2 from second CREATE)
- Status: PASSING ✓
- Fixed: WITH clause parser and planner support implemented!

**Test 4: MATCH-CREATE visibility** - Newly-created nodes not visible to preceding MATCH
- Query: `MATCH () CREATE ()`
- Expected: Only finds existing nodes, creates 1 new node
- Status: PASSING ✓

**Test 9: WITH-CREATE: Bound nodes after WITH + WITH** ✓ NEW!
- Query: `CREATE (a) WITH a WITH * CREATE (b) CREATE (a)<-[:T]-(b)`
- Expected: 2 nodes, 1 relationship
- Status: PASSING ✓
- Fixed: Multiple WITH clauses now work correctly!

### ❌ Tests Failing (9) - DOWN FROM 11!

**Category 1: WITH Aliasing Not Implemented (5 tests)** - Tests 5-8, plus Test 3
- Issue: WITH with explicit variable aliasing (WITH n AS a) not implemented
- Root cause: Parser supports WITH *, but not WITH n AS a syntax
- Examples:
  - Test 3: `MATCH () CREATE () WITH * MATCH () CREATE ()` - visibility issue
  - Test 5-8: WITH n AS a, WITH aliasing combinations not working

**Category 2: UNWIND Not Implemented (1 test)** - Test 10
- Issue: UNWIND clause not implemented
- Root cause: Parser doesn't recognize UNWIND keyword

**Category 3: MERGE Interactions (2 tests)** - Tests 11-12
- Issue: MERGE + WITH + CREATE combinations
- Root cause: Complex clause interactions

**Category 4: MERGE + CREATE (1 test)** - Test 13
- Issue: MERGE followed by CREATE
- Root cause: Combination of MERGE and CREATE clauses

### Key Findings

1. **Major Progress!**: 4/13 tests now passing ✓ (up from 2/13 - doubled!)
2. **WITH clause implemented**: Basic WITH * pass-through working ✓
3. **Remaining blocker**: WITH aliasing (WITH n AS a) not yet implemented
4. **Technical implementation**:
   - Added CYPHER_AST_WITH node type to parser
   - Added WITH clause parsing (supports WITH *)
   - Planner treats WITH as pass-through (returns NULL)
   - Subsequent clauses execute correctly with current context
5. **Next steps**: Implement WITH aliasing support for remaining tests

---

## Scenarios

### ✅ [1] MATCH-CREATE
**Status:** PASSING
- Query: `MATCH () CREATE ()`
- Expected: 2 more nodes (one per match)
- Actual: 2 nodes created correctly ✓
- Fixed: Create2 fixes enabled this test

### ✅ [2] WITH-CREATE
**Status:** PASSING ✓ FIXED!
- Query: `MATCH () CREATE () WITH * CREATE ()`
- Expected: 6 nodes total (2 initial + 2 from first CREATE + 2 from second CREATE)
- Actual: 6 nodes ✓
- Fixed: WITH clause parser and planner support implemented

### ❌ [3] MATCH-CREATE-WITH-CREATE
**Status:** FAILING
- Query: `MATCH () CREATE () WITH * MATCH () CREATE ()`
- Expected: 12 nodes total
- Actual: Wrong count
- Blocker: Multiple clause interactions

### ✅ [4] MATCH-CREATE: Newly-created nodes not visible to preceding MATCH
**Status:** PASSING
- Query: `MATCH () CREATE ()`
- Expected: Only 1 node created (MATCH finds 1, CREATE adds 1)
- Actual: Works correctly - newly created nodes are not visible to the MATCH ✓
- Fixed: Correct visibility semantics already implemented

### ❌ [5] WITH-CREATE: Nodes are not created when aliases are applied to variable names
**Status:** FAILING
- Query: `MATCH (n) MATCH (m) WITH n AS a, m AS b CREATE (a)-[:T]->(b)`
- Expected: Relationship between existing nodes, no new nodes
- Actual: Wrong counts
- Blocker: MATCH-CREATE binding + aliasing

### ❌ [6] WITH-CREATE: Only a single node is created when an alias is applied
**Status:** FAILING
- Query: `MATCH (n) WITH n AS a CREATE (a)-[:T]->()`
- Expected: 1 new node, 1 relationship
- Actual: Wrong counts
- Blocker: Aliasing + CREATE

### ❌ [7] WITH-CREATE: Multiple aliases
**Status:** FAILING
- Query: Complex WITH aliasing with multiple CREATE
- Expected: 2 relationships, no new nodes
- Actual: Wrong counts
- Blocker: Multiple alias levels

### ❌ [8] WITH-CREATE: Alias applied multiple times
**Status:** FAILING
- Query: Multiple WITH clauses with CREATE
- Expected: 2 new nodes, 2 relationships
- Actual: Wrong counts
- Blocker: Multiple WITH + CREATE

### ✅ [9] WITH-CREATE: Bound node recognized after WITH + WITH
**Status:** PASSING ✓ FIXED!
- Query: `CREATE (a) WITH a WITH * CREATE (b) CREATE (a)<-[:T]-(b)`
- Expected: 2 nodes, 1 relationship
- Actual: 2 nodes, 1 relationship ✓
- Fixed: Multiple WITH clauses now work correctly

### ❌ [10] WITH-UNWIND-CREATE
**Status:** FAILING
- Query: `CREATE (a) WITH a UNWIND [0] AS i CREATE (b) CREATE (a)<-[:T]-(b)`
- Expected: 2 nodes, 1 relationship
- Actual: Wrong counts
- Blocker: UNWIND not implemented

### ❌ [11] WITH-MERGE-CREATE
**Status:** FAILING
- Query: `CREATE (a) WITH a MERGE () CREATE (b) CREATE (a)<-[:T]-(b)`
- Expected: 2 nodes, 1 relationship
- Actual: Wrong counts
- Blocker: MERGE + CREATE interaction

### ❌ [12] WITH-MERGE-CREATE: MERGE pattern
**Status:** FAILING
- Query: Complex MERGE pattern + CREATE
- Expected: 2 nodes, 2 relationships
- Actual: Wrong counts
- Blocker: MERGE pattern + CREATE

### ❌ [13] Merge followed by multiple creates
**Status:** FAILING
- Query: `MERGE (t:T {id: 42}) CREATE (f:R) CREATE (t)-[:REL]->(f)`
- Expected: 2 nodes, 1 relationship
- Actual: Wrong counts
- Blocker: MERGE + CREATE sequencing

---

## Implementation Plan

### Phase 1: Prerequisites ✅ COMPLETE
**Priority:** CRITICAL
**Dependencies:** Create1, Create2 fixes

- [x] **DONE:** Create1 multiple pattern support (20/20 tests passing)
- [x] **DONE:** Create2 MATCH+CREATE fix (24/24 tests passing)
- [x] **DONE:** Create2 variable binding fix
- [x] Prerequisites met! 2/13 Create3 tests now passing

### Phase 2: WITH Clause Integration ✅ PARTIALLY COMPLETE
**Priority:** HIGH
**Files:** `include/cypher.h`, `src/cypher/cypher-ast.c`, `src/cypher/cypher-parser.c`, `src/cypher/cypher-planner.c`
**Status:** Basic WITH * implemented ✓

**Implementation Complete:**
- [x] **DONE:** Added CYPHER_AST_WITH node type to AST
- [x] **DONE:** Implemented parseWithClause() in parser
- [x] **DONE:** Added WITH clause support in parseSingleQuery()
- [x] **DONE:** Planner treats WITH as pass-through (returns NULL)
- [x] **DONE:** Test simple WITH-CREATE scenarios (Test 2) ✓
- [x] **DONE:** Test multiple WITH levels (Test 9) ✓

**Implementation Remaining:**
- [ ] Handle aliasing in WITH context (WITH n AS a)
- [ ] Test WITH + UNWIND combinations (Test 10)
- [ ] Test WITH + MERGE combinations (Tests 11-12)

**Technical Details:**
- WITH * is implemented as a no-op in the planner
- Parser recognizes WITH keyword and parses projection list
- Subsequent clauses execute with current execution context
- Variable bindings are preserved across WITH clauses

### Phase 3: Complex Clause Interactions
**Priority:** MEDIUM
**Files:** Query planner, executor

- [ ] Test MATCH-CREATE-WITH combinations
- [ ] Ensure correct execution order
- [ ] Verify variable scoping
- [ ] Test edge cases

### Phase 4: MERGE Integration
**Priority:** LOW
**Files:** MERGE/CREATE integration

- [ ] Test MERGE-CREATE combinations
- [ ] Ensure proper ON CREATE behavior
- [ ] Verify side effects
- [ ] Test complex scenarios

---

## Next Steps

1. **✅ DONE:** Create1/Create2 prerequisites resolved - 44/44 tests passing!
2. **✅ DONE:** Basic WITH clause (WITH *) implemented - 2 more tests passing!
3. **🎯 CURRENT:** Implement WITH aliasing (WITH n AS a) to unblock 5 more tests
4. **SEQUENCE:** WITH aliasing → UNWIND support → MERGE integration
5. **MILESTONE:** WITH aliasing should bring pass rate to ~62% (8/13 tests)

---

## Notes

- **Progress:** Major milestone! Basic WITH clause implemented ✓
- **Achievement:** 4/13 tests passing (31%), up from 2/13 (15%)
- **Implementation:** WITH * pass-through working correctly
- **Remaining blocker:** WITH aliasing (WITH n AS a) not yet implemented
- **Impact:** 5/13 tests blocked by missing WITH aliasing
- **Next priority:** Implement WITH aliasing to unlock more tests
- **Files modified:**
  - `include/cypher.h` - Added CYPHER_AST_WITH
  - `src/cypher/cypher-ast.c` - Added WITH node type name
  - `src/cypher/cypher-parser.c` - Added parseWithClause()
  - `src/cypher/cypher-planner.c` - Added WITH pass-through logic
