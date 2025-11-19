# Literals5 - Float - Task List

**Feature:** Literals5 - Float
**Status:** 🔴 NOT STARTED (0/27 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Float

**Feature File:** `tck/features/expressions/literals/Literals5.feature`
**Total Scenarios:** 27

**Key Capabilities:**
- Return a short positive float
- Return a short positive float without integer digits
- Return a long positive float
- Return a long positive float without integer digits
- Return a very long positive float
- ... and 22 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 27 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 |

---

## Test Scenarios

### 🔴 [Scenario 01] Return a short positive float
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1.0 AS literal
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

### 🔴 [Scenario 02] Return a short positive float without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN .1 AS literal
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

### 🔴 [Scenario 03] Return a long positive float
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 3985764.3405892687 AS literal
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

### 🔴 [Scenario 04] Return a long positive float without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN .3405892687 AS literal
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

### 🔴 [Scenario 05] Return a very long positive float
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 126354186523812635418263552340512384016094862983471987543918591348961093487896783409268730945879405123840160948812635418265234051238401609486298347198754391859134896109348789678340926873094587962983471812635265234051238401609486298348126354182652340512384016094862983471987543918591348961093487896783409218.0 AS literal
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

### 🔴 [Scenario 06] Return a very long positive float without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN .00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001 AS literal
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

### 🔴 [Scenario 07] Return a positive zero float
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 0.0 AS literal
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

### 🔴 [Scenario 08] Return a positive zero float without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN .0 AS literal
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

### 🔴 [Scenario 09] Return a negative zero float
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -0.0 AS literal
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

### 🔴 [Scenario 10] Return a negative zero float without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -.0 AS literal
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

### 🔴 [Scenario 11] Return a very long negative float
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -126354186523812635418263552340512384016094862983471987543918591348961093487896783409268730945879405123840160948812635418265234051238401609486298347198754391859134896109348789678340926873094587962983471812635265234051238401609486298348126354182652340512384016094862983471987543918591348961093487896783409218.0 AS literal
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

### 🔴 [Scenario 12] Return a very long negative float without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001 AS literal
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

### 🔴 [Scenario 13] Return a positive float with positive lower case exponent
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1e9 AS literal
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

### 🔴 [Scenario 14] Return a positive float with positive upper case exponent
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1E9 AS literal
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

### 🔴 [Scenario 15] Return a positive float with positive lower case exponent without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN .1e9 AS literal
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

### 🔴 [Scenario 16] Return a positive float with negative lower case exponent
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1e-5 AS literal
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

### 🔴 [Scenario 17] Return a positive float with negative lower case exponent without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN .1e-5 AS literal
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

### 🔴 [Scenario 18] Return a positive float with negative upper case exponent without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN .1E-5 AS literal
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

### 🔴 [Scenario 19] Return a negative float in with positive lower case exponent
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -1e9 AS literal
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

### 🔴 [Scenario 20] Return a negative float in with positive upper case exponent
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -1E9 AS literal
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

### 🔴 [Scenario 21] Return a negative float with positive lower case exponent without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -.1e9 AS literal
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

### 🔴 [Scenario 22] Return a negative float with negative lower case exponent
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -1e-5 AS literal
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

### 🔴 [Scenario 23] Return a negative float with negative lower case exponent without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -.1e-5 AS literal
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

### 🔴 [Scenario 24] Return a negative float with negative upper case exponent without integer digits
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN -.1E-5 AS literal
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

### 🔴 [Scenario 25] Return a positive float with one integer digit and maximum positive exponent
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1e308 AS literal
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

### 🔴 [Scenario 26] Return a positive float with nine integer digit and maximum positive exponent
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 123456789e300 AS literal
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

### 🔴 [Scenario 27] Fail when float value is too large
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1.34E999
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
**Goal:** Implement basic Literals5 functionality
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

- Feature file: `tck/features/expressions/literals/Literals5.feature`
- Total scenarios: 27
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
tck/features/expressions/literals/Literals5.feature
```
