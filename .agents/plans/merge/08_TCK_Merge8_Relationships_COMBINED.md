# TCK Merge8 - Relationships ON MATCH and ON CREATE

**Feature:** Merge8 - Merge relationships - on match and on create
**Total Scenarios:** 1
**Status:** 📋 NOT STARTED (0/1 passing)

---

## Pre-Implementation Baseline

- [ ] Run `scripts/verify_all.sh` and save output to `logs/tck_merge8_baseline.log`
- [ ] Run current test suite to establish baseline: `make -C tests test`
- [ ] Document current state of Merge8 tests

---

## Scenarios

### [1] Using ON CREATE and ON MATCH
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test first execution (should trigger ON CREATE):
  - `MERGE (a)-[r:KNOWS]->(b) ON CREATE SET r.created = true ON MATCH SET r.matched = true`
  - Verify `r.created = true` and `r.matched` is not set
- [ ] Test second execution (should trigger ON MATCH):
  - Same query
  - Verify `r.matched = true` and `r.created` unchanged
- [ ] Verify both clauses work correctly based on create vs match
- [ ] Verify test passes

---

## Post-Implementation Verification

- [ ] Run `make -C tests test` and verify all tests pass
- [ ] Run `scripts/verify_all.sh` to check for memory leaks
- [ ] Compare with baseline - ensure no regressions
- [ ] Update `docs/TCK_MERGE_COVERAGE.md` with Merge8 results
- [ ] Document any remaining failures with clear explanations

---

## Summary

**Target:** 1/1 passing (100%)
**Expected Challenges:**
- Ensuring correct clause triggers based on create vs match
- Property state management across multiple executions

**Dependencies:**
- Merge6 (ON CREATE for relationships) should be working
- Merge7 (ON MATCH for relationships) should be working
