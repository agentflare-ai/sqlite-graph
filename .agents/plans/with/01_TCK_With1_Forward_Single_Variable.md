# With1 - Forward Single Variable - Task List

**Feature:** With1 - Forward single variable
**Status:** 🔴 NOT STARTED (0/6 scenarios)
**Priority:** 🔴 CRITICAL - Foundation for WITH clause
**Last Updated:** 2025-11-15

---

## Overview

This feature tests the basic capability of WITH to forward single variables across query parts. This is the foundation of the WITH clause and must work before any other WITH features.

**Key Capabilities:**
- Forward node variables
- Forward relationship variables
- Forward path variables
- Handle NULL values
- Forward possibly-null variables

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 6 | 01, 02, 03, 04, 05, 06 |

---

## Test Scenarios

### 🔴 [Scenario 01] Forwarding a node variable 1
**Status:** 🔴 NOT STARTED
**Priority:** 🔴 CRITICAL - Foundation

**Test:**
```cypher
# Given: (:A)-[:REL]->(:B)
MATCH (a:A)
WITH a
MATCH (a)-->(b)
RETURN *
```

**Expected:**
| a    | b    |
| (:A) | (:B) |

**TODO:**
- [ ] Implement basic WITH clause parsing
  - [ ] Add CYPHER_AST_WITH to AST types
  - [ ] Parse `WITH` keyword
  - [ ] Parse projection list (variables and expressions)
  - [ ] Handle alias syntax: `expr AS name`
- [ ] Implement WITH in planner
  - [ ] Create LOGICAL_WITH operator
  - [ ] Forward variable bindings to next query part
  - [ ] Maintain execution context across WITH boundary
- [ ] Implement WITH in physical plan
  - [ ] Map LOGICAL_WITH to PHYSICAL_PROJECT operator
  - [ ] Pass through variable values unchanged
- [ ] Implement iterator for WITH projection
  - [ ] Simple pass-through for variables
  - [ ] Handle column aliasing
- [ ] Test basic node forwarding
- [ ] Verify results match expected output

**Dependencies:** None (foundation)
**Estimated Effort:** 5-7 days

---

### 🔴 [Scenario 02] Forwarding a node variable 2
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
# Given: (:A)-[:REL]->(:B), (:X)
MATCH (a:A)
WITH a
MATCH (x:X), (a)-->(b)
RETURN *
```

**Expected:**
| a    | b    | x    |
| (:A) | (:B) | (:X) |

**TODO:**
- [ ] Verify WITH forwards variable correctly
- [ ] Test that forwarded variable works in Cartesian product
- [ ] Verify forwarded variable available in subsequent patterns
- [ ] Test result correctness

**Dependencies:** Scenario 01
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 03] Forwarding a relationship variable
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
# Given: ()-[:T1]->(:X), ()-[:T2]->(:X), ()-[:T3]->()
MATCH ()-[r1]->(:X)
WITH r1 AS r2
MATCH ()-[r2]->()
RETURN r2 AS rel
```

**Expected:**
| rel   |
| [:T1] |
| [:T2] |

**TODO:**
- [ ] Test relationship variable forwarding
- [ ] Verify aliasing works: `r1 AS r2`
- [ ] Ensure forwarded relationship can be used in pattern matching
- [ ] Verify only relationships matching original predicate are returned

**Dependencies:** Scenario 01
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 04] Forwarding a path variable
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
# Given: ()
MATCH p = (a)
WITH p
RETURN p
```

**Expected:**
| p    |
| <()> |

**TODO:**
- [ ] Test path variable forwarding
- [ ] Verify path data structure is preserved
- [ ] Ensure path can be returned after WITH
- [ ] Test path serialization to JSON

**Dependencies:** Scenario 01, Path support
**Estimated Effort:** 1-2 days

---

### 🔴 [Scenario 05] Forwarding null
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
OPTIONAL MATCH (a:Start)
WITH a
MATCH (a)-->(b)
RETURN *
```

**Expected:** Empty result (no rows)

**TODO:**
- [ ] Test NULL value forwarding
- [ ] Verify NULL values don't match in subsequent MATCH
- [ ] Ensure empty result set when forwarded value is NULL
- [ ] Test NULL propagation through WITH

