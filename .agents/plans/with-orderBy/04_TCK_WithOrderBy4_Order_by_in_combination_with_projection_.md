# WithOrderBy4 - Order by in combination with projection and aliasing - Task List

**Feature:** WithOrderBy4 - Order by in combination with projection and aliasing
**Status:** 🔴 NOT STARTED (0/20 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Order by in combination with projection and aliasing

**Feature File:** `tck/features/clauses/with-orderBy/WithOrderBy4.feature`
**Total Scenarios:** 20

**Key Capabilities:**
- Sort by a projected expression
- Sort by an alias of a projected expression
- Sort by two projected expressions with order priority being different than projection order
- Sort by one projected expression and one alias of a projected expression with order priority being different than projection order
- Sort by one alias of a projected expression and one projected expression with order priority being different than projection order
- ... and 15 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 20 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 |

---

## Test Scenarios

### 🔴 [Scenario 01] Sort by a projected expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a, a.num + a.num2 AS sum
        ORDER BY a.num + a.num2
        LIMIT 3
      RETURN a, sum
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

### 🔴 [Scenario 02] Sort by an alias of a projected expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a, a.num + a.num2 AS sum
        ORDER BY sum
        LIMIT 3
      RETURN a, sum
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

### 🔴 [Scenario 03] Sort by two projected expressions with order priority being different than projection order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a, a.num + a.num2 AS sum, a.num2 % 3 AS mod
        ORDER BY a.num2 % 3, a.num + a.num2
        LIMIT 3
      RETURN a, sum, mod
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

### 🔴 [Scenario 04] Sort by one projected expression and one alias of a projected expression with order priority being different than projection order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a, a.num + a.num2 AS sum, a.num2 % 3 AS mod
        ORDER BY a.num2 % 3, sum
        LIMIT 3
      RETURN a, sum, mod
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

### 🔴 [Scenario 05] Sort by one alias of a projected expression and one projected expression with order priority being different than projection order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a, a.num + a.num2 AS sum, a.num2 % 3 AS mod
        ORDER BY mod, a.num + a.num2
        LIMIT 3
      RETURN a, sum, mod
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

### 🔴 [Scenario 06] Sort by aliases of two projected expressions with order priority being different than projection order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a, a.num + a.num2 AS sum, a.num2 % 3 AS mod
        ORDER BY mod, sum
        LIMIT 3
      RETURN a, sum, mod
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

### 🔴 [Scenario 07] Sort by an alias of a projected expression where the alias shadows an existing variable
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a, a.num2 % 3 AS x
      WITH a, a.num + a.num2 AS x
        ORDER BY x
        LIMIT 3
      RETURN a, x
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

### 🔴 [Scenario 08] Sort by non-projected existing variable
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a, a.num + a.num2 AS sum
      WITH a, a.num2 % 3 AS mod
        ORDER BY sum
        LIMIT 3
      RETURN a, mod
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

### 🔴 [Scenario 09] Sort by an alias of a projected expression containing the variable shadowed by the alias
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a.num2 AS x
      WITH x % 3 AS x
        ORDER BY x
        LIMIT 3
      RETURN x
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

### 🔴 [Scenario 10] Sort by a non-projected expression containing an alias of a projected expression containing the variable shadowed by the alias
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a.num2 AS x
      WITH x % 3 AS x
        ORDER BY x * -1
        LIMIT 3
      RETURN x
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

### 🔴 [Scenario 11] Sort by an aggregate projection
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a.num2 % 3 AS mod, sum(a.num + a.num2) AS sum
        ORDER BY sum(a.num + a.num2)
        LIMIT 2
      RETURN mod, sum
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

### 🔴 [Scenario 12] Sort by an aliased aggregate projection
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a.num2 % 3 AS mod, sum(a.num + a.num2) AS sum
        ORDER BY sum
        LIMIT 2
      RETURN mod, sum
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

### 🔴 [Scenario 13] Fail on sorting by a non-projected aggregation on a variable
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a, a.num + a.num2 AS sum
      WITH a.num2 % 3 AS mod, min(sum) AS min
        ORDER BY sum(sum)
        LIMIT 2
      RETURN mod, min
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

### 🔴 [Scenario 14] Fail on sorting by a non-projected aggregation on an expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a.num2 % 3 AS mod, min(a.num + a.num2) AS min
        ORDER BY sum(a.num + a.num2)
        LIMIT 2
      RETURN mod, min
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

### 🔴 [Scenario 15] Sort by an aliased aggregate projection does allow subsequent matching
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a)-[r]->(b:X)
      WITH a, r, b, count(*) AS c
        ORDER BY c
      MATCH (a)-[r]->(b)
      RETURN r AS rel
        ORDER BY rel.id
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

### 🔴 [Scenario 16] Handle constants and parameters inside an order by item which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (person)
      WITH avg(person.age) AS avgAge
      ORDER BY $age + avg(person.age) - 1000
      RETURN avgAge
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

### 🔴 [Scenario 17] Handle projected variables inside an order by item which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      WITH me.age AS age, count(you.age) AS cnt
      ORDER BY age, age + count(you.age)
      RETURN age
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

### 🔴 [Scenario 18] Handle projected property accesses inside an order by item which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      WITH me.age AS age, count(you.age) AS cnt
      ORDER BY me.age + count(you.age)
      RETURN age
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

### 🔴 [Scenario 19] Fail if not projected variables are used inside an order by item which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      WITH count(you.age) AS agg
      ORDER BY me.age + count(you.age)
      RETURN *
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

### 🔴 [Scenario 20] Fail if more complex expressions, even if projected, are used inside an order by item which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      WITH me.age + you.age, count(*) AS cnt
      ORDER BY me.age + you.age + count(*)
      RETURN *
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
**Goal:** Implement basic WithOrderBy4 functionality
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

- Feature file: `tck/features/clauses/with-orderBy/WithOrderBy4.feature`
- Total scenarios: 20
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_with-orderBy
```

**Feature File Location:**
```
tck/features/clauses/with-orderBy/WithOrderBy4.feature
```
