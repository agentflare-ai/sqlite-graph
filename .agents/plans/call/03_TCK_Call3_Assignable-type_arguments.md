# Call3 - Assignable-type arguments - Task List

**Feature:** Call3 - Assignable-type arguments
**Status:** 🔴 NOT STARTED (0/6 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Assignable-type arguments

**Feature File:** `tck/features/clauses/call/Call3.feature`
**Total Scenarios:** 6

**Key Capabilities:**
- Standalone call to procedure with argument of type NUMBER accepts value of type INTEGER
- In-query call to procedure with argument of type NUMBER accepts value of type INTEGER
- Standalone call to procedure with argument of type NUMBER accepts value of type FLOAT
- In-query call to procedure with argument of type NUMBER accepts value of type FLOAT
- Standalone call to procedure with argument of type FLOAT accepts value of type INTEGER
- ... and 1 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 6 | 01, 02, 03, 04, 05, 06 |

---

## Test Scenarios

### 🔴 [Scenario 01] Standalone call to procedure with argument of type NUMBER accepts value of type INTEGER
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(42)
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

### 🔴 [Scenario 02] In-query call to procedure with argument of type NUMBER accepts value of type INTEGER
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(42) YIELD out
      RETURN out
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

### 🔴 [Scenario 03] Standalone call to procedure with argument of type NUMBER accepts value of type FLOAT
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(42.3)
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

### 🔴 [Scenario 04] In-query call to procedure with argument of type NUMBER accepts value of type FLOAT
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(42.3) YIELD out
      RETURN out
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

### 🔴 [Scenario 05] Standalone call to procedure with argument of type FLOAT accepts value of type INTEGER
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(42)
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

### 🔴 [Scenario 06] In-query call to procedure with argument of type FLOAT accepts value of type INTEGER
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(42) YIELD out
      RETURN out
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
**Goal:** Implement basic Call3 functionality
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

- Feature file: `tck/features/clauses/call/Call3.feature`
- Total scenarios: 6
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
tck/features/clauses/call/Call3.feature
```
