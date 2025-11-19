# WITH Clause - Master Implementation Plan

**Feature:** WITH Clause (Complete)
**Status:** 🔴 NOT STARTED (0/27 scenarios)
**Priority:** 🔴 CRITICAL - Blocks multiple features
**Last Updated:** 2025-11-15

---

## Overview

The WITH clause is one of the most important features in Cypher. It enables:
- Breaking queries into logical parts
- Variable scoping and aliasing
- Data aggregation and grouping
- Filtering intermediate results
- Complex multi-stage query pipelines

**This blocks:** Match1 Test 11, many RETURN tests, complex query patterns

---

## Feature Breakdown

| Feature | Scenarios | Priority | Status | Estimated Effort |
|---------|-----------|----------|--------|------------------|
| With1 - Forward Single Variable | 6 | 🔴 CRITICAL | 🔴 Not Started | 1-2 weeks |
| With2 - Forward Single Expression | 2 | HIGH | 🔴 Not Started | 3-5 days |
| With3 - Forward Multiple Expressions | 1 | HIGH | 🔴 Not Started | 2-3 days |
| With4 - Variable Aliasing | 7 | HIGH | 🔴 Not Started | 1-2 weeks |
| With5 - DISTINCT | 2 | MEDIUM | 🔴 Not Started | 5-7 days |
| With6 - Aggregates | 9 | 🔴 CRITICAL | 🔴 Not Started | 2-3 weeks |
| With7 - WITH on WITH | 2 | MEDIUM | 🔴 Not Started | 4-5 days |
| **TOTAL** | **27** | - | **0/27 (0%)** | **6-10 weeks** |

---

## Quick Status

| Status | Count | Features |
|--------|-------|----------|
| 🔴 Not Started | 7 | With1-7 |
| 🟡 In Progress | 0 | - |
| ✅ Complete | 0 | - |

**Overall Progress:** 0/27 scenarios (0%)

---

## Implementation Phases

### Phase 1: Foundation (With1-3) - CRITICAL
**Goal:** Basic WITH clause working
**Timeline:** 3-4 weeks
**Priority:** 🔴 CRITICAL
**Scenarios:** 9 total

**Features:**
1. **With1** - Forward single variable (6 scenarios)
   - Basic WITH clause parsing
   - Variable forwarding
   - NULL handling
   - Path variables

2. **With2** - Forward single expression (2 scenarios)
   - Expression projection
   - Property access
   - Map literals

3. **With3** - Forward multiple expressions (1 scenario)
   - Multi-column projection
   - Row integrity

**Success Criteria:**
- [ ] 9/9 scenarios passing
- [ ] WITH clause parsed and executed
- [ ] Variables and expressions forwarded correctly
- [ ] Integration with MATCH and RETURN

**Deliverables:**
- [ ] Parser support for WITH clause
- [ ] AST node: CYPHER_AST_WITH
- [ ] Planner: LOGICAL_WITH operator
- [ ] Physical plan: PHYSICAL_PROJECT operator
- [ ] Projection iterator
- [ ] Test suite passing

**Estimated Effort:** 3-4 weeks

---

### Phase 2: Aliasing & Error Handling (With4) - HIGH
**Goal:** Variable aliasing and validation
**Timeline:** 1-2 weeks
**Priority:** HIGH
**Scenarios:** 7 total

**Features:**
1. **With4** - Variable aliasing (7 scenarios)
   - Alias syntax: `var AS newname`
   - Variable shadowing
   - Duplicate alias detection
   - Expression alias requirements
   - Complex aliasing chains

**Success Criteria:**
- [ ] 7/7 scenarios passing
- [ ] Aliasing works correctly
- [ ] Error validation in place

**Dependencies:** With1-3

**Estimated Effort:** 1-2 weeks

---

### Phase 3: DISTINCT & Aggregation (With5-6) - CRITICAL
**Goal:** Grouping and aggregation support
**Timeline:** 3-4 weeks
**Priority:** 🔴 CRITICAL (aggregation)
**Scenarios:** 11 total

