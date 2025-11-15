# Expression Evaluation Support

## Overview

The Cypher expression evaluator provides runtime evaluation of expressions in openCypher queries. This document describes the supported expression types, operators, functions, and semantics.

## Architecture

### Components

1. **AST-to-Expression Converter** (`cypherAstToExpression`)
   - Converts parser AST nodes to evaluator expression objects
   - Handles recursive expression trees

2. **Expression Evaluator** (`cypherExpressionEvaluate`)
   - Evaluates expression trees with execution context
   - Supports variable lookup and property access

3. **Public API** (`cypherEvaluateAstExpression`)
   - Main entry point for evaluating AST expressions
   - Combines conversion and evaluation in one call

### Files

- `src/cypher/cypher-expressions.c` - Core evaluator implementation
- `include/cypher-expressions.h` - Public API and type definitions

## Supported Expression Types

### Literals

Literal values that evaluate to themselves:

- **Strings**: `'hello'`, `"world"`
- **Integers**: `42`, `-10`, `0`
- **Floats**: `3.14`, `-0.5`, `1e-10`
- **Booleans**: `true`, `false`
- **Null**: `null`

```cypher
MERGE (p:Person {id: 1})
ON CREATE SET p.name = 'Alice', p.age = 30
```

### Variables

References to variables bound in the execution context:

```cypher
MATCH (p:Person)
SET p.visits = p.visits + 1
```

### Property Access

Access properties of nodes, relationships, or maps:

```cypher
MATCH (p:Person)
RETURN p.name, p.age
```

**Current Limitations:**
- Property access on nodes/relationships requires database lookup (not yet implemented)
- Map property access is fully supported

### Arithmetic Operators

Binary arithmetic operations on numeric values:

| Operator | Description | Example |
|----------|-------------|---------|
| `+` | Addition (or string concatenation) | `p.age + 1` or `'hello' + ' world'` |
| `-` | Subtraction | `p.balance - 100` |
| `*` | Multiplication | `p.price * 2` |
| `/` | Division | `p.total / p.count` |
| `%` | Modulo | `p.value % 10` |
| `^` | Power | `p.base ^ 2` |

**Type Handling:**
- Integer + Integer → Integer
- Integer / Integer → Float
- Integer + Float → Float
- String + any → String (concatenation, automatic type conversion)
- NULL + any → NULL

**Examples:**

```cypher
MERGE (p:Product {sku: 'ABC'})
ON MATCH SET p.stock = p.stock + 10

MERGE (u:User {id: 1})
ON CREATE SET u.credits = 100 * 2
```

### String Concatenation (Phase 04.2)

**The `+` operator performs string concatenation when at least one operand is a string:**

```cypher
MERGE (p:Person {id: 1})
ON CREATE SET
  p.fullName = p.first + ' ' + p.last,
  p.label = 'User #' + toString(p.id)

MERGE (u:User {email: 'user@example.com'})
ON MATCH SET
  u.displayName = u.first + ' ' + u.last,
  u.greeting = 'Hello, ' + u.name + '!'
```

**Type Conversion:**
- Non-string values are automatically converted to strings
- Use `toString()` for explicit conversion: `'Count: ' + toString(42)`
- Empty strings are supported: `'' + 'test'` → `'test'`

**Null Handling:**
- `'hello' + NULL` → `NULL` (null propagation)
- Use `coalesce()` to provide defaults: `coalesce(p.name, 'Unknown') + ' Smith'`

**Examples:**

```cypher
-- Basic concatenation
MERGE (p:Person {id: 1})
ON CREATE SET p.fullName = 'John' + ' ' + 'Doe'

-- With variables
MERGE (p:Person {id: 2})
ON MATCH SET p.fullName = p.first + ' ' + p.last

-- With type conversion
MERGE (u:User {id: 3})
ON CREATE SET u.label = 'User #' + toString(u.id)

-- Complex expression
MERGE (p:Product {id: 4})
ON CREATE SET
  p.description = p.name + ' (Price: $' + toString(p.price) + ')'
```

### Comparison Operators

Binary comparison operations that return booleans:

| Operator | Description | Example |
|----------|-------------|---------|
| `=` | Equal | `p.age = 30` |
| `<>` | Not equal | `p.status <> 'deleted'` |
| `<` | Less than | `p.price < 100` |
| `<=` | Less than or equal | `p.age <= 18` |
| `>` | Greater than | `p.score > 50` |
| `>=` | Greater than or equal | `p.balance >= 0` |

**String Operators:**

