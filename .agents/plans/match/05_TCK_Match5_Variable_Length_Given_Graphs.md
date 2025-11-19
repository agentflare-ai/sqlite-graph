# TCK Match5 - Match Variable Length Patterns Over Given Graphs

**Feature:** Match5 - Match variable length patterns over given graphs
**Total Scenarios:** 29
**Status:** 🔴 NOT STARTED (0/29 passing, 0%)
**Last Updated:** 2025-01-17

---

## Overview

Match5 is a comprehensive test suite for variable length patterns using specific graph structures.
It tests edge cases, complex scenarios, and real-world patterns.

This is NOT a new feature - it's extensive testing of Match4 functionality with carefully designed graphs.

**Implementation Status:**
- **Phase:** Specification Complete (awaits Match4 implementation)
- **Code Status:** Test stubs created, all marked IGNORE
- **Dependency:** Match4 must be fully implemented first

**Detailed Specification:** See `VARIABLE_LENGTH_IMPLEMENTATION_SPEC.md` for complete technical details.

---

## Scenario Categories

### Graph Structures Tested
- Linear chains
- Trees
- Cycles
- Diamond patterns
- Complex interconnected graphs

### Pattern Variations
- All variable length syntax forms
- Direction variations
- Type filtering with variable length
- Path constraints
- Edge cases (zero length, disconnected, etc.)

---

## Purpose

- **Comprehensive validation** of Match4 implementation
- **Edge case discovery** - finds bugs in path finding
- **Performance testing** - complex graphs stress the system
- **Real-world patterns** - common graph query patterns

---

## Dependencies

### Prerequisites
- ✅ Match4 - Variable length patterns MUST be working
- ✅ Match1-3 - Foundation features

### Relationship
Match5 doesn't add features - it validates Match4 thoroughly.

---

## Implementation Plan

### Approach
1. [ ] **PREREQUISITE:** Complete Match4 implementation (all phases)
2. [ ] Implement Match5 test scenarios (remove IGNORE markers)
3. [ ] Create helper functions for complex graph setup
4. [ ] Run Match5 tests to validate Match4 implementation
5. [ ] Fix bugs discovered through systematic debugging
6. [ ] Iterate until 90%+ passing

### Strategy
- Use Match5 as validation suite for Match4
- Group scenarios by pattern type for easier debugging
- Focus on edge cases that Match4 basic tests might miss
- Test graph structures: trees, chains, cycles, diamonds

### Match5 Test Implementation Tasks

**Binary Tree Tests (Scenarios 1-18):**
- [ ] Create `createMatch5BinaryTree()` helper function
- [ ] Implement scenarios 1-2: Unbounded patterns `[*]` and `[*..]`
- [ ] Implement scenarios 3-5: Single bounded `[*0]`, `[*1]`, `[*2]`
- [ ] Implement scenarios 6-7: Range bounded `[*0..2]`, `[*1..2]`
- [ ] Implement scenarios 8-10: Symmetrical bounds `[*0..0]`, `[*1..1]`, `[*2..2]`
- [ ] Implement scenarios 11-13: Empty intervals `[*2..1]`, `[*1..0]`, `[*..0]`
- [ ] Implement scenarios 14-15: Upper bounded `[*..1]`, `[*..2]`
- [ ] Implement scenarios 16-18: Lower bounded `[*0..]`, `[*1..]`, `[*2..]`

**Chained Pattern Tests (Scenarios 19-25):**
- [ ] Extend tree with additional depth (add E layer)
- [ ] Implement scenarios 19-20: Zero-length + fixed `[*0]->()-[]->()` patterns
- [ ] Implement scenarios 21-22: Single-length + fixed `[*1]->()-[]->()` patterns
- [ ] Implement scenarios 23-24: Multi-hop + fixed `[*2]->()-[]->()` patterns
- [ ] Implement scenario 25: Three-hop + fixed `()-[]->()-[*3]->()` pattern

**Direction Tests (Scenarios 26-27):**
- [ ] Modify graph to reverse directions
- [ ] Implement scenario 26: Backward then forward `<-[]--[*3]->()`
- [ ] Implement scenario 27: Forward then backward chain patterns

**Complex Pattern Tests (Scenarios 28-29):**
- [ ] Implement scenario 28: Multiple variable-length in one pattern
- [ ] Implement scenario 29: Variable-length with intermediate bindings

**Validation:**
- [ ] Verify all 29 tests execute without crashes
- [ ] Verify result counts match expected
- [ ] Verify result values match expected nodes
- [ ] Check for memory leaks (valgrind/sanitizers)

---

## Expected Pass Rate

### After Match4 Basic Implementation
- ~40% - Many edge cases will fail

### After Match4 Full Implementation
- ~70% - Some edge cases remain

### After Bug Fixes
- ~90%+ - Edge cases resolved

---

## Notes

- Most scenarios will pass if Match4 is solid
- Failures indicate Match4 bugs, not missing Match5 features
- Excellent test suite for validation
- Use to guide Match4 implementation quality

---

## Success Criteria

### Target
- [ ] 80%+ pass rate
- [ ] All major pattern types working
- [ ] Performance acceptable on complex graphs

---

**Priority:** MEDIUM - Validation of Match4
**Estimated Effort:** LOW - No new features, just debugging
**Dependencies:** Match4 must be implemented first
