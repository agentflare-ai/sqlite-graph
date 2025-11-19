# Return4 - Column renaming - Task List

**Feature:** Return4 - Column renaming
**Status:** 🔴 NOT STARTED (0/11 scenarios)
**Priority:** HIGH
**Last Updated:** 2025-11-17

---

## Overview

Column renaming

**Feature File:** `tck/features/clauses/return/Return4.feature`
**Total Scenarios:** 11

**Key Capabilities:**
- Honour the column name for RETURN items
- Support column renaming
- Aliasing expressions
- Keeping used expression 1
- Keeping used expression 2
- ... and 6 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 11 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11 |

---

## Test Scenarios

### 🔴 [Scenario 01] Honour the column name for RETURN items
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      WITH a.name AS a
      RETURN a
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

### 🔴 [Scenario 02] Support column renaming
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN a AS ColumnName
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

### 🔴 [Scenario 03] Aliasing expressions
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN a.id AS a, a.id
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

### 🔴 [Scenario 04] Keeping used expression 1
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      RETURN cOuNt( * )
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

### 🔴 [Scenario 05] Keeping used expression 2
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH p = (n)-->(b)
      RETURN nOdEs( p )
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

### 🔴 [Scenario 06] Keeping used expression 3
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH p = (n)-->(b)
      RETURN coUnt( dIstInct p )
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

### 🔴 [Scenario 07] Keeping used expression 4
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH p = (n)-->(b)
      RETURN aVg(    n.aGe     )
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

### 🔴 [Scenario 08] Support column renaming for aggregations
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH ()
      RETURN count(*) AS columnName
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

### 🔴 [Scenario 09] Handle subexpression in aggregation also occurring as standalone expression with nested aggregation in a literal map
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a:A), (b:B)
      RETURN coalesce(a.num, b.num) AS foo,
        b.num AS bar,
        {name: count(b)} AS baz
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

### 🔴 [Scenario 10] Fail when returning multiple columns with same name
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
RETURN 1 AS a, 2 AS a
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

### 🔴 [Scenario 11] Reusing variable names in RETURN
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (person:Person)<--(message)<-[like]-(:Person)
      WITH like.creationDate AS likeTime, person AS person
        ORDER BY likeTime, message.id
      WITH head(collect({likeTime: likeTime})) AS latestLike, person AS person
      RETURN latestLike.likeTime AS likeTime
        ORDER BY likeTime
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
**Goal:** Implement basic Return4 functionality
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

- Feature file: `tck/features/clauses/return/Return4.feature`
- Total scenarios: 11
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
tck/features/clauses/return/Return4.feature
```
