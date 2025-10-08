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

// An enum of all token types that can be produced by the Cypher lexer.
typedef enum {
    CYPHER_TOK_EOF = 0,
    CYPHER_TOK_ERROR,
    CYPHER_TOK_WHITESPACE, // To be skipped
    CYPHER_TOK_COMMENT,    // To be skipped

    // Keywords
    CYPHER_TOK_MATCH,
    CYPHER_TOK_OPTIONAL,
    CYPHER_TOK_WHERE,
    CYPHER_TOK_RETURN,
    CYPHER_TOK_CREATE,
    CYPHER_TOK_MERGE,
    CYPHER_TOK_SET,
    CYPHER_TOK_DELETE,
    CYPHER_TOK_DETACH,
    CYPHER_TOK_REMOVE,
    CYPHER_TOK_WITH,
    CYPHER_TOK_UNION,
    CYPHER_TOK_AS,
    CYPHER_TOK_ORDER,
    CYPHER_TOK_BY,
    CYPHER_TOK_ASC,
    CYPHER_TOK_DESC,
    CYPHER_TOK_LIMIT,
    CYPHER_TOK_SKIP,
    CYPHER_TOK_DISTINCT,
    CYPHER_TOK_AND,
    CYPHER_TOK_OR,
    CYPHER_TOK_XOR,
    CYPHER_TOK_NOT,
    CYPHER_TOK_IN,
    CYPHER_TOK_STARTS_WITH,
    CYPHER_TOK_ENDS_WITH,
    CYPHER_TOK_CONTAINS,
    CYPHER_TOK_IS_NULL,
    CYPHER_TOK_IS_NOT_NULL,
    CYPHER_TOK_NULL, // Actual NULL keyword

    // Operators
    CYPHER_TOK_EQ,  // =
    CYPHER_TOK_NE,  // <>
    CYPHER_TOK_LT,  // <
    CYPHER_TOK_LE,  // <=
    CYPHER_TOK_GT,  // >
    CYPHER_TOK_GE,  // >=
    CYPHER_TOK_PLUS, // +
    CYPHER_TOK_MINUS, // -
    CYPHER_TOK_MULT, // *
    CYPHER_TOK_DIV,  // /
    CYPHER_TOK_MOD,  // %
    CYPHER_TOK_POW,  // ^
    CYPHER_TOK_DOT,  // .
    CYPHER_TOK_COLON, // :
    CYPHER_TOK_COMMA, // ,
    CYPHER_TOK_SEMICOLON, // ;
    CYPHER_TOK_LPAREN, // (
    CYPHER_TOK_RPAREN, // )
    CYPHER_TOK_LBRACKET, // [
    CYPHER_TOK_RBRACKET, // ]
    CYPHER_TOK_LBRACE, // {
    CYPHER_TOK_RBRACE, // }
    CYPHER_TOK_DASH, // - (for relationships)
    CYPHER_TOK_ARROW_RIGHT, // ->
    CYPHER_TOK_ARROW_LEFT, // <-
    CYPHER_TOK_ARROW_BOTH, // <->
    CYPHER_TOK_PIPE, // | (for ranges)
    CYPHER_TOK_REGEX, // =~
    CYPHER_TOK_DOLLAR, // $ (for parameters)

    // Literals
    CYPHER_TOK_INTEGER,
    CYPHER_TOK_FLOAT,
    CYPHER_TOK_STRING,
    CYPHER_TOK_BOOLEAN, // TRUE, FALSE

    // Identifiers
    CYPHER_TOK_IDENTIFIER,
    CYPHER_TOK_LABEL,    // For node/relationship labels
    CYPHER_TOK_PROPERTY, // For property names
    CYPHER_TOK_REL_TYPE, // For relationship types

    CYPHER_TOK_MAX // Sentinel for max token value
} CypherTokenType;

// Represents a single token in a Cypher query.
typedef struct {
    CypherTokenType type;
    const char *text;
    int len;
    unsigned int line;
    unsigned int column;
} CypherToken;
