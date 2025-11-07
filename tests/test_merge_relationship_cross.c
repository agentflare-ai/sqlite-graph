/* Cross-area test: MATCH interop with MERGE relationship */
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

void
test_match_then_merge_relationship (void)
{
  int rc;
  sqlite3_stmt *stmt = NULL;
  rc = sqlite3_exec (db, "CREATE VIRTUAL TABLE graph USING graph()", NULL,
                     NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  /* Create nodes via CREATE (use numeric props to avoid string lexer path) */
  rc = sqlite3_exec (
      db,
      "SELECT cypher_execute('CREATE (a:Person {pid:1}), (b:Person {pid:2})')",
      NULL, NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  /* Match and merge relationship */
  rc = sqlite3_exec (db,
                     "SELECT cypher_execute('MATCH (a:Person {pid:1}), "
                     "(b:Person {pid:2}) MERGE (a)-[:FRIEND]->(b) RETURN 1')",
                     NULL, NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  /* Verify only one edge */
  rc = sqlite3_prepare_v2 (
      db, "SELECT COUNT(*) FROM graph_edges WHERE edge_type='FRIEND'", -1,
      &stmt, NULL);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);
  TEST_ASSERT_EQUAL (SQLITE_ROW, sqlite3_step (stmt));
  TEST_ASSERT_EQUAL_INT (1, sqlite3_column_int (stmt, 0));
  sqlite3_finalize (stmt);
}

int
main (void)
{
  UNITY_BEGIN ();
  RUN_TEST (test_match_then_merge_relationship);
  return UNITY_END ();
}
