# Aggregation6 - Percentiles - Task List

**Feature:** Aggregation6 - Percentiles
**Status:** 🔴 NOT STARTED (0/1 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Percentiles

**Feature File:** `tck/features/expressions/aggregation/Aggregation6.feature`
**Total Scenarios:** 1

**Key Capabilities:**
- `percentileDisc()` failing in more involved query

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 1 | 05 |

---

## Test Scenarios

### 🔴 [Scenario 05] `percentileDisc()` failing in more involved query
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:S)
      WITH n, size([(n)-->() | 1]) AS deg
      WHERE deg > 2
      WITH deg
      LIMIT 100
      RETURN percentileDisc(0.90, deg), deg
```

**Expected:** ArgumentError

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
**Goal:** Implement basic Aggregation6 functionality
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

- Feature file: `tck/features/expressions/aggregation/Aggregation6.feature`
- Total scenarios: 1
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_expressions_aggregation
```

**Feature File Location:**
```
tck/features/expressions/aggregation/Aggregation6.feature
```
