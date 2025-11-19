# TCK CREATE Test Coverage - Index

**Last Updated:** 2025-01-14
**Overall Status:** 🔄 IN PROGRESS (27/78 passing, 35%)

---

## Summary

| Feature | Scenarios | Passing | Failing | Ignored | Pass Rate |
|---------|-----------|---------|---------|---------|-----------|
| Create1 - Creating nodes | 20 | 12 | 8 | 0 | 60% |
| Create2 - Creating relationships | 24 | 10 | 14 | 0 | 42% |
| Create3 - Interoperation | 13 | 0 | 13 | 0 | 0% |
| Create4 - Large query | 2 | 0 | 0 | 2 | N/A |
| Create5 - Multiple hops | 5 | 1 | 4 | 0 | 20% |
| Create6 - Side effects | 14 | 4 | 10 | 0 | 29% |
| **TOTAL** | **78** | **27** | **49** | **2** | **35%** |

---

## Feature Details

### Create1 - Creating Nodes (60% passing)
**File:** `01_TCK_Create1_Creating_Nodes.md`
**Status:** 🔄 IN PROGRESS

**Passing:** 12/20 (60%)
- Basic node creation ✅
- Single labels ✅
- Properties (including null and large integers) ✅
- RETURN integration ✅

**Failing:** 8/20 (40%)
- Multiple patterns in one CREATE statement (2 tests)
- Multiple labels syntax `:A:B:C` (2 tests)
- Variable binding validation (4 tests)

**Priority Fixes:**
1. Multiple pattern support
2. Multiple label syntax parsing
3. Variable binding validation

---

### Create2 - Creating Relationships (42% passing)
**File:** `02_TCK_Create2_Creating_Relationships.md`
**Status:** 🔄 IN PROGRESS

**Passing:** 10/24 (42%)
- Basic relationship creation ✅
- Properties on relationships ✅
- Some error validation ✅

**Failing:** 14/24 (58%)
- MATCH + CREATE variable binding (6 tests)
- Reverse arrow `<-` syntax (1 test)
- Self-loops (4 tests)
- Multiple patterns/clauses (3 tests)

**Priority Fixes:**
1. **CRITICAL:** MATCH variable binding in CREATE
2. Reverse arrow support in parser
3. Self-loop detection
4. Multiple pattern support

---

### Create3 - Interoperation (0% passing)
**File:** `03_TCK_Create3_Interoperation.md`
**Status:** 🔴 BLOCKED

**Passing:** 0/13 (0%)
**Failing:** 13/13 (100%)

**Blockers:**
- Depends on Create1 multiple pattern fix
- Depends on Create2 MATCH+CREATE fix
- Complex clause interactions (WITH, MERGE, UNWIND)

**Status:** Cannot proceed until Create1/Create2 issues resolved

---

### Create4 - Large Create Query (Deferred)
**File:** `04_TCK_Create4_Large_Query.md`
**Status:** ⏸️ DEFERRED

**Ignored:** 2/2 (100%)
- Movie graph creation (very large query)
- May exceed parser/execution limits
- Not priority for initial TCK compliance

**Status:** Deferred until core functionality complete

---

### Create5 - Multiple Hops (20% passing)
**File:** `05_TCK_Create5_Multiple_Hops.md`
**Status:** 🔄 PARTIAL

**Passing:** 1/5 (20%)
- Forward direction multi-hop works ✅

**Failing:** 4/5 (80%)
- All failures due to reverse arrow `<-` parsing
- Same root cause as Create2 issue

**Priority Fix:**
- Reverse arrow support (shared with Create2)

---

### Create6 - Side Effects Persistence (29% passing)
**File:** `06_TCK_Create6_Side_Effects.md`
**Status:** 🔄 PARTIAL

**Passing:** 4/14 (29%)
- LIMIT/SKIP with CREATE works ✅
- Side effects persist correctly ✅

**Failing:** 10/14 (71%)
- All failures due to UNWIND clause not implemented
- UNWIND is a separate major feature

**Blocker:** Requires UNWIND implementation

---

## Critical Issues Blocking Progress

### 1. Multiple Pattern Support (HIGH PRIORITY)
**Affects:** Create1 (2 tests), Create2 (3 tests), Create3 (all tests)
**Issue:** `CREATE (), ()` only creates 1 node instead of 2
**Files:** Parser and executor
**Impact:** Blocking 18+ tests

### 2. MATCH + CREATE Variable Binding (CRITICAL)
**Affects:** Create2 (6 tests), Create3 (all tests)
**Issue:** Variables from MATCH not accessible in CREATE
**Files:** Variable binding logic
**Impact:** Blocking 19+ tests

### 3. Reverse Arrow Support (HIGH PRIORITY)
**Affects:** Create2 (1 test), Create5 (4 tests)
**Issue:** Parser fails on `<-` in CREATE patterns
**Files:** `cypher-parser.c`
**Impact:** Blocking 5 tests

