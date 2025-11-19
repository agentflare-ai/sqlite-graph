# MERGE Implementation Status & Task List

**Last Updated:** 2025-01-15
**Overall Status:** 🟡 IN PROGRESS (13/75 passing, ~17%)

---

## Quick Summary

| Feature | Total | Pass | Fail | Ignored | Pass Rate | Status |
|---------|-------|------|------|---------|-----------|--------|
| Merge1 - Basic Node | 17 | 9 | 4 | 4 | 53% | 🟡 In Progress |
| Merge2 - ON CREATE (nodes) | 6 | 3 | 2 | 1 | 50% | 🟡 In Progress |
| Merge3 - ON MATCH (nodes) | 5 | 2 | 1 | 2 | 40% | 🟡 In Progress |
| Merge4 - ON MATCH + ON CREATE | 2 | 0 | 2 | 0 | 0% | 🔴 Needs Work |
| Merge5 - Relationships | 29 | 0 | 0 | 29 | 0% | 🔴 Not Started |
| Merge6 - Rels ON CREATE | 6 | 0 | 0 | 6 | 0% | 🔴 Not Started |
| Merge7 - Rels ON MATCH | 5 | 0 | 0 | 5 | 0% | 🔴 Not Started |
| Merge8 - Rels Combined | 1 | 0 | 0 | 1 | 0% | 🔴 Not Started |
| Merge9 - Clause Interop | 4 | 0 | 0 | 4 | 0% | 🔴 Not Started |
| **TOTAL** | **75** | **14** | **9** | **52** | **19%** | 🟡 **In Progress** |

---

## Test Results Detail

### ✅ Currently Passing (14 tests)

#### Merge1 - Basic Node (9 passing)
- ✅ Merge1_01: Merge node when no nodes exist
- ✅ Merge1_02: Merge node with label
- ✅ Merge1_03: Merge node with label when it exists
- ✅ Merge1_04: Create when doesn't match (properties)
- ✅ Merge1_05: Create when doesn't match (properties and label)
- ✅ Merge1_06: Merge node with prop and label
- ✅ Merge1_08: Handle argument properly (bound variables)
- ✅ Merge1_09: Support updates while merging (ON MATCH)
- ✅ Merge1_17: Fail on merging node with null property

#### Merge2 - ON CREATE (3 passing)
- ✅ Merge2_02: ON CREATE on created nodes
- ✅ Merge2_03: Add property on create
- ✅ Merge2_04: Property not added when node exists

#### Merge3 - ON MATCH (2 passing)
- ✅ Merge3_03: Set property on match
- ✅ Merge3_04: Use properties of bound node in ON MATCH

---

### ❌ Currently Failing (9 tests)

#### Merge1 - Basic Node (4 failing)
- ❌ **Merge1_07:** Work when finding multiple elements
  - **Issue:** Expected 2 nodes, got 1
  - **Root Cause:** Multiple element matching logic

- ❌ **Merge1_11:** Merge using property of bound node
  - **Issue:** Expected 2 nodes, got 1
  - **Root Cause:** Property expression evaluation `MERGE (b {name: a.name})`

- ❌ **Merge1_12:** Merge using property of freshly created node
  - **Issue:** Expected 2 nodes, got 1
  - **Root Cause:** Property copying from newly created nodes

- ❌ **Merge1_15:** Fail when merge node that is already bound
  - **Issue:** Should fail but doesn't
  - **Root Cause:** Already-bound variable validation not triggering

#### Merge2 - ON CREATE (2 failing)
- ❌ **Merge2_05:** Use properties of bound node in ON CREATE
  - **Issue:** Expected 2 nodes, got 1
  - **Root Cause:** Property expression evaluation in ON CREATE clause

- ❌ **Merge2_06:** Fail when using undefined variable in ON CREATE
  - **Issue:** Should fail but doesn't
  - **Root Cause:** Undefined variable validation missing

