/*
** test_storage.c - Storage engine and persistence tests
*/

#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "unity.h"

static sqlite3 *db = NULL;

sqlite3* create_test_db(const char* db_path) {
    sqlite3 *db;
    int rc;
    
    rc = sqlite3_open(db_path, &db);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_enable_load_extension(db, 1);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_load_extension(db, "../build/libgraph", "sqlite3_graph_init", NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Create backing tables if they don't exist
    sqlite3_exec(db, 
        "CREATE TABLE IF NOT EXISTS nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT);"
        "CREATE TABLE IF NOT EXISTS edges(id INTEGER PRIMARY KEY, source INTEGER, target INTEGER, edge_type TEXT, weight REAL, properties TEXT);",
        NULL, NULL, NULL);
    
    // Create virtual table if it doesn't exist (ignore errors)
    sqlite3_exec(db, "CREATE VIRTUAL TABLE IF NOT EXISTS graph USING graph(nodes, edges)", NULL, NULL, NULL);
    
    return db;
}

void setUp(void) {
    db = NULL;
}

void tearDown(void) {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

void test_file_persistence(void) {
    char db_file[256];
    snprintf(db_file, sizeof(db_file), "test_persistence_%ld.db", (long)time(NULL));
    
    // Create and populate database
    db = create_test_db(db_file);
    
    int rc = sqlite3_exec(db, 
        "INSERT INTO nodes (labels, properties) VALUES "
        "('Person', '{\"name\": \"Alice\"}'), "
        "('Person', '{\"name\": \"Bob\"}');"
        "INSERT INTO edges (source, target, edge_type, weight, properties) VALUES "
        "(1, 2, 'KNOWS', 1.0, '{\"since\": 2020}');",
        NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    sqlite3_close(db);
    db = NULL;
    
    // Reopen database and verify data persists
    db = create_test_db(db_file);
    
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM nodes;", -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_step(stmt);
    TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
    
    int node_count = sqlite3_column_int(stmt, 0);
    TEST_ASSERT_EQUAL(2, node_count);
    sqlite3_finalize(stmt);
    
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM edges;", -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_step(stmt);
    TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
    
    int edge_count = sqlite3_column_int(stmt, 0);
    TEST_ASSERT_EQUAL(1, edge_count);
    sqlite3_finalize(stmt);
    
    sqlite3_close(db);
    db = NULL;
    unlink(db_file);
}

void test_large_data_storage(void) {
    char db_file[256];
    snprintf(db_file, sizeof(db_file), "test_large_storage_%ld.db", (long)time(NULL));
    
    db = create_test_db(db_file);
    
    const int NUM_NODES = 1000; // Reduced for faster testing
    const int NUM_EDGES = 2000;
    
    // Insert large number of nodes
    int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    sqlite3_stmt *node_stmt;
    rc = sqlite3_prepare_v2(db, 
        "INSERT INTO nodes (labels, properties) VALUES (?, ?);",
        -1, &node_stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    for (int i = 0; i < NUM_NODES; i++) {
        char props[512];
        snprintf(props, sizeof(props), 
            "{\"id\": %d, \"name\": \"node_%d\"}", 
            i, i);
        
        sqlite3_bind_text(node_stmt, 1, "LargeNode", -1, SQLITE_STATIC);
        sqlite3_bind_text(node_stmt, 2, props, -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(node_stmt);
        TEST_ASSERT_EQUAL(SQLITE_DONE, rc);
        sqlite3_reset(node_stmt);
    }
    sqlite3_finalize(node_stmt);
    
    rc = sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Verify data was stored correctly
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM nodes;", -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_step(stmt);
    TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
    
    int stored_nodes = sqlite3_column_int(stmt, 0);
    TEST_ASSERT_EQUAL(NUM_NODES, stored_nodes);
    sqlite3_finalize(stmt);
    
    sqlite3_close(db);
    db = NULL;
    unlink(db_file);
}

void test_data_integrity_after_crashes(void) {
    char db_file[256];
    snprintf(db_file, sizeof(db_file), "test_crash_recovery_%ld.db", (long)time(NULL));
    
    // Simulate crash scenario by not properly closing database
    {
        sqlite3 *crash_db = create_test_db(db_file);
        
        int rc = sqlite3_exec(crash_db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
        TEST_ASSERT_EQUAL(SQLITE_OK, rc);
        
        rc = sqlite3_exec(crash_db, 
            "INSERT INTO nodes (labels, properties) VALUES "
            "('Person', '{\"name\": \"Committed1\"}'), "
            "('Person', '{\"name\": \"Committed2\"}');",
            NULL, NULL, NULL);
        TEST_ASSERT_EQUAL(SQLITE_OK, rc);
        
        rc = sqlite3_exec(crash_db, "COMMIT;", NULL, NULL, NULL);
        TEST_ASSERT_EQUAL(SQLITE_OK, rc);
        
        // Start another transaction but don't commit (simulate crash)
        rc = sqlite3_exec(crash_db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
        TEST_ASSERT_EQUAL(SQLITE_OK, rc);
        
        rc = sqlite3_exec(crash_db, 
            "INSERT INTO nodes (labels, properties) VALUES "
            "('Person', '{\"name\": \"NotCommitted\"}');",
            NULL, NULL, NULL);
        TEST_ASSERT_EQUAL(SQLITE_OK, rc);
        
        // Close database without committing (simulates crash)
        sqlite3_close(crash_db);
    }
    
    // Reopen database and verify only committed data exists
    db = create_test_db(db_file);
    
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM nodes;", -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_step(stmt);
    TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
    
    int count = sqlite3_column_int(stmt, 0);
    TEST_ASSERT_EQUAL(2, count); // Only committed nodes should exist
    sqlite3_finalize(stmt);
    
    // Verify uncommitted data is not present
    rc = sqlite3_prepare_v2(db, 
        "SELECT COUNT(*) FROM nodes WHERE properties LIKE '%NotCommitted%';", 
        -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_step(stmt);
    TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
    
    int uncommitted_count = sqlite3_column_int(stmt, 0);
    TEST_ASSERT_EQUAL(0, uncommitted_count);
    sqlite3_finalize(stmt);
    
    sqlite3_close(db);
    db = NULL;
    unlink(db_file);
}

void test_storage_optimization(void) {
    char db_file[256];
    snprintf(db_file, sizeof(db_file), "test_optimization_%ld.db", (long)time(NULL));
    
    db = create_test_db(db_file);
    
    // Insert some data
    int rc = sqlite3_exec(db, 
        "BEGIN TRANSACTION;"
        "INSERT INTO nodes (labels, properties) VALUES "
        "('Person', '{\"name\": \"Test1\"}'), "
        "('Person', '{\"name\": \"Test2\"}'), "
        "('Person', '{\"name\": \"Test3\"}');"
        "INSERT INTO edges (source, target, edge_type, weight, properties) VALUES "
        "(1, 2, 'KNOWS', 1.0, '{}'), "
        "(2, 3, 'KNOWS', 1.0, '{}');"
        "COMMIT;",
        NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Test VACUUM operation
    rc = sqlite3_exec(db, "VACUUM;", NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Verify data still exists after optimization
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM nodes;", -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_step(stmt);
    TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
    
    int count = sqlite3_column_int(stmt, 0);
    TEST_ASSERT_EQUAL(3, count);
    sqlite3_finalize(stmt);
    
    sqlite3_close(db);
    db = NULL;
    unlink(db_file);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_file_persistence);
    RUN_TEST(test_large_data_storage);
    RUN_TEST(test_data_integrity_after_crashes);
    RUN_TEST(test_storage_optimization);
    
    return UNITY_END();
}
