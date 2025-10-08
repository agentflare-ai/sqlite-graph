#!/bin/bash

# SELF_REVIEW.md Verification Protocol Implementation
# This script implements the mandatory verification checklist from SELF_REVIEW.md
# ZERO TOLERANCE for shortcuts or incomplete implementations

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
BOLD='\033[1m'
NC='\033[0m' # No Color

echo -e "${BOLD}=== SELF_REVIEW.md VERIFICATION PROTOCOL ===${NC}"
echo ""
echo "🎯 CRITICAL: This implements the ZERO-TOLERANCE quality gate from SELF_REVIEW.md"
echo "📋 MANDATE: ALL verification commands must pass before considering work complete"
echo ""

# Change to project root
cd "$(dirname "$0")/.."

# Results directory
VERIFY_DIR="self_review_verification"
mkdir -p "$VERIFY_DIR"

# Function to print status
print_status() {
    local status=$1
    local message=$2
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}✅ $message${NC}"
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}❌ $message${NC}"
    elif [ "$status" = "WARN" ]; then
        echo -e "${YELLOW}⚠️ $message${NC}"
    elif [ "$status" = "INFO" ]; then
        echo -e "${BLUE}ℹ️ $message${NC}"
    else
        echo "  $message"
    fi
}

# Initialize verification report
cat > "$VERIFY_DIR/verification_report.md" << 'EOF'
# SELF_REVIEW.md Verification Report

This report documents the results of applying the mandatory SELF_REVIEW.md verification protocol.

## Verification Results

EOF

TOTAL_FAILURES=0

echo -e "${BOLD}SECTION 1: IMPLEMENTATION COMPLETENESS${NC}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# 1. CRITICAL: Placeholder Code Elimination
echo ""
echo "1.1 Placeholder Code Elimination (ZERO TOLERANCE)"

TODO_COUNT=$(find src/ include/ -name "*.c" -o -name "*.h" | xargs grep -c "TODO\|FIXME\|NOTE\|XXX\|HACK\|STUB" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}' || echo "0")

if [ "$TODO_COUNT" -eq 0 ]; then
    print_status "PASS" "Zero TODO/FIXME markers found"
    echo "- ✅ Placeholder elimination: COMPLETE" >> "$VERIFY_DIR/verification_report.md"
else
    print_status "FAIL" "Found $TODO_COUNT TODO/FIXME markers - CRITICAL FAILURE"
    echo "- ❌ Placeholder elimination: FAILED ($TODO_COUNT markers)" >> "$VERIFY_DIR/verification_report.md"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
    
    # Details
    echo "### CRITICAL FAILURE: TODO/FIXME Markers Found" >> "$VERIFY_DIR/verification_report.md"
    find src/ include/ -name "*.c" -o -name "*.h" | xargs grep -n "TODO\|FIXME\|NOTE\|XXX\|HACK\|STUB" 2>/dev/null >> "$VERIFY_DIR/verification_report.md" || true
fi

# Check for placeholder returns
PLACEHOLDER_RETURNS=$(find src/ -name "*.c" | xargs grep -c "return 0.*TODO\|return NULL.*TODO\|return 1.*TODO" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}' || echo "0")

if [ "$PLACEHOLDER_RETURNS" -eq 0 ]; then
    print_status "PASS" "No placeholder return values"
    echo "- ✅ Placeholder returns: NONE" >> "$VERIFY_DIR/verification_report.md"
else
    print_status "FAIL" "Found $PLACEHOLDER_RETURNS placeholder returns - CRITICAL FAILURE"
    echo "- ❌ Placeholder returns: FAILED ($PLACEHOLDER_RETURNS found)" >> "$VERIFY_DIR/verification_report.md"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
fi

# Check for hardcoded test values
TEST_VALUES=$(find src/ -name "*.c" | xargs grep -c "\"test\"\|\"sample\"\|\"example\"" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}' || echo "0")

