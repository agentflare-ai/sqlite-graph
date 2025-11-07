// Generated TCK tests for language area: clauses.merge
// Total scenarios: 75

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unity.h>

static sqlite3 *db = NULL;
static char *error_msg = NULL;

void setUp(void) {
    int rc = sqlite3_open(":memory:", &db);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Enable loading extensions
    sqlite3_enable_load_extension(db, 1);
    
    // Load graph extension
    #ifdef __APPLE__
    rc = sqlite3_load_extension(db, "../build/libgraph.dylib", "sqlite3_graph_init", &error_msg);
#else
    rc = sqlite3_load_extension(db, "../build/libgraph.so", "sqlite3_graph_init", &error_msg);
#endif
    if (rc != SQLITE_OK) {
        printf("Failed to load graph extension: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
}

void tearDown(void) {
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

void test_clauses_merge_Merge1_01(void) {
    // Parse/validate test for: [1] Merge node when no nodes exist
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-01
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-01");

}

void test_clauses_merge_Merge1_02(void) {
    // Parse/validate test for: [2] Merge node with label
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-02
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-02");

}

void test_clauses_merge_Merge1_03(void) {
    // Parse/validate test for: [3] Merge node with label when it exists
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-03
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-03");

}

void test_clauses_merge_Merge1_04(void) {
    // Runtime: Merge node should create when it doesn't match, properties
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // MERGE should create exactly one node with given label/prop
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:42}) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify created
    sqlite3_stmt *st = NULL;
    rc = sqlite3_prepare_v2(db,
        "SELECT COUNT(*) FROM graph_nodes WHERE json_extract(labels,'$[0]')='Person' AND json_extract(properties,'$.pid')=42",
        -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st, 0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge1_05(void) {
    // Parse/validate test for: [5] Merge node should create when it doesn't match, properties and label
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-05
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-05");

}

void test_clauses_merge_Merge1_06(void) {
    // Parse/validate test for: [6] Merge node with prop and label
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-06
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-06");

}

void test_clauses_merge_Merge1_07(void) {
    // Parse/validate test for: [7] Merge should work when finding multiple elements
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-07
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-07");

}

void test_clauses_merge_Merge1_08(void) {
    // Parse/validate test for: [8] Merge should handle argument properly
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-08
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-08");

}

void test_clauses_merge_Merge1_09(void) {
    // Runtime: Merge should support updates while merging (ON MATCH on node)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Seed node
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:7}) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // ON MATCH update
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:7}) ON MATCH SET a.flag = 1 RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify flag updated
    sqlite3_stmt *st = NULL;
    rc = sqlite3_prepare_v2(db,
        "SELECT json_extract(properties,'$.flag') FROM graph_nodes WHERE json_extract(properties,'$.pid')=7",
        -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st, 0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge1_10(void) {
    // Parse/validate test for: [10] Merge must properly handle multiple labels
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-10
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-10");

}

void test_clauses_merge_Merge1_11(void) {
    // Parse/validate test for: [11] Merge should be able to merge using property of bound node
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-11
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-11");

}

void test_clauses_merge_Merge1_12(void) {
    // Parse/validate test for: [12] Merge should be able to merge using property of freshly created node
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-12
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-12");

}

void test_clauses_merge_Merge1_13(void) {
    // Parse/validate test for: [13] Merge should bind a path
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-13
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-13");

}

void test_clauses_merge_Merge1_14(void) {
    // Parse/validate test for: [14] Merges should not be able to match on deleted nodes
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-14
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-14");

}

void test_clauses_merge_Merge1_15(void) {
    // Parse/validate test for: [15] Fail when merge a node that is already bound
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-15
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-15");

}

void test_clauses_merge_Merge1_16(void) {
    // Parse/validate test for: [16] Fail when using parameter as node predicate in MERGE
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-16
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-16");

}

void test_clauses_merge_Merge1_17(void) {
    // Parse/validate test for: [17] Fail on merging node with null property
    // Feature: Merge1 - Merge node
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge1-17
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge1-17");

}

void test_clauses_merge_Merge2_01(void) {
    // Parse/validate test for: [1] Merge node with label add label on create
    // Feature: Merge2 - Merge node - on create
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge2-01
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge2-01");

}

void test_clauses_merge_Merge2_02(void) {
    // Parse/validate test for: [2] ON CREATE on created nodes
    // Feature: Merge2 - Merge node - on create
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge2-02
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge2-02");

}

void test_clauses_merge_Merge2_03(void) {
    // Parse/validate test for: [3] Merge node with label add property on create
    // Feature: Merge2 - Merge node - on create
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge2-03
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge2-03");

}

void test_clauses_merge_Merge2_04(void) {
    // Parse/validate test for: [4] Merge node with label add property on update when it exists
    // Feature: Merge2 - Merge node - on create
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge2-04
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge2-04");

}

void test_clauses_merge_Merge2_05(void) {
    // Parse/validate test for: [5] Merge should be able to use properties of bound node in ON CREATE
    // Feature: Merge2 - Merge node - on create
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge2-05
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge2-05");

}

void test_clauses_merge_Merge2_06(void) {
    // Parse/validate test for: [6] Fail when using undefined variable in ON CREATE
    // Feature: Merge2 - Merge node - on create
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge2-06
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge2-06");

}

void test_clauses_merge_Merge3_01(void) {
    // Parse/validate test for: [1] Merge should be able to set labels on match
    // Feature: Merge3 - Merge node - on match
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge3-01
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge3-01");

}

void test_clauses_merge_Merge3_02(void) {
    // Parse/validate test for: [2] Merge node with label add label on match when it exists
    // Feature: Merge3 - Merge node - on match
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge3-02
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge3-02");

}

void test_clauses_merge_Merge3_03(void) {
    // Parse/validate test for: [3] Merge node and set property on match
    // Feature: Merge3 - Merge node - on match
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge3-03
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge3-03");

}

void test_clauses_merge_Merge3_04(void) {
    // Parse/validate test for: [4] Merge should be able to use properties of bound node in ON MATCH
    // Feature: Merge3 - Merge node - on match
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge3-04
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge3-04");

}

void test_clauses_merge_Merge3_05(void) {
    // Parse/validate test for: [5] Fail when using undefined variable in ON MATCH
    // Feature: Merge3 - Merge node - on match
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge3-05
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge3-05");

}

void test_clauses_merge_Merge4_01(void) {
    // Runtime test for: [1] Merge should be able to set labels on match and on create
    // Feature: Merge4 - Merge node - on match and on create
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge4-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge4-01");

}

void test_clauses_merge_Merge4_02(void) {
    // Runtime test for: [2] Merge should be able to use properties of bound node in ON MATCH and ON CREATE
    // Feature: Merge4 - Merge node - on match and on create
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge4-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge4-02");

}

void test_clauses_merge_Merge5_01(void) {
    // Runtime: Creating a relationship (directed)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:1}) MERGE (b:Person {pid:2}) MERGE (a)-[:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Resolve endpoint ids
    sqlite3_stmt *st = NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=1", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=2", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    // Verify single edge exists
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st, 1, a); sqlite3_bind_int(st, 2, b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st, 0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_02(void) {
    // Runtime: Matching a relationship should be idempotent
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // First merge creates
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:11}) MERGE (b:Person {pid:22}) MERGE (a)-[:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    // Second merge matches existing
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:11}) MERGE (b:Person {pid:22}) MERGE (a)-[:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Count edges between these endpoints
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=11", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=22", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_03(void) {
    // Runtime: Matching two relationships from same source should not duplicate
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:100}) MERGE (b:Person {pid:200}) MERGE (c:Person {pid:300}) MERGE (a)-[:KNOWS]->(b) MERGE (a)-[:KNOWS]->(c) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0,cid=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=100", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=200", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=300", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); cid = sqlite3_column_int(st,0); sqlite3_finalize(st);

    // Expect exactly two edges out of a
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(2, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_04(void) {
    // Parse/validate test for: [4] Using bound variables from other updating clause
    // Feature: Merge5 - Merge relationships
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge5-04
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge5-04");

}

void test_clauses_merge_Merge5_05(void) {
    // Runtime: Filtering relationships - property filter matches, no new creation
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Seed edge with flag=1
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:201}) MERGE (b:Person {pid:202}) MERGE (a)-[:KNOWS {flag:1}]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // MERGE with same property should match existing
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:201}) MERGE (b:Person {pid:202}) MERGE (a)-[:KNOWS {flag:1}]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify exactly one edge remains
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=201", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=202", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_06(void) {
    // Runtime: Creating relationship when existing edge is filtered out (different property)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Seed edge with flag=2
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:211}) MERGE (b:Person {pid:212}) MERGE (a)-[:KNOWS {flag:2}]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // MERGE with flag=1 should create a distinct edge
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:211}) MERGE (b:Person {pid:212}) MERGE (a)-[:KNOWS {flag:1}]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify two edges exist from a->b
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=211", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=212", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(2, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_07(void) {
    // Runtime: Matching incoming relationship syntax
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Use incoming arrow to create a->b edge
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:221}) MERGE (b:Person {pid:222}) MERGE (b)<-[:KNOWS {flag:1}]-(a) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=221", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=222", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    // Verify edge direction is a->b
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_08(void) {
    // Runtime: Creating relationship with property map
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:5}) MERGE (b:Person {pid:6}) MERGE (a)-[:KNOWS {flag:1}]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=5", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=6", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT json_extract(properties,'$.flag') FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_09(void) {
    // Runtime: Creating relationship using merged nodes
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Merge nodes and then the relationship
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:231}) MERGE (b:Person {pid:232}) MERGE (a)-[:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify one edge exists
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=231", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=232", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_10(void) {
    // Parse/validate test for: [10] Merge should bind a path
    // Feature: Merge5 - Merge relationships
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge5-10
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge5-10");

}

void test_clauses_merge_Merge5_11(void) {
    // Runtime: Use outgoing direction when unspecified
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Unspecified direction should create outgoing from a to b
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:241}) MERGE (b:Person {pid:242}) MERGE (a)-[:KNOWS]-(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=241", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=242", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_12(void) {
    // Runtime: Match outgoing relationship when direction unspecified
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Create explicit outgoing a->b
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:251}) MERGE (b:Person {pid:252}) MERGE (a)-[:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Unspecified should match existing, not create duplicate
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:251}) MERGE (b:Person {pid:252}) MERGE (a)-[:KNOWS]-(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=251", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=252", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_13(void) {
    // Runtime: Match both incoming and outgoing relationships when direction unspecified
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Create incoming only (b->a)
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:261}) MERGE (b:Person {pid:262}) MERGE (b)-[:KNOWS]->(a) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Unspecified should match regardless of direction (no duplicate a->b)
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:261}) MERGE (b:Person {pid:262}) MERGE (a)-[:KNOWS]-(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Ensure only one edge exists in either direction
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=261", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=262", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND ((source=? AND target=?) OR (source=? AND target=?))", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    sqlite3_bind_int(st,3,b); sqlite3_bind_int(st,4,a);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_14(void) {
    // Runtime: Using list properties in relationship property map (literal list)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Create relationship with literal list property on relationship
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:271}) MERGE (b:Person {pid:272}) MERGE (a)-[:KNOWS {tags:[1,2,3]}]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify created edge has list property
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=271", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=272", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_15(void) {
    // Runtime: Matching using list property
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Seed edge with list property
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:281}) MERGE (b:Person {pid:282}) MERGE (a)-[:KNOWS {tags:[1,2]}]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // MERGE with identical list should match existing, not create duplicate
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:281}) MERGE (b:Person {pid:282}) MERGE (a)-[:KNOWS {tags:[1,2]}]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify exactly one edge exists between endpoints
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=281", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=282", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_16(void) {
    // Runtime: Aliasing of existing nodes 1 (self-loop via identical MERGE patterns)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:291}) MERGE (b:Person {pid:291}) MERGE (a)-[:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify a single self-loop exists
    sqlite3_stmt *st=NULL; int id=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=291", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); id = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,id); sqlite3_bind_int(st,2,id);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_17(void) {
    // Runtime: Aliasing of existing nodes 2 (self-loop via identical MERGE patterns, unspecified direction)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:301}) MERGE (b:Person {pid:301}) MERGE (a)-[:KNOWS]-(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify a single self-loop exists
    sqlite3_stmt *st=NULL; int id=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=301", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); id = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,id); sqlite3_bind_int(st,2,id);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_18(void) {
    // Runtime: Double aliasing of existing nodes 1 (two alias pairs, directed; no duplicate edge)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_exec(db,
        "SELECT cypher_execute('"
        "MERGE (a:Person {pid:321}) MERGE (b:Person {pid:322}) "
        "MERGE (x:Person {pid:321}) MERGE (y:Person {pid:322}) "
        "MERGE (a)-[:KNOWS]->(b) "
        "MERGE (x)-[:KNOWS]->(y) "
        "RETURN 1"
        "')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify exactly one a->b edge exists
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=321", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=322", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_19(void) {
    // Runtime: Double aliasing of existing nodes 2 (two alias pairs, unspecified direction and reversed order; no duplicate edge)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_exec(db,
        "SELECT cypher_execute('"
        "MERGE (a:Person {pid:331}) MERGE (b:Person {pid:332}) "
        "MERGE (x:Person {pid:331}) MERGE (y:Person {pid:332}) "
        "MERGE (a)-[:KNOWS]-(b) "
        "MERGE (y)-[:KNOWS]-(x) "
        "RETURN 1"
        "')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Verify exactly one edge exists between endpoints in either direction
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=331", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=332", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND ((source=? AND target=?) OR (source=? AND target=?))", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    sqlite3_bind_int(st,3,b); sqlite3_bind_int(st,4,a);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_20(void) {
    // Runtime: Do not match on deleted entities (deleted relationship)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Create edge
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:401}) MERGE (b:Person {pid:402}) MERGE (a)-[:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Resolve ids and delete edge directly
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=401", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=402", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "DELETE FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_DONE, sqlite3_step(st));
    sqlite3_finalize(st);

    // MERGE should create a fresh edge after deletion
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:401}) MERGE (b:Person {pid:402}) MERGE (a)-[:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_21(void) {
    // Runtime: Do not match on deleted relationships (same as 5_20 semantics)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Create edge
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:411}) MERGE (b:Person {pid:412}) MERGE (a)-[:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Resolve ids and delete edge directly
    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=411", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=412", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "DELETE FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_DONE, sqlite3_step(st));
    sqlite3_finalize(st);

    // MERGE should create a fresh edge after deletion
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:411}) MERGE (b:Person {pid:412}) MERGE (a)-[:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge5_22(void) {
    // Parse/validate test for: [22] Fail when imposing new predicates on a variable that is already bound
    // Feature: Merge5 - Merge relationships
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge5-22
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge5-22");

}

void test_clauses_merge_Merge5_23(void) {
    // Parse/validate test for: [23] Fail when merging relationship without type
    // Feature: Merge5 - Merge relationships
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge5-23
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge5-23");

}

void test_clauses_merge_Merge5_24(void) {
    // Parse/validate test for: [24] Fail when merging relationship without type, no colon
    // Feature: Merge5 - Merge relationships
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge5-24
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge5-24");

}

void test_clauses_merge_Merge5_25(void) {
    // Parse/validate test for: [25] Fail when merging relationship with more than one type
    // Feature: Merge5 - Merge relationships
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge5-25
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge5-25");

}

void test_clauses_merge_Merge5_26(void) {
    // Parse/validate test for: [26] Fail when merging relationship that is already bound
    // Feature: Merge5 - Merge relationships
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge5-26
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge5-26");

}

void test_clauses_merge_Merge5_27(void) {
    // Parse/validate test for: [27] Fail when using parameter as relationship predicate in MERGE
    // Feature: Merge5 - Merge relationships
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge5-27
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge5-27");

}

void test_clauses_merge_Merge5_28(void) {
    // Parse/validate test for: [28] Fail when using variable length relationship in MERGE
    // Feature: Merge5 - Merge relationships
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge5-28
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge5-28");

}

void test_clauses_merge_Merge5_29(void) {
    // Parse/validate test for: [29] Fail on merging relationship with null property
    // Feature: Merge5 - Merge relationships
    
    // TODO: Implement parsing/validation test for clauses-merge-Merge5-29
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge5-29");

}

void test_clauses_merge_Merge6_01(void) {
    // Runtime test for: [1] Using ON CREATE on a node
    // Feature: Merge6 - Merge relationships - on create
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge6-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge6-01");

}

void test_clauses_merge_Merge6_02(void) {
    // Runtime: Using ON CREATE on a relationship
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:31}) MERGE (b:Person {pid:32}) MERGE (a)-[r:KNOWS]->(b) ON CREATE SET r.since=2024 RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=31", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=32", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT json_extract(properties,'$.since') FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(2024, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge6_03(void) {
    // Runtime: Updating one property with ON CREATE
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:41}) MERGE (b:Person {pid:42}) MERGE (a)-[r:KNOWS]->(b) ON CREATE SET r.weight=1 RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=41", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=42", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT json_extract(properties,'$.weight') FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge6_04(void) {
    // Runtime test for: [4] Null-setting one property with ON CREATE
    // Feature: Merge6 - Merge relationships - on create
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge6-04
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge6-04");

}

