/* MERGE relationship semantics: directed vs undirected */
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
get_count (const char *sql)
{
  sqlite3_stmt *stmt = NULL;
  int rc = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);
  TEST_ASSERT_EQUAL (SQLITE_ROW, sqlite3_step (stmt));
  int c = sqlite3_column_int (stmt, 0);
  sqlite3_finalize (stmt);
  return c;
}

void
test_merge_relationship_directed_creates_edge (void)
{
  int rc;
  rc = sqlite3_exec (db, "CREATE VIRTUAL TABLE graph USING graph()", NULL,
                     NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  rc = sqlite3_exec (
      db,
      "SELECT cypher_execute('CREATE (a:Person {pid:1}), (b:Person {pid:2})')",
      NULL, NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  rc = sqlite3_exec (db,
                     "SELECT cypher_execute('MATCH (a:Person {pid:1}), "
                     "(b:Person {pid:2}) MERGE (a)-[:FRIEND]->(b) RETURN 1')",
                     NULL, NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  TEST_ASSERT_EQUAL_INT (
      1,
      get_count ("SELECT COUNT(*) FROM graph_edges WHERE edge_type='FRIEND'"));
}

void
test_merge_relationship_undirected_matches_reverse (void)
{
  int rc;
  rc = sqlite3_exec (db, "CREATE VIRTUAL TABLE graph USING graph()", NULL,
                     NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  rc = sqlite3_exec (
      db,
      "SELECT cypher_execute('CREATE (a:Person {pid:3}), (b:Person {pid:4})')",
      NULL, NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  /* Pre-insert reverse edge (b)->(a) */
  sqlite3_stmt *stmt = NULL;
  sqlite3_int64 ida = 0, idb = 0;
  TEST_ASSERT_EQUAL (SQLITE_OK,
                     sqlite3_prepare_v2 (db,
                                         "SELECT id FROM graph_nodes WHERE "
                                         "json_extract(properties,'$.pid')=3",
                                         -1, &stmt, NULL));
  TEST_ASSERT_EQUAL (SQLITE_ROW, sqlite3_step (stmt));
  ida = sqlite3_column_int64 (stmt, 0);
  sqlite3_finalize (stmt);
  TEST_ASSERT_EQUAL (SQLITE_OK,
                     sqlite3_prepare_v2 (db,
                                         "SELECT id FROM graph_nodes WHERE "
                                         "json_extract(properties,'$.pid')=4",
                                         -1, &stmt, NULL));
  TEST_ASSERT_EQUAL (SQLITE_ROW, sqlite3_step (stmt));
  idb = sqlite3_column_int64 (stmt, 0);
  sqlite3_finalize (stmt);

  char *zIns = sqlite3_mprintf (
      "INSERT INTO graph_edges(source,target,edge_type,weight,properties) "
      "VALUES(%lld,%lld,'KNOWS',1.0,'{}')",
      idb, ida);
  TEST_ASSERT_EQUAL (SQLITE_OK,
                     sqlite3_exec (db, zIns, NULL, NULL, &error_msg));
  sqlite3_free (zIns);

  /* Undirected MERGE should match existing reverse */
  rc = sqlite3_exec (db,
                     "SELECT cypher_execute('MATCH (a:Person {pid:3}), "
                     "(b:Person {pid:4}) MERGE (a)-[:KNOWS]-(b) RETURN 1')",
                     NULL, NULL, &error_msg);
  TEST_ASSERT_EQUAL (SQLITE_OK, rc);

  TEST_ASSERT_EQUAL_INT (
      1,
      get_count ("SELECT COUNT(*) FROM graph_edges WHERE edge_type='KNOWS'"));
}

int
main (void)
{
  UNITY_BEGIN ();
  RUN_TEST (test_merge_relationship_directed_creates_edge);
  RUN_TEST (test_merge_relationship_undirected_matches_reverse);
  return UNITY_END ();
}
