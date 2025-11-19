# TCK Delete5 - Delete clause interoperation with built-in data types

**Feature:** Delete5 - Delete clause interoperation with built-in data types
**Total Scenarios:** 9
**Status:** 🔄 IN PROGRESS (2/9 passing, 22%)

***

## Implementation Summary

**Date:** 2025-01-15
**Result:** 2/9 passing (22%), 7 failing

### ✅ Passing Tests (2)

* Test 3: Delete nodes from a map ✅
* Test 4: Delete relationships from a map ✅

### ❌ Main Failures (7)

**Category 1: Collection Operations (4 tests)** - Tests 1-2, 5-6

* Issue: List indexing and collection operations not supported
* Root cause: WITH clause and collect() function missing

**Category 2: Advanced Features (2 tests)** - Tests 7-8

* Issue: Path operations and undefined variable detection missing
* Root cause: Path support and semantic validation incomplete

**Category 3: Expression Validation (1 test)** - Test 9

* Issue: Type checking for DELETE expressions not implemented
* Root cause: Syntax validation lacks expression type checking

### Key Findings

1. **Basic map property deletion works** ✅
2. **Collection operations missing** ❌
3. **Path deletion not implemented** ❌
4. **Semantic validation weak** ❌

***

## Scenarios

### ❌ \[1] Delete node from a list

**Status:** BLOCKED (list operations and WITH clause not implemented)

* Query: `MATCH (:User)-[:FRIEND]->(n) WITH collect(n) AS friends DETACH DELETE friends[$friendIndex]`
* Expected: Delete node at index 1 from collected list
* Issue: `WITH collect(n)` and list indexing not supported
* Required: Collection functions and WITH clause
* Expected side effects: `-nodes: 1, -relationships: 1`

### ❌ \[2] Delete relationship from a list

**Status:** BLOCKED (list operations not implemented)

* Query: `MATCH (:User)-[r:FRIEND]->() WITH collect(r) AS friendships DETACH DELETE friendships[$friendIndex]`
* Expected: Delete relationship at index 1 from collected list
* Issue: `collect(r)` and list indexing not supported
* Required: Collection functions and list operations
* Expected side effects: `-relationships: 1`

### ✅ \[3] Delete nodes from a map

**Status:** PASSING

* Query: `MATCH (u:User) WITH {key: u} AS nodes DELETE nodes.key`
* Expected: Delete node accessed via map property
* Implementation: ✅ Map property access in DELETE working
* Result: Empty result with `-nodes: 2, -labels: 1`

### ✅ \[4] Delete relationships from a map

**Status:** PASSING

* Query: `MATCH (:User)-[r]->(:User) WITH {key: r} AS rels DELETE rels.key`
* Expected: Delete relationship accessed via map property
* Implementation: ✅ Map property access in DELETE working
* Result: Empty result with `-relationships: 2`

### ❌ \[5] Detach delete nodes from nested map/list

**Status:** BLOCKED (nested collection operations not implemented)

* Query: `MATCH (u:User) WITH {key: collect(u)} AS nodeMap DETACH DELETE nodeMap.key[0]`
* Expected: Delete first node from collected array in map
* Issue: Nested collection operations `collect(u)` and indexing not supported
* Required: Collection functions and nested access
* Expected side effects: `-nodes: 1, -relationships: 2`

### ❌ \[6] Delete relationships from nested map/list

**Status:** BLOCKED (nested collection operations not implemented)

* Query: `MATCH (:User)-[r]->(:User) WITH {key: {key: collect(r)}} AS rels DELETE rels.key.key[0]`
* Expected: Delete first relationship from nested collections
* Issue: Complex nested access `rels.key.key[0]` not supported
* Required: Deep nested property access
* Expected side effects: `-relationships: 1`

### ❌ \[7] Delete paths from nested map/list

**Status:** BLOCKED (path operations not implemented)

* Query: `MATCH p = (:User)-[r]->(:User) WITH {key: collect(p)} AS pathColls DELETE pathColls.key[0], pathColls.key[1]`
* Expected: Delete path elements from collection
* Issue: Path variables `p` and path deletion not supported
* Required: Path data type and path operations
* Expected side effects: `-nodes: 2, -relationships: 2, -labels: 1`

### ❌ \[8] Failing when using undefined variable in DELETE

**Status:** BLOCKED (semantic validation not implemented)

* Query: `MATCH (a) DELETE x`
* Expected: SyntaxError: UndefinedVariable at compile time
* Issue: No validation that `x` is not defined
* Required: Variable scope checking in DELETE expressions
* Expected: Compile-time error detection

### ❌ \[9] Failing when deleting an integer expression

**Status:** BLOCKED (expression type validation not implemented)

* Query: `MATCH () DELETE 1 + 1`
* Expected: SyntaxError: InvalidArgumentType at compile time
* Issue: No validation that expressions must be nodes/relationships
* Required: Type checking for DELETE arguments
* Expected: Compile-time type validation

