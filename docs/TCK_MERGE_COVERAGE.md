# TCK MERGE Test Coverage

**Last Updated:** 2025-01-14
**Phase:** 6.5 - MERGE Relationship Tests (Merge5)

## Summary

- **Total TCK Scenarios (Merge1-5):** 59
- **Passing:** 19 (32%)
- **Failing:** 26 (44%)
- **Not Implemented (Ignored):** 14 (24%)

## Test Results by Category

### Merge1 - Basic Merge Node (17 tests)

| Test | Description | Status | Notes |
|------|-------------|--------|-------|
| `[1]` | Merge node when no nodes exist | ✅ **PASS** | |
| `[2]` | Merge node with label | ✅ **PASS** | |
| `[3]` | Merge node with label when it exists | ✅ **PASS** | |
| `[4]` | Merge node should create when it doesn't match, properties | ✅ **PASS** | |
| `[5]` | Merge node should create when it doesn't match, properties and label | ✅ **PASS** | |
| `[6]` | Merge node with prop and label | ✅ **PASS** | |
| `[7]` | Merge should work when finding multiple elements | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[8]` | Merge should handle argument properly | ✅ **PASS** | |
| `[9]` | Merge should support updates while merging | ✅ **PASS** | |
| `[10]` | Merge must properly handle multiple labels | 🚫 **NOT IMPL** | Multiple labels not yet supported |
| `[11]` | Merge should be able to merge using property of bound node | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[12]` | Merge should be able to merge using property of freshly created node | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[13]` | Merge should bind a path | 🚫 **NOT IMPL** | Path binding not implemented |
| `[14]` | Merges should not be able to match on deleted nodes | 🚫 **NOT IMPL** | DELETE clause not implemented |
| `[15]` | Fail when merge a node that is already bound | ❌ **FAIL** | Should error but doesn't |
| `[16]` | Fail when using parameter as node predicate in MERGE | 🚫 **NOT IMPL** | Parameters not implemented |
| `[17]` | Fail on merging node with null property | ❌ **FAIL** | Should error but doesn't |

**Merge1 Summary:** 8 passing, 5 failing, 4 not implemented (47% pass rate)

### Merge2 - ON CREATE (6 tests)

| Test | Description | Status | Notes |
|------|-------------|--------|-------|
| `[1]` | Merge node with label add label on create | 🚫 **NOT IMPL** | Label assignment in ON CREATE not supported |
| `[2]` | ON CREATE on created nodes | ✅ **PASS** | |
| `[3]` | Merge node with label add property on create | ✅ **PASS** | |
| `[4]` | Merge node with label add property on update when it exists | ✅ **PASS** | |
| `[5]` | Merge should be able to use properties of bound node in ON CREATE | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[6]` | Fail when using undefined variable in ON CREATE | ❌ **FAIL** | Should error but doesn't |

**Merge2 Summary:** 3 passing, 2 failing, 1 not implemented (50% pass rate)

### Merge3 - ON MATCH (5 tests)

| Test | Description | Status | Notes |
|------|-------------|--------|-------|
| `[1]` | Merge should be able to set labels on match | 🚫 **NOT IMPL** | Label assignment in ON MATCH not supported |
| `[2]` | Merge node with label add label on match when it exists | 🚫 **NOT IMPL** | Label assignment in ON MATCH not supported |
| `[3]` | Merge node and set property on match | ✅ **PASS** | |
| `[4]` | Merge should be able to use properties of bound node in ON MATCH | ✅ **PASS** | |
| `[5]` | Fail when using undefined variable in ON MATCH | ❌ **FAIL** | Should error but doesn't |

**Merge3 Summary:** 2 passing, 1 failing, 2 not implemented (40% pass rate)

### Merge4 - ON MATCH and ON CREATE (2 tests)

| Test | Description | Status | Notes |
|------|-------------|--------|-------|
| `[1]` | Merge should be able to set labels on match and on create | 🚫 **NOT IMPL** | Label assignment not supported |
| `[2]` | Merge should be able to use properties of bound node in ON MATCH and ON CREATE | ❌ **FAIL** | Expected 2 nodes, got 1 |

**Merge4 Summary:** 0 passing, 1 failing, 1 not implemented (0% pass rate)

### Merge5 - Merge Relationships (29 tests)