#### Merge3 - ON MATCH (1 failing)
- ❌ **Merge3_05:** Fail when using undefined variable in ON MATCH
  - **Issue:** Should fail but doesn't
  - **Root Cause:** Undefined variable validation missing

#### Merge4 - ON MATCH + ON CREATE (2 failing)
- ❌ **Merge4_01:** Use properties in ON CREATE and ON MATCH
  - **Issue:** Property expressions not working
  - **Root Cause:** Combined clause property expression handling

- ❌ **Merge4_02:** Update different properties
  - **Issue:** Property expressions not working
  - **Root Cause:** Combined clause property expression handling

---

### 🔵 Ignored/Not Started (52 tests)

#### Merge1 (4 ignored)
- 🔵 Merge1_10: Multiple labels (not yet supported)
- 🔵 Merge1_13: Path binding (not implemented)
- 🔵 Merge1_14: Match on deleted nodes (DELETE not implemented)
- 🔵 Merge1_16: Parameters (not implemented)

#### Merge2 (1 ignored)
- 🔵 Merge2_01: Label assignment in ON CREATE (not supported)

#### Merge3 (2 ignored)
- 🔵 Merge3_01: Set labels on match (not supported)
- 🔵 Merge3_02: Add label on match when exists (not supported)

#### Merge5-9 (45 ignored)
- 🔵 All 29 Merge5 tests (Relationships - not started)
- 🔵 All 6 Merge6 tests (Rels ON CREATE - not started)
- 🔵 All 5 Merge7 tests (Rels ON MATCH - not started)
- 🔵 All 1 Merge8 test (Rels Combined - not started)
- 🔵 All 4 Merge9 tests (Clause Interop - not started)

---

## Task List by Priority

### 🔴 CRITICAL - Fix Failing Node Tests (Phase 1)

#### Task 1: Property Expression Evaluation
**Affects:** Merge1_11, Merge1_12, Merge2_05, Merge4_01, Merge4_02

- [ ] Investigate property expression handling in MERGE patterns
- [ ] Fix `MERGE (b {name: a.name})` where `a` is a bound variable
- [ ] Support property access across clause boundaries
- [ ] Test with various expression types (literal, variable, function)
- [ ] Verify property copying from newly created nodes works

**Example queries that should work:**
```cypher
CREATE (a {name: 'Alice'})
MERGE (b {name: a.name})  -- Should create node with name='Alice'

CREATE (a {value: 10})
MERGE (b:Node {id: 1}) ON CREATE SET b.copied = a.value
```

**Files to modify:**
- `src/cypher/cypher-planner.c` - Property expression evaluation in MERGE
- `src/cypher/cypher-write.c` - Property copying logic

---

#### Task 2: Variable Validation (Error Handling)
**Affects:** Merge1_15, Merge2_06, Merge3_05

- [ ] Implement variable type tracking across clauses
- [ ] Detect already-bound variables in MERGE patterns
- [ ] Validate undefined variables in SET expressions
- [ ] Add compile-time or runtime validation
- [ ] Generate appropriate error messages

**Example queries that should fail:**
```cypher
CREATE (a) MERGE (a)  -- 'a' already bound
MERGE (a) ON CREATE SET a.value = b.value  -- 'b' undefined
MERGE (a) ON MATCH SET a.value = b.value   -- 'b' undefined
```

**Files to modify:**
- `src/cypher/cypher-planner.c` - Variable validation logic
- Semantic validator or type checker

---

#### Task 3: Multiple Element Matching
**Affects:** Merge1_07

- [ ] Investigate multiple element matching behavior
- [ ] Fix MERGE behavior when pattern matches multiple nodes
- [ ] Ensure correct node count after multiple matches
- [ ] Test various scenarios (0, 1, 2+ matches)

**Example query:**
```cypher
CREATE (:X), (:X)  -- Create 2 nodes with :X label
MERGE (:X)         -- Should this match or create?
-- Expected: Returns both nodes (2 total)
```

