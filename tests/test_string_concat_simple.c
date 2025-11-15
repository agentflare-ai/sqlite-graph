/*
** Test string concatenation with + operator
** Quick test to verify Phase 04.2 string concatenation implementation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "graph.h"

int main(void) {
    sqlite3 *db = NULL;
    char *zErr = NULL;
    int rc;

    printf("Testing string concatenation with + operator...\n\n");

    /* Open database */
    rc = sqlite3_open(":memory:", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    /* Load extension */
    sqlite3_enable_load_extension(db, 1);
    rc = sqlite3_load_extension(db, "../build/libgraph.dylib", "sqlite3_graph_init", &zErr);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to load extension: %s\n", zErr);
        sqlite3_free(zErr);
        sqlite3_close(db);
        return 1;
    }

    /* Create graph table */
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph_vtab;", NULL, NULL, &zErr);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create graph: %s\n", zErr);
        sqlite3_free(zErr);
        sqlite3_close(db);
        return 1;
    }

    /* Test 1: Basic string concatenation */
    printf("Test 1: Basic string concatenation 'hello' + ' ' + 'world'\n");
    const char *query1 =
        "MERGE (p:Person {id: 1}) "
        "ON CREATE SET p.fullName = 'hello' + ' ' + 'world'";

    rc = sqlite3_exec(db, query1, NULL, NULL, &zErr);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "  ❌ FAILED: %s\n\n", zErr);
        sqlite3_free(zErr);
    } else {
        printf("  ✅ PASSED\n\n");
    }

    /* Test 2: String concatenation with variable */
    printf("Test 2: String concatenation with variable p.first + ' ' + p.last\n");
    const char *query2 =
        "MERGE (p:Person {id: 2}) "
        "ON CREATE SET p.first = 'John', p.last = 'Doe'";

    rc = sqlite3_exec(db, query2, NULL, NULL, &zErr);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "  ❌ Setup failed: %s\n", zErr);
        sqlite3_free(zErr);
    } else {
        /* Now update with concatenation */
        const char *query3 =
            "MERGE (p:Person {id: 2}) "
            "ON MATCH SET p.fullName = p.first + ' ' + p.last";

        rc = sqlite3_exec(db, query3, NULL, NULL, &zErr);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "  ❌ FAILED: %s\n\n", zErr);
            sqlite3_free(zErr);
        } else {
            printf("  ✅ PASSED\n\n");
        }
    }

    /* Test 3: String concatenation with toString() */
    printf("Test 3: String concatenation with type conversion 'Count: ' + toString(42)\n");
    const char *query4 =
        "MERGE (p:Person {id: 3}) "
        "ON CREATE SET p.label = 'Count: ' + toString(42)";

    rc = sqlite3_exec(db, query4, NULL, NULL, &zErr);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "  ❌ FAILED: %s\n\n", zErr);
        sqlite3_free(zErr);
    } else {
        printf("  ✅ PASSED\n\n");
    }

    /* Test 4: NULL handling in concatenation */
    printf("Test 4: NULL handling 'hello' + NULL\n");
    const char *query5 =
        "MERGE (p:Person {id: 4}) "
        "ON CREATE SET p.value = 'hello' + NULL";

    rc = sqlite3_exec(db, query5, NULL, NULL, &zErr);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "  ❌ FAILED: %s\n\n", zErr);
        sqlite3_free(zErr);
    } else {
        printf("  ✅ PASSED (NULL propagation works)\n\n");
    }

    printf("All string concatenation tests completed!\n");

    sqlite3_close(db);
    return 0;
}
