#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unity.h>

static sqlite3 *db = NULL;
static char *error_msg = NULL;

void debug_create_test(void) {
    int rc = sqlite3_open(":memory:", &db);
    printf("1. Open database: %s\n", rc == SQLITE_OK ? "OK" : sqlite3_errmsg(db));
    
    // Enable loading extensions
    sqlite3_enable_load_extension(db, 1);
    
    // Load graph extension
    rc = sqlite3_load_extension(db, "../build/libgraph.so", "sqlite3_graph_init", &error_msg);
    printf("2. Load extension: %s\n", rc == SQLITE_OK ? "OK" : (error_msg ? error_msg : "Unknown error"));
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return;
    }
    
    // Create backing tables
    rc = sqlite3_exec(db, 
        "CREATE TABLE nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT);",
        NULL, NULL, &error_msg);
    printf("3. Create nodes table: %s\n", rc == SQLITE_OK ? "OK" : (error_msg ? error_msg : "Unknown error"));
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    
    rc = sqlite3_exec(db, 
        "CREATE TABLE edges(id INTEGER PRIMARY KEY, from_id INTEGER, to_id INTEGER, type TEXT, weight REAL, properties TEXT);",
        NULL, NULL, &error_msg);
    printf("4. Create edges table: %s\n", rc == SQLITE_OK ? "OK" : (error_msg ? error_msg : "Unknown error"));
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    
    // Create virtual table
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph(nodes, edges);", NULL, NULL, &error_msg);
    printf("5. Create virtual table: %s\n", rc == SQLITE_OK ? "OK" : (error_msg ? error_msg : "Unknown error"));
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return;
    }
    
    // Try to insert a node
    rc = sqlite3_exec(db, 
        "INSERT INTO graph (type, properties) VALUES ('node', '{}');",
        NULL, NULL, &error_msg);
    printf("6. Insert node: %s\n", rc == SQLITE_OK ? "OK" : (error_msg ? error_msg : "Unknown error"));
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    
    // Try to count nodes
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM graph WHERE type = 'node'", -1, &stmt, NULL);
    printf("7. Prepare count query: %s\n", rc == SQLITE_OK ? "OK" : sqlite3_errmsg(db));
    
    if (rc == SQLITE_OK) {
        rc = sqlite3_step(stmt);
        printf("8. Execute count query: %s\n", rc == SQLITE_ROW ? "OK" : sqlite3_errmsg(db));
        
        if (rc == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            printf("9. Node count: %d\n", count);
        }
        
        sqlite3_finalize(stmt);
    }
    
    sqlite3_close(db);
}

int main(void) {
    debug_create_test();
    return 0;
}
