# Unwind1 - Unwind Lists and Expressions - Task List

**Feature:** Unwind1 - Unwinding lists and expressions into individual rows
**Status:** 🔴 NOT STARTED (0/14 scenarios)
**Priority:** MEDIUM
**Last Updated:** 2025-11-17

---

## Overview

The UNWIND clause expands a list into individual rows, creating one row per list element. This is essential for transforming list data into relational format.

**Feature File:** `tck/features/clauses/unwind/Unwind1.feature`
**Total Scenarios:** 14

**Key Capabilities:**
- Unwinding simple lists
- Unwinding ranges
- Unwinding concatenated lists
- Unwinding collected expressions
- Unwinding nested lists
- Handling NULL values
- Unwinding with variables
- Creating distinct rows
- Pattern matching with unwound values
- Multiple UNWIND clauses
- Nested UNWIND operations

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 14 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14 |

---

## Test Scenarios

### 🔴 [Scenario 01] Unwinding a list
**Status:** 🔴 NOT STARTED
**Priority:** CRITICAL

**Test:**
```cypher
UNWIND [1, 2, 3] AS x
RETURN x
```

**Expected:**
| x |
| 1 |
| 2 |
| 3 |

**TODO:**
- [ ] Implement UNWIND clause parsing
- [ ] Parse list expressions
- [ ] Create LOGICAL_UNWIND operator
- [ ] Implement unwind iterator
- [ ] Test basic list unwinding

**Dependencies:** Basic query execution
**Estimated Effort:** 3-4 days

---

### 🔴 [Scenario 02] Unwinding a range
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
UNWIND range(1, 3) AS x
RETURN x
```

**Expected:**
| x |
| 1 |
| 2 |
| 3 |

**TODO:**
- [ ] Implement range() function
- [ ] Handle function call in UNWIND
- [ ] Test range unwinding

**Dependencies:** Scenario 01, range() function
**Estimated Effort:** 1-2 days

---

### 🔴 [Scenario 03] Unwinding a concatenation of lists
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
WITH [1, 2, 3] AS first, [4, 5, 6] AS second
UNWIND (first + second) AS x
RETURN x
```

**Expected:** 6 rows (1-6)

**TODO:**
- [ ] Support list concatenation operator
- [ ] Handle expressions in UNWIND
- [ ] Test list concatenation

**Dependencies:** Scenario 01, WITH clause, list operators
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 04] Unwinding a collected unwound expression
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
UNWIND [1, 2, 3] AS row
WITH collect(row) AS rows
UNWIND rows AS x
RETURN x
```

**Expected:** 3 rows (1, 2, 3)

**TODO:**
- [ ] Implement collect() aggregation
- [ ] Test UNWIND -> collect -> UNWIND pipeline
- [ ] Handle aggregated lists

**Dependencies:** Scenario 01, collect() function
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 05] Unwinding a list of lists
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
UNWIND [[1, 2], [3, 4], 5] AS x
RETURN x
```

**Expected:**
| x      |
| [1, 2] |
| [3, 4] |
| 5      |

**TODO:**
- [ ] Handle nested lists
- [ ] Unwind outer list only
- [ ] Preserve inner list structure

**Dependencies:** Scenario 01
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 06] Unwinding empty list returns no rows
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
UNWIND [] AS empty
RETURN empty
```

**Expected:** Empty result (0 rows)

**TODO:**
- [ ] Handle empty list case
- [ ] Return no rows for empty list
- [ ] Test edge case

**Dependencies:** Scenario 01
**Estimated Effort:** 0.5 days

---

### 🔴 [Scenario 07] Unwinding null returns no rows
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
UNWIND null AS x
RETURN x
```

**Expected:** Empty result (0 rows)

**TODO:**
- [ ] Handle NULL in UNWIND
- [ ] Return no rows for NULL
- [ ] Test NULL edge case

**Dependencies:** Scenario 01
**Estimated Effort:** 0.5 days

---

### 🔴 [Scenario 08] Unwinding list with null elements
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
UNWIND [1, null, 2] AS x
RETURN x
```

**Expected:**
| x    |
| 1    |
| null |
| 2    |

**TODO:**
- [ ] Handle NULL elements in lists
- [ ] Preserve NULL values in output
- [ ] Test NULL handling

**Dependencies:** Scenario 01
**Estimated Effort:** 0.5 days

---

### 🔴 [Scenario 09] Unwinding WITH variable reference
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
WITH [1, 2, 3] AS list
UNWIND list AS x
RETURN x
```

**Expected:** 3 rows (1, 2, 3)

**TODO:**
- [ ] Support variable references in UNWIND
- [ ] Read list from context
- [ ] Test variable unwinding

**Dependencies:** Scenario 01, WITH clause
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 10] Create nodes from unwound list
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
UNWIND [1, 2, 3] AS x
CREATE (n {num: x})
RETURN n.num
```

**Expected:** 3 nodes created with num property

**TODO:**
- [ ] Support CREATE after UNWIND
- [ ] Use unwound values in CREATE
- [ ] Test side effects

**Dependencies:** Scenario 01, CREATE clause
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 11] Multiple UNWIND clauses
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
UNWIND [1, 2] AS x
UNWIND [3, 4] AS y
RETURN x, y
```

**Expected:** 4 rows (Cartesian product)
| x | y |
| 1 | 3 |
| 1 | 4 |
| 2 | 3 |
| 2 | 4 |

**TODO:**
- [ ] Support multiple UNWIND clauses
- [ ] Create Cartesian product
- [ ] Test chaining

