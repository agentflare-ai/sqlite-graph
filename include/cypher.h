#ifndef CYPHER_H
#define CYPHER_H

#include <sqlite3.h> // For sqlite3_malloc, sqlite3_free, etc.
#include <stdarg.h> // For va_list in error reporting

/* Forward declarations */
typedef struct CypherLexer CypherLexer;
typedef struct CypherParser CypherParser;
typedef struct CypherAst CypherAst;

// Lexer Definitions

// Token Types
typedef enum {
    CYPHER_TOK_EOF = 0,
    CYPHER_TOK_ERROR,
    CYPHER_TOK_WHITESPACE, // To be skipped
    CYPHER_TOK_COMMENT,    // To be skipped

    // Keywords
    CYPHER_TOK_MATCH,
    CYPHER_TOK_OPTIONAL,
    CYPHER_TOK_ON,
    CYPHER_TOK_WHERE,
    CYPHER_TOK_RETURN,
    CYPHER_TOK_CREATE,
    CYPHER_TOK_MERGE,
    CYPHER_TOK_SET,
    CYPHER_TOK_DELETE,
    CYPHER_TOK_DETACH,
    CYPHER_TOK_REMOVE,
    CYPHER_TOK_WITH,
    CYPHER_TOK_CALL,
    CYPHER_TOK_YIELD,
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

// Token Structure
typedef struct CypherToken {
    CypherTokenType type;
    const char *text; // Pointer to input text (not allocated)
    int len; // Length of token text
    int line;
    int column;
} CypherToken;

// Lexer context structure
struct CypherLexer {
    const char *zInput;
    int iPos;
    int iLine;
    int iColumn;
    char *zErrorMsg;
    CypherToken *pLastToken; // Last token returned
};

// Lexer Functions
CypherLexer *cypherLexerCreate(const char *zInput);
void cypherLexerDestroy(CypherLexer *pLexer);
CypherToken *cypherLexerNextToken(CypherLexer *pLexer);
const char *cypherTokenTypeName(CypherTokenType type); // For debugging

// AST Definitions

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
    CYPHER_AST_LIST,
    CYPHER_AST_FUNCTION_CALL,
    CYPHER_AST_CASE,
    CYPHER_AST_PROPERTY_PAIR, // For key: value pairs in maps
    
    // Expression AST node types
    CYPHER_AST_AND,
    CYPHER_AST_OR,
    CYPHER_AST_NOT,
    CYPHER_AST_COMPARISON,
    CYPHER_AST_ADDITIVE,
    CYPHER_AST_MULTIPLICATIVE,
    
    // Collection types
    CYPHER_AST_ARRAY,
    CYPHER_AST_OBJECT,
    
    // Advanced operators
    CYPHER_AST_STARTS_WITH,
    CYPHER_AST_ENDS_WITH,
    CYPHER_AST_CONTAINS_OP,
    CYPHER_AST_REGEX,

    // Mutation clauses
    CYPHER_AST_ON_CREATE,
    CYPHER_AST_ON_MATCH,
    CYPHER_AST_CREATE,
    CYPHER_AST_MERGE,
    CYPHER_AST_SET,
    CYPHER_AST_DELETE,
    CYPHER_AST_REMOVE,
    CYPHER_AST_WITH,

    CYPHER_AST_COUNT // Sentinel for max AST node type
} CypherAstNodeType;

// AST Node Structure
struct CypherAst {
    CypherAstNodeType type;
    char *zValue; // Value for literals, identifiers, operators (e.g., "+", "Person")
    struct CypherAst **apChildren; // Array of child AST nodes
    int nChildren; // Number of children
    int nChildrenAlloc; // Allocated size of apChildren
    int iLine; // Line number from source
    int iColumn; // Column number from source
    int iFlags; // General purpose flags (e.g., DISTINCT for RETURN clause)

    // Variable-length relationship fields (for CYPHER_AST_REL_PATTERN)
    int iMinHops; // Minimum hops for variable-length relationships (0 for fixed-length)
    int iMaxHops; // Maximum hops for variable-length relationships (0 for fixed-length)
};

// AST iFlags values for relationship patterns
#define REL_PATTERN_VARLEN 0x0100  // Indicates variable-length relationship pattern

// AST Node creation functions
CypherAst *cypherAstCreate(CypherAstNodeType type, int iLine, int iColumn);
CypherAst *cypherAstCreateIdentifier(const char *zName, int iLine, int iColumn);
CypherAst *cypherAstCreateLiteral(const char *zValue, int iLine, int iColumn);
CypherAst *cypherAstCreateBinaryOp(const char *zOp, CypherAst *pLeft, CypherAst *pRight, int iLine, int iColumn);
CypherAst *cypherAstCreateUnaryOp(const char *zOp, CypherAst *pExpr, int iLine, int iColumn);
CypherAst *cypherAstCreateProperty(CypherAst *pObj, const char *zProp, int iLine, int iColumn);
CypherAst *cypherAstCreateNodeLabel(const char *zLabel, int iLine, int iColumn);

// AST Node manipulation functions
void cypherAstAddChild(CypherAst *pParent, CypherAst *pChild);
void cypherAstSetValue(CypherAst *pNode, const char *zValue);
CypherAst *cypherAstGetChild(CypherAst *pNode, int iChild);
int cypherAstGetChildCount(CypherAst *pNode);
const char *cypherAstGetValue(CypherAst *pNode);
int cypherAstIsType(CypherAst *pAst, CypherAstNodeType type);

// AST Node destruction
void cypherAstDestroy(CypherAst *pNode);

// Debugging
void cypherAstPrint(CypherAst *pNode, int iIndent);
const char *cypherAstNodeTypeName(CypherAstNodeType type);

// Parser Definitions

// Parser context structure
struct CypherParser {
    char *zErrorMsg;
    CypherAst *pAst;
    int bInMergeClause;
};

// Parser Functions
CypherParser *cypherParserCreate(void);
void cypherParserDestroy(CypherParser *pParser);
CypherAst *cypherParse(CypherParser *pParser, const char *zQuery, char **pzErrMsg);

// SQL Function Registration
int cypherRegisterSqlFunctions(sqlite3 *db);
int cypherRegisterWriteSqlFunctions(sqlite3 *db);
int cypherRegisterPlannerSqlFunctions(sqlite3 *db);
int cypherRegisterExecutorSqlFunctions(sqlite3 *db);

#endif // CYPHER_H
