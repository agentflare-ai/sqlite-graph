# Variable-Length Relationship Implementation Specification

**Date:** 2025-01-17
**Status:** NOT STARTED - Specification Phase
**Priority:** HIGH - Required for Match4 and Match5 TCK compliance

---

## Executive Summary

Variable-length relationships (`[*]`, `[*n]`, `[*min..max]`) are a critical Cypher feature that enables graph traversal patterns. This specification outlines the complete implementation requirements based on TCK scenarios from Match4 and Match5.

**Scope:**
- **Match4:** 10 TCK scenarios testing variable-length pattern syntax and semantics
- **Match5:** 29 TCK scenarios testing variable-length patterns on complex graphs
- **Total:** 39 scenarios requiring full path-finding implementation

**Current Status:**
- Parser: Detects `*` token but only to reject it in MERGE
- AST: No fields for min/max hop bounds
- Planner: No variable-length relationship operators
- Executor: Has `cypherExpand()` function (foundation exists)

---

## TCK Requirements Analysis

### Match4 - Variable Length Patterns (10 scenarios)

#### Syntax Requirements

1. **Fixed-length variable pattern** `[*1..1]`
   - Returns list of relationships
   - Example: `[[:T]]` for single relationship

2. **Unbounded pattern** `[*]`
   - Min: 1 hop (at least one relationship)
   - Max: unlimited
   - Traverses entire reachable subgraph

3. **Zero-length patterns** `[*0..1]`, `[*0..0]`
   - `*0..0` returns the starting node (zero hops)
   - `*0..1` includes starting node + 1-hop neighbors

4. **Range patterns** `[*min..max]`
   - Both bounds inclusive
   - Example: `[*1..3]` matches 1, 2, or 3 hops

5. **Open upper bound** `[*min..]`
   - Example: `[*2..]` means 2 or more hops

6. **Open lower bound** `[*..max]`
   - Example: `[*..2]` means 0, 1, or 2 hops (implied min=0)

7. **Type filtering** `[:TYPE*]`
   - Only traverse relationships of specified type(s)
   - Example: `[:KNOWS*1..3]`

8. **Property filtering** `[*{prop: value}]`
   - Only traverse relationships matching properties
   - Example: `[:WORKED_WITH*{year: 1988}]`

9. **Binding to variable** `-[r*1..2]->`
   - Variable `r` contains list of relationships
   - Example: `r = [[:X], [:Y]]`

10. **Error conditions**
    - Missing `*`: `[:TYPE..]` → SyntaxError
    - Negative bounds: `[*-2]` → SyntaxError

#### Execution Requirements

- **Path finding:** BFS or DFS algorithm to find all paths
- **Cycle detection:** Prevent infinite loops
- **Length constraints:** Enforce min/max hop counts
- **Result format:** List of relationships for each path
- **Performance:** Handle deep traversals (e.g., 20+ hops)

### Match5 - Variable Length Over Graphs (29 scenarios)

Tests variable-length patterns on carefully constructed graphs:

1. **Tree structures** (scenarios 1-18)
   - Binary tree traversal
   - All combinations of bounds
   - Tests min/max enforcement

2. **Chained patterns** (scenarios 19-25)
   - Variable-length followed by fixed-length
   - Fixed-length followed by variable-length
   - Multiple variable-length in same pattern

3. **Direction handling** (scenarios 26-27)
   - Forward, backward, bidirectional
   - Mixed directions in patterns

4. **Complex patterns** (scenarios 28-29)
   - Multiple variable-length segments
   - Intermediate node binding

**Key Edge Cases from Match5:**
- Empty intervals: `[*2..1]`, `[*1..0]` → should return 0 results
- Zero-length: `[*0]`, `[*0..0]` → returns starting node
- Very large depths: 20+ hop traversals
- Dense graphs with many paths

---

## Implementation Architecture

### Phase 1: Parser Extensions

**File:** `src/cypher/cypher-parser.c`

**Current Code Location:** Lines 860-865
```c
if (pNext && pNext->type == CYPHER_TOK_MULT && pParser && pParser->bInMergeClause) {
    parserSetError(pParser, pLexer, "MERGE does not support variable-length relationships");
    cypherAstDestroy(pRelPattern);
    cypherAstDestroy(pPattern);
    return NULL;
}
```

