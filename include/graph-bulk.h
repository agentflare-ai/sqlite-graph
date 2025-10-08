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

#include <sqlite3.h>

/* Bulk loader configuration */
typedef struct BulkLoaderConfig {
    int batchSize;               /* Nodes/edges per transaction */
    int deferIndexing;           /* Defer index updates */
    int parallelImport;          /* Use parallel processing */
    int validateData;            /* Validate during import */
    int compressProperties;      /* Enable property compression */
    void (*progressCallback)(int percent, void *arg);
    void *progressArg;
} BulkLoaderConfig;

/* Bulk load statistics */
typedef struct BulkLoadStats {
    sqlite3_int64 nodesLoaded;
    sqlite3_int64 edgesLoaded;
    sqlite3_int64 nodesSkipped;
    sqlite3_int64 edgesSkipped;
    sqlite3_int64 bytesProcessed;
    double elapsedTime;
    char *lastError;
} BulkLoadStats;
