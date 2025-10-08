/*
** sqlite3_noext.h - SQLite header for non-main extension files
**
** This header should be included by all source files EXCEPT graph.c
** It provides SQLite definitions without SQLITE_EXTENSION_INIT1
*/
#ifndef SQLITE3_NOEXT_H
#define SQLITE3_NOEXT_H

#include "sqlite3ext.h"

/* The sqlite3_api pointer is defined in graph.c and should be
** declared as extern in all other files */
extern const sqlite3_api_routines *sqlite3_api;

#endif /* SQLITE3_NOEXT_H */