**Files to modify:**
- `src/cypher/cypher-write.c` - MERGE matching logic

---

### 🟡 HIGH PRIORITY - Relationship MERGE (Phase 2)

#### Task 4: Basic Relationship MERGE (Merge5)
**29 tests total, target: 20 passing (69%)**

**Core Features:**
- [ ] Create relationship with MERGE: `MERGE (a)-[r:KNOWS]->(b)`
- [ ] Match existing relationships
- [ ] Handle relationship properties: `MERGE (a)-[r:KNOWS {since: 2024}]->(b)`
- [ ] Support direction (outgoing, incoming, undirected)
- [ ] Handle bound node variables: `MATCH (a) MERGE (a)-[r:KNOWS]->(b)`
- [ ] Test with merged nodes: `MERGE (a:Person {name: 'Alice'})-[r:KNOWS]->(b:Person {name: 'Bob'})`

**Known Issues (from previous implementation):**
1. ✅ Core functionality exists in `src/cypher/cypher-write.c` (lines 2193-2300)
2. ❌ Bare patterns fail: `MERGE (a)-[r:TYPE]->(b)` creates 1 node instead of 2
3. ❌ Incoming direction not parsed: `MERGE (a)<-[r:TYPE]-(b)`
4. ✅ Works with node properties

**Implementation subtasks:**
- [ ] Fix bare relationship pattern node creation
- [ ] Add parser support for incoming direction `<-` in MERGE
- [ ] Implement relationship property filtering
- [ ] Handle undirected relationships `-[r]-`
- [ ] Test relationship matching (not just creation)
- [ ] Add self-relationship support

**Files to modify:**
- `src/cypher/cypher-parser.c` - Parse `<-` in MERGE relationships
- `src/cypher/cypher-write.c` - Fix node creation in bare patterns
- `src/cypher/cypher-planner.c` - Relationship MERGE planning

---

#### Task 5: Relationship Error Validation (Merge5)
**5 error tests**

- [ ] Fail when merging relationship without type: `MERGE (a)-[r]->(b)`
- [ ] Fail with multiple types: `MERGE (a)-[r:TYPE1|TYPE2]->(b)`
- [ ] Fail with variable length: `MERGE (a)-[r*1..3]->(b)`
- [ ] Fail when relationship already bound
- [ ] Fail on null property in relationship: `MERGE ()-[r:KNOWS {prop: NULL}]->()`

**Files to modify:**
- `src/cypher/cypher-planner.c` - Add relationship MERGE validation

---

#### Task 6: Relationship ON CREATE (Merge6)
**6 tests, target: 5 passing**

- [ ] Implement ON CREATE for relationships
- [ ] Set relationship properties on creation
- [ ] Copy properties from bound nodes
- [ ] Test: `MERGE (a)-[r:KNOWS]->(b) ON CREATE SET r.created = timestamp()`

**Files to modify:**
- `src/cypher/cypher-write.c` - ON CREATE for relationships
- `src/cypher/cypher-planner.c` - Plan ON CREATE for relationships

---

#### Task 7: Relationship ON MATCH (Merge7)
**5 tests, target: 4 passing**

- [ ] Implement ON MATCH for relationships
- [ ] Update relationship properties on match
- [ ] Copy properties from bound nodes
- [ ] Test: `MERGE (a)-[r:KNOWS]->(b) ON MATCH SET r.matched = true`

**Files to modify:**
- `src/cypher/cypher-write.c` - ON MATCH for relationships

---

### 🟢 MEDIUM PRIORITY - Advanced Features (Phase 3)

#### Task 8: Combined ON CREATE + ON MATCH
**Merge4: 2 tests, Merge8: 1 test**

- [ ] Fix property expressions in combined clauses
- [ ] Support different properties in ON CREATE vs ON MATCH
- [ ] Test: `MERGE (a) ON CREATE SET a.created=1 ON MATCH SET a.matched=1`

