/*
#include <stdarg.h>
** SQLite Graph Database Extension - Memory Management Implementation
**
** Implementation of RAII-style helpers and memory management hardening.
** Eliminates double-frees and ensures consistent use of sqlite3_malloc/sqlite3_free.
*/

#include "graph-memory.h"
#include <stdarg.h>
#include <string.h>

/*
** Initialize a memory management context
*/
void graph_memory_context_init(GraphMemoryContext *ctx) {
  if (!ctx) return;
  ctx->cleanup_list = NULL;
  ctx->is_active = 1;
}

/*
** Register a pointer for automatic cleanup
*/
int graph_memory_auto_free(GraphMemoryContext *ctx, void *ptr, void (*cleanup_func)(void*)) {
  GraphAutoFree *entry;
  
  if (!ctx || !ptr || !cleanup_func) return GRAPH_MEMORY_ERROR;
  if (!ctx->is_active) return GRAPH_MEMORY_ERROR;
  
  entry = sqlite3_malloc(sizeof(GraphAutoFree));
  if (!entry) return GRAPH_MEMORY_NOMEM;
  
  entry->ptr = ptr;
  entry->cleanup_func = cleanup_func;
  entry->next = ctx->cleanup_list;
  ctx->cleanup_list = entry;
  
  return GRAPH_MEMORY_OK;
}

/*
** Cleanup all registered pointers and reset context
*/
void graph_memory_context_cleanup(GraphMemoryContext *ctx) {
  GraphAutoFree *current, *next;
  
  if (!ctx || !ctx->is_active) return;
  
  current = ctx->cleanup_list;
  while (current) {
    next = current->next;
    if (current->ptr && current->cleanup_func) {
      current->cleanup_func(current->ptr);
    }
    sqlite3_free(current);
    current = next;
  }
  
  ctx->cleanup_list = NULL;
  ctx->is_active = 0;
}

/*
** Safe allocation wrapper that integrates with memory context
*/
void* graph_malloc_safe(GraphMemoryContext *ctx, size_t size) {
  void *ptr = sqlite3_malloc((int)size);
  if (ptr && ctx) {
    if (graph_memory_auto_free(ctx, ptr, (void(*)(void*))sqlite3_free) != GRAPH_MEMORY_OK) {
      sqlite3_free(ptr);
      return NULL;
    }
  }
  return ptr;
}

/*
** Safe string formatting with memory context integration
*/
char* graph_mprintf_safe(GraphMemoryContext *ctx, const char *fmt, ...) {
  va_list args;
  char *result;
  
  va_start(args, fmt);
  result = sqlite3_vmprintf(fmt, args);
  va_end(args);
  
  if (result && ctx) {
    if (graph_memory_auto_free(ctx, result, (void(*)(void*))sqlite3_free) != GRAPH_MEMORY_OK) {
      sqlite3_free(result);
      return NULL;
    }
  }
  
  return result;
}

/*
** Create a new node with proper memory management
*/
GraphNode* graph_node_create(GraphMemoryContext *ctx, sqlite3_int64 id, 
                             const char **labels, int num_labels, 
                             const char *properties) {
  GraphNode *node;
  int i;
  
  node = sqlite3_malloc(sizeof(GraphNode));
  if (!node) return NULL;
  
  memset(node, 0, sizeof(GraphNode));
  node->iNodeId = id;
  node->nLabels = num_labels;
  
  /* Allocate and copy labels */
  if (num_labels > 0 && labels) {
    node->azLabels = sqlite3_malloc(sizeof(char*) * num_labels);
    if (!node->azLabels) {
      sqlite3_free(node);
      return NULL;
    }
    
    memset(node->azLabels, 0, sizeof(char*) * num_labels);
    for (i = 0; i < num_labels; i++) {
      if (labels[i]) {
        node->azLabels[i] = sqlite3_mprintf("%s", labels[i]);
        if (!node->azLabels[i]) {
          graph_node_destroy(node);
          return NULL;
        }
      }
    }
  }
  
  /* Copy properties */
  if (properties) {
    node->zProperties = sqlite3_mprintf("%s", properties);
    if (!node->zProperties) {
      graph_node_destroy(node);
      return NULL;
    }
  }
  
  /* Register for automatic cleanup if context provided */
  if (ctx) {
    if (graph_memory_auto_free(ctx, node, (void(*)(void*))graph_node_destroy) != GRAPH_MEMORY_OK) {
      graph_node_destroy(node);
      return NULL;
    }
  }
  
  return node;
}

