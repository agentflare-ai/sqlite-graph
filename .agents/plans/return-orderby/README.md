# RETURN-ORDERBY TCK Implementation Plans

This directory contains implementation plans for all RETURN-ORDERBY TCK features.

## Features

1. **ReturnOrderBy1** - Order by a single variable (correct order of values according to their type)
   - Plan: [01_TCK_ReturnOrderBy1.md](01_TCK_ReturnOrderBy1.md)
   - Status: 🔴 NOT STARTED

2. **ReturnOrderBy2** - Order by a single expression (order of projection)
   - Plan: [02_TCK_ReturnOrderBy2.md](02_TCK_ReturnOrderBy2.md)
   - Status: 🔴 NOT STARTED

3. **ReturnOrderBy3** - Order by multiple expressions (order obey priority of expressions)
   - Plan: [03_TCK_ReturnOrderBy3.md](03_TCK_ReturnOrderBy3.md)
   - Status: 🔴 NOT STARTED

4. **ReturnOrderBy4** - Order by in combination with projection
   - Plan: [04_TCK_ReturnOrderBy4.md](04_TCK_ReturnOrderBy4.md)
   - Status: 🔴 NOT STARTED

5. **ReturnOrderBy5** - Order by in combination with column renaming
   - Plan: [05_TCK_ReturnOrderBy5.md](05_TCK_ReturnOrderBy5.md)
   - Status: 🔴 NOT STARTED

6. **ReturnOrderBy6** - Aggregation expressions in order by
   - Plan: [06_TCK_ReturnOrderBy6.md](06_TCK_ReturnOrderBy6.md)
   - Status: 🔴 NOT STARTED


## Progress Tracking

| Feature | Status | Scenarios | Pass Rate |
|---------|--------|-----------|-----------|
| ReturnOrderBy1 | 🔴 NOT STARTED | 0/? | 0% |
| ReturnOrderBy2 | 🔴 NOT STARTED | 0/? | 0% |
| ReturnOrderBy3 | 🔴 NOT STARTED | 0/? | 0% |
| ReturnOrderBy4 | 🔴 NOT STARTED | 0/? | 0% |
| ReturnOrderBy5 | 🔴 NOT STARTED | 0/? | 0% |
| ReturnOrderBy6 | 🔴 NOT STARTED | 0/? | 0% |

## Quick Commands

**Run all return-orderby tests:**
```bash
./build/tests/tck_test_return-orderby
```

**View specific plan:**
```bash
cat .agents/plans/return-orderby/NN_TCK_FeatureName.md
```
