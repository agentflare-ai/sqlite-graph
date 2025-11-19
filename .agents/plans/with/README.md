# WITH Clause Implementation Plans

This directory contains detailed implementation plans for the Cypher WITH clause.

## Overview

The WITH clause is a critical Cypher feature that enables:
- Query pipelining (breaking queries into logical stages)
- Variable scoping and aliasing
- Data aggregation and grouping
- Intermediate result filtering

**Total Scenarios:** 27 across 7 feature groups
**Current Status:** 🔴 Not Started (0/27, 0%)
**Estimated Timeline:** 6-10 weeks

---

## Quick Start

**Read these in order:**

1. **Start here:** `00_WITH_CLAUSE_MASTER_PLAN.md`
   - Complete overview
   - Timeline and phases
   - Dependencies and risks

2. **Then review feature plans:**
   - `01_TCK_With1_Forward_Single_Variable.md` - Foundation
   - `02_TCK_With2_Forward_Single_Expression.md` - Expressions
   - `03_TCK_With3_Forward_Multiple_Expressions.md` - Multi-column
   - `04_TCK_With4_Variable_Aliasing.md` - Aliasing & errors
   - `05_TCK_With5_Implicit_Grouping_DISTINCT.md` - DISTINCT
   - `06_TCK_With6_Implicit_Grouping_Aggregates.md` - Aggregation
   - `07_TCK_With7_WITH_on_WITH.md` - Chained WITH

---

## Implementation Priority

### Phase 1: Foundation (3-4 weeks) 🔴 CRITICAL
- With1: Forward single variable (6 scenarios)
- With2: Forward single expression (2 scenarios)
- With3: Forward multiple expressions (1 scenario)
- **Total:** 9 scenarios

**Unblocks:** Basic WITH functionality, Match1 Test 11

### Phase 2: Aliasing (1-2 weeks) 🟡 HIGH
- With4: Variable aliasing (7 scenarios)

**Unblocks:** Variable shadowing, error validation

### Phase 3: Aggregation (3-4 weeks) 🔴 CRITICAL
- With5: DISTINCT (2 scenarios)
- With6: Aggregates (9 scenarios)
- **Total:** 11 scenarios

**Unblocks:** GROUP BY, aggregation functions, complex analytics

### Phase 4: Advanced (1 week) 🟢 MEDIUM
- With7: WITH on WITH (2 scenarios)

**Unblocks:** Complex query chains

---

## File Structure

```
with/
├── README.md                                          # This file
├── 00_WITH_CLAUSE_MASTER_PLAN.md                     # Master overview
├── 01_TCK_With1_Forward_Single_Variable.md           # 6 scenarios
├── 02_TCK_With2_Forward_Single_Expression.md         # 2 scenarios
├── 03_TCK_With3_Forward_Multiple_Expressions.md      # 1 scenario
├── 04_TCK_With4_Variable_Aliasing.md                 # 7 scenarios
├── 05_TCK_With5_Implicit_Grouping_DISTINCT.md        # 2 scenarios
├── 06_TCK_With6_Implicit_Grouping_Aggregates.md      # 9 scenarios
└── 07_TCK_With7_WITH_on_WITH.md                      # 2 scenarios
```

---

## Key Features

### With1 - Forward Single Variable
**Foundation feature - must implement first**

Basic WITH clause functionality:
```cypher
MATCH (a:A)
WITH a
MATCH (a)-->(b)
RETURN *
```

Includes: Variable forwarding, NULL handling, path variables

### With2 - Forward Single Expression
Property projection:
```cypher
MATCH (a:Begin)
WITH a.num AS property
MATCH (b) WHERE b.id = property
RETURN b
```

Includes: Expression evaluation, property access

### With3 - Forward Multiple Expressions
Multi-column projection:
```cypher
MATCH (a)-[r]->(b:X)
WITH a, r, b
MATCH (a)-[r]->(b)
RETURN r
```

