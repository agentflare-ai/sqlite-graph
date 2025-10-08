/*
** graph-internal.h - Internal header for SQLite Graph Extension
**
** This header should be included by all graph source files EXCEPT graph.c
** It includes SQLite headers without SQLITE_EXTENSION_INIT1 to avoid
** multiple definition errors.
*/
#ifndef GRAPH_INTERNAL_H
#define GRAPH_INTERNAL_H

#include <sqlite3.h>
#include "sqlite3ext.h"

/* Do NOT define SQLITE_EXTENSION_INIT1 here - it's only in graph.c */

#endif /* GRAPH_INTERNAL_H */