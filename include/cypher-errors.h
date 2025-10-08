/*
** cypher-errors.h - openCypher-compliant error codes and messages
**
** This file defines standardized error codes and messages following
** the openCypher specification for consistent error reporting.
*/

#ifndef CYPHER_ERRORS_H
#define CYPHER_ERRORS_H

/* Error categories following openCypher specification */
#define CYPHER_ERROR_SYNTAX         1000
#define CYPHER_ERROR_SEMANTIC       2000
#define CYPHER_ERROR_TYPE           3000
#define CYPHER_ERROR_RUNTIME        4000
#define CYPHER_ERROR_CONSTRAINT     5000
#define CYPHER_ERROR_TRANSACTION    6000

/* Syntax errors (1xxx) */
#define CYPHER_ERROR_SYNTAX_GENERAL              1001
#define CYPHER_ERROR_SYNTAX_INVALID_TOKEN        1002
#define CYPHER_ERROR_SYNTAX_UNEXPECTED_TOKEN     1003
#define CYPHER_ERROR_SYNTAX_MISSING_TOKEN        1004
#define CYPHER_ERROR_SYNTAX_INVALID_EXPRESSION   1005
#define CYPHER_ERROR_SYNTAX_INVALID_PATTERN      1006

/* Semantic errors (2xxx) */
#define CYPHER_ERROR_SEMANTIC_UNDEFINED_VARIABLE 2001
#define CYPHER_ERROR_SEMANTIC_VARIABLE_REDEFINED 2002
#define CYPHER_ERROR_SEMANTIC_UNDEFINED_LABEL    2003
#define CYPHER_ERROR_SEMANTIC_UNDEFINED_TYPE     2004
#define CYPHER_ERROR_SEMANTIC_UNDEFINED_PROPERTY 2005
#define CYPHER_ERROR_SEMANTIC_UNDEFINED_FUNCTION 2006
#define CYPHER_ERROR_SEMANTIC_INVALID_ARGUMENT   2007

/* Type errors (3xxx) */
#define CYPHER_ERROR_TYPE_MISMATCH               3001
#define CYPHER_ERROR_TYPE_INVALID_OPERATION      3002
#define CYPHER_ERROR_TYPE_INVALID_CONVERSION     3003
#define CYPHER_ERROR_TYPE_INVALID_COMPARISON     3004
#define CYPHER_ERROR_TYPE_INVALID_ARGUMENT_TYPE  3005

/* Runtime errors (4xxx) */
#define CYPHER_ERROR_RUNTIME_GENERAL             4001
#define CYPHER_ERROR_RUNTIME_NODE_NOT_FOUND      4002
#define CYPHER_ERROR_RUNTIME_RELATIONSHIP_NOT_FOUND 4003
#define CYPHER_ERROR_RUNTIME_PROPERTY_NOT_FOUND  4004
#define CYPHER_ERROR_RUNTIME_INDEX_OUT_OF_BOUNDS 4005
#define CYPHER_ERROR_RUNTIME_DIVISION_BY_ZERO    4006
#define CYPHER_ERROR_RUNTIME_OUT_OF_MEMORY       4007
#define CYPHER_ERROR_RUNTIME_OVERFLOW            4008

/* Constraint errors (5xxx) */
#define CYPHER_ERROR_CONSTRAINT_VIOLATION        5001
#define CYPHER_ERROR_CONSTRAINT_UNIQUE           5002
#define CYPHER_ERROR_CONSTRAINT_NODE_EXISTS      5003
#define CYPHER_ERROR_CONSTRAINT_REQUIRED         5004
#define CYPHER_ERROR_CONSTRAINT_DELETE_CONNECTED 5005

/* Transaction errors (6xxx) */
#define CYPHER_ERROR_TRANSACTION_FAILED          6001
#define CYPHER_ERROR_TRANSACTION_ROLLBACK        6002
#define CYPHER_ERROR_TRANSACTION_DEADLOCK        6003
#define CYPHER_ERROR_TRANSACTION_NOT_FOUND       6004

/* Error structure for detailed error reporting */
typedef struct CypherError {
    int code;                    /* Error code from above */
    const char *category;        /* Error category name */
    const char *title;          /* Short error title */
    const char *message;        /* Detailed error message */
    int line;                   /* Line number where error occurred */
    int column;                 /* Column number where error occurred */
    const char *context;        /* Query context around error */
} CypherError;

/* Error handling functions */
CypherError* cypherErrorCreate(int code, const char *message, 
                              int line, int column);
void cypherErrorDestroy(CypherError *error);
const char* cypherErrorGetCategory(int code);
const char* cypherErrorGetTitle(int code);
char* cypherErrorFormat(CypherError *error);

/* Error reporting macros */
#define CYPHER_SET_ERROR(ctx, code, msg) \
    cypherSetError(ctx, code, msg, __LINE__, 0)

#define CYPHER_SET_ERROR_LOC(ctx, code, msg, line, col) \
    cypherSetError(ctx, code, msg, line, col)

/* Context error handling */
void cypherSetError(void *context, int code, const char *message,
                   int line, int column);
CypherError* cypherGetLastError(void *context);
void cypherClearError(void *context);

#endif /* CYPHER_ERRORS_H */