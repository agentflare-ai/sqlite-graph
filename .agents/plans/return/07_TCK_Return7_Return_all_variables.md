# Return7 - Return all variables - Task List

**Feature:** Return7 - Return all variables
**Status:** 🔴 NOT STARTED (0/2 scenarios)
**Priority:** HIGH
**Last Updated:** 2025-11-17

---

## Overview

Return all variables

**Feature File:** `tck/features/clauses/return/Return7.feature`
**Total Scenarios:** 2

**Key Capabilities:**
- Return all variables
- Fail when using RETURN * without variables in scope

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 2 | 01, 02 |

---

## Test Scenarios

### 🔴 [Scenario 01] Return all variables
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH p = (a:Start)-->(b)
      RETURN *
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

### 🔴 [Scenario 02] Fail when using RETURN * without variables in scope
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
MATCH ()
      RETURN *
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
**Goal:** Implement basic Return7 functionality
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

- Feature file: `tck/features/clauses/return/Return7.feature`
- Total scenarios: 2
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_return
```

**Feature File Location:**
```
tck/features/clauses/return/Return7.feature
```
