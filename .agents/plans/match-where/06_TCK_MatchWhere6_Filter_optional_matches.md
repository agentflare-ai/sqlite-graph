# MatchWhere6 - Filter optional matches - Task List

**Feature:** MatchWhere6 - Filter optional matches
**Status:** 🔴 NOT STARTED (0/8 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Filter optional matches

**Feature File:** `tck/features/clauses/match-where/MatchWhere6.feature`
**Total Scenarios:** 8

**Key Capabilities:**
- Filter node with node label predicate on multi variables with multiple bindings after MATCH and OPTIONAL MATCH
- Filter node with false node label predicate after OPTIONAL MATCH
- Filter node with property predicate on multi variables with multiple bindings after OPTIONAL MATCH
- Do not fail when predicates on optionally matched and missed nodes are invalid
- Matching and optionally matching with unbound nodes and equality predicate in reverse direction
- ... and 3 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 8 | 01, 02, 03, 04, 05, 06, 07, 08 |

---

## Test Scenarios

### 🔴 [Scenario 01] Filter node with node label predicate on multi variables with multiple bindings after MATCH and OPTIONAL MATCH
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a)-->(b)
      WHERE b:B
      OPTIONAL MATCH (a)-->(c)
      WHERE c:C
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

### 🔴 [Scenario 02] Filter node with false node label predicate after OPTIONAL MATCH
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:Single)
      OPTIONAL MATCH (n)-[r]-(m)
      WHERE m:NonExistent
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

### 🔴 [Scenario 03] Filter node with property predicate on multi variables with multiple bindings after OPTIONAL MATCH
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:Single)
      OPTIONAL MATCH (n)-[r]-(m)
      WHERE m.num = 42
      RETURN m
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

### 🔴 [Scenario 04] Do not fail when predicates on optionally matched and missed nodes are invalid
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)-->(x0)
      OPTIONAL MATCH (x0)-->(x1)
      WHERE x1.name = 'bar'
      RETURN x0.name
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

### 🔴 [Scenario 05] Matching and optionally matching with unbound nodes and equality predicate in reverse direction
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a1)-[r]->()
      WITH r, a1
        LIMIT 1
      OPTIONAL MATCH (a2)<-[r]-(b2)
      WHERE a1 = a2
      RETURN a1, r, b2, a2
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

### 🔴 [Scenario 06] Join nodes on non-equality of properties – OPTIONAL MATCH and WHERE
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (x:X)
      OPTIONAL MATCH (x)-[:E1]->(y:Y)
      WHERE x.val < y.val
      RETURN x, y
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

### 🔴 [Scenario 07] Join nodes on non-equality of properties – OPTIONAL MATCH on two relationships and WHERE
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (x:X)
      OPTIONAL MATCH (x)-[:E1]->(y:Y)-[:E2]->(z:Z)
      WHERE x.val < z.val
      RETURN x, y, z
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

### 🔴 [Scenario 08] Join nodes on non-equality of properties – Two OPTIONAL MATCH clauses and WHERE
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (x:X)
      OPTIONAL MATCH (x)-[:E1]->(y:Y)
      OPTIONAL MATCH (y)-[:E2]->(z:Z)
      WHERE x.val < z.val
      RETURN x, y, z
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
**Goal:** Implement basic MatchWhere6 functionality
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

- Feature file: `tck/features/clauses/match-where/MatchWhere6.feature`
- Total scenarios: 8
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_match-where
```

**Feature File Location:**
```
tck/features/clauses/match-where/MatchWhere6.feature
```
