# ReturnOrderBy6 - Aggregation expressions in order by - Task List

**Feature:** ReturnOrderBy6 - Aggregation expressions in order by
**Status:** 🔴 NOT STARTED (0/5 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Aggregation expressions in order by

**Feature File:** `tck/features/clauses/return-orderby/ReturnOrderBy6.feature`
**Total Scenarios:** 5

**Key Capabilities:**
- Handle constants and parameters inside an order by item which contains an aggregation expression
- Handle returned aliases inside an order by item which contains an aggregation expression
- Handle returned property accesses inside an order by item which contains an aggregation expression
- Fail if not returned variables are used inside an order by item which contains an aggregation expression
- Fail if more complex expressions, even if returned, are used inside an order by item which contains an aggregation expression

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 5 | 01, 02, 03, 04, 05 |

---

## Test Scenarios

### 🔴 [Scenario 01] Handle constants and parameters inside an order by item which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (person)
      RETURN avg(person.age) AS avgAge
      ORDER BY $age + avg(person.age) - 1000
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

### 🔴 [Scenario 02] Handle returned aliases inside an order by item which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      RETURN me.age AS age, count(you.age) AS cnt
      ORDER BY age, age + count(you.age)
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

### 🔴 [Scenario 03] Handle returned property accesses inside an order by item which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      RETURN me.age AS age, count(you.age) AS cnt
      ORDER BY me.age + count(you.age)
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

### 🔴 [Scenario 04] Fail if not returned variables are used inside an order by item which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      RETURN count(you.age) AS agg
      ORDER BY me.age + count(you.age)
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

### 🔴 [Scenario 05] Fail if more complex expressions, even if returned, are used inside an order by item which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      RETURN me.age + you.age, count(*) AS cnt
      ORDER BY me.age + you.age + count(*)
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
**Goal:** Implement basic ReturnOrderBy6 functionality
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

- Feature file: `tck/features/clauses/return-orderby/ReturnOrderBy6.feature`
- Total scenarios: 5
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_return-orderby
```

**Feature File Location:**
```
tck/features/clauses/return-orderby/ReturnOrderBy6.feature
```
