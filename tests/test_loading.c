#include <unity.h>

#include "test_common.h"

static sqlite3 *db = NULL;
static char *err_msg = NULL;

void setUp(void) {
  TEST_ASSERT_EQUAL(SQLITE_OK, test_open_db(&db));
}

void tearDown(void) {
  test_cleanup(&db, &err_msg);
}

void test_extension_can_be_enabled_and_loaded(void) {
  TEST_ASSERT_EQUAL(SQLITE_OK, test_enable_extensions(db));
  TEST_ASSERT_EQUAL(SQLITE_OK, test_load_graph_extension(db, &err_msg));
  test_clear_error(&err_msg);
}

void test_graph_virtual_table_can_be_created(void) {
  TEST_ASSERT_EQUAL(SQLITE_OK, test_enable_extensions(db));
  TEST_ASSERT_EQUAL(SQLITE_OK, test_load_graph_extension(db, &err_msg));
  TEST_ASSERT_EQUAL(SQLITE_OK, test_create_graph_vtab(db, &err_msg));
  test_clear_error(&err_msg);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_extension_can_be_enabled_and_loaded);
  RUN_TEST(test_graph_virtual_table_can_be_created);
  return UNITY_END();
}