/*
** Destroy a node and all its allocated memory
*/
void graph_node_destroy(GraphNode *node) {
  int i;
  
  if (!node) return;
  
  /* Free labels */
  if (node->azLabels) {
    for (i = 0; i < node->nLabels; i++) {
      sqlite3_free(node->azLabels[i]);
    }
    sqlite3_free(node->azLabels);
  }
  
  /* Free properties */
  sqlite3_free(node->zProperties);
  
  /* Free the node itself */
  sqlite3_free(node);
}

/*
** Create a new edge with proper memory management
*/
GraphEdge* graph_edge_create(GraphMemoryContext *ctx, sqlite3_int64 edge_id,
                             sqlite3_int64 from_id, sqlite3_int64 to_id,
                             const char *type, double weight, 
                             const char *properties) {
  GraphEdge *edge;
  
  edge = sqlite3_malloc(sizeof(GraphEdge));
  if (!edge) return NULL;
  
  memset(edge, 0, sizeof(GraphEdge));
  edge->iEdgeId = edge_id;
  edge->iFromId = from_id;
  edge->iToId = to_id;
  edge->rWeight = weight;
  
  /* Copy type */
  if (type) {
    edge->zType = sqlite3_mprintf("%s", type);
    if (!edge->zType) {
      sqlite3_free(edge);
      return NULL;
    }
  }
  
  /* Copy properties */
  if (properties) {
    edge->zProperties = sqlite3_mprintf("%s", properties);
    if (!edge->zProperties) {
      sqlite3_free(edge->zType);
      sqlite3_free(edge);
      return NULL;
    }
  }
  
  /* Register for automatic cleanup if context provided */
  if (ctx) {
    if (graph_memory_auto_free(ctx, edge, (void(*)(void*))graph_edge_destroy) != GRAPH_MEMORY_OK) {
      graph_edge_destroy(edge);
      return NULL;
    }
  }
  
  return edge;
}

/*
** Destroy an edge and all its allocated memory
*/
void graph_edge_destroy(GraphEdge *edge) {
  if (!edge) return;
  
  sqlite3_free(edge->zType);
  sqlite3_free(edge->zProperties);
  sqlite3_free(edge);
}

/*
** Safe virtual table destruction
*/
int graph_vtab_destroy_safe(GraphVtab *vtab) {
  if (!vtab) return SQLITE_OK;
  
  /* Free allocated strings */
  sqlite3_free(vtab->zDbName);
  sqlite3_free(vtab->zTableName);
  
  /* Free the vtab structure itself */
  sqlite3_free(vtab);
  
  return SQLITE_OK;
}

/*
** Create a cursor with proper memory management
*/
GraphCursor* graph_cursor_create(GraphMemoryContext *ctx, GraphVtab *vtab) {
  GraphCursor *cursor;
  
  cursor = sqlite3_malloc(sizeof(GraphCursor));
  if (!cursor) return NULL;
  
  memset(cursor, 0, sizeof(GraphCursor));
  cursor->pVtab = vtab;
  cursor->iRowid = 0;
  cursor->iIterMode = 0; /* Default to node iteration */
  
  /* Register for automatic cleanup if context provided */
  if (ctx) {
    if (graph_memory_auto_free(ctx, cursor, (void(*)(void*))graph_cursor_destroy) != GRAPH_MEMORY_OK) {
      sqlite3_free(cursor);
      return NULL;
    }
  }
  
  return cursor;
}

/*
** Destroy a cursor and clean up resources
*/
void graph_cursor_destroy(GraphCursor *cursor) {
  if (!cursor) return;
  
  /* Finalize any prepared statements */
  if (cursor->pNodeStmt) {
    sqlite3_finalize(cursor->pNodeStmt);
  }
  if (cursor->pEdgeStmt) {
    sqlite3_finalize(cursor->pEdgeStmt);
  }
  
  sqlite3_free(cursor);
}

#ifdef GRAPH_DEBUG_MEMORY
static int memory_debug_active = 0;
static int allocation_count = 0;
static int deallocation_count = 0;

void graph_memory_debug_init(void) {
  memory_debug_active = 1;
  allocation_count = 0;
  deallocation_count = 0;
}

void graph_memory_debug_report(void) {
  if (memory_debug_active) {
    fprintf(stderr, "Memory Debug: Allocations=%d, Deallocations=%d, Leaks=%d\n",
            allocation_count, deallocation_count, 
            allocation_count - deallocation_count);
  }
}

int graph_memory_validate_ptr(void *ptr) {
  /* Basic validation - in a real implementation, this would 
     track allocated pointers */
  return ptr != NULL;
}
#endif