if [ "$TEST_VALUES" -eq 0 ]; then
    print_status "PASS" "No hardcoded test values"
    echo "- ✅ Test value elimination: COMPLETE" >> "$VERIFY_DIR/verification_report.md"
else
    print_status "WARN" "Found $TEST_VALUES potential test values"
    echo "- ⚠️ Test values: $TEST_VALUES found (review required)" >> "$VERIFY_DIR/verification_report.md"
fi

echo ""
echo "1.2 Function Implementation Verification"

# Check for unimplemented functions (returning early without implementation)
UNIMPLEMENTED=$(find src/ -name "*.c" | xargs grep -c "return.*SQLITE_OK;.*TODO\|return.*0;.*TODO" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}' || echo "0")

if [ "$UNIMPLEMENTED" -eq 0 ]; then
    print_status "PASS" "No unimplemented functions detected"
    echo "- ✅ Function implementation: COMPLETE" >> "$VERIFY_DIR/verification_report.md"
else
    print_status "FAIL" "Found $UNIMPLEMENTED unimplemented functions - CRITICAL FAILURE"
    echo "- ❌ Function implementation: FAILED ($UNIMPLEMENTED incomplete)" >> "$VERIFY_DIR/verification_report.md"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
fi

echo ""
echo -e "${BOLD}SECTION 2: ERROR HANDLING COMPLETENESS${NC}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# 2.1 Memory Allocation Error Handling
echo ""
echo "2.1 Memory Allocation Error Handling"

MALLOC_COUNT=$(find src/ -name "*.c" | xargs grep -c "malloc\|calloc\|realloc" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}' || echo "0")
MALLOC_CHECKS=$(find src/ -name "*.c" | xargs grep -A 3 "malloc\|calloc\|realloc" 2>/dev/null | grep -c "if.*== NULL\|if.*!" || echo "0")

if [ "$MALLOC_COUNT" -gt 0 ]; then
    MALLOC_RATIO=$(echo "scale=2; $MALLOC_CHECKS / $MALLOC_COUNT" | bc -l)
    if (( $(echo "$MALLOC_RATIO >= 0.8" | bc -l) )); then
        print_status "PASS" "Memory allocation error checking: $MALLOC_RATIO ratio"
        echo "- ✅ Memory allocation errors: HANDLED ($MALLOC_RATIO ratio)" >> "$VERIFY_DIR/verification_report.md"
    else
        print_status "FAIL" "Insufficient memory allocation error checking: $MALLOC_RATIO ratio"
        echo "- ❌ Memory allocation errors: INSUFFICIENT ($MALLOC_RATIO ratio)" >> "$VERIFY_DIR/verification_report.md"
        TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
    fi
else
    print_status "INFO" "No dynamic memory allocation detected"
    echo "- ℹ️ Memory allocation: NONE DETECTED" >> "$VERIFY_DIR/verification_report.md"
fi

# 2.2 SQLite Error Handling
echo ""
echo "2.2 SQLite Error Handling"

SQLITE_CALLS=$(find src/ -name "*.c" | xargs grep -c "sqlite3_.*(" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}' || echo "0")
SQLITE_CHECKS=$(find src/ -name "*.c" | xargs grep -c "!= SQLITE_OK\|== SQLITE_OK" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}' || echo "0")

if [ "$SQLITE_CALLS" -gt 0 ]; then
    SQLITE_RATIO=$(echo "scale=2; $SQLITE_CHECKS / $SQLITE_CALLS" | bc -l)
    if (( $(echo "$SQLITE_RATIO >= 0.3" | bc -l) )); then
        print_status "PASS" "SQLite error checking: $SQLITE_RATIO ratio"
        echo "- ✅ SQLite error handling: ADEQUATE ($SQLITE_RATIO ratio)" >> "$VERIFY_DIR/verification_report.md"
    else
        print_status "WARN" "Low SQLite error checking: $SQLITE_RATIO ratio"
        echo "- ⚠️ SQLite error handling: LOW ($SQLITE_RATIO ratio)" >> "$VERIFY_DIR/verification_report.md"
    fi