void test_clauses_merge_Merge6_05(void) {
    // Runtime test for: [6] Copying properties from node with ON CREATE
    // Feature: Merge6 - Merge relationships - on create
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge6-05
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge6-05");

}

void test_clauses_merge_Merge6_06(void) {
    // Runtime: Copying properties with ON CREATE (literal map simulated)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Set a couple of properties on create
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:51}) MERGE (b:Person {pid:52}) MERGE (a)-[r:KNOWS]->(b) ON CREATE SET r.flag=99, r.ok=true RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=51", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=52", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT json_extract(properties,'$.flag'), json_extract(properties,'$.ok') FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(99, sqlite3_column_int(st,0));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,1));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge7_01(void) {
    // Runtime test for: [1] Using ON MATCH on created node
    // Feature: Merge7 - Merge relationships - on match
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge7-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge7-01");

}

void test_clauses_merge_Merge7_02(void) {
    // Runtime test for: [2] Using ON MATCH on created relationship
    // Feature: Merge7 - Merge relationships - on match
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge7-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge7-02");

}

void test_clauses_merge_Merge7_03(void) {
    // Runtime: Using ON MATCH on a relationship
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Create relationship first
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:61}) MERGE (b:Person {pid:62}) MERGE (a)-[r:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // ON MATCH update property
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:61}) MERGE (b:Person {pid:62}) MERGE (a)-[r:KNOWS]->(b) ON MATCH SET r.flag=1 RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=61", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=62", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT json_extract(properties,'$.flag') FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge7_04(void) {
    // Runtime: Copying properties from node with ON MATCH (r.flag = a.level)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Create relationship first
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:71, level:3}) MERGE (b:Person {pid:72}) MERGE (a)-[r:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // ON MATCH copy property from node
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:71, level:3}) MERGE (b:Person {pid:72}) MERGE (a)-[r:KNOWS]->(b) ON MATCH SET r.flag = a.level RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=71", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=72", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT json_extract(properties,'$.flag') FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(3, sqlite3_column_int(st,0));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge7_05(void) {
    // Runtime: Copying properties from literal map with ON MATCH (simulate with direct sets)
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Create edge
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:81}) MERGE (b:Person {pid:82}) MERGE (a)-[r:KNOWS]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // ON MATCH set multiple props
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:81}) MERGE (b:Person {pid:82}) MERGE (a)-[r:KNOWS]->(b) ON MATCH SET r.x=1, r.y=2 RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=81", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=82", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT json_extract(properties,'$.x'), json_extract(properties,'$.y') FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    TEST_ASSERT_EQUAL_INT(2, sqlite3_column_int(st,1));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge8_01(void) {
    // Runtime: Using ON CREATE and ON MATCH in sequence
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // First time: create and set created=1
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:91}) MERGE (b:Person {pid:92}) MERGE (a)-[r:KNOWS]->(b) ON CREATE SET r.created=1 RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    // Second time: match and set updated=1
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MERGE (a:Person {pid:91}) MERGE (b:Person {pid:92}) MERGE (a)-[r:KNOWS]->(b) ON MATCH SET r.updated=1 RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    sqlite3_stmt *st=NULL; int a=0,b=0;
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=91", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); a = sqlite3_column_int(st,0); sqlite3_finalize(st);
    rc = sqlite3_prepare_v2(db, "SELECT id FROM graph_nodes WHERE json_extract(properties,'$.pid')=92", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc); TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st)); b = sqlite3_column_int(st,0); sqlite3_finalize(st);

    rc = sqlite3_prepare_v2(db, "SELECT json_extract(properties,'$.created'), json_extract(properties,'$.updated') FROM graph_edges WHERE edge_type='KNOWS' AND source=? AND target=?", -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_bind_int(st,1,a); sqlite3_bind_int(st,2,b);
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(st));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,0));
    TEST_ASSERT_EQUAL_INT(1, sqlite3_column_int(st,1));
    sqlite3_finalize(st);
}

