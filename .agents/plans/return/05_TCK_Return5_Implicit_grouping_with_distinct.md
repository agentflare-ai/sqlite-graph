# Return5 - Implicit grouping with distinct - Task List

**Feature:** Return5 - Implicit grouping with distinct
**Status:** 🔴 NOT STARTED (0/5 scenarios)
**Priority:** HIGH
**Last Updated:** 2025-11-17

---

## Overview

Implicit grouping with distinct

**Feature File:** `tck/features/clauses/return/Return5.feature`
**Total Scenarios:** 5

**Key Capabilities:**
- DISTINCT inside aggregation should work with lists in maps
- DISTINCT on nullable values
- DISTINCT inside aggregation should work with nested lists in maps
- DISTINCT inside aggregation should work with nested lists of maps in maps
- Aggregate on list values

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 5 | 01, 02, 03, 04, 05 |

---

## Test Scenarios

### 🔴 [Scenario 01] DISTINCT inside aggregation should work with lists in maps
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      RETURN count(DISTINCT {name: n.list}) AS count
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

### 🔴 [Scenario 02] DISTINCT on nullable values
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      RETURN DISTINCT n.name
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

### 🔴 [Scenario 03] DISTINCT inside aggregation should work with nested lists in maps
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      RETURN count(DISTINCT {name: [[n.list, n.list], [n.list, n.list]]}) AS count
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

### 🔴 [Scenario 04] DISTINCT inside aggregation should work with nested lists of maps in maps
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      RETURN count(DISTINCT {name: [{name2: n.list}, {baz: {apa: n.list}}]}) AS count
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

### 🔴 [Scenario 05] Aggregate on list values
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN DISTINCT a.color, count(*)
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
**Goal:** Implement basic Return5 functionality
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

- Feature file: `tck/features/clauses/return/Return5.feature`
- Total scenarios: 5
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_return
```

**Feature File Location:**
```
tck/features/clauses/return/Return5.feature
```