| Test | Description | Status | Notes |
|------|-------------|--------|-------|
| `[1]` | Creating a relationship | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[2]` | Matching a relationship | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[3]` | Matching two relationships | ❌ **FAIL** | Expected 3 nodes, got 1 |
| `[4]` | Using bound variables from other updating clause | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[5]` | Filtering relationships | ❌ **FAIL** | Property filtering issue |
| `[6]` | Creating relationship when all matches filtered out | ✅ **PASS** | |
| `[7]` | Matching incoming relationship | ❌ **FAIL** | Parser error: expected node pattern |
| `[8]` | Creating relationship with property | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[9]` | Creating relationship using merged nodes | ✅ **PASS** | |
| `[10]` | Merge should bind a path | 🚫 **NOT IMPL** | Path binding not implemented |
| `[11]` | Use outgoing direction when unspecified | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[12]` | Match outgoing relationship when direction unspecified | ✅ **PASS** | |
| `[13]` | Match both incoming and outgoing when direction unspecified | ❌ **FAIL** | Expected 2 edges, got 1 |
| `[14]` | Using list properties via variable | 🚫 **NOT IMPL** | List properties not implemented |
| `[15]` | Matching using list property | 🚫 **NOT IMPL** | List properties not implemented |
| `[16]` | Aliasing of existing nodes 1 | ✅ **PASS** | |
| `[17]` | Aliasing of existing nodes 2 | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[18]` | Double aliasing of existing nodes 1 | ❌ **FAIL** | Expected 1 edge, got 0 |
| `[19]` | Double aliasing of existing nodes 2 | ❌ **FAIL** | Expected 2 nodes, got 1 |
| `[20]` | Do not match on deleted entities | 🚫 **NOT IMPL** | DELETE clause not implemented |
| `[21]` | Do not match on deleted relationships | 🚫 **NOT IMPL** | DELETE clause not implemented |
| `[22]` | Fail when imposing new predicates on already-bound variable | ❌ **FAIL** | Should error but doesn't |
| `[23]` | Fail when merging relationship without type | ❌ **FAIL** | Should error but doesn't |
| `[24]` | Fail when merging relationship without type, no colon | ❌ **FAIL** | Should error but doesn't |
| `[25]` | Fail when merging relationship with more than one type | ✅ **PASS** | |
| `[26]` | Fail when relationship already bound | ❌ **FAIL** | Should error but doesn't |
| `[27]` | Fail when using parameter as relationship predicate | 🚫 **NOT IMPL** | Parameters not implemented |
| `[28]` | Fail when using variable length relationship in MERGE | ✅ **PASS** | |
| `[29]` | Fail on merging relationship with null property | ❌ **FAIL** | Should error but doesn't |

**Merge5 Summary:** 6 passing, 17 failing, 6 not implemented (21% pass rate)

## Implementation Gaps

### 1. Multiple Labels
**Tests Affected:** Merge1-10

Multiple label syntax (e.g., `MERGE (n:Label1:Label2)`) is not yet implemented. This is a feature gap that requires parser and storage layer updates.

### 2. Label Assignment
**Tests Affected:** Merge2-01, Merge3-01, Merge3-02, Merge4-01

The `SET n:Label` syntax for adding labels to nodes is not implemented. This affects tests for ON CREATE and ON MATCH label operations.

### 3. Path Binding
**Tests Affected:** Merge1-13, Merge5-10

Path binding (e.g., `p = MERGE (a)`) is not yet implemented.

### 4. DELETE Clause
**Tests Affected:** Merge1-14, Merge5-20, Merge5-21

The DELETE clause and deletion tracking are not fully implemented, so tests involving deleted nodes and relationships cannot be validated.

### 5. Parameters
**Tests Affected:** Merge1-16, Merge5-27

Query parameters are not yet implemented in the system.

### 6. List Properties
**Tests Affected:** Merge5-14, Merge5-15

List/array properties are not currently implemented in the system.

### 7. Error Validation Issues
**Tests Affected:** Merge1-15, Merge1-17, Merge2-06, Merge3-05, Merge5-22, Merge5-23, Merge5-24, Merge5-26, Merge5-29

Several error validation tests are failing because the system doesn't properly reject invalid queries:
- Merging an already-bound node/relationship should fail
- Merging with NULL property values should fail
- Merging relationships without a type should fail
- Using undefined variables should fail

**Root Cause:** Error validation logic needs to be strengthened in the MERGE implementation.

### 8. Property Expression Evaluation in MERGE
**Tests Affected:** Merge1-11, Merge1-12, Merge2-05, Merge4-02

Tests that use expressions to copy properties from one node to another during MERGE are failing. The issue appears to be with evaluating property access expressions (e.g., `a.name`) within MERGE patterns.

**Example Failing Query:**
```cypher
MATCH (a {id: 1})
MERGE (b {name: a.name})  // Should use property from bound node 'a'
```

**Root Cause:** MERGE pattern matching may not be properly evaluating expressions that reference bound variables.

### 9. MERGE Relationship with Bare Patterns
**Tests Affected:** Merge5-01, Merge5-02, Merge5-03, Merge5-04, Merge5-08, Merge5-11, Merge5-17, Merge5-19

MERGE patterns with bare relationships (no node labels or properties) are only creating 1 node instead of 2.

**Example Failing Query:**
```cypher
MERGE (a)-[r:KNOWS]->(b)  // Creates 1 node instead of 2
```

