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

// SQLite Extension API handling - check if we're in extension mode
#ifdef SQLITE_EXTENSION_BUILD
#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
// Use extension API functions
#define CYPHER_MALLOC(x) sqlite3_malloc(x)
#define CYPHER_FREE(x) sqlite3_free(x)
#define CYPHER_REALLOC(x, y) sqlite3_realloc(x, y)
#else
// Use standard library functions for standalone/test mode
#include <sqlite3.h>
#include <stdlib.h>
#define CYPHER_MALLOC(x) sqlite3_malloc(x)
#define CYPHER_FREE(x) sqlite3_free(x)
#define CYPHER_REALLOC(x, y) sqlite3_realloc(x, y)
#endif

#include "cypher.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

// Initializes a new lexer instance.
CypherLexer *cypherLexerCreate(const char *zInput) {
    if (!zInput) {
        return NULL;
    }
    CypherLexer *pLexer = (CypherLexer *)CYPHER_MALLOC(sizeof(CypherLexer));
    if (!pLexer) {
        return NULL;
    }
    
    pLexer->zInput = zInput;
    pLexer->iPos = 0;
    pLexer->iLine = 1;
    pLexer->iColumn = 1;
    pLexer->zErrorMsg = NULL;
    pLexer->pLastToken = NULL;
    return pLexer;
}

// Frees all allocated memory associated with the lexer.
void cypherLexerDestroy(CypherLexer *pLexer) {
    if (!pLexer) return;
    if (pLexer->zErrorMsg) {
        CYPHER_FREE(pLexer->zErrorMsg);
    }
    if (pLexer->pLastToken) {
        CYPHER_FREE(pLexer->pLastToken);
    }
    CYPHER_FREE(pLexer);
}

static char lexerPeek(CypherLexer *pLexer, int offset) {
    if (pLexer->iPos + offset >= (int)strlen(pLexer->zInput)) {
        return '\0';
    }
    return pLexer->zInput[pLexer->iPos + offset];
}

static char lexerNext(CypherLexer *pLexer) {
    char c = pLexer->zInput[pLexer->iPos];
    if (c == '\n') {
        pLexer->iLine++;
        pLexer->iColumn = 1;
    } else {
        pLexer->iColumn++;
    }
    pLexer->iPos++;
    return c;
}

static void lexerSkipWhitespace(CypherLexer *pLexer) {
    while (isspace(lexerPeek(pLexer, 0))) {
        lexerNext(pLexer);
    }
}

static void lexerSkipComment(CypherLexer *pLexer) {
    if (lexerPeek(pLexer, 0) == '/' && lexerPeek(pLexer, 1) == '/') {
        while (lexerPeek(pLexer, 0) != '\n' && lexerPeek(pLexer, 0) != '\0') {
            lexerNext(pLexer);
        }
    } else if (lexerPeek(pLexer, 0) == '/' && lexerPeek(pLexer, 1) == '*') {
        lexerNext(pLexer); // consume /
        lexerNext(pLexer); // consume *
        while (lexerPeek(pLexer, 0) != '*' || lexerPeek(pLexer, 1) != '/') {
            if (lexerPeek(pLexer, 0) == '\0') {
                // Unterminated comment
                return;
            }
            lexerNext(pLexer);
        }
        lexerNext(pLexer); // consume *
        lexerNext(pLexer); // consume /
    }
}

static CypherToken *lexerAddToken(CypherLexer *pLexer, CypherTokenType type, int startPos, int endPos) {
    if (pLexer->pLastToken) {
        CYPHER_FREE(pLexer->pLastToken);
    }
    CypherToken *pToken = (CypherToken *)CYPHER_MALLOC(sizeof(CypherToken));
    if (!pToken) {
        return NULL;
    }
    
    pToken->type = type;
    pToken->text = &pLexer->zInput[startPos];
    pToken->len = endPos - startPos;
    pToken->line = pLexer->iLine;
    pToken->column = pLexer->iColumn - pToken->len;
    pLexer->pLastToken = pToken;
    return pToken;
}

