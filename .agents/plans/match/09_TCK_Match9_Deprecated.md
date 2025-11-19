# TCK Match9 - Match Deprecated Scenarios

**Feature:** Match9 - Match deprecated scenarios
**Total Scenarios:** 9
**Status:** 🔴 DEFERRED (Can skip)

---

## Overview

Match9 contains deprecated features and syntax that may not be in current openCypher spec:
- Old syntax forms
- Deprecated patterns
- Backward compatibility tests
- Features removed from spec

---

## Recommendation

**DEFER THIS FEATURE**

Reasons:
1. Deprecated features - not in current spec
2. Low priority for new implementation
3. May conflict with modern Cypher
4. Better to focus on current spec

---

## Strategy

### Phase 1-4 (Now)
- Skip Match9 entirely
- Focus on Match1-8

### Future (If Needed)
- Evaluate if any scenarios are actually needed
- Check if they test still-valid features
- Implement only if required

---

## Notes

- 9 scenarios total
- All marked as deprecated
- Can achieve high TCK coverage without this
- Revisit only if specifically requested

---

## Success Criteria

### Current
- [ ] Can be safely ignored

### Future (Optional)
- [ ] Evaluate scenarios
- [ ] Implement if needed for compatibility

---

**Priority:** VERY LOW - Can defer indefinitely
**Estimated Effort:** UNKNOWN - Depends on actual content
**Recommendation:** SKIP
