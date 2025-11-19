# With3 - Forward Multiple Expressions - Task List

**Feature:** With3 - Forward multiple expressions
**Status:** 🔴 NOT STARTED (0/1 scenarios)
**Priority:** HIGH - Multiple projections
**Last Updated:** 2025-11-15

---

## Overview

This feature tests WITH's ability to project multiple variables/expressions simultaneously. This is essential for forwarding complex state across query parts.

**Key Capabilities:**
- Project multiple variables in single WITH
- Forward nodes, relationships together
- Preserve variable relationships (a-[r]->b forwarded together)
- Use multiple forwarded variables in subsequent patterns

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 1 | 01 |

---

## Test Scenarios

### 🔴 [Scenario 01] Forwarding multiple node and relationship variables
**Status:** 🔴 NOT STARTED
**Priority:** 🔴 CRITICAL - Common pattern

**Test:**
```cypher
# Given: ()-[:T1 {id: 0}]->(:X), ()-[:T2 {id: 1}]->(:X), ()-[:T2 {id: 2}]->()
MATCH (a)-[r]->(b:X)
WITH a, r, b
MATCH (a)-[r]->(b)
RETURN r AS rel
  ORDER BY rel.id
```

**Expected:**
| rel           |
| [:T1 {id: 0}] |
| [:T2 {id: 1}] |

**TODO:**
- [ ] Parse multiple projection items in WITH
  - [ ] Comma-separated projection list: `WITH a, r, b`
  - [ ] Handle variable-only projections (no alias needed)
  - [ ] Support mixing variables and aliased expressions
- [ ] Forward multiple variables simultaneously
  - [ ] Maintain row integrity (values from same match stay together)
  - [ ] Create result set with multiple columns
  - [ ] Pass all columns to next query part
- [ ] Use multiple forwarded variables in patterns
  - [ ] Bind forwarded variables in subsequent MATCH
  - [ ] Test pattern: `(a)-[r]->(b)` with all three forwarded
  - [ ] Verify constraint: only rows matching all three variables returned
- [ ] Test result correctness with ORDER BY

**Dependencies:** With1 (basic WITH), With2 (expressions)
**Estimated Effort:** 2-3 days

---

## Implementation Plan

### Phase 1: Multiple Projections (Scenario 01)
**Goal:** Forward multiple variables in single WITH
**Timeline:** 2-3 days
**Priority:** 🔴 CRITICAL

**Tasks:**
1. **Parser Enhancement** (1 day)
   - Parse comma-separated projection items
   - Handle variable-only projections (implicit alias = variable name)
   - Support mixing `var` and `expr AS alias` syntax

2. **Multi-Column Projection** (1-2 days)
   - Create result set with multiple columns
   - Maintain row integrity across WITH boundary
   - Forward all columns to next query part
   - Test with node, relationship, and path variables

**Success Criteria:**
- Scenario 01 passing
- Multiple variables forwarded correctly
- Row integrity maintained

---

## Dependencies

**External:**
- With1 (basic WITH clause) 🔴 REQUIRED
- With2 (expression projection) 🔴 REQUIRED

**Internal:**
- Multi-column result sets ✅ (exists from RETURN)

---

## Success Criteria

### Production Ready
- [ ] Scenario 01 passing
- [ ] Multiple variables forwarded correctly
- [ ] Pattern matching with forwarded variables works
- **Target:** 1/1 (100%)

---

## Files to Modify

### Parser
- [ ] `src/cypher/cypher-parser.c` - Parse multiple projection items

### Planner
- [ ] `src/cypher/cypher-planner.c` - Create multi-column projection

### Executor
- [ ] `src/cypher/cypher-iterators.c` - Multi-column projection iterator

### Tests
- [ ] `tests/tck_test_clauses_with.c` - With3 scenarios

---

## Notes

- This is very common in real queries - critical functionality
- Row integrity is key: values from same MATCH must stay together
- Similar to RETURN with multiple columns (can reuse infrastructure)
- Tests constraint checking: forwarded `(a)-[r]->(b)` must maintain relationship

---

## Quick Reference

**Feature File:** `tck/features/clauses/with/With3.feature`
**Test File:** `tests/tck_test_clauses_with.c`
**Scenarios:** 1 total

**Run Tests:**
```bash
./build/tests/tck_test_clauses_with
```