static void lexerSetError(CypherLexer *pLexer, const char *zFormat, ...) {
    if (pLexer->zErrorMsg) {
        CYPHER_FREE(pLexer->zErrorMsg);
    }
    
    va_list args;
    va_start(args, zFormat);
    
    // Determine required size
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(NULL, 0, zFormat, args_copy);
    va_end(args_copy);

    if (size < 0) {
        va_end(args);
        // Encoding error
        pLexer->zErrorMsg = NULL;
        return;
    }

    pLexer->zErrorMsg = (char *)CYPHER_MALLOC(size + 1);
    if (!pLexer->zErrorMsg) {
        va_end(args);
        // Allocation failed
        return;
    }

    vsnprintf(pLexer->zErrorMsg, size + 1, zFormat, args);
    va_end(args);
}

static CypherTokenType cypherGetKeywordToken(const char *zKeyword, size_t len) {
    switch(len) {
        case 2:
            if (strncasecmp(zKeyword, "AS", 2) == 0) return CYPHER_TOK_AS;
            if (strncasecmp(zKeyword, "BY", 2) == 0) return CYPHER_TOK_BY;
            if (strncasecmp(zKeyword, "IS", 2) == 0) return CYPHER_TOK_IS_NULL; // Simplified
            if (strncasecmp(zKeyword, "IN", 2) == 0) return CYPHER_TOK_IN;
            if (strncasecmp(zKeyword, "OR", 2) == 0) return CYPHER_TOK_OR;
            break;
        case 3:
            if (strncasecmp(zKeyword, "AND", 3) == 0) return CYPHER_TOK_AND;
            if (strncasecmp(zKeyword, "ASC", 3) == 0) return CYPHER_TOK_ASC;
            if (strncasecmp(zKeyword, "NOT", 3) == 0) return CYPHER_TOK_NOT;
            if (strncasecmp(zKeyword, "SET", 3) == 0) return CYPHER_TOK_SET;
            if (strncasecmp(zKeyword, "XOR", 3) == 0) return CYPHER_TOK_XOR;
            break;
        case 4:
            if (strncasecmp(zKeyword, "DESC", 4) == 0) return CYPHER_TOK_DESC;
            if (strncasecmp(zKeyword, "SKIP", 4) == 0) return CYPHER_TOK_SKIP;
            if (strncasecmp(zKeyword, "WITH", 4) == 0) return CYPHER_TOK_WITH;
            if (strncasecmp(zKeyword, "NULL", 4) == 0) return CYPHER_TOK_NULL;
            if (strncasecmp(zKeyword, "TRUE", 4) == 0) return CYPHER_TOK_BOOLEAN;
            break;
        case 5:
            if (strncasecmp(zKeyword, "MATCH", 5) == 0) return CYPHER_TOK_MATCH;
            if (strncasecmp(zKeyword, "LIMIT", 5) == 0) return CYPHER_TOK_LIMIT;
            if (strncasecmp(zKeyword, "MERGE", 5) == 0) return CYPHER_TOK_MERGE;
            if (strncasecmp(zKeyword, "ORDER", 5) == 0) return CYPHER_TOK_ORDER;
            if (strncasecmp(zKeyword, "UNION", 5) == 0) return CYPHER_TOK_UNION;
            if (strncasecmp(zKeyword, "WHERE", 5) == 0) return CYPHER_TOK_WHERE;
            if (strncasecmp(zKeyword, "FALSE", 5) == 0) return CYPHER_TOK_BOOLEAN;
            break;
        case 6:
            if (strncasecmp(zKeyword, "CREATE", 6) == 0) return CYPHER_TOK_CREATE;
            if (strncasecmp(zKeyword, "DELETE", 6) == 0) return CYPHER_TOK_DELETE;
            if (strncasecmp(zKeyword, "DETACH", 6) == 0) return CYPHER_TOK_DETACH;
            if (strncasecmp(zKeyword, "REMOVE", 6) == 0) return CYPHER_TOK_REMOVE;
            if (strncasecmp(zKeyword, "RETURN", 6) == 0) return CYPHER_TOK_RETURN;
            break;
        case 8:
            if (strncasecmp(zKeyword, "DISTINCT", 8) == 0) return CYPHER_TOK_DISTINCT;
            if (strncasecmp(zKeyword, "OPTIONAL", 8) == 0) return CYPHER_TOK_OPTIONAL;
            if (strncasecmp(zKeyword, "CONTAINS", 8) == 0) return CYPHER_TOK_CONTAINS;
            break;
    }
    return CYPHER_TOK_IDENTIFIER;
}