**Features:**
1. **With5** - DISTINCT (2 scenarios)
   - DISTINCT keyword
   - Duplicate elimination
   - Complex type equality

2. **With6** - Aggregates (9 scenarios)
   - Implicit GROUP BY
   - count(), avg() functions
   - Grouping by multiple keys
   - Aggregation expression validation

**Success Criteria:**
- [ ] 11/11 scenarios passing
- [ ] DISTINCT working
- [ ] Aggregation and grouping correct

**Dependencies:** With1-4

**Estimated Effort:** 3-4 weeks

---

### Phase 4: Advanced Patterns (With7) - MEDIUM
**Goal:** Chained WITH clauses
**Timeline:** 1 week
**Priority:** MEDIUM
**Scenarios:** 2 total

**Features:**
1. **With7** - WITH on WITH (2 scenarios)
   - Multiple consecutive WITH
   - Scope chaining
   - Integration with WHERE, LIMIT

**Success Criteria:**
- [ ] 2/2 scenarios passing
- [ ] WITH chains work correctly

**Dependencies:** With1-6, WHERE, LIMIT

**Estimated Effort:** 1 week

---

## Total Timeline

**Minimum (best case):** 6 weeks
**Maximum (worst case):** 10 weeks
**Most Likely:** 8 weeks

**Critical Path:**
1. With1 (1-2 weeks)
2. With2-3 (1 week)
3. With4 (1-2 weeks)
4. With6 (2-3 weeks) - Most complex
5. With5 (1 week)
6. With7 (1 week)

---

## Dependencies

### External Dependencies
- [ ] OPTIONAL MATCH (With1 scenarios 5-6)
- [ ] Path support (With1 scenario 4, With6 scenario 4)
- [ ] Variable-length paths (With6 scenario 4)
- [ ] ORDER BY (With4 scenario 6, With7)
- [ ] LIMIT (With7 scenario 1)
- [ ] WHERE clause (With7)
- [ ] Aggregation functions: count(), avg() (With6)
- [ ] List functions: collect(), head() (With4 scenario 6)
- [ ] Path functions: nodes() (With6 scenario 4)
- [ ] Map literals (With2 scenario 2, With4 scenario 7)
- [ ] Parameters (With6 scenario 5)

### Internal Dependencies
- [x] Parser infrastructure
- [x] Planner infrastructure
- [x] Iterator framework
- [x] Expression evaluator
- [x] Type system
- [ ] Aggregation framework (needs implementation)
- [ ] Hash-based grouping (needs implementation)

---

## Success Criteria

### Minimum Viable Product (MVP)
**Goal:** Basic WITH working for simple queries
**Target:** With1-3 complete (9 scenarios, 33%)
**Timeline:** 3-4 weeks

Enables:
- Basic variable forwarding
- Simple expression projection
- Breaking queries into parts

### Production Ready
**Goal:** Full WITH support except advanced patterns
**Target:** With1-6 complete (25 scenarios, 93%)
**Timeline:** 7-9 weeks

Enables:
- All variable forwarding patterns
- Aliasing and shadowing
- DISTINCT
- Aggregation and grouping
- Most real-world use cases

### Complete Implementation
**Goal:** All WITH features
**Target:** With1-7 complete (27 scenarios, 100%)
**Timeline:** 8-10 weeks

Enables:
- Everything in Production Ready
- Complex WITH chains
- Advanced query patterns

---

## Impact Analysis

### Features Unblocked by WITH

**Immediate:**
- Match1 Test 11 (variable type validation)
- Complex RETURN queries with aggregation
- Multi-stage data transformation queries

**Medium-term:**
- Subqueries (depend on WITH for scoping)
- CALL {} (subquery syntax)
- Complex analytics queries

**Long-term:**
- Query optimization opportunities
- Pipeline-based execution model

### Test Coverage Improvement

**Current State:**
- Match1: 10/11 (91%) - blocked on Test 11

**After WITH MVP (With1-3):**
- Match1: 11/11 (100%) ✅
- Basic query patterns working

**After Production Ready (With1-6):**
- Full aggregation support
- Complex query patterns
- Real-world use cases covered

