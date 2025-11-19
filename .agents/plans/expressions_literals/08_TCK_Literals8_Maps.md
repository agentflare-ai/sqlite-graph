# Literals8 - Maps - Task List

**Feature:** Literals8 - Maps
**Status:** 🔴 NOT STARTED (0/27 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Maps

**Feature File:** `tck/features/expressions/literals/Literals8.feature`
**Total Scenarios:** 27

**Key Capabilities:**
- Return an empty map
- Return a map containing one value with alphabetic lower case key
- Return a map containing one value with alphabetic upper case key
- Return a map containing one value with alphabetic mixed case key
- Return a map containing one value with alphanumeric mixed case key
- ... and 22 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 27 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 |

---

## Test Scenarios

### 🔴 [Scenario 01] Return an empty map
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {} AS literal
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

### 🔴 [Scenario 02] Return a map containing one value with alphabetic lower case key
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {abc: 1} AS literal
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

### 🔴 [Scenario 03] Return a map containing one value with alphabetic upper case key
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {ABC: 1} AS literal
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

### 🔴 [Scenario 04] Return a map containing one value with alphabetic mixed case key
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {aBCdeF: 1} AS literal
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

### 🔴 [Scenario 05] Return a map containing one value with alphanumeric mixed case key
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {a1B2c3e67: 1} AS literal
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

### 🔴 [Scenario 06] Return a map containing a boolean
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {k: false} AS literal
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

### 🔴 [Scenario 07] Return a map containing a null
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {k: null} AS literal
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

### 🔴 [Scenario 08] Return a map containing a integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {k: 1} AS literal
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

### 🔴 [Scenario 09] Return a map containing a hexadecimal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {F: -0x162CD4F6} AS literal
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

### 🔴 [Scenario 10] Return a map containing a octal integer
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {k: 0o2613152366} AS literal
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

### 🔴 [Scenario 11] Return a map containing a float
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {k: -.1e-5} AS literal
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

### 🔴 [Scenario 12] Return a map containing a string
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {k: 'ab: c, as#?lßdj '} AS literal
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

### 🔴 [Scenario 13] Return a map containing an empty map
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {a: {}} AS literal
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

### 🔴 [Scenario 14] Return seven-deep nested maps
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {a1: {a2: {a3: {a4: {a5: {a6: {}}}}}}} AS literal
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

### 🔴 [Scenario 15] Return 20-deep nested maps
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {a1: {a2: {a3: {a4: {a5: {a6: {a7: {a8: {a9: {a10: {a11: {a12: {a13: {a14: {a15: {a16: {a17: {a18: {a19: {}}}}}}}}}}}}}}}}}}}} AS literal
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

### 🔴 [Scenario 16] Return 40-deep nested maps
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {a1: {a2: {a3: {a4: {a5: {a6: {a7: {a8: {a9: {a10: {a11: {a12: {a13: {a14: {a15: {a16: {a17: {a18: {a19: {a20: {a21: {a22: {a23: {a24: {a25: {a26: {a27: {a28: {a29: {a30: {a31: {a32: {a33: {a34: {a35: {a36: {a37: {a38: {a39: {}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}} AS literal
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

### 🔴 [Scenario 17] Return a map containing real and fake nested maps
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN { a : ' { b : ' , c : { d : ' ' } , d : ' } ' } AS literal
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

### 🔴 [Scenario 18] Return a complex map containing multiple mixed and nested values
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN  { data: [ {
                  id: '0001',
                  type: 'donut',
                  name: 'Cake',
                  ppu: 0.55,
                  batters:
                      {
                          batter:
                              [
                                  { id: '1001', type: 'Regular' },
                                  { id: '1002', type: 'Chocolate' },
                                  { id: '1003', type: 'Blueberry' },
                                  { id: '1004', type: 'Devils Food' }
                              ]
                      },
                  topping:
                      [
                          { id: '5001', type: 'None' },
                          { id: '5002', type: 'Glazed' },
                          { id: '5005', type: 'Sugar' },
                          { id: '5007', type: 'Powdered Sugar' },
                          { id: '5006', type: 'Chocolate Sprinkles' },
                          { id: '5003', type: 'Chocolate' },
                          { id: '5004', type: 'Maple' }
                      ]
              },
              {
                  id: '0002',
                  type: 'donut',
                  name: 'Raised',
                  ppu: 0.55,
                  batters:
                      {
                          batter:
                              [
                                  { id: '1001', type: 'Regular' }
                              ]
                      },
                  topping:
                      [
                          { id: '5001', type: 'None' },
                          { id: '5002', type: 'Glazed' },
                          { id: '5005', type: 'Sugar' },
                          { id: '5003', type: 'Chocolate' },
                          { id: '5004', type: 'Maple' }
                      ]
              },
              {
                  id: '0003',
                  type: 'donut',
                  name: 'Old Fashioned',
                  ppu: 0.55,
                  batters:
                      {
                          batter:
                              [
                                  { id: '1001', type: 'Regular' },
                                  { id: '1002', type: 'Chocolate' }
                              ]
                      },
                  topping:
                      [
                          { id: '5001', type: 'None' },
                          { id: '5002', type: 'Glazed' },
                          { id: '5003', type: 'Chocolate' },
                          { id: '5004', type: 'Maple' }
                      ]
              } ] } AS literal
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

### 🔴 [Scenario 19] Fail on a map containing key starting with a number
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {1B2c3e67:1} AS literal
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

### 🔴 [Scenario 20] Fail on a map containing key with symbol
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {k1#k: 1} AS literal
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

### 🔴 [Scenario 21] Fail on a map containing key with dot
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {k1.k: 1} AS literal
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

### 🔴 [Scenario 22] Fail on a map containing unquoted string
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {k1: k2} AS literal
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

### 🔴 [Scenario 23] Fail on a map containing only a comma
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {, } AS literal
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

### 🔴 [Scenario 24] Fail on a map containing a value without key
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {1} AS literal
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

### 🔴 [Scenario 25] Fail on a map containing a list without key
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {[]} AS literal
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

### 🔴 [Scenario 26] Fail on a map containing a map without key
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {{}} AS literal
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

### 🔴 [Scenario 27] Fail on a nested map with non-matching braces
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN {k: {k: {}} AS literal
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
**Goal:** Implement basic Literals8 functionality
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

- Feature file: `tck/features/expressions/literals/Literals8.feature`
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
tck/features/expressions/literals/Literals8.feature
```