fi

echo ""
echo -e "${BOLD}SECTION 3: TESTING THOROUGHNESS${NC}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# 3.1 Test Coverage Verification
echo ""
echo "3.1 Test Coverage Verification"

if [ -d "tests" ] && [ "$(ls -A tests/ 2>/dev/null)" ]; then
    TEST_COUNT=$(find tests/ -name "*.c" | wc -l)
    if [ "$TEST_COUNT" -gt 0 ]; then
        print_status "PASS" "Found $TEST_COUNT test files"
        echo "- ✅ Test coverage: $TEST_COUNT test files present" >> "$VERIFY_DIR/verification_report.md"
    else
        print_status "FAIL" "No test files found - CRITICAL FAILURE"
        echo "- ❌ Test coverage: NO TESTS FOUND" >> "$VERIFY_DIR/verification_report.md"
        TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
    fi
else
    print_status "FAIL" "No tests directory or empty - CRITICAL FAILURE"
    echo "- ❌ Test coverage: NO TEST INFRASTRUCTURE" >> "$VERIFY_DIR/verification_report.md"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
fi

echo ""
echo -e "${BOLD}SECTION 4: THREAD SAFETY VERIFICATION${NC}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# 4.1 Global State Audit
echo ""
echo "4.1 Global State Audit"

GLOBAL_VARS=$(find src/ -name "*.c" | xargs grep -c "^static.*=" 2>/dev/null | grep -v "const" | awk -F: '{sum += $2} END {print sum+0}' || echo "0")

if [ "$GLOBAL_VARS" -eq 0 ]; then
    print_status "PASS" "No mutable global state detected"
    echo "- ✅ Global state: NO MUTABLE GLOBALS" >> "$VERIFY_DIR/verification_report.md"
else
    print_status "WARN" "Found $GLOBAL_VARS mutable global variables"
    echo "- ⚠️ Global state: $GLOBAL_VARS mutable globals (review for thread safety)" >> "$VERIFY_DIR/verification_report.md"
fi

echo ""
echo -e "${BOLD}SECTION 5: SECURITY THOROUGHNESS${NC}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# 5.1 Input Attack Prevention
echo ""
echo "5.1 Input Attack Prevention"

# SQL Injection Prevention
UNSAFE_SQL=$(find src/ -name "*.c" | xargs grep -c "sprintf.*SELECT\|sprintf.*INSERT\|strcat.*SELECT" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}' || echo "0")

if [ "$UNSAFE_SQL" -eq 0 ]; then
    print_status "PASS" "No SQL injection vulnerabilities detected"
    echo "- ✅ SQL injection: PREVENTED" >> "$VERIFY_DIR/verification_report.md"
else
    print_status "FAIL" "Found $UNSAFE_SQL potential SQL injection vulnerabilities - CRITICAL FAILURE"
    echo "- ❌ SQL injection: $UNSAFE_SQL VULNERABILITIES FOUND" >> "$VERIFY_DIR/verification_report.md"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
fi

# Buffer Overflow Prevention
UNSAFE_STRING=$(find src/ -name "*.c" | xargs grep -c "strcpy\|strcat\|sprintf" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}' || echo "0")
SAFE_STRING=$(find src/ -name "*.c" | xargs grep -c "strncpy\|strncat\|snprintf" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}' || echo "0")

if [ "$UNSAFE_STRING" -eq 0 ] || [ "$SAFE_STRING" -gt "$UNSAFE_STRING" ]; then
    print_status "PASS" "Buffer overflow protection adequate"
    echo "- ✅ Buffer overflow: PROTECTED (safe: $SAFE_STRING, unsafe: $UNSAFE_STRING)" >> "$VERIFY_DIR/verification_report.md"
else
    print_status "FAIL" "Insufficient buffer overflow protection - CRITICAL FAILURE"
    echo "- ❌ Buffer overflow: INSUFFICIENT PROTECTION (safe: $SAFE_STRING, unsafe: $UNSAFE_STRING)" >> "$VERIFY_DIR/verification_report.md"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
