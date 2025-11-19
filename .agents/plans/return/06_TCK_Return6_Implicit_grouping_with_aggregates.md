# Return6 - Implicit grouping with aggregates - Task List

**Feature:** Return6 - Implicit grouping with aggregates
**Status:** 🔴 NOT STARTED (0/21 scenarios)
**Priority:** HIGH
**Last Updated:** 2025-11-17

---

## Overview

Implicit grouping with aggregates

**Feature File:** `tck/features/clauses/return/Return6.feature`
**Total Scenarios:** 21

**Key Capabilities:**
- Return count aggregation over nodes
- Projecting an arithmetic expression with aggregation
- Aggregating by a list property has a correct definition of equality
- Support multiple divisions in aggregate function
- Aggregates inside normal functions
- ... and 16 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 21 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 |

---

## Test Scenarios

### 🔴 [Scenario 01] Return count aggregation over nodes
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      RETURN n.num AS n, count(n) AS count
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

### 🔴 [Scenario 02] Projecting an arithmetic expression with aggregation
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN a, count(a) + 3
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

### 🔴 [Scenario 03] Aggregating by a list property has a correct definition of equality
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      WITH a.num AS a, count(*) AS count
      RETURN count
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

### 🔴 [Scenario 04] Support multiple divisions in aggregate function
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      RETURN count(n) / 60 / 60 AS count
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

### 🔴 [Scenario 05] Aggregates inside normal functions
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN size(collect(a))
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

### 🔴 [Scenario 06] Handle aggregates inside non-aggregate expressions
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a {name: 'Andres'})<-[:FATHER]-(child)
      RETURN a.name, {foo: a.name='Andres', kids: collect(child.name)}
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

### 🔴 [Scenario 07] Aggregate on property
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      RETURN n.num, count(*)
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

### 🔴 [Scenario 08] Handle aggregation on functions
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH p=(a:L)-[*]->(b)
      RETURN b, avg(length(p))
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

### 🔴 [Scenario 09] Aggregates with arithmetics
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH ()
      RETURN count(*) * 10 AS c
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

### 🔴 [Scenario 10] Multiple aggregates on same variable
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      RETURN count(n), collect(n)
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

### 🔴 [Scenario 11] Counting matches
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH ()
      RETURN count(*)
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

### 🔴 [Scenario 12] Counting matches per group
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a:L)-[rel]->(b)
      RETURN a, count(*)
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

### 🔴 [Scenario 13] Returning the minimum length of paths
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH p = (a:T {name: 'a'})-[:R*]->(other:T)
      WHERE other <> a
      WITH a, other, min(length(p)) AS len
      RETURN a.name AS name, collect(other.name) AS others, len
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

### 🔴 [Scenario 14] Aggregates in aggregates
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
RETURN count(count(*))
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

### 🔴 [Scenario 15] Using `rand()` in aggregations
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
RETURN count(rand())
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

### 🔴 [Scenario 16] Aggregation on complex expressions
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (me)-[r1:ATE]->()<-[r2:ATE]-(you)
      WHERE me.name = 'Michael'
      WITH me, count(DISTINCT r1) AS H1, count(DISTINCT r2) AS H2, you
      MATCH (me)-[r1:ATE]->()<-[r2:ATE]-(you)
      RETURN me, you, sum((1 - abs(r1.times / H1 - r2.times / H2)) * (r1.times + r2.times) / (H1 + H2)) AS sum
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

### 🔴 [Scenario 17] Handle constants and parameters inside an expression which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (person)
      RETURN $age + avg(person.age) - 1000
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

### 🔴 [Scenario 18] Handle returned variables inside an expression which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      WITH me.age AS age, you
      RETURN age, age + count(you.age)
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

### 🔴 [Scenario 19] Handle returned property accesses inside an expression which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      RETURN me.age, me.age + count(you.age)
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

### 🔴 [Scenario 20] Fail if not returned variables are used inside an expression which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      RETURN me.age + count(you.age)
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

### 🔴 [Scenario 21] Fail if more complex expressions, even if returned, are used inside expression which contains an aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (me: Person)--(you: Person)
      RETURN me.age + you.age, me.age + you.age + count(*)
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
**Goal:** Implement basic Return6 functionality
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

- Feature file: `tck/features/clauses/return/Return6.feature`
- Total scenarios: 21
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
tck/features/clauses/return/Return6.feature
```
