/* MERGE node: create vs match and ON CREATE/ON MATCH */
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

static sqlite3 *db = NULL;
static char *error_msg = NULL;

void
setUp (void)
{
  int rc = sqlite3_open (":memory:", &db);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);
  sqlite3_enable_load_extension (db, 1);
  rc = sqlite3_load_extension (db, "../build/libgraph",
                               "sqlite3_graph_init", &error_msg);
  if (rc != SQLITE_OK)
    {
      printf ("Failed to load graph extension: %s\n", error_msg);
      sqlite3_free (error_msg);
      error_msg = NULL;
    }
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);
}

void
tearDown (void)
{
  if (error_msg)
    {
      sqlite3_free (error_msg);
      error_msg = NULL;
    }
  if (db)
    {
      sqlite3_close (db);
      db = NULL;
    }
}

static int
get_node_count_pid (int pid)
{
  sqlite3_stmt *stmt = NULL;
  int rc = sqlite3_prepare_v2 (db,
                               "SELECT COUNT(*) FROM graph_nodes WHERE "
                               "json_extract(properties,'$.pid')=?",
                               -1, &stmt, NULL);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);
  sqlite3_bind_int (stmt, 1, pid);
  TEST_ASSERT_EQUAL (SQLITE_ROW, sqlite3_step (stmt));
  int c = sqlite3_column_int (stmt, 0);
  sqlite3_finalize (stmt);
  return c;
}

static int
get_node_flag_pid (int pid)
{
  sqlite3_stmt *stmt = NULL;
  int rc = sqlite3_prepare_v2 (
      db,
      "SELECT json_extract(properties,'$.flag') FROM graph_nodes WHERE "
      "json_extract(properties,'$.pid')=?",
      -1, &stmt, NULL);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);
  sqlite3_bind_int (stmt, 1, pid);
  if (sqlite3_step (stmt) != SQLITE_ROW)
    {
      sqlite3_finalize (stmt);
      return -1;
    }
  int v = sqlite3_column_int (stmt, 0);
  sqlite3_finalize (stmt);
  return v;
}

void
test_merge_node_create_and_match (void)
{
  int rc;
  rc = sqlite3_exec (db, "CREATE VIRTUAL TABLE graph USING graph()", NULL,
                     NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  rc = sqlite3_exec (
      db, "SELECT cypher_execute('MERGE (a:Person {pid:10}) RETURN 1')", NULL,
      NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  {
    int cnt = get_node_count_pid (10);
    TEST_ASSERT_EQUAL_INT (1, cnt);
  }

  rc = sqlite3_exec (db,
                     "SELECT cypher_execute('MERGE (a:Person {pid:10}) ON "
                     "MATCH SET a.flag = 1 RETURN 1')",
                     NULL, NULL, &error_msg);
  if (rc != SQLITE_OK && error_msg)
    {
      printf ("SQL error: %s\n", error_msg);
    }
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);
  TEST_ASSERT_EQUAL_INT (1, get_node_flag_pid (10));
}

int
main (void)
{
  UNITY_BEGIN ();
  RUN_TEST (test_merge_node_create_and_match);
  return UNITY_END ();
}
