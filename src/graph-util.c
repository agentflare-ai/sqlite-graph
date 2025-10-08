/*
** SQLite Graph Database Extension - Utility Functions
**
** This file implements core storage functions for graph nodes and edges.
** All functions follow SQLite patterns with proper error handling and
** memory management using sqlite3_malloc()/sqlite3_free().
**
** Node operations: Add, remove, get, update nodes with JSON properties
** Edge operations: Add, remove, get edges with weights and properties
*/

#ifdef SQLITE_CORE
#include "sqlite3.h"
#else
#include "sqlite3ext.h"
extern const sqlite3_api_routines *sqlite3_api;
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#endif
#include "graph.h"
#include "graph-memory.h"
#include "graph-util.h"
#include "graph-memory.h"
#include <string.h>
#include <assert.h>

Queue *graphQueueCreate(void){
  Queue *q = sqlite3_malloc(sizeof(Queue));
  if( q==0 ) return 0;
  q->pHead = q->pTail = 0;
  return q;
}

void graphQueueDestroy(Queue *q){
  QueueNode *p = q->pHead;
  while( p ){
    QueueNode *pNext = p->pNext;
    sqlite3_free(p);
    p = pNext;
  }
  sqlite3_free(q);
}

int graphQueueEnqueue(Queue *q, sqlite3_int64 iNodeId){
  QueueNode *p = sqlite3_malloc(sizeof(QueueNode));
  if( p==0 ) return SQLITE_NOMEM;
  p->iNodeId = iNodeId;
  p->pNext = 0;
  if( q->pTail ){
    q->pTail->pNext = p;
  }else{
    q->pHead = p;
  }
  q->pTail = p;
  return SQLITE_OK;
}

int graphQueueDequeue(Queue *q, sqlite3_int64 *piNodeId){
  QueueNode *p = q->pHead;
  if( p==0 ) return SQLITE_DONE;
  *piNodeId = p->iNodeId;
  q->pHead = p->pNext;
  if( q->pHead==0 ) q->pTail = 0;
  sqlite3_free(p);
  return SQLITE_OK;
}