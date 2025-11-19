# With4 - Variable Aliasing - Task List

**Feature:** With4 - Variable aliasing
**Status:** 🔴 NOT STARTED (0/7 scenarios)
**Priority:** HIGH - Aliasing semantics
**Last Updated:** 2025-11-15

---

## Overview

This feature tests WITH's variable aliasing capabilities and error handling. Includes renaming variables, shadowing, and error conditions.

**Key Capabilities:**
- Alias variables to new names
- Shadow variable names (reuse same name for different value)
- Detect duplicate alias errors
- Detect missing alias errors
- Complex aliasing chains

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 5 | 01, 02, 03, 06, 07 |
| 🔴 Error Tests | 2 | 04, 05 |

---

## Test Scenarios

### 🔴 [Scenario 01] Aliasing relationship variable
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
# Given: ()-[:T1]->(), ()-[:T2]->()
MATCH ()-[r1]->()
WITH r1 AS r2
RETURN r2 AS rel
```

**Expected:**
| rel   |
| [:T1] |
| [:T2] |

**TODO:**
- [ ] Test basic aliasing: `var AS newname`
- [ ] Verify original variable not available after WITH
- [ ] Verify new alias is available
- [ ] Test with relationships

**Dependencies:** With1
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 02] Aliasing expression to new variable name
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
# Given: (:Begin {num: 42}), (:End {num: 42}), (:End {num: 3})
MATCH (a:Begin)
WITH a.num AS property
MATCH (b:End)
WHERE property = b.num
RETURN b
```

**Expected:**
| b                |
| (:End {num: 42}) |

**TODO:**
- [ ] Test expression aliasing
- [ ] Verify projected value available as alias
- [ ] Test in WHERE predicate
- [ ] Verify original variable not available

**Dependencies:** With1, With2
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 03] Aliasing expression to existing variable name
**Status:** 🔴 NOT STARTED
**Priority:** 🔴 CRITICAL - Variable shadowing

**Test:**
```cypher
# Given: ({num: 1, name: 'King Kong'}), ({num: 2, name: 'Ann Darrow'})
MATCH (n)
WITH n.name AS n
RETURN n
```

**Expected:**
| n            |
| 'Ann Darrow' |
| 'King Kong'  |

**TODO:**
- [ ] Implement variable shadowing
  - [ ] Allow aliasing expression to same variable name
  - [ ] Replace original variable binding with new value
  - [ ] Test that old variable is completely replaced
- [ ] Verify type can change (node → string)
- [ ] Test shadowing with different types

**Dependencies:** With1, With2
**Estimated Effort:** 2 days

---

### 🔴 [Scenario 04] Fail when forwarding multiple aliases with the same name
**Status:** 🔴 NOT STARTED
**Priority:** HIGH - Error validation

**Test:**
```cypher
WITH 1 AS a, 2 AS a
RETURN a
```

**Expected:** SyntaxError: ColumnNameConflict

**TODO:**
- [ ] Implement duplicate alias detection
  - [ ] Check for duplicate names in projection list
  - [ ] Raise ColumnNameConflict error
  - [ ] Error at compile time (not runtime)
- [ ] Test error message clarity
- [ ] Add error code to cypher-errors.h

**Dependencies:** With1
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 05] Fail when not aliasing expressions in WITH
**Status:** 🔴 NOT STARTED
**Priority:** HIGH - Error validation

**Test:**
```cypher
MATCH (a)
WITH a, count(*)
RETURN a
```

**Expected:** SyntaxError: NoExpressionAlias

**TODO:**
- [ ] Implement expression alias requirement
  - [ ] Detect non-variable expressions in projection
  - [ ] Require AS alias for all expressions
  - [ ] Allow variables without alias (implicit alias = var name)
  - [ ] Raise NoExpressionAlias error
- [ ] Test with aggregation functions
- [ ] Add error code to cypher-errors.h

**Dependencies:** With1, aggregation functions
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 06] Reusing variable names in WITH
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
# Complex query with multiple WITH clauses reusing variable names
MATCH (person:Person)<--(message)<-[like]-(:Person)
WITH like.creationDate AS likeTime, person AS person
  ORDER BY likeTime, message.id
WITH head(collect({likeTime: likeTime})) AS latestLike, person AS person
WITH latestLike.likeTime AS likeTime
  ORDER BY likeTime
