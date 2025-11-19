# TCK Match6 - Match Named Paths

**Feature:** Match6 - Match named paths
**Total Scenarios:** 20
**Status:** 🔴 NOT STARTED (0/20 passing, 0%)

---

## Overview

Match6 focuses on **path binding** - assigning patterns to path variables:

```cypher
MATCH p = (a)-[*]->(b)
RETURN p
```

The variable `p` holds the entire matched path (nodes and relationships).

---

## Key Features

### Path Binding Syntax
- `p = (a)-[]->(b)` - Single hop path
- `p = (a)-[*]->(b)` - Variable length path
- `p = (a)-[]->()-[]->(b)` - Multi-hop path

### Path Functions
- `length(p)` - Number of relationships in path
- `nodes(p)` - List of nodes in path
- `relationships(p)` - List of relationships in path

### Path Operations
- Returning paths
- Filtering on path properties
- Path comparisons
- Multiple path variables in same query

---

## Implementation Requirements

1. **Path data type** ❌ New type in type system
2. **Path binding syntax** ❌ Parser support for `p = pattern`
3. **Path storage** ❌ How to represent paths in memory
4. **Path functions** ❌ length(), nodes(), relationships()
5. **Path serialization** ❌ Returning paths to user

---

## Dependencies

### Prerequisites
- ✅ Match1-3 - Pattern matching
- ⚠️ Match4 - Variable length (many tests use it)

### New Components Needed
- Path type definition
- Path construction during traversal
- Path manipulation functions

---

## Implementation Plan

### Phase 1: Path Type
**Tasks:**
1. [ ] Define path data structure
2. [ ] Path creation/destruction
3. [ ] Path storage in result sets

### Phase 2: Path Binding
**Tasks:**
1. [ ] Parse `p = pattern` syntax
2. [ ] Bind matched pattern to path variable
3. [ ] Return paths in results

### Phase 3: Path Functions
**Tasks:**
1. [ ] Implement length(p)
2. [ ] Implement nodes(p)
3. [ ] Implement relationships(p)

### Phase 4: Advanced Path Features
**Tasks:**
1. [ ] Multiple paths in same query
2. [ ] Path filtering
3. [ ] Path comparisons

---

## Expected Pass Rate

### Phase 1-2
- Basic path binding: ~40%

### Phase 3
- With functions: ~70%

### Phase 4
- Complete: ~90%+

---

## Notes

- Paths are first-class values in Cypher
- Essential for many graph algorithms
- Memory intensive (paths can be large)
- Used in shortest path queries

---

## Success Criteria

### Minimum Viable
- [ ] Path binding working
- [ ] Basic path return
- [ ] 40%+ pass rate

### Production Ready
- [ ] Path functions working
- [ ] Multiple paths supported
- [ ] 80%+ pass rate

---

**Priority:** LOW-MEDIUM - Important but not critical
**Estimated Effort:** MEDIUM - New type system work
**Dependencies:** Strong Match1-4 implementation
