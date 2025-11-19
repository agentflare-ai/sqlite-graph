# ReturnOrderBy4 - Order by in combination with projection - Task List

**Feature:** ReturnOrderBy4 - Order by in combination with projection
**Status:** 🔴 NOT STARTED (0/2 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Order by in combination with projection

**Feature File:** `tck/features/clauses/return-orderby/ReturnOrderBy4.feature`
**Total Scenarios:** 2

**Key Capabilities:**
- ORDER BY of a column introduced in RETURN should return salient results in ascending order
- Handle projections with ORDER BY

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 2 | 01, 02 |

---

## Test Scenarios

### 🔴 [Scenario 01] ORDER BY of a column introduced in RETURN should return salient results in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
WITH [0, 1] AS prows, [[2], [3, 4]] AS qrows
      UNWIND prows AS p
      UNWIND qrows[p] AS q
      WITH p, count(q) AS rng
      RETURN p
      ORDER BY rng
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

### 🔴 [Scenario 02] Handle projections with ORDER BY
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (c:Crew {name: 'Neo'})
      WITH c, 0 AS relevance
      RETURN c.rank AS rank
      ORDER BY relevance, c.rank
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
**Goal:** Implement basic ReturnOrderBy4 functionality
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

- Feature file: `tck/features/clauses/return-orderby/ReturnOrderBy4.feature`
- Total scenarios: 2
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_return-orderby
```

**Feature File Location:**
```
tck/features/clauses/return-orderby/ReturnOrderBy4.feature
```