| Operator | Description | Example |
|----------|-------------|---------|
| `STARTS WITH` | String prefix | `p.name STARTS WITH 'A'` |
| `ENDS WITH` | String suffix | `p.email ENDS WITH '@gmail.com'` |
| `CONTAINS` | String contains | `p.text CONTAINS 'error'` |

**NULL Handling:**
- NULL = NULL → NULL (not true!)
- Use `IS NULL` / `IS NOT NULL` for null checks

### Logical Operators

Boolean logic operations:

| Operator | Description | Example |
|----------|-------------|---------|
| `AND` | Logical AND | `p.age > 18 AND p.age < 65` |
| `OR` | Logical OR | `p.admin OR p.moderator` |
| `NOT` | Logical NOT | `NOT p.deleted` |

**NULL Handling:**
- AND: NULL AND true → NULL, NULL AND false → false
- OR: NULL OR true → true, NULL OR false → NULL
- NOT: NOT NULL → NULL

## Built-in Functions

### String Functions

| Function | Description | Example |
|----------|-------------|---------|
| `toUpper(s)` | Convert to uppercase | `toUpper('hello')` → `'HELLO'` |
| `toLower(s)` | Convert to lowercase | `toLower('WORLD')` → `'world'` |
| `toString(x)` | Convert to string | `toString(42)` → `'42'` |
| `length(s)` | String length | `length('hello')` → `5` |
| `size(list)` | List/string size | `size([1,2,3])` → `3` |

### Numeric Functions

| Function | Description | Example |
|----------|-------------|---------|
| `toInteger(x)` | Convert to integer | `toInteger('42')` → `42` |
| `toFloat(x)` | Convert to float | `toFloat('3.14')` → `3.14` |
| `abs(n)` | Absolute value | `abs(-5)` → `5` |
| `ceil(n)` | Ceiling | `ceil(3.2)` → `4.0` |
| `floor(n)` | Floor | `floor(3.8)` → `3.0` |
| `round(n)` | Round | `round(3.6)` → `4.0` |
| `sqrt(n)` | Square root | `sqrt(16)` → `4.0` |

### Null Handling Functions

| Function | Description | Example |
|----------|-------------|---------|
| `coalesce(e1, e2, ...)` | First non-null value | `coalesce(p.name, 'Unknown')` |

**Usage in Queries:**

```cypher
MERGE (p:Person {id: 1})
ON MATCH SET p.visits = coalesce(p.visits, 0) + 1

MERGE (u:User {id: 42})
ON CREATE SET u.score = toInteger('100')
```

### Aggregate Functions

| Function | Description | Example |
|----------|-------------|---------|
| `count(x)` | Count values | `count(*)` |
| `sum(n)` | Sum values | `sum(p.amount)` |
| `avg(n)` | Average | `avg(p.score)` |
| `min(x)` | Minimum | `min(p.price)` |
| `max(x)` | Maximum | `max(p.age)` |

*Note: Aggregate functions are currently basic implementations for single values. Full aggregation with GROUP BY is planned.*

## Type Coercion

### Automatic Coercion

The evaluator performs automatic type coercion in these cases:

1. **Arithmetic operations**: Integer + Float → Float
2. **String concatenation**: `toString()` applied implicitly (future)
3. **Boolean context**: Non-zero numbers → true, empty strings → false

### Explicit Conversion

Use conversion functions for explicit type changes:

```cypher
SET p.age = toInteger('30')
SET p.score = toFloat('98.5')
SET p.id = toString(12345)
```

## Null Propagation

### General Rule

Most operations propagate NULL:

- `NULL + 5` → `NULL`
- `NULL = 'test'` → `NULL`
- `NULL AND true` → `NULL` (with exceptions)

### Exceptions

1. **coalesce()**: Returns first non-null
2. **IS NULL / IS NOT NULL**: Always returns boolean
3. **OR with true**: `NULL OR true` → `true`

## Usage in MERGE Clauses

### ON MATCH with Expressions

```cypher
MERGE (p:Person {id: 1})
ON MATCH SET
  p.visits = p.visits + 1,
  p.lastSeen = timestamp(),
  p.name = coalesce(p.name, 'Unknown')
```

### ON CREATE with Expressions

```cypher
MERGE (p:Product {sku: 'ABC'})
ON CREATE SET
  p.price = 50 * 2,
  p.created = 2024,
  p.stock = toInteger('100')
```

### Complex Expressions

```cypher
MERGE (u:User {email: 'user@example.com'})
ON MATCH SET
  u.loginCount = coalesce(u.loginCount, 0) + 1,
  u.lastLogin = timestamp()
ON CREATE SET
  u.loginCount = 1,
  u.created = timestamp()
```

