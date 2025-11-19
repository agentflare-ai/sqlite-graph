# TCK Merge2 - ON CREATE

**Feature:** Merge2 - Merge node - on create
**Total Scenarios:** 6
**Status:** 🔄 IN PROGRESS (3/6 passing)

---

## Pre-Implementation Baseline

- [ ] Run `scripts/verify_all.sh` and save output to `logs/tck_merge2_baseline.log`
- [ ] Run current test suite to establish baseline: `make -C tests test`
- [ ] Document current state of Merge2 tests

---

## Scenarios

### 🚫 [1] Merge node with label add label on create
**Status:** NOT IMPLEMENTED
- [x] Mark test as TEST_IGNORE_MESSAGE("Label assignment in ON CREATE not yet supported")
- [ ] [FUTURE] Implement `SET n:Label` syntax in ON CREATE
- [ ] [FUTURE] Update parser for label assignment
- [ ] [FUTURE] Test: `MERGE (a) ON CREATE SET a:NewLabel`
- [ ] [FUTURE] Verify label is added on creation

### ✅ [2] ON CREATE on created nodes
**Status:** PASSING
- [x] Implement test in `tests/tck_test_clauses_merge.c`
- [x] Test: `MERGE (a:Node {id: 1}) ON CREATE SET a.created = true`
- [x] Verify property set only on first execution (creation)
- [x] Verify second MERGE doesn't trigger ON CREATE

### ✅ [3] Merge node with label add property on create
**Status:** PASSING
- [x] Implement test in `tests/tck_test_clauses_merge.c`
- [x] Test: `MERGE (a:Person {name: 'Alice'}) ON CREATE SET a.created = 2024`
- [x] Verify property is set when node is created

### ✅ [4] Merge node with label add property on update when it exists
**Status:** PASSING
- [x] Implement test in `tests/tck_test_clauses_merge.c`
- [x] Test: Create node first, then MERGE with ON CREATE
- [x] Verify ON CREATE is NOT triggered when node exists

### ❌ [5] Merge should be able to use properties of bound node in ON CREATE
**Status:** FAILING
- [x] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Debug: Expected 2 nodes but got 1
- [ ] Fix: `MATCH (a) MERGE (b:New {id: 1}) ON CREATE SET b.copied = a.value`
- [ ] Ensure property expressions work in ON CREATE
- [ ] Verify bound node properties are accessible
- [ ] Verify test passes

### ❌ [6] Fail when using undefined variable in ON CREATE
**Status:** FAILING
- [x] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Debug: Should fail but doesn't
- [ ] Fix: Add validation for undefined variables
- [ ] Test: `MERGE (a) ON CREATE SET a.value = b.value` should fail
- [ ] Verify error is thrown for undefined variable 'b'

---

## Post-Implementation Verification

- [ ] Run `make -C tests test` and verify all tests pass
- [ ] Run `scripts/verify_all.sh` to check for memory leaks
- [ ] Compare with baseline - ensure no regressions
- [ ] Update `docs/TCK_MERGE_COVERAGE.md` with Merge2 results
- [ ] Document any remaining failures with clear explanations

---

## Summary

**Passing:** 3/6 (50%)
**Failing:** 2/6 (33%)
**Not Implemented:** 1/6 (17%)

**Priority Fixes:**
1. Property expression evaluation in ON CREATE (test 5)
2. Undefined variable validation (test 6)

**Future Work:**
- Label assignment in ON CREATE
