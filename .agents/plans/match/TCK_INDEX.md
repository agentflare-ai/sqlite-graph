# TCK MATCH Test Coverage - Index

**Last Updated:** 2025-01-15
**Overall Status:** 🔴 NOT STARTED (0/146 passing, 0%)

---

## Summary

| Feature | Scenarios | Passing | Failing | Ignored | Pass Rate |
|---------|-----------|---------|---------|---------|-----------|
| Match1 - Match nodes | 6 | 0 | 0 | 6 | N/A |
| Match2 - Match relationships | 8 | 0 | 0 | 8 | N/A |
| Match3 - Match fixed length patterns | 30 | 0 | 0 | 30 | N/A |
| Match4 - Match variable length patterns | 10 | 0 | 0 | 10 | N/A |
| Match5 - Match variable length patterns over given graphs | 29 | 0 | 0 | 29 | N/A |
| Match6 - Match named paths | 20 | 0 | 0 | 20 | N/A |
| Match7 - Optional match | 31 | 0 | 0 | 31 | N/A |
| Match8 - Match clause interoperation | 3 | 0 | 0 | 3 | N/A |
| Match9 - Match deprecated scenarios | 9 | 0 | 0 | 9 | N/A |
| **TOTAL** | **146** | **0** | **0** | **146** | **0%** |

---

## Feature Details

### Match1 - Match Nodes (Not Started)
**File:** `01_TCK_Match1_Match_Nodes.md`
**Status:** 🔴 NOT STARTED

**Scenarios:** 6
- Basic node matching
- Multiple labels
- Property predicates
- Cartesian products
- Error validation

**Priority:** CRITICAL - Foundation for all MATCH operations

---

### Match2 - Match Relationships (Not Started)
**File:** `02_TCK_Match2_Match_Relationships.md`
**Status:** 🔴 NOT STARTED

**Scenarios:** 8
- Basic relationship matching
- Label predicates on endpoints
- Self-loops
- Relationship properties
- Multiple relationship types
- Error validation

**Priority:** CRITICAL - Foundation for graph traversal

---

### Match3 - Match Fixed Length Patterns (Not Started)
**File:** `03_TCK_Match3_Fixed_Length_Patterns.md`
**Status:** 🔴 NOT STARTED

**Scenarios:** 30
- Multi-hop patterns
- Complex patterns
- Bidirectional relationships
- Pattern variations

**Priority:** HIGH - Extended pattern matching

---

### Match4 - Match Variable Length Patterns (Not Started)
**File:** `04_TCK_Match4_Variable_Length_Patterns.md`
**Status:** 🔴 NOT STARTED

**Scenarios:** 10
- Variable length relationships
- Path finding
- Min/max hop constraints

**Priority:** MEDIUM - Advanced pattern matching

---

### Match5 - Match Variable Length Over Given Graphs (Not Started)
**File:** `05_TCK_Match5_Variable_Length_Given_Graphs.md`
**Status:** 🔴 NOT STARTED

**Scenarios:** 29
- Complex graph scenarios
- Variable length edge cases
- Real-world patterns

**Priority:** MEDIUM - Comprehensive variable length testing

---

### Match6 - Match Named Paths (Not Started)
**File:** `06_TCK_Match6_Named_Paths.md`
**Status:** 🔴 NOT STARTED

**Scenarios:** 20
- Path binding
- Path expressions
- Path reuse

**Priority:** LOW - Path features

---

### Match7 - Optional Match (Not Started)
**File:** `07_TCK_Match7_Optional_Match.md`
**Status:** 🔴 NOT STARTED

**Scenarios:** 31
- OPTIONAL MATCH clause
- Null handling
- Mixed MATCH and OPTIONAL MATCH

**Priority:** MEDIUM - Important for LEFT JOIN semantics

---

### Match8 - Match Clause Interoperation (Not Started)
**File:** `08_TCK_Match8_Clause_Interoperation.md`
**Status:** 🔴 NOT STARTED

**Scenarios:** 3
- Interaction with other clauses
- Complex query patterns

**Priority:** HIGH - Integration testing

---

### Match9 - Match Deprecated Scenarios (Not Started)
**File:** `09_TCK_Match9_Deprecated.md`
**Status:** 🔴 DEFERRED

**Scenarios:** 9
- Deprecated features
- Backward compatibility

**Priority:** LOW - Can be deferred

---

## Implementation Roadmap

### Phase 1: Core Match Functionality (Target: 40% overall)
**Priority:** CRITICAL
**Timeframe:** Immediate

1. ✅ Implement Match1 - Basic node matching
2. ✅ Implement Match2 - Basic relationship matching
3. ✅ Test with existing CREATE/MERGE tests

**Expected Result:** Match1 80%+, Match2 70%+

### Phase 2: Pattern Matching (Target: 60% overall)
**Priority:** HIGH
**Timeframe:** Short-term

1. ✅ Implement Match3 - Fixed length patterns
2. ✅ Implement Match8 - Clause interoperation
3. ✅ Test complex patterns