### With4 - Variable Aliasing
Aliasing and error handling:
```cypher
MATCH (n)
WITH n.name AS n  -- Shadow variable
RETURN n
```

Includes: Variable shadowing, duplicate detection, alias requirements

### With5 - DISTINCT
Duplicate elimination:
```cypher
MATCH (a)
WITH DISTINCT a.name AS name
RETURN name
```

### With6 - Aggregates
**Most complex feature**

Implicit grouping and aggregation:
```cypher
MATCH (a)
WITH a.name AS name, count(*) AS cnt
RETURN name, cnt
```

Includes: GROUP BY logic, count(), avg(), expression validation

### With7 - WITH on WITH
Chained WITH clauses:
```cypher
MATCH (a)
WITH a.x AS x, count(*) AS c WHERE c > 1
WITH x WHERE x <> 'foo'
RETURN count(*)
```

---

## Dependencies

### Critical External Dependencies
- OPTIONAL MATCH (for NULL handling)
- WHERE clause (for filtering)
- ORDER BY (for sorting after WITH)
- LIMIT (for row limiting)
- Aggregation functions: count(), avg(), sum(), etc.
- Path support and variable-length relationships

### What's Already Available
✅ Parser infrastructure
✅ Planner infrastructure
✅ Iterator framework
✅ Expression evaluator
✅ Type system

### What Needs Implementation
🔴 WITH clause parser
🔴 Projection operator
🔴 Aggregation framework
🔴 Hash-based grouping
🔴 DISTINCT operator

---

## Testing

### Test Files
- `tests/tck_test_clauses_with.c` - All WITH scenarios
- TCK feature files in `tck/features/clauses/with/`

### Run Tests
```bash
# Build tests
make tests

# Run WITH tests
./build/tests/tck_test_clauses_with

# Run specific test
./build/tests/tck_test_clauses_with --test With1_01
```

---

## Success Metrics

### Minimum Viable Product (MVP)
- **Target:** With1-3 (9/27 scenarios, 33%)
- **Timeline:** 3-4 weeks
- **Unblocks:** Match1 Test 11, basic query pipelines

### Production Ready
- **Target:** With1-6 (25/27 scenarios, 93%)
- **Timeline:** 7-9 weeks
- **Unblocks:** All aggregation, most real-world queries

### Complete Implementation
- **Target:** With1-7 (27/27 scenarios, 100%)
- **Timeline:** 8-10 weeks
- **Unblocks:** All WITH features

---

## Current Blockers

**Match1 Test 11** is blocked waiting for WITH:
```cypher
WITH 123 AS n
MATCH (n)  -- Should fail: n is integer, not node
RETURN n
```

Once WITH is implemented, this test can pass.

---

## Related Documentation

### Related Plans
- `../match/01_TCK_Match1_Match_Nodes.md` - Test 11 blocked on WITH
- Other clause plans (when created)

### TCK Features
- `tck/features/clauses/with/With1.feature`
- `tck/features/clauses/with/With2.feature`
- `tck/features/clauses/with/With3.feature`
- `tck/features/clauses/with/With4.feature`
- `tck/features/clauses/with/With5.feature`
- `tck/features/clauses/with/With6.feature`
- `tck/features/clauses/with/With7.feature`

---

## Contributing

When implementing:

1. **Start with With1** - It's the foundation
2. **Follow the TODO lists** in each plan document
3. **Update status** as scenarios are completed
4. **Run tests frequently** - catch issues early
5. **Document blockers** if you hit dependencies
6. **Update this README** when milestones are reached

---

## Questions?

- Check the master plan: `00_WITH_CLAUSE_MASTER_PLAN.md`
- Review individual feature plans for details
- Look at TCK feature files for exact test cases
- Check related plans for dependencies

---

**Last Updated:** 2025-11-15
**Maintainer:** Development Team
**Status:** Ready for implementation