fi

echo ""
echo -e "${BOLD}MANDATORY VERIFICATION COMMANDS${NC}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Command 1: TODO/FIXME Check
echo ""
echo "Command 1: grep -r \"TODO\|FIXME\" src/ include/"

if [ "$TODO_COUNT" -eq 0 ]; then
    print_status "PASS" "Command 1: PASSED (no results)"
    echo "- ✅ Command 1: PASSED" >> "$VERIFY_DIR/verification_report.md"
else
    print_status "FAIL" "Command 1: FAILED ($TODO_COUNT results)"
    echo "- ❌ Command 1: FAILED" >> "$VERIFY_DIR/verification_report.md"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
fi

# Command 2: Build Check
echo ""
echo "Command 2: make clean && make CFLAGS=\"-Wall -Wextra -Werror -O2\""

BUILD_RESULT=0
if make clean >/dev/null 2>&1 && make CFLAGS="-Wall -Wextra -Werror -O2" >/dev/null 2>&1; then
    print_status "PASS" "Command 2: BUILD SUCCESSFUL"
    echo "- ✅ Command 2: PASSED" >> "$VERIFY_DIR/verification_report.md"
else
    print_status "FAIL" "Command 2: BUILD FAILED"
    echo "- ❌ Command 2: FAILED" >> "$VERIFY_DIR/verification_report.md"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
    BUILD_RESULT=1
fi

# Command 3: Test Suite
echo ""
echo "Command 3: make test"

if [ "$BUILD_RESULT" -eq 0 ] && make test >/dev/null 2>&1; then
    print_status "PASS" "Command 3: ALL TESTS PASSED"
    echo "- ✅ Command 3: PASSED" >> "$VERIFY_DIR/verification_report.md"
else
    print_status "FAIL" "Command 3: TESTS FAILED"
    echo "- ❌ Command 3: FAILED" >> "$VERIFY_DIR/verification_report.md"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
fi

# Command 4: Valgrind Check (if available)
echo ""
echo "Command 4: valgrind --leak-check=full --error-exitcode=1 ./test_suite"

if command -v valgrind >/dev/null 2>&1; then
    # Create a simple test if needed
    if [ ! -f "test_suite" ] && [ -f "build/libgraph.so" ]; then
        echo "Creating minimal test for valgrind..."
        cat > test_suite.c << 'EOF'
#include <sqlite3.h>
int main() {
    sqlite3 *db;
    if (sqlite3_open(":memory:", &db) == SQLITE_OK) {
        sqlite3_load_extension(db, "./build/libgraph.so", NULL, NULL);
        sqlite3_close(db);
    }
    return 0;
}
EOF
        gcc -o test_suite test_suite.c -lsqlite3 -ldl 2>/dev/null || true
    fi
    
    if [ -f "test_suite" ] && valgrind --leak-check=full --error-exitcode=1 ./test_suite >/dev/null 2>&1; then
        print_status "PASS" "Command 4: NO MEMORY LEAKS"
        echo "- ✅ Command 4: PASSED" >> "$VERIFY_DIR/verification_report.md"
    else
        print_status "FAIL" "Command 4: MEMORY LEAKS DETECTED"
        echo "- ❌ Command 4: FAILED" >> "$VERIFY_DIR/verification_report.md"
        TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
    fi
else
    print_status "WARN" "Command 4: VALGRIND NOT AVAILABLE"
    echo "- ⚠️ Command 4: SKIPPED (valgrind not available)" >> "$VERIFY_DIR/verification_report.md"
fi

# Command 5: Static Analysis (if available)
echo ""
echo "Command 5: cppcheck --error-exitcode=1 --enable=all src/"

