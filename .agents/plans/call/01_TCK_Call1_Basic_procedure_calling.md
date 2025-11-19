# Call1 - Basic procedure calling - Task List

**Feature:** Call1 - Basic procedure calling
**Status:** 🔴 NOT STARTED (0/16 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Basic procedure calling

**Feature File:** `tck/features/clauses/call/Call1.feature`
**Total Scenarios:** 16

**Key Capabilities:**
- Standalone call to procedure that takes no arguments and yields no results
- Standalone call to procedure that takes no arguments and yields no results, called with implicit arguments
- In-query call to procedure that takes no arguments and yields no results
- In-query call to procedure that takes no arguments and yields no results and consumes no rows
- Standalone call to STRING procedure that takes no arguments
- ... and 11 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 16 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16 |

---

## Test Scenarios

### 🔴 [Scenario 01] Standalone call to procedure that takes no arguments and yields no results
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.doNothing()
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

### 🔴 [Scenario 02] Standalone call to procedure that takes no arguments and yields no results, called with implicit arguments
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.doNothing
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

### 🔴 [Scenario 03] In-query call to procedure that takes no arguments and yields no results
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      CALL test.doNothing()
      RETURN n
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

### 🔴 [Scenario 04] In-query call to procedure that takes no arguments and yields no results and consumes no rows
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      CALL test.doNothing()
      RETURN n.name AS `name`
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

### 🔴 [Scenario 05] Standalone call to STRING procedure that takes no arguments
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.labels()
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

### 🔴 [Scenario 06] In-query call to STRING procedure that takes no arguments
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.labels() YIELD label
      RETURN label
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

### 🔴 [Scenario 07] Standalone call to procedure should fail if explicit argument is missing
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc('Dobby')
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

### 🔴 [Scenario 08] In-query call to procedure should fail if explicit argument is missing
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc('Dobby') YIELD out
      RETURN out
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

### 🔴 [Scenario 09] Standalone call to procedure should fail if too many explicit argument are given
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(1, 2, 3, 4)
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

### 🔴 [Scenario 10] In-query call to procedure should fail if too many explicit argument are given
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(1, 2, 3, 4) YIELD out
      RETURN out
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

### 🔴 [Scenario 11] Standalone call to procedure should fail if implicit argument is missing
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc
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

### 🔴 [Scenario 12] In-query call to procedure that has outputs fails if no outputs are yielded
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(1)
      RETURN out
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

### 🔴 [Scenario 13] Standalone call to unknown procedure should fail
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc
```

**Expected:** ProcedureError

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Error handling infrastructure
**Estimated Effort:** TBD

---

### 🔴 [Scenario 14] In-query call to unknown procedure should fail
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc() YIELD out
      RETURN out
```

**Expected:** ProcedureError

**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

**Dependencies:** Error handling infrastructure
**Estimated Effort:** TBD

---

### 🔴 [Scenario 15] In-query procedure call should fail if shadowing an already bound variable
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH 'Hi' AS label
      CALL test.labels() YIELD label
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

### 🔴 [Scenario 16] In-query procedure call should fail if one of the argument expressions uses an aggregation function
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n)
      CALL test.labels(count(n)) YIELD label
      RETURN label
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
**Goal:** Implement basic Call1 functionality
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

- Feature file: `tck/features/clauses/call/Call1.feature`
- Total scenarios: 16
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_call
```

**Feature File Location:**
```
tck/features/clauses/call/Call1.feature
```
