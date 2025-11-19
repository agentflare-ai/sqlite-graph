# List5 - List Membership Validation - IN Operator - Task List

**Feature:** List5 - List Membership Validation - IN Operator
**Status:** 🔴 NOT STARTED (0/41 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

List Membership Validation - IN Operator

**Feature File:** `tck/features/expressions/list/List5.feature`
**Total Scenarios:** 41

**Key Capabilities:**
- IN should work with nested list subscripting
- IN should work with nested literal list subscripting
- IN should work with list slices
- IN should work with literal list slices
- IN should return false when matching a number with a string
- ... and 36 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 41 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 |

---

## Test Scenarios

### 🔴 [Scenario 01] IN should work with nested list subscripting
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH [[1, 2, 3]] AS list
      RETURN 3 IN list[0] AS r
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

### 🔴 [Scenario 02] IN should work with nested literal list subscripting
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 3 IN [[1, 2, 3]][0] AS r
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

### 🔴 [Scenario 03] IN should work with list slices
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH [1, 2, 3] AS list
      RETURN 3 IN list[0..1] AS r
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

### 🔴 [Scenario 04] IN should work with literal list slices
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 3 IN [1, 2, 3][0..1] AS r
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

### 🔴 [Scenario 05] IN should return false when matching a number with a string
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1 IN ['1', 2] AS res
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

### 🔴 [Scenario 06] IN should return false when matching a number with a string - list version
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [1, [1, '2']] AS res
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

### 🔴 [Scenario 07] IN should return false when types of LHS and RHS don't match - singleton list
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1] IN [1, 2] AS res
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

### 🔴 [Scenario 08] IN should return false when types of LHS and RHS don't match - list
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [1, 2] AS res
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

### 🔴 [Scenario 09] IN should return true when types of LHS and RHS match - singleton list
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1] IN [1, 2, [1]] AS res
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

### 🔴 [Scenario 10] IN should return true when types of LHS and RHS match - list
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [1, [1, 2]] AS res
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

### 🔴 [Scenario 11] IN should return false when order of elements in LHS list and RHS list don't match
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [1, [2, 1]] AS res
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

### 🔴 [Scenario 12] IN with different length lists should return false
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [1, [1, 2, 3]] AS res
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

### 🔴 [Scenario 13] IN should return false when matching a list with a nested list with same elements
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [1, [[1, 2]]] AS res
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

### 🔴 [Scenario 14] IN should return true when both LHS and RHS contain nested lists
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [[1, 2], [3, 4]] IN [5, [[1, 2], [3, 4]]] AS res
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

### 🔴 [Scenario 15] IN should return true when both LHS and RHS contain a nested list alongside a scalar element
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [[1, 2], 3] IN [1, [[1, 2], 3]] AS res
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

### 🔴 [Scenario 16] IN should return true when LHS and RHS contain a nested list - singleton version
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [[1]] IN [2, [[1]]] AS res
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

### 🔴 [Scenario 17] IN should return true when LHS and RHS contain a nested list
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [[1, 3]] IN [2, [[1, 3]]] AS res
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

### 🔴 [Scenario 18] IN should return false when LHS contains a nested list and type mismatch on RHS - singleton version
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [[1]] IN [2, [1]] AS res
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

### 🔴 [Scenario 19] IN should return false when LHS contains a nested list and type mismatch on RHS
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [[1, 3]] IN [2, [1, 3]] AS res
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

### 🔴 [Scenario 20] IN should return null if LHS and RHS are null
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN null IN [null] AS res
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

### 🔴 [Scenario 21] IN should return null if LHS and RHS are null - list version
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [null] IN [[null]] AS res
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

### 🔴 [Scenario 22] IN should return null when LHS and RHS both ultimately contain null, even if LHS and RHS are of different types (nested list and flat list)
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [null] IN [null] AS res
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

### 🔴 [Scenario 23] IN with different length lists should return false despite nulls
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1] IN [[1, null]] AS res
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

### 🔴 [Scenario 24] IN should return true if match despite nulls
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 3 IN [1, null, 3] AS res
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

### 🔴 [Scenario 25] IN should return null if comparison with null is required
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 4 IN [1, null, 3] AS res
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

### 🔴 [Scenario 26] IN should return true if correct list found despite other lists having nulls
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [[null, 'foo'], [1, 2]] AS res
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

### 🔴 [Scenario 27] IN should return true if correct list found despite null being another element within containing list
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [1, [1, 2], null] AS res
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

### 🔴 [Scenario 28] IN should return false if no match can be found, despite nulls
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [[null, 'foo']] AS res
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

### 🔴 [Scenario 29] IN should return null if comparison with null is required, list version
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [[null, 2]] AS res
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

### 🔴 [Scenario 30] IN should return false if different length lists compared, even if the extra element is null
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [1, [1, 2, null]] AS res
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

### 🔴 [Scenario 31] IN should return null when comparing two so-called identical lists where one element is null
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2, null] IN [1, [1, 2, null]] AS res
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

### 🔴 [Scenario 32] IN should return true with previous null match, list version
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [[null, 2], [1, 2]] AS res
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

### 🔴 [Scenario 33] IN should return false if different length lists with nested elements compared, even if the extra element is null
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [[1, 2], [3, 4]] IN [5, [[1, 2], [3, 4], null]] AS res
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

### 🔴 [Scenario 34] IN should return null if comparison with null is required, list version 2
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [1, 2] IN [[null, 2], [1, 3]] AS res
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

### 🔴 [Scenario 35] IN should work with an empty list
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [] IN [[]] AS res
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

### 🔴 [Scenario 36] IN should return false for the empty list if the LHS and RHS types differ
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [] IN [] AS res
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

### 🔴 [Scenario 37] IN should work with an empty list in the presence of other list elements: matching
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [] IN [1, []] AS res
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

### 🔴 [Scenario 38] IN should work with an empty list in the presence of other list elements: not matching
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [] IN [1, 2] AS res
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

### 🔴 [Scenario 39] IN should work with an empty list when comparing nested lists
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [[]] IN [1, [[]]] AS res
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

### 🔴 [Scenario 40] IN should return null if comparison with null is required for empty list
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN [] IN [1, 2, null] AS res
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

### 🔴 [Scenario 41] IN should return true when LHS and RHS contain nested list with multiple empty lists
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1 IN <invalid>
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
**Goal:** Implement basic List5 functionality
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

- Feature file: `tck/features/expressions/list/List5.feature`
- Total scenarios: 41
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_expressions_list
```

**Feature File Location:**
```
tck/features/expressions/list/List5.feature
```
