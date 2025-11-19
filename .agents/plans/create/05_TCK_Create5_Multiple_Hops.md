# TCK Create5 - Multiple Hops Create Patterns

**Feature:** Create5 - Multiple hops create patterns
**Total Scenarios:** 5
**Status:** 🔄 PARTIAL (1/5 passing, 20%)

---

## Implementation Summary

**Date:** 2025-01-14
**Result:** 1/5 passing (20%), 4 failing

### ✅ Passing Tests (1)
- Test 1: Create a pattern with multiple hops (forward direction only)

### ❌ Main Failures (4)

**Category: Reverse/Mixed Direction Arrows** - Tests 2-5
- Issue: Parser fails on any pattern with `<-` in CREATE
- Error: "Expected node pattern after relationship near '-'"
- Root cause: Reverse arrow syntax not supported in CREATE

### Key Findings

1. **Forward chaining works**: `()-[:R]->()-[:R]->()` ✅
2. **Reverse arrows fail**: `()<-[:R]-()<-[:R]-()` ❌
3. **Mixed directions fail**: Combinations of `->` and `<-` ❌
4. **Same root cause**: All failures are parser-level issues
5. **Simple fix**: Once reverse arrows work, all should pass

---

## Scenarios

### ✅ [1] Create a pattern with multiple hops
**Status:** PASSING
- Query: `CREATE (:A)-[:R]->(:B)-[:R]->(:C)`
- Expected: 3 nodes, 2 relationships
- Result: ✅ Works correctly
- Verify: `MATCH (a:A)-[:R]->(b:B)-[:R]->(c:C) RETURN a, b, c`

### ❌ [2] Create a pattern with multiple hops in the reverse direction
**Status:** FAILING
- Query: `CREATE (:A)<-[:R]-(:B)<-[:R]-(:C)`
- Expected: 3 nodes, 2 relationships
- Error: "Expected node pattern after relationship near '-'"
- Blocker: Reverse arrow parsing in CREATE

### ❌ [3] Create a pattern with multiple hops in varying directions
**Status:** FAILING
- Query: `CREATE (:A)-[:R]->(:B)<-[:R]-(:C)`
- Expected: 3 nodes, 2 relationships (A->B, C->B)
- Error: Parser error on `<-`
- Blocker: Mixed direction parsing

### ❌ [4] Create a pattern with multiple hops with multiple types and varying directions
**Status:** FAILING
- Query: `CREATE ()-[:R1]->()<-[:R2]-()-[:R3]->()`
- Expected: 4 nodes, 3 relationships
- Error: Parser error on `<-`
- Blocker: Reverse arrow in multi-hop pattern

### ❌ [5] Create a pattern with multiple hops and varying directions
**Status:** FAILING
- Query: `CREATE (:A)<-[:R1]-(:B)-[:R2]->(:C)`
- Expected: 3 nodes, 2 relationships
- Error: Parser error on `<-`
- Blocker: Reverse arrow at start of pattern

---

## Implementation Plan

### Phase 1: Reverse Arrow Support in CREATE
**Priority:** HIGH
**Files:** `src/cypher/cypher-parser.c`
**Dependencies:** Same fix needed for Create2

- [ ] Add `<-` parsing in CREATE relationship patterns
- [ ] Handle both `->` and `<-` in same pattern
- [ ] Ensure correct relationship direction storage
- [ ] Test all directional combinations

### Phase 2: Multi-Hop Validation
**Priority:** MEDIUM
**Files:** `src/cypher/cypher-write.c`

- [ ] Verify relationships created with correct directions
- [ ] Test long chains (5+ hops)
- [ ] Verify intermediate nodes created correctly
- [ ] Test with control queries (MATCH verification)

### Phase 3: Edge Cases
**Priority:** LOW

- [ ] Test very long chains
- [ ] Test cycles in patterns
- [ ] Test with properties on intermediate nodes
- [ ] Performance testing with many hops

---

## Next Steps

1. **Critical:** Implement reverse arrow support (shared with Create2)
2. **Test:** Verify all 5 scenarios pass after parser fix
3. **Validate:** Run control queries to verify graph structure
4. **Performance:** Test with longer chains

---

## Notes

- Single root cause for all failures
- Should be easy fix once reverse arrows work
- Tests are well-designed for verification
- Control queries provide good validation
- 80% of tests blocked by one parser issue