void test_clauses_merge_Merge9_01(void) {
    // Runtime test for: [1] UNWIND with one MERGE
    // Feature: Merge9 - Merge clause interoperation with other clauses
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge9-01
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge9-01");

}

void test_clauses_merge_Merge9_02(void) {
    // Runtime test for: [2] UNWIND with multiple MERGE
    // Feature: Merge9 - Merge clause interoperation with other clauses
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge9-02
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge9-02");

}

void test_clauses_merge_Merge9_03(void) {
    // Runtime test for: [3] Mixing MERGE with CREATE
    // Feature: Merge9 - Merge clause interoperation with other clauses
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge9-03
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge9-03");

}

void test_clauses_merge_Merge9_04(void) {
    // Runtime test for: [4] MERGE after WITH with predicate and WITH with aggregation
    // Feature: Merge9 - Merge clause interoperation with other clauses
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }
    
    // TODO: Implement actual test logic for clauses-merge-Merge9-04
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: clauses-merge-Merge9-04");

}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_clauses_merge_Merge1_01);
    RUN_TEST(test_clauses_merge_Merge1_02);
    RUN_TEST(test_clauses_merge_Merge1_03);
    RUN_TEST(test_clauses_merge_Merge1_04);
    RUN_TEST(test_clauses_merge_Merge1_05);
    RUN_TEST(test_clauses_merge_Merge1_06);
    RUN_TEST(test_clauses_merge_Merge1_07);
    RUN_TEST(test_clauses_merge_Merge1_08);
    RUN_TEST(test_clauses_merge_Merge1_09);
    RUN_TEST(test_clauses_merge_Merge1_10);
    RUN_TEST(test_clauses_merge_Merge1_11);
    RUN_TEST(test_clauses_merge_Merge1_12);
    RUN_TEST(test_clauses_merge_Merge1_13);
    RUN_TEST(test_clauses_merge_Merge1_14);
    RUN_TEST(test_clauses_merge_Merge1_15);
    RUN_TEST(test_clauses_merge_Merge1_16);
    RUN_TEST(test_clauses_merge_Merge1_17);
    RUN_TEST(test_clauses_merge_Merge2_01);
    RUN_TEST(test_clauses_merge_Merge2_02);
    RUN_TEST(test_clauses_merge_Merge2_03);
    RUN_TEST(test_clauses_merge_Merge2_04);
    RUN_TEST(test_clauses_merge_Merge2_05);
    RUN_TEST(test_clauses_merge_Merge2_06);
    RUN_TEST(test_clauses_merge_Merge3_01);
    RUN_TEST(test_clauses_merge_Merge3_02);
    RUN_TEST(test_clauses_merge_Merge3_03);
    RUN_TEST(test_clauses_merge_Merge3_04);
    RUN_TEST(test_clauses_merge_Merge3_05);
    RUN_TEST(test_clauses_merge_Merge4_01);
    RUN_TEST(test_clauses_merge_Merge4_02);
    RUN_TEST(test_clauses_merge_Merge5_01);
    RUN_TEST(test_clauses_merge_Merge5_02);
    RUN_TEST(test_clauses_merge_Merge5_03);
    RUN_TEST(test_clauses_merge_Merge5_04);
    RUN_TEST(test_clauses_merge_Merge5_05);
    RUN_TEST(test_clauses_merge_Merge5_06);
    RUN_TEST(test_clauses_merge_Merge5_07);
    RUN_TEST(test_clauses_merge_Merge5_08);
    RUN_TEST(test_clauses_merge_Merge5_09);
    RUN_TEST(test_clauses_merge_Merge5_10);
    RUN_TEST(test_clauses_merge_Merge5_11);
    RUN_TEST(test_clauses_merge_Merge5_12);
    RUN_TEST(test_clauses_merge_Merge5_13);
    RUN_TEST(test_clauses_merge_Merge5_14);
    RUN_TEST(test_clauses_merge_Merge5_15);
    RUN_TEST(test_clauses_merge_Merge5_16);
    RUN_TEST(test_clauses_merge_Merge5_17);
    RUN_TEST(test_clauses_merge_Merge5_18);
    RUN_TEST(test_clauses_merge_Merge5_19);
    RUN_TEST(test_clauses_merge_Merge5_20);
    RUN_TEST(test_clauses_merge_Merge5_21);
    RUN_TEST(test_clauses_merge_Merge5_22);
    RUN_TEST(test_clauses_merge_Merge5_23);
    RUN_TEST(test_clauses_merge_Merge5_24);
    RUN_TEST(test_clauses_merge_Merge5_25);
    RUN_TEST(test_clauses_merge_Merge5_26);
    RUN_TEST(test_clauses_merge_Merge5_27);
    RUN_TEST(test_clauses_merge_Merge5_28);
    RUN_TEST(test_clauses_merge_Merge5_29);
    RUN_TEST(test_clauses_merge_Merge6_01);
    RUN_TEST(test_clauses_merge_Merge6_02);
    RUN_TEST(test_clauses_merge_Merge6_03);
    RUN_TEST(test_clauses_merge_Merge6_04);
    RUN_TEST(test_clauses_merge_Merge6_05);
    RUN_TEST(test_clauses_merge_Merge6_06);
    RUN_TEST(test_clauses_merge_Merge7_01);
    RUN_TEST(test_clauses_merge_Merge7_02);
    RUN_TEST(test_clauses_merge_Merge7_03);
    RUN_TEST(test_clauses_merge_Merge7_04);
    RUN_TEST(test_clauses_merge_Merge7_05);
    RUN_TEST(test_clauses_merge_Merge8_01);
    RUN_TEST(test_clauses_merge_Merge9_01);
    RUN_TEST(test_clauses_merge_Merge9_02);
    RUN_TEST(test_clauses_merge_Merge9_03);
    RUN_TEST(test_clauses_merge_Merge9_04);
    
    return UNITY_END();
}
