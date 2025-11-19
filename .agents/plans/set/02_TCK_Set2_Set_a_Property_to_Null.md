# Set2 - Set a Property to Null - Task List

**Feature:** Set2 - Set a Property to Null
**Status:** 🔴 NOT STARTED (0/3 scenarios)
**Priority:** MEDIUM
**Last Updated:** 2025-11-17

---

## Overview

Set a Property to Null

**Feature File:** `tck/features/clauses/set/Set2.feature`
**Total Scenarios:** 3

**Key Capabilities:**
- Setting a node property to null removes the existing property
- Setting a node property to null removes the existing property, but not before SET
- Setting a relationship property to null removes the existing property

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 3 | 01, 02, 03 |

---

## Test Scenarios

### 🔴 [Scenario 01] Setting a node property to null removes the existing property
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:A)
      SET n.property1 = null
      RETURN n
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

### 🔴 [Scenario 02] Setting a node property to null removes the existing property, but not before SET
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n)
      WHERE n.name = 'Michael'
      SET n.name = null
      RETURN n
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

### 🔴 [Scenario 03] Setting a relationship property to null removes the existing property
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH ()-[r]->()
      SET r.property1 = null
      RETURN r
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
**Goal:** Implement basic Set2 functionality
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

- Feature file: `tck/features/clauses/set/Set2.feature`
- Total scenarios: 3
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_set
```

**Feature File Location:**
```
tck/features/clauses/set/Set2.feature
```
