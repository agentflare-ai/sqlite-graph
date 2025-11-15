#ifndef PLACEHOLDER_SUITE
#error "PLACEHOLDER_SUITE must be defined before including test_placeholder_suite.inc"
#endif

#include <unity.h>

#include "test_common.h"

static sqlite3 *placeholder_db = NULL;
static char *placeholder_err = NULL;

void setUp(void) {
  TEST_ASSERT_EQUAL(SQLITE_OK, test_prepare_graph(&placeholder_db, &placeholder_err));
}

void tearDown(void) {
  test_cleanup(&placeholder_db, &placeholder_err);
}

void test_placeholder_suite(void) {
  TEST_IGNORE_MESSAGE("TODO: implement " PLACEHOLDER_SUITE " scenarios");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_placeholder_suite);
  return UNITY_END();
}

#undef PLACEHOLDER_SUITE