***

## Implementation Plan

### Phase 1: WITH Clause Implementation (Required for Tests 1-2, 5-7)

**Priority:** HIGH
**Files:** `src/cypher/cypher-parser.c`, `src/cypher/cypher-executor.c`

* [ ] Add WITH keyword to lexer (`CYPHER_TOK_WITH`)
* [ ] Implement WITH clause parsing and AST construction
* [ ] Add variable projection and aliasing
* [ ] Support WITH in multi-clause queries
* [ ] Test variable scoping across clauses

### Phase 2: Collection Functions (Required for Tests 1-2, 5-7)

**Priority:** HIGH
**Files:** `src/cypher/cypher-functions.c` (new file)

* [ ] Implement `collect()` aggregation function
* [ ] Add list data type support (`CYPHER_VALUE_LIST`)
* [ ] Support list construction and manipulation
* [ ] Implement list indexing `list[index]` syntax
* [ ] Add list serialization/deserialization

### Phase 3: Map Operations Enhancement (Already Working)

**Priority:** LOW
**Files:** `src/cypher/cypher-value.c`

* [ ] Verify map property access `map.key` works correctly
* [ ] Test nested map operations `{key: {inner: value}}`
* [ ] Performance optimization for map operations
* [ ] Add map construction syntax support

### Phase 4: Nested Property Access (Required for Tests 5-6)

**Priority:** MEDIUM
**Files:** `src/cypher/cypher-evaluator.c`

* [ ] Implement chained property access `map.key[index]`
* [ ] Support deep nesting `obj.a.b.c[0].d`
* [ ] Add null-safe property access operators
* [ ] Optimize property access performance

### Phase 5: Path Support (Required for Test 7)

**Priority:** HIGH
**Files:** `include/cypher.h`, `src/cypher/cypher-value.c`

* [ ] Implement path data type (`CYPHER_VALUE_PATH`)
* [ ] Add path construction and traversal
* [ ] Support path collection in lists/maps
* [ ] Implement path deletion logic

### Phase 6: Semantic Validation (Required for Tests 8-9)

**Priority:** MEDIUM
**Files:** `src/cypher/cypher-ast.c`, `src/cypher/cypher-validator.c` (new)

* [ ] Implement variable scope analysis
* [ ] Add undefined variable detection
* [ ] Implement expression type checking
* [ ] Validate DELETE argument types
* [ ] Add compile-time error reporting

### Phase 7: Testing & Verification

**Priority:** MEDIUM

* [ ] Test collection operations with DELETE
* [ ] Verify nested property access
* [ ] Test semantic validation edge cases
* [ ] Performance testing for complex expressions
* [ ] Validate error handling and messages

***

## Technical Architecture

### Collection DELETE Processing

```c
// For DELETE friends[$friendIndex]:
1. Evaluate `friends` -> List of nodes
2. Evaluate `$friendIndex` -> Integer index
3. Extract node at index from list
4. Apply DELETE logic to extracted node
5. Handle DETACH if specified
```

### Nested Access Chain

```c
// For DELETE rels.key.key[0]:
1. Evaluate `rels` -> Map
2. Access `rels.key` -> Nested Map
3. Access `rels.key.key` -> List of relationships
4. Index `rels.key.key[0]` -> Specific relationship
5. Apply DELETE to relationship
```

### Semantic Validation Rules

```c
DELETE expression validation:
1. Must be node, relationship, path, or collection thereof
2. Cannot be literal values (numbers, strings, booleans)
3. Cannot be undefined variables
4. Collections must contain only graph elements
```

***

## Current Implementation Status

* **Basic map property deletion**: ✅ Working (Tests 3-4)
* **WITH clause**: ❌ Not implemented
* **Collection functions**: ❌ Not implemented
* **List data type**: ❌ Not implemented
* **Path operations**: ❌ Not implemented
* **Semantic validation**: ❌ Not implemented

***

## Dependencies

This feature depends on:

1. **WITH clause support** (for data flow between clauses)
2. **Collection data types** (lists and enhanced maps)
3. **Aggregation functions** (collect, etc.)
4. **Path data type** (for complex graph structures)
5. **Semantic analysis** (for validation)

***

## Next Steps

1. **Immediate:** Implement WITH clause (unblocks collection tests)
2. **Short-term:** Add collection functions and list operations
3. **Medium-term:** Implement path support and semantic validation
4. **Long-term:** Optimize complex expression evaluation

***

## Notes

* **Map property access already works perfectly** ✅
* **Major parser and type system extensions needed** ⚠️
* **Tests 3-4 validate core DELETE + map functionality** ✅
* **Collection operations are significant undertaking** ⚠️

The built-in data type interoperation requires substantial extensions to the Cypher type system and expression evaluation engine. The basic functionality works, but advanced collection operations need significant architectural work.
