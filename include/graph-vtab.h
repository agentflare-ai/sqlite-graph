/*
** SQLite Graph Database Extension - Virtual Table Declarations
**
** This file contains virtual table function declarations and module
** definitions for the SQLite graph database extension.
**
** Virtual table methods follow SQLite's vtab interface exactly.
** All functions return SQLite error codes and handle OOM gracefully.
*/
#ifndef GRAPH_VTAB_H
#define GRAPH_VTAB_H

#include "graph.h"

/*
** Virtual table module methods.
** These implement the sqlite3_module interface for graph tables.
*/

/*
** Create a new virtual table instance.
** Called when CREATE VIRTUAL TABLE is executed.
** Memory allocation: Creates new GraphVtab with sqlite3_malloc().
** Error handling: Returns SQLITE_NOMEM on allocation failure.
*/
int graphCreate(sqlite3 *pDb, void *pAux, int argc, 
                const char *const *argv, sqlite3_vtab **ppVtab, 
                char **pzErr);

/*
** Connect to an existing virtual table.
** Called when accessing existing virtual table.
** Reference counting: Increments nRef on successful connection.
*/
int graphConnect(sqlite3 *pDb, void *pAux, int argc,
                 const char *const *argv, sqlite3_vtab **ppVtab,
                 char **pzErr);

/*
** Query planner interface.
** Provides cost estimates and index usage hints to SQLite.
** Performance: Critical for query optimization.
*/
int graphBestIndex(sqlite3_vtab *pVtab, sqlite3_index_info *pInfo);

/*
** Disconnect from virtual table.
** Reference counting: Decrements nRef, cleans up if zero.
*/
int graphDisconnect(sqlite3_vtab *pVtab);

/*
** Destroy virtual table instance.
** Memory management: Frees all nodes, edges, and vtab structure.
** Called during DROP TABLE or database close.
*/
int graphDestroy(sqlite3_vtab *pVtab);

/*
** Open a cursor for table iteration.
** Memory allocation: Creates new GraphCursor with sqlite3_malloc().
** Cursor state: Initializes position to beginning of iteration.
*/
int graphOpen(sqlite3_vtab *pVtab, sqlite3_vtab_cursor **ppCursor);

/*
** Close cursor and free resources.
** Memory management: Calls sqlite3_free() on cursor structure.
*/
int graphClose(sqlite3_vtab_cursor *pCursor);

/*
** Filter cursor based on constraints.
** Query processing: Implements WHERE clause filtering.
** Performance: Optimizes iteration based on provided constraints.
*/
int graphFilter(sqlite3_vtab_cursor *pCursor, int idxNum,
                const char *idxStr, int argc, sqlite3_value **argv);

/*
** Move cursor to next row.
** Iteration: Advances through nodes or edges based on mode.
** End detection: Sets cursor state for graphEof() check.
*/
int graphNext(sqlite3_vtab_cursor *pCursor);

/*
** Check if cursor is at end of iteration.
** Returns: Non-zero if no more rows, zero if rows remain.
*/
int graphEof(sqlite3_vtab_cursor *pCursor);

/*
** Return column value for current cursor position.
** Data retrieval: Extracts node/edge properties as SQLite values.
** Memory management: Uses sqlite3_result_* functions appropriately.
*/
int graphColumn(sqlite3_vtab_cursor *pCursor, sqlite3_context *pCtx,
                int iCol);

/*
** Return rowid for current cursor position.
** Unique identification: Provides stable row identifier.
*/
int graphRowid(sqlite3_vtab_cursor *pCursor, sqlite3_int64 *pRowid);

/*
** Accessor for the graph virtual table module structure.
** Returns a pointer to the sqlite3_module implementing the graph vtab.
*/
const sqlite3_module *graphGetModule(void);

#endif /* GRAPH_VTAB_H */