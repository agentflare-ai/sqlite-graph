# Union3 - Union in combination with Union All - Task List

**Feature:** Union3 - Union in combination with Union All
**Status:** 🔴 NOT STARTED (0/2 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Union in combination with Union All

**Feature File:** `tck/features/clauses/union/Union3.feature`
**Total Scenarios:** 2

**Key Capabilities:**
- Failing when mixing UNION and UNION ALL
- Failing when mixing UNION ALL and UNION

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 2 | 01, 02 |

---

## Test Scenarios

### 🔴 [Scenario 01] Failing when mixing UNION and UNION ALL
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1 AS a
      UNION
      RETURN 2 AS a
      UNION ALL
      RETURN 3 AS a
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

### 🔴 [Scenario 02] Failing when mixing UNION ALL and UNION
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
RETURN 1 AS a
      UNION ALL
      RETURN 2 AS a
      UNION
      RETURN 3 AS a
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
**Goal:** Implement basic Union3 functionality
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

- Feature file: `tck/features/clauses/union/Union3.feature`
- Total scenarios: 2
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_union
```

**Feature File Location:**
```
tck/features/clauses/union/Union3.feature
```
