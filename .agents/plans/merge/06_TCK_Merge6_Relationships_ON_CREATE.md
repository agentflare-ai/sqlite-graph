# TCK Merge6 - Relationships ON CREATE

**Feature:** Merge6 - Merge relationships - on create
**Total Scenarios:** 6
**Status:** 📋 NOT STARTED (0/6 passing)

---

## Pre-Implementation Baseline

- [ ] Run `scripts/verify_all.sh` and save output to `logs/tck_merge6_baseline.log`
- [ ] Run current test suite to establish baseline: `make -C tests test`
- [ ] Document current state of Merge6 tests
- [ ] Review existing ON CREATE implementation for relationships

---

## Scenarios

### [1] Using ON CREATE on a node
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)-[r:KNOWS]->(b) ON CREATE SET a.created = true`
- [ ] Verify ON CREATE works on nodes in relationship pattern
- [ ] Verify test passes

### [2] Using ON CREATE on a relationship
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)-[r:KNOWS]->(b) ON CREATE SET r.created = true`
- [ ] Verify ON CREATE sets relationship properties
- [ ] Verify property only set on creation (not match)
- [ ] Verify test passes

### [3] Updating one property with ON CREATE
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE ()-[r:REL]->() ON CREATE SET r.prop = value`
- [ ] Verify single property update works
- [ ] Verify test passes

### [4] Null-setting one property with ON CREATE
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE ()-[r:REL]->() ON CREATE SET r.prop = NULL`
- [ ] Verify NULL value handling
- [ ] Verify test passes

### [5] Copying properties from node with ON CREATE
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)-[r:KNOWS]->(b) ON CREATE SET r = a` (property copy)
- [ ] Verify properties copied from node to relationship
- [ ] Verify test passes

### [6] Copying properties from literal map with ON CREATE
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE ()-[r:REL]->() ON CREATE SET r = {prop: 'value'}`
- [ ] Verify map literal property copy works
- [ ] Verify test passes

---

## Post-Implementation Verification

- [ ] Run `make -C tests test` and verify all tests pass
- [ ] Run `scripts/verify_all.sh` to check for memory leaks
- [ ] Compare with baseline - ensure no regressions
- [ ] Update `docs/TCK_MERGE_COVERAGE.md` with Merge6 results
- [ ] Document any remaining failures with clear explanations

---

## Summary

**Target:** 5/6 passing (83%)
**Expected Challenges:**
- Property copying from nodes/maps
- Relationship property handling in ON CREATE

**Implementation Priority:**
1. Basic ON CREATE on relationships (tests 1-2)
2. Property updates (tests 3-4)
3. Property copying (tests 5-6)