### 4. Multiple Label Syntax (MEDIUM PRIORITY)
**Affects:** Create1 (2 tests)
**Issue:** `:A:B:C` syntax not parsed
**Files:** Parser
**Impact:** Blocking 2 tests

### 5. UNWIND Clause (SEPARATE FEATURE)
**Affects:** Create6 (10 tests)
**Issue:** UNWIND not implemented
**Files:** Parser and executor
**Impact:** Blocking 10 tests (separate feature)

### 6. Variable Binding Validation (MEDIUM PRIORITY)
**Affects:** Create1 (4 tests), Create2 (4 tests)
**Issue:** Invalid queries not rejected
**Files:** Semantic validator
**Impact:** Blocking 8 tests

---

## Implementation Roadmap

### Phase 1: Core Fixes (Target: 50% overall)
**Priority:** CRITICAL
**Timeframe:** Immediate

1. ✅ Fix MATCH + CREATE variable binding
2. ✅ Implement multiple pattern support
3. ✅ Add reverse arrow parsing
4. ✅ Add self-loop detection

**Expected Result:** Create1 80%, Create2 75%, Create5 100%

### Phase 2: Parser Enhancements (Target: 60% overall)
**Priority:** HIGH
**Timeframe:** Short-term

1. ✅ Multiple label syntax support
2. ✅ Improve error validation
3. ✅ Variable binding checks

**Expected Result:** Create1 95%, Create2 90%

### Phase 3: Integration Testing (Target: 70% overall)
**Priority:** MEDIUM
**Timeframe:** Medium-term

1. ✅ Fix Create3 clause interactions
2. ✅ Test complex scenarios
3. ✅ WITH clause integration

**Expected Result:** Create3 80%+

### Phase 4: UNWIND Feature (Separate)
**Priority:** MEDIUM (separate feature)
**Timeframe:** Long-term

1. ✅ Implement UNWIND clause
2. ✅ Integration with CREATE
3. ✅ Test Create6 scenarios

**Expected Result:** Create6 100%

### Phase 5: Large Queries (Optional)
**Priority:** LOW
**Timeframe:** Future

1. ✅ Test parser limits
2. ✅ Optimize for large queries
3. ✅ Enable Create4 tests

**Expected Result:** Create4 100%

---

## Quick Wins

These changes would have immediate high impact:

1. **Multiple Pattern Support** → +18 tests passing
2. **MATCH+CREATE Binding** → +19 tests passing
3. **Reverse Arrow Parsing** → +5 tests passing

**Total potential: +42 tests (from 35% to 89% passing)**

---

## Dependencies

```
Create1 (basic) 
  ↓
Create2 (relationships)
  ↓
Create3 (integration) ← needs Create1 + Create2 fixes
  ↓
Create5 (multi-hop) ← needs reverse arrow from Create2

Create6 (side effects) ← needs UNWIND (separate)

Create4 (large) ← deferred
```

---

## Files Requiring Changes

### High Priority
1. `src/cypher/cypher-parser.c` - Multiple patterns, labels, reverse arrows
2. `src/cypher/cypher-write.c` - Variable binding, self-loops
3. Variable binding/scoping logic - MATCH+CREATE integration

### Medium Priority  
4. Semantic validator - Error validation
5. `src/cypher/cypher-executor.c` - WITH clause integration

### Low Priority
6. UNWIND implementation (separate feature)
7. Parser optimization for large queries

---

## Testing Strategy

### Current Approach
- All 78 scenarios implemented in `tests/tck_test_clauses_create.c`
- Using proper `exec_cypher()` approach
- Helper functions for assertions
- Well-documented test code

### Recommended Testing Order
1. Fix and verify Create1 tests
2. Fix and verify Create2 tests
3. Fix and verify Create5 tests (shares Create2 fixes)
4. Fix and verify Create3 tests (integration)
5. Implement UNWIND, verify Create6
6. Optimize and test Create4

---

## Success Metrics

### Current State
- 27/78 tests passing (35%)
- 3/6 features partially working
- Core functionality present but incomplete

### Target State (Phase 1)
- 60/78 tests passing (77%)
- 5/6 features mostly working
- All critical blockers resolved

### Target State (Phase 2)
- 70/78 tests passing (90%)
- 5/6 features fully working
- Only UNWIND and large queries remaining

### Final State
- 76/78 tests passing (97%)
- All features complete
- Large query tests may remain deferred

---

## Notes

- Strong foundation already in place (35% passing with no optimization)
- Most failures have clear root causes
- Parser limitations are main blocker
- Well-isolated issues make fixes straightforward
- Good test coverage and documentation
- Memory management appears sound
- No architectural blockers identified

---

## References

- Feature files: `tck/features/clauses/create/Create*.feature`
- Test implementation: `tests/tck_test_clauses_create.c`
- Backup: `tests/tck_test_clauses_create.c.backup`
