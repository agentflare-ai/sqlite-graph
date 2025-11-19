# TCK Merge7 - Relationships ON MATCH

**Feature:** Merge7 - Merge relationships - on match
**Total Scenarios:** 5
**Status:** 📋 NOT STARTED (0/5 passing)

---

## Pre-Implementation Baseline

- [ ] Run `scripts/verify_all.sh` and save output to `logs/tck_merge7_baseline.log`
- [ ] Run current test suite to establish baseline: `make -C tests test`
- [ ] Document current state of Merge7 tests
- [ ] Review existing ON MATCH implementation for relationships

---

## Scenarios

### [1] Using ON MATCH on created node
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Create relationship pattern first, then MERGE with ON MATCH on node
- [ ] Verify ON MATCH triggers when pattern matches
- [ ] Verify node property updated
- [ ] Verify test passes

### [2] Using ON MATCH on created relationship
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Create relationship first, then MERGE with ON MATCH
- [ ] Verify ON MATCH triggers for matched relationship
- [ ] Verify relationship property updated
- [ ] Verify test passes

### [3] Using ON MATCH on a relationship
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE ()-[r:REL]->() ON MATCH SET r.matched = true`
- [ ] Verify ON MATCH updates relationship properties
- [ ] Verify test passes

### [4] Copying properties from node with ON MATCH
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)-[r:KNOWS]->(b) ON MATCH SET r = a` (property copy)
- [ ] Verify properties copied from node to relationship on match
- [ ] Verify test passes

### [5] Copying properties from literal map with ON MATCH
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE ()-[r:REL]->() ON MATCH SET r = {prop: 'value'}`
- [ ] Verify map literal property copy works on match
- [ ] Verify test passes

---

## Post-Implementation Verification

- [ ] Run `make -C tests test` and verify all tests pass
- [ ] Run `scripts/verify_all.sh` to check for memory leaks
- [ ] Compare with baseline - ensure no regressions
- [ ] Update `docs/TCK_MERGE_COVERAGE.md` with Merge7 results
- [ ] Document any remaining failures with clear explanations

---

## Summary

**Target:** 4/5 passing (80%)
**Expected Challenges:**
- Property copying from nodes/maps
- Ensuring ON MATCH only triggers on match (not create)

**Implementation Priority:**
1. Basic ON MATCH on relationships (tests 1-3)
2. Property copying (tests 4-5)
