#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static sqlite3 *db = NULL;
static char *error_msg = NULL;

int main(void) {
    int rc = sqlite3_open(":memory:", &db);
    printf("1. Open database: %s\n", rc == SQLITE_OK ? "OK" : sqlite3_errmsg(db));
    
    // Enable loading extensions
    sqlite3_enable_load_extension(db, 1);
    
    // Load graph extension
    rc = sqlite3_load_extension(db, "../build/libgraph", "sqlite3_graph_init", &error_msg);
    printf("2. Load extension: %s\n", rc == SQLITE_OK ? "OK" : (error_msg ? error_msg : "Unknown error"));
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }
    
    // Create virtual table (this should create backing tables automatically)
    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE my_graph USING graph();", NULL, NULL, &error_msg);
    printf("3. Create virtual table: %s\n", rc == SQLITE_OK ? "OK" : (error_msg ? error_msg : "Unknown error"));
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return 1;
    }
    
    // Check what tables were created
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT name FROM sqlite_master WHERE type='table'", -1, &stmt, NULL);
    printf("4. Check created tables:\n");
    
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("   Table: %s\n", sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    
    // Try to insert a node directly via virtual table
    rc = sqlite3_exec(db, 
        "INSERT INTO my_graph (type, properties) VALUES ('node', '{\"name\": \"test\"}');",
        NULL, NULL, &error_msg);
    printf("5. Insert node: %s\n", rc == SQLITE_OK ? "OK" : (error_msg ? error_msg : "Unknown error"));
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    
    // Check backing table contents
    printf("6. Check backing tables:\n");
    
    // Check nodes table
    rc = sqlite3_prepare_v2(db, "SELECT * FROM my_graph_nodes", -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        printf("   Nodes:\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("     ID: %lld, Labels: %s, Properties: %s\n", 
                   sqlite3_column_int64(stmt, 0),
                   sqlite3_column_text(stmt, 1) ? (char*)sqlite3_column_text(stmt, 1) : "NULL",
                   sqlite3_column_text(stmt, 2) ? (char*)sqlite3_column_text(stmt, 2) : "NULL");
        }
        sqlite3_finalize(stmt);
    } else {
        printf("   Error querying nodes: %s\n", sqlite3_errmsg(db));
    }
    
    // Check edges table
    rc = sqlite3_prepare_v2(db, "SELECT * FROM my_graph_edges", -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        printf("   Edges:\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("     ID: %lld, Source: %lld, Target: %lld, Type: %s, Weight: %f, Properties: %s\n", 
                   sqlite3_column_int64(stmt, 0),
                   sqlite3_column_int64(stmt, 1),
                   sqlite3_column_int64(stmt, 2),
                   sqlite3_column_text(stmt, 3) ? (char*)sqlite3_column_text(stmt, 3) : "NULL",
                   sqlite3_column_double(stmt, 4),
                   sqlite3_column_text(stmt, 5) ? (char*)sqlite3_column_text(stmt, 5) : "NULL");
        }
        sqlite3_finalize(stmt);
    } else {
        printf("   Error querying edges: %s\n", sqlite3_errmsg(db));
    }
    
    // Try to query virtual table
    printf("7. Query virtual table:\n");
    rc = sqlite3_prepare_v2(db, "SELECT type, id, from_id, to_id, labels, rel_type, weight, properties FROM my_graph", -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("   Type: %s, ID: %lld, From: %lld, To: %lld, Labels: %s, RelType: %s, Weight: %f, Props: %s\n",
                   sqlite3_column_text(stmt, 0) ? (char*)sqlite3_column_text(stmt, 0) : "NULL",
                   sqlite3_column_int64(stmt, 1),
                   sqlite3_column_int64(stmt, 2),
                   sqlite3_column_int64(stmt, 3),
                   sqlite3_column_text(stmt, 4) ? (char*)sqlite3_column_text(stmt, 4) : "NULL",
                   sqlite3_column_text(stmt, 5) ? (char*)sqlite3_column_text(stmt, 5) : "NULL",
                   sqlite3_column_double(stmt, 6),
                   sqlite3_column_text(stmt, 7) ? (char*)sqlite3_column_text(stmt, 7) : "NULL");
        }
        sqlite3_finalize(stmt);
    } else {
        printf("   Error querying virtual table: %s\n", sqlite3_errmsg(db));
    }
    
    sqlite3_close(db);
    return 0;
}
