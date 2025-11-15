#include <sqlite3.h>
#include <stdio.h>

#ifdef __APPLE__
#define GRAPH_LIB_PATH "../build/libgraph.dylib"
#else
#define GRAPH_LIB_PATH "../build/libgraph.so"
#endif

int main(void) {
  sqlite3 *db = NULL;
  char *err_msg = NULL;
  int rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to open SQLite database: %s\n", sqlite3_errmsg(db));
    return rc;
  }

  sqlite3_enable_load_extension(db, 1);
  rc = sqlite3_load_extension(db, GRAPH_LIB_PATH, "sqlite3_graph_init", &err_msg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to load graph extension: %s\n", err_msg ? err_msg : "unknown error");
    if (err_msg) {
      sqlite3_free(err_msg);
    }
    sqlite3_close(db);
    return rc;
  }

  printf("Graph extension loaded. Invoke generated tck_test_* binaries for detailed coverage.\n");

  if (err_msg) {
    sqlite3_free(err_msg);
  }
  sqlite3_close(db);
  return 0;
}
