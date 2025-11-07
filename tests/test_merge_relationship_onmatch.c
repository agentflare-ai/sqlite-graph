/* MERGE relationship ON MATCH property update */
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
  /* Pass base name without suffix; SQLite will append the platform extension. */
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

static double
get_score (void)
{
  sqlite3_stmt *stmt = NULL;
  int rc = sqlite3_prepare_v2 (db,
                               "SELECT json_extract(properties,'$.score') "
                               "FROM graph_edges WHERE edge_type='LIKES'",
                               -1, &stmt, NULL);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);
  if (sqlite3_step (stmt) != SQLITE_ROW)
    {
      sqlite3_finalize (stmt);
      return -1.0;
    }
  double v = sqlite3_column_double (stmt, 0);
  sqlite3_finalize (stmt);
  return v;
}

void
test_merge_relationship_on_match_updates (void)
{
  int rc;
  rc = sqlite3_exec (db, "CREATE VIRTUAL TABLE graph USING graph()", NULL,
                     NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  rc = sqlite3_exec (db,
                     "SELECT cypher_execute('CREATE (a:Person {pid:21}), "
                     "(b:Person {pid:22})')",
                     NULL, NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  /* Create initial relationship with score 1 */
  rc = sqlite3_exec (
      db,
      "SELECT cypher_execute('MATCH (a:Person {pid:21}), (b:Person {pid:22}) "
      "CREATE (a)-[:LIKES {score:1}]->(b) RETURN 1')",
      NULL, NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  /* MERGE with ON MATCH SET to update score */
  rc = sqlite3_exec (
      db,
      "SELECT cypher_execute('MATCH (a:Person {pid:21}), (b:Person {pid:22}) "
      "MERGE (a)-[r:LIKES {score:1}]->(b) ON MATCH SET r.score = 2 RETURN 1')",
      NULL, NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  TEST_ASSERT_DOUBLE_WITHIN (0.001, 2.0, get_score ());
}

int
main (void)
{
  UNITY_BEGIN ();
  RUN_TEST (test_merge_relationship_on_match_updates);
  return UNITY_END ();
}
