/* MERGE relationship ON MATCH expression copy (r.prop = a.prop) */
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

static sqlite3 *db = NULL;
static char *error_msg = NULL;

void setUp(void) {
    int rc = sqlite3_open(":memory:", &db);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    sqlite3_enable_load_extension(db, 1);
    rc = sqlite3_load_extension(db, "../build/libgraph", "sqlite3_graph_init", &error_msg);
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

static int get_flag(void) {
    sqlite3_stmt *st = NULL;
    int rc = sqlite3_prepare_v2(db,
        "SELECT json_extract(properties,'$.flag') FROM graph_edges WHERE edge_type='TEAMMATE'",
        -1, &st, NULL);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    if (sqlite3_step(st) != SQLITE_ROW) {
        sqlite3_finalize(st);
        return -1;
    }
    int v = sqlite3_column_int(st, 0);
    sqlite3_finalize(st);
    return v;
}

void test_merge_onmatch_expression_copy(void) {
    int rc;
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    /* Seed nodes with source property to copy */
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('CREATE (a:Person {pid:170, level:9}), (b:Person {pid:171})')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    /* Pre-create relationship so MERGE hits ON MATCH */
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MATCH (a:Person {pid:170}), (b:Person {pid:171}) CREATE (a)-[:TEAMMATE]->(b) RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    /* MERGE and copy a.level into r.flag via expression */
    rc = sqlite3_exec(db,
        "SELECT cypher_execute('MATCH (a:Person {pid:170}), (b:Person {pid:171}) MERGE (a)-[r:TEAMMATE]->(b) ON MATCH SET r.flag = a.level RETURN 1')",
        NULL, NULL, &error_msg);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    TEST_ASSERT_EQUAL_INT(9, get_flag());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_merge_onmatch_expression_copy);
    return UNITY_END();
}
