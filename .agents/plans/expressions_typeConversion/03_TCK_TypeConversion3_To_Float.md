# TypeConversion3 - To Float - Task List

**Feature:** TypeConversion3 - To Float
**Status:** 🔴 NOT STARTED (0/5 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

To Float

**Feature File:** `tck/features/expressions/typeConversion/TypeConversion3.feature`
**Total Scenarios:** 5

**Key Capabilities:**
- `toFloat()` on mixed number types
- `toFloat()` returning null on non-numerical string
- `toFloat()` handling Any type
- `toFloat()` on a list of strings
- `toFloat()` on node property

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 5 | 01, 02, 03, 04, 05 |

---

## Test Scenarios

### 🔴 [Scenario 01] `toFloat()` on mixed number types
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH [3.4, 3] AS numbers
      RETURN [n IN numbers | toFloat(n)] AS float_numbers
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

### 🔴 [Scenario 02] `toFloat()` returning null on non-numerical string
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH 'foo' AS foo_string, '' AS empty_string
      RETURN toFloat(foo_string) AS foo, toFloat(empty_string) AS empty
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

### 🔴 [Scenario 03] `toFloat()` handling Any type
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH [3.4, 3, '5'] AS numbers
      RETURN [n IN numbers | toFloat(n)] AS float_numbers
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

### 🔴 [Scenario 04] `toFloat()` on a list of strings
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH ['1', '2', 'foo'] AS numbers
      RETURN [n IN numbers | toFloat(n)] AS float_numbers
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

### 🔴 [Scenario 05] `toFloat()` on node property
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH p = (n)-[r:T]->()
      RETURN [x IN [1.0, <invalid>] | toFloat(x) ] AS list
```

**Expected:** TypeError

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
**Goal:** Implement basic TypeConversion3 functionality
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

- Feature file: `tck/features/expressions/typeConversion/TypeConversion3.feature`
- Total scenarios: 5
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_expressions_typeConversion
```

**Feature File Location:**
```
tck/features/expressions/typeConversion/TypeConversion3.feature
```
