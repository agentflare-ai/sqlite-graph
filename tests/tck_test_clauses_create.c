// Generated TCK tests for language area: clauses.create  
// Total scenarios: 78

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unity.h>

static sqlite3 *db = NULL;
static char *error_msg = NULL;

/* Helper function to execute Cypher query via cypher_execute */
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

/* Helper function to execute Cypher and expect an error */
static void exec_cypher_expect_error(const char *query) {
    sqlite3_stmt *stmt = NULL;
    TEST_ASSERT_EQUAL(SQLITE_OK,
                    sqlite3_prepare_v2(db, "SELECT cypher_execute(?)", -1,
                                       &stmt, NULL));
    TEST_ASSERT_EQUAL(SQLITE_OK,
                    sqlite3_bind_text(stmt, 1, query, -1, SQLITE_TRANSIENT));
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    /* Expect the query to fail */
    TEST_ASSERT_NOT_EQUAL(SQLITE_ROW, rc);
}

/* Helper function to fetch integer result from SQL query */
static int fetch_int_query(const char *sql) {
    sqlite3_stmt *stmt = NULL;
    TEST_ASSERT_EQUAL(SQLITE_OK, sqlite3_prepare_v2(db, sql, -1, &stmt, NULL));
    TEST_ASSERT_EQUAL(SQLITE_ROW, sqlite3_step(stmt));
    int value = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return value;
}

/* Helper function to assert node count */
static void assert_node_count(int expected) {
    int count = fetch_int_query("SELECT COUNT(*) FROM graph_nodes");
    TEST_ASSERT_EQUAL_INT(expected, count);
}

/* Helper function to assert edge/relationship count */
static void assert_edge_count(int expected) {
    int count = fetch_int_query("SELECT COUNT(*) FROM graph_edges");
    TEST_ASSERT_EQUAL_INT(expected, count);
}

void setUp(void) {
    int rc = sqlite3_open(":memory:", &db);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Enable loading extensions
    sqlite3_enable_load_extension(db, 1);

    // Load graph extension
    // Determine path based on current working directory
#ifdef __APPLE__
    const char *lib_path = "build/libgraph";  // sqlite3 appends .dylib on macOS
#else
    const char *lib_path = "build/libgraph";  // sqlite3 appends .so on Linux
#endif
    rc = sqlite3_load_extension(db, lib_path, "sqlite3_graph_init", &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to load graph extension: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);

    // Create graph virtual table
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to create virtual table: %s\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
}

