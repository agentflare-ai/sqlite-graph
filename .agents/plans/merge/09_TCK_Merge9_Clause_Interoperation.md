# TCK Merge9 - Clause Interoperation

**Feature:** Merge9 - Merge clause interoperation with other clauses
**Total Scenarios:** 4
**Status:** 📋 NOT STARTED (0/4 passing)

---

## Pre-Implementation Baseline

- [ ] Run `scripts/verify_all.sh` and save output to `logs/tck_merge9_baseline.log`
- [ ] Run current test suite to establish baseline: `make -C tests test`
- [ ] Document current state of Merge9 tests
- [ ] Document current implementation status of other clauses (UNWIND, WITH)

---

## Scenarios

### [1] UNWIND with one MERGE
**Status:** PENDING
- [ ] Check if UNWIND clause is implemented
- [ ] If not implemented, mark as TEST_IGNORE_MESSAGE("UNWIND not yet implemented")
- [ ] If implemented, test: `UNWIND [1, 2, 3] AS x MERGE (n {id: x})`
- [ ] Verify multiple nodes created from unwound list
- [ ] Verify test passes or is properly ignored

### [2] UNWIND with multiple MERGE
**Status:** PENDING
- [ ] Check if UNWIND clause is implemented
- [ ] If not implemented, mark as TEST_IGNORE_MESSAGE("UNWIND not yet implemented")
- [ ] If implemented, test: Multiple MERGE operations with UNWIND
- [ ] Verify complex pattern works correctly
- [ ] Verify test passes or is properly ignored

### [3] Mixing MERGE with CREATE
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `CREATE (a) MERGE (b:Node {id: 1})` or similar pattern
- [ ] Verify MERGE and CREATE can coexist in same query
- [ ] Verify both operations execute correctly
- [ ] Verify test passes

### [4] MERGE after WITH with predicate and WITH with aggregation
**Status:** PENDING
- [ ] Check if WITH clause is fully implemented
- [ ] If not implemented, mark as TEST_IGNORE_MESSAGE("WITH clause not yet implemented")
- [ ] If implemented, test:
  - `MATCH (a) WITH a WHERE a.prop > 5 WITH count(a) as cnt MERGE (n {count: cnt})`
- [ ] Verify WITH + WHERE + aggregation + MERGE work together
- [ ] Verify test passes or is properly ignored

---

## Additional Clause Interaction Tests (If Applicable)

### MERGE + MATCH
**Status:** PENDING
- [ ] Test: `MATCH (a) MERGE (b {prop: a.prop})`
- [ ] Verify MATCH + MERGE interaction works
- [ ] Document if not already covered above

### MERGE + RETURN
**Status:** PENDING
- [ ] Test: `MERGE (a:Node {id: 1}) RETURN a`
- [ ] Verify RETURN works with MERGE
- [ ] Document if not already covered in other tests

### MERGE + WHERE (if WHERE clause exists)
**Status:** PENDING
- [ ] Check if WHERE clause works with MERGE
- [ ] Test: `MERGE (a) WHERE a.prop = 'value'` (if valid syntax)
- [ ] Document behavior or limitations

---

## Post-Implementation Verification

- [ ] Run `make -C tests test` and verify all tests pass
- [ ] Run `scripts/verify_all.sh` to check for memory leaks
- [ ] Compare with baseline - ensure no regressions
- [ ] Update `docs/TCK_MERGE_COVERAGE.md` with Merge9 results
- [ ] Document any remaining failures with clear explanations
- [ ] Create final TCK compliance report

---

## Final TCK Compliance Report

- [ ] Calculate overall passing rate across all Merge1-9 tests
- [ ] Document total: X/75 passing (X%)
- [ ] List all NOT IMPLEMENTED features with explanations
- [ ] List all FAILING tests with root causes
- [ ] Create roadmap for achieving 80%+ compliance
- [ ] Update `docs/TCK_MERGE_COVERAGE.md` with final summary

---

## Summary

**Target:** 2-3/4 passing (50-75%)
**Expected Challenges:**
- UNWIND may not be implemented
- WITH clause may have limited support
- Complex multi-clause query planning

**Expected Not Implemented:**
- Tests 1-2 (if UNWIND not available)
- Test 4 (if WITH not fully available)

**Expected to Pass:**
- Test 3 (MERGE + CREATE)
- Potentially test 4 (if WITH is implemented)

**Dependencies:**
- UNWIND clause implementation
- WITH clause implementation
- Multi-clause query planner
