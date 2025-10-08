#!/bin/bash

# Security Audit Script for SQLite Graph Extension
# Comprehensive security analysis and vulnerability assessment

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "=== SQLite Graph Extension Security Audit ==="
echo ""

# Change to project root
cd "$(dirname "$0")/.."

# Results directory
AUDIT_DIR="security_audit"
mkdir -p "$AUDIT_DIR"

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
    elif [ "$status" = "INFO" ]; then
        echo -e "${BLUE}ℹ $message${NC}"
    else
        echo "  $message"
    fi
}

# Initialize audit report
cat > "$AUDIT_DIR/security_audit_report.md" << 'EOF'
# SQLite Graph Extension Security Audit Report

## Executive Summary

This report documents the security audit findings for the SQLite Graph Extension.

## Audit Scope

- Source code security analysis
- Input validation and sanitization
- SQL injection prevention
- Buffer overflow protection
- Memory safety
- Authentication and authorization
- Cryptographic practices
- Error handling security
- Configuration security

## Findings

EOF

# Security check functions
check_sql_injection() {
    echo "=== SQL Injection Vulnerability Assessment ==="
    
    local issues=0
    
    # Check for unsafe string formatting in SQL
    print_status "INFO" "Checking for unsafe SQL string construction..."
    
    local unsafe_sql=$(find src/ -name "*.c" -exec grep -l "sprintf.*SELECT\|sprintf.*INSERT\|sprintf.*UPDATE\|sprintf.*DELETE\|strcat.*SELECT\|strcat.*INSERT" {} \; 2>/dev/null | wc -l)
    
    if [ "$unsafe_sql" -eq 0 ]; then
        print_status "PASS" "No unsafe SQL string construction found"
        echo "- ✅ No sprintf/strcat with SQL commands detected" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "FAIL" "Found $unsafe_sql files with potentially unsafe SQL construction"
        echo "- ❌ Found $unsafe_sql files with unsafe SQL string construction" >> "$AUDIT_DIR/security_audit_report.md"
        issues=$((issues + 1))
        
        # Detail the findings
        echo "### SQL Injection Vulnerabilities" >> "$AUDIT_DIR/security_audit_report.md"
        find src/ -name "*.c" -exec grep -l "sprintf.*SELECT\|sprintf.*INSERT\|sprintf.*UPDATE\|sprintf.*DELETE" {} \; 2>/dev/null >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for parameterized queries usage
    local bind_usage=$(find src/ -name "*.c" -exec grep -c "sqlite3_bind_" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$bind_usage" -gt 0 ]; then
        print_status "PASS" "Using parameterized queries ($bind_usage bind calls)"
        echo "- ✅ Parameterized queries used ($bind_usage sqlite3_bind calls)" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "No parameterized query usage detected"
        echo "- ⚠️ Limited use of parameterized queries" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for SQL preparation
    local prepare_usage=$(find src/ -name "*.c" -exec grep -c "sqlite3_prepare" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$prepare_usage" -gt 0 ]; then
        print_status "PASS" "Using prepared statements ($prepare_usage prepare calls)"
        echo "- ✅ Prepared statements used ($prepare_usage sqlite3_prepare calls)" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "No prepared statement usage detected"
        echo "- ⚠️ No prepared statements detected" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    return $issues
}

check_buffer_overflows() {
    echo ""
    echo "=== Buffer Overflow Protection Assessment ==="
    
    local issues=0
    
    # Check for unsafe string functions
    print_status "INFO" "Checking for unsafe string functions..."
    
    local unsafe_funcs=$(find src/ -name "*.c" -exec grep -c "strcpy\|strcat\|sprintf\|gets" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    local safe_funcs=$(find src/ -name "*.c" -exec grep -c "strncpy\|strncat\|snprintf\|fgets" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    echo "### Buffer Overflow Protection" >> "$AUDIT_DIR/security_audit_report.md"
    
    if [ "$unsafe_funcs" -eq 0 ]; then
        print_status "PASS" "No unsafe string functions found"
        echo "- ✅ No unsafe string functions (strcpy, strcat, sprintf, gets) detected" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "Found $unsafe_funcs uses of potentially unsafe string functions"
        echo "- ⚠️ Found $unsafe_funcs uses of potentially unsafe string functions" >> "$AUDIT_DIR/security_audit_report.md"
        
        # Detail unsafe function usage
        echo "#### Unsafe String Function Usage:" >> "$AUDIT_DIR/security_audit_report.md"
        find src/ -name "*.c" -exec grep -n "strcpy\|strcat\|sprintf\|gets" {} /dev/null \; 2>/dev/null >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    if [ "$safe_funcs" -gt 0 ]; then
        print_status "PASS" "Using safe string functions ($safe_funcs calls)"
        echo "- ✅ Safe string functions used ($safe_funcs calls to strncpy/strncat/snprintf/fgets)" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for bounds checking
    local bounds_checks=$(find src/ -name "*.c" -exec grep -c "sizeof\|strlen.*<\|.*< sizeof" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$bounds_checks" -gt 0 ]; then
        print_status "PASS" "Found bounds checking patterns ($bounds_checks instances)"
        echo "- ✅ Bounds checking patterns found ($bounds_checks instances)" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "Limited bounds checking detected"
        echo "- ⚠️ Limited bounds checking patterns detected" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    return $issues
}

check_input_validation() {
    echo ""
    echo "=== Input Validation Assessment ==="
    
    local issues=0
    
    echo "### Input Validation" >> "$AUDIT_DIR/security_audit_report.md"
    
    # Check for NULL pointer validation
    local null_checks=$(find src/ -name "*.c" -exec grep -c "if.*== NULL\|if.*!.*)" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$null_checks" -gt 0 ]; then
        print_status "PASS" "NULL pointer validation found ($null_checks checks)"
        echo "- ✅ NULL pointer validation implemented ($null_checks checks)" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "Limited NULL pointer validation"
        echo "- ⚠️ Limited NULL pointer validation detected" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for range validation
    local range_checks=$(find src/ -name "*.c" -exec grep -c "if.*> \|if.*< \|if.*>= \|if.*<= " {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$range_checks" -gt 0 ]; then
        print_status "PASS" "Range validation found ($range_checks checks)"
        echo "- ✅ Range validation implemented ($range_checks checks)" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "Limited range validation"
        echo "- ⚠️ Limited range validation detected" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for JSON validation
    local json_validation=$(find src/ -name "*.c" -exec grep -c "json_valid\|json_type" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$json_validation" -gt 0 ]; then
        print_status "PASS" "JSON validation found ($json_validation instances)"
        echo "- ✅ JSON validation implemented ($json_validation instances)" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "No JSON validation detected"
        echo "- ⚠️ No JSON validation detected" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    return $issues
}

check_memory_safety() {
    echo ""
    echo "=== Memory Safety Assessment ==="
    
    local issues=0
    
    echo "### Memory Safety" >> "$AUDIT_DIR/security_audit_report.md"
    
    # Check malloc/free balance
    local malloc_calls=$(find src/ -name "*.c" -exec grep -c "malloc\|calloc\|realloc" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    local free_calls=$(find src/ -name "*.c" -exec grep -c "free(" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$malloc_calls" -gt 0 ] && [ "$free_calls" -gt 0 ]; then
        local ratio=$(echo "scale=2; $free_calls / $malloc_calls" | bc -l)
        if (( $(echo "$ratio >= 0.8" | bc -l) )); then
            print_status "PASS" "Memory allocation/deallocation balanced (ratio: $ratio)"
            echo "- ✅ Memory allocation/deallocation balanced ($malloc_calls malloc, $free_calls free, ratio: $ratio)" >> "$AUDIT_DIR/security_audit_report.md"
        else
            print_status "WARN" "Potential memory leaks (ratio: $ratio)"
            echo "- ⚠️ Potential memory leaks ($malloc_calls malloc, $free_calls free, ratio: $ratio)" >> "$AUDIT_DIR/security_audit_report.md"
        fi
    else
        print_status "INFO" "No dynamic memory allocation detected"
        echo "- ℹ️ No dynamic memory allocation detected" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for double-free protection
    local double_free_protection=$(find src/ -name "*.c" -exec grep -c "= NULL.*free\|free.*= NULL" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$double_free_protection" -gt 0 ]; then
        print_status "PASS" "Double-free protection found ($double_free_protection instances)"
        echo "- ✅ Double-free protection implemented ($double_free_protection instances)" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "No double-free protection patterns detected"
        echo "- ⚠️ No double-free protection patterns detected" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for use-after-free protection
    local uaf_protection=$(find src/ -name "*.c" -exec grep -c "if.*!= NULL.*before\|NULL.*check.*before" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    return $issues
}

check_error_handling() {
    echo ""
    echo "=== Error Handling Security Assessment ==="
    
    local issues=0
    
    echo "### Error Handling Security" >> "$AUDIT_DIR/security_audit_report.md"
    
    # Check for information disclosure in error messages
    local error_disclosure=$(find src/ -name "*.c" -exec grep -c "printf.*error\|fprintf.*error\|sprintf.*error" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$error_disclosure" -eq 0 ]; then
        print_status "PASS" "No direct error message disclosure found"
        echo "- ✅ No direct error message disclosure in printf/fprintf/sprintf" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "Found $error_disclosure potential error disclosures"
        echo "- ⚠️ Found $error_disclosure potential error message disclosures" >> "$AUDIT_DIR/security_audit_report.md"
        
        # Detail the findings
        echo "#### Potential Error Disclosures:" >> "$AUDIT_DIR/security_audit_report.md"
        find src/ -name "*.c" -exec grep -n "printf.*error\|fprintf.*error\|sprintf.*error" {} /dev/null \; 2>/dev/null >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for proper error code usage
    local error_codes=$(find src/ -name "*.c" -exec grep -c "SQLITE_.*\|GRAPH_.*_ERROR" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$error_codes" -gt 0 ]; then
        print_status "PASS" "Using structured error codes ($error_codes instances)"
        echo "- ✅ Structured error codes used ($error_codes instances)" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "Limited structured error code usage"
        echo "- ⚠️ Limited structured error code usage" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    return $issues
}

check_crypto_practices() {
    echo ""
    echo "=== Cryptographic Practices Assessment ==="
    
    local issues=0
    
    echo "### Cryptographic Practices" >> "$AUDIT_DIR/security_audit_report.md"
    
    # Check for weak random number generation
    local weak_random=$(find src/ -name "*.c" -exec grep -c "rand()\|srand(" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$weak_random" -eq 0 ]; then
        print_status "PASS" "No weak random number generation found"
        echo "- ✅ No weak random number generation (rand/srand) detected" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "Found $weak_random uses of weak random number generation"
        echo "- ⚠️ Found $weak_random uses of weak random number generation (rand/srand)" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for cryptographic functions
    local crypto_usage=$(find src/ -name "*.c" -exec grep -c "sha\|md5\|hash\|encrypt\|decrypt" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$crypto_usage" -eq 0 ]; then
        print_status "INFO" "No cryptographic functions detected"
        echo "- ℹ️ No cryptographic functions detected" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "INFO" "Cryptographic functions found ($crypto_usage instances)"
        echo "- ℹ️ Cryptographic functions found ($crypto_usage instances) - manual review recommended" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    return $issues
}

check_configuration_security() {
    echo ""
    echo "=== Configuration Security Assessment ==="
    
    local issues=0
    
    echo "### Configuration Security" >> "$AUDIT_DIR/security_audit_report.md"
    
    # Check for hardcoded secrets
    local hardcoded_secrets=$(find src/ -name "*.c" -exec grep -ci "password\|secret\|key\|token" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$hardcoded_secrets" -eq 0 ]; then
        print_status "PASS" "No hardcoded secrets found"
        echo "- ✅ No hardcoded secrets (password/secret/key/token) detected" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "Found $hardcoded_secrets potential hardcoded secrets"
        echo "- ⚠️ Found $hardcoded_secrets potential hardcoded secrets - manual review required" >> "$AUDIT_DIR/security_audit_report.md"
        
        # Detail the findings (but don't expose actual secrets)
        echo "#### Files with potential secrets:" >> "$AUDIT_DIR/security_audit_report.md"
        find src/ -name "*.c" -exec grep -l "password\|secret\|key\|token" {} \; 2>/dev/null >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for configuration validation
    local config_validation=$(find src/ -name "*.c" -exec grep -c "validate.*config\|check.*config" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$config_validation" -gt 0 ]; then
        print_status "PASS" "Configuration validation found ($config_validation instances)"
        echo "- ✅ Configuration validation implemented ($config_validation instances)" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "INFO" "No explicit configuration validation detected"
        echo "- ℹ️ No explicit configuration validation detected" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    return $issues
}

check_privilege_escalation() {
    echo ""
    echo "=== Privilege Escalation Assessment ==="
    
    local issues=0
    
    echo "### Privilege Escalation" >> "$AUDIT_DIR/security_audit_report.md"
    
    # Check for dangerous system calls
    local dangerous_calls=$(find src/ -name "*.c" -exec grep -c "system(\|exec\|popen\|chmod\|chown" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$dangerous_calls" -eq 0 ]; then
        print_status "PASS" "No dangerous system calls found"
        echo "- ✅ No dangerous system calls (system/exec/popen/chmod/chown) detected" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "WARN" "Found $dangerous_calls dangerous system calls"
        echo "- ⚠️ Found $dangerous_calls dangerous system calls - manual review required" >> "$AUDIT_DIR/security_audit_report.md"
        
        # Detail the findings
        echo "#### Dangerous System Calls:" >> "$AUDIT_DIR/security_audit_report.md"
        find src/ -name "*.c" -exec grep -n "system(\|exec\|popen\|chmod\|chown" {} /dev/null \; 2>/dev/null >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    # Check for file operations
    local file_ops=$(find src/ -name "*.c" -exec grep -c "fopen\|open(\|creat(" {} \; 2>/dev/null | awk '{sum += $1} END {print sum+0}')
    
    if [ "$file_ops" -gt 0 ]; then
        print_status "INFO" "File operations found ($file_ops instances) - ensure proper permissions"
        echo "- ℹ️ File operations found ($file_ops instances) - ensure proper permission checking" >> "$AUDIT_DIR/security_audit_report.md"
    else
        print_status "PASS" "No direct file operations detected"
        echo "- ✅ No direct file operations detected" >> "$AUDIT_DIR/security_audit_report.md"
    fi
    
    return $issues
}

# Run all security checks
echo "Starting comprehensive security audit..."
echo ""

total_issues=0

total_issues=$((total_issues + $(check_sql_injection)))
total_issues=$((total_issues + $(check_buffer_overflows)))
total_issues=$((total_issues + $(check_input_validation)))
total_issues=$((total_issues + $(check_memory_safety)))
total_issues=$((total_issues + $(check_error_handling)))
total_issues=$((total_issues + $(check_crypto_practices)))
total_issues=$((total_issues + $(check_configuration_security)))
total_issues=$((total_issues + $(check_privilege_escalation)))

# Finalize the audit report
cat >> "$AUDIT_DIR/security_audit_report.md" << EOF

## Summary

Total security issues found: $total_issues

### Risk Assessment

$(if [ $total_issues -eq 0 ]; then
    echo "**LOW RISK** - No critical security issues identified."
elif [ $total_issues -le 3 ]; then
    echo "**MEDIUM RISK** - Some security concerns identified that should be addressed."
else
    echo "**HIGH RISK** - Multiple security issues identified that require immediate attention."
fi)

### Recommendations

1. **Input Validation**: Ensure all user inputs are properly validated and sanitized
2. **Memory Safety**: Continue using safe string functions and implement bounds checking
3. **Error Handling**: Avoid exposing sensitive information in error messages
4. **Regular Audits**: Perform regular security audits and penetration testing
5. **Security Training**: Ensure development team is trained in secure coding practices

## Conclusion

This automated security audit provides a baseline assessment. Manual security review by security experts is recommended for production deployment.

---

**Audit Date**: $(date)
**Auditor**: Automated Security Audit Script
**Scope**: SQLite Graph Extension Source Code
EOF

# Final summary
echo ""
echo "=== Security Audit Summary ==="

if [ $total_issues -eq 0 ]; then
    print_status "PASS" "No critical security issues found"
    echo "Risk Level: LOW"
elif [ $total_issues -le 3 ]; then
    print_status "WARN" "Some security concerns identified ($total_issues issues)"
    echo "Risk Level: MEDIUM"
else
    print_status "FAIL" "Multiple security issues found ($total_issues issues)"
    echo "Risk Level: HIGH"
fi

echo ""
echo "Detailed audit report: $AUDIT_DIR/security_audit_report.md"
echo ""

if [ $total_issues -eq 0 ]; then
    exit 0
else
    exit 1
fi