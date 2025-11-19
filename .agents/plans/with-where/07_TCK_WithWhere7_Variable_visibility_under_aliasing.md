# WithWhere7 - Variable visibility under aliasing - Task List

**Feature:** WithWhere7 - Variable visibility under aliasing
**Status:** 🔴 NOT STARTED (0/3 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Variable visibility under aliasing

**Feature File:** `tck/features/clauses/with-where/WithWhere7.feature`
**Total Scenarios:** 3

**Key Capabilities:**
- WHERE sees a variable bound before but not after WITH
- WHERE sees a variable bound after but not before WITH
- WHERE sees both, variable bound before but not after WITH and variable bound after but not before WITH

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 3 | 01, 02, 03 |

---

## Test Scenarios

### 🔴 [Scenario 01] WHERE sees a variable bound before but not after WITH
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a)
      WITH a.name2 AS name
      WHERE a.name2 = 'B'
      RETURN *
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

### 🔴 [Scenario 02] WHERE sees a variable bound after but not before WITH
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a)
      WITH a.name2 AS name
      WHERE name = 'B'
      RETURN *
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

### 🔴 [Scenario 03] WHERE sees both, variable bound before but not after WITH and variable bound after but not before WITH
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a)
      WITH a.name2 AS name
      WHERE name = 'B' OR a.name2 = 'C'
      RETURN *
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
**Goal:** Implement basic WithWhere7 functionality
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

- Feature file: `tck/features/clauses/with-where/WithWhere7.feature`
- Total scenarios: 3
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_with-where
```

**Feature File Location:**
```
tck/features/clauses/with-where/WithWhere7.feature
```