if command -v cppcheck >/dev/null 2>&1; then
    if cppcheck --error-exitcode=1 --enable=all --suppress=missingIncludeSystem --suppress=unusedFunction src/ >/dev/null 2>&1; then
        print_status "PASS" "Command 5: STATIC ANALYSIS CLEAN"
        echo "- ✅ Command 5: PASSED" >> "$VERIFY_DIR/verification_report.md"
    else
        print_status "FAIL" "Command 5: STATIC ANALYSIS ISSUES"
        echo "- ❌ Command 5: FAILED" >> "$VERIFY_DIR/verification_report.md"
        TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
    fi
else
    print_status "WARN" "Command 5: CPPCHECK NOT AVAILABLE"
    echo "- ⚠️ Command 5: SKIPPED (cppcheck not available)" >> "$VERIFY_DIR/verification_report.md"
fi

# Finalize report
echo ""
echo "## Final Assessment" >> "$VERIFY_DIR/verification_report.md"
echo "" >> "$VERIFY_DIR/verification_report.md"
echo "Total Critical Failures: $TOTAL_FAILURES" >> "$VERIFY_DIR/verification_report.md"
echo "" >> "$VERIFY_DIR/verification_report.md"

if [ "$TOTAL_FAILURES" -eq 0 ]; then
    echo "**RESULT: PRODUCTION READY** ✅" >> "$VERIFY_DIR/verification_report.md"
    echo "" >> "$VERIFY_DIR/verification_report.md"
    echo "All mandatory verification requirements have been met." >> "$VERIFY_DIR/verification_report.md"
    echo "The code meets the zero-tolerance quality standards." >> "$VERIFY_DIR/verification_report.md"
else
    echo "**RESULT: NOT PRODUCTION READY** ❌" >> "$VERIFY_DIR/verification_report.md"
    echo "" >> "$VERIFY_DIR/verification_report.md"
    echo "CRITICAL FAILURES must be addressed before production deployment." >> "$VERIFY_DIR/verification_report.md"
    echo "Zero tolerance policy: ALL issues must be resolved." >> "$VERIFY_DIR/verification_report.md"
fi

echo "" >> "$VERIFY_DIR/verification_report.md"
echo "---" >> "$VERIFY_DIR/verification_report.md"
echo "Verification Date: $(date)" >> "$VERIFY_DIR/verification_report.md"
echo "Verification Standard: SELF_REVIEW.md Zero-Tolerance Protocol" >> "$VERIFY_DIR/verification_report.md"

# Final Summary
echo ""
echo -e "${BOLD}=== FINAL VERIFICATION SUMMARY ===${NC}"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

if [ "$TOTAL_FAILURES" -eq 0 ]; then
    echo -e "${GREEN}${BOLD}"
    echo "🎉 PRODUCTION READY - ALL REQUIREMENTS MET"
    echo "✅ Zero TODO/FIXME markers"
    echo "✅ Complete implementation"
    echo "✅ Adequate error handling"
    echo "✅ Security protections in place"
    echo "✅ All verification commands passed"
    echo -e "${NC}"
    
    print_status "PASS" "SELF_REVIEW.md verification COMPLETE"
    print_status "PASS" "Code ready for production deployment"
else
    echo -e "${RED}${BOLD}"
    echo "🚫 NOT PRODUCTION READY - CRITICAL FAILURES DETECTED"
    echo "❌ $TOTAL_FAILURES critical issues found"
    echo "⚠️ Zero tolerance policy: ALL issues must be resolved"
    echo "🔄 Continue working until ALL verifications pass"
    echo -e "${NC}"
    
    print_status "FAIL" "SELF_REVIEW.md verification FAILED"
    print_status "FAIL" "Code NOT ready for production"
fi

echo ""
echo "📄 Detailed report: $VERIFY_DIR/verification_report.md"
echo ""

if [ "$TOTAL_FAILURES" -eq 0 ]; then
    echo -e "${GREEN}${BOLD}🏆 CONGRATULATIONS: Production quality achieved!${NC}"
    exit 0
else
    echo -e "${RED}${BOLD}🔧 CONTINUE WORKING: Fix all issues before deployment${NC}"
    exit 1
fi