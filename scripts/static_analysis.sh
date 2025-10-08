#!/bin/bash

# Static Analysis Script for SQLite Graph Extension
# Runs various static analysis tools to detect code quality issues

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "=== SQLite Graph Extension Static Analysis ==="
echo ""

# Change to project root
cd "$(dirname "$0")/.."

# Source directories
SRC_DIRS="src include"
TEST_DIRS="tests benchmarks"

# Results directory
RESULTS_DIR="analysis_results"
mkdir -p "$RESULTS_DIR"

# Function to print status
print_status() {
    local status=$1
    local message=$2
    if [ "$status" = "PASS" ]; then
        echo -e "${GREEN}✓ $message${NC}"
    elif [ "$status" = "FAIL" ]; then
        echo -e "${RED}✗ $message${NC}"
    elif [ "$status" = "WARN" ]; then
        echo -e "${YELLOW}⚠ $message${NC}"
    else
        echo "  $message"
    fi
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

echo "Checking available tools..."

# Check for cppcheck
if command_exists cppcheck; then
    print_status "INFO" "cppcheck found"
    HAVE_CPPCHECK=1
else
    print_status "WARN" "cppcheck not found - install with: sudo apt-get install cppcheck"
    HAVE_CPPCHECK=0
fi

# Check for clang-tidy
if command_exists clang-tidy; then
    print_status "INFO" "clang-tidy found"
    HAVE_CLANG_TIDY=1
else
    print_status "WARN" "clang-tidy not found - install with: sudo apt-get install clang-tidy"
    HAVE_CLANG_TIDY=0
fi

# Check for scan-build
if command_exists scan-build; then
    print_status "INFO" "scan-build found"
    HAVE_SCAN_BUILD=1
else
    print_status "WARN" "scan-build not found - install with: sudo apt-get install clang-tools"
    HAVE_SCAN_BUILD=0
fi

# Check for splint
if command_exists splint; then
    print_status "INFO" "splint found"
    HAVE_SPLINT=1
else
    print_status "WARN" "splint not found - install with: sudo apt-get install splint"
    HAVE_SPLINT=0
fi

echo ""

# Manual code analysis based on SELF_REVIEW.md requirements
echo "=== Manual Code Quality Analysis ==="

# 1. Check for TODO/FIXME markers
echo "1. Checking for placeholder code..."
TODO_COUNT=$(find $SRC_DIRS -name "*.c" -o -name "*.h" | xargs grep -c "TODO\|FIXME\|NOTE\|XXX\|HACK\|STUB" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')

if [ "$TODO_COUNT" -eq 0 ]; then
    print_status "PASS" "No TODO/FIXME markers found"
else
    print_status "FAIL" "Found $TODO_COUNT TODO/FIXME markers"
    find $SRC_DIRS -name "*.c" -o -name "*.h" | xargs grep -n "TODO\|FIXME\|NOTE\|XXX\|HACK\|STUB" > "$RESULTS_DIR/todo_markers.txt" 2>/dev/null || true
fi

# 2. Check for placeholder returns
echo "2. Checking for placeholder return values..."
PLACEHOLDER_RETURNS=$(find $SRC_DIRS -name "*.c" | xargs grep -c "return 0.*TODO\|return NULL.*TODO\|return 1.*TODO" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')

if [ "$PLACEHOLDER_RETURNS" -eq 0 ]; then
    print_status "PASS" "No placeholder return values found"
else
    print_status "FAIL" "Found $PLACEHOLDER_RETURNS placeholder return values"
fi

# 3. Check for hardcoded test values
echo "3. Checking for hardcoded test values..."
TEST_VALUES=$(find $SRC_DIRS -name "*.c" | xargs grep -c "\"test\"\|\"sample\"\|\"example\"" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')

if [ "$TEST_VALUES" -eq 0 ]; then
    print_status "PASS" "No hardcoded test values found"
else
    print_status "WARN" "Found $TEST_VALUES potential hardcoded test values"
fi

# 4. Check for malloc without free patterns
echo "4. Checking memory allocation patterns..."
MALLOC_COUNT=$(find $SRC_DIRS -name "*.c" | xargs grep -c "malloc\|calloc\|realloc" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')
FREE_COUNT=$(find $SRC_DIRS -name "*.c" | xargs grep -c "free(" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')

echo "  malloc/calloc/realloc calls: $MALLOC_COUNT"
echo "  free() calls: $FREE_COUNT"

if [ "$MALLOC_COUNT" -gt 0 ] && [ "$FREE_COUNT" -gt 0 ]; then
    RATIO=$(echo "scale=2; $FREE_COUNT / $MALLOC_COUNT" | bc -l)
    if (( $(echo "$RATIO >= 0.8" | bc -l) )); then
        print_status "PASS" "Memory allocation/deallocation ratio acceptable ($RATIO)"
    else
        print_status "WARN" "Low free/malloc ratio ($RATIO) - check for memory leaks"
    fi
else
    print_status "INFO" "No dynamic memory allocation found"
fi

# 5. Check for proper error handling
echo "5. Checking error handling patterns..."
ERROR_CHECKS=$(find $SRC_DIRS -name "*.c" | xargs grep -c "if.*== NULL\|if.*!= SQLITE_OK\|if.*< 0" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')
FUNCTION_COUNT=$(find $SRC_DIRS -name "*.c" | xargs grep -c "^[a-zA-Z_][a-zA-Z0-9_]*.*(" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')

if [ "$FUNCTION_COUNT" -gt 0 ]; then
    ERROR_RATIO=$(echo "scale=2; $ERROR_CHECKS / $FUNCTION_COUNT" | bc -l)
    if (( $(echo "$ERROR_RATIO >= 0.3" | bc -l) )); then
        print_status "PASS" "Good error handling coverage ($ERROR_RATIO)"
    else
        print_status "WARN" "Low error handling coverage ($ERROR_RATIO)"
    fi
fi

# 6. Check for SQL injection prevention
echo "6. Checking for SQL injection prevention..."
SPRINTF_SQL=$(find $SRC_DIRS -name "*.c" | xargs grep -c "sprintf.*SELECT\|sprintf.*INSERT\|sprintf.*UPDATE\|sprintf.*DELETE" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')
BIND_CALLS=$(find $SRC_DIRS -name "*.c" | xargs grep -c "sqlite3_bind_" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')

if [ "$SPRINTF_SQL" -eq 0 ]; then
    print_status "PASS" "No unsafe SQL string formatting found"
else
    print_status "FAIL" "Found $SPRINTF_SQL potential SQL injection vulnerabilities"
fi

if [ "$BIND_CALLS" -gt 0 ]; then
    print_status "PASS" "Using parameterized queries ($BIND_CALLS bind calls)"
else
    print_status "WARN" "No parameterized query usage found"
fi

# 7. Check for buffer overflow protection
echo "7. Checking for buffer overflow protection..."
STRCPY_CALLS=$(find $SRC_DIRS -name "*.c" | xargs grep -c "strcpy\|strcat\|sprintf" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')
SAFE_CALLS=$(find $SRC_DIRS -name "*.c" | xargs grep -c "strncpy\|strncat\|snprintf" 2>/dev/null | awk -F: '{sum += $2} END {print sum+0}')

if [ "$STRCPY_CALLS" -eq 0 ]; then
    print_status "PASS" "No unsafe string functions found"
elif [ "$SAFE_CALLS" -gt "$STRCPY_CALLS" ]; then
    print_status "PASS" "More safe string functions than unsafe ones"
else
    print_status "FAIL" "Found $STRCPY_CALLS unsafe string function calls"
fi

echo ""

# Run available static analysis tools
if [ "$HAVE_CPPCHECK" -eq 1 ]; then
    echo "=== Running cppcheck ==="
    cppcheck --enable=all --error-exitcode=1 --xml --xml-version=2 \
        --suppress=missingIncludeSystem \
        --suppress=unusedFunction \
        $SRC_DIRS 2> "$RESULTS_DIR/cppcheck.xml" || {
        print_status "FAIL" "cppcheck found issues"
        echo "See $RESULTS_DIR/cppcheck.xml for details"
    }
    
    if [ ! -s "$RESULTS_DIR/cppcheck.xml" ]; then
        print_status "PASS" "cppcheck found no issues"
    fi
    echo ""
fi

if [ "$HAVE_CLANG_TIDY" -eq 1 ]; then
    echo "=== Running clang-tidy ==="
    # Create compile_commands.json if it doesn't exist
    if [ ! -f "compile_commands.json" ]; then
        echo "Creating compile_commands.json..."
        bear -- make clean all 2>/dev/null || {
            print_status "WARN" "Could not create compile_commands.json - skipping clang-tidy"
            HAVE_CLANG_TIDY=0
        }
    fi
    
    if [ "$HAVE_CLANG_TIDY" -eq 1 ]; then
        find $SRC_DIRS -name "*.c" | while read -r file; do
            clang-tidy "$file" -- -I./include -I./src 2>> "$RESULTS_DIR/clang-tidy.log" || true
        done
        print_status "INFO" "clang-tidy analysis complete - see $RESULTS_DIR/clang-tidy.log"
    fi
    echo ""
fi

if [ "$HAVE_SCAN_BUILD" -eq 1 ]; then
    echo "=== Running scan-build ==="
    scan-build -o "$RESULTS_DIR/scan-build" make clean all 2>&1 | tee "$RESULTS_DIR/scan-build.log"
    
    if [ -d "$RESULTS_DIR/scan-build" ] && [ "$(ls -A $RESULTS_DIR/scan-build)" ]; then
        print_status "WARN" "scan-build found potential issues"
        echo "See $RESULTS_DIR/scan-build/ for details"
    else
        print_status "PASS" "scan-build found no issues"
    fi
    echo ""
fi

# GCC static analysis
echo "=== Running GCC Static Analysis ==="
if command_exists gcc; then
    echo "Compiling with GCC static analysis flags..."
    make clean >/dev/null 2>&1 || true
    
    GCC_FLAGS="-Wall -Wextra -Wformat=2 -Wformat-security -Wnull-dereference -Wstack-protector -Wtrampolines -Walloca -Wvla -Warray-bounds=2 -Wimplicit-fallthrough=3 -Wtraditional-conversion -Wpedantic -Wlogical-op -Wduplicated-cond -Wduplicated-branches -Wformat-overflow=2 -Wformat-truncation=2 -Wstringop-overflow=4 -fanalyzer"
    
    # Try to compile with static analysis
    if gcc $GCC_FLAGS -I./include -I./src -c src/*.c 2> "$RESULTS_DIR/gcc-analysis.log"; then
        print_status "PASS" "GCC static analysis passed"
    else
        print_status "WARN" "GCC static analysis found issues"
        echo "See $RESULTS_DIR/gcc-analysis.log for details"
    fi
    
    # Clean up object files
    rm -f *.o
fi

echo ""

# Summary
echo "=== Static Analysis Summary ==="
echo "Results saved in: $RESULTS_DIR/"
echo ""

# Count issues
TOTAL_ISSUES=$((TODO_COUNT + PLACEHOLDER_RETURNS))

if [ "$TOTAL_ISSUES" -eq 0 ]; then
    print_status "PASS" "No critical issues found"
    exit 0
else
    print_status "FAIL" "Found $TOTAL_ISSUES critical issues that must be fixed"
    exit 1
fi