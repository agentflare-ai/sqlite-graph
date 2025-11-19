# WithSkipLimit2 - Limit - Task List

**Feature:** WithSkipLimit2 - Limit
**Status:** 🔴 NOT STARTED (0/4 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Limit

**Feature File:** `tck/features/clauses/with-skip-limit/WithSkipLimit2.feature`
**Total Scenarios:** 4

**Key Capabilities:**
- ORDER BY and LIMIT can be used
- Handle dependencies across WITH with LIMIT
- Connected components succeeding WITH with LIMIT
- Ordering and limiting on aggregate

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 4 | 01, 02, 03, 04 |

---

## Test Scenarios

### 🔴 [Scenario 01] ORDER BY and LIMIT can be used
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A)
      WITH a
      ORDER BY a.name
      LIMIT 1
      MATCH (a)-->(b)
      RETURN a
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

### 🔴 [Scenario 02] Handle dependencies across WITH with LIMIT
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:Begin)
      WITH a.num AS property
        LIMIT 1
      MATCH (b)
      WHERE b.id = property
      RETURN b
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

### 🔴 [Scenario 03] Connected components succeeding WITH with LIMIT
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (n:A)
      WITH n
      LIMIT 1
      MATCH (m:B), (n)-->(x:X)
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

### 🔴 [Scenario 04] Ordering and limiting on aggregate
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH ()-[r1]->(x)
      WITH x, sum(r1.num) AS c
        ORDER BY c LIMIT 1
      RETURN x, c
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
**Goal:** Implement basic WithSkipLimit2 functionality
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

- Feature file: `tck/features/clauses/with-skip-limit/WithSkipLimit2.feature`
- Total scenarios: 4
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_with-skip-limit
```

**Feature File Location:**
```
tck/features/clauses/with-skip-limit/WithSkipLimit2.feature
```
