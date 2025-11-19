# MATCH-WHERE TCK Implementation Plans

This directory contains implementation plans for all MATCH-WHERE TCK features.

## Features

1. **MatchWhere1** - Filter single variable
   - Plan: [01_TCK_MatchWhere1.md](01_TCK_MatchWhere1.md)
   - Status: 🔴 NOT STARTED

2. **MatchWhere2** - Filter multiple variables
   - Plan: [02_TCK_MatchWhere2.md](02_TCK_MatchWhere2.md)
   - Status: 🔴 NOT STARTED

3. **MatchWhere3** - Equi-Joins on variables
   - Plan: [03_TCK_MatchWhere3.md](03_TCK_MatchWhere3.md)
   - Status: 🔴 NOT STARTED

4. **MatchWhere4** - Non-Equi-Joins on variables
   - Plan: [04_TCK_MatchWhere4.md](04_TCK_MatchWhere4.md)
   - Status: 🔴 NOT STARTED

5. **MatchWhere5** - Filter on predicate resulting in null
   - Plan: [05_TCK_MatchWhere5.md](05_TCK_MatchWhere5.md)
   - Status: 🔴 NOT STARTED

6. **MatchWhere6** - Filter optional matches
   - Plan: [06_TCK_MatchWhere6.md](06_TCK_MatchWhere6.md)
   - Status: 🔴 NOT STARTED


## Progress Tracking

| Feature | Status | Scenarios | Pass Rate |
|---------|--------|-----------|-----------|
| MatchWhere1 | 🔴 NOT STARTED | 0/? | 0% |
| MatchWhere2 | 🔴 NOT STARTED | 0/? | 0% |
| MatchWhere3 | 🔴 NOT STARTED | 0/? | 0% |
| MatchWhere4 | 🔴 NOT STARTED | 0/? | 0% |
| MatchWhere5 | 🔴 NOT STARTED | 0/? | 0% |
| MatchWhere6 | 🔴 NOT STARTED | 0/? | 0% |

## Quick Commands

**Run all match-where tests:**
```bash
./build/tests/tck_test_match-where
```

**View specific plan:**
```bash
cat .agents/plans/match-where/NN_TCK_FeatureName.md
```
