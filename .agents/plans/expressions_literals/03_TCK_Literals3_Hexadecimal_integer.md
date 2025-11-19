# Literals3 - Hexadecimal integer - Task List

**Feature:** Literals3 - Hexadecimal integer
**Status:** 🔴 NOT STARTED (0/16 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Hexadecimal integer

**Feature File:** `tck/features/expressions/literals/Literals3.feature`
**Total Scenarios:** 16

**Key Capabilities:**
- Return a short positive hexadecimal integer
- Return a long positive hexadecimal integer
- Return the largest hexadecimal integer
- Return a positive hexadecimal zero
- Return a negative hexadecimal zero
- ... and 11 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 16 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 16, 17 |

---

## Test Scenarios

### 🔴 [Scenario 01] Return a short positive hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0x1 AS literal
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

### 🔴 [Scenario 02] Return a long positive hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0x162CD4F6 AS literal
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

### 🔴 [Scenario 03] Return the largest hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0x7FFFFFFFFFFFFFFF AS literal
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

### 🔴 [Scenario 04] Return a positive hexadecimal zero
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0x0 AS literal
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

### 🔴 [Scenario 05] Return a negative hexadecimal zero
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -0x0 AS literal
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

### 🔴 [Scenario 06] Return a short negative hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -0x1 AS literal
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

### 🔴 [Scenario 07] Return a long negative hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -0x162CD4F6 AS literal
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

### 🔴 [Scenario 08] Return the smallest hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -0x8000000000000000 AS literal
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

### 🔴 [Scenario 09] Return a lower case hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0x1a2b3c4d5e6f7 AS literal
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

### 🔴 [Scenario 10] Return a upper case hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0x1A2B3C4D5E6F7 AS literal
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

### 🔴 [Scenario 11] Return a mixed case hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0x1A2b3c4D5E6f7 AS literal
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

### 🔴 [Scenario 12] Fail on an incomplete hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0x AS literal
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

### 🔴 [Scenario 13] Fail on an hexadecimal literal containing a lower case invalid alphanumeric character
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0x1A2b3j4D5E6f7 AS literal
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

### 🔴 [Scenario 14] Fail on an hexadecimal literal containing a upper case invalid alphanumeric character
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
#      RETURN 0x1A2b3c4#5E6f7 AS literal
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

### 🔴 [Scenario 16] Fail on a too large hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0x8000000000000000 AS literal
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

### 🔴 [Scenario 17] Fail on a too small hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -0x8000000000000001 AS literal
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
**Goal:** Implement basic Literals3 functionality
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

- Feature file: `tck/features/expressions/literals/Literals3.feature`
- Total scenarios: 16
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_expressions_literals
```

**Feature File Location:**
```
tck/features/expressions/literals/Literals3.feature
```
