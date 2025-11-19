# CountingSubgraphMatches1 - Matching subgraph patterns and count the number of matches - Task List

**Feature:** CountingSubgraphMatches1 - Matching subgraph patterns and count the number of matches
**Status:** 🔴 NOT STARTED (0/11 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Matching subgraph patterns and count the number of matches

**Feature File:** `tck/features/useCases/countingSubgraphMatches/CountingSubgraphMatches1.feature`
**Total Scenarios:** 11

**Key Capabilities:**
- Undirected match in self-relationship graph, count
- Undirected match of self-relationship in self-relationship graph, count
- Undirected match on simple relationship graph, count
- Directed match on self-relationship graph, count
- Directed match of self-relationship on self-relationship graph, count
- ... and 6 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 11 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11 |

---

## Test Scenarios

### 🔴 [Scenario 01] Undirected match in self-relationship graph, count
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()--()
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

### 🔴 [Scenario 02] Undirected match of self-relationship in self-relationship graph, count
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)--(n)
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

### 🔴 [Scenario 03] Undirected match on simple relationship graph, count
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()--()
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

### 🔴 [Scenario 04] Directed match on self-relationship graph, count
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-->()
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

### 🔴 [Scenario 05] Directed match of self-relationship on self-relationship graph, count
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)-->(n)
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

### 🔴 [Scenario 06] Counting undirected self-relationships in self-relationship graph
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)-[r]-(n)
      RETURN count(r)
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

### 🔴 [Scenario 07] Counting distinct undirected self-relationships in self-relationship graph
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)-[r]-(n)
      RETURN count(DISTINCT r)
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

### 🔴 [Scenario 08] Directed match of a simple relationship, count
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-->()
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

### 🔴 [Scenario 09] Counting directed self-relationships
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)-[r]->(n)
      RETURN count(r)
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

### 🔴 [Scenario 10] Mixing directed and undirected pattern parts with self-relationship, count
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (:A)-->()--()
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

### 🔴 [Scenario 11] Mixing directed and undirected pattern parts with self-relationship, undirected count
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[]-()-[]-()
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

## Implementation Plan

### Phase 1: Foundation
**Goal:** Implement basic CountingSubgraphMatches1 functionality
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

- Feature file: `tck/features/useCases/countingSubgraphMatches/CountingSubgraphMatches1.feature`
- Total scenarios: 11
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_usecases_countingSubgraphMatches
```

**Feature File Location:**
```
tck/features/useCases/countingSubgraphMatches/CountingSubgraphMatches1.feature
```
