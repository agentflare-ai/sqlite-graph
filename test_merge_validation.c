#include <stdio.h>
#include <sqlite3.h>

int main() {
    sqlite3 *db;
    char *err = NULL;

    sqlite3_open(":memory:", &db);
    sqlite3_enable_load_extension(db, 1);

    int rc = sqlite3_load_extension(db, "build/libgraph.dylib", "sqlite3_graph_init", &err);
    if (rc != SQLITE_OK) {
        printf("Failed to load extension: %s\n", err);
        return 1;
    }

    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        printf("Failed to create table: %s\n", err);
        return 1;
    }

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT cypher_execute(?)", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, "CREATE (a) MERGE (a)", -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    printf("Result: %d (SQLITE_ROW=%d, SQLITE_DONE=%d, SQLITE_ERROR=%d)\n",
           rc, SQLITE_ROW, SQLITE_DONE, SQLITE_ERROR);

    if (rc != SQLITE_ROW) {
        printf("Error: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Query succeeded (should have failed!)\n");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}
