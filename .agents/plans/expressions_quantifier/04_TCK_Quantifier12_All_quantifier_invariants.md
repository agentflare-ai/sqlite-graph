# Quantifier12 - All quantifier invariants - Task List

**Feature:** Quantifier12 - All quantifier invariants
**Status:** 🔴 NOT STARTED (0/2 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

All quantifier invariants

**Feature File:** `tck/features/expressions/quantifier/Quantifier12.feature`
**Total Scenarios:** 2

**Key Capabilities:**
- All quantifier is always false if the predicate is statically false and the list is not empty
- All quantifier is always true if the predicate is statically true and the list is not empty

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 2 | 01, 02 |

---

## Test Scenarios

### 🔴 [Scenario 01] All quantifier is always false if the predicate is statically false and the list is not empty
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH [1, null, true, 4.5, 'abc', false, '', [234, false], {a: null, b: true, c: 15.2}, {}, [], [null], [[{b: [null]}]]] AS inputList
      UNWIND inputList AS x
      WITH inputList, x, [ y IN inputList WHERE rand() > 0.5 | y] AS list
      WITH inputList, CASE WHEN rand() < 0.5 THEN reverse(list) ELSE list END + x AS list
      UNWIND inputList AS x
      WITH inputList, x, [ y IN inputList WHERE rand() > 0.5 | y] AS list
      WITH inputList, CASE WHEN rand() < 0.5 THEN reverse(list) ELSE list END + x AS list
      UNWIND inputList AS x
      WITH inputList, x, [ y IN inputList WHERE rand() > 0.5 | y] AS list
      WITH inputList, CASE WHEN rand() < 0.5 THEN reverse(list) ELSE list END + x AS list
      WITH list WHERE size(list) > 0
      WITH all(x IN list WHERE false) AS result, count(*) AS cnt
      RETURN result
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

### 🔴 [Scenario 02] All quantifier is always true if the predicate is statically true and the list is not empty
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [{list: [2], fixed: true},
              {list: [6], fixed: true},
              {list: [7], fixed: true},
              {list: [1, 2, 3, 4, 5, 6, 7, 8, 9], fixed: false}] AS input
      WITH CASE WHEN input.fixed THEN input.list ELSE null END AS fixedList,
           CASE WHEN NOT input.fixed THEN input.list ELSE [1] END AS inputList
      UNWIND inputList AS x
      WITH fixedList, inputList, x, [ y IN inputList WHERE rand() > 0.5 | y] AS list
      WITH fixedList, inputList, CASE WHEN rand() < 0.5 THEN reverse(list) ELSE list END + x AS list
      UNWIND inputList AS x
      WITH fixedList, inputList, x, [ y IN inputList WHERE rand() > 0.5 | y] AS list
      WITH fixedList, inputList, CASE WHEN rand() < 0.5 THEN reverse(list) ELSE list END + x AS list
      UNWIND inputList AS x
      WITH fixedList, inputList, x, [ y IN inputList WHERE rand() > 0.5 | y] AS list
      WITH fixedList, inputList, CASE WHEN rand() < 0.5 THEN reverse(list) ELSE list END + x AS list
      WITH coalesce(fixedList, list) AS list
      WITH all(x IN list WHERE <predicate>) = (size([x IN list WHERE <predicate> | x]) = size(list)) AS result, count(*) AS cnt
      RETURN result
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
**Goal:** Implement basic Quantifier12 functionality
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

- Feature file: `tck/features/expressions/quantifier/Quantifier12.feature`
- Total scenarios: 2
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_expressions_quantifier
```

**Feature File Location:**
```
tck/features/expressions/quantifier/Quantifier12.feature
```
