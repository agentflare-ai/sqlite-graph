# Graph9 - Retrieve all properties as a property map - Task List

**Feature:** Graph9 - Retrieve all properties as a property map
**Status:** 🔴 NOT STARTED (0/7 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Retrieve all properties as a property map

**Feature File:** `tck/features/expressions/graph/Graph9.feature`
**Total Scenarios:** 7

**Key Capabilities:**
- `properties()` on a node
- `properties()` on a relationship
- `properties()` on null
- `properties()` on a map
- `properties()` failing on an integer literal
- ... and 2 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 7 | 01, 02, 03, 04, 05, 06, 07 |

---

## Test Scenarios

### 🔴 [Scenario 01] `properties()` on a node
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (p:Person)
      RETURN properties(p) AS m
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

### 🔴 [Scenario 02] `properties()` on a relationship
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r:R]->()
      RETURN properties(r) AS m
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

### 🔴 [Scenario 03] `properties()` on null
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
OPTIONAL MATCH (n:DoesNotExist)
      OPTIONAL MATCH (n)-[r:NOT_THERE]->()
      RETURN properties(n), properties(r), properties(null)
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

### 🔴 [Scenario 04] `properties()` on a map
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN properties({name: 'Popeye', level: 9001}) AS m
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

### 🔴 [Scenario 05] `properties()` failing on an integer literal
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN properties(1)
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

### 🔴 [Scenario 06] `properties()` failing on a string literal
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN properties('Cypher')
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

### 🔴 [Scenario 07] `properties()` failing on a list of booleans
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN properties([true, false])
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
**Goal:** Implement basic Graph9 functionality
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

- Feature file: `tck/features/expressions/graph/Graph9.feature`
- Total scenarios: 7
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
tck/features/expressions/graph/Graph9.feature
```