**Files to modify:**
- `src/cypher/cypher-write.c` - Combined clause handling

---

#### Task 9: Clause Interoperation (Merge9)
**4 tests, target: 2-3 passing**

- [ ] Test MERGE with UNWIND
- [ ] Test MERGE with WITH
- [ ] Test MERGE with CREATE
- [ ] Ensure variable scoping works correctly

**Dependencies:** UNWIND, WITH clauses

---

#### Task 10: Label Assignment (SET syntax)
**Affects:** Merge2_01, Merge3_01, Merge3_02

- [ ] Implement `SET n:Label` syntax
- [ ] Support in ON CREATE: `MERGE (a) ON CREATE SET a:NewLabel`
- [ ] Support in ON MATCH: `MERGE (a) ON MATCH SET a:MatchedLabel`
- [ ] Handle multiple labels: `SET n:Label1:Label2`

**Files to modify:**
- `src/cypher/cypher-parser.c` - Parse `SET n:Label`
- `src/cypher/cypher-write.c` - Execute label assignment

---

### 🔵 LOW PRIORITY - Future Features (Phase 4)

#### Task 11: Multiple Labels in MERGE
**Affects:** Merge1_10

- [ ] Support MERGE with multiple labels: `MERGE (n:Label1:Label2)`
- [ ] Update parser for multiple label syntax
- [ ] Update storage layer if needed

---

#### Task 12: Path Binding
**Affects:** Merge1_13

- [ ] Implement path binding: `p = MERGE (a)-[r]->(b)`
- [ ] Add path type to type system
- [ ] Support path expressions

---

#### Task 13: DELETE Integration
**Affects:** Merge1_14

- [ ] Implement DELETE clause
- [ ] Add deletion tracking
- [ ] Test: MERGE shouldn't match deleted nodes/relationships

---

#### Task 14: Parameters
**Affects:** Merge1_16

- [ ] Implement query parameters
- [ ] Add parameter validation in MERGE
- [ ] Test error handling for invalid parameter usage

---

## Implementation Roadmap

### Phase 1: Fix Node MERGE Issues ⏰ CURRENT
**Target:** 17-20/30 node tests passing (57-67%)
**Timeline:** Immediate

**Tasks:**
1. ✅ Fix property expression evaluation (Tasks 1)
2. ✅ Fix variable validation (Task 2)
3. ✅ Fix multiple element matching (Task 3)

**Success Criteria:**
- All Merge1 basic scenarios passing (except ignored)
- Merge2 and Merge3 at 80%+ pass rate
- Merge4 at least 1 test passing

**Expected Results:**
- Merge1: 13/17 passing (76%) - up from 53%
- Merge2: 5/6 passing (83%) - up from 50%
- Merge3: 3/5 passing (60%) - up from 40%
- Merge4: 1/2 passing (50%) - up from 0%
- **Total: 22/30 node tests (73%)**

---

### Phase 2: Basic Relationship MERGE
**Target:** 20/29 Merge5 tests passing (69%)
**Timeline:** Short-term

**Tasks:**
1. ✅ Fix bare pattern node creation (Task 4)
2. ✅ Add incoming direction support (Task 4)
3. ✅ Implement basic relationship MERGE (Task 4)
4. ✅ Add relationship error validation (Task 5)

**Success Criteria:**
- Basic relationship creation works
- Relationship matching works
- Direction handling correct
- Error validation in place

**Expected Results:**
- Merge5: 20/29 passing (69%)
- **Total: 42/59 tests (71%)**

---

### Phase 3: Relationship ON CREATE/ON MATCH
**Target:** 9/12 relationship clause tests (75%)
**Timeline:** Medium-term

**Tasks:**
1. ✅ Implement ON CREATE for relationships (Task 6)
2. ✅ Implement ON MATCH for relationships (Task 7)
3. ✅ Fix combined clauses (Task 8)

