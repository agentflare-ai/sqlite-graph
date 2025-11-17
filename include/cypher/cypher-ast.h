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

// AST Node Types
typedef enum {
    CYPHER_AST_QUERY = 0,
    CYPHER_AST_SINGLE_QUERY,
    CYPHER_AST_UNION,
    CYPHER_AST_MATCH,
    CYPHER_AST_OPTIONAL_MATCH,
    CYPHER_AST_WHERE,
    CYPHER_AST_RETURN,
    CYPHER_AST_PROJECTION_LIST,
    CYPHER_AST_PROJECTION_ITEM,
    CYPHER_AST_ORDER_BY,
    CYPHER_AST_SORT_LIST,
    CYPHER_AST_SORT_ITEM,
    CYPHER_AST_LIMIT,
    CYPHER_AST_SKIP,
    CYPHER_AST_PATTERN,
    CYPHER_AST_PATTERN_LIST,
    CYPHER_AST_NODE_PATTERN,
    CYPHER_AST_REL_PATTERN,
    CYPHER_AST_LABELS,
    CYPHER_AST_PATH,
    CYPHER_AST_IDENTIFIER,
    CYPHER_AST_LITERAL,
    CYPHER_AST_UNARY_OP,
    CYPHER_AST_BINARY_OP,
    CYPHER_AST_PROPERTY,
    CYPHER_AST_MAP,
    CYPHER_AST_PROPERTY_PAIR,
    CYPHER_AST_LIST,
    CYPHER_AST_FUNCTION_CALL,
    CYPHER_AST_CASE,
    CYPHER_AST_AND,
    CYPHER_AST_OR,
    CYPHER_AST_NOT,
    CYPHER_AST_COMPARISON,
    CYPHER_AST_ADDITIVE,
    CYPHER_AST_MULTIPLICATIVE,
    CYPHER_AST_UNARY_PLUS,
    CYPHER_AST_UNARY_MINUS,
    CYPHER_AST_COUNT // Sentinel for max AST node type
} CypherAstNodeType;

// AST Node Structure
typedef struct CypherAst {
    CypherAstNodeType type;
    char *zValue; // Value for literals, identifiers, operators (e.g., "+", "Person")
    struct CypherAst **apChildren; // Array of child AST nodes
    int nChildren; // Number of children
    int nChildrenAlloc; // Allocated size of apChildren
    int iLine; // Line number from source
    int iColumn; // Column number from source
    int iFlags; // General purpose flags (e.g., DISTINCT for RETURN clause)
} CypherAst;

// AST Node creation functions
CypherAst *cypherAstCreate(CypherAstNodeType type, int iLine, int iColumn);
CypherAst *cypherAstCreateIdentifier(const char *zName, int iLine, int iColumn);
CypherAst *cypherAstCreateLiteral(const char *zValue, int iLine, int iColumn);
CypherAst *cypherAstCreateBinaryOp(const char *zOp, CypherAst *pLeft, CypherAst *pRight, int iLine, int iColumn);
CypherAst *cypherAstCreateProperty(CypherAst *pObj, const char *zProp, int iLine, int iColumn);
CypherAst *cypherAstCreateNodeLabel(const char *zName, int iLine, int iColumn);
CypherAst *cypherAstCreateUnaryOp(const char *zOp, CypherAst *pExpr, int iLine, int iColumn);

// AST Node manipulation functions
void cypherAstAddChild(CypherAst *pParent, CypherAst *pChild);
void cypherAstSetValue(CypherAst *pNode, const char *zValue);
CypherAst *cypherAstGetChild(CypherAst *pNode, int iChild);
int cypherAstGetChildCount(CypherAst *pNode);
const char *cypherAstGetValue(CypherAst *pNode);

// AST Node destruction
void cypherAstDestroy(CypherAst *pNode);

// Debugging
void cypherAstPrint(CypherAst *pNode, int iIndent);
const char *cypherAstNodeTypeName(CypherAstNodeType type);
