#include <sqlite3.h>
#include <string.h>
#include <unity.h>

#include "test_common.h"

static sqlite3 *db = NULL;
static char *err_msg = NULL;

static void exec_cypher(const char *query) {
  sqlite3_stmt *stmt = NULL;
  TEST_ASSERT_EQUAL(SQLITE_OK,
                    sqlite3_prepare_v2(db, "SELECT cypher_execute(?)", -1,
                                       &stmt, NULL));
  TEST_ASSERT_EQUAL(SQLITE_OK,
                    sqlite3_bind_text(stmt, 1, query, -1, SQLITE_TRANSIENT));
  int rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    const char *detail = sqlite3_errmsg(db);
    sqlite3_finalize(stmt);
    TEST_FAIL_MESSAGE(detail ? detail : "cypher_execute did not return a row");
  }
  sqlite3_finalize(stmt);
}

static int fetch_int_query(const char *sql) {
  sqlite3_stmt *stmt = NULL;
  TEST_ASSERT_EQUAL(SQLITE_OK, sqlite3_prepare_v2(db, sql, -1, &stmt, NULL));
  TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
  int value = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return value;
}

static void setUp_graph(void) {
  TEST_ASSERT_EQUAL(SQLITE_OK, test_prepare_graph(&db, &err_msg));
}

void setUp(void) { setUp_graph(); }

void tearDown(void) { test_cleanup(&db, &err_msg); }

void test_merge_node_creates_then_matches(void) {
  exec_cypher("MERGE (p:Person {name: 'Alice'}) RETURN p");
  TEST_ASSERT_EQUAL_INT(1,
                        fetch_int_query("SELECT COUNT(*) FROM graph_nodes"));

  exec_cypher("MERGE (p:Person {name: 'Alice'}) RETURN p");
  TEST_ASSERT_EQUAL_INT(1,
                        fetch_int_query("SELECT COUNT(*) FROM graph_nodes"));
}

void test_merge_node_label_only(void) {
  // First MERGE creates a node with just a label (no properties)
  exec_cypher("MERGE (p:Company) RETURN p");
  TEST_ASSERT_EQUAL_INT(1,
                        fetch_int_query("SELECT COUNT(*) FROM graph_nodes"));

  // Second MERGE should match the existing node
  exec_cypher("MERGE (p:Company) RETURN p");
  TEST_ASSERT_EQUAL_INT(1,
                        fetch_int_query("SELECT COUNT(*) FROM graph_nodes"));
}

void test_merge_node_multi_properties(void) {
  // First MERGE creates a node with multiple properties
  exec_cypher("MERGE (p:Person {name: 'Alice', age: 30, city: 'NYC'}) RETURN p");
  TEST_ASSERT_EQUAL_INT(1,
                        fetch_int_query("SELECT COUNT(*) FROM graph_nodes"));

  // Second MERGE with same properties should match
  exec_cypher("MERGE (p:Person {name: 'Alice', age: 30, city: 'NYC'}) RETURN p");
  TEST_ASSERT_EQUAL_INT(1,
                        fetch_int_query("SELECT COUNT(*) FROM graph_nodes"));

  // MERGE with different properties should create new node
  exec_cypher("MERGE (p:Person {name: 'Alice', age: 31, city: 'NYC'}) RETURN p");
  TEST_ASSERT_EQUAL_INT(2,
                        fetch_int_query("SELECT COUNT(*) FROM graph_nodes"));
}

void test_merge_node_on_match_updates_properties(void) {
  exec_cypher("CREATE (:Person {name: 'Bob', visits: 1})");
  exec_cypher(
      "MERGE (p:Person {name: 'Bob'}) "
      "ON MATCH SET p.visits = 2 RETURN p");

  TEST_ASSERT_EQUAL_INT(
      2, fetch_int_query("SELECT json_extract(properties, '$.visits') "
                         "FROM graph_nodes "
                         "WHERE json_extract(properties, '$.name') = 'Bob'"));
}

void test_merge_relationship_created_once(void) {
  exec_cypher("MERGE (a:Person {name: 'Alice'}) RETURN a");
  exec_cypher("MERGE (b:Person {name: 'Carol'}) RETURN b");

  exec_cypher("MERGE (a:Person {name: 'Alice'})-[:KNOWS]->(b:Person {name: "
              "'Carol'}) RETURN a, b");
  TEST_ASSERT_EQUAL_INT(1,
                        fetch_int_query("SELECT COUNT(*) FROM graph_edges"));

  exec_cypher("MERGE (a:Person {name: 'Alice'})-[:KNOWS]->(b:Person {name: "
              "'Carol'}) RETURN a, b");
  TEST_ASSERT_EQUAL_INT(1,
                        fetch_int_query("SELECT COUNT(*) FROM graph_edges"));
}

