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

static void seed_two_nodes(void) {
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

void test_graph_count_matches_seeded_nodes(void) {
  seed_two_nodes();
  sqlite3_stmt *stmt = NULL;
  TEST_ASSERT_EQUAL(SQLITE_OK,
                    sqlite3_prepare_v2(db, "SELECT graph_count_nodes()", -1, &stmt, NULL));
  TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
  TEST_ASSERT_EQUAL_INT(2, sqlite3_column_int(stmt, 0));
  sqlite3_finalize(stmt);
}

void test_cypher_execute_returns_json_payload(void) {
  seed_two_nodes();
  sqlite3_stmt *stmt = NULL;
  TEST_ASSERT_EQUAL(SQLITE_OK,
                    sqlite3_prepare_v2(db,
                                       "SELECT cypher_execute('MATCH (n) RETURN n')",
                                       -1, &stmt, NULL));
  TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
  const unsigned char *payload = sqlite3_column_text(stmt, 0);
  TEST_ASSERT_NOT_NULL(payload);
  TEST_ASSERT_TRUE(strlen((const char *)payload) > 0);
  sqlite3_finalize(stmt);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_graph_count_matches_seeded_nodes);
  RUN_TEST(test_cypher_execute_returns_json_payload);
  return UNITY_END();
}
