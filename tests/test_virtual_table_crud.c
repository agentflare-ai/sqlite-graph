#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static sqlite3 *db = NULL;
static char *error_msg = NULL;

void execute_sql(const char *sql, const char *description) {
    int rc = sqlite3_exec(db, sql, NULL, NULL, &error_msg);
    printf("%s: %s\n", description, rc == SQLITE_OK ? "OK" : (error_msg ? error_msg : "Unknown error"));
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
}

void query_and_print(const char *sql, const char *description) {
    printf("%s:\n", description);
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int cols = sqlite3_column_count(stmt);
            for (int i = 0; i < cols; i++) {
                if (i > 0) printf(", ");
                const char *text = (const char*)sqlite3_column_text(stmt, i);
                printf("%s", text ? text : "NULL");
            }
            printf("\n");
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error: %s\n", sqlite3_errmsg(db));
    }
}

int main(void) {
    // 1. Setup
    int rc = sqlite3_open(":memory:", &db);
    printf("1. Open database: %s\n", rc == SQLITE_OK ? "OK" : sqlite3_errmsg(db));
    
    sqlite3_enable_load_extension(db, 1);
    
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
    
    // 2. Create virtual table
    execute_sql("CREATE VIRTUAL TABLE graph USING graph();", "3. Create virtual table");
    
    // 3. Insert nodes
    printf("\n=== TESTING NODE OPERATIONS ===\n");
    execute_sql("INSERT INTO graph (type, properties) VALUES ('node', '{\"name\": \"Alice\", \"age\": 30}');", "4. Insert node Alice");
    execute_sql("INSERT INTO graph (type, properties) VALUES ('node', '{\"name\": \"Bob\", \"age\": 25}');", "5. Insert node Bob");
    execute_sql("INSERT INTO graph (type, labels, properties) VALUES ('node', '[\"Person\", \"User\"]', '{\"name\": \"Charlie\", \"role\": \"admin\"}');", "6. Insert node Charlie with labels");
    
    // 4. Query nodes
    query_and_print("SELECT type, id, labels, properties FROM graph WHERE type = 'node'", "7. Query all nodes");
    
    // 5. Insert edges
    printf("\n=== TESTING EDGE OPERATIONS ===\n");
    execute_sql("INSERT INTO graph (type, from_id, to_id, rel_type, weight, properties) VALUES ('edge', 1, 2, 'KNOWS', 0.8, '{\"since\": \"2020\"}');", "8. Insert edge Alice->Bob");
    execute_sql("INSERT INTO graph (type, from_id, to_id, rel_type, weight, properties) VALUES ('edge', 2, 3, 'FOLLOWS', 0.5, '{\"platform\": \"twitter\"}');", "9. Insert edge Bob->Charlie");
    execute_sql("INSERT INTO graph (type, from_id, to_id, rel_type, weight) VALUES ('edge', 1, 3, 'MANAGED_BY', 1.0);", "10. Insert edge Alice->Charlie");
    
    // 6. Query edges
    query_and_print("SELECT type, id, from_id, to_id, rel_type, weight, properties FROM graph WHERE type = 'edge'", "11. Query all edges");
    
    // 7. Query all data
    printf("\n=== TESTING COMBINED QUERIES ===\n");
    query_and_print("SELECT type, id, from_id, to_id, labels, rel_type, weight, properties FROM graph ORDER BY type, id", "12. Query all nodes and edges");
    
    // 8. Test specific node/edge queries
    query_and_print("SELECT * FROM graph WHERE id = 1", "13. Query node by ID");
    query_and_print("SELECT * FROM graph WHERE from_id = 1", "14. Query edges from node 1");
    
    // 9. Update operations
    printf("\n=== TESTING UPDATE OPERATIONS ===\n");
    execute_sql("UPDATE graph SET properties = '{\"name\": \"Alice Smith\", \"age\": 31, \"updated\": true}' WHERE id = 1 AND type = 'node'", "15. Update node properties");
    query_and_print("SELECT type, id, properties FROM graph WHERE id = 1", "16. Verify node update");
    
    // 10. Delete operations
    printf("\n=== TESTING DELETE OPERATIONS ===\n");
    execute_sql("DELETE FROM graph WHERE id = 3 AND type = 'node'", "17. Delete node Charlie");
    query_and_print("SELECT COUNT(*) FROM graph WHERE type = 'node'", "18. Count nodes after delete");
    query_and_print("SELECT COUNT(*) FROM graph WHERE type = 'edge'", "19. Count edges after delete (should still be 3)");
    
    // 11. Check backing tables directly
    printf("\n=== TESTING BACKING TABLES ===\n");
    query_and_print("SELECT id, labels, properties FROM graph_nodes", "20. Direct query of nodes table");
    query_and_print("SELECT id, source, target, edge_type, weight, properties FROM graph_edges", "21. Direct query of edges table");
    
    // 12. Test error cases
    printf("\n=== TESTING ERROR CASES ===\n");
    execute_sql("INSERT INTO graph (type, from_id, to_id, rel_type) VALUES ('edge', 999, 1000, 'INVALID');", "22. Try to insert edge with non-existent nodes (should fail)");
    
    sqlite3_close(db);
    printf("\n=== ALL TESTS COMPLETED ===\n");
    return 0;
}
