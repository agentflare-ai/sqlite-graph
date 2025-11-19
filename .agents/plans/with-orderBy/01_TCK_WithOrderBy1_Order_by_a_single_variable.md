# WithOrderBy1 - Order by a single variable - Task List

**Feature:** WithOrderBy1 - Order by a single variable
**Status:** 🔴 NOT STARTED (0/22 scenarios)
**Priority:** LOW
**Last Updated:** 2025-11-17

---

## Overview

Order by a single variable

**Feature File:** `tck/features/clauses/with-orderBy/WithOrderBy1.feature`
**Total Scenarios:** 22

**Key Capabilities:**
- Sort booleans in ascending order
- Sort booleans in descending order
- Sort integers in ascending order
- Sort integers in descending order
- Sort floats in ascending order
- ... and 17 more scenarios

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 22 | 01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 |

---

## Test Scenarios

### 🔴 [Scenario 01] Sort booleans in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false] AS bools
      WITH bools
        ORDER BY bools
        LIMIT 1
      RETURN bools
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

### 🔴 [Scenario 02] Sort booleans in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [true, false] AS bools
      WITH bools
        ORDER BY bools DESC
        LIMIT 1
      RETURN bools
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

### 🔴 [Scenario 03] Sort integers in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [1, 3, 2] AS ints
      WITH ints
        ORDER BY ints
        LIMIT 2
      RETURN ints
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

### 🔴 [Scenario 04] Sort integers in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [1, 3, 2] AS ints
      WITH ints
        ORDER BY ints DESC
        LIMIT 2
      RETURN ints
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

### 🔴 [Scenario 05] Sort floats in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [1.5, 1.3, 999.99] AS floats
      WITH floats
        ORDER BY floats
        LIMIT 2
      RETURN floats
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

### 🔴 [Scenario 06] Sort floats in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [1.5, 1.3, 999.99] AS floats
      WITH floats
        ORDER BY floats DESC
        LIMIT 2
      RETURN floats
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

### 🔴 [Scenario 07] Sort strings in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND ['.*', '', ' ', 'one'] AS strings
      WITH strings
        ORDER BY strings
        LIMIT 2
      RETURN strings
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

### 🔴 [Scenario 08] Sort strings in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND ['.*', '', ' ', 'one'] AS strings
      WITH strings
        ORDER BY strings DESC
        LIMIT 2
      RETURN strings
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

### 🔴 [Scenario 09] Sort lists in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [[], ['a'], ['a', 1], [1], [1, 'a'], [1, null], [null, 1], [null, 2]] AS lists
      WITH lists
        ORDER BY lists
        LIMIT 4
      RETURN lists
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

### 🔴 [Scenario 10] Sort lists in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [[], ['a'], ['a', 1], [1], [1, 'a'], [1, null], [null, 1], [null, 2]] AS lists
      WITH lists
        ORDER BY lists DESC
        LIMIT 4
      RETURN lists
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

### 🔴 [Scenario 11] Sort dates in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [date({year: 1910, month: 5, day: 6}),
              date({year: 1980, month: 12, day: 24}),
              date({year: 1984, month: 10, day: 12}),
              date({year: 1985, month: 5, day: 6}),
              date({year: 1980, month: 10, day: 24}),
              date({year: 1984, month: 10, day: 11})] AS dates
      WITH dates
        ORDER BY dates
        LIMIT 2
      RETURN dates
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

### 🔴 [Scenario 12] Sort dates in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [date({year: 1910, month: 5, day: 6}),
              date({year: 1980, month: 12, day: 24}),
              date({year: 1984, month: 10, day: 12}),
              date({year: 1985, month: 5, day: 6}),
              date({year: 1980, month: 10, day: 24}),
              date({year: 1984, month: 10, day: 11})] AS dates
      WITH dates
        ORDER BY dates DESC
        LIMIT 2
      RETURN dates
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

### 🔴 [Scenario 13] Sort local times in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [localtime({hour: 10, minute: 35}),
              localtime({hour: 12, minute: 31, second: 14, nanosecond: 645876123}),
              localtime({hour: 12, minute: 31, second: 14, nanosecond: 645876124}),
              localtime({hour: 12, minute: 35, second: 13}),
              localtime({hour: 12, minute: 30, second: 14, nanosecond: 645876123})] AS localtimes
      WITH localtimes
        ORDER BY localtimes
        LIMIT 3
      RETURN localtimes
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

### 🔴 [Scenario 14] Sort local times in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [localtime({hour: 10, minute: 35}),
              localtime({hour: 12, minute: 31, second: 14, nanosecond: 645876123}),
              localtime({hour: 12, minute: 31, second: 14, nanosecond: 645876124}),
              localtime({hour: 12, minute: 35, second: 13}),
              localtime({hour: 12, minute: 30, second: 14, nanosecond: 645876123})] AS localtimes
      WITH localtimes
        ORDER BY localtimes DESC
        LIMIT 3
      RETURN localtimes
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

### 🔴 [Scenario 15] Sort times in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [time({hour: 10, minute: 35, timezone: '-08:00'}),
              time({hour: 12, minute: 31, second: 14, nanosecond: 645876123, timezone: '+01:00'}),
              time({hour: 12, minute: 31, second: 14, nanosecond: 645876124, timezone: '+01:00'}),
              time({hour: 12, minute: 35, second: 15, timezone: '+05:00'}),
              time({hour: 12, minute: 30, second: 14, nanosecond: 645876123, timezone: '+01:01'})] AS times
      WITH times
        ORDER BY times
        LIMIT 3
      RETURN times
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

