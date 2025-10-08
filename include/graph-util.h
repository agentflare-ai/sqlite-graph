/*
* Copyright 2018-2024 Redis Labs Ltd. and Contributors
*
* This file is available under the Redis Source Available License 2.0,
* applying the same terms and conditions as the Redis Source Available License 2.0.
* You may not use this file except in compliance with the Redis Source Available License 2.0.
*
* A copy of the Redis Source Available License 2.0 is available at
* https://redis.io/rsal/Redis-Source-Available-License-2.0/
*
* The Redis Source Available License 2.0 is a copy-left license that requires any
* derivative work to be made available under the same terms and conditions.
* 
* See the file LICENSE for more details.
*/

#pragma once

#include "sqlite3ext.h"

/*
** Queue for BFS
*/
typedef struct QueueNode QueueNode;
struct QueueNode {
  sqlite3_int64 iNodeId;
  QueueNode *pNext;
};

typedef struct Queue Queue;
struct Queue {
  QueueNode *pHead;
  QueueNode *pTail;
};

/*
** Queue functions
*/
Queue *graphQueueCreate(void);
void graphQueueDestroy(Queue *pQueue);
int graphQueueEnqueue(Queue *pQueue, sqlite3_int64 iNodeId);
int graphQueueDequeue(Queue *pQueue, sqlite3_int64 *piNodeId);