**Root Cause:** The MERGE relationship implementation may not be properly creating both endpoint nodes when they don't have identifying properties. The system might be treating both endpoint variables as referring to the same node.

### 10. Incoming Relationship Direction Parsing
**Tests Affected:** Merge5-07

Parser error when using incoming relationship direction in MERGE.

**Example Failing Query:**
```cypher
MERGE (a)<-[r:KNOWS]-(b)  // Parser error: expected node pattern
```

**Root Cause:** The Cypher parser may not support incoming relationships (`<-`) in MERGE patterns.

### 11. MATCH with MERGE Relationship Issues
**Tests Affected:** Merge5-18

When using MATCH to bind a variable and then MERGE a relationship using that variable, the operation fails.

**Example Failing Query:**
```cypher
MATCH (a:Person {name: 'Alice'})
MERGE (a)-[r:KNOWS]->(a)  // Should create self-relationship
```

**Root Cause:** The MERGE implementation may have issues when trying to create relationships where both endpoints are the same pre-bound node.

## Recommendations

### Short-term Fixes (High Priority)

1. **Fix MERGE Relationship with Bare Patterns** (CRITICAL)
   - Tests: Merge5-01, Merge5-02, Merge5-03, Merge5-04, Merge5-08, Merge5-11, Merge5-17, Merge5-19
   - Impact: Would add 8 more passing tests in Merge5
   - Effort: Medium - needs investigation of node creation logic
   - **This is blocking most Merge5 tests**

2. **Fix Incoming Relationship Direction Parsing** (High Priority)
   - Test: Merge5-07
   - Impact: Would add 1 more passing test
   - Effort: Low - parser enhancement needed

3. **Improve Error Validation** (High Priority)
   - Tests: Merge1-15, Merge1-17, Merge2-06, Merge3-05, Merge5-22, Merge5-23, Merge5-24, Merge5-26, Merge5-29
   - Impact: Would add 9 more passing tests total
   - Effort: Low to Medium

4. **Fix Property Expression Evaluation** (High Priority)
   - Tests: Merge1-11, Merge1-12, Merge2-05, Merge4-02
   - Impact: Would add 4 more passing tests
   - Effort: Medium

5. **Fix MATCH with MERGE Relationship (self-relationships)** (Medium Priority)
   - Test: Merge5-18
   - Impact: Would add 1 more passing test
   - Effort: Medium

### Long-term Features

1. **Implement Label Assignment** (Merge2-01, Merge3-01, Merge3-02, Merge4-01)
2. **Implement Multiple Labels** (Merge1-10)
3. **Implement Path Binding** (Merge1-13, Merge5-10)
4. **Implement DELETE Clause** (Merge1-14, Merge5-20, Merge5-21)
5. **Implement Parameters** (Merge1-16, Merge5-27)
6. **Implement List Properties** (Merge5-14, Merge5-15)

## Roadmap for Higher Compliance

### Merge1-4 (Node MERGE) - Target: 80%+ (24/30 tests)

1. ✅ **Phase 6.1 Complete:** 13 tests passing (43%)
2. 🔄 **Fix Property Expressions:** +4 tests = 17 passing (57%)
3. 🔄 **Fix Error Validation (nodes):** +4 tests = 21 passing (70%)
4. 🔄 **Fix Multiple Elements:** +1 test = 22 passing (73%)
5. 📋 **Implement Label Assignment:** +4 tests = 26 passing (87%)

### Merge5 (Relationship MERGE) - Target: 60%+ (17/29 tests)

1. ✅ **Phase 6.5 Complete:** 6 tests passing (21%)
2. 🔄 **Fix Bare Relationship Patterns:** +8 tests = 14 passing (48%)
3. 🔄 **Fix Error Validation (rels):** +5 tests = 19 passing (66%)
4. 🔄 **Fix Incoming Direction + Self-rels:** +2 tests = 21 passing (72%)

## Notes

- TCK tests are more comprehensive than custom integration tests
- Many edge cases discovered through this exercise
- The current **32% overall pass rate (19/59)** represents foundational MERGE support for both nodes and relationships
- Most failures are due to missing features or specific bugs rather than fundamentally incorrect behavior
- Error handling needs improvement to properly reject invalid queries
- **Merge5 key finding:** The core relationship MERGE implementation exists and works for patterns with node properties, but fails for bare patterns

### Key Insights from Merge5

1. **Core functionality is present:** Relationship MERGE works when nodes have identifying properties
2. **Main blocker:** Bare relationship patterns (`MERGE (a)-[r:TYPE]->(b)`) don't create both endpoint nodes correctly
3. **Parser limitation:** Incoming relationship direction (`<-`) not supported in MERGE
4. **Error validation weak:** Most error cases don't properly fail as expected

---

*Generated as part of Phase 6.1 - TCK Test Enablement (Merge1-4)*
*Updated in Phase 6.5 - TCK Merge5 Implementation*
