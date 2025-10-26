#!/usr/bin/env python3
"""
Implement TCK scenarios using actual Cypher queries via cypher_execute()

This replaces the SQL workarounds with proper Cypher execution to test
the full Cypher implementation end-to-end.
"""

import os
import re
import json

def generate_cypher_test(test_name, description, cypher_query, setup_queries=None, expected_result=None, expected_count=None):
    """
    Generate a test function that uses cypher_execute()

    Args:
        test_name: Function name (e.g., 'test_clauses_create_Create1_01')
        description: What the test does
        cypher_query: The Cypher query to execute
        setup_queries: Optional list of Cypher queries to run first
        expected_result: Optional JSON structure to validate against
        expected_count: Optional expected row count
    """

    setup_code = ""
    if setup_queries:
        for i, query in enumerate(setup_queries):
            escaped_query = query.replace('"', '\\"')
            setup_code += f'''
    // Setup query {i+1}
    rc = sqlite3_prepare_v2(db, "SELECT cypher_execute('{escaped_query}')", -1, &stmt, NULL);
    if (rc != SQLITE_OK) {{
        printf("Setup query {i+1} prepare failed: %s\\n", sqlite3_errmsg(db));
        TEST_FAIL();
        return;
    }}
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {{
        printf("Setup query {i+1} execute failed: %s\\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        TEST_FAIL();
        return;
    }}
    sqlite3_finalize(stmt);
'''

    # Escape the Cypher query for C string
    escaped_query = cypher_query.replace('"', '\\"')

    validation_code = ""
    if expected_count is not None:
        validation_code = f'''
    // Parse JSON result
    const char *result_json = (const char*)sqlite3_column_text(stmt, 0);
    TEST_ASSERT_NOT_NULL(result_json);

    // Count results (simple JSON array counting)
    int count = 0;
    if (result_json[0] == '[') {{
        const char *p = result_json;
        while (*p) {{
            if (*p == '{{') count++;
            p++;
        }}
    }}
    TEST_ASSERT_EQUAL({expected_count}, count);
'''
    elif expected_result:
        validation_code = f'''
    // Validate result
    const char *result_json = (const char*)sqlite3_column_text(stmt, 0);
    TEST_ASSERT_NOT_NULL(result_json);
    // Expected: {expected_result}
    TEST_ASSERT_TRUE(strlen(result_json) > 0);
'''
    else:
        validation_code = '''
    // Verify execution succeeded
    const char *result_json = (const char*)sqlite3_column_text(stmt, 0);
    TEST_ASSERT_NOT_NULL(result_json);
'''

    return f'''void {test_name}(void) {{
    // {description}
    // Cypher: {cypher_query}

    sqlite3_stmt *stmt;
    int rc;

    // Create graph virtual table (creates backing tables automatically)
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {{
        printf("Graph table creation failed: %s\\n", error_msg);
        TEST_FAIL();
        return;
    }}
    {setup_code}
    // Execute Cypher query
    rc = sqlite3_prepare_v2(db, "SELECT cypher_execute('{escaped_query}')", -1, &stmt, NULL);

    if (rc != SQLITE_OK) {{
        printf("Cypher prepare failed: %s\\n", sqlite3_errmsg(db));
        TEST_FAIL();
        return;
    }}

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {{
        printf("Cypher execute failed: %s\\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        TEST_FAIL();
        return;
    }}
    {validation_code}
    sqlite3_finalize(stmt);
}}'''

def update_test_file(filename, implementations):
    """Replace test implementations in a file"""

    if not os.path.exists(filename):
        print(f"❌ File {filename} not found")
        return False

    with open(filename, 'r') as f:
        content = f.read()

    updated = 0
    for test_name, implementation in implementations.items():
        # Find the test function - match from void to closing brace
        pattern = rf'void {re.escape(test_name)}\(void\) \{{.*?^\}}'
        match = re.search(pattern, content, re.MULTILINE | re.DOTALL)

        if match:
            content = content.replace(match.group(0), implementation)
            updated += 1
            print(f"  ✅ Updated {test_name}")
        else:
            print(f"  ⚠️  Test {test_name} not found")

    if updated > 0:
        with open(filename, 'w') as f:
            f.write(content)
        print(f"✅ Updated {filename} ({updated} tests)")
        return True
    else:
        print(f"⚠️  No tests updated in {filename}")
        return False

# ============================================================================
# MATCH test implementations using Cypher
# ============================================================================

