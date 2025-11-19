# Graph6 - Static property access - Task List

**Feature:** Graph6 - Static property access
**Status:** 🔴 NOT STARTED (0/8 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Static property access

**Feature File:** `tck/features/expressions/graph/Graph6.feature`
**Total Scenarios:** 8

**Key Capabilities:**
- Statically access a property of a non-null node
- Statically access a property of a optional non-null node
- Statically access a property of a null node
- Statically access a property of a node resulting from an expression
- Statically access a property of a non-null relationship
- ... and 3 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 8 | 01, 02, 03, 04, 05, 06, 07, 08 |

---

## Test Scenarios

### 🔴 [Scenario 01] Statically access a property of a non-null node
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      RETURN n.missing, n.missingToo, n.existing
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

### 🔴 [Scenario 02] Statically access a property of a optional non-null node
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
OPTIONAL MATCH (n)
      RETURN n.missing, n.missingToo, n.existing
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

### 🔴 [Scenario 03] Statically access a property of a null node
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
OPTIONAL MATCH (n)
      RETURN n.missing
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

### 🔴 [Scenario 04] Statically access a property of a node resulting from an expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      WITH [123, n] AS list
      RETURN (list[1]).missing, (list[1]).missingToo, (list[1]).existing
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

### 🔴 [Scenario 05] Statically access a property of a non-null relationship
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r]->()
      RETURN r.missing, r.missingToo, r.existing
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

### 🔴 [Scenario 06] Statically access a property of a optional non-null relationship
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
OPTIONAL MATCH ()-[r]->()
      RETURN r.missing, r.missingToo, r.existing
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

### 🔴 [Scenario 07] Statically access a property of a null relationship
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
OPTIONAL MATCH ()-[r]->()
      RETURN r.missing
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

### 🔴 [Scenario 08] Statically access a property of a relationship resulting from an expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH <exp> AS nonGraphElement
      RETURN nonGraphElement.num
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
**Goal:** Implement basic Graph6 functionality
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

- Feature file: `tck/features/expressions/graph/Graph6.feature`
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
tck/features/expressions/graph/Graph6.feature
```
