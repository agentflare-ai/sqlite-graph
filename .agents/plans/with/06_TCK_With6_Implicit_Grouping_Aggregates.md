# With6 - Implicit Grouping with Aggregates - Task List

**Feature:** With6 - Implicit grouping with aggregates
**Status:** 🔴 NOT STARTED (0/9 scenarios)
**Priority:** 🔴 CRITICAL - Aggregation support
**Last Updated:** 2025-11-15

---

## Overview

This feature tests WITH's implicit grouping behavior when using aggregation functions. This is one of the most complex and important WITH features.

**Key Capabilities:**
- Implicit GROUP BY based on non-aggregated expressions
- Aggregation over grouped rows
- Multiple aggregation functions
- Grouping by nodes, relationships, paths
- Complex aggregation expression validation

---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | 7 | 01, 02, 03, 04, 05, 06, 07 |
| 🔴 Error Tests | 2 | 08, 09 |

---

## Test Scenarios

### 🔴 [Scenario 01] Implicit grouping with single expression as grouping key and single aggregation
**Status:** 🔴 NOT STARTED
**Priority:** 🔴 CRITICAL - Foundation

**Test:**
```cypher
# Given: ({name: 'A'}), ({name: 'A'}), ({name: 'B'})
MATCH (a)
WITH a.name AS name, count(*) AS relCount
RETURN name, relCount
```

**Expected:**
| name | relCount |
| 'A'  | 2        |
| 'B'  | 1        |

**TODO:**
- [ ] Implement implicit GROUP BY logic
  - [ ] Detect aggregation functions in projection
  - [ ] Identify non-aggregated expressions (grouping keys)
  - [ ] Create LOGICAL_AGGREGATE operator
  - [ ] Group rows by grouping keys
- [ ] Implement count(*) aggregation
  - [ ] Parse count(*) syntax
  - [ ] Count rows in each group
- [ ] Implement aggregation in physical plan
  - [ ] Map to PHYSICAL_AGGREGATE operator
  - [ ] Hash-based grouping
  - [ ] Maintain aggregate state per group
- [ ] Implement aggregation iterator
  - [ ] Group rows by key
  - [ ] Compute aggregates per group
  - [ ] Emit one row per group
- [ ] Test basic grouping and aggregation

**Dependencies:** With1, With2, count() function
**Estimated Effort:** 5-7 days

---

### 🔴 [Scenario 02] Implicit grouping with single relationship variable as grouping key and single aggregation
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
# Given: ()-[:T1]->(:X), ()-[:T2]->(:X), ()-[:T3]->()
MATCH ()-[r1]->(:X)
WITH r1 AS r2, count(*) AS c
MATCH ()-[r2]->()
RETURN r2 AS rel
```

**Expected:**
| rel   |
| [:T1] |
| [:T2] |

**TODO:**
- [ ] Test grouping by relationship variable
- [ ] Verify relationship equality for grouping
- [ ] Test forwarding grouped relationship to next MATCH

**Dependencies:** Scenario 01
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 03] Implicit grouping with multiple node and relationship variables as grouping key
**Status:** 🔴 NOT STARTED
**Priority:** HIGH

**Test:**
```cypher
# Given: ()-[:T1]->(:X), ()-[:T2]->(:X), ()-[:T3]->()
MATCH (a)-[r1]->(b:X)
WITH a, r1 AS r2, b, count(*) AS c
MATCH (a)-[r2]->(b)
RETURN r2 AS rel
```

**Expected:**
| rel   |
| [:T1] |
| [:T2] |

**TODO:**
- [ ] Test grouping by multiple variables
- [ ] Verify composite key grouping (a, r2, b)
- [ ] Ensure row integrity in groups

**Dependencies:** Scenario 01
**Estimated Effort:** 1-2 days

---

### 🔴 [Scenario 04] Implicit grouping with single path variable as grouping key
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
# Given: (n1 {num: 1})-[:T]->(n2 {num: 2}), (n3 {num: 3})-[:T]->(n4 {num: 4})
MATCH p = ()-[*]->()
WITH count(*) AS count, p AS p
RETURN nodes(p) AS nodes
```