**Dependencies:** Scenario 01, OPTIONAL MATCH support
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 06] Forwarding a node variable possibly null
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
# Given: Complex graph with :A and :B nodes
OPTIONAL MATCH (a:A)
WITH a AS a
MATCH (b:B)
RETURN a, b
```

**Expected:**
| a              | b              |
| (:A {num: 42}) | (:B {num: 46}) |

**TODO:**
- [ ] Test possibly-NULL variable forwarding
- [ ] Verify OPTIONAL MATCH result forwarded correctly
- [ ] Ensure Cartesian product works with possibly-NULL variables
- [ ] Test that non-NULL values are preserved

**Dependencies:** Scenario 01, OPTIONAL MATCH support
**Estimated Effort:** 1 day

---

## Implementation Plan

### Phase 1: Foundation (Scenarios 01-03)
**Goal:** Basic WITH clause working for nodes and relationships
**Timeline:** 1-2 weeks
**Priority:** 🔴 CRITICAL

**Tasks:**
1. **Parser Implementation** (3-4 days)
   - Add CYPHER_AST_WITH to AST
   - Implement WITH keyword recognition
   - Parse projection list (variables and aliases)
   - Handle `expr AS name` syntax
   - Parse multiple WITH clauses in sequence

2. **Planner Implementation** (2-3 days)
   - Create LOGICAL_WITH operator
   - Implement variable forwarding logic
   - Handle execution context across WITH boundary
   - Support variable aliasing

3. **Physical Plan** (1-2 days)
   - Map LOGICAL_WITH to PHYSICAL_PROJECT
   - Implement pass-through iterator
   - Handle column renaming for aliases

4. **Testing** (1 day)
   - Implement Scenarios 01-03
   - Verify basic forwarding works
   - Test aliasing

**Success Criteria:**
- Scenarios 01-03 passing
- Basic node and relationship forwarding working
- Variable aliasing supported

---

### Phase 2: Advanced Features (Scenarios 04-06)
**Goal:** Path variables and NULL handling
**Timeline:** 1 week
**Priority:** HIGH

**Tasks:**
1. **Path Support** (2-3 days)
   - Implement path forwarding
   - Test path preservation across WITH
   - Scenario 04 passing

2. **NULL Handling** (2-3 days)
   - Implement NULL value forwarding
   - Test NULL propagation
   - Scenarios 05-06 passing

**Success Criteria:**
- All 6 scenarios passing
- NULL handling correct
- Path variables working

---

## Dependencies

**External:**
- OPTIONAL MATCH (for Scenarios 05-06)
- Path variable support (for Scenario 04)

**Internal:**
- Parser infrastructure ✅ (exists)
- Planner infrastructure ✅ (exists)
- Iterator framework ✅ (exists)

---

## Success Criteria

### Minimum Viable
- [x] WITH clause parsed correctly
- [ ] Scenarios 01-03 passing (node and relationship forwarding)
- **Target:** 3/6 (50%)

### Production Ready
- [ ] Scenarios 01-06 passing
- [ ] NULL handling correct
- [ ] Path variables working
- **Target:** 6/6 (100%)

---

## Files to Modify

### Parser
- [ ] `include/cypher/cypher-ast.h` - Add CYPHER_AST_WITH
- [ ] `src/cypher/cypher-parser.c` - WITH clause parsing
- [ ] `src/cypher/cypher-ast.c` - WITH AST node handling

### Planner
- [ ] `include/cypher-planner.h` - LOGICAL_WITH operator
- [ ] `src/cypher/cypher-planner.c` - WITH compilation and variable forwarding

### Physical Plan
- [ ] `include/cypher-executor.h` - PHYSICAL_PROJECT operator
- [ ] `src/cypher/cypher-physical-plan.c` - LOGICAL_WITH → PHYSICAL_PROJECT mapping
- [ ] `src/cypher/cypher-iterators.c` - Projection iterator

### Tests
- [ ] `tests/tck_test_clauses_with.c` - With1 scenarios

---

## Notes

- With1 is the **absolute foundation** of WITH functionality
- All other WITH features depend on this working
- Focus on simple variable forwarding first
- Aliasing is critical - many tests use `var AS newname`
- NULL handling is important for OPTIONAL MATCH integration

---

## Quick Reference

**Feature File:** `tck/features/clauses/with/With1.feature`
**Test File:** `tests/tck_test_clauses_with.c`
**Scenarios:** 6 total

**Run Tests:**
```bash
./build/tests/tck_test_clauses_with
```
