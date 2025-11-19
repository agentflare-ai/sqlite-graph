# TCK Match4 - Match Variable Length Patterns

**Feature:** Match4 - Match variable length patterns
**Total Scenarios:** 10
**Status:** 🔴 NOT STARTED (0/10 passing, 0%)
**Last Updated:** 2025-01-17

---

## Overview

Match4 introduces variable length relationship patterns:
- `[*]` - One or more hops (min=1, max=unlimited)
- `[*2]` - Exactly 2 hops (min=2, max=2)
- `[*1..3]` - Between 1 and 3 hops (inclusive)
- `[*0..5]` - Between 0 and 5 hops (zero-length allowed)
- `[*..4]` - Up to 4 hops (min=0, max=4)
- `[*2..]` - At least 2 hops (min=2, max=unlimited)

Variable length patterns require path-finding algorithms (BFS recommended).

**IMPORTANT NOTE:** Default for `[*]` is min=1 (NOT zero). Use `[*0..]` for zero-length paths.

---

## Implementation Status

**Phase:** Specification Complete
**Code Status:** Not Started
**Test Status:** 10 stub tests created, all marked IGNORE

**Detailed Specification:** See `VARIABLE_LENGTH_IMPLEMENTATION_SPEC.md` for complete technical details.

**Next Steps:**
1. Implement parser support for `[*]` syntax (Phase 1)
2. Extend AST to store min/max hop counts (Phase 2)
3. Implement BFS path-finding iterator (Phase 3-4)
4. Enable and verify TCK tests (Phase 5)

---

## Key Scenarios

### Basic Variable Length
- `()-[*]->()` - Any path length
- `()-[*2]->()` - Exactly 2 hops
- `()-[*1..3]->()` - 1 to 3 hops

### Direction
- Forward: `()-[*]->()`
- Backward: `()<-[*]-()`
- Undirected: `()-[*]-()`

### Type Filtering
- `()-[:KNOWS*]->()` - Only KNOWS relationships
- `()-[:KNOWS|LIKES*]->()` - Multiple types

### With Endpoints
- `(:Person)-[*]->(:City)` - Labeled endpoints
- Named nodes for constraints

---

## Key Implementation Requirements

1. **Variable length syntax parsing** ❌ `[*min..max]`
2. **Path finding algorithm** ❌ BFS, DFS, or similar
3. **Cycle detection** ⚠️ Avoid infinite loops
4. **Path length constraints** ❌ Min/max enforcement
5. **Result deduplication** ⚠️ Same path found multiple ways
6. **Performance** ⚠️ Can be exponential

---

## Dependencies

### Prerequisites
- ✅ Match3 - Fixed length patterns
- ✅ Match1/Match2 - Basic matching

### Algorithm Needs
- BFS/DFS traversal
- Path collection
- Cycle detection
- Memory management (paths can be large)

---

## Implementation Plan

**DETAILED SPECIFICATION:** See `VARIABLE_LENGTH_IMPLEMENTATION_SPEC.md` for complete architecture.

### Phase 1: Parser Support (Week 1-2)
**File:** `src/cypher/cypher-parser.c` (lines 860-865)
**Tasks:**
1. [ ] Detect `CYPHER_TOK_MULT` after relationship pattern
2. [ ] Parse `[*]` syntax (min=1, max=unlimited)
3. [ ] Parse `[*n]` for exact length (min=n, max=n)
4. [ ] Parse `[*min..max]` for ranges
5. [ ] Parse `[*..max]` (min=0) and `[*min..]` (max=unlimited)
6. [ ] Validate: reject negative numbers, reject in MERGE
7. [ ] Store min/max in AST node

**Deliverable:** Parser accepts all variable-length syntax variants

### Phase 2: AST Extensions (Week 2)
**Files:** `include/cypher.h`, `src/cypher/cypher-ast.c`
**Tasks:**
1. [ ] Add `iMinHops` field to `CypherAst` struct
2. [ ] Add `iMaxHops` field to `CypherAst` struct
3. [ ] Add `REL_PATTERN_VARLEN` flag constant
4. [ ] Initialize fields in `cypherAstCreate()`
5. [ ] Update AST printing for debugging

**Deliverable:** AST can represent variable-length patterns

### Phase 3: Planner Support (Week 2-3)
**Files:** `src/cypher/cypher-planner.c`, `include/cypher-planner.h`
**Tasks:**
1. [ ] Add `LOGICAL_VAR_EXPAND` operator type
2. [ ] Detect variable-length in `CYPHER_AST_REL_PATTERN` case
3. [ ] Create var-expand logical plan node
4. [ ] Pass min/max hops to logical operator
5. [ ] Compile to `PHYSICAL_VAR_EXPAND` in physical plan

**Deliverable:** Logical and physical plans created for variable-length

### Phase 4: BFS Iterator (Week 3-5)
**File:** `src/cypher/cypher-iterators.c`
**Tasks:**
1. [ ] Define `VarLengthExpandIterator` structure
2. [ ] Implement BFS queue management
3. [ ] Implement cycle detection (per-path, not global)
4. [ ] Implement path expansion logic
5. [ ] Implement min/max hop filtering
6. [ ] Build relationship list results
7. [ ] Add memory/path limits (safety)

**Deliverable:** Basic variable-length queries execute correctly

### Phase 5: Testing & Validation (Week 5-6)
**File:** `tests/tck_test_clauses_match.c`
**Tasks:**
1. [ ] Implement Match4_01: Fixed-length variable pattern
2. [ ] Implement Match4_02: Simple variable length
3. [ ] Implement Match4_03: Zero-length patterns
4. [ ] Implement Match4_04: Longer paths (20+ hops)
5. [ ] Implement Match4_05: Property predicates
6. [ ] Implement Match4_06: Bound nodes
7. [ ] Implement Match4_07: Bound relationships
8. [ ] Implement Match4_08: Relationship lists
9. [ ] Implement Match4_09: Error - missing asterisk
10. [ ] Implement Match4_10: Error - negative bounds

**Deliverable:** All 10 Match4 TCK tests passing

### Phase 6: Optimization (Week 6+)
**Tasks:**
1. [ ] Profile performance on dense graphs
2. [ ] Optimize path queue management
3. [ ] Add early termination for LIMIT
4. [ ] Consider bidirectional search for bound endpoints
5. [ ] Tune memory limits

**Deliverable:** Performance acceptable for production use

---

## Algorithms

### Option 1: BFS (Breadth-First Search)
- Finds shortest paths first
- Good for min depth constraints
- Memory intensive

### Option 2: DFS (Depth-First Search)
- Lower memory
- May find longer paths first
- Needs depth limit

### Option 3: Bidirectional Search
- Start from both ends
- Faster for specific endpoints
- More complex

---

## Performance Considerations

Variable length matching can be **exponential**:
- Dense graphs → many paths
- Large max depth → combinatorial explosion
- Need safeguards (max path limit)

---

## Expected Pass Rate

### Phase 1-2
- Basic `[*]` working: ~30%

### Phase 3
- With constraints: ~70%

### Phase 4
- Optimized: ~100%

---

## Notes

- Most complex MATCH feature
- Performance critical
- May need query planner hints
- Consider path length limits

---

## Success Criteria

### Minimum Viable
- [ ] `[*]` unlimited length working
- [ ] Basic path finding
- [ ] 30%+ pass rate

### Production Ready
- [ ] All length constraints working
- [ ] Performance acceptable
- [ ] 80%+ pass rate

---

**Priority:** MEDIUM - Advanced feature
**Estimated Effort:** VERY HIGH - Complex algorithms