**Expected:**
| nodes                    |
| [({num: 1}), ({num: 2})] |
| [({num: 3}), ({num: 4})] |

**TODO:**
- [ ] Test grouping by path variable
- [ ] Verify path equality for grouping
- [ ] Test nodes() function on grouped paths

**Dependencies:** Scenario 01, path support, variable-length relationships, nodes()
**Estimated Effort:** 2 days

---

### 🔴 [Scenario 05] Handle constants and parameters in aggregation expressions
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
# Parameters: {age: 38}
MATCH (person)
WITH $age + avg(person.age) - 1000 AS agg
RETURN *
```

**Expected:**
| agg  |
| null |

**TODO:**
- [ ] Test parameters in aggregation expressions
- [ ] Test constants mixed with aggregates
- [ ] Test arithmetic with aggregates
- [ ] Handle NULL from empty groups

**Dependencies:** Scenario 01, parameters, avg() function
**Estimated Effort:** 1-2 days

---

### 🔴 [Scenario 06] Handle projected variables in aggregation expressions
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (me:Person)--(you:Person)
WITH me.age AS age, you
WITH age, age + count(you.age) AS agg
RETURN *
```

**Expected:**
| age | agg |
| (empty result)

**TODO:**
- [ ] Test projected variable reuse in aggregation
- [ ] Verify grouping key used in aggregate expression
- [ ] Test with multiple WITH clauses

**Dependencies:** Scenario 01, count()
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 07] Handle projected property accesses in aggregation expressions
**Status:** 🔴 NOT STARTED
**Priority:** MEDIUM

**Test:**
```cypher
MATCH (me:Person)--(you:Person)
WITH me.age AS age, me.age + count(you.age) AS agg
RETURN *
```

**Expected:**
| age | agg |
| (empty result)

**TODO:**
- [ ] Test property access in aggregate expression
- [ ] Verify grouping by property value
- [ ] Test arithmetic with grouping key

**Dependencies:** Scenario 01
**Estimated Effort:** 1 day

---

### 🔴 [Scenario 08] Fail if non-projected variables in aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** HIGH - Error validation

**Test:**
```cypher
MATCH (me:Person)--(you:Person)
WITH me.age + count(you.age) AS agg
RETURN *
```

**Expected:** SyntaxError: AmbiguousAggregationExpression

**TODO:**
- [ ] Implement aggregation expression validation
  - [ ] Detect non-aggregated variables in aggregate expression
  - [ ] Require grouping keys to be projected first
  - [ ] Raise AmbiguousAggregationExpression error
- [ ] Add error code to cypher-errors.h
- [ ] Test error detection at compile time

**Dependencies:** Scenario 01
**Estimated Effort:** 2 days

---

### 🔴 [Scenario 09] Fail if complex non-projected expressions in aggregation expression
**Status:** 🔴 NOT STARTED
**Priority:** HIGH - Error validation

**Test:**
```cypher
MATCH (me:Person)--(you:Person)
WITH me.age + you.age AS grp, me.age + you.age + count(*) AS agg
RETURN *
```

**Expected:** SyntaxError: AmbiguousAggregationExpression

**TODO:**
- [ ] Validate complex expressions with aggregates
  - [ ] Detect when aggregate expression doesn't exactly match grouping key
  - [ ] Require exact match or simpler structure
  - [ ] Raise AmbiguousAggregationExpression error
- [ ] Test with arithmetic expressions

**Dependencies:** Scenario 01
**Estimated Effort:** 1-2 days

---

## Implementation Plan

### Phase 1: Basic Aggregation (Scenarios 01-03)
**Goal:** Implicit grouping and count() working
**Timeline:** 1-2 weeks
**Priority:** 🔴 CRITICAL

