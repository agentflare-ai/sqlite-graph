# Set5 - Set multiple properties with a map - Task List

**Feature:** Set5 - Set multiple properties with a map
**Status:** 🔴 NOT STARTED (0/5 scenarios)
**Priority:** MEDIUM
**Last Updated:** 2025-11-17

---

## Overview

Set multiple properties with a map

**Feature File:** `tck/features/clauses/set/Set5.feature`
**Total Scenarios:** 5

**Key Capabilities:**
- Ignore null when setting properties using an appending map
- Overwrite values when using +=
- Retain old values when using +=
- Explicit null values in a map remove old values
- Set an empty map when using += has no effect

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 5 | 01, 02, 03, 04, 05 |

---

## Test Scenarios

### 🔴 [Scenario 01] Ignore null when setting properties using an appending map
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
OPTIONAL MATCH (a:DoesNotExist)
      SET a += {num: 42}
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

### 🔴 [Scenario 02] Overwrite values when using +=
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:X {name: 'A'})
      SET n += {name2: 'C'}
      RETURN n
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

### 🔴 [Scenario 03] Retain old values when using +=
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:X {name: 'A'})
      SET n += {name2: 'B'}
      RETURN n
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

### 🔴 [Scenario 04] Explicit null values in a map remove old values
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:X {name: 'A'})
      SET n += {name: null}
      RETURN n
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

### 🔴 [Scenario 05] Set an empty map when using += has no effect
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (n:X {name: 'A'})
      SET n += { }
      RETURN n
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
**Goal:** Implement basic Set5 functionality
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

- Feature file: `tck/features/clauses/set/Set5.feature`
- Total scenarios: 5
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_set
```

**Feature File Location:**
```
tck/features/clauses/set/Set5.feature
```
