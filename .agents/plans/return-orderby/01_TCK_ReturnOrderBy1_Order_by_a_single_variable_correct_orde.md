# ReturnOrderBy1 - Order by a single variable (correct order of values according to their type) - Task List

**Feature:** ReturnOrderBy1 - Order by a single variable (correct order of values according to their type)
**Status:** 🔴 NOT STARTED (0/12 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Order by a single variable (correct order of values according to their type)

**Feature File:** `tck/features/clauses/return-orderby/ReturnOrderBy1.feature`
**Total Scenarios:** 12

**Key Capabilities:**
- ORDER BY should order booleans in the expected order
- ORDER BY DESC should order booleans in the expected order
- ORDER BY should order strings in the expected order
- ORDER BY DESC should order strings in the expected order
- ORDER BY should order ints in the expected order
- ... and 7 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 12 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12 |

---

## Test Scenarios

### 🔴 [Scenario 01] ORDER BY should order booleans in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false] AS bools
      RETURN bools
      ORDER BY bools
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

### 🔴 [Scenario 02] ORDER BY DESC should order booleans in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false] AS bools
      RETURN bools
      ORDER BY bools DESC
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

### 🔴 [Scenario 03] ORDER BY should order strings in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND ['.*', '', ' ', 'one'] AS strings
      RETURN strings
      ORDER BY strings
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

### 🔴 [Scenario 04] ORDER BY DESC should order strings in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND ['.*', '', ' ', 'one'] AS strings
      RETURN strings
      ORDER BY strings DESC
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

### 🔴 [Scenario 05] ORDER BY should order ints in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [1, 3, 2] AS ints
      RETURN ints
      ORDER BY ints
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

### 🔴 [Scenario 06] ORDER BY DESC should order ints in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [1, 3, 2] AS ints
      RETURN ints
      ORDER BY ints DESC
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

### 🔴 [Scenario 07] ORDER BY should order floats in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [1.5, 1.3, 999.99] AS floats
      RETURN floats
      ORDER BY floats
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

### 🔴 [Scenario 08] ORDER BY DESC should order floats in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [1.5, 1.3, 999.99] AS floats
      RETURN floats
      ORDER BY floats DESC
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

### 🔴 [Scenario 09] ORDER BY should order lists in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [[], ['a'], ['a', 1], [1], [1, 'a'], [1, null], [null, 1], [null, 2]] AS lists
      RETURN lists
      ORDER BY lists
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

### 🔴 [Scenario 10] ORDER BY DESC should order lists in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [[], ['a'], ['a', 1], [1], [1, 'a'], [1, null], [null, 1], [null, 2]] AS lists
      RETURN lists
      ORDER BY lists DESC
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

### 🔴 [Scenario 11] ORDER BY should order distinct types in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH p = (n:N)-[r:REL]->()
      UNWIND [n, r, p, 1.5, ['list'], 'text', null, false, 0.0 / 0.0, {a: 'map'}] AS types
      RETURN types
      ORDER BY types
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

### 🔴 [Scenario 12] ORDER BY DESC should order distinct types in the expected order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH p = (n:N)-[r:REL]->()
      UNWIND [n, r, p, 1.5, ['list'], 'text', null, false, 0.0 / 0.0, {a: 'map'}] AS types
      RETURN types
      ORDER BY types DESC
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
**Goal:** Implement basic ReturnOrderBy1 functionality
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

- Feature file: `tck/features/clauses/return-orderby/ReturnOrderBy1.feature`
- Total scenarios: 12
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
tck/features/clauses/return-orderby/ReturnOrderBy1.feature
```
