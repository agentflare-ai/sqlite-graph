# Call5 - Results projection - Task List

**Feature:** Call5 - Results projection
**Status:** 🔴 NOT STARTED (0/6 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Results projection

**Feature File:** `tck/features/clauses/call/Call5.feature`
**Total Scenarios:** 6

**Key Capabilities:**
- Explicit procedure result projection
- Explicit procedure result projection with RETURN *
- Fail on renaming to an already bound variable name
- Fail on renaming all outputs to the same variable name
- Fail on in-query call to procedure with YIELD *
- ... and 1 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 6 | 01, 02, 05, 06, 07, 08 |

---

## Test Scenarios

### 🔴 [Scenario 01] Explicit procedure result projection
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(null) YIELD out
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

### 🔴 [Scenario 02] Explicit procedure result projection with RETURN *
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(null) YIELD <rename>
      RETURN <a>, <b>
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

### 🔴 [Scenario 05] Fail on renaming to an already bound variable name
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(null) YIELD a, b AS a
      RETURN a
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

### 🔴 [Scenario 06] Fail on renaming all outputs to the same variable name
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc(null) YIELD a AS c, b AS c
      RETURN c
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

### 🔴 [Scenario 07] Fail on in-query call to procedure with YIELD *
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc('Stefan', 1) YIELD *
      RETURN city, country_code
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

### 🔴 [Scenario 08] Allow standalone call to procedure with YIELD *
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
CALL test.my.proc('Stefan', 1) YIELD *
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
**Goal:** Implement basic Call5 functionality
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

- Feature file: `tck/features/clauses/call/Call5.feature`
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
tck/features/clauses/call/Call5.feature
```