**Required Changes:**

1. **Parse `*` token after relationship type/properties**
   - Location: After line 849 (after property map parsing)
   - Check for `CYPHER_TOK_MULT`

2. **Parse range syntax**
   - `*` alone → min=1, max=INT_MAX
   - `*n` → min=n, max=n
   - `*min..max` → both specified
   - `*..max` → min=0, max specified
   - `*min..` → min specified, max=INT_MAX

3. **Validate bounds**
   - No negative numbers
   - min <= max (unless empty interval intended)
   - Reject if in MERGE clause

4. **Store in AST**
   - Add fields to `CYPHER_AST_REL_PATTERN` node
   - Need: `iMinHops`, `iMaxHops` fields

**Pseudo-code:**
```c
// After parsing properties, before ']'
pNext = parserPeekToken(pLexer);
if (pNext && pNext->type == CYPHER_TOK_MULT) {
    if (pParser->bInMergeClause) {
        return error("MERGE doesn't support variable-length");
    }

    parserConsumeToken(pLexer, CYPHER_TOK_MULT);

    int minHops = 1, maxHops = INT_MAX;  // defaults for [*]

    pNext = parserPeekToken(pLexer);
    if (pNext && pNext->type == CYPHER_TOK_INTEGER) {
        // Could be [*n] or [*min..max]
        int firstNum = parseInteger(pLexer);

        pNext = parserPeekToken(pLexer);
        if (pNext && pNext->type == CYPHER_TOK_DOTDOT) {
            // [*min..max] or [*min..]
            parserConsumeToken(pLexer, CYPHER_TOK_DOTDOT);
            minHops = firstNum;

            pNext = parserPeekToken(pLexer);
            if (pNext && pNext->type == CYPHER_TOK_INTEGER) {
                maxHops = parseInteger(pLexer);
            } else {
                maxHops = INT_MAX;  // [*min..]
            }
        } else {
            // [*n]
            minHops = maxHops = firstNum;
        }
    } else if (pNext && pNext->type == CYPHER_TOK_DOTDOT) {
        // [*..max]
        parserConsumeToken(pLexer, CYPHER_TOK_DOTDOT);
        minHops = 0;

        pNext = parserPeekToken(pLexer);
        if (pNext && pNext->type == CYPHER_TOK_INTEGER) {
            maxHops = parseInteger(pLexer);
        } else {
            // [*..]  equivalent to [*]
            minHops = 1;
            maxHops = INT_MAX;
        }
    }
    // else: [*] already has defaults

    // Validate
    if (minHops < 0 || maxHops < 0) {
        return error("Negative bounds not allowed");
    }
    if (minHops > maxHops) {
        // Empty interval - allowed but will match nothing
    }

    // Store in AST
    pRelPattern->iMinHops = minHops;
    pRelPattern->iMaxHops = maxHops;
    pRelPattern->iFlags |= REL_PATTERN_VARLEN;
}
```

### Phase 2: AST Extensions

**File:** `include/cypher.h`

**Add fields to relationship pattern structure:**

```c
struct CypherAst {
    CypherAstNodeType type;
    char *zValue;           // Existing: identifier, literal, etc.
    int iFlags;            // Existing: direction, etc.

    // NEW FOR VARIABLE-LENGTH:
    int iMinHops;          // Minimum hops (default 0 for fixed-length)
    int iMaxHops;          // Maximum hops (default 0 for fixed-length, INT_MAX for *)

    CypherAst **apChildren;
    int nChildren;
    int nChildrenAlloc;
    int iLine;
    int iColumn;
};

// Flag to indicate variable-length relationship
#define REL_PATTERN_VARLEN  0x0100
```

**Initialization:**
```c
// In cypherAstCreate():
pAst->iMinHops = 0;
pAst->iMaxHops = 0;  // 0 means fixed-length (1 hop)
```

### Phase 3: Planner Extensions

**File:** `src/cypher/cypher-planner.c`