static CypherToken *lexerTokenizeIdentifier(CypherLexer *pLexer) {
    int startPos = pLexer->iPos;
    while (isalnum(lexerPeek(pLexer, 0)) || lexerPeek(pLexer, 0) == '_') {
        lexerNext(pLexer);
    }
    int endPos = pLexer->iPos;
    CypherTokenType type = cypherGetKeywordToken(&pLexer->zInput[startPos], endPos - startPos);
    return lexerAddToken(pLexer, type, startPos, endPos);
}

static CypherToken *lexerTokenizeNumber(CypherLexer *pLexer) {
    int startPos = pLexer->iPos;
    CypherTokenType type = CYPHER_TOK_INTEGER;
    while (isdigit(lexerPeek(pLexer, 0))) {
        lexerNext(pLexer);
    }
    if (lexerPeek(pLexer, 0) == '.') {
        type = CYPHER_TOK_FLOAT;
        lexerNext(pLexer);
        while (isdigit(lexerPeek(pLexer, 0))) {
            lexerNext(pLexer);
        }
    }
    int endPos = pLexer->iPos;
    return lexerAddToken(pLexer, type, startPos, endPos);
}

static CypherToken *lexerTokenizeString(CypherLexer *pLexer) {
    int startPos = pLexer->iPos + 1;
    char quote = lexerNext(pLexer); // Consume opening quote
    while (lexerPeek(pLexer, 0) != quote) {
        if (lexerPeek(pLexer, 0) == '\\') {
            lexerNext(pLexer); // Consume backslash
        }
        if (lexerPeek(pLexer, 0) == '\0') {
            lexerSetError(pLexer, "Unterminated string literal");
            return lexerAddToken(pLexer, CYPHER_TOK_ERROR, startPos - 1, pLexer->iPos);
        }
        lexerNext(pLexer);
    }
    int endPos = pLexer->iPos;
    lexerNext(pLexer); // Consume closing quote
    return lexerAddToken(pLexer, CYPHER_TOK_STRING, startPos, endPos);
}


