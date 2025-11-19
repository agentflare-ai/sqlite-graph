# Remove3 - Persistence of remove clause side effects - Task List

**Feature:** Remove3 - Persistence of remove clause side effects
**Status:** 🔴 NOT STARTED (0/21 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Persistence of remove clause side effects

**Feature File:** `tck/features/clauses/remove/Remove3.feature`
**Total Scenarios:** 21

**Key Capabilities:**
- Limiting to zero results after removing a property from nodes affects the result set but not the side effects
- Skipping all results after removing a property from nodes affects the result set but not the side effects
- Skipping and limiting to a few results after removing a property from nodes affects the result set but not the side effects
- Skipping zero results and limiting to all results after removing a property from nodes does not affect the result set nor the side effects
- Filtering after removing a property from nodes affects the result set but not the side effects
- ... and 16 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 21 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 |

---

## Test Scenarios

### 🔴 [Scenario 01] Limiting to zero results after removing a property from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n.num
      RETURN n
      LIMIT 0
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

### 🔴 [Scenario 02] Skipping all results after removing a property from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n.num
      RETURN n
      SKIP 1
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

### 🔴 [Scenario 03] Skipping and limiting to a few results after removing a property from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n.name
      RETURN n.num AS num
      SKIP 2 LIMIT 2
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

### 🔴 [Scenario 04] Skipping zero results and limiting to all results after removing a property from nodes does not affect the result set nor the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n.name
      RETURN n.num AS num
      SKIP 0 LIMIT 5
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

### 🔴 [Scenario 05] Filtering after removing a property from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n.name
      WITH n
      WHERE n.num % 2 = 0
      RETURN n.num AS num
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

### 🔴 [Scenario 06] Aggregating in `RETURN` after removing a property from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n.name
      RETURN sum(n.num) AS sum
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

### 🔴 [Scenario 07] Aggregating in `WITH` after removing a property from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n.name
      WITH sum(n.num) AS sum
      RETURN sum
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

### 🔴 [Scenario 08] Limiting to zero results after removing a label from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n:N
      RETURN n
      LIMIT 0
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

### 🔴 [Scenario 09] Skipping all results after removing a label from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n:N
      RETURN n
      SKIP 1
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

### 🔴 [Scenario 10] Skipping and limiting to a few results after removing a label from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n:N
      RETURN n.num AS num
      SKIP 2 LIMIT 2
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

### 🔴 [Scenario 11] Skipping zero result and limiting to all results after removing a label from nodes does not affect the result set nor the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n:N
      RETURN n.num AS num
      SKIP 0 LIMIT 5
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

### 🔴 [Scenario 12] Filtering after removing a label from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n:N
      WITH n
      WHERE n.num % 2 = 0
      RETURN n.num AS num
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

### 🔴 [Scenario 13] Aggregating in `RETURN` after removing a label from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n:N
      RETURN sum(n.num) AS sum
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

### 🔴 [Scenario 14] Aggregating in `WITH` after removing a label from nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:N)
      REMOVE n:N
      WITH sum(n.num) AS sum
      RETURN sum
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

### 🔴 [Scenario 15] Limiting to zero results after removing a property from relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r:R]->()
      REMOVE r.num
      RETURN r
      LIMIT 0
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

### 🔴 [Scenario 16] Skipping all results after removing a property from relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r:R]->()
      REMOVE r.num
      RETURN r
      SKIP 1
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

### 🔴 [Scenario 17] Skipping and limiting to a few results after removing a property from relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r:R]->()
      REMOVE r.name
      RETURN r.num AS num
      SKIP 2 LIMIT 2
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

### 🔴 [Scenario 18] Skipping zero result and limiting to all results after removing a property from relationships does not affect the result set nor the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r:R]->()
      REMOVE r.name
      RETURN r.num AS num
      SKIP 0 LIMIT 5
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

### 🔴 [Scenario 19] Filtering after removing a property from relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r:R]->()
      REMOVE r.name
      WITH r
      WHERE r.num % 2 = 0
      RETURN r.num AS num
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

### 🔴 [Scenario 20] Aggregating in `RETURN` after removing a property from relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r:R]->()
      REMOVE r.name
      RETURN sum(r.num) AS sum
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

### 🔴 [Scenario 21] Aggregating in `WITH` after removing a property from relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r:R]->()
      REMOVE r.name
      WITH sum(r.num) AS sum
      RETURN sum
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
**Goal:** Implement basic Remove3 functionality
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

- Feature file: `tck/features/clauses/remove/Remove3.feature`
- Total scenarios: 21
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_remove
```

**Feature File Location:**
```
tck/features/clauses/remove/Remove3.feature
```