---

## Risk Assessment

### High Risk Items

1. **Aggregation Complexity** (With6)
   - Most complex feature
   - Implicit GROUP BY semantics
   - Performance critical
   - **Mitigation:** Allocate 2-3 weeks, extensive testing

2. **Scope Management** (With4, With7)
   - Variable shadowing tricky
   - Multi-WITH scope chains
   - **Mitigation:** Careful design, thorough testing

3. **Performance** (With5, With6)
   - Hash-based operations
   - Large result sets
   - **Mitigation:** Efficient algorithms, profiling

### Medium Risk Items

1. **Integration Complexity**
   - Many dependencies
   - Interactions with MATCH, RETURN, WHERE
   - **Mitigation:** Incremental integration, integration tests

2. **Error Handling**
   - Complex validation rules (With4, With6)
   - Clear error messages needed
   - **Mitigation:** Define error codes early, test error paths

---

## Resource Requirements

### Development Time
- **Core Implementation:** 6-8 weeks (1 developer)
- **Testing & Integration:** 1-2 weeks
- **Documentation:** 1 week
- **Total:** 8-11 weeks

### Skills Required
- Cypher query language expertise
- Parser/compiler development
- Query execution and optimization
- Testing and debugging

---

## Testing Strategy

### Unit Tests
- [ ] Parser tests for each WITH variant
- [ ] Planner tests for logical operators
- [ ] Iterator tests for execution
- [ ] Aggregation tests

### Integration Tests
- [ ] WITH + MATCH integration
- [ ] WITH + RETURN integration
- [ ] WITH + WHERE integration
- [ ] Multi-clause queries

### TCK Tests
- [ ] All 27 WITH scenarios
- [ ] Cross-feature tests
- [ ] Error condition tests

### Performance Tests
- [ ] Large result set projection
- [ ] Deep WITH chains
- [ ] Complex aggregations
- [ ] Grouping performance

---

## Monitoring & Metrics

### Development Metrics
- Scenarios implemented per week
- Test pass rate
- Code coverage

### Quality Metrics
- TCK compliance rate
- Memory leak detection
- Performance benchmarks

---

## Next Steps

1. **Immediate (Week 1-2):**
   - Review and approve this plan
   - Set up development environment
   - Begin With1 implementation
   - Design AST structure for WITH

2. **Short-term (Week 3-4):**
   - Complete With1 foundation
   - Begin With2-3
   - Start test infrastructure

3. **Medium-term (Week 5-8):**
   - Complete With1-4
   - Begin aggregation work (With6)
   - Integration testing

4. **Long-term (Week 9-11):**
   - Complete all features
   - Full TCK compliance
   - Performance optimization
   - Documentation

---

## References

**Feature Files:**
- `tck/features/clauses/with/With1.feature`
- `tck/features/clauses/with/With2.feature`
- `tck/features/clauses/with/With3.feature`
- `tck/features/clauses/with/With4.feature`
- `tck/features/clauses/with/With5.feature`
- `tck/features/clauses/with/With6.feature`
- `tck/features/clauses/with/With7.feature`

**Plan Documents:**
- `01_TCK_With1_Forward_Single_Variable.md`
- `02_TCK_With2_Forward_Single_Expression.md`
- `03_TCK_With3_Forward_Multiple_Expressions.md`
- `04_TCK_With4_Variable_Aliasing.md`
- `05_TCK_With5_Implicit_Grouping_DISTINCT.md`
- `06_TCK_With6_Implicit_Grouping_Aggregates.md`
- `07_TCK_With7_WITH_on_WITH.md`

**Related Plans:**
- `.agents/plans/match/01_TCK_Match1_Match_Nodes.md` (Test 11 blocked)

---

## Contact & Support

**Questions?** Review individual feature plans for detailed TODOs and implementation guidance.

**Issues?** Document blockers and dependencies clearly.

**Updates?** Keep this master plan in sync with feature plan status.

---

**Last Updated:** 2025-11-15
**Status:** Ready for implementation
**Approval:** Pending review
