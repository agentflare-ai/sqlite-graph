# Boolean3 - XOR logical operations - Task List

**Feature:** Boolean3 - XOR logical operations
**Status:** 🔴 NOT STARTED (0/7 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

XOR logical operations

**Feature File:** `tck/features/expressions/boolean/Boolean3.feature`
**Total Scenarios:** 7

**Key Capabilities:**
- Exclusive disjunction of two truth values
- Exclusive disjunction of three truth values
- Exclusive disjunction of many truth values
- Exclusive disjunction is commutative on non-null
- Exclusive disjunction is commutative on null
- ... and 2 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 7 | 01, 02, 03, 04, 05, 06, 07 |

---

## Test Scenarios

### 🔴 [Scenario 01] Exclusive disjunction of two truth values
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN true XOR true AS tt,
             true XOR false AS tf,
             true XOR null AS tn,
             false XOR true AS ft,
             false XOR false AS ff,
             false XOR null AS fn,
             null XOR true AS nt,
             null XOR false AS nf,
             null XOR null AS nn
```

**Expected:** Result table (see feature file)

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Basic query execution
**Estimated Effort:** TBD

---

### 🔴 [Scenario 02] Exclusive disjunction of three truth values
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN true XOR true XOR true AS ttt,
             true XOR true XOR false AS ttf,
             true XOR true XOR null AS ttn,
             true XOR false XOR true AS tft,
             true XOR false XOR false AS tff,
             true XOR false XOR null AS tfn,
             true XOR null XOR true AS tnt,
             true XOR null XOR false AS tnf,
             true XOR null XOR null AS tnn,
             false XOR true XOR true AS ftt,
             false XOR true XOR false AS ftf,
             false XOR true XOR null AS ftn,
             false XOR false XOR true AS fft,
             false XOR false XOR false AS fff,
             false XOR false XOR null AS ffn,
             false XOR null XOR true AS fnt,
             false XOR null XOR false AS fnf,
             false XOR null XOR null AS fnn,
             null XOR true XOR true AS ntt,
             null XOR true XOR false AS ntf,
             null XOR true XOR null AS ntn,
             null XOR false XOR true AS nft,
             null XOR false XOR false AS nff,
             null XOR false XOR null AS nfn,
             null XOR null XOR true AS nnt,
             null XOR null XOR false AS nnf,
             null XOR null XOR null AS nnn
```

**Expected:** Result table (see feature file)

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Basic query execution
**Estimated Effort:** TBD

---

### 🔴 [Scenario 03] Exclusive disjunction of many truth values
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN true XOR true XOR true XOR true XOR true XOR true XOR true XOR true XOR true XOR true XOR true AS t,
             true XOR true XOR true XOR false XOR true XOR true XOR true XOR true XOR true XOR true XOR true AS tsf,
             true XOR true XOR true XOR null XOR true XOR true XOR true XOR true XOR true XOR true XOR true AS tsn,
             false XOR false XOR false XOR false XOR false XOR false XOR false XOR false XOR false XOR false XOR false AS f,
             false XOR false XOR false XOR false XOR true XOR false XOR false XOR false XOR false XOR false XOR false AS fst,
             false XOR false XOR false XOR false XOR false XOR false XOR null XOR false XOR false XOR false XOR false AS fsn,
             null XOR null XOR null XOR null XOR null XOR null XOR null XOR null XOR null XOR null XOR null AS n,
             null XOR null XOR null XOR null XOR true XOR null XOR null XOR null XOR null XOR null XOR null AS nst,
             null XOR null XOR null XOR null XOR false XOR null XOR null XOR null XOR null XOR null XOR null AS nsf,
             true XOR false XOR false XOR false XOR true XOR false XOR false XOR true XOR true XOR true XOR false AS m1,
             true XOR true XOR false XOR false XOR true XOR false XOR false XOR true XOR true XOR true XOR false AS m2,
             true XOR true XOR false XOR false XOR true XOR null XOR false XOR true XOR true XOR null XOR false AS m3
```

**Expected:** Result table (see feature file)

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Basic query execution
**Estimated Effort:** TBD

---

### 🔴 [Scenario 04] Exclusive disjunction is commutative on non-null
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false] AS a
      UNWIND [true, false] AS b
      RETURN a, b, (a XOR b) = (b XOR a) AS result
```

**Expected:** Result table (see feature file)

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Basic query execution
**Estimated Effort:** TBD

---

### 🔴 [Scenario 05] Exclusive disjunction is commutative on null
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false, null] AS a
      UNWIND [true, false, null] AS b
      WITH a, b WHERE a IS NULL OR b IS NULL
      RETURN a, b, (a XOR b) IS NULL = (b XOR a) IS NULL AS result
```

**Expected:** Result table (see feature file)

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Basic query execution
**Estimated Effort:** TBD

---

### 🔴 [Scenario 06] Exclusive disjunction is associative on non-null
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false] AS a
      UNWIND [true, false] AS b
      UNWIND [true, false] AS c
      RETURN a, b, c, (a XOR (b XOR c)) = ((a XOR b) XOR c) AS result
```

**Expected:** Result table (see feature file)

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Basic query execution
**Estimated Effort:** TBD

---

### 🔴 [Scenario 07] Exclusive disjunction is associative on null
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN <a> XOR <b>
```

**Expected:** SyntaxError

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Error handling infrastructure
**Estimated Effort:** TBD

---

## Implementation Plan

### Phase 1: Foundation
**Goal:** Implement basic Boolean3 functionality
**Timeline:** TBD

**Tasks:**
1. Study feature requirements
2. Design implementation approach
3. Implement core functionality
4. Write initial tests
5. Debug and validate

### Phase 2: Complete Implementation
**Goal:** All scenarios passing
**Timeline:** TBD

**Tasks:**
1. Implement remaining scenarios
2. Handle edge cases
3. Error validation
4. Performance optimization
5. Complete test coverage

---

## Files to Modify

**Likely files to modify:**
- [ ] `src/cypher/cypher-parser.c` - Query parsing
- [ ] `src/cypher/cypher-planner.c` - Query planning
- [ ] `src/cypher/cypher-physical-plan.c` - Physical execution plan
- [ ] `src/cypher/cypher-iterators.c` - Execution iterators
- [ ] `tests/tck_test_*.c` - Test implementation

---

## Success Criteria

### Minimum Viable
- [ ] At least 50% of scenarios passing
- [ ] Basic functionality working
- [ ] Core use cases covered

### Production Ready
- [ ] All scenarios passing
- [ ] Error handling complete
- [ ] Edge cases handled
- [ ] Performance acceptable

---

## Dependencies

**Prerequisites:**
- Basic query parsing infrastructure
- Execution engine
- Result formatting

**Blocks:**
- Features that depend on this implementation

---

## Notes

- Feature file: `tck/features/expressions/boolean/Boolean3.feature`
- Total scenarios: 7
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_expressions_boolean
```

**Feature File Location:**
```
tck/features/expressions/boolean/Boolean3.feature
```
