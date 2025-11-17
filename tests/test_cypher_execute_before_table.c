/* Test for issue #8: NULL pointer dereference in cypher_execute()
 * 
 * This test verifies that calling cypher_execute() before creating
 * a graph virtual table returns an error instead of segfaulting.
 */
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

int main() {
    sqlite3 *db;
    char *err = NULL;
    int rc;
    sqlite3_stmt *stmt;
    const char *error_msg;

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

    /* Test 1: Call cypher_execute() BEFORE creating graph virtual table
     * This should return an error, not segfault
     */
    printf("\nTest 1: cypher_execute() before graph table creation\n");
    printf("  Expected: Error message indicating graph table not initialized\n");
    printf("  Actual: ");
    
    rc = sqlite3_prepare_v2(db, "SELECT cypher_execute('MATCH (n) RETURN n')", -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ERROR) {
        error_msg = sqlite3_errmsg(db);
        printf("Error returned: %s\n", error_msg);
        
        /* Check that the error message indicates the graph table needs to be created */
        if (strstr(error_msg, "graph") != NULL || 
            strstr(error_msg, "Graph") != NULL ||
            strstr(error_msg, "virtual table") != NULL ||
            strstr(error_msg, "not initialized") != NULL ||
            strstr(error_msg, "CREATE VIRTUAL TABLE") != NULL) {
            printf("✅ Correct error message returned (mentions graph/virtual table)\n");
        } else {
            printf("⚠️  Error returned but message doesn't mention graph/virtual table\n");
        }
    } else if (rc == SQLITE_ROW) {
        fprintf(stderr, "❌ Query succeeded unexpectedly (should have failed)\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    } else {
        fprintf(stderr, "❌ Unexpected return code: %d\n", rc);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_finalize(stmt);

    /* Test 2: Now create the graph virtual table */
    printf("\nTest 2: Create graph virtual table\n");
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ Cannot create virtual table: %s\n", err);
        sqlite3_free(err);
        sqlite3_close(db);
        return 1;
    }
    printf("✅ Virtual table created\n");

    /* Test 3: Call cypher_execute() AFTER creating graph virtual table
     * This should work normally
     */
    printf("\nTest 3: cypher_execute() after graph table creation\n");
    printf("  Expected: Query executes successfully\n");
    rc = sqlite3_prepare_v2(db, "SELECT cypher_execute('MATCH (n) RETURN n')", -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        printf("✅ Query executed successfully\n");
        const char *result = (const char*)sqlite3_column_text(stmt, 0);
        if (result) {
            printf("   Result: %s\n", result);
        }
    } else {
        error_msg = sqlite3_errmsg(db);
        fprintf(stderr, "❌ Query failed: %s\n", error_msg);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_finalize(stmt);

    /* Test 4: Call cypher_execute() with invalid query after table creation
     * This should return a parse/execution error, not segfault
     */
    printf("\nTest 4: cypher_execute() with invalid query (after table creation)\n");
    printf("  Expected: Parse/execution error, not segfault\n");
    rc = sqlite3_prepare_v2(db, "SELECT cypher_execute('INVALID QUERY SYNTAX')", -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ERROR) {
        error_msg = sqlite3_errmsg(db);
        printf("✅ Error returned (as expected): %s\n", error_msg);
    } else if (rc == SQLITE_ROW) {
        printf("⚠️  Query succeeded (unexpected, but not a crash)\n");
    } else {
        fprintf(stderr, "❌ Unexpected return code: %d\n", rc);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_finalize(stmt);

    sqlite3_close(db);
    printf("\n✅ All tests completed successfully\n");
    printf("   The fix prevents segfaults when cypher_execute() is called\n");
    printf("   before graph table creation.\n");
    return 0;
}

