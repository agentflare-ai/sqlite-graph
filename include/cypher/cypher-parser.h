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

#include "cypher/cypher-ast.h"

typedef struct {
    CypherAst *pAst;
    char *zErrorMsg;
} CypherParser;

// Creates a new parser instance.
CypherParser *cypherParserCreate(void);

// Frees all allocated memory associated with the parser.
void cypherParserDestroy(CypherParser *pParser);

// Main parsing function.
CypherAst *cypherParse(CypherParser *pParser, const char *zQuery, char **pzErrMsg);
