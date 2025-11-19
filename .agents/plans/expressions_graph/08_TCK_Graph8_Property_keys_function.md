# Graph8 - Property keys function - Task List

**Feature:** Graph8 - Property keys function
**Status:** 🔴 NOT STARTED (0/8 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Property keys function

**Feature File:** `tck/features/expressions/graph/Graph8.feature`
**Total Scenarios:** 8

**Key Capabilities:**
- Using `keys()` on a single node, non-empty result
- Using `keys()` on multiple nodes, non-empty result
- Using `keys()` on a single node, empty result
- Using `keys()` on an optionally matched node
- Using `keys()` on a relationship, non-empty result
- ... and 3 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 8 | 01, 02, 03, 04, 05, 06, 07, 08 |

---

## Test Scenarios

### 🔴 [Scenario 01] Using `keys()` on a single node, non-empty result
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      UNWIND keys(n) AS x
      RETURN DISTINCT x AS theProps
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

### 🔴 [Scenario 02] Using `keys()` on multiple nodes, non-empty result
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      UNWIND keys(n) AS x
      RETURN DISTINCT x AS theProps
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

### 🔴 [Scenario 03] Using `keys()` on a single node, empty result
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      UNWIND keys(n) AS x
      RETURN DISTINCT x AS theProps
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

### 🔴 [Scenario 04] Using `keys()` on an optionally matched node
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
OPTIONAL MATCH (n)
      UNWIND keys(n) AS x
      RETURN DISTINCT x AS theProps
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

### 🔴 [Scenario 05] Using `keys()` on a relationship, non-empty result
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r:KNOWS]-()
      UNWIND keys(r) AS x
      RETURN DISTINCT x AS theProps
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

### 🔴 [Scenario 06] Using `keys()` on a relationship, empty result
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r:KNOWS]-()
      UNWIND keys(r) AS x
      RETURN DISTINCT x AS theProps
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

### 🔴 [Scenario 07] Using `keys()` on an optionally matched relationship
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
OPTIONAL MATCH ()-[r:KNOWS]-()
      UNWIND keys(r) AS x
      RETURN DISTINCT x AS theProps
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

### 🔴 [Scenario 08] Using `keys()` and `IN` to check property existence
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      RETURN 'exists' IN keys(n) AS a,
             'missing' IN keys(n) AS b,
             'missingToo' IN keys(n) AS c
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
**Goal:** Implement basic Graph8 functionality
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

- Feature file: `tck/features/expressions/graph/Graph8.feature`
- Total scenarios: 8
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_expressions_graph
```

**Feature File Location:**
```
tck/features/expressions/graph/Graph8.feature
```
