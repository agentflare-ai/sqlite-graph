# ReturnOrderBy2 - Order by a single expression (order of projection) - Task List

**Feature:** ReturnOrderBy2 - Order by a single expression (order of projection)
**Status:** 🔴 NOT STARTED (0/14 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Order by a single expression (order of projection)

**Feature File:** `tck/features/clauses/return-orderby/ReturnOrderBy2.feature`
**Total Scenarios:** 14

**Key Capabilities:**
- ORDER BY should return results in ascending order
- ORDER BY DESC should return results in descending order
- Sort on aggregated function
- Support sort and distinct
- Support ordering by a property after being distinct-ified
- ... and 9 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 14 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14 |

---

## Test Scenarios

### 🔴 [Scenario 01] ORDER BY should return results in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      RETURN n.num AS prop
      ORDER BY n.num
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

### 🔴 [Scenario 02] ORDER BY DESC should return results in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      RETURN n.num AS prop
      ORDER BY n.num DESC
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

### 🔴 [Scenario 03] Sort on aggregated function
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      RETURN n.division, max(n.age)
        ORDER BY max(n.age)
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

### 🔴 [Scenario 04] Support sort and distinct
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a)
      RETURN DISTINCT a
        ORDER BY a.name
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

### 🔴 [Scenario 05] Support ordering by a property after being distinct-ified
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a)-->(b)
      RETURN DISTINCT b
        ORDER BY b.name
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

### 🔴 [Scenario 06] Count star should count everything in scope
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a)
      RETURN a, count(*)
      ORDER BY count(*)
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

### 🔴 [Scenario 07] Ordering with aggregation
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      RETURN n.name, count(*) AS foo
        ORDER BY n.name
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

### 🔴 [Scenario 08] Returning all variables with ordering
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      RETURN *
        ORDER BY n.id
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

### 🔴 [Scenario 09] Using aliased DISTINCT expression in ORDER BY
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      RETURN DISTINCT n.id AS id
        ORDER BY id DESC
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

### 🔴 [Scenario 10] Returned columns do not change from using ORDER BY
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      RETURN DISTINCT n
        ORDER BY n.id
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

### 🔴 [Scenario 11] Aggregates ordered by arithmetics
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A), (b:X)
      RETURN count(a) * 10 + count(b) * 5 AS x
      ORDER BY x
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

### 🔴 [Scenario 12] Aggregation of named paths
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH p = (a)-[*]->(b)
      RETURN collect(nodes(p)) AS paths, length(p) AS l
      ORDER BY l
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

### 🔴 [Scenario 13] Fail when sorting on variable removed by DISTINCT
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a)
      RETURN DISTINCT a.name
        ORDER BY a.age
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

### 🔴 [Scenario 14] Fail on aggregation in ORDER BY after RETURN
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      RETURN n.num1
        ORDER BY max(n.num2)
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
**Goal:** Implement basic ReturnOrderBy2 functionality
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

- Feature file: `tck/features/clauses/return-orderby/ReturnOrderBy2.feature`
- Total scenarios: 14
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
tck/features/clauses/return-orderby/ReturnOrderBy2.feature
```
