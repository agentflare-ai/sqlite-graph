# Set6 - Persistence of set clause side effects - Task List

**Feature:** Set6 - Persistence of set clause side effects
**Status:** 🔴 NOT STARTED (0/21 scenarios)
**Priority:** MEDIUM
**Last Updated:** 2025-11-17

---

## Overview

Persistence of set clause side effects

**Feature File:** `tck/features/clauses/set/Set6.feature`
**Total Scenarios:** 21

**Key Capabilities:**
- Limiting to zero results after setting a property on nodes affects the result set but not the side effects
- Skipping all results after setting a property on nodes affects the result set but not the side effects
- Skipping and limiting to a few results after setting a property on nodes affects the result set but not the side effects
- Skipping zero results and limiting to all results after setting a property on nodes does not affect the result set nor the side effects
- Filtering after setting a property on nodes affects the result set but not the side effects
- ... and 16 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 21 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 |

---

## Test Scenarios

### 🔴 [Scenario 01] Limiting to zero results after setting a property on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n.num = 43
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

### 🔴 [Scenario 02] Skipping all results after setting a property on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n.num = 43
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

### 🔴 [Scenario 03] Skipping and limiting to a few results after setting a property on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n.num = 42
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

### 🔴 [Scenario 04] Skipping zero results and limiting to all results after setting a property on nodes does not affect the result set nor the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n.num = 42
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

### 🔴 [Scenario 05] Filtering after setting a property on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n.num = n.num + 1
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

### 🔴 [Scenario 06] Aggregating in `RETURN` after setting a property on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n.num = n.num + 1
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

### 🔴 [Scenario 07] Aggregating in `WITH` after setting a property on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n.num = n.num + 1
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

### 🔴 [Scenario 08] Limiting to zero results after adding a label on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n:Foo
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

### 🔴 [Scenario 09] Skipping all results after adding a label on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n:Foo
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

### 🔴 [Scenario 10] Skipping and limiting to a few results after adding a label on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n:Foo
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

### 🔴 [Scenario 11] Skipping zero result and limiting to all results after adding a label on nodes does not affect the result set nor the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n:Foo
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

### 🔴 [Scenario 12] Filtering after adding a label on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n:Foo
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

### 🔴 [Scenario 13] Aggregating in `RETURN` after adding a label on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n:Foo
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

### 🔴 [Scenario 14] Aggregating in `WITH` after adding a label on nodes affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:N)
      SET n:Foo
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

### 🔴 [Scenario 15] Limiting to zero results after setting a property on relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH ()-[r:R]->()
      SET r.num = 43
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

### 🔴 [Scenario 16] Skipping all results after setting a property on relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH ()-[r:R]->()
      SET r.num = 43
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

### 🔴 [Scenario 17] Skipping and limiting to a few results after setting a property on relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH ()-[r:R]->()
      SET r.num = 42
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

### 🔴 [Scenario 18] Skipping zero result and limiting to all results after setting a property on relationships does not affect the result set nor the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH ()-[r:R]->()
      SET r.num = 42
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

### 🔴 [Scenario 19] Filtering after setting a property on relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH ()-[r:R]->()
      SET r.num = r.num + 1
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

### 🔴 [Scenario 20] Aggregating in `RETURN` after setting a property on relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH ()-[r:R]->()
      SET r.num = r.num + 1
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

### 🔴 [Scenario 21] Aggregating in `WITH` after setting a property on relationships affects the result set but not the side effects
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH ()-[r:R]->()
      SET r.num = r.num + 1
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
**Goal:** Implement basic Set6 functionality
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

- Feature file: `tck/features/clauses/set/Set6.feature`
- Total scenarios: 21
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_set
```

**Feature File Location:**
```
tck/features/clauses/set/Set6.feature
```