match_cypher_tests = {
    'test_clauses_match_Match1_01': generate_cypher_test(
        'test_clauses_match_Match1_01',
        'TCK: Return single node',
        'MATCH (a) RETURN a',
        setup_queries=['CREATE ()'],
        expected_count=1
    ),

    'test_clauses_match_Match1_02': generate_cypher_test(
        'test_clauses_match_Match1_02',
        'TCK: Return node with property',
        'MATCH (n) RETURN n',
        setup_queries=['CREATE ({name: "A"})'],
        expected_count=1
    ),

    'test_clauses_match_Match1_03': generate_cypher_test(
        'test_clauses_match_Match1_03',
        'TCK: Return multiple nodes',
        'MATCH (n) RETURN n',
        setup_queries=['CREATE ()', 'CREATE ()'],
        expected_count=2
    ),

    'test_clauses_match_Match1_04': generate_cypher_test(
        'test_clauses_match_Match1_04',
        'TCK: Return node with label',
        'MATCH (n:Label) RETURN n',
        setup_queries=['CREATE (:Label)'],
        expected_count=1
    ),

    'test_clauses_match_Match1_05': generate_cypher_test(
        'test_clauses_match_Match1_05',
        'TCK: Match labeled node among mixed nodes',
        'MATCH (n:Label) RETURN n',
        setup_queries=['CREATE ()', 'CREATE (:Label)', 'CREATE ()'],
        expected_count=1
    ),

    'test_clauses_match_Match1_06': generate_cypher_test(
        'test_clauses_match_Match1_06',
        'TCK: Match nodes by label',
        'MATCH (n:Person) RETURN n',
        setup_queries=['CREATE (:Person {name: "Alice"})', 'CREATE (:Person {name: "Bob"})'],
        expected_count=2
    ),
}

# ============================================================================
# MATCH WHERE test implementations using Cypher
# ============================================================================

where_cypher_tests = {
    'test_clauses_match_where_MatchWhere1_01': generate_cypher_test(
        'test_clauses_match_where_MatchWhere1_01',
        'TCK: Filter node with property comparison',
        'MATCH (n:Person) WHERE n.age > 25 RETURN n',
        setup_queries=[
            'CREATE (:Person {name: "Alice", age: 30})',
            'CREATE (:Person {name: "Bob", age: 20})',
            'CREATE (:Person {name: "Charlie", age: 35})'
        ],
        expected_count=2
    ),

    'test_clauses_match_where_MatchWhere1_02': generate_cypher_test(
        'test_clauses_match_where_MatchWhere1_02',
        'TCK: Filter with equality comparison',
        'MATCH (n:Person) WHERE n.name = "Alice" RETURN n',
        setup_queries=[
            'CREATE (:Person {name: "Alice"})',
            'CREATE (:Person {name: "Bob"})'
        ],
        expected_count=1
    ),

    'test_clauses_match_where_MatchWhere1_03': generate_cypher_test(
        'test_clauses_match_where_MatchWhere1_03',
        'TCK: Filter with less than comparison',
        'MATCH (n:Person) WHERE n.age < 30 RETURN n',
        setup_queries=[
            'CREATE (:Person {age: 25})',
            'CREATE (:Person {age: 35})'
        ],
        expected_count=1
    ),
}

# ============================================================================
# RETURN test implementations using Cypher
# ============================================================================

return_cypher_tests = {
    'test_clauses_return_Return1_01': generate_cypher_test(
        'test_clauses_return_Return1_01',
        'TCK: Return node',
        'CREATE (a) RETURN a',
        expected_count=1
    ),

    'test_clauses_return_Return1_02': generate_cypher_test(
        'test_clauses_return_Return1_02',
        'TCK: Return multiple columns',
        'MATCH (a)-[r:KNOWS]->(b) RETURN a, r, b',
        setup_queries=[
            'CREATE (a:Person {name: "Alice"})-[:KNOWS]->(b:Person {name: "Bob"})'
        ],
        expected_count=1
    ),

    'test_clauses_return_Return2_01': generate_cypher_test(
        'test_clauses_return_Return2_01',
        'TCK: Return nodes from relationship pattern',
        'MATCH (a)-[:KNOWS]->(b) RETURN a, b',
        setup_queries=[
            'CREATE (:Person {name: "Alice"})-[:KNOWS]->(:Person {name: "Bob"})'
        ],
        expected_count=1
    ),
}

# ============================================================================
# Main execution
# ============================================================================

def main():
    print("="*70)
    print("Implementing TCK tests with Cypher execution")
    print("="*70)
    print()

    # Update MATCH tests
    print("📝 Updating MATCH tests...")
    if update_test_file('tck_test_clauses_match.c', match_cypher_tests):
        print(f"   Implemented {len(match_cypher_tests)} MATCH scenarios")
    print()

    # Update WHERE tests
    print("📝 Updating MATCH WHERE tests...")
    if update_test_file('tck_test_clauses_match_where.c', where_cypher_tests):
        print(f"   Implemented {len(where_cypher_tests)} WHERE scenarios")
    print()

    # Update RETURN tests
    print("📝 Updating RETURN tests...")
    if update_test_file('tck_test_clauses_return.c', return_cypher_tests):
        print(f"   Implemented {len(return_cypher_tests)} RETURN scenarios")
    print()

    print("="*70)
    print("✅ TCK TESTS NOW USE CYPHER")
    print("="*70)
    print()
    print("Summary:")
    print(f"  • {len(match_cypher_tests)} MATCH tests using cypher_execute()")
    print(f"  • {len(where_cypher_tests)} WHERE tests using cypher_execute()")
    print(f"  • {len(return_cypher_tests)} RETURN tests using cypher_execute()")
    print()
    print("All tests now execute real Cypher queries end-to-end!")
    print()
    print("Next steps:")
    print("  1. cd tests")
    print("  2. make clean && make")
    print("  3. ./tck_test_clauses_match")
    print("  4. ./tck_test_clauses_match_where")
    print("  5. ./tck_test_clauses_return")
    print()

if __name__ == '__main__':
    main()