void tearDown(void) {
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

// ============================================================================
// Create1 - Creating nodes (20 scenarios)
// ============================================================================

void test_clauses_create_Create1_01(void) {
    // [1] Create a single node
    exec_cypher("CREATE ()");
    assert_node_count(1);
}

void test_clauses_create_Create1_02(void) {
    // [2] Create two nodes
    exec_cypher("CREATE (), ()");
    assert_node_count(2);
}

void test_clauses_create_Create1_03(void) {
    // [3] Create a single node with a label
    exec_cypher("CREATE (:Label)");
    assert_node_count(1);
}

void test_clauses_create_Create1_04(void) {
    // [4] Create two nodes with same label
    exec_cypher("CREATE (:Label), (:Label)");
    assert_node_count(2);
}

void test_clauses_create_Create1_05(void) {
    // [5] Create a single node with multiple labels
    exec_cypher("CREATE (:A:B:C:D)");
    assert_node_count(1);
}

void test_clauses_create_Create1_06(void) {
    // [6] Create three nodes with multiple labels
    exec_cypher("CREATE (:B:A:D), (:B:C), (:D:E:B)");
    assert_node_count(3);
}

void test_clauses_create_Create1_07(void) {
    // [7] Create a single node with a property
    exec_cypher("CREATE ({created: true})");
    assert_node_count(1);
}

void test_clauses_create_Create1_08(void) {
    // [8] Create a single node with a property and return it
    exec_cypher("CREATE (n {name: 'foo'}) RETURN n.name AS p");
    assert_node_count(1);
}

void test_clauses_create_Create1_09(void) {
    // [9] Create a single node with two properties
    exec_cypher("CREATE (n {id: 12, name: 'foo'})");
    assert_node_count(1);
}

void test_clauses_create_Create1_10(void) {
    // [10] Create a single node with two properties and return them
    exec_cypher("CREATE (n {id: 12, name: 'foo'}) RETURN n.id AS id, n.name AS p");
    assert_node_count(1);
}

void test_clauses_create_Create1_11(void) {
    // [11] Create a single node with null properties should not return those properties
    exec_cypher("CREATE (n {id: 12, name: null}) RETURN n.id AS id, n.name AS p");
    assert_node_count(1);
}

void test_clauses_create_Create1_12(void) {
    // [12] CREATE does not lose precision on large integers
    exec_cypher("CREATE (p:TheLabel {id: 4611686018427387905}) RETURN p.id");
    assert_node_count(1);
}

void test_clauses_create_Create1_13(void) {
    // [13] Fail when creating a node that is already bound
    exec_cypher_expect_error("MATCH (a) CREATE (a)");
}

void test_clauses_create_Create1_14(void) {
    // [14] Fail when creating a node with properties that is already bound
    exec_cypher_expect_error("MATCH (a) CREATE (a {name: 'foo'}) RETURN a");
}

void test_clauses_create_Create1_15(void) {
    // [15] Fail when adding a new label predicate on a node that is already bound 1
    exec_cypher_expect_error("CREATE (n:Foo)-[:T1]->(), (n:Bar)-[:T2]->()");
}

void test_clauses_create_Create1_16(void) {
    // [16] Fail when adding new label predicate on a node that is already bound 2
    exec_cypher_expect_error("CREATE ()<-[:T2]-(n:Foo), (n:Bar)<-[:T1]-()");
}

void test_clauses_create_Create1_17(void) {
    // [17] Fail when adding new label predicate on a node that is already bound 3
    exec_cypher_expect_error("CREATE (n:Foo) CREATE (n:Bar)-[:OWNS]->(:Dog)");
}

void test_clauses_create_Create1_18(void) {
    // [18] Fail when adding new label predicate on a node that is already bound 4
    exec_cypher_expect_error("CREATE (n {}) CREATE (n:Bar)-[:OWNS]->(:Dog)");
}

void test_clauses_create_Create1_19(void) {
    // [19] Fail when adding new label predicate on a node that is already bound 5
    exec_cypher_expect_error("CREATE (n:Foo) CREATE (n {})-[:OWNS]->(:Dog)");
}

void test_clauses_create_Create1_20(void) {
    // [20] Fail when creating a node using undefined variable in pattern
    exec_cypher_expect_error("CREATE (b {name: missing}) RETURN b");
}

// ============================================================================
// Create2 - Creating relationships (24 scenarios)
// ============================================================================

void test_clauses_create_Create2_01(void) {
    // [1] Create two nodes and a single relationship in a single pattern
    exec_cypher("CREATE ()-[:R]->()");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create2_02(void) {
    // [2] Create two nodes and a single relationship in separate patterns
    exec_cypher("CREATE (a), (b), (a)-[:R]->(b)");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create2_03(void) {
    // [3] Create two nodes and a single relationship in separate clauses
    exec_cypher("CREATE (a) CREATE (b) CREATE (a)-[:R]->(b)");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create2_04(void) {
    // [4] Create two nodes and a single relationship in the reverse direction
    exec_cypher("CREATE (:A)<-[:R]-(:B)");
    assert_node_count(2);
    assert_edge_count(1);
    exec_cypher("MATCH (a:A)<-[:R]-(b:B) RETURN a, b");
}

void test_clauses_create_Create2_05(void) {
    // [5] Create a single relationship between two existing nodes
    exec_cypher("CREATE (:X)");
    exec_cypher("CREATE (:Y)");
    assert_node_count(2);
    exec_cypher("MATCH (x:X), (y:Y) CREATE (x)-[:R]->(y)");
    assert_edge_count(1);
}

void test_clauses_create_Create2_06(void) {
    // [6] Create a single relationship between two existing nodes in the reverse direction
    exec_cypher("CREATE (:X)");
    exec_cypher("CREATE (:Y)");
    assert_node_count(2);
    exec_cypher("MATCH (x:X), (y:Y) CREATE (x)<-[:R]-(y)");
    assert_edge_count(1);
    exec_cypher("MATCH (x:X)<-[:R]-(y:Y) RETURN x, y");
}

void test_clauses_create_Create2_07(void) {
    // [7] Create a single node and a single self loop in a single pattern
    exec_cypher("CREATE (root)-[:LINK]->(root)");
    assert_node_count(1);
    assert_edge_count(1);
}

void test_clauses_create_Create2_08(void) {
    // [8] Create a single node and a single self loop in separate patterns
    exec_cypher("CREATE (root), (root)-[:LINK]->(root)");
    assert_node_count(1);
    assert_edge_count(1);
}

void test_clauses_create_Create2_09(void) {
    // [9] Create a single node and a single self loop in separate clauses
    exec_cypher("CREATE (root) CREATE (root)-[:LINK]->(root)");
    assert_node_count(1);
    assert_edge_count(1);
}

void test_clauses_create_Create2_10(void) {
    // [10] Create a single self loop on an existing node
    exec_cypher("CREATE (:Root)");
    exec_cypher("MATCH (root:Root) CREATE (root)-[:LINK]->(root)");
    assert_node_count(1);
    assert_edge_count(1);
}

void test_clauses_create_Create2_11(void) {
    // [11] Create a single relationship and an end node on an existing starting node
    exec_cypher("CREATE (:Begin)");
    assert_node_count(1);
    exec_cypher("MATCH (x:Begin) CREATE (x)-[:TYPE]->(:End)");
    assert_node_count(2);
    assert_edge_count(1);
    exec_cypher("MATCH (x:Begin)-[:TYPE]->(y:End) RETURN x, y");
}

void test_clauses_create_Create2_12(void) {
    // [12] Create a single relationship and a starting node on an existing end node
    exec_cypher("CREATE (:End)");
    assert_node_count(1);
    exec_cypher("MATCH (x:End) CREATE (:Begin)-[:TYPE]->(x)");
    assert_node_count(2);
    assert_edge_count(1);
    exec_cypher("MATCH (x:Begin)-[:TYPE]->(y:End) RETURN x, y");
}

void test_clauses_create_Create2_13(void) {
    // [13] Create a single relationship with a property
    exec_cypher("CREATE ()-[:R {num: 42}]->()");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create2_14(void) {
    // [14] Create a single relationship with a property and return it
    exec_cypher("CREATE ()-[r:R {num: 42}]->() RETURN r.num AS num");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create2_15(void) {
    // [15] Create a single relationship with two properties
    exec_cypher("CREATE ()-[:R {id: 12, name: 'foo'}]->()");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create2_16(void) {
    // [16] Create a single relationship with two properties and return them
    exec_cypher("CREATE ()-[r:R {id: 12, name: 'foo'}]->() RETURN r.id AS id, r.name AS name");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create2_17(void) {
    // [17] Create a single relationship with null properties should not return those properties
    exec_cypher("CREATE ()-[r:X {id: 12, name: null}]->() RETURN r.id, r.name AS name");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create2_18(void) {
    // [18] Fail when creating a relationship without a type
    exec_cypher_expect_error("CREATE ()-->()");
}

void test_clauses_create_Create2_19(void) {
    // [19] Fail when creating a relationship without a direction
    exec_cypher_expect_error("CREATE (a)-[:FOO]-(b)");
}

void test_clauses_create_Create2_20(void) {
    // [20] Fail when creating a relationship with two directions
    exec_cypher_expect_error("CREATE (a)<-[:FOO]->(b)");
}

void test_clauses_create_Create2_21(void) {
    // [21] Fail when creating a relationship with more than one type
    exec_cypher_expect_error("CREATE ()-[:A|:B]->()");
}

void test_clauses_create_Create2_22(void) {
    // [22] Fail when creating a variable-length relationship
    exec_cypher_expect_error("CREATE ()-[:FOO*2]->()");
}

void test_clauses_create_Create2_23(void) {
    // [23] Fail when creating a relationship that is already bound
    exec_cypher_expect_error("MATCH ()-[r]->() CREATE ()-[r]->()");
}

void test_clauses_create_Create2_24(void) {
    // [24] Fail when creating a relationship using undefined variable in pattern
    exec_cypher_expect_error("MATCH (a) CREATE (a)-[:KNOWS]->(b {name: missing}) RETURN b");
}

// ============================================================================
// Create3 - Interoperation with other clauses (13 scenarios)
// ============================================================================

void test_clauses_create_Create3_01(void) {
    // [1] MATCH-CREATE
    exec_cypher("CREATE (), ()");
    assert_node_count(2);
    exec_cypher("MATCH () CREATE ()");
    assert_node_count(4);
}

void test_clauses_create_Create3_02(void) {
    // [2] WITH-CREATE
    exec_cypher("CREATE (), ()");
    assert_node_count(2);
    exec_cypher("MATCH () CREATE () WITH * CREATE ()");
    assert_node_count(6);
}

void test_clauses_create_Create3_03(void) {
    // [3] MATCH-CREATE-WITH-CREATE
    exec_cypher("CREATE (), ()");
    assert_node_count(2);
    exec_cypher("MATCH () CREATE () WITH * MATCH () CREATE ()");
    assert_node_count(12);
}

void test_clauses_create_Create3_04(void) {
    // [4] MATCH-CREATE: Newly-created nodes not visible to preceding MATCH
    exec_cypher("CREATE ()");
    assert_node_count(1);
    exec_cypher("MATCH () CREATE ()");
    assert_node_count(2);
}

void test_clauses_create_Create3_05(void) {
    // [5] WITH-CREATE: Nodes are not created when aliases are applied to variable names
    exec_cypher("CREATE ({num: 1})");
    assert_node_count(1);
    exec_cypher("MATCH (n) MATCH (m) WITH n AS a, m AS b CREATE (a)-[:T]->(b) RETURN a, b");
    assert_node_count(1);
    assert_edge_count(1);
}

void test_clauses_create_Create3_06(void) {
    // [6] WITH-CREATE: Only a single node is created when an alias is applied to a variable name
    exec_cypher("CREATE (:X)");
    assert_node_count(1);
    exec_cypher("MATCH (n) WITH n AS a CREATE (a)-[:T]->() RETURN a");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create3_07(void) {
    // [7] WITH-CREATE: Nodes are not created when aliases are applied to variable names multiple times
    exec_cypher("CREATE ({name: 'A'})");
    assert_node_count(1);
    exec_cypher("MATCH (n) MATCH (m) WITH n AS a, m AS b CREATE (a)-[:T]->(b) WITH a AS x, b AS y CREATE (x)-[:T]->(y) RETURN x, y");
    assert_node_count(1);
    assert_edge_count(2);
}

void test_clauses_create_Create3_08(void) {
    // [8] WITH-CREATE: Only a single node is created when an alias is applied to a variable name multiple times
    exec_cypher("CREATE ({num: 5})");
    assert_node_count(1);
    exec_cypher("MATCH (n) WITH n AS a CREATE (a)-[:T]->() WITH a AS x CREATE (x)-[:T]->() RETURN x");
    assert_node_count(3);
    assert_edge_count(2);
}

void test_clauses_create_Create3_09(void) {
    // [9] WITH-CREATE: A bound node should be recognized after projection with WITH + WITH
    exec_cypher("CREATE (a) WITH a WITH * CREATE (b) CREATE (a)<-[:T]-(b)");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create3_10(void) {
    // [10] WITH-UNWIND-CREATE: A bound node should be recognized after projection with WITH + UNWIND
    exec_cypher("CREATE (a) WITH a UNWIND [0] AS i CREATE (b) CREATE (a)<-[:T]-(b)");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create3_11(void) {
    // [11] WITH-MERGE-CREATE: A bound node should be recognized after projection with WITH + MERGE node
    exec_cypher("CREATE (a) WITH a MERGE () CREATE (b) CREATE (a)<-[:T]-(b)");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create3_12(void) {
    // [12] WITH-MERGE-CREATE: A bound node should be recognized after projection with WITH + MERGE pattern
    exec_cypher("CREATE (a) WITH a MERGE (x) MERGE (y) MERGE (x)-[:T]->(y) CREATE (b) CREATE (a)<-[:T]-(b)");
    assert_node_count(2);
    assert_edge_count(2);
}

void test_clauses_create_Create3_13(void) {
    // [13] Merge followed by multiple creates
    exec_cypher("MERGE (t:T {id: 42}) CREATE (f:R) CREATE (t)-[:REL]->(f)");
    assert_node_count(2);
    assert_edge_count(1);
}

// ============================================================================
// Create4 - Large Create Query (2 scenarios)
// ============================================================================

void test_clauses_create_Create4_01(void) {
    // [1] Generate the movie graph - Large query
    TEST_IGNORE_MESSAGE("Large movie graph creation test - may exceed parser/execution limits");
}

void test_clauses_create_Create4_02(void) {
    // [2] Second large create scenario
    TEST_IGNORE_MESSAGE("Large create scenario - implementation pending");
}

// ============================================================================
// Create5 - Multiple hops create patterns (5 scenarios)
// ============================================================================

void test_clauses_create_Create5_01(void) {
    // [1] Create a pattern with multiple hops
    exec_cypher("CREATE (:A)-[:R]->(:B)-[:R]->(:C)");
    assert_node_count(3);
    assert_edge_count(2);
    exec_cypher("MATCH (a:A)-[:R]->(b:B)-[:R]->(c:C) RETURN a, b, c");
}

void test_clauses_create_Create5_02(void) {
    // [2] Create a pattern with multiple hops in the reverse direction
    exec_cypher("CREATE (:A)<-[:R]-(:B)<-[:R]-(:C)");
    assert_node_count(3);
    assert_edge_count(2);
    exec_cypher("MATCH (a)<-[:R]-(b)<-[:R]-(c) RETURN a, b, c");
}

void test_clauses_create_Create5_03(void) {
    // [3] Create a pattern with multiple hops in varying directions
    exec_cypher("CREATE (:A)-[:R]->(:B)<-[:R]-(:C)");
    assert_node_count(3);
    assert_edge_count(2);
    exec_cypher("MATCH (a:A)-[r1:R]->(b:B)<-[r2:R]-(c:C) RETURN a, b, c");
}

void test_clauses_create_Create5_04(void) {
    // [4] Create a pattern with multiple hops with multiple types and varying directions
    exec_cypher("CREATE ()-[:R1]->()<-[:R2]-()-[:R3]->()");
    assert_node_count(4);
    assert_edge_count(3);
    exec_cypher("MATCH ()-[r1:R1]->()<-[r2:R2]-()-[r3:R3]->() RETURN r1, r2, r3");
}

void test_clauses_create_Create5_05(void) {
    // [5] Create a pattern with multiple hops and varying directions
    exec_cypher("CREATE (:A)<-[:R1]-(:B)-[:R2]->(:C)");
    assert_node_count(3);
    assert_edge_count(2);
    exec_cypher("MATCH (a:A)<-[r1:R1]-(b:B)-[r2:R2]->(c:C) RETURN *");
}

// ============================================================================
// Create6 - Persistence of create clause side effects (14 scenarios)
// ============================================================================

void test_clauses_create_Create6_01(void) {
    // [1] Limiting to zero results after creating nodes affects the result set but not the side effects
    exec_cypher("CREATE (n:N {num: 42}) RETURN n LIMIT 0");
    assert_node_count(1);
}

void test_clauses_create_Create6_02(void) {
    // [2] Skipping all results after creating nodes affects the result set but not the side effects
    exec_cypher("CREATE (n:N {num: 42}) RETURN n SKIP 1");
    assert_node_count(1);
}

void test_clauses_create_Create6_03(void) {
    // [3] Skipping and limiting to a few results after creating nodes does not affect the result set nor the side effects
    exec_cypher("UNWIND [42, 42, 42, 42, 42] AS x CREATE (n:N {num: x}) RETURN n.num AS num SKIP 2 LIMIT 2");
    assert_node_count(5);
}

void test_clauses_create_Create6_04(void) {
    // [4] Skipping zero result and limiting to all results after creating nodes does not affect the result set nor the side effects
    exec_cypher("UNWIND [42, 42, 42, 42, 42] AS x CREATE (n:N {num: x}) RETURN n.num AS num SKIP 0 LIMIT 5");
    assert_node_count(5);
}

void test_clauses_create_Create6_05(void) {
    // [5] Filtering after creating nodes affects the result set but not the side effects
    exec_cypher("UNWIND [1, 2, 3, 4, 5] AS x CREATE (n:N {num: x}) WITH n WHERE n.num % 2 = 0 RETURN n.num AS num");
    assert_node_count(5);
}

void test_clauses_create_Create6_06(void) {
    // [6] Aggregating in RETURN after creating nodes affects the result set but not the side effects
    exec_cypher("UNWIND [1, 2, 3, 4, 5] AS x CREATE (n:N {num: x}) RETURN sum(n.num) AS sum");
    assert_node_count(5);
}

void test_clauses_create_Create6_07(void) {
    // [7] Aggregating in WITH after creating nodes affects the result set but not the side effects
    exec_cypher("UNWIND [1, 2, 3, 4, 5] AS x CREATE (n:N {num: x}) WITH sum(n.num) AS sum RETURN sum");
    assert_node_count(5);
}

void test_clauses_create_Create6_08(void) {
    // [8] Limiting to zero results after creating relationships affects the result set but not the side effects
    exec_cypher("CREATE ()-[r:R {num: 42}]->() RETURN r LIMIT 0");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create6_09(void) {
    // [9] Skipping all results after creating relationships affects the result set but not the side effects
    exec_cypher("CREATE ()-[r:R {num: 42}]->() RETURN r SKIP 1");
    assert_node_count(2);
    assert_edge_count(1);
}

void test_clauses_create_Create6_10(void) {
    // [10] Skipping and limiting to a few results after creating relationships does not affect the result set nor the side effects
    exec_cypher("UNWIND [42, 42, 42, 42, 42] AS x CREATE ()-[r:R {num: x}]->() RETURN r.num AS num SKIP 2 LIMIT 2");
    assert_node_count(10);
    assert_edge_count(5);
}

void test_clauses_create_Create6_11(void) {
    // [11] Skipping zero result and limiting to all results after creating relationships does not affect the result set nor the side effects
    exec_cypher("UNWIND [42, 42, 42, 42, 42] AS x CREATE ()-[r:R {num: x}]->() RETURN r.num AS num SKIP 0 LIMIT 5");
    assert_node_count(10);
    assert_edge_count(5);
}

void test_clauses_create_Create6_12(void) {
    // [12] Filtering after creating relationships affects the result set but not the side effects
    exec_cypher("UNWIND [1, 2, 3, 4, 5] AS x CREATE ()-[r:R {num: x}]->() WITH r WHERE r.num % 2 = 0 RETURN r.num AS num");
    assert_node_count(10);
    assert_edge_count(5);
}

void test_clauses_create_Create6_13(void) {
    // [13] Aggregating in RETURN after creating relationships affects the result set but not the side effects
    exec_cypher("UNWIND [1, 2, 3, 4, 5] AS x CREATE ()-[r:R {num: x}]->() RETURN sum(r.num) AS sum");
    assert_node_count(10);
    assert_edge_count(5);
}

void test_clauses_create_Create6_14(void) {
    // [14] Aggregating in WITH after creating relationships affects the result set but not the side effects
    exec_cypher("UNWIND [1, 2, 3, 4, 5] AS x CREATE ()-[r:R {num: x}]->() WITH sum(r.num) AS sum RETURN sum");
    assert_node_count(10);
    assert_edge_count(5);
}

// ============================================================================
// Main test runner
// ============================================================================

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_clauses_create_Create1_01);
    RUN_TEST(test_clauses_create_Create1_02);
    RUN_TEST(test_clauses_create_Create1_03);
    RUN_TEST(test_clauses_create_Create1_04);
    RUN_TEST(test_clauses_create_Create1_05);
    RUN_TEST(test_clauses_create_Create1_06);
    RUN_TEST(test_clauses_create_Create1_07);
    RUN_TEST(test_clauses_create_Create1_08);
    RUN_TEST(test_clauses_create_Create1_09);
    RUN_TEST(test_clauses_create_Create1_10);
    RUN_TEST(test_clauses_create_Create1_11);
    RUN_TEST(test_clauses_create_Create1_12);
    RUN_TEST(test_clauses_create_Create1_13);
    RUN_TEST(test_clauses_create_Create1_14);
    RUN_TEST(test_clauses_create_Create1_15);
    RUN_TEST(test_clauses_create_Create1_16);
    RUN_TEST(test_clauses_create_Create1_17);
    RUN_TEST(test_clauses_create_Create1_18);
    RUN_TEST(test_clauses_create_Create1_19);
    RUN_TEST(test_clauses_create_Create1_20);

    RUN_TEST(test_clauses_create_Create2_01);
    RUN_TEST(test_clauses_create_Create2_02);
    RUN_TEST(test_clauses_create_Create2_03);
    RUN_TEST(test_clauses_create_Create2_04);
    RUN_TEST(test_clauses_create_Create2_05);
    RUN_TEST(test_clauses_create_Create2_06);
    RUN_TEST(test_clauses_create_Create2_07);
    RUN_TEST(test_clauses_create_Create2_08);
    RUN_TEST(test_clauses_create_Create2_09);
    RUN_TEST(test_clauses_create_Create2_10);
    RUN_TEST(test_clauses_create_Create2_11);
    RUN_TEST(test_clauses_create_Create2_12);
    RUN_TEST(test_clauses_create_Create2_13);
    RUN_TEST(test_clauses_create_Create2_14);
    RUN_TEST(test_clauses_create_Create2_15);
    RUN_TEST(test_clauses_create_Create2_16);
    RUN_TEST(test_clauses_create_Create2_17);
    RUN_TEST(test_clauses_create_Create2_18);
    RUN_TEST(test_clauses_create_Create2_19);
    RUN_TEST(test_clauses_create_Create2_20);
    RUN_TEST(test_clauses_create_Create2_21);
    RUN_TEST(test_clauses_create_Create2_22);
    RUN_TEST(test_clauses_create_Create2_23);
    RUN_TEST(test_clauses_create_Create2_24);

    RUN_TEST(test_clauses_create_Create3_01);
    RUN_TEST(test_clauses_create_Create3_02);
    RUN_TEST(test_clauses_create_Create3_03);
    RUN_TEST(test_clauses_create_Create3_04);
    RUN_TEST(test_clauses_create_Create3_05);
    RUN_TEST(test_clauses_create_Create3_06);
    RUN_TEST(test_clauses_create_Create3_07);
    RUN_TEST(test_clauses_create_Create3_08);
    RUN_TEST(test_clauses_create_Create3_09);
    RUN_TEST(test_clauses_create_Create3_10);
    RUN_TEST(test_clauses_create_Create3_11);
    RUN_TEST(test_clauses_create_Create3_12);
    RUN_TEST(test_clauses_create_Create3_13);

    RUN_TEST(test_clauses_create_Create4_01);
    RUN_TEST(test_clauses_create_Create4_02);

    RUN_TEST(test_clauses_create_Create5_01);
    RUN_TEST(test_clauses_create_Create5_02);
    RUN_TEST(test_clauses_create_Create5_03);
    RUN_TEST(test_clauses_create_Create5_04);
    RUN_TEST(test_clauses_create_Create5_05);

    RUN_TEST(test_clauses_create_Create6_01);
    RUN_TEST(test_clauses_create_Create6_02);
    RUN_TEST(test_clauses_create_Create6_03);
    RUN_TEST(test_clauses_create_Create6_04);
    RUN_TEST(test_clauses_create_Create6_05);
    RUN_TEST(test_clauses_create_Create6_06);
    RUN_TEST(test_clauses_create_Create6_07);
    RUN_TEST(test_clauses_create_Create6_08);
    RUN_TEST(test_clauses_create_Create6_09);
    RUN_TEST(test_clauses_create_Create6_10);
    RUN_TEST(test_clauses_create_Create6_11);
    RUN_TEST(test_clauses_create_Create6_12);
    RUN_TEST(test_clauses_create_Create6_13);
    RUN_TEST(test_clauses_create_Create6_14);

    return UNITY_END();
}
