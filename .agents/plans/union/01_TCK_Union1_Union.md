# Union1 - Union - Task List

**Feature:** Union1 - Union
**Status:** 🔴 NOT STARTED (0/5 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Union

**Feature File:** `tck/features/clauses/union/Union1.feature`
**Total Scenarios:** 5

**Key Capabilities:**
- Two elements, both unique, distinct
- Three elements, two unique, distinct
- Two single-column inputs, one with duplicates, distinct
- Should be able to create text output from union queries
- Failing when UNION has different columns

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 5 | 01, 02, 03, 04, 05 |

---

## Test Scenarios

### 🔴 [Scenario 01] Two elements, both unique, distinct
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1 AS x
      UNION
      RETURN 2 AS x
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

### 🔴 [Scenario 02] Three elements, two unique, distinct
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 2 AS x
      UNION
      RETURN 1 AS x
      UNION
      RETURN 2 AS x
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

### 🔴 [Scenario 03] Two single-column inputs, one with duplicates, distinct
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [2, 1, 2, 3] AS x
      RETURN x
      UNION
      UNWIND [3, 4] AS x
      RETURN x
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

### 🔴 [Scenario 04] Should be able to create text output from union queries
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      RETURN a AS a
      UNION
      MATCH (b:B)
      RETURN b AS a
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

### 🔴 [Scenario 05] Failing when UNION has different columns
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1 AS a
      UNION
      RETURN 2 AS b
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
**Goal:** Implement basic Union1 functionality
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

- Feature file: `tck/features/clauses/union/Union1.feature`
- Total scenarios: 5
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_union
```

**Feature File Location:**
```
tck/features/clauses/union/Union1.feature
```
