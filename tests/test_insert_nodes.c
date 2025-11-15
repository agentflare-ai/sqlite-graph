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
  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  TEST_ASSERT_EQUAL(SQLITE_OK, rc);
  TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
  int value = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return value;
}

void test_insert_single_node_updates_count(void) {
  TEST_ASSERT_EQUAL(SQLITE_OK,
                    test_exec_sql(db,
                                  "SELECT graph_node_add(1, '{\"name\":\"Alice\"}')",
                                  &err_msg));
  test_clear_error(&err_msg);
  TEST_ASSERT_EQUAL_INT(1, fetch_int(db, "SELECT graph_count_nodes()"));
}

void test_insert_multiple_nodes_are_tracked(void) {
  for (int i = 0; i < 3; ++i) {
    char sql[128];
    snprintf(sql, sizeof(sql),
             "SELECT graph_node_add(%d, '{\"idx\":%d}')", i + 1, i);
    TEST_ASSERT_EQUAL(SQLITE_OK, test_exec_sql(db, sql, &err_msg));
    test_clear_error(&err_msg);
  }
  TEST_ASSERT_EQUAL_INT(3, fetch_int(db, "SELECT graph_count_nodes()"));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_insert_single_node_updates_count);
  RUN_TEST(test_insert_multiple_nodes_are_tracked);
  return UNITY_END();
}
