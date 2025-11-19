# TCK Merge4 - ON MATCH and ON CREATE

**Feature:** Merge4 - Merge node - on match and on create
**Total Scenarios:** 2
**Status:** ⚠️ NEEDS WORK (0/2 passing)

---

## Pre-Implementation Baseline

- [ ] Run `scripts/verify_all.sh` and save output to `logs/tck_merge4_baseline.log`
- [ ] Run current test suite to establish baseline: `make -C tests test`
- [ ] Document current state of Merge4 tests

---

## Scenarios

### 🚫 [1] Merge should be able to set labels on match and on create
**Status:** NOT IMPLEMENTED
- [x] Mark test as TEST_IGNORE_MESSAGE("Label assignment in ON MATCH/ON CREATE not yet supported")
- [ ] [FUTURE] Implement `SET n:Label` syntax in both ON MATCH and ON CREATE
- [ ] [FUTURE] Update parser for label assignment
- [ ] [FUTURE] Test: `MERGE (a) ON CREATE SET a:Created ON MATCH SET a:Matched`
- [ ] [FUTURE] Verify correct label is added based on create vs match

### ❌ [2] Merge should be able to use properties of bound node in ON MATCH and ON CREATE
**Status:** FAILING
- [x] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Debug: Expected 2 nodes but got 1
- [ ] Fix property expression evaluation in combined ON CREATE/ON MATCH
- [ ] Test: `MATCH (a) MERGE (b:Target {id: 1}) ON CREATE SET b.created = a.value ON MATCH SET b.matched = a.value`
- [ ] Verify first execution triggers ON CREATE
- [ ] Verify second execution triggers ON MATCH
- [ ] Verify test passes

---

## Post-Implementation Verification

- [ ] Run `make -C tests test` and verify all tests pass
- [ ] Run `scripts/verify_all.sh` to check for memory leaks
- [ ] Compare with baseline - ensure no regressions
- [ ] Update `docs/TCK_MERGE_COVERAGE.md` with Merge4 results
- [ ] Document any remaining failures with clear explanations

---

## Summary

**Passing:** 0/2 (0%)
**Failing:** 1/2 (50%)
**Not Implemented:** 1/2 (50%)

**Priority Fixes:**
1. Property expression evaluation in combined ON CREATE/ON MATCH (test 2)

**Future Work:**
- Label assignment in ON MATCH/ON CREATE