### 🔴 [Scenario 16] Sort times in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [time({hour: 10, minute: 35, timezone: '-08:00'}),
              time({hour: 12, minute: 31, second: 14, nanosecond: 645876123, timezone: '+01:00'}),
              time({hour: 12, minute: 31, second: 14, nanosecond: 645876124, timezone: '+01:00'}),
              time({hour: 12, minute: 35, second: 15, timezone: '+05:00'}),
              time({hour: 12, minute: 30, second: 14, nanosecond: 645876123, timezone: '+01:01'})] AS times
      WITH times
        ORDER BY times DESC
        LIMIT 3
      RETURN times
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

### 🔴 [Scenario 17] Sort local date times in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [localdatetime({year: 1984, month: 10, day: 11, hour: 12, minute: 30, second: 14, nanosecond: 12}),
              localdatetime({year: 1984, month: 10, day: 11, hour: 12, minute: 31, second: 14, nanosecond: 645876123}),
              localdatetime({year: 1, month: 1, day: 1, hour: 1, minute: 1, second: 1, nanosecond: 1}),
              localdatetime({year: 9999, month: 9, day: 9, hour: 9, minute: 59, second: 59, nanosecond: 999999999}),
              localdatetime({year: 1980, month: 12, day: 11, hour: 12, minute: 31, second: 14})] AS localdatetimes
      WITH localdatetimes
        ORDER BY localdatetimes
        LIMIT 3
      RETURN localdatetimes
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

### 🔴 [Scenario 18] Sort local date times in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [localdatetime({year: 1984, month: 10, day: 11, hour: 12, minute: 30, second: 14, nanosecond: 12}),
              localdatetime({year: 1984, month: 10, day: 11, hour: 12, minute: 31, second: 14, nanosecond: 645876123}),
              localdatetime({year: 1, month: 1, day: 1, hour: 1, minute: 1, second: 1, nanosecond: 1}),
              localdatetime({year: 9999, month: 9, day: 9, hour: 9, minute: 59, second: 59, nanosecond: 999999999}),
              localdatetime({year: 1980, month: 12, day: 11, hour: 12, minute: 31, second: 14})] AS localdatetimes
      WITH localdatetimes
        ORDER BY localdatetimes DESC
        LIMIT 3
      RETURN localdatetimes
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

### 🔴 [Scenario 19] Sort date times in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [datetime({year: 1984, month: 10, day: 11, hour: 12, minute: 30, second: 14, nanosecond: 12, timezone: '+00:15'}),
              datetime({year: 1984, month: 10, day: 11, hour: 12, minute: 31, second: 14, nanosecond: 645876123, timezone: '+00:17'}),
              datetime({year: 1, month: 1, day: 1, hour: 1, minute: 1, second: 1, nanosecond: 1, timezone: '-11:59'}),
              datetime({year: 9999, month: 9, day: 9, hour: 9, minute: 59, second: 59, nanosecond: 999999999, timezone: '+11:59'}),
              datetime({year: 1980, month: 12, day: 11, hour: 12, minute: 31, second: 14, timezone: '-11:59'})] AS datetimes
      WITH datetimes
        ORDER BY datetimes
        LIMIT 3
      RETURN datetimes
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

### 🔴 [Scenario 20] Sort date times in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
UNWIND [datetime({year: 1984, month: 10, day: 11, hour: 12, minute: 30, second: 14, nanosecond: 12, timezone: '+00:15'}),
              datetime({year: 1984, month: 10, day: 11, hour: 12, minute: 31, second: 14, nanosecond: 645876123, timezone: '+00:17'}),
              datetime({year: 1, month: 1, day: 1, hour: 1, minute: 1, second: 1, nanosecond: 1, timezone: '-11:59'}),
              datetime({year: 9999, month: 9, day: 9, hour: 9, minute: 59, second: 59, nanosecond: 999999999, timezone: '+11:59'}),
              datetime({year: 1980, month: 12, day: 11, hour: 12, minute: 31, second: 14, timezone: '-11:59'})] AS datetimes
      WITH datetimes
        ORDER BY datetimes DESC
        LIMIT 3
      RETURN datetimes
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

### 🔴 [Scenario 21] Sort distinct types in ascending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH p = (n:N)-[r:REL]->()
      UNWIND [n, r, p, 1.5, ['list'], 'text', null, false, 0.0 / 0.0, {a: 'map'}] AS types
      WITH types
        ORDER BY types
        LIMIT 5
      RETURN types
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

### 🔴 [Scenario 22] Sort distinct types in descending order
**Status:** 🔴 NOT STARTED
**Priority:** LOW

**Test:**
```cypher
MATCH (a:A), (b:B), (c:C)
      WITH a, b
      WITH a
        ORDER BY <sort>
      RETURN a
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
**Goal:** Implement basic WithOrderBy1 functionality
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

- Feature file: `tck/features/clauses/with-orderBy/WithOrderBy1.feature`
- Total scenarios: 22
- This plan was auto-generated on 2025-11-17
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_with-orderBy
```

**Feature File Location:**
```
tck/features/clauses/with-orderBy/WithOrderBy1.feature
```