**Current:** Line 1784-1786 handles `CYPHER_AST_REL_PATTERN` as simple expand

**Required:**

1. **Detect variable-length patterns**
   ```c
   case CYPHER_AST_REL_PATTERN:
       int minHops = pRelAst->iMinHops;
       int maxHops = pRelAst->iMaxHops;

       if (maxHops > 0) {
           // Variable-length relationship
           pLogical = logicalPlanNodeCreate(LOGICAL_VAR_EXPAND);
           pLogical->iMinHops = minHops;
           pLogical->iMaxHops = maxHops;
       } else {
           // Fixed-length (existing code)
           pLogical = logicalPlanNodeCreate(LOGICAL_EXPAND);
       }
   ```

2. **Add LOGICAL_VAR_EXPAND operator type**
   ```c
   typedef enum {
       LOGICAL_EXPAND,
       LOGICAL_VAR_EXPAND,   // NEW
       // ...
   } LogicalOperatorType;
   ```

3. **Physical plan compilation**
   - Convert `LOGICAL_VAR_EXPAND` to `PHYSICAL_VAR_EXPAND`
   - Pass min/max hops to executor

### Phase 4: Execution Engine

**File:** `src/cypher/cypher-iterators.c`

**New Iterator:** `VarLengthExpandIterator`

**Algorithm: BFS (Breadth-First Search)**

Why BFS:
- Finds shortest paths first (important for min bound)
- Natural level-by-level traversal
- Easier to implement hop count tracking

**Core Structure:**
```c
typedef struct VarLengthExpandIterator {
    BaseIterator base;

    // Configuration
    int minHops;
    int maxHops;
    char *zRelType;           // NULL = any type
    CypherValue *pProperties; // NULL = any properties
    int iDirection;           // 1=forward, -1=backward, 0=both

    // State
    sqlite3_int64 iStartNode;
    PathQueue *pQueue;        // BFS queue of partial paths
    PathSet *pVisited;        // Cycle detection
    CypherValue *pCurrentPath; // Current result path
    int iCurrentHops;         // Length of current path

} VarLengthExpandIterator;

typedef struct PathEntry {
    sqlite3_int64 *aiNodes;   // Nodes in path
    sqlite3_int64 *aiRels;    // Relationships in path
    int nHops;                // Current length
    struct PathEntry *pNext;
} PathEntry;
```

**Algorithm Pseudo-code:**
```c
int varLengthExpandNext(VarLengthExpandIterator *pIter) {
    while (1) {
        // Get next partial path from queue
        PathEntry *pPath = pathQueuePop(pIter->pQueue);
        if (!pPath) {
            return SQLITE_DONE;  // No more paths
        }

        sqlite3_int64 iCurrentNode = pPath->aiNodes[pPath->nHops];

        // If path meets minimum length, yield it
        if (pPath->nHops >= pIter->minHops) {
            pIter->pCurrentPath = pathToValue(pPath);
            pIter->iCurrentHops = pPath->nHops;

            // Continue expanding if under max
            if (pPath->nHops < pIter->maxHops) {
                expandPathAndEnqueue(pIter, pPath);
            }

            pathEntryFree(pPath);
            return SQLITE_ROW;
        }

        // Path too short, expand and continue
        if (pPath->nHops < pIter->maxHops) {
            expandPathAndEnqueue(pIter, pPath);
        }

        pathEntryFree(pPath);
    }
}

void expandPathAndEnqueue(VarLengthExpandIterator *pIter, PathEntry *pPath) {
    sqlite3_int64 iNode = pPath->aiNodes[pPath->nHops];

    // Query relationships from this node
    char *zSQL = sqlite3_mprintf(
        "SELECT id, source_id, target_id, type FROM graph_relationships "
        "WHERE %s = %lld",
        pIter->iDirection >= 0 ? "source_id" : "target_id",
        iNode
    );

    // For each matching relationship:
    sqlite3_stmt *pStmt = ...;
    while (sqlite3_step(pStmt) == SQLITE_ROW) {
        sqlite3_int64 iRelId = sqlite3_column_int64(pStmt, 0);
        sqlite3_int64 iNextNode = sqlite3_column_int64(pStmt,
            pIter->iDirection >= 0 ? 2 : 1);

        // Filter by type if specified
        if (pIter->zRelType && !matchesType(pStmt, pIter->zRelType)) {
            continue;
        }

        // Filter by properties if specified
        if (pIter->pProperties && !matchesProps(pStmt, pIter->pProperties)) {
            continue;
        }

        // Cycle detection: don't revisit nodes in current path
        if (pathContainsNode(pPath, iNextNode)) {
            continue;
        }

        // Create extended path
        PathEntry *pNewPath = pathEntryCopy(pPath);
        pathEntryAddHop(pNewPath, iRelId, iNextNode);

        // Enqueue for later processing
        pathQueuePush(pIter->pQueue, pNewPath);
    }

    sqlite3_finalize(pStmt);
}
```