**Success Criteria:**
- Merge6: 5/6 passing (83%)
- Merge7: 4/5 passing (80%)
- Merge8: 1/1 passing (100%)
- Merge4: 2/2 passing (100%)

**Expected Results:**
- Merge4: 2/2 passing (100%) - up from 0%
- Merge6: 5/6 passing (83%)
- Merge7: 4/5 passing (80%)
- Merge8: 1/1 passing (100%)
- **Total: 54/71 tests (76%)**

---

### Phase 4: Advanced Features & Polish
**Target:** 60+/75 tests (80%+)
**Timeline:** Long-term

**Tasks:**
1. ✅ Clause interoperation (Task 9)
2. ✅ Label assignment (Task 10)
3. ⏸️ Optional: Multiple labels, path binding, DELETE, parameters

**Success Criteria:**
- Merge9: 2-3/4 passing (50-75%)
- Most skipped tests implemented
- 80%+ overall pass rate

**Expected Results:**
- **Total: 60-65/75 tests (80-87%)**

---

## Dependencies & Blockers

### Immediate Blockers (Phase 1)
- ❌ Property expression evaluation in MERGE patterns
- ❌ Variable validation (already-bound, undefined)
- ❌ Multiple element matching logic

### Short-term Blockers (Phase 2-3)
- ❌ Parser support for `<-` in MERGE relationships
- ❌ Bare pattern node creation logic
- ⏸️ Label assignment syntax (`SET n:Label`)

### Long-term Blockers (Phase 4)
- ⏸️ UNWIND clause (for Merge9)
- ⏸️ WITH clause (for Merge9)
- ⏸️ DELETE clause (for Merge1_14)
- ⏸️ Parameters (for Merge1_16)
- ⏸️ Path binding (for Merge1_13)
- ⏸️ Multiple labels in MERGE (for Merge1_10)

---

## Files Requiring Changes

### Critical Priority (Phase 1)
1. ✅ `src/cypher/cypher-planner.c` - Property expressions, variable validation
2. ✅ `src/cypher/cypher-write.c` - Multiple element matching, property copying
3. ✅ Semantic validator - Variable type tracking

### High Priority (Phase 2)
4. ✅ `src/cypher/cypher-parser.c` - Parse `<-` in MERGE, label assignment
5. ✅ `src/cypher/cypher-write.c` - Relationship MERGE fixes

### Medium Priority (Phase 3)
6. ✅ `src/cypher/cypher-write.c` - ON CREATE/ON MATCH for relationships
7. ✅ `src/cypher/cypher-planner.c` - Combined clause handling

### Low Priority (Phase 4)
8. ⏸️ UNWIND implementation
9. ⏸️ WITH implementation
10. ⏸️ DELETE implementation
11. ⏸️ Parameters implementation

---

## Success Metrics

### Current: 19% (14/75 passing)

### Phase 1 Target: 29% (22/75)
- Node operations: 22/30 (73%)
- Relationship operations: 0/41 (0%)
- **Overall improvement: +10%**

### Phase 2 Target: 56% (42/75)
- Node operations: 22/30 (73%)
- Relationship operations: 20/41 (49%)
- **Overall improvement: +37%**

### Phase 3 Target: 72% (54/75)
- Node operations: 24/30 (80%)
- Relationship operations: 30/41 (73%)
- **Overall improvement: +53%**

### Phase 4 Target: 80-87% (60-65/75)
- Node operations: 26-28/30 (87-93%)
- Relationship operations: 32-35/41 (78-85%)
- Advanced operations: 2-3/4 (50-75%)
- **Overall improvement: +61-68%**

---

## Known Issues & Workarounds

### Issue 1: Property Expression Evaluation
**Status:** Known bug
**Workaround:** Use literal values instead of variable properties
**Fix Priority:** Critical (Phase 1)

### Issue 2: Bare Relationship Patterns
**Status:** Known bug
**Example:** `MERGE (a)-[r:TYPE]->(b)` creates 1 node instead of 2
**Workaround:** Use node identifiers with properties
**Fix Priority:** High (Phase 2)

