# TCK Test Plans - Index

This directory contains detailed test plans for each TCK (Technology Compatibility Kit) test group for MERGE functionality.

## Overview

**Total TCK Scenarios:** 75
**Current Status:** 13/75 passing (17%)

***

## Test Groups

### Node Operations (30 scenarios)

#### ✅ [TCK\_Merge1\_Basic\_Node.md](01_TCK_Merge1_Basic_Node.md)

* **Status:** 🔄 IN PROGRESS (8/17 passing - 47%)
* **Focus:** Basic MERGE node creation, matching, and updates
* **Key Findings:** Basic operations work well, property expressions and error validation need fixes

#### ✅ [TCK\_Merge2\_ON\_CREATE.md](02_TCK_Merge2_ON_CREATE.md)

* **Status:** 🔄 IN PROGRESS (3/6 passing - 50%)
* **Focus:** ON CREATE clause for nodes
* **Key Findings:** Basic ON CREATE works, property expressions and validation need fixes

#### ✅ [TCK\_Merge3\_ON\_MATCH.md](03_TCK_Merge3_On_Match.md)

* **Status:** 🔄 IN PROGRESS (2/5 passing - 40%)
* **Focus:** ON MATCH clause for nodes
* **Key Findings:** Basic ON MATCH works, error validation needs fixes

#### ⚠️ [TCK\_Merge4\_ON\_MATCH\_ON\_CREATE.md](04_TCK_Merge4_ON_Match_On_Create.md)

* **Status:** ⚠️ NEEDS WORK (0/2 passing - 0%)
* **Focus:** Combined ON MATCH and ON CREATE for nodes
* **Key Findings:** Property expressions need fixes

**Node Operations Summary:** 13/30 passing (43%)

***

### Relationship Operations (41 scenarios)

#### 📋 [TCK\_Merge5\_Relationships.md](05_TCK_Merge5_Relationships.md)

* **Status:** 📋 NOT STARTED (0/29 target: 20 passing)
* **Focus:** Basic relationship MERGE patterns
* **Key Areas:** Creation, matching, direction, properties, error handling

#### 📋 [TCK\_Merge6\_Relationships\_ON\_CREATE.md](06_TCK_Merge6_Relationships_ON_CREATE.md)

* **Status:** 📋 NOT STARTED (0/6 target: 5 passing)
* **Focus:** ON CREATE for relationships
* **Key Areas:** Property setting, property copying

#### 📋 [TCK\_Merge7\_Relationships\_ON\_MATCH.md](07_TCK_Merge7_Relationships_ON_MATCH.md)

* **Status:** 📋 NOT STARTED (0/5 target: 4 passing)
* **Focus:** ON MATCH for relationships
* **Key Areas:** Property updates, property copying

#### 📋 [TCK\_Merge8\_Relationships\_COMBINED.md](08_TCK_Merge8_Relationships_COMBINED.md)

* **Status:** 📋 NOT STARTED (0/1 target: 1 passing)
* **Focus:** Combined ON MATCH and ON CREATE for relationships
* **Key Areas:** Clause interaction

**Relationship Operations Target:** 30/41 passing (73%)

***

### Advanced Operations (4 scenarios)

#### 📋 [TCK\_Merge9\_Clause\_Interoperation.md](09_TCK_Merge9_Clause_Interoperation.md)

* **Status:** 📋 NOT STARTED (0/4 target: 2-3 passing)
* **Focus:** MERGE with other Cypher clauses
* **Key Areas:** UNWIND, WITH, CREATE interaction

**Advanced Operations Target:** 2-3/4 passing (50-75%)

***

## Progress Tracking

### Current State

```
Node Operations:        13/30  (43%) ✅ Partially Complete
Relationship Ops:        0/41  ( 0%) 📋 Not Started
Advanced Ops:            0/4   ( 0%) 📋 Not Started
─────────────────────────────────────
Total:                  13/75  (17%)
```

### Target State (All Plans Complete)

```
Node Operations:     13-17/30  (43-57%) 🎯
Relationship Ops:       30/41  (73%)    🎯
Advanced Ops:          2-3/4   (50-75%) 🎯
─────────────────────────────────────
Total:              45-50/75   (60-67%)
```

### Long-term Goal

```
Total:                 60+/75  (80%+)  🏆
```

***

## Implementation Priorities

### High Priority (Currently Failing)

1. **Property Expression Evaluation** - Affects 4+ tests
   * `MERGE (b {name: a.name})` where `a` is bound
   * See: Merge1\[11-12], Merge2\[5], Merge4\[2]

2. **Error Validation** - Affects 4+ tests
   * NULL properties, undefined variables, already-bound nodes
   * See: Merge1\[15,17], Merge2\[6], Merge3\[5]

3. **Multiple Element Matching** - Affects 1 test
   * See: Merge1\[7]

### Medium Priority (Not Implemented)

4. **Label Assignment** - Affects 8+ tests
   * `SET n:Label` syntax
   * See: Merge2\[1], Merge3\[1-2], Merge4\[1]

5. **Relationship Operations** - Affects 41 tests
   * Basic MERGE for relationships
   * ON CREATE/ON MATCH for relationships

### Low Priority (Future Features)

6. **Multiple Labels** - Affects 2 tests
7. **Path Binding** - Affects 2 tests
8. **DELETE Clause** - Affects 2 tests
9. **Parameters** - Affects 2 tests
10. **UNWIND/WITH** - Affects 2-3 tests

***

## Testing Workflow

Each test plan follows this structure:

1. **Pre-Implementation Baseline**
   * Run `scripts/verify_all.sh` to establish baseline
   * Run current test suite
   * Document current state

2. **Scenario Implementation**
   * Test each scenario individually
   * Mark as PASSING, FAILING, or NOT IMPLEMENTED
   * Document issues and fixes

3. **Post-Implementation Verification**
   * Run full test suite
   * Run `scripts/verify_all.sh` for memory leak checks
   * Update TCK coverage documentation
   * Ensure no regressions

***

## Documentation

* **Test Implementation:** `tests/tck_test_clauses_merge.c`
* **Coverage Report:** `docs/TCK_MERGE_COVERAGE.md`
* **Baseline Logs:** `logs/tck_merge*.log`

***

## Quick Navigation

* **Start Here:** [TCK\_Merge1\_Basic\_Node.md](01_TCK_Merge1_Basic_Node.md) (Currently in progress)
* **Coverage Report:** `../../docs/TCK_MERGE_COVERAGE.md`
* **Test Code:** `../../tests/tck_test_clauses_merge.c`

***

*Last Updated: 2025-01-14*
*Next: Complete Merge1-4, then move to Merge5-9*
