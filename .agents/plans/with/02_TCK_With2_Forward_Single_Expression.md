# With2 - Forward Single Expression - Task List

**Feature:** With2 - Forward single expression
**Status:** 🔴 NOT STARTED (0/2 scenarios)
**Priority:** HIGH - Expression projection
**Last Updated:** 2025-11-15

---

## Overview

This feature tests WITH's ability to project expressions (not just variables). This is critical for complex queries that compute values and pass them forward.

**Key Capabilities:**
- Project property access expressions
- Project complex expressions (map literals, nested structures)
- Use projected values in WHERE clauses
- Expression aliasing

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 2 | 01, 02 |

---

## Test Scenarios

### 🔴 [Scenario 01] Forwarding a property to express a join
**Status:** 🔴 NOT STARTED
**Priority:** 🔴 CRITICAL - Common pattern

**Test:**
```cypher
# Given: (:End {num: 42, id: 0}), (:End {num: 3}), (:Begin {num: 0})
MATCH (a:Begin)
WITH a.num AS property
MATCH (b)
WHERE b.id = property
RETURN b
```

**Expected:**
| b                       |
| (:End {num: 42, id: 0}) |

**TODO:**
- [ ] Implement expression evaluation in WITH projection
  - [ ] Parse property access in WITH: `a.num AS property`
  - [ ] Evaluate property access at runtime
  - [ ] Store result with alias name
- [ ] Forward scalar values (not just variables)
  - [ ] Handle integer/string/float values
  - [ ] Preserve value types through WITH
- [ ] Support projected values in WHERE predicates
  - [ ] Make projected alias available to WHERE clause
  - [ ] Test equality comparison with projected value
- [ ] Test join-like pattern using projected property

**Dependencies:** With1 (basic WITH)
**Estimated Effort:** 3-4 days

---

### 🔴 [Scenario 02] Forwarding a nested map literal
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
WITH {name: {name2: 'baz'}} AS nestedMap
RETURN nestedMap.name.name2
```

**Expected:**
| nestedMap.name.name2 |
| 'baz'                |

**TODO:**
- [ ] Support map literal expressions in WITH
  - [ ] Parse map literal syntax: `{key: value}`
  - [ ] Support nested maps: `{k1: {k2: value}}`
  - [ ] Evaluate map literals at compile/runtime
- [ ] Support map property access chains
  - [ ] Parse chained property access: `map.name.name2`
  - [ ] Evaluate nested property lookups
- [ ] Test complex expression forwarding

**Dependencies:** With1, Map literal support, Property access chains
**Estimated Effort:** 2-3 days

---

## Implementation Plan

### Phase 1: Expression Projection (Scenario 01)
**Goal:** Project and forward computed expressions
**Timeline:** 3-4 days
**Priority:** 🔴 CRITICAL

**Tasks:**
1. **Expression Evaluation in WITH** (2 days)
   - Extend WITH projection to handle expressions
   - Implement property access evaluation
   - Store computed values with aliases
   - Support scalar types (int, string, float, bool)

2. **Projected Value Usage** (1-2 days)
   - Make projected aliases available to subsequent clauses
   - Test in WHERE predicates
   - Test in pattern matching
   - Verify join-like patterns work

**Success Criteria:**
- Scenario 01 passing
- Property access expressions work
- Projected values usable in WHERE

---

### Phase 2: Complex Expressions (Scenario 02)
**Goal:** Support map literals and nested expressions
**Timeline:** 2-3 days
**Priority:** MEDIUM

**Tasks:**
1. **Map Literal Support** (1-2 days)
   - Implement map literal parsing and evaluation
   - Support nested map structures
   - Test map construction

2. **Property Access Chains** (1 day)
   - Implement chained property access
   - Test nested lookups

**Success Criteria:**
- Scenario 02 passing
- Map literals work
- Nested property access works

---

## Dependencies

**External:**
- With1 (basic WITH clause) 🔴 REQUIRED
- Map literal support (for Scenario 02)
- Property access chains (for Scenario 02)

**Internal:**
- Expression evaluator ✅ (exists)
- Type system ✅ (exists)

---

## Success Criteria

### Minimum Viable
- [ ] Scenario 01 passing (property projection)
- **Target:** 1/2 (50%)

### Production Ready
- [ ] Both scenarios passing
- [ ] Expression projection working
- [ ] Map literals supported
- **Target:** 2/2 (100%)

---

## Files to Modify

### Planner
- [ ] `src/cypher/cypher-planner.c` - Expression evaluation in WITH projection

### Executor
- [ ] `src/cypher/cypher-iterators.c` - Evaluate expressions in projection iterator
- [ ] `src/cypher/cypher-execution-context.c` - Store computed values

### Expression Support
- [ ] `src/cypher/cypher-expressions.c` - Property access, map literals (if not exists)

### Tests
- [ ] `tests/tck_test_clauses_with.c` - With2 scenarios

---

## Notes

- Expression projection is more complex than variable forwarding
- Property access is very common - must be efficient
- Map literals may require significant implementation if not existing
- This enables join-like patterns without explicit JOIN syntax

---

## Quick Reference

**Feature File:** `tck/features/clauses/with/With2.feature`
**Test File:** `tests/tck_test_clauses_with.c`
**Scenarios:** 2 total

**Run Tests:**
```bash
./build/tests/tck_test_clauses_with
```
