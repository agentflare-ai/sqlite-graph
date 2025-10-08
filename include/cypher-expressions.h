/*
 * Cypher Expression Evaluation System
 * Implements openCypher expression evaluation for compliance
 */

#ifndef CYPHER_EXPRESSIONS_H
#define CYPHER_EXPRESSIONS_H

#include "cypher-executor.h"

/* Expression types for evaluation */
typedef enum {
    CYPHER_EXPR_LITERAL,     /* Numbers, strings, booleans, null */
    CYPHER_EXPR_VARIABLE,    /* Named variables like 'n', 'r' */
    CYPHER_EXPR_PROPERTY,    /* Property access like 'n.name' */
    CYPHER_EXPR_ARITHMETIC,  /* +, -, *, /, %, ^ */
    CYPHER_EXPR_COMPARISON,  /* =, <>, <, >, <=, >= */
    CYPHER_EXPR_LOGICAL,     /* AND, OR, NOT, XOR */
    CYPHER_EXPR_STRING,      /* String operations */
    CYPHER_EXPR_LIST,        /* List operations */
    CYPHER_EXPR_MAP,         /* Map operations */
    CYPHER_EXPR_FUNCTION,    /* Function calls */
    CYPHER_EXPR_CASE         /* CASE expressions */
} CypherExpressionType;

/* Arithmetic operators */
typedef enum {
    CYPHER_OP_ADD,           /* + */
    CYPHER_OP_SUBTRACT,      /* - */
    CYPHER_OP_MULTIPLY,      /* * */
    CYPHER_OP_DIVIDE,        /* / */
    CYPHER_OP_MODULO,        /* % */
    CYPHER_OP_POWER          /* ^ */
} CypherArithmeticOp;

/* Comparison operators */
typedef enum {
    CYPHER_CMP_EQUAL,        /* = */
    CYPHER_CMP_NOT_EQUAL,    /* <> */
    CYPHER_CMP_LESS,         /* < */
    CYPHER_CMP_LESS_EQUAL,   /* <= */
    CYPHER_CMP_GREATER,      /* > */
    CYPHER_CMP_GREATER_EQUAL,/* >= */
    CYPHER_CMP_IS_NULL,      /* IS NULL */
    CYPHER_CMP_IS_NOT_NULL,  /* IS NOT NULL */
    CYPHER_CMP_STARTS_WITH,  /* STARTS WITH */
    CYPHER_CMP_ENDS_WITH,    /* ENDS WITH */
    CYPHER_CMP_CONTAINS,     /* CONTAINS */
    CYPHER_CMP_IN            /* IN */
} CypherComparisonOp;

/* Logical operators */
typedef enum {
    CYPHER_LOGIC_AND,        /* AND */
    CYPHER_LOGIC_OR,         /* OR */
    CYPHER_LOGIC_NOT,        /* NOT */
    CYPHER_LOGIC_XOR         /* XOR */
} CypherLogicalOp;

/* String operators */
typedef enum {
    CYPHER_STR_CONCAT,       /* + */
    CYPHER_STR_REGEX,        /* =~ */
    CYPHER_STR_STARTS_WITH,  /* STARTS WITH */
    CYPHER_STR_ENDS_WITH,    /* ENDS WITH */
    CYPHER_STR_CONTAINS      /* CONTAINS */
} CypherStringOp;

/* Expression structure */
typedef struct CypherExpression {
    CypherExpressionType type;
    union {
        /* Literal value */
        CypherValue literal;
        
        /* Variable reference */
        struct {
            char *zName;
        } variable;
        
        /* Property access */
        struct {
            struct CypherExpression *pObject;
            char *zProperty;
        } property;
        
        /* Binary operations */
        struct {
            struct CypherExpression *pLeft;
            struct CypherExpression *pRight;
            int op; /* Cast to appropriate enum based on type */
        } binary;
        
        /* Unary operations */
        struct {
            struct CypherExpression *pOperand;
            int op;
        } unary;
        
        /* Function call */
        struct {
            char *zName;
            struct CypherExpression **apArgs;
            int nArgs;
        } function;
        
        /* List expression */
        struct {
            struct CypherExpression **apElements;
            int nElements;
        } list;
        
        /* Map expression */
        struct {
            char **azKeys;
            struct CypherExpression **apValues;
            int nPairs;
        } map;
    } u;
} CypherExpression;