CypherToken *cypherLexerNextToken(CypherLexer *pLexer) {
    lexerSkipWhitespace(pLexer);
    lexerSkipComment(pLexer);

    int startPos = pLexer->iPos;
    char c = lexerPeek(pLexer, 0);

    if (c == '\0') {
        return lexerAddToken(pLexer, CYPHER_TOK_EOF, startPos, pLexer->iPos);
    }

    if (isalpha(c) || c == '_') {
        return lexerTokenizeIdentifier(pLexer);
    }

    if (isdigit(c)) {
        return lexerTokenizeNumber(pLexer);
    }

    if (c == '\'' || c == '"') {
        return lexerTokenizeString(pLexer);
    }

    lexerNext(pLexer);
    switch (c) {
        case '=':
            if (lexerPeek(pLexer, 0) == '~') {
                lexerNext(pLexer);
                return lexerAddToken(pLexer, CYPHER_TOK_REGEX, startPos, pLexer->iPos);
            }
            return lexerAddToken(pLexer, CYPHER_TOK_EQ, startPos, pLexer->iPos);
        case '<':
            if (lexerPeek(pLexer, 0) == '>') {
                lexerNext(pLexer);
                return lexerAddToken(pLexer, CYPHER_TOK_NE, startPos, pLexer->iPos);
            } else if (lexerPeek(pLexer, 0) == '-') {
                lexerNext(pLexer);
                if (lexerPeek(pLexer, 0) == '>') {
                    lexerNext(pLexer);
                    return lexerAddToken(pLexer, CYPHER_TOK_ARROW_BOTH, startPos, pLexer->iPos);
                }
                return lexerAddToken(pLexer, CYPHER_TOK_ARROW_LEFT, startPos, pLexer->iPos);
            } else if (lexerPeek(pLexer, 0) == '=') {
                lexerNext(pLexer);
                return lexerAddToken(pLexer, CYPHER_TOK_LE, startPos, pLexer->iPos);
            }
            return lexerAddToken(pLexer, CYPHER_TOK_LT, startPos, pLexer->iPos);
        case '>':
            if (lexerPeek(pLexer, 0) == '=') {
                lexerNext(pLexer);
                return lexerAddToken(pLexer, CYPHER_TOK_GE, startPos, pLexer->iPos);
            }
            return lexerAddToken(pLexer, CYPHER_TOK_GT, startPos, pLexer->iPos);
        case '-':
            if (lexerPeek(pLexer, 0) == '>') {
                lexerNext(pLexer);
                return lexerAddToken(pLexer, CYPHER_TOK_ARROW_RIGHT, startPos, pLexer->iPos);
            }
            return lexerAddToken(pLexer, CYPHER_TOK_MINUS, startPos, pLexer->iPos);
        case '+': return lexerAddToken(pLexer, CYPHER_TOK_PLUS, startPos, pLexer->iPos);
        case '*': return lexerAddToken(pLexer, CYPHER_TOK_MULT, startPos, pLexer->iPos);
        case '/': return lexerAddToken(pLexer, CYPHER_TOK_DIV, startPos, pLexer->iPos);
        case '%': return lexerAddToken(pLexer, CYPHER_TOK_MOD, startPos, pLexer->iPos);
        case '^': return lexerAddToken(pLexer, CYPHER_TOK_POW, startPos, pLexer->iPos);
        case '.': return lexerAddToken(pLexer, CYPHER_TOK_DOT, startPos, pLexer->iPos);
        case ':': return lexerAddToken(pLexer, CYPHER_TOK_COLON, startPos, pLexer->iPos);
        case ',': return lexerAddToken(pLexer, CYPHER_TOK_COMMA, startPos, pLexer->iPos);
        case ';': return lexerAddToken(pLexer, CYPHER_TOK_SEMICOLON, startPos, pLexer->iPos);
        case '(': return lexerAddToken(pLexer, CYPHER_TOK_LPAREN, startPos, pLexer->iPos);
        case ')': return lexerAddToken(pLexer, CYPHER_TOK_RPAREN, startPos, pLexer->iPos);
        case '[': return lexerAddToken(pLexer, CYPHER_TOK_LBRACKET, startPos, pLexer->iPos);
        case ']': return lexerAddToken(pLexer, CYPHER_TOK_RBRACKET, startPos, pLexer->iPos);
        case '{': return lexerAddToken(pLexer, CYPHER_TOK_LBRACE, startPos, pLexer->iPos);
        case '}': return lexerAddToken(pLexer, CYPHER_TOK_RBRACE, startPos, pLexer->iPos);
        case '|': return lexerAddToken(pLexer, CYPHER_TOK_PIPE, startPos, pLexer->iPos);
        case '$': return lexerAddToken(pLexer, CYPHER_TOK_DOLLAR, startPos, pLexer->iPos);
    }

    lexerSetError(pLexer, "Unexpected character: %c", c);
    return lexerAddToken(pLexer, CYPHER_TOK_ERROR, startPos, pLexer->iPos);
}