**Tasks:**
1. **Aggregation Detection** (2 days)
   - Detect aggregation functions in WITH projection
   - Identify grouping keys (non-aggregated expressions)
   - Create LOGICAL_AGGREGATE operator

2. **Grouping Implementation** (3-4 days)
   - Hash-based grouping by keys
   - Support multiple grouping keys
   - Handle node, relationship, primitive types

3. **count() Implementation** (2 days)
   - Implement count(*) and count(expr)
   - Maintain count per group
   - Test basic scenarios

4. **Testing** (1 day)
   - Scenarios 01-03 passing

**Success Criteria:**
- Scenarios 01-03 passing
- Implicit grouping works
- count() aggregation correct

---

### Phase 2: Advanced Aggregations (Scenarios 04-07)
**Goal:** Path grouping and complex expressions
**Timeline:** 1 week
**Priority:** MEDIUM

**Tasks:**
1. **Path Grouping** (2 days)
   - Implement path equality for grouping
   - Test Scenario 04

2. **Aggregate Expressions** (3 days)
   - Parameters and constants in aggregates
   - Projected variable reuse
   - Arithmetic with aggregates
   - Test Scenarios 05-07

**Success Criteria:**
- Scenarios 04-07 passing
- Complex aggregate expressions work

---

### Phase 3: Error Validation (Scenarios 08-09)
**Goal:** Proper aggregation expression validation
**Timeline:** 3-4 days
**Priority:** HIGH

**Tasks:**
1. **Expression Validation** (2-3 days)
   - Detect ambiguous aggregation expressions
   - Implement validation rules
   - Raise appropriate errors

2. **Testing** (1 day)
   - Scenarios 08-09 passing
   - Error messages clear

**Success Criteria:**
- Scenarios 08-09 passing
- Aggregation validation correct

---

## Dependencies

**External:**
- With1 (basic WITH) 🔴 REQUIRED
- With2 (expressions) 🔴 REQUIRED
- count() function 🔴 REQUIRED
- avg() function (Scenario 05)
- nodes() function (Scenario 04)
- Variable-length paths (Scenario 04)
- Parameters (Scenario 05)

**Internal:**
- Hash table for grouping ✅
- Type equality for grouping keys ✅ (needs path equality)

---

## Success Criteria

### Minimum Viable
- [ ] Scenarios 01-03 passing (basic grouping and count)
- **Target:** 3/9 (33%)

### Production Ready
- [ ] Scenarios 01-07 passing (full aggregation support)
- **Target:** 7/9 (78%)

### Complete
- [ ] All 9 scenarios passing
- **Target:** 9/9 (100%)

---

## Files to Modify

### Parser
- [ ] `src/cypher/cypher-parser.c` - Parse aggregation function calls

### Planner
- [ ] `include/cypher-planner.h` - LOGICAL_AGGREGATE operator
- [ ] `src/cypher/cypher-planner.c` - Aggregation detection and grouping logic
- [ ] `include/cypher-errors.h` - AmbiguousAggregationExpression error

### Physical Plan
- [ ] `include/cypher-executor.h` - PHYSICAL_AGGREGATE operator
- [ ] `src/cypher/cypher-physical-plan.c` - Logical to physical mapping
- [ ] `src/cypher/cypher-iterators.c` - Aggregation iterator with grouping

### Functions
- [ ] `src/cypher/cypher-functions.c` - count(), avg(), other aggregates

### Tests
- [ ] `tests/tck_test_clauses_with.c` - With6 scenarios

---

## Notes

- Aggregation is complex - most challenging WITH feature
- Implicit GROUP BY is Cypher-specific behavior
- Must handle empty groups (return NULL for aggregates)
- Performance critical - hash-based grouping required
- Error validation is important for query correctness

---

## Quick Reference

**Feature File:** `tck/features/clauses/with/With6.feature`
**Test File:** `tests/tck_test_clauses_with.c`
**Scenarios:** 9 total (7 success, 2 error)

**Run Tests:**
```bash
./build/tests/tck_test_clauses_with
```
