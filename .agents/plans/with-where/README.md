# WITH-WHERE TCK Implementation Plans

This directory contains implementation plans for all WITH-WHERE TCK features.

## Features

1. **WithWhere1** - Filter single variable
   - Plan: [01_TCK_WithWhere1.md](01_TCK_WithWhere1.md)
   - Status: 🔴 NOT STARTED

2. **WithWhere2** - Filter multiple variables
   - Plan: [02_TCK_WithWhere2.md](02_TCK_WithWhere2.md)
   - Status: 🔴 NOT STARTED

3. **WithWhere3** - Equi-Joins on variables
   - Plan: [03_TCK_WithWhere3.md](03_TCK_WithWhere3.md)
   - Status: 🔴 NOT STARTED

4. **WithWhere4** - Non-Equi-Joins on variables
   - Plan: [04_TCK_WithWhere4.md](04_TCK_WithWhere4.md)
   - Status: 🔴 NOT STARTED

5. **WithWhere5** - Filter on predicate resulting in null
   - Plan: [05_TCK_WithWhere5.md](05_TCK_WithWhere5.md)
   - Status: 🔴 NOT STARTED

6. **WithWhere6** - Filter on aggregates
   - Plan: [06_TCK_WithWhere6.md](06_TCK_WithWhere6.md)
   - Status: 🔴 NOT STARTED

7. **WithWhere7** - Variable visibility under aliasing
   - Plan: [07_TCK_WithWhere7.md](07_TCK_WithWhere7.md)
   - Status: 🔴 NOT STARTED


## Progress Tracking

| Feature | Status | Scenarios | Pass Rate |
|---------|--------|-----------|-----------|
| WithWhere1 | 🔴 NOT STARTED | 0/? | 0% |
| WithWhere2 | 🔴 NOT STARTED | 0/? | 0% |
| WithWhere3 | 🔴 NOT STARTED | 0/? | 0% |
| WithWhere4 | 🔴 NOT STARTED | 0/? | 0% |
| WithWhere5 | 🔴 NOT STARTED | 0/? | 0% |
| WithWhere6 | 🔴 NOT STARTED | 0/? | 0% |
| WithWhere7 | 🔴 NOT STARTED | 0/? | 0% |

## Quick Commands

**Run all with-where tests:**
```bash
./build/tests/tck_test_with-where
```

**View specific plan:**
```bash
cat .agents/plans/with-where/NN_TCK_FeatureName.md
```