**Expected Result:** Match3 60%+, Match8 100%

### Phase 3: Advanced Features (Target: 70% overall)
**Priority:** MEDIUM
**Timeframe:** Medium-term

1. ✅ Implement Match7 - OPTIONAL MATCH
2. ✅ Implement Match4 - Variable length patterns
3. ✅ Test integration scenarios

**Expected Result:** Match7 50%+, Match4 50%+

### Phase 4: Comprehensive Testing (Target: 80% overall)
**Priority:** MEDIUM
**Timeframe:** Long-term

1. ✅ Complete Match5 - Variable length edge cases
2. ✅ Implement Match6 - Named paths
3. ✅ Full integration testing

**Expected Result:** Match5 60%+, Match6 60%+

### Phase 5: Deprecated Features (Optional)
**Priority:** LOW
**Timeframe:** Future

1. ✅ Evaluate Match9 scenarios
2. ✅ Implement if needed

**Expected Result:** Match9 optional

---

## Critical Dependencies

### Prerequisites
MATCH depends on having these working:
- ✅ CREATE - needed to set up test data
- ✅ RETURN - needed to verify results
- ⚠️ WHERE - may be needed for filtering
- ⚠️ WITH - may be needed for complex queries

### Implementation Order
```
Match1 (nodes) ← Foundation
  ↓
Match2 (relationships) ← Foundation
  ↓
Match3 (fixed length) ← Extended patterns
  ↓
Match8 (interop) ← Integration
  ↓
Match7 (optional) ← LEFT JOIN semantics
  ↓
Match4 (variable length) ← Advanced
  ↓
Match5 (variable length graphs) ← Comprehensive
  ↓
Match6 (named paths) ← Path features
  ↓
Match9 (deprecated) ← Optional
```

---

## Expected Implementation Gaps

Based on CREATE/MERGE experience, we anticipate:

### 1. Parser Limitations
- Variable length syntax `[*]`, `[*1..5]`
- OPTIONAL MATCH keyword
- Path binding syntax `p = ...`
- Multiple relationship types `[:TYPE1|TYPE2]`

### 2. Pattern Matching Engine
- Graph traversal algorithms
- Variable length path finding
- Cycle detection
- Performance optimization

### 3. Semantic Validation
- Variable type conflicts (node vs relationship vs path)
- Binding validation
- Pattern validation

### 4. Null Handling
- OPTIONAL MATCH null semantics
- Null propagation

---

## Quick Wins

Starting with Match1 and Match2:

1. **Basic Node Matching** → Foundation for all queries
2. **Basic Relationship Matching** → Foundation for traversal
3. **Simple Patterns** → Build on CREATE tests

**Estimated initial coverage: 30-40% of Match1+Match2**

---

## Files Requiring Changes

### High Priority
1. `src/cypher/cypher-parser.c` - MATCH clause parsing
2. `src/cypher/cypher-planner.c` - Query planning for MATCH
3. `src/cypher/cypher-executor.c` - MATCH execution
4. `src/cypher/cypher-iterators.c` - Graph traversal

### Medium Priority
5. `src/cypher/cypher-ast.c` - OPTIONAL MATCH, path binding
6. Semantic validator - Variable type checking
7. `src/storage/` - Graph traversal optimization

### Low Priority
8. Variable length pattern implementation
9. Named path support
10. Deprecated feature support

---

## Testing Strategy

### Approach
- Create `tests/tck_test_clauses_match.c`
- Follow pattern from `tck_test_clauses_create.c`
- Use `exec_cypher()` for queries
- Helper functions for assertions
- Well-documented test code

### Testing Order
1. Match1 - Basic node patterns
2. Match2 - Basic relationship patterns
3. Match3 - Fixed length patterns
4. Match8 - Interoperation
5. Match7 - OPTIONAL MATCH
6. Match4/5 - Variable length
7. Match6 - Named paths
8. Match9 - Deprecated (if needed)

---

## Success Metrics

### Initial Target (Phase 1)
- Match1: 60%+ passing
- Match2: 60%+ passing
- Overall: 30%+ passing

### Mid-term Target (Phase 2-3)
- Match1/2: 80%+ passing
- Match3: 60%+ passing
- Match7/8: 60%+ passing
- Overall: 60%+ passing

### Final Target (Phase 4)
- Match1/2/3: 90%+ passing
- Match4/5/6/7/8: 70%+ passing
- Overall: 80%+ passing

---

## Notes

- MATCH is the most fundamental read operation in Cypher
- Already implicitly used in CREATE/MERGE tests
- Strong foundation from existing code
- Parser and executor changes will be significant
- Pattern matching is the core challenge
- Performance considerations for variable length

---

## References

- Feature files: `tck/features/clauses/match/Match*.feature`
- Test implementation: `tests/tck_test_clauses_match.c` (to be created)
- Related: `tck_test_clauses_create.c`, `tck_test_clauses_merge.c`
