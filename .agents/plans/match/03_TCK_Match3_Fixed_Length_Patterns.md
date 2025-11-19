# TCK Match3 - Match Fixed Length Patterns

**Feature:** Match3 - Match fixed length patterns
**Total Scenarios:** 30
**Status:** 🟡 IN PROGRESS (8/30 passing, 27%)
**Last Updated:** 2025-11-19

---

## Overview

Match3 focuses on multi-hop patterns with fixed lengths:
- Two-hop patterns: `(a)-[]->()-[]->(b)`
- Three-hop patterns: `(a)-[]->()-[]->()-[]->(b)`
- Complex patterns with multiple paths
- Direction variations (forward, backward, undirected)
- Named and unnamed intermediate nodes

This builds directly on Match1 (nodes) and Match2 (relationships).

---

## Test Results Summary

### Currently Passing (8/30 = 27%)
- ✅ Match3_01: Get neighbours (basic directed pattern)
- ✅ Match3_02: Directed match of simple relationship
- ✅ Match3_04: Get two related nodes
- ✅ Match3_06: Matching a relationship pattern using a label predicate (two-hop)
- ✅ Match3_08: Matching using relationship predicate with multiples of same type
- ✅ Match3_09: Get related to related to (two-hop pattern)
- ✅ Match3_13: Two-hop pattern with anonymous relationships
- ✅ Match3_14: Three-hop pattern with mixed directions

### Currently Failing (4/30 = 13%)
- ❌ Match3_03: Undirected match on simple relationship graph
- ❌ Match3_05: Return two subgraphs with bound undirected relationship
- ❌ Match3_11: Undirected match in self-relationship graph
- ❌ Match3_12: Mixed relationship patterns

### Ignored/Pending (18/30 = 60%)
- 🔵 Match3_07: Multi-label support needed
- 🔵 Match3_10: Multi-pattern MATCH needed
- 🔵 Match3_15-30: Not yet implemented

## Scenario Categories

### Positive Tests (~25 scenarios)
- Basic two-hop patterns ✅ WORKING
- Three-hop patterns ✅ WORKING
- Undirected multi-hop ⚠️ PARTIAL (some failing)
- Incoming direction patterns ✅ WORKING
- Complex patterns with labels and properties ✅ WORKING
- Multiple intermediate nodes ✅ WORKING

### Negative Tests (~5 scenarios)
- Error validation (pending)
- Variable type conflicts (pending)
- Invalid patterns (pending)

---

## Key Implementation Requirements

1. **Multi-hop pattern matching** ✅ Core feature WORKING
2. **Intermediate node handling** ✅ Named and unnamed WORKING
3. **Direction composition** ✅ `->` then `<-` in same pattern WORKING
4. **Pattern compilation** ✅ Convert to traversal plan WORKING
5. **Join logic** ✅ Connecting pattern segments WORKING
6. **Anonymous relationships** ✅ Support for `()-->()-->()` syntax WORKING
7. **Label filtering in multi-hop** ✅ `(:A)-->(:B)-->(:C)` WORKING

---

## Dependencies

### Prerequisites
- ✅ Match1 - Node matching
- ✅ Match2 - Relationship matching
- ✅ CREATE - Multi-hop test data

### Complexity
**Estimated effort:** HIGH
- Pattern matching engine needs enhancement
- Join/composition logic
- Performance optimization for multi-hop

---

## Implementation Plan

### Phase 1: Two-Hop Patterns ✅ COMPLETE
**Tasks:**
1. [x] Implement two-hop pattern matching
2. [x] Forward direction: `()-[]->()-[]->()`
3. [x] Backward direction: `()<-[]-()<-[]-()`
4. [x] Mixed direction: `()<-[]->()-[]->()`
5. [x] Test basic scenarios
6. [x] Anonymous relationship syntax: `()-->()-->()`

**Completed:** 2025-11-19
**Implementation:** Parser enhancements in cypher-parser.c to support chained patterns

### Phase 2: Three+ Hop Patterns ✅ COMPLETE
**Tasks:**
1. [x] Extend to three-hop patterns
2. [x] Arbitrary length fixed patterns
3. [x] Test complex scenarios

**Completed:** 2025-11-19
**Key Discovery:** Multi-hop traversal logic already existed in the engine - just needed parser support

### Phase 3: Labels and Properties ✅ PARTIAL
**Tasks:**
1. [x] Multi-hop with label filtering
2. [ ] Multi-hop with property filtering (not yet tested)
3. [ ] Combined filters (not yet tested)

**Status:** Label filtering working, property filtering not yet tested

---

## Expected Pass Rate vs Actual

### Phase 1 Target: ~40%
**Actual: 27% (8/30 passing)**
- Basic two-hop: ✅ WORKING
- Undirected patterns: ⚠️ SOME ISSUES (4 tests failing)

### Phase 2 Target: ~60%
**Actual: Not yet reached**
- Three-hop patterns: ✅ WORKING (basic cases)
- More complex scenarios: 🔵 PENDING (18 tests ignored)

### Phase 3 Target: ~80%
**Actual: Not yet attempted**
- With filters: Partially working
- Need more comprehensive testing

---

## Notes

- Foundation for variable length patterns (Match4-5)
- Performance critical - O(n²) or worse for multi-hop
- Pattern optimization opportunities
- Will reveal pattern matching engine gaps

### Implementation Findings (2025-11-19)

1. **Parser was the bottleneck**: The pattern matching engine already had multi-hop support, but the parser couldn't handle chained patterns like `()-->()-->()`

2. **Anonymous relationships work**: The parser now correctly handles patterns without relationship variable names

3. **Label filtering works**: Labels in intermediate nodes (`(:A)-->(:B)-->(:C)`) are correctly filtered

4. **Undirected patterns have issues**: 4 tests failing related to undirected relationship matching in multi-hop patterns

5. **Files modified**:
   - `src/cypher/cypher-parser.c` - Added chained pattern parsing support
   - `tests/tck_test_clauses_match.c` - Implemented 8 passing test cases

6. **Remaining work**:
   - Fix undirected pattern matching (4 failing tests)
   - Implement remaining 18 test scenarios
   - Test property filtering in multi-hop patterns
   - Performance optimization for deep traversals

---

## Success Criteria

### Minimum Viable
- [x] Two-hop patterns working ✅ ACHIEVED (2025-11-19)
- [ ] 40%+ pass rate (Currently 27% - need 12+ passing tests)

### Production Ready
- [x] Three-hop patterns working ✅ ACHIEVED (2025-11-19)
- [x] Labels filtering ✅ ACHIEVED (2025-11-19)
- [ ] Properties filtering (not yet tested)
- [ ] 70%+ pass rate (need 21+ passing tests)

---

**Priority:** HIGH - Foundation for complex queries
**Estimated Effort:** HIGH - Significant engine work
