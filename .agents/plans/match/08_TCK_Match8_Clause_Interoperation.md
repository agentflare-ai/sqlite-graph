# TCK Match8 - Match Clause Interoperation

**Feature:** Match8 - Match clause interoperation with other clauses
**Total Scenarios:** 3
**Status:** 🔴 NOT STARTED (0/3 passing, 0%)

---

## Overview

Match8 tests MATCH in combination with other clauses:
- MATCH + WITH
- MATCH + WHERE
- MATCH + UNWIND
- MATCH + ORDER BY
- MATCH + LIMIT/SKIP
- Multiple MATCH clauses

Only 3 scenarios - focused integration tests.

---

## Purpose

- Validate MATCH works with clause compositions
- Test variable scoping across clauses
- Ensure proper data flow between clauses

---

## Dependencies

### Prerequisites
- ✅ Match1-2 - Basic MATCH
- ⚠️ WITH clause
- ⚠️ WHERE clause
- ⚠️ UNWIND clause
- ⚠️ ORDER BY, LIMIT, SKIP

### Nature
Integration testing - validates interaction between features.

---

## Implementation Plan

### Approach
1. [ ] Implement dependent clauses (WITH, WHERE, etc.)
2. [ ] Test MATCH with each clause
3. [ ] Fix integration issues

### Strategy
- These tests will likely pass once clauses are implemented
- More about validation than new features
- Good smoke tests

---

## Expected Pass Rate

### After Basic Clauses Implemented
- ~100% - If all clauses work individually, they should work together

### Before All Clauses
- ~0-30% - Depends on what's implemented

---

## Notes

- Small feature file (3 scenarios)
- High value for validation
- Catches integration bugs
- Run regularly as regression tests

---

## Success Criteria

### Target
- [ ] 100% pass rate
- [ ] All clause combinations working

---

**Priority:** HIGH - Integration validation
**Estimated Effort:** LOW - Mostly depends on other features
**Dependencies:** WITH, WHERE, UNWIND, ORDER BY, LIMIT/SKIP
