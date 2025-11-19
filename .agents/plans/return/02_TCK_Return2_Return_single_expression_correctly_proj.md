# Return2 - Return single expression (correctly projecting an expression) - Task List

**Feature:** Return2 - Return single expression (correctly projecting an expression)
**Status:** 🔴 NOT STARTED (0/18 scenarios)
**Priority:** HIGH
**Last Updated:** 2025-11-17

---

## Overview

Return single expression (correctly projecting an expression)

**Feature File:** `tck/features/clauses/return/Return2.feature`
**Total Scenarios:** 18

**Key Capabilities:**
- Arithmetic expressions should propagate null values
- Returning a node property value
- Missing node property should become null
- Returning a relationship property value
- Missing relationship property should become null
- ... and 13 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 18 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18 |

---

## Test Scenarios

### 🔴 [Scenario 01] Arithmetic expressions should propagate null values
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
RETURN 1 + (2 - (3 * (4 / (5 ^ (6 % null))))) AS a
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

### 🔴 [Scenario 02] Returning a node property value
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN a.num
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

### 🔴 [Scenario 03] Missing node property should become null
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN a.name
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

### 🔴 [Scenario 04] Returning a relationship property value
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH ()-[r]->()
      RETURN r.num
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

### 🔴 [Scenario 05] Missing relationship property should become null
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH ()-[r]->()
      RETURN r.name2
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

### 🔴 [Scenario 06] Adding a property and a literal in projection
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN a.num + 1 AS foo
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

### 🔴 [Scenario 07] Adding list properties in projection
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN a.list2 + a.list1 AS foo
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

### 🔴 [Scenario 08] Returning label predicate expression
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      RETURN (n:Foo)
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

### 🔴 [Scenario 09] Returning a projected map
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
RETURN {a: 1, b: 'foo'}
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

### 🔴 [Scenario 10] Return count aggregation over an empty graph
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN count(a) > 0
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

### 🔴 [Scenario 11] RETURN does not lose precision on large integers
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (p:TheLabel)
      RETURN p.id
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

### 🔴 [Scenario 12] Projecting a list of nodes and relationships
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)-[r]->(m)
      RETURN [n, r, m] AS r
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

### 🔴 [Scenario 13] Projecting a map of nodes and relationships
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)-[r]->(m)
      RETURN {node1: n, rel: r, node2: m} AS m
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

### 🔴 [Scenario 14] Do not fail when returning type of deleted relationships
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH ()-[r]->()
      DELETE r
      RETURN type(r)
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

### 🔴 [Scenario 15] Fail when returning properties of deleted nodes
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      DELETE n
      RETURN n.num
```

**Expected:** 

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Basic query execution
**Estimated Effort:** TBD

---

### 🔴 [Scenario 16] Fail when returning labels of deleted nodes
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (n)
      DELETE n
      RETURN labels(n)
```

**Expected:** 

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Basic query execution
**Estimated Effort:** TBD

---

### 🔴 [Scenario 17] Fail when returning properties of deleted relationships
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH ()-[r]->()
      DELETE r
      RETURN r.num
```

**Expected:** 

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Basic query execution
**Estimated Effort:** TBD

---

### 🔴 [Scenario 18] Fail on projecting a non-existent function
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH (a)
      RETURN foo(a)
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
**Goal:** Implement basic Return2 functionality
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

- Feature file: `tck/features/clauses/return/Return2.feature`
- Total scenarios: 18
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
tck/features/clauses/return/Return2.feature
```