void test_merge_relationship_on_create_sets_properties(void) {
  exec_cypher("MERGE (a:Person {name: 'Dana'}) RETURN a");
  exec_cypher("MERGE (b:Person {name: 'Eve'}) RETURN b");

  exec_cypher(
      "MERGE (a:Person {name: 'Dana'})-[r:FRIENDS]->(b:Person {name: 'Eve'}) "
      "ON CREATE SET r.since = 2024 RETURN r");

  TEST_ASSERT_EQUAL_INT(
      2024,
      fetch_int_query("SELECT json_extract(properties, '$.since') "
                      "FROM graph_edges "
                      "WHERE edge_type = 'FRIENDS'"));
}

void test_merge_on_match_adds_label(void) {
  // SKIPPED: Label assignment syntax (SET p:Label) not yet supported
  // This test would verify that ON MATCH can add labels to matched nodes
  TEST_IGNORE_MESSAGE("Label assignment syntax not yet supported");
}

void test_merge_on_match_multiple_properties(void) {
  // Create a node with basic properties
  exec_cypher("CREATE (:User {id: 1, lastLogin: 100})");

  // MERGE with ON MATCH that updates multiple properties
  exec_cypher(
      "MERGE (u:User {id: 1}) "
      "ON MATCH SET u.lastLogin = 200, u.loginCount = 1 RETURN u");

  // Verify both properties were updated
  TEST_ASSERT_EQUAL_INT(
      200,
      fetch_int_query("SELECT json_extract(properties, '$.lastLogin') "
                      "FROM graph_nodes WHERE json_extract(properties, '$.id') = 1"));
  TEST_ASSERT_EQUAL_INT(
      1,
      fetch_int_query("SELECT json_extract(properties, '$.loginCount') "
                      "FROM graph_nodes WHERE json_extract(properties, '$.id') = 1"));
}

void test_merge_on_create_copies_properties(void) {
  // MERGE with ON CREATE that sets properties from literals
  exec_cypher(
      "MERGE (p:Product {sku: 'ABC123'}) "
      "ON CREATE SET p.created = 2024, p.stock = 100 RETURN p");

  // Verify properties were set on creation
  TEST_ASSERT_EQUAL_INT(
      2024,
      fetch_int_query("SELECT json_extract(properties, '$.created') "
                      "FROM graph_nodes WHERE json_extract(properties, '$.sku') = 'ABC123'"));
  TEST_ASSERT_EQUAL_INT(
      100,
      fetch_int_query("SELECT json_extract(properties, '$.stock') "
                      "FROM graph_nodes WHERE json_extract(properties, '$.sku') = 'ABC123'"));
}

void test_merge_on_create_with_literal(void) {
  // MERGE with ON CREATE that sets a property to a literal value
  exec_cypher(
      "MERGE (p:Item {id: 42}) "
      "ON CREATE SET p.status = 'new' RETURN p");

  // Verify the property was set
  sqlite3_stmt *stmt = NULL;
  TEST_ASSERT_EQUAL(SQLITE_OK,
      sqlite3_prepare_v2(db, "SELECT json_extract(properties, '$.status') "
                             "FROM graph_nodes WHERE json_extract(properties, '$.id') = 42",
                         -1, &stmt, NULL));
  TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
  const unsigned char *status = sqlite3_column_text(stmt, 0);
  TEST_ASSERT_EQUAL_STRING("new", (const char*)status);
  sqlite3_finalize(stmt);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_merge_node_creates_then_matches);
  RUN_TEST(test_merge_node_label_only);
  RUN_TEST(test_merge_node_multi_properties);
  RUN_TEST(test_merge_node_on_match_updates_properties);
  RUN_TEST(test_merge_relationship_created_once);
  RUN_TEST(test_merge_relationship_on_create_sets_properties);
  RUN_TEST(test_merge_on_match_adds_label);
  RUN_TEST(test_merge_on_match_multiple_properties);
  RUN_TEST(test_merge_on_create_copies_properties);
  RUN_TEST(test_merge_on_create_with_literal);
  return UNITY_END();
}
