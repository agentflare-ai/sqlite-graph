#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <sqlite3.h>
#include <stdio.h>

/* Helpers shared across Unity test suites. */
static inline int test_open_db(sqlite3 **db) {
  int rc = sqlite3_open(":memory:", db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to open in-memory database: %d\n", rc);
  }
  return rc;
}

static inline int test_enable_extensions(sqlite3 *db) {
  int rc = sqlite3_enable_load_extension(db, 1);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to enable extension loading: %d\n", rc);
  }
  return rc;
}

static inline const char *test_graph_lib(void) {
#ifdef __APPLE__
  return "../build/libgraph.dylib";
#else
  return "../build/libgraph.so";
#endif
}

static inline int test_load_graph_extension(sqlite3 *db, char **err) {
  int rc = sqlite3_load_extension(db, test_graph_lib(), "sqlite3_graph_init", err);
  if (rc != SQLITE_OK) {
    if (err && *err) {
      fprintf(stderr, "Failed to load graph extension: %s\n", *err);
    } else {
      fprintf(stderr, "Failed to load graph extension (code %d)\n", rc);
    }
  }
  return rc;
}

static inline int test_create_graph_vtab(sqlite3 *db, char **err) {
  int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE IF NOT EXISTS graph USING graph()",
                        NULL, NULL, err);
  if (rc != SQLITE_OK) {
    if (err && *err) {
      fprintf(stderr, "Failed to create graph vtab: %s\n", *err);
    } else {
      fprintf(stderr, "Failed to create graph vtab (code %d)\n", rc);
    }
  }
  return rc;
}

static inline int test_prepare_graph(sqlite3 **db, char **err) {
  int rc = test_open_db(db);
  if (rc != SQLITE_OK) {
    return rc;
  }
  rc = test_enable_extensions(*db);
  if (rc != SQLITE_OK) {
    return rc;
  }
  rc = test_load_graph_extension(*db, err);
  if (rc != SQLITE_OK) {
    return rc;
  }
  return test_create_graph_vtab(*db, err);
}

static inline int test_exec_sql(sqlite3 *db, const char *sql, char **err) {
  int rc = sqlite3_exec(db, sql, NULL, NULL, err);
  if (rc != SQLITE_OK && err && *err) {
    fprintf(stderr, "SQL error: %s\n", *err);
  }
  return rc;
}

static inline void test_clear_error(char **err) {
  if (err && *err) {
    sqlite3_free(*err);
    *err = NULL;
  }
}

static inline void test_cleanup(sqlite3 **db, char **err) {
  test_clear_error(err);
  if (db && *db) {
    sqlite3_close(*db);
    *db = NULL;
  }
}

#endif /* TEST_COMMON_H */
