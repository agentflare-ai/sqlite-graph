/* Simple test for MATCH .. RETURN queries */
#include <stdio.h>
#include <sqlite3.h>

int main() {
    sqlite3 *db;
    char *err = NULL;
    int rc;

    /* Open database */
    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    /* Enable extension loading */
    sqlite3_enable_load_extension(db, 1);

    /* Load extension */
    rc = sqlite3_load_extension(db, "../build/libgraph", "sqlite3_graph_init", &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot load extension: %s\n", err);
        sqlite3_free(err);
        return 1;
    }
    printf("✅ Extension loaded\n");

    /* Create graph virtual table */
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create virtual table: %s\n", err);
        sqlite3_free(err);
        return 1;
    }
    printf("✅ Virtual table created\n");

    /* Test 1: CREATE a node */
    printf("\nTest 1: CREATE (n)\n");
    rc = sqlite3_exec(db, "SELECT cypher_execute('CREATE (n)')", NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ CREATE failed: %s\n", err);
        sqlite3_free(err);
    } else {
        printf("✅ CREATE succeeded\n");
    }

    /* Check node count */
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT graph_count_nodes()", -1, &stmt, NULL);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        printf("   Node count: %d\n", count);
    }
    sqlite3_finalize(stmt);

    /* Test 2: CREATE with label */
    printf("\nTest 2: CREATE (p:Person)\n");
    rc = sqlite3_exec(db, "SELECT cypher_execute('CREATE (p:Person)')", NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ CREATE with label failed: %s\n", err);
        sqlite3_free(err);
    } else {
        printf("✅ CREATE with label succeeded\n");
    }

    /* Test 3: MATCH all nodes */
    printf("\nTest 3: MATCH (n) RETURN n\n");
    rc = sqlite3_exec(db, "SELECT cypher_execute('MATCH (n) RETURN n')", NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ MATCH RETURN failed: %s\n", err);
        sqlite3_free(err);
    } else {
        printf("✅ MATCH RETURN succeeded\n");
    }

    /* Test 4: MATCH with label */
    printf("\nTest 4: MATCH (p:Person) RETURN p\n");
    rc = sqlite3_exec(db, "SELECT cypher_execute('MATCH (p:Person) RETURN p')", NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ MATCH with label failed: %s\n", err);
        sqlite3_free(err);
    } else {
        printf("✅ MATCH with label succeeded\n");
    }

    /* Test 5: RETURN literal */
    printf("\nTest 5: RETURN 1\n");
    rc = sqlite3_prepare_v2(db, "SELECT cypher_execute('RETURN 1')", -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ RETURN literal failed: %s\n", sqlite3_errmsg(db));
    } else {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *result = (const char*)sqlite3_column_text(stmt, 0);
            printf("✅ RETURN literal succeeded\n");
            printf("   Result: %s\n", result ? result : "(null)");
        }
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
    printf("\n✅ All tests completed\n");
    return 0;
}
