/*
** SQLite Graph Database Extension - Memory Management Hardening
**
** This file provides RAII-style helpers and consistent memory management
** for all graph objects to eliminate double-frees and memory leaks.
**
** All allocation/deallocation must use sqlite3_malloc/sqlite3_free for
** consistency with SQLite's memory management.
*/

#ifndef GRAPH_MEMORY_H
#define GRAPH_MEMORY_H

#include <stddef.h>
#include "sqlite3.h"
#include "graph.h"

/*
** Memory management error codes
*/
#define GRAPH_MEMORY_OK         SQLITE_OK
#define GRAPH_MEMORY_NOMEM      SQLITE_NOMEM
#define GRAPH_MEMORY_ERROR      SQLITE_ERROR

/*
** RAII-style helper for automatic cleanup of graph objects
*/
typedef struct GraphAutoFree GraphAutoFree;
struct GraphAutoFree {
  void *ptr;
  void (*cleanup_func)(void*);
  GraphAutoFree *next;
};

/*
** Context for managing multiple auto-cleanup objects
*/
typedef struct GraphMemoryContext GraphMemoryContext;
struct GraphMemoryContext {
  GraphAutoFree *cleanup_list;
  int is_active;
};

/*
** Initialize a memory management context
*/
void graph_memory_context_init(GraphMemoryContext *ctx);

/*
** Register a pointer for automatic cleanup
*/
int graph_memory_auto_free(GraphMemoryContext *ctx, void *ptr, void (*cleanup_func)(void*));

/*
** Cleanup all registered pointers and reset context
*/
void graph_memory_context_cleanup(GraphMemoryContext *ctx);

/*
** RAII-style macros for common patterns
*/
#define GRAPH_AUTO_FREE(ctx, ptr) graph_memory_auto_free(ctx, ptr, (void(*)(void*))sqlite3_free)
#define GRAPH_AUTO_NODE(ctx, node) graph_memory_auto_free(ctx, node, (void(*)(void*))graph_node_destroy)
#define GRAPH_AUTO_EDGE(ctx, edge) graph_memory_auto_free(ctx, edge, (void(*)(void*))graph_edge_destroy)

/*
** Safe allocation wrappers that integrate with memory context
*/
void* graph_malloc_safe(GraphMemoryContext *ctx, size_t size);
char* graph_mprintf_safe(GraphMemoryContext *ctx, const char *fmt, ...);

/*
** Graph object lifecycle management with proper destructors
*/

/*
** Create a new node with automatic cleanup registration
*/
GraphNode* graph_node_create(GraphMemoryContext *ctx, sqlite3_int64 id, 
                             const char **labels, int num_labels, 
                             const char *properties);

/*
** Destroy a node and all its allocated memory
*/
void graph_node_destroy(GraphNode *node);

/*
** Create a new edge with automatic cleanup registration  
*/
GraphEdge* graph_edge_create(GraphMemoryContext *ctx, sqlite3_int64 edge_id,
                             sqlite3_int64 from_id, sqlite3_int64 to_id,
                             const char *type, double weight, 
                             const char *properties);

/*
** Destroy an edge and all its allocated memory
*/
void graph_edge_destroy(GraphEdge *edge);

/*
** Virtual table lifecycle with proper destructor callbacks
*/

/*
** Safe virtual table destruction with SQLite destructor callback
*/
int graph_vtab_destroy_safe(GraphVtab *vtab);

/*
** Cursor lifecycle management
*/
GraphCursor* graph_cursor_create(GraphMemoryContext *ctx, GraphVtab *vtab);
void graph_cursor_destroy(GraphCursor *cursor);

/*
** Memory debugging and validation (enabled in debug builds)
*/
#ifdef GRAPH_DEBUG_MEMORY
void graph_memory_debug_init(void);
void graph_memory_debug_report(void);
int graph_memory_validate_ptr(void *ptr);
#else
#define graph_memory_debug_init() ((void)0)
#define graph_memory_debug_report() ((void)0)
#define graph_memory_validate_ptr(ptr) (1)
#endif

/*
** Convenience macros for memory-safe function patterns
*/
#define GRAPH_MEMORY_GUARD_BEGIN(ctx) \
  GraphMemoryContext ctx; \
  graph_memory_context_init(&ctx)

#define GRAPH_MEMORY_GUARD_END(ctx) \
  graph_memory_context_cleanup(&ctx)

#define GRAPH_MEMORY_GUARD_RETURN(ctx, retval) do { \
  graph_memory_context_cleanup(&ctx); \
  return (retval); \
} while(0)

#endif /* GRAPH_MEMORY_H */
