/* graph_debug.h - debug logging control
 * Define GRAPH_DEBUG=1 at compile time to enable debug logs.
 */
#ifndef GRAPH_DEBUG_H
#define GRAPH_DEBUG_H

#include <stdio.h>

#ifndef GRAPH_DEBUG
#define GRAPH_DEBUG 0
#endif

#define GDBG(...) do { if (GRAPH_DEBUG) fprintf(stderr, __VA_ARGS__); } while (0)

#endif /* GRAPH_DEBUG_H */
