# RETURN TCK Implementation Plans

This directory contains implementation plans for all RETURN TCK features.

## Features

1. **Return1** - Return single variable (correct return of values according to their type)
   - Plan: [01_TCK_Return1.md](01_TCK_Return1.md)
   - Status: 🔴 NOT STARTED

2. **Return2** - Return single expression (correctly projecting an expression)
   - Plan: [02_TCK_Return2.md](02_TCK_Return2.md)
   - Status: 🔴 NOT STARTED

3. **Return3** - Return multiple expressions (if column order correct)
   - Plan: [03_TCK_Return3.md](03_TCK_Return3.md)
   - Status: 🔴 NOT STARTED

4. **Return4** - Column renaming
   - Plan: [04_TCK_Return4.md](04_TCK_Return4.md)
   - Status: 🔴 NOT STARTED

5. **Return5** - Implicit grouping with distinct
   - Plan: [05_TCK_Return5.md](05_TCK_Return5.md)
   - Status: 🔴 NOT STARTED

6. **Return6** - Implicit grouping with aggregates
   - Plan: [06_TCK_Return6.md](06_TCK_Return6.md)
   - Status: 🔴 NOT STARTED

7. **Return7** - Return all variables
   - Plan: [07_TCK_Return7.md](07_TCK_Return7.md)
   - Status: 🔴 NOT STARTED

8. **Return8** - Return clause interoperation with other clauses
   - Plan: [08_TCK_Return8.md](08_TCK_Return8.md)
   - Status: 🔴 NOT STARTED


## Progress Tracking

| Feature | Status | Scenarios | Pass Rate |
|---------|--------|-----------|-----------|
| Return1 | 🔴 NOT STARTED | 0/? | 0% |
| Return2 | 🔴 NOT STARTED | 0/? | 0% |
| Return3 | 🔴 NOT STARTED | 0/? | 0% |
| Return4 | 🔴 NOT STARTED | 0/? | 0% |
| Return5 | 🔴 NOT STARTED | 0/? | 0% |
| Return6 | 🔴 NOT STARTED | 0/? | 0% |
| Return7 | 🔴 NOT STARTED | 0/? | 0% |
| Return8 | 🔴 NOT STARTED | 0/? | 0% |

## Quick Commands

**Run all return tests:**
```bash
./build/tests/tck_test_return
```

**View specific plan:**
```bash
cat .agents/plans/return/NN_TCK_FeatureName.md
```
