#include <string.h>
#include <unity.h>

#include "test_common.h"

static sqlite3 *db = NULL;
static char *err_msg = NULL;

void setUp(void) {
  TEST_ASSERT_EQUAL(SQLITE_OK, test_prepare_graph(&db, &err_msg));
}

void tearDown(void) {
  test_cleanup(&db, &err_msg);
}

static int fetch_int(sqlite3 *db, const char *sql) {
  sqlite3_stmt *stmt = NULL;
  TEST_ASSERT_EQUAL(SQLITE_OK, sqlite3_prepare_v2(db, sql, -1, &stmt, NULL));
  TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
  int value = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return value;
}

static void seed_nodes_for_edge_test(void) {
  TEST_ASSERT_EQUAL(SQLITE_OK,
                    test_exec_sql(db,
                                  "SELECT graph_node_add(1, '{\"name\":\"Alice\"}')",
                                  &err_msg));
  test_clear_error(&err_msg);
  TEST_ASSERT_EQUAL(SQLITE_OK,
                    test_exec_sql(db,
                                  "SELECT graph_node_add(2, '{\"name\":\"Bob\"}')",
                                  &err_msg));
  test_clear_error(&err_msg);
}

void test_graph_node_add_and_count_functions(void) {
  TEST_ASSERT_EQUAL(SQLITE_OK,
                    test_exec_sql(db,
                                  "SELECT graph_node_add(100, '{\"label\":\"Person\"}')",
                                  &err_msg));
  test_clear_error(&err_msg);
  TEST_ASSERT_EQUAL_INT(1, fetch_int(db, "SELECT graph_count_nodes()"));
}

void test_graph_edge_add_tracks_relationships(void) {
  seed_nodes_for_edge_test();
  TEST_ASSERT_EQUAL(SQLITE_OK,
                    test_exec_sql(db,
                                  "SELECT graph_edge_add(1, 2, 'KNOWS', '{\"since\":2024}')",
                                  &err_msg));
  test_clear_error(&err_msg);
  TEST_ASSERT_EQUAL_INT(1, fetch_int(db, "SELECT graph_count_edges()"));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_graph_node_add_and_count_functions);
  RUN_TEST(test_graph_edge_add_tracks_relationships);
  return UNITY_END();
}
