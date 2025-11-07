# SQLite Graph Extension Testing and Regression Prevention Rules

## Critical Testing Requirements

### Always Run Baseline Tests First

**MANDATORY**: Before making any changes, establish a baseline by running:

```bash
cd agentml-go/memory/extensions/sqlite_graph
make test_tck  # Run all TCK tests (1615+ scenarios)
make test      # Run basic functionality tests
./tests/comprehensive_test_runner.sh  # Full test suite
```

### TCK Compliance is Mandatory

* **TCK tests MUST pass** for any changes affecting Cypher query functionality
* **Zero regressions allowed** - all 1615+ openCypher TCK scenarios must continue passing
* TCK failures indicate **breaking changes** to openCypher compliance
* **TCK tests MUST be added for ALL new features** - any new Cypher functionality requires corresponding TCK test coverage
* **No regressions permitted** - existing TCK tests must continue to pass; any failures are blocking issues

### New Feature TCK Requirements

**MANDATORY for all new features:**

* **Create TCK test scenarios** for any new Cypher clauses, expressions, or functionality
* **Update `tck_scenarios_manifest.json`** with new test cases before feature implementation
* **Verify TCK coverage** - new features must have comprehensive TCK test scenarios
* **Regression testing** - ensure new TCK tests don't break existing functionality
* **TCK test generation** - use automated scripts to generate test cases from manifest

### Test Categories and Requirements

#### 1. Basic Functionality Tests

```bash
make test-basic  # Core extension loading, virtual tables, CRUD operations
```

* Must pass before any commits
* Covers fundamental SQLite extension functionality

#### 2. TCK (Technology Compatibility Kit) Tests

```bash
make test_tck    # All openCypher compliance tests
```

* **1615+ test scenarios** covering all Cypher clauses and expressions
* Generated from `tck_scenarios_manifest.json`
* **BLOCKING**: Failures prevent merges
* **MANDATORY for new features**: Add TCK tests before feature implementation
* **ZERO REGRESSIONS**: All existing TCK tests must continue passing

#### 3. Extended Functionality Tests

```bash
make test-extended  # Advanced features (may have expected failures)
```

* Tests for performance, edge cases, and advanced algorithms
* Some failures may be acceptable if documented

#### 4. Comprehensive Test Suite

```bash
./tests/comprehensive_test_runner.sh
```

* Runs all test categories with coverage reporting
* Provides summary of test status and next steps

## Code Change Verification Protocol

### For Cypher-Related Changes

1. **Run full TCK suite** before starting work
2. **Add TCK tests first** for new features - update manifest and generate tests
3. **Make incremental changes** with frequent testing
4. **Verify TCK compliance** after each logical change
5. **Document any expected TCK failures** with rationale

### For Core Extension Changes

1. **Run basic tests** to ensure extension loading works
2. **Test virtual table operations** (CREATE, INSERT, SELECT, etc.)
3. **Verify no memory leaks** with valgrind
4. **Run performance benchmarks** if performance-critical

### For Build System Changes

1. **Clean rebuild** from scratch: `make rebuild`
2. **Cross-platform testing** (Linux/macOS if applicable)
3. **Static analysis** passes: `cppcheck`, compiler warnings
4. **All make targets work**: `make all`, `make test`, `make clean`

## Regression Prevention Measures

### Automated Verification

```bash
./scripts/verify_all.sh  # Complete verification suite
```

* Checks for placeholder code, clean builds, test passing, memory leaks
* Static analysis and coverage infrastructure verification
* **Must pass 100%** before production deployment

### Memory Safety

```bash
make sanitize      # Build with ASAN/UBSAN
make test_hardened # Test with memory hardening
```

* Address sanitizer and undefined behavior sanitizer
* Memory leak detection and bounds checking
* Thread safety verification with helgrind

### Performance Regression Detection

```bash
./scripts/perf_regression.sh  # Performance baseline comparison
```

* Automated performance regression testing
* Benchmark comparisons against known good baselines
* Prevents performance degradation

## File-Specific Testing Requirements

### C Source Files (`src/**/*.c`)

* **Unit tests** in `tests/` directory must pass
* **TCK integration** if affecting Cypher functionality
* **Memory safety** verification with sanitizers

### Header Files (`include/**/*.h`)

* **API compatibility** - changes may break dependent code
* **Full rebuild** and test suite required
* **Documentation updates** for public APIs

### Python Files (`*.py`, `examples/**/*.py`)

* **Syntax validation** and execution testing
* **Integration tests** with SQLite extension
* **Example verification** - examples must work

### Test Files (`tests/**/*.c`, `tests/**/*.py`)

* **Self-verification** - test files must compile and run
* **Coverage requirements** - maintain test coverage levels
* **TCK test generation** - verify test generation scripts work

### Build Files (`Makefile`, `scripts/**/*.sh`)

* **Clean execution** - no errors or warnings
* **Cross-compatibility** - work on target platforms
* **Dependency management** - all required tools available

## Emergency Procedures

### If TCK Tests Fail

1. **STOP immediately** - do not commit or push
2. **Investigate root cause** - check what broke compliance
3. **Fix or revert** - restore TCK compliance
4. **Document exceptions** only with explicit approval

### If Basic Tests Fail

1. **Verify build environment** - clean rebuild required
2. **Check dependencies** - ensure all required libraries present
3. **Isolate failure** - run individual failing tests
4. **Fix or document** - resolve issues or create bug reports

### For Performance Regressions

1. **Profile changes** - identify performance bottlenecks
2. **Optimize or revert** - improve performance or rollback
3. **Update baselines** - if performance improvement is intentional
4. **Document trade-offs** - explain any performance compromises

## Quality Gates

### Pre-Commit Checks

* [ ] Basic tests pass: `make test`
* [ ] Clean build: `make rebuild`
* [ ] No compiler warnings: `make CFLAGS="-Wall -Wextra -Werror"`
* [ ] Memory safety: `make sanitize && make test_hardened`
* [ ] **TCK tests pass**: `make test_tck` (no regressions)
* [ ] **New feature TCK tests added** if applicable

### Pre-Merge Checks

* [ ] Full TCK suite passes: `make test_tck`
* [ ] Comprehensive test runner: `./tests/comprehensive_test_runner.sh`
* [ ] Verification script: `./scripts/verify_all.sh` (100% pass rate)
* [ ] Static analysis clean: `cppcheck`, other analyzers
* [ ] Performance regression check: `./scripts/perf_regression.sh`
* [ ] **Zero TCK regressions** - all existing tests pass
* [ ] **New feature TCK coverage** - comprehensive test scenarios added

### Release Checks

* [ ] All quality gates pass
* [ ] Documentation updated
* [ ] Examples verified working
* [ ] Cross-platform testing completed
* [ ] Security audit passed
* [ ] **Complete TCK compliance** - all 1615+ scenarios pass
* [ ] **No regressions** from previous release

**REMEMBER**: The TCK tests represent openCypher specification compliance. Breaking TCK tests breaks the contract with users expecting standard Cypher behavior. Always prioritize correctness over features when conflicts arise. **ALL NEW FEATURES MUST HAVE CORRESPONDING TCK TESTS** and **ZERO REGRESSIONS ARE ALLOWED**.