**Cycle Detection Strategy:**

1. **Per-path tracking** (not global)
   - Each path tracks its own visited nodes
   - Prevents cycles within a single path
   - DOES allow revisiting nodes via different paths

2. **Why not global visited set?**
   - Graph may have multiple paths to same node
   - All valid paths must be returned
   - Example: Diamond pattern A→B→D, A→C→D
     - Global visited would miss second path to D

3. **Memory concern:**
   - Deep paths (20+ hops) could explode
   - Add safety limit (e.g., max 1000 paths in queue)

### Phase 5: Result Handling

**Relationship List Format:**

Variable-length match returns list of relationships:
```
MATCH (a)-[r*1..2]->(b)
```

`r` is bound to: `[[:REL1], [:REL2]]` or `[[:REL1]]`

**Implementation:**
- Create `CYPHER_VALUE_LIST` containing relationship values
- Each relationship is `CYPHER_VALUE_RELATIONSHIP`
- Use `executionContextBind()` to bind variable

**JSON Representation:**
```json
[
  {"id": 1, "type": "KNOWS", "properties": {...}},
  {"id": 2, "type": "KNOWS", "properties": {...}}
]
```

---

## Performance Considerations

### Exponential Explosion

Variable-length matching is O(b^d) where:
- b = branching factor (avg outgoing relationships per node)
- d = maximum depth (max hops)

Example: Dense graph with b=10, maxHops=5
- Potential paths: 10^5 = 100,000 paths
- With b=100, d=4: 100,000,000 paths

### Mitigation Strategies

1. **Path Limit**
   - Add `LIMIT n` to query (already supported)
   - Internal limit: max 10,000 paths in queue

2. **Early Termination**
   - Stop when LIMIT reached
   - Don't explore paths that can't meet min bound

3. **Bidirectional Search**
   - For patterns with both endpoints bound
   - Search from both ends, meet in middle
   - Reduces from O(b^d) to O(b^(d/2))

4. **Indexed Lookup**
   - Use relationship type index
   - Use property filters early

5. **Depth Limit**
   - System maximum (e.g., 50 hops)
   - Prevent runaway queries

---

## Test Implementation Plan

### Match4 Test Updates

**File:** `tests/tck_test_clauses_match.c`

**Current Status:** 10 test stubs, all marked `TEST_IGNORE`

**Implementation Strategy:**

For each Match4 scenario:

1. **Remove `TEST_IGNORE`**
2. **Add full test implementation**
3. **Verify expected results**

**Example: Match4_01 (Fixed-length variable pattern)**

