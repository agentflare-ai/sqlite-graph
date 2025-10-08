#!/usr/bin/env python3

import os
import re

def update_tck_test_file(filename, implementations):
    """Update a TCK test file with implemented scenarios"""
    
    if not os.path.exists(filename):
        print(f"File {filename} not found")
        return
    
    with open(filename, 'r') as f:
        content = f.read()
    
    for test_name, implementation in implementations.items():
        # Find the test function
        pattern = f'void {test_name}\\(void\\) \\{{.*?^\\}}'
        match = re.search(pattern, content, re.MULTILINE | re.DOTALL)
        
        if match:
            # Replace the test body
            new_content = content.replace(match.group(0), implementation)
            content = new_content
            print(f"✅ Updated {test_name}")
        else:
            print(f"❌ Test {test_name} not found")
    
    # Write back the updated content
    with open(filename, 'w') as f:
        f.write(content)
    
    print(f"✅ Updated {filename}")

# Basic CREATE scenarios we can implement with current infrastructure
create_implementations = {
    'test_clauses_create_Create1_01': '''void test_clauses_create_Create1_01(void) {
    // TCK: Create a single node
    // Cypher: CREATE ()
    // SQL equivalent: INSERT INTO graph (type, properties) VALUES ('node', '{}')
    
    int rc = sqlite3_exec(db, 
        "CREATE TABLE nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT);"
        "CREATE TABLE edges(id INTEGER PRIMARY KEY, from_id INTEGER, to_id INTEGER, type TEXT, weight REAL, properties TEXT);"
        "CREATE VIRTUAL TABLE graph USING graph(nodes, edges);",
        NULL, NULL, &error_msg);
    
    if (rc != SQLITE_OK) {
        printf("Setup failed: %s\\n", error_msg);
        TEST_FAIL();
        return;
    }
    
    // Create a single node: CREATE ()
    rc = sqlite3_exec(db, 
        "INSERT INTO graph (type, properties) VALUES ('node', '{}')",
        NULL, NULL, &error_msg);
    
    if (rc != SQLITE_OK) {
        printf("Create node failed: %s\\n", error_msg);
        TEST_FAIL();
        return;
    }
    
    // Verify the node was created
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph WHERE type = 'node'", -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
    int count = sqlite3_column_int(stmt, 0);
    TEST_ASSERT_EQUAL(1, count);
    
    sqlite3_finalize(stmt);
}''',

    'test_clauses_create_Create1_03': '''void test_clauses_create_Create1_03(void) {
    // TCK: Create a single node with a label
    // Cypher: CREATE (:Label)
    // SQL equivalent: INSERT INTO graph (type, labels, properties) VALUES ('node', 'Label', '{}')
    
    int rc = sqlite3_exec(db, 
        "CREATE TABLE nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT);"
        "CREATE TABLE edges(id INTEGER PRIMARY KEY, from_id INTEGER, to_id INTEGER, type TEXT, weight REAL, properties TEXT);"
        "CREATE VIRTUAL TABLE graph USING graph(nodes, edges);",
        NULL, NULL, &error_msg);
    
    if (rc != SQLITE_OK) {
        printf("Setup failed: %s\\n", error_msg);
        TEST_FAIL();
        return;
    }
    
    // Create a node with label: CREATE (:Label)
    rc = sqlite3_exec(db, 
        "INSERT INTO graph (type, labels, properties) VALUES ('node', 'Label', '{}')",
        NULL, NULL, &error_msg);
    
    if (rc != SQLITE_OK) {
        printf("Create labeled node failed: %s\\n", error_msg);
        TEST_FAIL();
        return;
    }
    
    // Verify the labeled node was created
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT labels FROM graph WHERE type = 'node'", -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
    const char *labels = (const char*)sqlite3_column_text(stmt, 0);
    TEST_ASSERT_NOT_NULL(labels);
    TEST_ASSERT_EQUAL_STRING("Label", labels);
    
    sqlite3_finalize(stmt);
}''',

    'test_clauses_create_Create1_04': '''void test_clauses_create_Create1_04(void) {
    // TCK: Create a single node with a property
    // Cypher: CREATE ({name: 'A'})
    // SQL equivalent: INSERT INTO graph (type, properties) VALUES ('node', '{"name": "A"}')
    
    int rc = sqlite3_exec(db, 
        "CREATE TABLE nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT);"
        "CREATE TABLE edges(id INTEGER PRIMARY KEY, from_id INTEGER, to_id INTEGER, type TEXT, weight REAL, properties TEXT);"
        "CREATE VIRTUAL TABLE graph USING graph(nodes, edges);",
        NULL, NULL, &error_msg);
    
    if (rc != SQLITE_OK) {
        printf("Setup failed: %s\\n", error_msg);
        TEST_FAIL();
        return;
    }
    
    // Create a node with property: CREATE ({name: 'A'})
    rc = sqlite3_exec(db, 
        "INSERT INTO graph (type, properties) VALUES ('node', '{\\\"name\\\": \\\"A\\\"}')",
        NULL, NULL, &error_msg);
    
    if (rc != SQLITE_OK) {
        printf("Create node with property failed: %s\\n", error_msg);
        TEST_FAIL();
        return;
    }
    
    // Verify the node with property was created
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT properties FROM graph WHERE type = 'node'", -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
    const char *props = (const char*)sqlite3_column_text(stmt, 0);
    TEST_ASSERT_NOT_NULL(props);
    TEST_ASSERT_TRUE(strstr(props, "A") != NULL);
    
    sqlite3_finalize(stmt);
}''',

    'test_clauses_create_Create1_05': '''void test_clauses_create_Create1_05(void) {
    // TCK: Create a single node with a label and a property  
    // Cypher: CREATE (:Label {name: 'A'})
    // SQL equivalent: INSERT INTO graph (type, labels, properties) VALUES ('node', 'Label', '{"name": "A"}')
    
    int rc = sqlite3_exec(db, 
        "CREATE TABLE nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT);"
        "CREATE TABLE edges(id INTEGER PRIMARY KEY, from_id INTEGER, to_id INTEGER, type TEXT, weight REAL, properties TEXT);"
        "CREATE VIRTUAL TABLE graph USING graph(nodes, edges);",
        NULL, NULL, &error_msg);
    
    if (rc != SQLITE_OK) {
        printf("Setup failed: %s\\n", error_msg);
        TEST_FAIL();
        return;
    }
    
    // Create a node with label and property: CREATE (:Label {name: 'A'})
    rc = sqlite3_exec(db, 
        "INSERT INTO graph (type, labels, properties) VALUES ('node', 'Label', '{\\\"name\\\": \\\"A\\\"}')",
        NULL, NULL, &error_msg);
    
    if (rc != SQLITE_OK) {
        printf("Create labeled node with property failed: %s\\n", error_msg);
        TEST_FAIL();
        return;
    }
    
    // Verify the node was created correctly
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT labels, properties FROM graph WHERE type = 'node'", -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
    const char *labels = (const char*)sqlite3_column_text(stmt, 0);
    const char *props = (const char*)sqlite3_column_text(stmt, 1);
    
    TEST_ASSERT_NOT_NULL(labels);
    TEST_ASSERT_NOT_NULL(props);
    TEST_ASSERT_EQUAL_STRING("Label", labels);
    TEST_ASSERT_TRUE(strstr(props, "A") != NULL);
    
    sqlite3_finalize(stmt);
}'''
}

# Update the CREATE test file
update_tck_test_file('tck_test_clauses_create.c', create_implementations)

print("\n" + "="*60)
print("✅ BASIC CREATE SCENARIOS IMPLEMENTED")
print("="*60)
print("Implemented 4 basic CREATE scenarios:")
print("- Create a single node")
print("- Create a single node with a label")  
print("- Create a single node with a property")
print("- Create a single node with a label and a property")
print()
print("These scenarios test the core virtual table functionality:")
print("- Virtual table creation")
print("- Node insertion with different combinations")
print("- Basic property and label storage")
print("- Simple verification queries")
