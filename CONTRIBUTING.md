# Contributing Guide

We welcome contributions to the SQLite Graph Database Extension! This guide will help you get started with development, testing, and submitting contributions.

## Table of Contents

- [Development Setup](#development-setup)
- [Code Style](#code-style)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)
- [Project Structure](#project-structure)
- [Debugging](#debugging)
- [Performance](#performance)

## Development Setup

### Prerequisites

- GCC 4.8+ or Clang 3.8+
- Make
- Git
- Python 3.7+ (for testing and examples)
- Valgrind (optional, for memory debugging)

### Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork**:
   ```bash
   git clone https://github.com/agentflare-ai/sqlite-graph.git
   cd sqlite-graph
   ```

3. **Set up the build environment**:
   ```bash
   # Build in debug mode
   make debug
   
   # Run tests to verify setup
   make test
   ```

4. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

### Development Workflow

1. **Make your changes** in the appropriate source files
2. **Build and test** frequently:
   ```bash
   make clean && make debug
   make test
   ```
3. **Run specific tests** for your changes:
   ```bash
   cd build/tests && ./test_cypher_basic
   ```
4. **Check for memory leaks**:
   ```bash
   valgrind --leak-check=full ./build/tests/test_cypher_basic
   ```

## Code Style

### C Code Standards

We follow SQLite's coding conventions:

- **Standard**: C99 with GNU extensions (`-std=gnu99`)
- **Indentation**: 2 spaces (no tabs)
- **Line length**: 80 characters maximum
- **Naming**: `snake_case` for functions, `UPPER_CASE` for macros
- **Comments**: ANSI C-89 style (`/* */`)

### Example Code Style

```c
/*
** Add a node to the graph with proper error handling.
** Returns SQLITE_OK on success, error code on failure.
*/
int graphAddNode(GraphVtab *pVtab, sqlite3_int64 iNodeId, 
                 const char *zProperties) {
  GraphNode *pNode;
  int rc = SQLITE_OK;
  
  /* Validate input parameters */
  if( pVtab==NULL || iNodeId<0 ){
    return SQLITE_MISUSE;
  }
  
  /* Check if node already exists */
  pNode = graphFindNode(pVtab, iNodeId);
  if( pNode!=NULL ){
    return SQLITE_CONSTRAINT;
  }
  
  /* Allocate new node structure */
  pNode = (GraphNode*)sqlite3_malloc(sizeof(GraphNode));
  if( pNode==NULL ){
    return SQLITE_NOMEM;
  }
  
  /* Initialize node */
  memset(pNode, 0, sizeof(GraphNode));
  pNode->iNodeId = iNodeId;
  
  /* Copy properties if provided */
  if( zProperties!=NULL ){
    pNode->zProperties = sqlite3_mprintf("%s", zProperties);
    if( pNode->zProperties==NULL ){
      sqlite3_free(pNode);
      return SQLITE_NOMEM;
    }
  }
  
  /* Add to graph */
  pNode->pNext = pVtab->pNodes;
  pVtab->pNodes = pNode;
  
  return SQLITE_OK;
}
```

### Key Style Rules

1. **Use `sqlite3_malloc()` and `sqlite3_free()`** for memory management
2. **Check return values** for all function calls
3. **Use `UNUSED(x)` macro** for unused parameters
4. **Initialize all variables** before use
5. **Follow SQLite error codes** (`SQLITE_OK`, `SQLITE_NOMEM`, etc.)
6. **Use `testcase()` macro** for boundary conditions

### Header Files

- **Include guards**: Use `#ifndef FILENAME_H` / `#define FILENAME_H` / `#endif`
- **Forward declarations**: Use `typedef struct StructName StructName;`
- **Documentation**: Document all public functions with SQLite-style comments

## Testing

### Test Structure

Tests are organized by functionality:

- `test_cypher_basic.c` - Basic Cypher operations
- `test_write_simple.c` - Graph modification tests
- `test_algorithms.c` - Graph algorithm tests
- `test_performance.c` - Performance and stress tests

### Writing Tests

Use the Unity testing framework:

```c
#include "unity.h"
#include "graph.h"

void setUp(void) {
  /* Setup code before each test */
}

void tearDown(void) {
  /* Cleanup code after each test */
}

void test_graphAddNode_success(void) {
  GraphVtab *pVtab = createTestGraph();
  int rc;
  
  rc = graphAddNode(pVtab, 1, "{\"name\": \"Alice\"}");
  TEST_ASSERT_EQUAL(SQLITE_OK, rc);
  
  GraphNode *pNode = graphFindNode(pVtab, 1);
  TEST_ASSERT_NOT_NULL(pNode);
  TEST_ASSERT_EQUAL(1, pNode->iNodeId);
  
  destroyTestGraph(pVtab);
}

void test_graphAddNode_duplicate(void) {
  GraphVtab *pVtab = createTestGraph();
  int rc;
  
  /* Add first node */
  rc = graphAddNode(pVtab, 1, "{\"name\": \"Alice\"}");
  TEST_ASSERT_EQUAL(SQLITE_OK, rc);
  
  /* Try to add duplicate */
  rc = graphAddNode(pVtab, 1, "{\"name\": \"Bob\"}");
  TEST_ASSERT_EQUAL(SQLITE_CONSTRAINT, rc);
  
  destroyTestGraph(pVtab);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_graphAddNode_success);
  RUN_TEST(test_graphAddNode_duplicate);
  return UNITY_END();
}
```

### Running Tests

```bash
# Run all tests
make test

# Run specific test
cd build/tests && ./test_cypher_basic

# Run with verbose output
cd build/tests && ./test_cypher_basic -v

# Run with memory checking
valgrind --leak-check=full ./build/tests/test_cypher_basic
```

### Test Guidelines

1. **Test edge cases**: NULL pointers, empty strings, boundary values
2. **Test error conditions**: Memory allocation failures, constraint violations
3. **Use descriptive test names**: `test_function_condition_expectedResult`
4. **Clean up resources**: Always free allocated memory in tests
5. **Test both success and failure paths**

## Submitting Changes

### Before Submitting

1. **Run the full test suite**:
   ```bash
   make test
   ```

2. **Check for memory leaks**:
   ```bash
   make valgrind
   ```

3. **Verify code style**:
   ```bash
   # No automated checker yet, review manually
   ```

4. **Update documentation** if needed

### Pull Request Process

1. **Create a clear commit message**:
   ```bash
   git commit -m "Add support for graph traversal optimization
   
   - Implement breadth-first search with pruning
   - Add performance tests for large graphs
   - Update API documentation"
   ```

2. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

3. **Create a pull request** with:
   - Clear description of changes
   - Reference to related issues
   - Test results
   - Documentation updates

### Pull Request Template

```markdown
## Description
Brief description of the changes made.

## Related Issues
Closes #123

## Changes Made
- [ ] Added new feature X
- [ ] Fixed bug in Y
- [ ] Updated documentation

## Testing
- [ ] All existing tests pass
- [ ] Added new tests for changes
- [ ] Tested with Valgrind
- [ ] Performance testing completed

## Documentation
- [ ] Updated API documentation
- [ ] Added example code
- [ ] Updated README if needed
```

## Project Structure

```
sqlite-graph/
├── src/                    # Source code
│   ├── graph.c            # Main extension entry point
│   ├── graph-vtab.c       # Virtual table implementation
│   ├── graph-algo.c       # Graph algorithms
│   └── cypher/            # Cypher query engine
│       ├── cypher-lexer.c
│       ├── cypher-parser.c
│       └── cypher-executor.c
├── include/               # Header files
│   └── graph.h           # Public API
├── tests/                 # Test files
│   ├── test_cypher_basic.c
│   └── test_algorithms.c
├── examples/              # Example applications
├── docs/                  # Documentation
└── build/                 # Build output
```

### Key Components

- **`src/graph.c`**: SQLite extension entry point and initialization
- **`src/graph-vtab.c`**: Virtual table interface implementation
- **`src/cypher/`**: Cypher query language parser and executor
- **`src/graph-algo.c`**: Graph algorithms (shortest path, centrality, etc.)
- **`include/graph.h`**: Public API definitions

## Debugging

### Debugging Tools

1. **GDB**: Standard debugger
   ```bash
   gdb ./build/tests/test_cypher_basic
   (gdb) run
   (gdb) bt
   ```

2. **Valgrind**: Memory debugging
   ```bash
   valgrind --leak-check=full --show-leak-kinds=all ./build/tests/test_cypher_basic
   ```

3. **AddressSanitizer**: Build with sanitizer
   ```bash
   export CFLAGS="-g -O1 -fsanitize=address"
   make clean && make
   ```

### Debug Build

```bash
# Build with debug symbols
make debug

# Build with maximum debugging
export CFLAGS="-g -O0 -DDEBUG -Wall -Wextra"
make clean && make
```

### Common Issues

1. **Memory leaks**: Always pair `sqlite3_malloc()` with `sqlite3_free()`
2. **Null pointer dereference**: Check pointers before use
3. **Buffer overflows**: Use `snprintf()` instead of `sprintf()`
4. **Use after free**: Set pointers to NULL after freeing

## Performance

### Performance Testing

```bash
# Run performance tests
cd build/tests && ./test_performance

# Profile with gprof
export CFLAGS="-g -pg"
make clean && make
cd build/tests && ./test_performance
gprof ./test_performance gmon.out > profile.txt
```

### Optimization Guidelines

1. **Use appropriate data structures**: Hash tables for lookups, linked lists for iteration
2. **Minimize memory allocations**: Reuse buffers when possible
3. **Cache frequently accessed data**: Node/edge counts, schema information
4. **Use indexes**: Label and property indexes for fast queries
5. **Optimize critical paths**: Query execution, graph traversal

### Benchmarking

```c
#include <time.h>

void benchmark_operation(void) {
  clock_t start = clock();
  
  /* Perform operation */
  
  clock_t end = clock();
  double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Operation took %f seconds\n", time_taken);
}
```

## Code Review Guidelines

### For Contributors

1. **Write clear commit messages**
2. **Keep changes focused** - one feature per PR
3. **Add tests** for new functionality
4. **Update documentation** as needed
5. **Respond to feedback** promptly

### For Reviewers

1. **Check code style** and conventions
2. **Verify test coverage** for changes
3. **Test functionality** manually
4. **Review performance** implications
5. **Provide constructive feedback**

## Release Process

For maintainers with write access to create releases:

1. **Update VERSION file** with the new version number
2. **Update CHANGELOG.md** with new features and fixes
3. **Create the release** using one of these methods:
   
   **Option A: GitHub UI (Recommended)**
   - Go to Actions → "Create Release" workflow → "Run workflow"
   
   **Option B: Command Line**
   ```bash
   ./scripts/create-release.sh
   ```
4. **Monitor the release workflow** at GitHub Actions
5. **Verify the release** appears on GitHub Releases page

For detailed instructions, see [docs/RELEASE_PROCESS.md](docs/RELEASE_PROCESS.md).

## Getting Help

- **GitHub Issues**: For bugs and feature requests
- **GitHub Discussions**: For questions and general discussion
- **Code Review**: Tag maintainers for review
- **Email**: For security issues only

## License

By contributing to this project, you agree that your contributions will be licensed under the MIT License.

---

**Thank you for contributing to the AgentFlare AI SQLite Graph Database Extension!**