```c
void test_clauses_match_Match4_01(void) {
    // [1] Handling fixed-length variable length pattern
    // MATCH (a)-[r*1..1]->(b) RETURN r
    // Expected: [[:T]]

    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()",
                          NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL_MESSAGE(SQLITE_OK, rc, "Failed to create virtual table");

    // Setup: CREATE ()-[:T]->()
    rc = sqlite3_exec(db,
        "INSERT INTO graph(cypher) VALUES ('CREATE ()-[:T]->()')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL_MESSAGE(SQLITE_OK, rc, "Failed to create test data");

    // Execute query
    rc = sqlite3_exec(db,
        "SELECT cypher FROM graph WHERE cypher = 'MATCH (a)-[r*1..1]->(b) RETURN r'",
        test_callback, &cb_data, &error_msg);
    TEST_ASSERT_EQUAL_MESSAGE(SQLITE_OK, rc, "Query execution failed");

    // Verify results
    TEST_ASSERT_EQUAL(1, cb_data.row_count);

    // Verify r is list containing one relationship of type T
    const char *result = cb_data.rows[0][0];
    TEST_ASSERT_NOT_NULL(result);

    // Parse JSON array
    cJSON *json = cJSON_Parse(result);
    TEST_ASSERT_NOT_NULL(json);
    TEST_ASSERT_TRUE(cJSON_IsArray(json));
    TEST_ASSERT_EQUAL(1, cJSON_GetArraySize(json));

    cJSON *rel = cJSON_GetArrayItem(json, 0);
    cJSON *type = cJSON_GetObjectItem(rel, "type");
    TEST_ASSERT_EQUAL_STRING("T", type->valuestring);

    cJSON_Delete(json);
}
```

**Pattern for all Match4 tests:**
1. Create test graph
2. Execute variable-length query
3. Verify result count
4. Verify relationship list content
5. Check types, properties where applicable

### Match5 Test Updates

**Current Status:** 29 test stubs, all marked `TEST_IGNORE`

**Implementation Strategy:**

Match5 tests use complex pre-built graphs. Need helper functions:

```c
// Helper: Create Match5 binary tree graph
static void createMatch5BinaryTree(sqlite3 *db) {
    const char *setup =
        "CREATE (n0:A {name: 'n0'}), "
        "       (n00:B {name: 'n00'}), "
        "       (n01:B {name: 'n01'}), "
        "       (n000:C {name: 'n000'}), "
        // ... all nodes ...
        "CREATE (n0)-[:LIKES]->(n00), "
        "       (n0)-[:LIKES]->(n01), "
        // ... all relationships ...
        ;

    int rc = sqlite3_exec(db,
        sqlite3_mprintf("INSERT INTO graph(cypher) VALUES ('%s')", setup),
        NULL, NULL, NULL);
    assert(rc == SQLITE_OK);
}
```

**Match5 tests verify:**
- Correct number of results
- Correct nodes returned
- Proper min/max hop enforcement
- Edge cases (zero-length, empty intervals)

---

## Integration Points

### Dependencies

1. **Lexer:** Already has `CYPHER_TOK_MULT` and `CYPHER_TOK_DOTDOT`
2. **AST:** Needs `iMinHops`, `iMaxHops` fields
3. **Planner:** Needs `LOGICAL_VAR_EXPAND` operator
4. **Executor:** Needs complete BFS iterator
5. **Storage:** Can use existing relationship queries

### Affected Components

1. **Parser** (`cypher-parser.c`)
   - Parse variable-length syntax
   - Validate bounds

2. **AST** (`cypher-ast.c`, `cypher.h`)
   - Add hop count fields
   - Serialize/deserialize for debugging

3. **Planner** (`cypher-planner.c`)
   - Detect variable-length patterns
   - Create var-expand operators

4. **Physical Plan** (`cypher-physical-plan.c`)
   - Compile LOGICAL_VAR_EXPAND to iterator

5. **Iterators** (`cypher-iterators.c`)
   - Implement VarLengthExpandIterator
   - BFS algorithm
   - Cycle detection

6. **Executor** (`cypher-executor.c`)
   - Initialize var-expand iterators
   - Handle relationship list results

---

## Validation & Testing

### Unit Tests

1. **Parser Tests**
   - Valid syntax: `[*]`, `[*2]`, `[*1..3]`, `[*..5]`, `[*2..]`
   - Invalid syntax: `[:TYPE..]`, `[*-1]`, `[*3..1]`
   - Error messages

2. **BFS Iterator Tests**
   - Simple chain: A→B→C→D
   - Binary tree traversal
   - Cycle detection
   - Path limits

3. **Bounds Enforcement**
   - Exact length: `[*2]` on 3-hop chain → 1 result
   - Range: `[*1..2]` → multiple results
   - Empty interval: `[*5..3]` → 0 results

### Integration Tests (TCK)

