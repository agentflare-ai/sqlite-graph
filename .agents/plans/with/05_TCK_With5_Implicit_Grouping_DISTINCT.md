# With5 - Implicit Grouping with DISTINCT - Task List

**Feature:** With5 - Implicit grouping with DISTINCT
**Status:** 🔴 NOT STARTED (0/2 scenarios)
**Priority:** MEDIUM - DISTINCT support
**Last Updated:** 2025-11-15

---

## Overview

This feature tests WITH's DISTINCT modifier, which removes duplicate rows from the projected result set.

**Key Capabilities:**
- DISTINCT on simple expressions
- DISTINCT on complex structures (maps with lists)
- Duplicate elimination based on value equality

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 2 | 01, 02 |

---

## Test Scenarios

### 🔴 [Scenario 01] DISTINCT on an expression
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
# Given: ({name: 'A'}), ({name: 'A'}), ({name: 'B'})
MATCH (a)
WITH DISTINCT a.name AS name
RETURN name
```

**Expected:**
| name |
| 'A'  |
| 'B'  |

**TODO:**
- [ ] Parse DISTINCT keyword in WITH clause
  - [ ] Add DISTINCT flag to WITH AST node
  - [ ] Parse: `WITH DISTINCT expr AS alias`
  - [ ] Support multiple projections with DISTINCT
- [ ] Implement DISTINCT operator in planner
  - [ ] Create LOGICAL_DISTINCT operator
  - [ ] Apply after projection, before next query part
  - [ ] Handle multiple columns
- [ ] Implement DISTINCT in physical plan
  - [ ] Map to PHYSICAL_DISTINCT operator
  - [ ] Use hash table for duplicate detection
  - [ ] Support equality comparison for all types
- [ ] Implement DISTINCT iterator
  - [ ] Track seen values in hash set
  - [ ] Filter duplicate rows
  - [ ] Efficient for large result sets
- [ ] Test with simple expressions

**Dependencies:** With1, With2
**Estimated Effort:** 3-4 days

---

### 🔴 [Scenario 02] Handling DISTINCT with lists in maps
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
# Given: ({list: ['A', 'B']}), ({list: ['A', 'B']})
MATCH (n)
WITH DISTINCT {name: n.list} AS map
RETURN count(*)
```

**Expected:**
| count(*) |
| 1        |

**TODO:**
- [ ] Support DISTINCT on complex types
  - [ ] Implement map equality comparison
  - [ ] Implement list equality comparison
  - [ ] Handle nested structures
- [ ] Test with map containing list
- [ ] Verify only one row returned for duplicates

**Dependencies:** Scenario 01, map literals, list support, count()
**Estimated Effort:** 2-3 days

---

## Implementation Plan

### Phase 1: Basic DISTINCT (Scenario 01)
**Goal:** DISTINCT works on simple expressions
**Timeline:** 3-4 days
**Priority:** MEDIUM

**Tasks:**
1. **Parser** (1 day)
   - Add DISTINCT keyword to WITH parsing
   - Set DISTINCT flag on AST node

2. **Planner** (1 day)
   - Create LOGICAL_DISTINCT operator
   - Insert after LOGICAL_WITH projection

3. **Physical Plan** (1-2 days)
   - Implement PHYSICAL_DISTINCT operator
   - Hash-based duplicate elimination
   - Support multiple columns

**Success Criteria:**
- Scenario 01 passing
- DISTINCT works on simple values

---

### Phase 2: Complex Types (Scenario 02)
**Goal:** DISTINCT works on maps and lists
**Timeline:** 2-3 days
**Priority:** MEDIUM

**Tasks:**
1. **Equality Comparison** (2 days)
   - Implement map equality
   - Implement list equality
   - Handle nested structures

2. **Testing** (1 day)
   - Test Scenario 02
   - Test edge cases

**Success Criteria:**
- Scenario 02 passing
- DISTINCT works on complex types

---

## Dependencies

**External:**
- With1 (basic WITH) 🔴 REQUIRED
- With2 (expressions) 🔴 REQUIRED
- Map literals (Scenario 02)
- List support (Scenario 02)
- count() aggregation (Scenario 02)

**Internal:**
- Hash table implementation ✅
- Type equality comparison (needs enhancement for maps/lists)

---

## Success Criteria

### Minimum Viable
- [ ] Scenario 01 passing (simple DISTINCT)
- **Target:** 1/2 (50%)

### Production Ready
- [ ] Both scenarios passing
- [ ] DISTINCT works on all types
- **Target:** 2/2 (100%)

---

## Files to Modify

### Parser
- [ ] `src/cypher/cypher-parser.c` - Parse DISTINCT keyword

### Planner
- [ ] `include/cypher-planner.h` - LOGICAL_DISTINCT operator
- [ ] `src/cypher/cypher-planner.c` - DISTINCT operator creation

### Physical Plan
- [ ] `include/cypher-executor.h` - PHYSICAL_DISTINCT operator
- [ ] `src/cypher/cypher-physical-plan.c` - Logical to physical mapping
- [ ] `src/cypher/cypher-iterators.c` - DISTINCT iterator

### Type System
- [ ] `src/cypher/cypher-values.c` - Map/list equality (if needed)

### Tests
- [ ] `tests/tck_test_clauses_with.c` - With5 scenarios

---

## Notes

- DISTINCT is similar to RETURN DISTINCT (can reuse infrastructure)
- Hash-based implementation for efficiency
- Equality comparison critical for correctness
- Must handle NULL values correctly

---

## Quick Reference

**Feature File:** `tck/features/clauses/with/With5.feature`
**Test File:** `tests/tck_test_clauses_with.c`
**Scenarios:** 2 total

**Run Tests:**
```bash
./build/tests/tck_test_clauses_with
```
