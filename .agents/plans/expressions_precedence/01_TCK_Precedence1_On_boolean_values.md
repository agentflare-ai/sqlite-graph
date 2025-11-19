# Precedence1 - On boolean values - Task List

**Feature:** Precedence1 - On boolean values
**Status:** 🔴 NOT STARTED (0/19 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

On boolean values

**Feature File:** `tck/features/expressions/precedence/Precedence1.feature`
**Total Scenarios:** 19

**Key Capabilities:**
- Exclusive disjunction takes precedence over inclusive disjunction
- Conjunction disjunction takes precedence over exclusive disjunction
- Conjunction disjunction takes precedence over inclusive disjunction
- Negation takes precedence over conjunction
- Negation takes precedence over inclusive disjunction
- ... and 14 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 19 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 27 |

---

## Test Scenarios

### 🔴 [Scenario 01] Exclusive disjunction takes precedence over inclusive disjunction
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN true OR true XOR true AS a,
             true OR (true XOR true) AS b,
             (true OR true) XOR true AS c
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

### 🔴 [Scenario 02] Conjunction disjunction takes precedence over exclusive disjunction
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN true XOR false AND false AS a,
             true XOR (false AND false) AS b,
             (true XOR false) AND false AS c
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

### 🔴 [Scenario 03] Conjunction disjunction takes precedence over inclusive disjunction
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN true OR false AND false AS a,
             true OR (false AND false) AS b,
             (true OR false) AND false AS c
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

### 🔴 [Scenario 04] Negation takes precedence over conjunction
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN NOT true AND false AS a,
             (NOT true) AND false AS b,
             NOT (true AND false) AS c
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

### 🔴 [Scenario 05] Negation takes precedence over inclusive disjunction
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN NOT false OR true AS a,
             (NOT false) OR true AS b,
             NOT (false OR true) AS c
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

### 🔴 [Scenario 06] Comparison operator takes precedence over boolean negation
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN NOT false >= false AS a,
             NOT (false >= false) AS b,
             (NOT false) >= false AS c
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

### 🔴 [Scenario 07] Comparison operator takes precedence over binary boolean operator
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN true OR false = false AS a,
             true OR (false = false) AS b,
             (true OR false) = false AS c
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

### 🔴 [Scenario 08] Null predicate takes precedence over comparison operator
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN false = true IS NULL AS a,
             false = (true IS NULL) AS b,
             (false = true) IS NULL AS c
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

### 🔴 [Scenario 09] Null predicate takes precedence over negation
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN NOT false IS NULL AS a,
             NOT (false IS NULL) AS b,
             (NOT false) IS NULL AS c
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

### 🔴 [Scenario 10] Null predicate takes precedence over boolean operator
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN true OR false IS NULL AS a,
             true OR (false IS NULL) AS b,
             (true OR false) IS NULL AS c
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

### 🔴 [Scenario 11] List predicate takes precedence over comparison operator
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN false = true IN [true, false] AS a,
             false = (true IN [true, false]) AS b,
             (false = true) IN [true, false] AS c
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

### 🔴 [Scenario 12] List predicate takes precedence over negation
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN NOT true IN [true, false] AS a,
             NOT (true IN [true, false]) AS b,
             (NOT true) IN [true, false] AS c
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

### 🔴 [Scenario 13] List predicate takes precedence over boolean operator
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN false AND true IN [true, false] AS a,
             false AND (true IN [true, false]) AS b,
             (false AND true) IN [true, false] AS c
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

### 🔴 [Scenario 14] Exclusive disjunction takes precedence over inclusive disjunction in every combination of truth values
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false, null] AS a
      UNWIND [true, false, null] AS b
      UNWIND [true, false, null] AS c
      WITH collect((a OR b XOR c) = (a OR (b XOR c))) AS eq,
           collect((a OR b XOR c) <> ((a OR b) XOR c)) AS neq
      RETURN all(x IN eq WHERE x) AND any(x IN neq WHERE x) AS result
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

### 🔴 [Scenario 15] Conjunction takes precedence over exclusive disjunction in every combination of truth values
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false, null] AS a
      UNWIND [true, false, null] AS b
      UNWIND [true, false, null] AS c
      WITH collect((a XOR b AND c) = (a XOR (b AND c))) AS eq,
           collect((a XOR b AND c) <> ((a XOR b) AND c)) AS neq
      RETURN all(x IN eq WHERE x) AND any(x IN neq WHERE x) AS result
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

### 🔴 [Scenario 16] Conjunction takes precedence over inclusive disjunction in every combination of truth values
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false, null] AS a
      UNWIND [true, false, null] AS b
      UNWIND [true, false, null] AS c
      WITH collect((a OR b AND c) = (a OR (b AND c))) AS eq,
           collect((a OR b AND c) <> ((a OR b) AND c)) AS neq
      RETURN all(x IN eq WHERE x) AND any(x IN neq WHERE x) AS result
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

### 🔴 [Scenario 17] Negation takes precedence over conjunction in every combination of truth values
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false, null] AS a
      UNWIND [true, false, null] AS b
      WITH collect((NOT a AND b) = ((NOT a) AND b)) AS eq,
           collect((NOT a AND b) <> (NOT (a AND b))) AS neq
      RETURN all(x IN eq WHERE x) AND any(x IN neq WHERE x) AS result
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

### 🔴 [Scenario 18] Negation takes precedence over inclusive disjunction in every combination of truth values
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false, null] AS a
      UNWIND [true, false, null] AS b
      UNWIND [[], [true], [false], [null], [true, false], [true, false, null]] AS c
      WITH collect((a <comp> b IN c) = (a <comp> (b IN c))) AS eq,
           collect((a <comp> b IN c) <> ((a <comp> b) IN c)) AS neq
      RETURN all(x IN eq WHERE x) AND any(x IN neq WHERE x) AS result
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

### 🔴 [Scenario 27] List predicate takes precedence over negation in every combination of truth values
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false, null] AS a
      UNWIND [true, false, null] AS b
      UNWIND [[], [true], [false], [null], [true, false], [true, false, null]] AS c
      WITH collect((a <boolop> b IN c) = (a <boolop> (b IN c))) AS eq,
           collect((a <boolop> b IN c) <> ((a <boolop> b) IN c)) AS neq
      RETURN all(x IN eq WHERE x) AND any(x IN neq WHERE x) AS result
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
**Goal:** Implement basic Precedence1 functionality
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

- Feature file: `tck/features/expressions/precedence/Precedence1.feature`
- Total scenarios: 19
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_expressions_precedence
```

**Feature File Location:**
```
tck/features/expressions/precedence/Precedence1.feature
```