- All 10 Match4 scenarios
- All 29 Match5 scenarios
- 100% pass rate required

### Performance Tests

1. **Dense Graph**
   - 100 nodes, 1000 relationships
   - `MATCH ()-[*1..3]->()`
   - Should complete in <1s

2. **Deep Path**
   - Linear chain of 50 nodes
   - `MATCH ()-[*]->()`
   - Should complete in <100ms

3. **Path Explosion**
   - Dense graph with LIMIT
   - `MATCH ()-[*1..5]->() LIMIT 100`
   - Should stop at limit

---

## Implementation Phases

### Phase 1: Foundation (Week 1-2)
- [ ] Parser: Parse `[*]` syntax
- [ ] AST: Add hop count fields
- [ ] Basic validation

**Deliverable:** Parser accepts variable-length syntax

### Phase 2: Planning (Week 2-3)
- [ ] Planner: Detect variable-length
- [ ] Planner: Create var-expand operators
- [ ] Physical plan compilation

**Deliverable:** Logical/physical plans created

### Phase 3: Execution (Week 3-5)
- [ ] BFS iterator implementation
- [ ] Cycle detection
- [ ] Relationship list building
- [ ] Basic tests passing

**Deliverable:** Simple queries work

### Phase 4: Testing & Optimization (Week 5-6)
- [ ] Implement all Match4 tests
- [ ] Implement all Match5 tests
- [ ] Performance tuning
- [ ] Edge case fixes

**Deliverable:** All TCK tests pass

---

## Risk Assessment

### High Risk Areas

1. **Performance Explosion**
   - Dense graphs can generate millions of paths
   - Mitigation: Path limits, early termination

2. **Memory Usage**
   - Each path stores node/rel arrays
   - Mitigation: Limit queue size, streaming results

3. **Correctness**
   - Subtle bugs in cycle detection
   - Subtle bugs in bounds enforcement
   - Mitigation: Extensive testing, TCK compliance

### Medium Risk Areas

1. **Parser Complexity**
   - Multiple syntax variants
   - Mitigation: Systematic testing

2. **Iterator State Management**
   - BFS queue lifecycle
   - Mitigation: Careful resource management

### Low Risk Areas

1. **AST Changes**
   - Simple field additions
   - Well-defined structure

2. **Storage Queries**
   - Reuse existing relationship queries
   - Proven infrastructure

---

## Success Criteria

### Minimum Viable Product (MVP)

- [ ] Parse all variable-length syntax forms
- [ ] BFS iterator works for simple cases
- [ ] 50%+ Match4 tests passing
- [ ] No memory leaks
- [ ] No crashes on valid queries

### Production Ready

- [ ] 100% Match4 tests passing
- [ ] 80%+ Match5 tests passing
- [ ] Performance acceptable (<1s for typical queries)
- [ ] Path limits enforced
- [ ] All edge cases handled

### Full Compliance

- [ ] 100% Match4 and Match5 passing
- [ ] Performance optimized
- [ ] Comprehensive error messages
- [ ] Documentation complete

---

## References

- **TCK Feature Files:**
  - `tck/features/clauses/match/Match4.feature`
  - `tck/features/clauses/match/Match5.feature`

- **Implementation Files:**
  - Parser: `src/cypher/cypher-parser.c` (lines 750-900)
  - AST: `include/cypher.h`, `src/cypher/cypher-ast.c`
  - Planner: `src/cypher/cypher-planner.c` (line 1784+)
  - Executor: `src/cypher/cypher-iterators.c`

- **Test Files:**
  - `tests/tck_test_clauses_match.c` (Match4: lines 1494-1612, Match5: lines 1614+)

- **Existing Infrastructure:**
  - `cypherExpand()` function: `include/cypher/cypher-api.h:214`
  - Path structures: `include/cypher-executor.h`

---

## Next Steps

1. **Review this specification** with team
2. **Prioritize phases** based on project timeline
3. **Allocate resources** (estimated 6 weeks full-time)
4. **Begin Phase 1** (Parser) when approved

---

**Prepared by:** TCK Test Executor Agent
**Date:** 2025-01-17
**Version:** 1.0