## Current Limitations

### Not Yet Supported

1. **Property Access on Nodes/Relationships**
   - Pattern: `p.name` where `p` is a node
   - Currently returns NULL
   - Requires database lookup implementation

2. **List Comprehensions**
   - Pattern: `[x IN list | x.prop]`
   - Planned for future release

3. **CASE Expressions**
   - Pattern: `CASE WHEN ... THEN ... END`
   - Planned for future release

4. **Pattern Expressions**
   - Pattern: `(a)-[:KNOWS]->(b)` as expression
   - Planned for future release

5. **Map Literals**
   - Pattern: `{key: value}` in expressions
   - Basic support exists, full implementation pending

6. **String Concatenation**
   - Pattern: `p.first + ' ' + p.last`
   - ✅ **IMPLEMENTED** in Phase 04.2
   - Automatically handles type conversion to string

### Known Issues

1. **Performance**: Expression evaluation adds overhead
2. **Error Messages**: Limited error context for type mismatches
3. **Nested Properties**: `p.address.city` not supported

## Performance Considerations

### Expression Overhead

Each expression evaluation involves:
1. AST-to-Expression conversion
2. Recursive evaluation
3. Value copying for results

**Recommendations:**
- Use literals where possible
- Avoid deeply nested expressions
- Cache computed values in variables

### Optimization

Future optimizations planned:
- Constant folding
- Expression caching
- JIT compilation for hot paths

## Testing

### Unit Tests

Expression evaluator tests: `tests/test_expression_evaluator.c` (planned)

### Integration Tests

MERGE with expressions: `tests/test_merge_clauses.c`

```c
// Example test
void test_merge_on_match_with_expression(void) {
    // Test p.visits + 1
    // Test coalesce(p.name, 'default')
    // Test arithmetic: 42 * 2
}
```

## Migration Guide

### Updating Existing Queries

Old (literals only):
```cypher
MERGE (p:Person {id: 1})
ON MATCH SET p.visits = 10
```

New (with expressions):
```cypher
MERGE (p:Person {id: 1})
ON MATCH SET p.visits = p.visits + 1
```

### Compatibility

- Literal-only queries continue to work
- New expression support is backward compatible
- No breaking changes to existing functionality

## Future Enhancements

### Planned Features

1. **Full Property Access**
   - Load nodes/relationships from database
   - Support nested properties

2. **String Operations**
   - Concatenation with `+`
   - Substring, trim, replace

3. **List Operations**
   - List comprehensions
   - Filter, map, reduce

4. **Date/Time Functions**
   - timestamp(), datetime(), date()
   - Date arithmetic

5. **Regular Expressions**
   - Pattern matching with `=~`
   - Extract, replace operations

### Performance Improvements

1. Expression caching
2. Constant folding optimization
3. Type inference for better performance
4. Lazy evaluation where possible

## References

- [openCypher Specification](https://opencypher.org/)
- [Cypher Expression Syntax](https://neo4j.com/docs/cypher-manual/current/syntax/expressions/)
- [Phase 04.1 Implementation Plan](../.agents/plans/merge/04.1_Expression_Evaluation.phase.md)

## Changelog

### Phase 04.2 (Current)

**Added:**
- **String concatenation with `+` operator** (primary feature)
- Automatic type conversion for string concatenation
- Comprehensive unit test suite (23 passing tests)
- Enhanced documentation with string concatenation examples

**Test Coverage:**
- Literal values (integers, strings, floats)
- Arithmetic operators with expressions
- String concatenation (basic, with variables, with toString, null handling)
- Type conversion functions
- NULL handling functions
- String functions
- Complex nested expressions

### Phase 04.1

**Added:**
- AST-to-Expression conversion
- Runtime expression evaluation
- Arithmetic operators (+, -, *, /, %, ^)
- Comparison operators (=, <>, <, >, <=, >=, STARTS WITH, ENDS WITH, CONTAINS)
- Logical operators (AND, OR, NOT)
- Type conversion functions (toInteger, toFloat, toString)
- Null handling function (coalesce)
- String functions (toUpper, toLower, length)
- Math functions (abs, ceil, floor, round, sqrt)
- Map property access
- Integration with MERGE ON MATCH/ON CREATE

**Remaining Limitations:**
- Node/relationship property access returns NULL (requires database lookup)
- No CASE expressions
- No list comprehensions

---

*Last Updated: 2024 Phase 04.2*
*Status: String Concatenation & Testing Complete*