### Issue 3: Incoming Direction Not Parsed
**Status:** Parser limitation
**Example:** `MERGE (a)<-[r:TYPE]-(b)` fails to parse
**Workaround:** Use outgoing direction and swap variables
**Fix Priority:** High (Phase 2)

### Issue 4: Variable Validation Not Triggering
**Status:** Query compilation pipeline issue
**Example:** `CREATE (a) MERGE (a)` should fail but doesn't
**Workaround:** None (should be invalid)
**Fix Priority:** Critical (Phase 1)

---

## Testing Notes

### Memory Safety
✅ All passing tests verified with valgrind
✅ No memory leaks detected

### Test Organization
- **Test file:** `tests/tck_test_clauses_merge.c`
- **Coverage report:** `docs/TCK_MERGE_COVERAGE.md`
- **Baseline logs:** `logs/tck_merge*.log`

### Test Execution
```bash
# Run all MERGE tests
make test

# Run specific MERGE test group
./tests/test_clauses_merge

# Run with valgrind
valgrind --leak-check=full ./tests/test_clauses_merge
```

---

## Next Steps (Immediate)

### Week 1: Fix Critical Node Issues
1. **Day 1-2:** Fix property expression evaluation (Task 1)
   - Debug `MERGE (b {name: a.name})` scenarios
   - Implement property access across clause boundaries

2. **Day 3-4:** Fix variable validation (Task 2)
   - Implement already-bound variable detection
   - Add undefined variable validation in SET

3. **Day 5:** Fix multiple element matching (Task 3)
   - Debug and fix Merge1_07

### Week 2: Start Relationship MERGE
4. **Day 1-2:** Fix bare pattern node creation
   - Debug `MERGE (a)-[r:TYPE]->(b)` node count issue

5. **Day 3:** Add incoming direction parser support
   - Implement `<-` parsing in MERGE relationships

6. **Day 4-5:** Test basic relationship MERGE
   - Run Merge5 tests
   - Document results

---

## Progress Tracking

```
Current State:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Node Operations:        14/30  (47%) 🟡 In Progress
Relationship Ops:        0/41  ( 0%) 🔴 Not Started
Advanced Ops:            0/4   ( 0%) 🔴 Not Started
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total:                  14/75  (19%)
```

```
Phase 1 Target:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Node Operations:        22/30  (73%) 🟢 Target
Relationship Ops:        0/41  ( 0%) 🔴 Not Started
Advanced Ops:            0/4   ( 0%) 🔴 Not Started
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total:                  22/75  (29%) +10%
```

```
Final Target (Phase 4):
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Node Operations:     26-28/30  (87-93%) 🟢 Excellent
Relationship Ops:    32-35/41  (78-85%) 🟢 Good
Advanced Ops:          2-3/4   (50-75%) 🟡 Acceptable
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total:              60-65/75   (80-87%) 🏆 Target
```

---

## References

- **TCK Index:** `.agents/plans/merge/TCK_INDEX.md`
- **Detailed Plans:**
  - `01_TCK_Merge1_Basic_Node.md`
  - `02_TCK_Merge2_ON_CREATE.md`
  - `03_TCK_Merge3_ON_MATCH.md`
  - `04_TCK_Merge4_ON_MATCH_ON_CREATE.md`
  - `05_TCK_Merge5_Relationships.md`
  - `06_TCK_Merge6_Relationships_ON_CREATE.md`
  - `07_TCK_Merge7_Relationships_ON_MATCH.md`
  - `08_TCK_Merge8_Relationships_COMBINED.md`
  - `09_TCK_Merge9_Clause_Interoperation.md`
- **Test Implementation:** `tests/tck_test_clauses_merge.c`
- **Coverage Report:** `docs/TCK_MERGE_COVERAGE.md`
- **Feature Files:** `tck/features/clauses/merge/Merge*.feature`
