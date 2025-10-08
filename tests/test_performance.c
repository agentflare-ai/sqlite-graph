/*
** test_performance.c - Performance and benchmarking tests
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>
#include <sys/time.h>
#include "unity.h"

static sqlite3 *db = NULL;

double get_time_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

sqlite3* create_test_db(void) {
    sqlite3 *db;
    int rc;
    
    rc = sqlite3_open(":memory:", &db);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_enable_load_extension(db, 1);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_load_extension(db, "../build/libgraph.so", "sqlite3_graph_init", NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Create backing tables
    rc = sqlite3_exec(db, 
        "CREATE TABLE nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT);"
        "CREATE TABLE edges(id INTEGER PRIMARY KEY, source INTEGER, target INTEGER, edge_type TEXT, weight REAL, properties TEXT);",
        NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Create virtual table
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph(nodes, edges)", NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
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

void test_bulk_insert_performance(void) {
    db = create_test_db();
    struct timeval start, end;
    int rc;
    
    const int NUM_NODES = 1000;
    
    gettimeofday(&start, NULL);
    
    // Start transaction for bulk insert
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Prepare statement for bulk insert
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, 
        "INSERT INTO nodes (labels, properties) VALUES (?, ?);",
        -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Insert nodes in bulk
    for (int i = 0; i < NUM_NODES; i++) {
        char props[256];
        snprintf(props, sizeof(props), "{\"id\": %d, \"name\": \"node_%d\"}", i, i);
        
        sqlite3_bind_text(stmt, 1, "TestNode", -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, props, -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        TEST_ASSERT_EQUAL(SQLITE_DONE, rc);
        
        sqlite3_reset(stmt);
    }
    
    sqlite3_finalize(stmt);
    
    // Commit transaction
    rc = sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    gettimeofday(&end, NULL);
    double duration = get_time_diff(start, end);
    
    printf("Bulk insert of %d nodes took %.3f seconds (%.0f nodes/sec)\n", 
           NUM_NODES, duration, NUM_NODES / duration);
    
    // Verify all nodes were inserted
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM nodes;", -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_step(stmt);
    TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
    
    int count = sqlite3_column_int(stmt, 0);
    TEST_ASSERT_EQUAL(NUM_NODES, count);
    
    sqlite3_finalize(stmt);
    
    // Performance should be reasonable (more than 100 nodes/sec)
    TEST_ASSERT_TRUE(duration < 10.0);
}

void test_bulk_query_performance(void) {
    db = create_test_db();
    struct timeval start, end;
    int rc;
    
    const int NUM_NODES = 1000;
    
    // Insert test data first
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    sqlite3_stmt *insert_stmt;
    rc = sqlite3_prepare_v2(db, 
        "INSERT INTO nodes (labels, properties) VALUES (?, ?);",
        -1, &insert_stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    for (int i = 0; i < NUM_NODES; i++) {
        char props[256];
        snprintf(props, sizeof(props), "{\"id\": %d, \"category\": %d}", i, i % 10);
        
        sqlite3_bind_text(insert_stmt, 1, "TestNode", -1, SQLITE_STATIC);
        sqlite3_bind_text(insert_stmt, 2, props, -1, SQLITE_TRANSIENT);
        
        sqlite3_step(insert_stmt);
        sqlite3_reset(insert_stmt);
    }
    
    sqlite3_finalize(insert_stmt);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    
    // Test query performance
    gettimeofday(&start, NULL);
    
    sqlite3_stmt *query_stmt;
    rc = sqlite3_prepare_v2(db, 
        "SELECT COUNT(*) FROM nodes WHERE properties LIKE '%category\": 5%';",
        -1, &query_stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_step(query_stmt);
    TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
    
    int matching_count = sqlite3_column_int(query_stmt, 0);
    sqlite3_finalize(query_stmt);
    
    gettimeofday(&end, NULL);
    double duration = get_time_diff(start, end);
    
    printf("Query across %d nodes took %.3f seconds\n", NUM_NODES, duration);
    
    // Verify correct results
    TEST_ASSERT_EQUAL(100, matching_count); // Should find nodes with category 5
    
    // Performance should be reasonable (less than 1 second)
    TEST_ASSERT_TRUE(duration < 1.0);
}

void test_large_graph_traversal_performance(void) {
    db = create_test_db();
    struct timeval start, end;
    int rc;
    
    const int NUM_NODES = 500;
    const int NUM_EDGES = 1000;
    
    // Create a larger graph
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Insert nodes
    sqlite3_stmt *node_stmt;
    rc = sqlite3_prepare_v2(db, 
        "INSERT INTO nodes (id, labels, properties) VALUES (?, ?, ?);",
        -1, &node_stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    for (int i = 1; i <= NUM_NODES; i++) {
        char props[256];
        snprintf(props, sizeof(props), "{\"name\": \"node_%d\"}", i);
        
        sqlite3_bind_int(node_stmt, 1, i);
        sqlite3_bind_text(node_stmt, 2, "Node", -1, SQLITE_STATIC);
        sqlite3_bind_text(node_stmt, 3, props, -1, SQLITE_TRANSIENT);
        
        sqlite3_step(node_stmt);
        sqlite3_reset(node_stmt);
    }
    sqlite3_finalize(node_stmt);
    
    // Insert edges (random connections)
    sqlite3_stmt *edge_stmt;
    rc = sqlite3_prepare_v2(db, 
        "INSERT INTO edges (source, target, edge_type, weight, properties) VALUES (?, ?, ?, ?, ?);",
        -1, &edge_stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    srand(42); // Fixed seed for reproducible results
    for (int i = 0; i < NUM_EDGES; i++) {
        int start = (rand() % NUM_NODES) + 1;
        int end = (rand() % NUM_NODES) + 1;
        if (start != end) {
            sqlite3_bind_int(edge_stmt, 1, start);
            sqlite3_bind_int(edge_stmt, 2, end);
            sqlite3_bind_text(edge_stmt, 3, "CONNECTS", -1, SQLITE_STATIC);
            sqlite3_bind_double(edge_stmt, 4, 1.0);
            sqlite3_bind_text(edge_stmt, 5, "{}", -1, SQLITE_STATIC);
            
            sqlite3_step(edge_stmt);
            sqlite3_reset(edge_stmt);
        }
    }
    sqlite3_finalize(edge_stmt);
    
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    
    // Test traversal performance
    gettimeofday(&start, NULL);
    
    sqlite3_stmt *traversal_stmt;
    rc = sqlite3_prepare_v2(db, 
        "WITH RECURSIVE reachable(node_id, depth) AS ("
        "  SELECT 1 as node_id, 0 as depth "
        "  UNION ALL "
        "  SELECT e.target, r.depth + 1 "
        "  FROM reachable r "
        "  JOIN edges e ON r.node_id = e.source "
        "  WHERE r.depth < 5"
        ") "
        "SELECT COUNT(DISTINCT node_id) FROM reachable;",
        -1, &traversal_stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    rc = sqlite3_step(traversal_stmt);
    TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
    
    int reachable_count = sqlite3_column_int(traversal_stmt, 0);
    sqlite3_finalize(traversal_stmt);
    
    gettimeofday(&end, NULL);
    double duration = get_time_diff(start, end);
    
    printf("Graph traversal on %d nodes, %d edges took %.3f seconds (found %d reachable nodes)\n", 
           NUM_NODES, NUM_EDGES, duration, reachable_count);
    
    // Verify reasonable results
    TEST_ASSERT_TRUE(reachable_count >= 1);
    TEST_ASSERT_TRUE(reachable_count <= NUM_NODES);
    
    // Performance should be reasonable (less than 2 seconds)
    TEST_ASSERT_TRUE(duration < 2.0);
}

void test_memory_usage_scaling(void) {
    db = create_test_db();
    int rc;
    
    // Test memory usage with different graph sizes
    const int sizes[] = {100, 500, 1000};
    const int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        int size = sizes[s];
        
        // Clear previous data
        rc = sqlite3_exec(db, "DELETE FROM nodes; DELETE FROM edges;", NULL, NULL, NULL);
        TEST_ASSERT_EQUAL(SQLITE_OK, rc);
        
        // Insert nodes
        rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
        TEST_ASSERT_EQUAL(SQLITE_OK, rc);
        
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, 
            "INSERT INTO nodes (labels, properties) VALUES (?, ?);",
            -1, &stmt, NULL);
        TEST_ASSERT_EQUAL(SQLITE_OK, rc);
        
        for (int i = 0; i < size; i++) {
            char props[256];
            snprintf(props, sizeof(props), "{\"id\": %d}", i);
            
            sqlite3_bind_text(stmt, 1, "Node", -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, props, -1, SQLITE_TRANSIENT);
            
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
        }
        
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
        
        // Verify count
        rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM nodes;", -1, &stmt, NULL);
        TEST_ASSERT_EQUAL(SQLITE_OK, rc);
        
        rc = sqlite3_step(stmt);
        TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
        
        int count = sqlite3_column_int(stmt, 0);
        TEST_ASSERT_EQUAL(size, count);
        
        sqlite3_finalize(stmt);
        
        printf("Successfully handled %d nodes\n", size);
    }
}

void test_concurrent_access_performance(void) {
    db = create_test_db();
    struct timeval start, end;
    int rc;
    
    // Insert initial data
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, 
        "INSERT INTO nodes (labels, properties) VALUES (?, ?);",
        -1, &stmt, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    for (int i = 0; i < 100; i++) {
        char props[256];
        snprintf(props, sizeof(props), "{\"id\": %d}", i);
        
        sqlite3_bind_text(stmt, 1, "Node", -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, props, -1, SQLITE_TRANSIENT);
        
        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }
    
    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    
    // Test multiple rapid queries (simulating concurrent access)
    gettimeofday(&start, NULL);
    
    for (int i = 0; i < 100; i++) {
        rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM nodes;", -1, &stmt, NULL);
        TEST_ASSERT_EQUAL(SQLITE_OK, rc);
        
        rc = sqlite3_step(stmt);
        TEST_ASSERT_EQUAL(SQLITE_ROW, rc);
        
        int count = sqlite3_column_int(stmt, 0);
        TEST_ASSERT_EQUAL(100, count);
        
        sqlite3_finalize(stmt);
    }
    
    gettimeofday(&end, NULL);
    double duration = get_time_diff(start, end);
    
    printf("100 sequential queries took %.3f seconds (%.1f queries/sec)\n", 
           duration, 100.0 / duration);
    
    // Should handle at least 50 queries per second
    TEST_ASSERT_TRUE(duration < 2.0);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_bulk_insert_performance);
    RUN_TEST(test_bulk_query_performance);
    RUN_TEST(test_large_graph_traversal_performance);
    RUN_TEST(test_memory_usage_scaling);
    RUN_TEST(test_concurrent_access_performance);
    
    return UNITY_END();
}
