# Map3 - Keys function - Task List

**Feature:** Map3 - Keys function
**Status:** 🔴 NOT STARTED (0/4 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Keys function

**Feature File:** `tck/features/expressions/map/Map3.feature`
**Total Scenarios:** 4

**Key Capabilities:**
- Using `keys()` on a literal map
- Using `keys()` on a parameter map
- Using `keys()` on null map
- Using `keys()` and `IN` to check field existence

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 4 | 01, 02, 03, 05 |

---

## Test Scenarios

### 🔴 [Scenario 01] Using `keys()` on a literal map
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN keys({name: 'Alice', age: 38, address: {city: 'London', residential: true}}) AS k
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

### 🔴 [Scenario 02] Using `keys()` on a parameter map
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN keys($param) AS k
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

### 🔴 [Scenario 03] Using `keys()` on null map
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN keys(<map>) AS keys
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

### 🔴 [Scenario 05] Using `keys()` and `IN` to check field existence
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH {exists: 42, notMissing: null} AS map
      RETURN 'exists' IN keys(map) AS a,
             'notMissing' IN keys(map) AS b,
             'missing' IN keys(map) AS c
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

## Implementation Plan

### Phase 1: Foundation
**Goal:** Implement basic Map3 functionality
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

- Feature file: `tck/features/expressions/map/Map3.feature`
- Total scenarios: 4
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_expressions_map
```

**Feature File Location:**
```
tck/features/expressions/map/Map3.feature
```
