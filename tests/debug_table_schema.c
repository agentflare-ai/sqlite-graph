#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static sqlite3 *db = NULL;
static char *error_msg = NULL;

int main(void) {
    int rc = sqlite3_open(":memory:", &db);
    sqlite3_enable_load_extension(db, 1);
    
    rc = sqlite3_load_extension(db, "../build/libgraph", "sqlite3_graph_init", &error_msg);
    if (rc != SQLITE_OK) {
        printf("Load extension failed: %s\n", error_msg);
        return 1;
    }
    
    // Create backing tables
    sqlite3_exec(db, 
        "CREATE TABLE nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT);"
        "CREATE TABLE edges(id INTEGER PRIMARY KEY, from_id INTEGER, to_id INTEGER, type TEXT, weight REAL, properties TEXT);",
        NULL, NULL, NULL);
    
    // Create virtual table
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph(nodes, edges);", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {
        printf("Virtual table creation failed: %s\n", error_msg);
        return 1;
    }
    
    // Check table schema
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "PRAGMA table_info(graph);", -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        printf("Graph table schema:\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int cid = sqlite3_column_int(stmt, 0);
            const char *name = (const char*)sqlite3_column_text(stmt, 1);
            const char *type = (const char*)sqlite3_column_text(stmt, 2);
            printf("  %d: %s (%s)\n", cid, name, type);
        }
        sqlite3_finalize(stmt);
    }
    
    // Try a simple SELECT to see what we can query
    rc = sqlite3_prepare_v2(db, "SELECT * FROM graph LIMIT 1;", -1, &stmt, NULL);
    printf("\nSimple SELECT: %s\n", rc == SQLITE_OK ? "OK" : sqlite3_errmsg(db));
    
    if (rc == SQLITE_OK) {
        int cols = sqlite3_column_count(stmt);
        printf("Number of columns: %d\n", cols);
        
        for (int i = 0; i < cols; i++) {
            const char *colname = sqlite3_column_name(stmt, i);
            printf("  Column %d: %s\n", i, colname);
        }
        
        sqlite3_finalize(stmt);
    }
    
    sqlite3_close(db);
    return 0;
}
