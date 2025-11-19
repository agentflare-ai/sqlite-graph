# TCK Match7 - Optional Match

**Feature:** Match7 - Optional match
**Total Scenarios:** 31
**Status:** 🔴 NOT STARTED (0/31 passing, 0%)

---

## Overview

OPTIONAL MATCH is the graph equivalent of SQL's LEFT OUTER JOIN.

```cypher
MATCH (a:Person)
OPTIONAL MATCH (a)-[:KNOWS]->(b)
RETURN a, b
```

If the OPTIONAL MATCH fails, `b` will be NULL, but the row still returns.

---

## Key Concepts

### Regular MATCH
- If pattern doesn't match, entire row is filtered out
- All-or-nothing

### OPTIONAL MATCH
- If pattern doesn't match, variables are NULL
- Row still appears in results
- Allows "maybe" patterns

---

## Scenario Categories

### Basic OPTIONAL MATCH (~10 scenarios)
- Simple optional patterns
- With regular MATCH
- Multiple OPTIONAL MATCH clauses

### Null Handling (~10 scenarios)
- NULL propagation
- NULL in WHERE clauses
- NULL in RETURN

### Complex Patterns (~10 scenarios)
- Multi-hop optional patterns
- Variable length optional
- Nested optional patterns
- OPTIONAL MATCH with CREATE/MERGE

---

## Implementation Requirements

1. **OPTIONAL MATCH keyword** ❌ Parser support
2. **Null injection** ❌ When pattern fails, inject NULLs
3. **Null propagation** ⚠️ NULL handling in expressions
4. **Query planning** ❌ LEFT JOIN semantics in planner
5. **Iterator changes** ⚠️ Support optional matching

---

## Key Differences from MATCH

| Feature | MATCH | OPTIONAL MATCH |
|---------|-------|----------------|
| No match found | Row filtered | Row kept, NULLs injected |
| Variable binding | Always bound | May be NULL |
| Execution | Required | Optional |
| SQL equivalent | INNER JOIN | LEFT OUTER JOIN |

---

## Dependencies

### Prerequisites
- ✅ Match1-3 - Pattern matching
- ✅ NULL handling in expressions
- ⚠️ WHERE clause (for NULL filtering)

### Impact
- Changes query execution model
- Affects iterator design
- Requires LEFT JOIN planning

---

## Implementation Plan

### Phase 1: Parser & AST
**Tasks:**
1. [ ] Add OPTIONAL MATCH keyword
2. [ ] AST node for optional match
3. [ ] Semantic validation

### Phase 2: Basic OPTIONAL MATCH
**Tasks:**
1. [ ] Implement optional matching logic
2. [ ] NULL injection when pattern fails
3. [ ] Test simple scenarios

### Phase 3: Null Handling
**Tasks:**
1. [ ] NULL propagation in expressions
2. [ ] NULL in WHERE clauses
3. [ ] NULL in aggregations

### Phase 4: Complex Patterns
**Tasks:**
1. [ ] Multi-hop optional patterns
2. [ ] Multiple OPTIONAL MATCH clauses
3. [ ] Integration with MATCH

---

## Expected Pass Rate

### Phase 1-2
- Basic optional match: ~30%

### Phase 3
- With NULL handling: ~60%

### Phase 4
- Complex patterns: ~80%+

---

## Examples

### Example 1: Basic
```cypher
MATCH (a:Person {name: 'Alice'})
OPTIONAL MATCH (a)-[:KNOWS]->(b)
RETURN a.name, b.name
```

If Alice knows someone: `'Alice', 'Bob'`
If Alice knows nobody: `'Alice', NULL`

### Example 2: Multiple Optional
```cypher
MATCH (a:Person)
OPTIONAL MATCH (a)-[:KNOWS]->(b)
OPTIONAL MATCH (a)-[:LIVES_IN]->(c)
RETURN a.name, b.name, c.name
```

Each OPTIONAL MATCH is independent - both, one, or neither may match.

### Example 3: With WHERE
```cypher
MATCH (a:Person)
OPTIONAL MATCH (a)-[:KNOWS]->(b)
WHERE b.age > 30
RETURN a, b
```

If `b` doesn't exist or doesn't meet WHERE condition, `b` is NULL.

---

## Notes

- Very common in production queries
- NULL handling is critical
- Performance can be worse than MATCH (can't filter early)
- Similar to LEFT JOIN in SQL

---

## Success Criteria

### Minimum Viable
- [ ] Basic OPTIONAL MATCH working
- [ ] NULL injection correct
- [ ] 40%+ pass rate

### Production Ready
- [ ] NULL handling complete
- [ ] Complex patterns working
- [ ] 70%+ pass rate

---

**Priority:** MEDIUM-HIGH - Common in real queries
**Estimated Effort:** MEDIUM-HIGH - Execution model changes
**Dependencies:** Strong Match foundation