/* Expression evaluation functions */
int cypherExpressionCreate(CypherExpression **ppExpr, CypherExpressionType type);
int cypherExpressionDestroy(CypherExpression *pExpr);
int cypherExpressionEvaluate(const CypherExpression *pExpr, 
                            ExecutionContext *pContext, 
                            CypherValue *pResult);

/* Literal expression creation */
int cypherExpressionCreateLiteral(CypherExpression **ppExpr, const CypherValue *pValue);
int cypherExpressionCreateVariable(CypherExpression **ppExpr, const char *zName);
int cypherExpressionCreateProperty(CypherExpression **ppExpr, 
                                  CypherExpression *pObject, 
                                  const char *zProperty);

/* Binary expression creation */
int cypherExpressionCreateArithmetic(CypherExpression **ppExpr,
                                    CypherExpression *pLeft,
                                    CypherExpression *pRight,
                                    CypherArithmeticOp op);

int cypherExpressionCreateComparison(CypherExpression **ppExpr,
                                   CypherExpression *pLeft,
                                   CypherExpression *pRight,
                                   CypherComparisonOp op);

int cypherExpressionCreateLogical(CypherExpression **ppExpr,
                                CypherExpression *pLeft,
                                CypherExpression *pRight,
                                CypherLogicalOp op);

int cypherExpressionCreateString(CypherExpression **ppExpr,
                               CypherExpression *pLeft,
                               CypherExpression *pRight,
                               CypherStringOp op);

/* Function expression creation */
int cypherExpressionCreateFunction(CypherExpression **ppExpr,
                                 const char *zName,
                                 CypherExpression **apArgs,
                                 int nArgs);

/* List and map expression creation */
int cypherExpressionCreateList(CypherExpression **ppExpr,
                             CypherExpression **apElements,
                             int nElements);

int cypherExpressionCreateMap(CypherExpression **ppExpr,
                            const char **azKeys,
                            CypherExpression **apValues,
                            int nPairs);

/* Expression type checking */
int cypherExpressionGetType(const CypherExpression *pExpr, CypherValueType *pType);
int cypherExpressionIsConstant(const CypherExpression *pExpr);

/* Expression optimization */
int cypherExpressionOptimize(CypherExpression **ppExpr);
int cypherExpressionSimplify(CypherExpression **ppExpr);

/* Expression utilities */
char *cypherExpressionToString(const CypherExpression *pExpr);
int cypherExpressionEqual(const CypherExpression *pLeft, const CypherExpression *pRight);

/* Built-in function definitions */
typedef struct CypherBuiltinFunction {
    const char *zName;
    int nMinArgs;
    int nMaxArgs; /* -1 for variadic */
    int (*xFunction)(CypherValue *apArgs, int nArgs, CypherValue *pResult);
} CypherBuiltinFunction;

/* Built-in function registry */
int cypherRegisterBuiltinFunctions(void);
const CypherBuiltinFunction *cypherGetBuiltinFunction(const char *zName);

/* String functions */
int cypherFunctionToUpper(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionToLower(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionLength(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionTrim(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionSubstring(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionReplace(CypherValue *apArgs, int nArgs, CypherValue *pResult);

/* Numeric functions */
int cypherFunctionAbs(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionCeil(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionFloor(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionRound(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionSqrt(CypherValue *apArgs, int nArgs, CypherValue *pResult);

/* List functions */
int cypherFunctionSize(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionHead(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionTail(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionLast(CypherValue *apArgs, int nArgs, CypherValue *pResult);

/* Type functions */
int cypherFunctionToString(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionToInteger(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionToFloat(CypherValue *apArgs, int nArgs, CypherValue *pResult);

/* Aggregate functions */
int cypherFunctionCount(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionSum(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionAvg(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionMin(CypherValue *apArgs, int nArgs, CypherValue *pResult);
int cypherFunctionMax(CypherValue *apArgs, int nArgs, CypherValue *pResult);

#endif /* CYPHER_EXPRESSIONS_H */