RETURN likeTime
```

**Expected:**
| likeTime |
| 20160614 |

**TODO:**
- [ ] Test multiple WITH clauses in sequence
- [ ] Test variable reuse across WITH boundaries
- [ ] Test aliasing variable to same name: `person AS person`
- [ ] Test with ORDER BY and aggregation functions
- [ ] Verify complex query chain works

**Dependencies:** With1-3, ORDER BY, aggregations, collect(), head()
**Estimated Effort:** 2-3 days

---

### 🔴 [Scenario 07] Multiple aliasing and backreferencing
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
CREATE (m {id: 0})
WITH {first: m.id} AS m
WITH {second: m.first} AS m
RETURN m.second
```

**Expected:**
| m.second |
| 0        |

**TODO:**
- [ ] Test shadowing with map construction
- [ ] Test backreferencing shadowed variable
- [ ] Verify value transformations through multiple WITH
- [ ] Test property access on constructed maps

**Dependencies:** With1-3, map literals, CREATE
**Estimated Effort:** 1-2 days

---

## Implementation Plan

### Phase 1: Basic Aliasing (Scenarios 01-03)
**Goal:** Aliasing and shadowing work correctly
**Timeline:** 3-4 days
**Priority:** 🔴 CRITICAL

**Tasks:**
1. **Aliasing Implementation** (2 days)
   - Support `var AS newname` syntax
   - Remove original variable from scope after WITH
   - Add alias to scope for next query part
   - Test basic aliasing

2. **Variable Shadowing** (1-2 days)
   - Allow `expr AS samename`
   - Replace variable binding
   - Support type changes
   - Test Scenario 03

**Success Criteria:**
- Scenarios 01-03 passing
- Aliasing works correctly
- Shadowing supported

---

### Phase 2: Error Validation (Scenarios 04-05)
**Goal:** Proper error detection
**Timeline:** 2 days
**Priority:** HIGH

**Tasks:**
1. **Duplicate Detection** (1 day)
   - Check for duplicate alias names
   - Raise ColumnNameConflict error
   - Test Scenario 04

2. **Alias Requirement** (1 day)
   - Detect unaliased expressions
   - Raise NoExpressionAlias error
   - Test Scenario 05

**Success Criteria:**
- Scenarios 04-05 passing
- Error messages clear
- Errors raised at compile time

---

### Phase 3: Advanced Patterns (Scenarios 06-07)
**Goal:** Complex aliasing scenarios
**Timeline:** 3-4 days
**Priority:** MEDIUM

**Tasks:**
1. **Multiple WITH Chains** (2 days)
   - Test chained WITH clauses
   - Verify variable reuse
   - Test Scenario 06

2. **Backreferencing** (1-2 days)
   - Test shadowed variable references
   - Test map construction with shadowing
   - Test Scenario 07

**Success Criteria:**
- Scenarios 06-07 passing
- Complex patterns working

---

## Dependencies

**External:**
- With1 (basic WITH) 🔴 REQUIRED
- With2 (expressions) 🔴 REQUIRED
- ORDER BY (Scenario 06)
- Aggregation functions (Scenarios 05-06)
- Map literals (Scenario 07)
- CREATE (Scenario 07)

**Internal:**
- Variable scope management ✅
- Error reporting ✅

---

## Success Criteria

### Minimum Viable
- [ ] Scenarios 01-03 passing (basic aliasing and shadowing)
- **Target:** 3/7 (43%)

### Production Ready
- [ ] Scenarios 01-05 passing (includes error validation)
- **Target:** 5/7 (71%)

### Complete
- [ ] All 7 scenarios passing
- **Target:** 7/7 (100%)

---

## Files to Modify

### Parser
- [ ] `src/cypher/cypher-parser.c` - Alias syntax parsing

### Planner
- [ ] `src/cypher/cypher-planner.c` - Variable scope management, aliasing logic
- [ ] `include/cypher-errors.h` - Add ColumnNameConflict, NoExpressionAlias errors

### Tests
- [ ] `tests/tck_test_clauses_with.c` - With4 scenarios

---

## Notes

- Variable shadowing is important but subtle - test thoroughly
- Error validation ensures query correctness
- Scenario 06 is complex - requires many dependencies
- Aliasing semantics must match Cypher spec exactly

---

## Quick Reference

**Feature File:** `tck/features/clauses/with/With4.feature`
**Test File:** `tests/tck_test_clauses_with.c`
**Scenarios:** 7 total (5 success, 2 error)

**Run Tests:**
```bash
./build/tests/tck_test_clauses_with
```
