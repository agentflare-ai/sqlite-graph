# TCK Merge5 - Merge Relationships

**Feature:** Merge5 - Merge relationships
**Total Scenarios:** 29
**Status:** 📋 NOT STARTED (0/29 passing)

---

## Pre-Implementation Baseline

- [ ] Run `scripts/verify_all.sh` and save output to `logs/tck_merge5_baseline.log`
- [ ] Run current test suite to establish baseline: `make -C tests test`
- [ ] Document current state of Merge5 tests
- [ ] Review existing relationship MERGE implementation in `tests/test_merge_clauses.c`

---

## Scenarios

### [1] Creating a relationship
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)-[r:KNOWS]->(b)`
- [ ] Verify relationship is created
- [ ] Verify test passes

### [2] Matching a relationship
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Create relationship first, then MERGE same pattern
- [ ] Verify relationship is matched (not recreated)
- [ ] Verify test passes

### [3] Matching two relationships
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: MERGE pattern with two relationships
- [ ] Verify both relationships handled correctly
- [ ] Verify test passes

### [4] Using bound variables from other updating clause
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Create nodes, then MERGE relationship using bound variables
- [ ] Verify bound variables work in relationship patterns
- [ ] Verify test passes

### [5] Filtering relationships
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: MERGE relationship with property filters
- [ ] Verify filtering works correctly
- [ ] Verify test passes

### [6] Creating relationship when all matches filtered out
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: MERGE with filter that excludes all matches
- [ ] Verify new relationship is created
- [ ] Verify test passes

### [7] Matching incoming relationship
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)<-[r:KNOWS]-(b)`
- [ ] Verify incoming direction works
- [ ] Verify test passes

### [8] Creating relationship with property
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)-[r:KNOWS {since: 2024}]->(b)`
- [ ] Verify relationship created with property
- [ ] Verify test passes

### [9] Creating relationship using merged nodes
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: MERGE nodes and relationship in same pattern
- [ ] Verify nodes and relationship created/matched correctly
- [ ] Verify test passes

### [10] Merge should bind a path
**Status:** PENDING
- [ ] Implement test or mark as NOT IMPLEMENTED
- [ ] Test: `p = MERGE (a)-[r]->(b)` (path binding)
- [ ] If not supported, document limitation
- [ ] Verify test passes or is properly ignored

### [11] Use outgoing direction when unspecified
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)-[r:KNOWS]-(b)` (no direction arrow)
- [ ] Verify defaults to outgoing direction
- [ ] Verify test passes

### [12] Match outgoing relationship when direction unspecified
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Create outgoing rel, then MERGE with unspecified direction
- [ ] Verify match works
- [ ] Verify test passes

### [13] Match both incoming and outgoing relationships when direction unspecified
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Create both directions, MERGE with unspecified direction
- [ ] Verify behavior matches openCypher spec
- [ ] Verify test passes

### [14] Using list properties via variable
**Status:** PENDING
- [ ] Implement test or mark as NOT IMPLEMENTED if lists not supported
- [ ] Test: MERGE with list property
- [ ] Verify test passes or is properly ignored

### [15] Matching using list property
**Status:** PENDING
- [ ] Implement test or mark as NOT IMPLEMENTED if lists not supported
- [ ] Test: Match relationship by list property
- [ ] Verify test passes or is properly ignored

### [16] Aliasing of existing nodes 1
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Node aliasing in relationship patterns
- [ ] Verify aliasing works correctly
- [ ] Verify test passes

### [17] Aliasing of existing nodes 2
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: More complex aliasing scenario
- [ ] Verify aliasing works correctly
- [ ] Verify test passes

### [18] Double aliasing of existing nodes 1
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Double aliasing scenario
- [ ] Verify aliasing works correctly
- [ ] Verify test passes

### [19] Double aliasing of existing nodes 2
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Another double aliasing scenario
- [ ] Verify aliasing works correctly
- [ ] Verify test passes

### [20] Do not match on deleted entities
**Status:** PENDING
- [ ] Implement test or mark as NOT IMPLEMENTED if DELETE not supported
- [ ] Test: DELETE then MERGE should not match deleted entity
- [ ] Verify test passes or is properly ignored

### [21] Do not match on deleted relationships
**Status:** PENDING
- [ ] Implement test or mark as NOT IMPLEMENTED if DELETE not supported
- [ ] Test: DELETE relationship then MERGE should not match
- [ ] Verify test passes or is properly ignored

### [22] Fail when imposing new predicates on a variable that is already bound
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: MERGE with already-bound variable should fail
- [ ] Verify error is thrown
- [ ] Verify test passes

### [23] Fail when merging relationship without type
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)-[r]->(b)` should fail (no type)
- [ ] Verify error is thrown
- [ ] Verify test passes

### [24] Fail when merging relationship without type, no colon
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Another variant of missing type
- [ ] Verify error is thrown
- [ ] Verify test passes

### [25] Fail when merging relationship with more than one type
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)-[r:TYPE1|TYPE2]->(b)` should fail
- [ ] Verify error is thrown
- [ ] Verify test passes

### [26] Fail when merging relationship that is already bound
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: Relationship variable already bound should fail
- [ ] Verify error is thrown
- [ ] Verify test passes

### [27] Fail when using parameter as relationship predicate in MERGE
**Status:** PENDING
- [ ] Implement test or mark as NOT IMPLEMENTED if parameters not supported
- [ ] Test: MERGE with parameter should fail
- [ ] Verify test passes or is properly ignored

### [28] Fail when using variable length relationship in MERGE
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE (a)-[r*1..3]->(b)` should fail
- [ ] Verify error is thrown
- [ ] Verify test passes

### [29] Fail on merging relationship with null property
**Status:** PENDING
- [ ] Implement test in `tests/tck_test_clauses_merge.c`
- [ ] Test: `MERGE ()-[r:KNOWS {prop: NULL}]->()` should fail
- [ ] Verify error is thrown
- [ ] Verify test passes

---

## Post-Implementation Verification

- [ ] Run `make -C tests test` and verify all tests pass
- [ ] Run `scripts/verify_all.sh` to check for memory leaks
- [ ] Compare with baseline - ensure no regressions
- [ ] Update `docs/TCK_MERGE_COVERAGE.md` with Merge5 results
- [ ] Document any remaining failures with clear explanations

---

## Summary

**Target:** 20/29 passing (69%)
**Expected Not Implemented:** ~5 tests (path binding, lists, DELETE, parameters)
**Expected to Pass:** ~20 tests (basic relationship operations)
**Expected to Need Fixes:** ~4 tests (error validation)

**Implementation Priority:**
1. Basic relationship creation and matching (tests 1-9)
2. Direction handling (tests 7, 11-13)
3. Property handling (test 8)
4. Error validation (tests 22-29)
5. Advanced features (tests 10, 14-21)
