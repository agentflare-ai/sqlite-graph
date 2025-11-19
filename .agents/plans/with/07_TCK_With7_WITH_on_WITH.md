# With7 - WITH on WITH - Task List

**Feature:** With7 - WITH on WITH
**Status:** 🔴 NOT STARTED (0/2 scenarios)
**Priority:** MEDIUM - Chained WITH clauses
**Last Updated:** 2025-11-15

---

## Overview

This feature tests complex queries with multiple consecutive WITH clauses. Tests variable renaming, predicate application, and aggregation across WITH boundaries.

**Key Capabilities:**
- Multiple WITH clauses in sequence
- Variable shadowing across WITH boundaries
- WHERE predicates after WITH
- Aggregation followed by filtering
- Complex query chains

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 2 | 01, 02 |

---

## Test Scenarios

### 🔴 [Scenario 01] A simple pattern with one bound endpoint
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
# Given: (:A)-[:REL]->(:B)
MATCH (a:A)-[r:REL]->(b:B)
WITH a AS b, b AS tmp, r AS r
WITH b AS a, r
LIMIT 1
MATCH (a)-[r]->(b)
RETURN a, r, b
```

**Expected:**
| a    | r      | b    |
| (:A) | [:REL] | (:B) |

**TODO:**
- [ ] Parse multiple consecutive WITH clauses
  - [ ] Allow WITH after WITH
  - [ ] Each WITH starts new scope
  - [ ] Variables from previous WITH available to next
- [ ] Test variable renaming chains
  - [ ] `a AS b` in first WITH
  - [ ] `b AS a` in second WITH (uses renamed value)
  - [ ] Verify variable swapping works
- [ ] Test LIMIT with WITH
  - [ ] Apply LIMIT after WITH projection
  - [ ] LIMIT before MATCH reduces rows for pattern
- [ ] Test pattern matching with renamed variables
  - [ ] `MATCH (a)-[r]->(b)` uses renamed variables
  - [ ] Verify correct binding and result

**Dependencies:** With1, With4 (aliasing), LIMIT
**Estimated Effort:** 2-3 days

---

### 🔴 [Scenario 02] Multiple WITHs using predicate and aggregation
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
# Given: Complex graph with David and connections
MATCH (david {name: 'David'})--(otherPerson)-->()
WITH otherPerson, count(*) AS foaf
WHERE foaf > 1
WITH otherPerson
WHERE otherPerson.name <> 'NotOther'
RETURN count(*)
```

**Expected:**
| count(*) |
| 1        |

**TODO:**
- [ ] Test aggregation in first WITH
  - [ ] Group by otherPerson
  - [ ] Compute count(*) AS foaf
- [ ] Test WHERE after first WITH
  - [ ] Filter groups: `WHERE foaf > 1`
  - [ ] Only groups with count > 1 pass through
- [ ] Test second WITH without aggregation
  - [ ] Forward otherPerson only
  - [ ] Drop foaf variable
- [ ] Test WHERE after second WITH
  - [ ] Filter by property: `WHERE otherPerson.name <> 'NotOther'`
  - [ ] Property access on forwarded variable
- [ ] Test final aggregation in RETURN
  - [ ] count(*) on filtered results
  - [ ] Verify correct count

**Dependencies:** With1, With6 (aggregation), WHERE clause, count()
**Estimated Effort:** 2-3 days

---

## Implementation Plan

### Phase 1: Chained WITH (Scenarios 01-02)
**Goal:** Multiple WITH clauses work correctly
**Timeline:** 4-5 days
**Priority:** MEDIUM

**Tasks:**
1. **Parser Enhancement** (1 day)
   - Allow multiple consecutive WITH clauses
   - Each WITH creates new scope
   - Test parsing chains

2. **Scope Management** (2 days)
   - Variables from previous WITH available to next
   - Implement scope chaining
   - Test variable shadowing across WITH

3. **Integration Testing** (1-2 days)
   - Test with LIMIT (Scenario 01)
   - Test with WHERE and aggregation (Scenario 02)
   - Verify complex chains work

**Success Criteria:**
- Both scenarios passing
- WITH chains working correctly
- Scope management correct

---

## Dependencies

**External:**
- With1 (basic WITH) 🔴 REQUIRED
- With4 (aliasing) 🔴 REQUIRED
- With6 (aggregation) 🔴 REQUIRED (Scenario 02)
- WHERE clause 🔴 REQUIRED
- LIMIT 🔴 REQUIRED (Scenario 01)
- count() function (Scenario 02)

**Internal:**
- Variable scope chaining ✅ (needs enhancement)

---

## Success Criteria

### Production Ready
- [ ] Both scenarios passing
- [ ] WITH chains work correctly
- [ ] Scope management solid
- **Target:** 2/2 (100%)

---

## Files to Modify

### Parser
- [ ] `src/cypher/cypher-parser.c` - Allow consecutive WITH clauses

### Planner
- [ ] `src/cypher/cypher-planner.c` - Scope chaining for multiple WITH

### Tests
- [ ] `tests/tck_test_clauses_with.c` - With7 scenarios

---

## Notes

- Multiple WITH is common in real-world queries
- Scope management is critical - each WITH creates boundary
- WHERE after WITH is important for filtering grouped/projected data
- This tests integration of many features

---

## Quick Reference

**Feature File:** `tck/features/clauses/with/With7.feature`
**Test File:** `tests/tck_test_clauses_with.c`
**Scenarios:** 2 total

**Run Tests:**
```bash
./build/tests/tck_test_clauses_with
```
