# WithWhere4 - Non-Equi-Joins on variables - Task List

**Feature:** WithWhere4 - Non-Equi-Joins on variables
**Status:** 🔴 NOT STARTED (0/2 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Non-Equi-Joins on variables

**Feature File:** `tck/features/clauses/with-where/WithWhere4.feature`
**Total Scenarios:** 2

**Key Capabilities:**
- Join nodes on inequality
- Join with disjunctive multi-part predicates including patterns

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 2 | 01, 02 |

---

## Test Scenarios

### 🔴 [Scenario 01] Join nodes on inequality
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a), (b)
      WITH a, b
      WHERE a <> b
      RETURN a, b
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

### 🔴 [Scenario 02] Join with disjunctive multi-part predicates including patterns
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a), (b)
      WITH a, b
      WHERE a.id = 0
        AND (a)-[:T]->(b:TheLabel)
        OR (a)-[:T*]->(b:MissingLabel)
      RETURN DISTINCT b
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
**Goal:** Implement basic WithWhere4 functionality
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

- Feature file: `tck/features/clauses/with-where/WithWhere4.feature`
- Total scenarios: 2
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
tck/features/clauses/with-where/WithWhere4.feature
```
