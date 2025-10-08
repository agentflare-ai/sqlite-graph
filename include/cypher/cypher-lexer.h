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

#include "cypher/cypher-token.h"

// Lexer context structure
typedef struct CypherLexer {
    const char *zInput;
    int iPos;
    int iLine;
    int iColumn;
    char *zErrorMsg;
    CypherToken *pLastToken; // Last token returned
} CypherLexer;

// Initializes a new lexer instance.
CypherLexer *cypherLexerCreate(const char *zInput);

// Frees all allocated memory associated with the lexer.
void cypherLexerDestroy(CypherLexer *pLexer);

// Returns the next token in the stream.
CypherToken *cypherLexerNextToken(CypherLexer *pLexer);