/*
** Return string name for token type - useful for debugging
*/
const char *cypherTokenTypeName(CypherTokenType type) {
    switch (type) {
        case CYPHER_TOK_EOF: return "EOF";
        case CYPHER_TOK_ERROR: return "ERROR";
        case CYPHER_TOK_WHITESPACE: return "WHITESPACE";
        case CYPHER_TOK_COMMENT: return "COMMENT";
        case CYPHER_TOK_MATCH: return "MATCH";
        case CYPHER_TOK_OPTIONAL: return "OPTIONAL";
        case CYPHER_TOK_WHERE: return "WHERE";
        case CYPHER_TOK_RETURN: return "RETURN";
        case CYPHER_TOK_CREATE: return "CREATE";
        case CYPHER_TOK_MERGE: return "MERGE";
        case CYPHER_TOK_SET: return "SET";
        case CYPHER_TOK_DELETE: return "DELETE";
        case CYPHER_TOK_DETACH: return "DETACH";
        case CYPHER_TOK_REMOVE: return "REMOVE";
        case CYPHER_TOK_WITH: return "WITH";
        case CYPHER_TOK_UNION: return "UNION";
        case CYPHER_TOK_AS: return "AS";
        case CYPHER_TOK_ORDER: return "ORDER";
        case CYPHER_TOK_BY: return "BY";
        case CYPHER_TOK_ASC: return "ASC";
        case CYPHER_TOK_DESC: return "DESC";
        case CYPHER_TOK_LIMIT: return "LIMIT";
        case CYPHER_TOK_SKIP: return "SKIP";
        case CYPHER_TOK_DISTINCT: return "DISTINCT";
        case CYPHER_TOK_AND: return "AND";
        case CYPHER_TOK_OR: return "OR";
        case CYPHER_TOK_XOR: return "XOR";
        case CYPHER_TOK_NOT: return "NOT";
        case CYPHER_TOK_IN: return "IN";
        case CYPHER_TOK_STARTS_WITH: return "STARTS_WITH";
        case CYPHER_TOK_ENDS_WITH: return "ENDS_WITH";
        case CYPHER_TOK_CONTAINS: return "CONTAINS";
        case CYPHER_TOK_IS_NULL: return "IS_NULL";
        case CYPHER_TOK_IS_NOT_NULL: return "IS_NOT_NULL";
        case CYPHER_TOK_NULL: return "NULL";
        case CYPHER_TOK_EQ: return "=";
        case CYPHER_TOK_NE: return "<>";
        case CYPHER_TOK_LT: return "<";
        case CYPHER_TOK_LE: return "<=";
        case CYPHER_TOK_GT: return ">";
        case CYPHER_TOK_GE: return ">=";
        case CYPHER_TOK_PLUS: return "+";
        case CYPHER_TOK_MINUS: return "-";
        case CYPHER_TOK_MULT: return "*";
        case CYPHER_TOK_DIV: return "/";
        case CYPHER_TOK_MOD: return "%";
        case CYPHER_TOK_POW: return "^";
        case CYPHER_TOK_DOT: return ".";
        case CYPHER_TOK_COLON: return ":";
        case CYPHER_TOK_COMMA: return ",";
        case CYPHER_TOK_SEMICOLON: return ";";
        case CYPHER_TOK_LPAREN: return "(";
        case CYPHER_TOK_RPAREN: return ")";
        case CYPHER_TOK_LBRACKET: return "[";
        case CYPHER_TOK_RBRACKET: return "]";
        case CYPHER_TOK_LBRACE: return "{";
        case CYPHER_TOK_RBRACE: return "}";
        case CYPHER_TOK_DASH: return "-";
        case CYPHER_TOK_ARROW_RIGHT: return "->";
        case CYPHER_TOK_ARROW_LEFT: return "<-";
        case CYPHER_TOK_ARROW_BOTH: return "<->";
        case CYPHER_TOK_PIPE: return "|";
        case CYPHER_TOK_REGEX: return "=~";
        case CYPHER_TOK_DOLLAR: return "$";
        case CYPHER_TOK_INTEGER: return "INTEGER";
        case CYPHER_TOK_FLOAT: return "FLOAT";
        case CYPHER_TOK_STRING: return "STRING";
        case CYPHER_TOK_BOOLEAN: return "BOOLEAN";
        case CYPHER_TOK_IDENTIFIER: return "IDENTIFIER";
        case CYPHER_TOK_LABEL: return "LABEL";
        case CYPHER_TOK_PROPERTY: return "PROPERTY";
        case CYPHER_TOK_REL_TYPE: return "REL_TYPE";
        default: return "UNKNOWN";
    }
}
