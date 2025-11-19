# Quantifier2 - Single quantifier - Task List

**Feature:** Quantifier2 - Single quantifier
**Status:** 🔴 NOT STARTED (0/6 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Single quantifier

**Feature File:** `tck/features/expressions/quantifier/Quantifier2.feature`
**Total Scenarios:** 6

**Key Capabilities:**
- Single quantifier is always false on empty list
- Single quantifier on list containing nodes
- Single quantifier on list containing relationships
- Single quantifier is false if the predicate is statically false and the list is not empty
- Single quantifier is false if the predicate is statically true and the list has more than one element
- ... and 1 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 6 | 01, 08, 09, 13, 14, 15 |

---

## Test Scenarios

### 🔴 [Scenario 01] Single quantifier is always false on empty list
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN single(x IN <list> WHERE <condition>) AS result
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

### 🔴 [Scenario 08] Single quantifier on list containing nodes
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH p = (:SNodes)-[*0..3]->(x)
      WITH tail(nodes(p)) AS nodes
      RETURN nodes, single(x IN nodes WHERE x.name = 'a') AS result
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

### 🔴 [Scenario 09] Single quantifier on list containing relationships
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN single(x IN <list> WHERE x IS NOT NULL) AS result
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

### 🔴 [Scenario 13] Single quantifier is false if the predicate is statically false and the list is not empty
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN single(x IN [1, null, true, 4.5, 'abc', false] WHERE false) AS result
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

### 🔴 [Scenario 14] Single quantifier is false if the predicate is statically true and the list has more than one element
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN single(x IN [1, null, true, 4.5, 'abc', false] WHERE true) AS result
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

### 🔴 [Scenario 15] Single quantifier is true if the predicate is statically true and the list has exactly one non-null element
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN single(x IN <list> WHERE <condition>) AS result
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
**Goal:** Implement basic Quantifier2 functionality
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

- Feature file: `tck/features/expressions/quantifier/Quantifier2.feature`
- Total scenarios: 6
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
tck/features/expressions/quantifier/Quantifier2.feature
```