**Dependencies:** Scenario 01
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 12] UNWIND with MATCH pattern
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n)
UNWIND n.list AS item
RETURN item
```

**Expected:** Unwound values from node property

**TODO:**
- [ ] Support UNWIND after MATCH
- [ ] Read property values
- [ ] Unwind node properties

**Dependencies:** Scenario 01, MATCH clause
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 13] UNWIND creating DISTINCT results
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [1, 1, 2, 2, 3] AS x
RETURN DISTINCT x
```

**Expected:** 3 rows (1, 2, 3) - duplicates removed

**TODO:**
- [ ] Support DISTINCT with UNWIND
- [ ] Remove duplicate rows
- [ ] Test distinct functionality

**Dependencies:** Scenario 01, DISTINCT operator
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 14] Unwind with ORDER BY
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [3, 1, 2] AS x
RETURN x ORDER BY x
```

**Expected:** 3 rows (1, 2, 3) - sorted

**TODO:**
- [ ] Support ORDER BY after UNWIND
- [ ] Sort unwound values
- [ ] Test ordering

**Dependencies:** Scenario 01, ORDER BY clause
**Estimated Effort:** 1 day

---

## Implementation Plan

### Phase 1: Foundation (Scenarios 01-02)
**Goal:** Basic UNWIND functionality
**Timeline:** 1 week
**Priority:** CRITICAL

**Tasks:**
1. **Parser Implementation** (2 days)
   - Add CYPHER_AST_UNWIND to AST
   - Parse UNWIND keyword
   - Parse list expressions
   - Parse AS clause for variable binding

2. **Planner Implementation** (2 days)
   - Create LOGICAL_UNWIND operator
   - Handle list expression compilation
   - Variable binding for unwound elements

3. **Physical Plan** (2 days)
   - Map LOGICAL_UNWIND to PHYSICAL_UNWIND
   - Implement unwind iterator
   - Handle list expansion logic

4. **Testing** (1 day)
   - Implement Scenarios 01-02
   - Verify basic unwinding works

**Success Criteria:**
- Scenarios 01-02 passing
- Basic list unwinding working
- Range unwinding working

---

### Phase 2: Edge Cases (Scenarios 03-09)
**Goal:** Handle all edge cases and combinations
**Timeline:** 1 week
**Priority:** HIGH

**Tasks:**
1. **Expression Support** (2 days)
   - List concatenation
   - Function calls (range, collect)
   - Variable references

2. **NULL Handling** (1 day)
   - Empty lists
   - NULL values
   - NULL elements in lists

3. **Integration** (2 days)
   - WITH clause integration
   - Nested lists
   - Collected expressions

4. **Testing** (1 day)
   - Scenarios 03-09 passing

**Success Criteria:**
- All edge cases handled
- NULL handling correct
- Expression evaluation working

---

### Phase 3: Advanced Features (Scenarios 10-14)
**Goal:** Complex UNWIND operations
**Timeline:** 1 week
**Priority:** MEDIUM

**Tasks:**
1. **Clause Integration** (2 days)
   - UNWIND with CREATE
   - UNWIND with MATCH
   - Multiple UNWIND clauses

2. **Advanced Operations** (2 days)
   - DISTINCT with UNWIND
   - ORDER BY with UNWIND
   - Cartesian products

3. **Testing** (1 day)
   - Scenarios 10-14 passing
   - Integration tests

**Success Criteria:**
- All 14 scenarios passing
- Full clause integration
- Complex queries working

---

## Files to Modify

### Parser
- [ ] `include/cypher/cypher-ast.h` - Add CYPHER_AST_UNWIND
- [ ] `src/cypher/cypher-parser.c` - UNWIND clause parsing
- [ ] `src/cypher/cypher-ast.c` - UNWIND AST node handling

### Planner
- [ ] `include/cypher-planner.h` - LOGICAL_UNWIND operator
- [ ] `src/cypher/cypher-planner.c` - UNWIND compilation

### Physical Plan
- [ ] `include/cypher-executor.h` - PHYSICAL_UNWIND operator
- [ ] `src/cypher/cypher-physical-plan.c` - LOGICAL_UNWIND → PHYSICAL_UNWIND mapping
- [ ] `src/cypher/cypher-iterators.c` - Unwind iterator

### Tests
- [ ] `tests/tck_test_clauses_unwind.c` - Unwind1 scenarios

---

## Success Criteria

### Minimum Viable
- [ ] UNWIND clause parsed correctly
- [ ] Scenarios 01-02 passing (basic list and range unwinding)
- [ ] Empty list and NULL handled
- **Target:** 4/14 (29%)

### Production Ready
- [ ] All 14 scenarios passing
- [ ] Full expression support
- [ ] Integration with other clauses
- [ ] Edge cases handled
- **Target:** 14/14 (100%)

---

## Dependencies

**External:**
- range() function (for Scenario 02)
- collect() function (for Scenario 04)
- List operators (for Scenario 03)
- CREATE clause (for Scenario 10)
- MATCH clause (for Scenario 12)
- DISTINCT operator (for Scenario 13)
- ORDER BY clause (for Scenario 14)

**Internal:**
- Parser infrastructure ✅ (exists)
- Planner infrastructure ✅ (exists)
- Iterator framework ✅ (exists)

---

## Notes

- UNWIND is critical for working with lists in Cypher
- Essential for transforming array data into rows
- Used frequently in data import and transformation
- Performance matters - unwinding large lists should be efficient
- Cartesian products can create very large result sets

---

## Quick Reference

**Feature File:** `tck/features/clauses/unwind/Unwind1.feature`
**Test File:** `tests/tck_test_clauses_unwind.c`
**Scenarios:** 14 total

**Run Tests:**
```bash
./build/tests/tck_test_clauses_unwind
```
