# TCK Merge3 - ON MATCH

**Feature:** Merge3 - Merge node - on match
**Total Scenarios:** 5
**Status:** 🔄 IN PROGRESS (2/5 passing)

---

## Pre-Implementation Baseline

- [ ] Run `scripts/verify_all.sh` and save output to `logs/tck_merge3_baseline.log`
- [ ] Run current test suite to establish baseline: `make -C tests test`
- [ ] Document current state of Merge3 tests

---

## Scenarios

### 🚫 [1] Merge should be able to set labels on match
**Status:** NOT IMPLEMENTED
- [x] Mark test as TEST_IGNORE_MESSAGE("Label assignment in ON MATCH not yet supported")
- [ ] [FUTURE] Implement `SET n:Label` syntax in ON MATCH
- [ ] [FUTURE] Update parser for label assignment
- [ ] [FUTURE] Test: `MERGE (a) ON MATCH SET a:MatchedLabel`
- [ ] [FUTURE] Verify label is added on match

### 🚫 [2] Merge node with label add label on match when it exists
**Status:** NOT IMPLEMENTED
- [x] Mark test as TEST_IGNORE_MESSAGE("Label assignment in ON MATCH not yet supported")
- [ ] [FUTURE] Implement label assignment on matched nodes
- [ ] [FUTURE] Test: `MERGE (a:Person) ON MATCH SET a:Verified`
- [ ] [FUTURE] Verify multiple labels work correctly

### ✅ [3] Merge node and set property on match
**Status:** PASSING
- [x] Implement test in `tests/tck_test_clauses_merge.c`
- [x] Test: Create node, then `MERGE (a:Person {name: 'Alice'}) ON MATCH SET a.matched = true`
- [x] Verify property is updated when node is matched

### ✅ [4] Merge should be able to use properties of bound node in ON MATCH
**Status:** PASSING
- [x] Implement test in `tests/tck_test_clauses_merge.c`
- [x] Test: `MATCH (a) MERGE (b:Target {id: 1}) ON MATCH SET b.copied = a.value`
- [x] Verify bound node properties accessible in ON MATCH

### ❌ [5] Fail when using undefined variable in ON MATCH
**Status:** FAILING
- [x] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Debug: Should fail but doesn't
- [ ] Fix: Add validation for undefined variables
- [ ] Test: `MERGE (a) ON MATCH SET a.value = b.value` should fail
- [ ] Verify error is thrown for undefined variable 'b'

---

## Post-Implementation Verification

- [ ] Run `make -C tests test` and verify all tests pass
- [ ] Run `scripts/verify_all.sh` to check for memory leaks
- [ ] Compare with baseline - ensure no regressions
- [ ] Update `docs/TCK_MERGE_COVERAGE.md` with Merge3 results
- [ ] Document any remaining failures with clear explanations

---

## Summary

**Passing:** 2/5 (40%)
**Failing:** 1/5 (20%)
**Not Implemented:** 2/5 (40%)

**Priority Fixes:**
1. Undefined variable validation in ON MATCH (test 5)

**Future Work:**
- Label assignment in ON MATCH
