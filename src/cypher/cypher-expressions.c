/*
 * Cypher Expression Evaluation Implementation
 * Core expression system for openCypher compliance
 */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sqlite3.h>
#include "cypher-expressions.h"

/* Forward declarations */
int cypherEvaluateArithmetic(const CypherValue *pLeft, const CypherValue *pRight, 
                           CypherArithmeticOp op, CypherValue *pResult);
int cypherEvaluateComparison(const CypherValue *pLeft, const CypherValue *pRight, 
                           CypherComparisonOp op, CypherValue *pResult);
int cypherEvaluateFunction(const char *zName, CypherExpression **apArgs, int nArgs,
                         ExecutionContext *pContext, CypherValue *pResult);

/* Global function registry */
static CypherBuiltinFunction *g_functions = NULL;
static int g_nFunctions = 0;

/* Forward declaration for AST conversion */
static int cypherAstToExpression(CypherAst *pAst, CypherExpression **ppExpr);

/* Expression creation */
int cypherExpressionCreate(CypherExpression **ppExpr, CypherExpressionType type) {
    CypherExpression *pExpr;
    
    if (!ppExpr) return SQLITE_MISUSE;
    
    pExpr = sqlite3_malloc(sizeof(CypherExpression));
    if (!pExpr) return SQLITE_NOMEM;
    
    memset(pExpr, 0, sizeof(CypherExpression));
    pExpr->type = type;
    
    *ppExpr = pExpr;
    return SQLITE_OK;
}

int cypherExpressionDestroy(CypherExpression *pExpr) {
    int i;
    
    if (!pExpr) return SQLITE_OK;
    
    switch (pExpr->type) {
        case CYPHER_EXPR_LITERAL:
            cypherValueDestroy(&pExpr->u.literal);
            break;
            
        case CYPHER_EXPR_VARIABLE:
            sqlite3_free(pExpr->u.variable.zName);
            break;
            
        case CYPHER_EXPR_PROPERTY:
            cypherExpressionDestroy(pExpr->u.property.pObject);
            sqlite3_free(pExpr->u.property.zProperty);
            break;
            
        case CYPHER_EXPR_ARITHMETIC:
        case CYPHER_EXPR_COMPARISON:
        case CYPHER_EXPR_LOGICAL:
        case CYPHER_EXPR_STRING:
            cypherExpressionDestroy(pExpr->u.binary.pLeft);
            cypherExpressionDestroy(pExpr->u.binary.pRight);
            break;
            
        case CYPHER_EXPR_FUNCTION:
            sqlite3_free(pExpr->u.function.zName);
            for (i = 0; i < pExpr->u.function.nArgs; i++) {
                cypherExpressionDestroy(pExpr->u.function.apArgs[i]);
            }
            sqlite3_free(pExpr->u.function.apArgs);
            break;
            
        case CYPHER_EXPR_LIST:
            for (i = 0; i < pExpr->u.list.nElements; i++) {
                cypherExpressionDestroy(pExpr->u.list.apElements[i]);
            }
            sqlite3_free(pExpr->u.list.apElements);
            break;
            
        case CYPHER_EXPR_MAP:
            for (i = 0; i < pExpr->u.map.nPairs; i++) {
                sqlite3_free(pExpr->u.map.azKeys[i]);
                cypherExpressionDestroy(pExpr->u.map.apValues[i]);
            }
            sqlite3_free(pExpr->u.map.azKeys);
            sqlite3_free(pExpr->u.map.apValues);
            break;
            
        default:
            break;
    }
    
    sqlite3_free(pExpr);
    return SQLITE_OK;
}

/* Expression evaluation */
int cypherExpressionEvaluate(const CypherExpression *pExpr, 
                            ExecutionContext *pContext, 
                            CypherValue *pResult) {
    CypherValue left, right;
    int rc = SQLITE_OK;
    
    if (!pExpr || !pResult) return SQLITE_MISUSE;
    
    /* Initialize result */
    cypherValueInit(pResult);
    
    switch (pExpr->type) {
        case CYPHER_EXPR_LITERAL:
            {
                CypherValue *pCopy = cypherValueCopy((CypherValue*)&pExpr->u.literal);
                if( pCopy ) {
                    *pResult = *pCopy;
                    sqlite3_free(pCopy);
                    return SQLITE_OK;
                } else {
                    return SQLITE_NOMEM;
                }
            }
            
        case CYPHER_EXPR_VARIABLE:
            /* Look up variable in execution context */
            if (pContext && pExpr->u.variable.zName) {
                CypherValue *pValue = executionContextGet(pContext, pExpr->u.variable.zName);
                if (pValue) {
                    CypherValue *pCopy = cypherValueCopy(pValue);
                    if (!pCopy) return SQLITE_NOMEM;
                    *pResult = *pCopy;
                    sqlite3_free(pCopy);
                } else {
                    cypherValueSetNull(pResult);
                }
            } else {
                cypherValueSetNull(pResult);
            }
            return SQLITE_OK;

        case CYPHER_EXPR_PROPERTY: {
            /* Property access: object.property */
            CypherValue objectValue;
            cypherValueInit(&objectValue);

            /* First evaluate the object expression */
            rc = cypherExpressionEvaluate(pExpr->u.property.pObject, pContext, &objectValue);
            if (rc != SQLITE_OK) {
                cypherValueDestroy(&objectValue);
                return rc;
            }

            /* Extract property from the object */
            if (cypherValueIsNull(&objectValue)) {
                cypherValueSetNull(pResult);
            } else if (objectValue.type == CYPHER_VALUE_MAP) {
                /* Get property from map */
                int i;
                int bFound = 0;
                for (i = 0; i < objectValue.u.map.nPairs; i++) {
                    if (strcmp(objectValue.u.map.azKeys[i], pExpr->u.property.zProperty) == 0) {
                        CypherValue *pCopy = cypherValueCopy(&objectValue.u.map.apValues[i]);
                        if (!pCopy) {
                            cypherValueDestroy(&objectValue);
                            return SQLITE_NOMEM;
                        }
                        *pResult = *pCopy;
                        sqlite3_free(pCopy);
                        bFound = 1;
                        break;
                    }
                }
                if (!bFound) {
                    cypherValueSetNull(pResult);
                }
            } else if (objectValue.type == CYPHER_VALUE_NODE ||
                       objectValue.type == CYPHER_VALUE_RELATIONSHIP) {
                /* TODO: Load node/relationship from database and access properties */
                /* For now, return NULL - full implementation requires database access */
                cypherValueSetNull(pResult);
            } else {
                /* Property access on non-object type returns NULL */
                cypherValueSetNull(pResult);
            }

            cypherValueDestroy(&objectValue);
            return SQLITE_OK;
        }
            
        case CYPHER_EXPR_ARITHMETIC:
            cypherValueInit(&left);
            cypherValueInit(&right);
            
            rc = cypherExpressionEvaluate(pExpr->u.binary.pLeft, pContext, &left);
            if (rc != SQLITE_OK) goto arithmetic_cleanup;
            
            rc = cypherExpressionEvaluate(pExpr->u.binary.pRight, pContext, &right);
            if (rc != SQLITE_OK) goto arithmetic_cleanup;
            
            rc = cypherEvaluateArithmetic(&left, &right, 
                                        (CypherArithmeticOp)pExpr->u.binary.op, 
                                        pResult);
                                        
        arithmetic_cleanup:
            cypherValueDestroy(&left);
            cypherValueDestroy(&right);
            return rc;
            
        case CYPHER_EXPR_COMPARISON:
            cypherValueInit(&left);
            cypherValueInit(&right);
            
            rc = cypherExpressionEvaluate(pExpr->u.binary.pLeft, pContext, &left);
            if (rc != SQLITE_OK) goto comparison_cleanup;
            
            rc = cypherExpressionEvaluate(pExpr->u.binary.pRight, pContext, &right);
            if (rc != SQLITE_OK) goto comparison_cleanup;
            
            rc = cypherEvaluateComparison(&left, &right, 
                                        (CypherComparisonOp)pExpr->u.binary.op, 
                                        pResult);
                                        
        comparison_cleanup:
            cypherValueDestroy(&left);
            cypherValueDestroy(&right);
            return rc;
            
        case CYPHER_EXPR_FUNCTION:
            return cypherEvaluateFunction(pExpr->u.function.zName,
                                        pExpr->u.function.apArgs,
                                        pExpr->u.function.nArgs,
                                        pContext,
                                        pResult);
            
        default:
            cypherValueSetNull(pResult);
            return SQLITE_OK;
    }
}

/* Arithmetic evaluation */
int cypherEvaluateArithmetic(const CypherValue *pLeft, const CypherValue *pRight,
                           CypherArithmeticOp op, CypherValue *pResult) {
    double leftVal, rightVal, result;

    if (!pLeft || !pRight || !pResult) return SQLITE_MISUSE;

    /* Handle NULL values */
    if (pLeft->type == CYPHER_VALUE_NULL || pRight->type == CYPHER_VALUE_NULL) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }

    /* String concatenation with + operator */
    if (op == CYPHER_OP_ADD &&
        (pLeft->type == CYPHER_VALUE_STRING || pRight->type == CYPHER_VALUE_STRING)) {
        char *zLeft = NULL;
        char *zRight = NULL;
        char *zConcat = NULL;
        int nLeft, nRight;
        int rc;

        /* Convert both operands to strings */
        zLeft = cypherValueToString(pLeft);
        if (!zLeft) return SQLITE_NOMEM;

        zRight = cypherValueToString(pRight);
        if (!zRight) {
            sqlite3_free(zLeft);
            return SQLITE_NOMEM;
        }

        /* Concatenate strings */
        nLeft = strlen(zLeft);
        nRight = strlen(zRight);
        zConcat = sqlite3_malloc(nLeft + nRight + 1);
        if (!zConcat) {
            sqlite3_free(zLeft);
            sqlite3_free(zRight);
            return SQLITE_NOMEM;
        }

        memcpy(zConcat, zLeft, nLeft);
        memcpy(zConcat + nLeft, zRight, nRight);
        zConcat[nLeft + nRight] = '\0';

        rc = cypherValueSetString(pResult, zConcat);

        sqlite3_free(zLeft);
        sqlite3_free(zRight);
        sqlite3_free(zConcat);

        return rc;
    }

    /* Convert to numeric values */
    if (pLeft->type == CYPHER_VALUE_INTEGER) {
        leftVal = (double)pLeft->u.iInteger;
    } else if (pLeft->type == CYPHER_VALUE_FLOAT) {
        leftVal = pLeft->u.rFloat;
    } else {
        return SQLITE_MISMATCH; /* Type error */
    }

    if (pRight->type == CYPHER_VALUE_INTEGER) {
        rightVal = (double)pRight->u.iInteger;
    } else if (pRight->type == CYPHER_VALUE_FLOAT) {
        rightVal = pRight->u.rFloat;
    } else {
        return SQLITE_MISMATCH; /* Type error */
    }
    
    /* Perform operation */
    switch (op) {
        case CYPHER_OP_ADD:
            result = leftVal + rightVal;
            break;
        case CYPHER_OP_SUBTRACT:
            result = leftVal - rightVal;
            break;
        case CYPHER_OP_MULTIPLY:
            result = leftVal * rightVal;
            break;
        case CYPHER_OP_DIVIDE:
            if (rightVal == 0.0) {
                cypherValueSetNull(pResult);
                return SQLITE_OK;
            }
            result = leftVal / rightVal;
            break;
        case CYPHER_OP_MODULO:
            if (rightVal == 0.0) {
                cypherValueSetNull(pResult);
                return SQLITE_OK;
            }
            result = fmod(leftVal, rightVal);
            break;
        case CYPHER_OP_POWER:
            result = pow(leftVal, rightVal);
            break;
        default:
            return SQLITE_ERROR;
    }
    
    /* Return result as appropriate type */
    if (pLeft->type == CYPHER_VALUE_INTEGER && pRight->type == CYPHER_VALUE_INTEGER &&
        op != CYPHER_OP_DIVIDE && op != CYPHER_OP_POWER) {
        cypherValueSetInteger(pResult, (sqlite3_int64)result);
    } else {
        cypherValueSetFloat(pResult, result);
    }
    
    return SQLITE_OK;
}

/* Comparison evaluation */
int cypherEvaluateComparison(const CypherValue *pLeft, const CypherValue *pRight,
                           CypherComparisonOp op, CypherValue *pResult) {
    int cmp;
    int result;
    
    if (!pLeft || !pRight || !pResult) return SQLITE_MISUSE;
    
    /* Handle NULL comparisons */
    if (op == CYPHER_CMP_IS_NULL) {
        cypherValueSetBoolean(pResult, pLeft->type == CYPHER_VALUE_NULL);
        return SQLITE_OK;
    }
    if (op == CYPHER_CMP_IS_NOT_NULL) {
        cypherValueSetBoolean(pResult, pLeft->type != CYPHER_VALUE_NULL);
        return SQLITE_OK;
    }
    
    if (pLeft->type == CYPHER_VALUE_NULL || pRight->type == CYPHER_VALUE_NULL) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }
    
    /* Compare values */
    cmp = cypherValueCompare(pLeft, pRight);
    
    switch (op) {
        case CYPHER_CMP_EQUAL:
            result = (cmp == 0);
            break;
        case CYPHER_CMP_NOT_EQUAL:
            result = (cmp != 0);
            break;
        case CYPHER_CMP_LESS:
            result = (cmp < 0);
            break;
        case CYPHER_CMP_LESS_EQUAL:
            result = (cmp <= 0);
            break;
        case CYPHER_CMP_GREATER:
            result = (cmp > 0);
            break;
        case CYPHER_CMP_GREATER_EQUAL:
            result = (cmp >= 0);
            break;
        case CYPHER_CMP_STARTS_WITH:
            if (pLeft->type == CYPHER_VALUE_STRING && pRight->type == CYPHER_VALUE_STRING) {
                result = (strncmp(pLeft->u.zString, pRight->u.zString, strlen(pRight->u.zString)) == 0);
            } else {
                cypherValueSetNull(pResult);
                return SQLITE_OK;
            }
            break;
        case CYPHER_CMP_ENDS_WITH:
            if (pLeft->type == CYPHER_VALUE_STRING && pRight->type == CYPHER_VALUE_STRING) {
                int leftLen = strlen(pLeft->u.zString);
                int rightLen = strlen(pRight->u.zString);
                if (rightLen > leftLen) {
                    result = 0;
                } else {
                    result = (strcmp(pLeft->u.zString + leftLen - rightLen, pRight->u.zString) == 0);
                }
            } else {
                cypherValueSetNull(pResult);
                return SQLITE_OK;
            }
            break;
        case CYPHER_CMP_CONTAINS:
            if (pLeft->type == CYPHER_VALUE_STRING && pRight->type == CYPHER_VALUE_STRING) {
                result = (strstr(pLeft->u.zString, pRight->u.zString) != NULL);
            } else {
                cypherValueSetNull(pResult);
                return SQLITE_OK;
            }
            break;
        case CYPHER_CMP_IN:
            if (pRight->type == CYPHER_VALUE_LIST) {
                result = 0;
                for (int i = 0; i < pRight->u.list.nValues; i++) {
                    if (cypherValueCompare(pLeft, &pRight->u.list.apValues[i]) == 0) {
                        result = 1;
                        break;
                    }
                }
            } else {
                cypherValueSetNull(pResult);
                return SQLITE_OK;
            }
            break;
        default:
            return SQLITE_ERROR;
    }
    
    cypherValueSetBoolean(pResult, result);
    return SQLITE_OK;
}

/* Function evaluation */
int cypherEvaluateFunction(const char *zName, CypherExpression **apArgs, int nArgs,
                         ExecutionContext *pContext, CypherValue *pResult) {
    const CypherBuiltinFunction *pFunc;
    CypherValue *aValues = NULL;
    int rc = SQLITE_OK;
    int i;
    
    if (!zName || !pResult) return SQLITE_MISUSE;
    
    /* Look up function */
    pFunc = cypherGetBuiltinFunction(zName);
    if (!pFunc) {
        return SQLITE_ERROR; /* Unknown function */
    }
    
    /* Check argument count */
    if (nArgs < pFunc->nMinArgs || (pFunc->nMaxArgs >= 0 && nArgs > pFunc->nMaxArgs)) {
        return SQLITE_ERROR; /* Wrong argument count */
    }
    
    /* Evaluate arguments */
    if (nArgs > 0) {
        aValues = sqlite3_malloc(sizeof(CypherValue) * nArgs);
        if (!aValues) return SQLITE_NOMEM;
        
        for (i = 0; i < nArgs; i++) {
            cypherValueInit(&aValues[i]);
            rc = cypherExpressionEvaluate(apArgs[i], pContext, &aValues[i]);
            if (rc != SQLITE_OK) {
                /* Cleanup on error */
                for (int j = 0; j < i; j++) {
                    cypherValueDestroy(&aValues[j]);
                }
                sqlite3_free(aValues);
                return rc;
            }
        }
    }
    
    /* Call function */
    rc = pFunc->xFunction(aValues, nArgs, pResult);
    
    /* Cleanup */
    if (aValues) {
        for (i = 0; i < nArgs; i++) {
            cypherValueDestroy(&aValues[i]);
        }
        sqlite3_free(aValues);
    }
    
    return rc;
}

/* Literal expression creation */
int cypherExpressionCreateLiteral(CypherExpression **ppExpr, const CypherValue *pValue) {
    CypherExpression *pExpr;
    int rc;
    
    if (!ppExpr || !pValue) return SQLITE_MISUSE;
    
    rc = cypherExpressionCreate(&pExpr, CYPHER_EXPR_LITERAL);
    if (rc != SQLITE_OK) return rc;
    
    {
        CypherValue *pCopy = cypherValueCopy((CypherValue*)pValue);
        if( pCopy ) {
            pExpr->u.literal = *pCopy;
            sqlite3_free(pCopy);
            rc = SQLITE_OK;
        } else {
            rc = SQLITE_NOMEM;
        }
    }
    if (rc != SQLITE_OK) {
        cypherExpressionDestroy(pExpr);
        return rc;
    }
    
    *ppExpr = pExpr;
    return SQLITE_OK;
}

/* Arithmetic expression creation */
int cypherExpressionCreateArithmetic(CypherExpression **ppExpr,
                                    CypherExpression *pLeft,
                                    CypherExpression *pRight,
                                    CypherArithmeticOp op) {
    CypherExpression *pExpr;
    int rc;
    
    if (!ppExpr || !pLeft || !pRight) return SQLITE_MISUSE;
    
    rc = cypherExpressionCreate(&pExpr, CYPHER_EXPR_ARITHMETIC);
    if (rc != SQLITE_OK) return rc;
    
    pExpr->u.binary.pLeft = pLeft;
    pExpr->u.binary.pRight = pRight;
    pExpr->u.binary.op = op;
    
    *ppExpr = pExpr;
    return SQLITE_OK;
}

/* Helper function to convert token text to comparison operator */
static CypherComparisonOp __attribute__((unused)) getComparisonOpFromToken(const char *zToken) {
    if (strcmp(zToken, "=") == 0) return CYPHER_CMP_EQUAL;
    if (strcmp(zToken, "<>") == 0) return CYPHER_CMP_NOT_EQUAL;
    if (strcmp(zToken, "<") == 0) return CYPHER_CMP_LESS;
    if (strcmp(zToken, "<=") == 0) return CYPHER_CMP_LESS_EQUAL;
    if (strcmp(zToken, ">") == 0) return CYPHER_CMP_GREATER;
    if (strcmp(zToken, ">=") == 0) return CYPHER_CMP_GREATER_EQUAL;
    if (strcmp(zToken, "STARTS WITH") == 0) return CYPHER_CMP_STARTS_WITH;
    if (strcmp(zToken, "ENDS WITH") == 0) return CYPHER_CMP_ENDS_WITH;
    if (strcmp(zToken, "CONTAINS") == 0) return CYPHER_CMP_CONTAINS;
    if (strcmp(zToken, "IN") == 0) return CYPHER_CMP_IN;
    return CYPHER_CMP_EQUAL; // Default fallback
}

/* Comparison expression creation */
int cypherExpressionCreateComparison(CypherExpression **ppExpr,
                                   CypherExpression *pLeft,
                                   CypherExpression *pRight,
                                   CypherComparisonOp op) {
    CypherExpression *pExpr;
    int rc;
    
    if (!ppExpr || !pLeft || !pRight) return SQLITE_MISUSE;
    
    rc = cypherExpressionCreate(&pExpr, CYPHER_EXPR_COMPARISON);
    if (rc != SQLITE_OK) return rc;
    
    pExpr->u.binary.pLeft = pLeft;
    pExpr->u.binary.pRight = pRight;
    pExpr->u.binary.op = op;
    
    *ppExpr = pExpr;
    return SQLITE_OK;
}

/* Built-in function registration */
static CypherBuiltinFunction g_builtinFunctions[] = {
    {"toUpper", 1, 1, cypherFunctionToUpper},
    {"toLower", 1, 1, cypherFunctionToLower},
    {"length", 1, 1, cypherFunctionLength},
    {"size", 1, 1, cypherFunctionSize},
    {"abs", 1, 1, cypherFunctionAbs},
    {"ceil", 1, 1, cypherFunctionCeil},
    {"floor", 1, 1, cypherFunctionFloor},
    {"round", 1, 1, cypherFunctionRound},
    {"sqrt", 1, 1, cypherFunctionSqrt},
    {"toString", 1, 1, cypherFunctionToString},
    {"toInteger", 1, 1, cypherFunctionToInteger},
    {"toFloat", 1, 1, cypherFunctionToFloat},
    {"coalesce", 1, -1, cypherFunctionCoalesce},
    {"count", 1, 1, cypherFunctionCount},
    {"sum", 1, 1, cypherFunctionSum},
    {"avg", 1, 1, cypherFunctionAvg},
    {"min", 1, 1, cypherFunctionMin},
    {"max", 1, 1, cypherFunctionMax},
    {NULL, 0, 0, NULL} /* Sentinel */
};

int cypherRegisterBuiltinFunctions(void) {
    g_functions = g_builtinFunctions;
    g_nFunctions = sizeof(g_builtinFunctions) / sizeof(g_builtinFunctions[0]) - 1;
    return SQLITE_OK;
}

const CypherBuiltinFunction *cypherGetBuiltinFunction(const char *zName) {
    int i;
    
    if (!zName) return NULL;
    
    for (i = 0; i < g_nFunctions; i++) {
        if (sqlite3_stricmp(zName, g_functions[i].zName) == 0) {
            return &g_functions[i];
        }
    }
    
    return NULL;
}

/* String functions implementation */
int cypherFunctionToUpper(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    const char *zInput;
    char *zOutput;
    int nLen, i;
    
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    if (apArgs[0].type == CYPHER_VALUE_NULL) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }
    
    if (apArgs[0].type != CYPHER_VALUE_STRING) {
        return SQLITE_MISMATCH;
    }
    
    zInput = apArgs[0].u.zString;
    nLen = strlen(zInput);
    
    zOutput = sqlite3_malloc(nLen + 1);
    if (!zOutput) return SQLITE_NOMEM;
    
    for (i = 0; i < nLen; i++) {
        zOutput[i] = toupper(zInput[i]);
    }
    zOutput[nLen] = '\0';
    
    cypherValueSetString(pResult, zOutput);
    sqlite3_free(zOutput);
    
    return SQLITE_OK;
}

int cypherFunctionToLower(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    const char *zInput;
    char *zOutput;
    int nLen, i;
    
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    if (apArgs[0].type == CYPHER_VALUE_NULL) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }
    
    if (apArgs[0].type != CYPHER_VALUE_STRING) {
        return SQLITE_MISMATCH;
    }
    
    zInput = apArgs[0].u.zString;
    nLen = strlen(zInput);
    
    zOutput = sqlite3_malloc(nLen + 1);
    if (!zOutput) return SQLITE_NOMEM;
    
    for (i = 0; i < nLen; i++) {
        zOutput[i] = tolower(zInput[i]);
    }
    zOutput[nLen] = '\0';
    
    cypherValueSetString(pResult, zOutput);
    sqlite3_free(zOutput);
    
    return SQLITE_OK;
}

int cypherFunctionLength(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    if (apArgs[0].type == CYPHER_VALUE_NULL) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }
    
    if (apArgs[0].type == CYPHER_VALUE_STRING) {
        cypherValueSetInteger(pResult, strlen(apArgs[0].u.zString));
    } else {
        return SQLITE_MISMATCH;
    }
    
    return SQLITE_OK;
}

/* Math functions implementation */
int cypherFunctionAbs(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    if (apArgs[0].type == CYPHER_VALUE_NULL) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }
    
    if (apArgs[0].type == CYPHER_VALUE_INTEGER) {
        sqlite3_int64 val = apArgs[0].u.iInteger;
        cypherValueSetInteger(pResult, val < 0 ? -val : val);
    } else if (apArgs[0].type == CYPHER_VALUE_FLOAT) {
        cypherValueSetFloat(pResult, fabs(apArgs[0].u.rFloat));
    } else {
        return SQLITE_MISMATCH;
    }
    
    return SQLITE_OK;
}

int cypherFunctionCeil(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    double val;
    
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    if (apArgs[0].type == CYPHER_VALUE_NULL) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }
    
    if (apArgs[0].type == CYPHER_VALUE_INTEGER) {
        cypherValueSetInteger(pResult, apArgs[0].u.iInteger);
    } else if (apArgs[0].type == CYPHER_VALUE_FLOAT) {
        val = ceil(apArgs[0].u.rFloat);
        cypherValueSetFloat(pResult, val);
    } else {
        return SQLITE_MISMATCH;
    }
    
    return SQLITE_OK;
}

int cypherFunctionSqrt(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    double val;
    
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    if (apArgs[0].type == CYPHER_VALUE_NULL) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }
    
    if (apArgs[0].type == CYPHER_VALUE_INTEGER) {
        val = sqrt((double)apArgs[0].u.iInteger);
    } else if (apArgs[0].type == CYPHER_VALUE_FLOAT) {
        val = sqrt(apArgs[0].u.rFloat);
    } else {
        return SQLITE_MISMATCH;
    }
    
    if (val < 0) {
        cypherValueSetNull(pResult); /* NaN for negative values */
    } else {
        cypherValueSetFloat(pResult, val);
    }
    
    return SQLITE_OK;
}

/* Additional mathematical functions */
int cypherFunctionFloor(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    double rValue;
    
    if (!pResult) return SQLITE_MISUSE;
    cypherValueInit(pResult);
    
    if (nArgs != 1) {
        return SQLITE_MISUSE;
    }
    
    if (cypherValueIsNull(&apArgs[0])) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }
    
    switch (apArgs[0].type) {
        case CYPHER_VALUE_INTEGER:
            cypherValueSetInteger(pResult, apArgs[0].u.iInteger);
            return SQLITE_OK;
            
        case CYPHER_VALUE_FLOAT:
            rValue = floor(apArgs[0].u.rFloat);
            cypherValueSetFloat(pResult, rValue);
            return SQLITE_OK;
            
        default:
            return SQLITE_MISMATCH;
    }
}

int cypherFunctionRound(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    double rValue;
    
    if (!pResult) return SQLITE_MISUSE;
    cypherValueInit(pResult);
    
    if (nArgs != 1) {
        return SQLITE_MISUSE;
    }
    
    if (cypherValueIsNull(&apArgs[0])) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }
    
    switch (apArgs[0].type) {
        case CYPHER_VALUE_INTEGER:
            cypherValueSetInteger(pResult, apArgs[0].u.iInteger);
            return SQLITE_OK;
            
        case CYPHER_VALUE_FLOAT:
            rValue = round(apArgs[0].u.rFloat);
            cypherValueSetFloat(pResult, rValue);
            return SQLITE_OK;
            
        default:
            return SQLITE_MISMATCH;
    }
}

int cypherFunctionSize(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    /* Alias for length for now */
    return cypherFunctionLength(apArgs, nArgs, pResult);
}

int cypherFunctionToString(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    char *zStr;
    
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    zStr = cypherValueToString(&apArgs[0]);
    if (!zStr) return SQLITE_NOMEM;
    
    cypherValueSetString(pResult, zStr);
    sqlite3_free(zStr);
    
    return SQLITE_OK;
}

/*
** Evaluate logical operations (AND, OR, NOT).
**
** Parameters:
**   pLeft - Left operand (NULL for unary NOT)
**   pRight - Right operand
**   op - Logical operation type
**   pResult - Result value
**
** Returns: SQLITE_OK on success, error code on failure
*/
int cypherEvaluateLogical(const CypherValue *pLeft, const CypherValue *pRight,
                         CypherLogicalOp op, CypherValue *pResult) {
    int bLeft = 0, bRight = 0;
    
    if (!pResult) return SQLITE_MISUSE;
    cypherValueInit(pResult);
    
    switch (op) {
        case CYPHER_LOGIC_AND:
            if (!pLeft || !pRight) return SQLITE_MISUSE;
            
            /* Handle NULL values */
            if (cypherValueIsNull(pLeft) || cypherValueIsNull(pRight)) {
                cypherValueSetNull(pResult);
                return SQLITE_OK;
            }
            
            /* Convert to boolean */
            bLeft = cypherValueGetBoolean(pLeft);
            bRight = cypherValueGetBoolean(pRight);
            
            cypherValueSetBoolean(pResult, bLeft && bRight);
            return SQLITE_OK;
            
        case CYPHER_LOGIC_OR:
            if (!pLeft || !pRight) return SQLITE_MISUSE;
            
            /* Handle NULL values */
            if (cypherValueIsNull(pLeft) && cypherValueIsNull(pRight)) {
                cypherValueSetNull(pResult);
                return SQLITE_OK;
            }
            
            /* Convert to boolean */
            bLeft = cypherValueIsNull(pLeft) ? 0 : cypherValueGetBoolean(pLeft);
            bRight = cypherValueIsNull(pRight) ? 0 : cypherValueGetBoolean(pRight);
            
            /* If either is true, result is true */
            if (bLeft || bRight) {
                cypherValueSetBoolean(pResult, 1);
            } else if (cypherValueIsNull(pLeft) || cypherValueIsNull(pRight)) {
                cypherValueSetNull(pResult);
            } else {
                cypherValueSetBoolean(pResult, 0);
            }
            return SQLITE_OK;
            
        case CYPHER_LOGIC_NOT:
            if (!pRight) return SQLITE_MISUSE;
            
            /* Handle NULL values */
            if (cypherValueIsNull(pRight)) {
                cypherValueSetNull(pResult);
                return SQLITE_OK;
            }
            
            /* Convert to boolean and negate */
            bRight = cypherValueGetBoolean(pRight);
            cypherValueSetBoolean(pResult, !bRight);
            return SQLITE_OK;
            
        default:
            return SQLITE_MISUSE;
    }
}

/*
** Variable lookup in execution context.
**
** Parameters:
**   pCtx - Execution context
**   zVariable - Variable name
**   pResult - Result value
**
** Returns: SQLITE_OK on success, error code on failure
*/
int cypherExecutionContextLookupVariable(ExecutionContext *pCtx, 
                                       const char *zVariable, 
                                       CypherValue *pResult) {
    CypherValue *pValue;
    
    if (!pCtx || !zVariable || !pResult) return SQLITE_MISUSE;
    
    cypherValueInit(pResult);
    
    pValue = executionContextGet(pCtx, zVariable);
    if (pValue) {
        CypherValue *pCopy = cypherValueCopy((CypherValue*)pValue);
        if (!pCopy) return SQLITE_NOMEM;
        
        *pResult = *pCopy;
        sqlite3_free(pCopy);
    } else {
    cypherValueSetNull(pResult);
    }

    return SQLITE_OK;
}

/* Aggregate functions implementation */
int cypherFunctionCount(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    /* For COUNT(*), any non-null value counts as 1 */
    if (apArgs[0].type == CYPHER_VALUE_NULL) {
        cypherValueSetInteger(pResult, 0);
    } else {
        cypherValueSetInteger(pResult, 1);
    }
    
    return SQLITE_OK;
}

int cypherFunctionSum(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    switch (apArgs[0].type) {
        case CYPHER_VALUE_INTEGER:
            cypherValueSetInteger(pResult, apArgs[0].u.iInteger);
            return SQLITE_OK;
            
        case CYPHER_VALUE_FLOAT:
            cypherValueSetFloat(pResult, apArgs[0].u.rFloat);
            return SQLITE_OK;
            
        case CYPHER_VALUE_NULL:
            cypherValueSetNull(pResult);
            return SQLITE_OK;
            
        default:
            return SQLITE_MISMATCH;
    }
}

int cypherFunctionAvg(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    switch (apArgs[0].type) {
        case CYPHER_VALUE_INTEGER:
            cypherValueSetFloat(pResult, (double)apArgs[0].u.iInteger);
            return SQLITE_OK;
            
        case CYPHER_VALUE_FLOAT:
            cypherValueSetFloat(pResult, apArgs[0].u.rFloat);
            return SQLITE_OK;
            
        case CYPHER_VALUE_NULL:
            cypherValueSetNull(pResult);
            return SQLITE_OK;
            
        default:
            return SQLITE_MISMATCH;
    }
}

int cypherFunctionMin(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    
    *pResult = apArgs[0];
    return SQLITE_OK;
}

int cypherFunctionMax(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;

    *pResult = apArgs[0];
    return SQLITE_OK;
}

/* Type conversion functions */
int cypherFunctionToInteger(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    cypherValueInit(pResult);

    if (cypherValueIsNull(&apArgs[0])) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }

    switch (apArgs[0].type) {
        case CYPHER_VALUE_INTEGER:
            cypherValueSetInteger(pResult, apArgs[0].u.iInteger);
            return SQLITE_OK;

        case CYPHER_VALUE_FLOAT:
            cypherValueSetInteger(pResult, (sqlite3_int64)apArgs[0].u.rFloat);
            return SQLITE_OK;

        case CYPHER_VALUE_STRING: {
            const char *z = apArgs[0].u.zString;
            char *zEnd;
            sqlite3_int64 iValue = strtoll(z, &zEnd, 10);
            if (*zEnd == '\0' && zEnd != z) {
                cypherValueSetInteger(pResult, iValue);
                return SQLITE_OK;
            }
            cypherValueSetNull(pResult);
            return SQLITE_OK;
        }

        case CYPHER_VALUE_BOOLEAN:
            cypherValueSetInteger(pResult, apArgs[0].u.bBoolean ? 1 : 0);
            return SQLITE_OK;

        default:
            cypherValueSetNull(pResult);
            return SQLITE_OK;
    }
}

int cypherFunctionToFloat(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    if (nArgs != 1 || !pResult) return SQLITE_MISUSE;
    cypherValueInit(pResult);

    if (cypherValueIsNull(&apArgs[0])) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }

    switch (apArgs[0].type) {
        case CYPHER_VALUE_INTEGER:
            cypherValueSetFloat(pResult, (double)apArgs[0].u.iInteger);
            return SQLITE_OK;

        case CYPHER_VALUE_FLOAT:
            cypherValueSetFloat(pResult, apArgs[0].u.rFloat);
            return SQLITE_OK;

        case CYPHER_VALUE_STRING: {
            const char *z = apArgs[0].u.zString;
            char *zEnd;
            double rValue = strtod(z, &zEnd);
            if (*zEnd == '\0' && zEnd != z) {
                cypherValueSetFloat(pResult, rValue);
                return SQLITE_OK;
            }
            cypherValueSetNull(pResult);
            return SQLITE_OK;
        }

        case CYPHER_VALUE_BOOLEAN:
            cypherValueSetFloat(pResult, apArgs[0].u.bBoolean ? 1.0 : 0.0);
            return SQLITE_OK;

        default:
            cypherValueSetNull(pResult);
            return SQLITE_OK;
    }
}

/* Coalesce function - returns first non-null value */
int cypherFunctionCoalesce(CypherValue *apArgs, int nArgs, CypherValue *pResult) {
    int i;

    if (nArgs < 1 || !pResult) return SQLITE_MISUSE;
    cypherValueInit(pResult);

    for (i = 0; i < nArgs; i++) {
        if (!cypherValueIsNull(&apArgs[i])) {
            CypherValue *pCopy = cypherValueCopy(&apArgs[i]);
            if (!pCopy) return SQLITE_NOMEM;
            *pResult = *pCopy;
            sqlite3_free(pCopy);
            return SQLITE_OK;
        }
    }

    cypherValueSetNull(pResult);
    return SQLITE_OK;
}

/*
** Convert a CypherAst node to a CypherExpression object.
** This bridges the parser's AST with the expression evaluator.
*/
static int cypherAstToExpression(CypherAst *pAst, CypherExpression **ppExpr) {
    CypherExpression *pExpr = NULL;
    CypherValue value;
    const char *zValue;
    int rc = SQLITE_OK;

    if (!pAst || !ppExpr) return SQLITE_MISUSE;

    zValue = cypherAstGetValue(pAst);

    switch (pAst->type) {
        case CYPHER_AST_LITERAL: {
            /* Parse literal value */
            cypherValueInit(&value);

            if (!zValue) {
                cypherValueSetNull(&value);
            } else {
                size_t nLen = strlen(zValue);

                /* String literal (quoted) */
                if (nLen >= 2 &&
                    ((zValue[0] == '\'' && zValue[nLen-1] == '\'') ||
                     (zValue[0] == '"' && zValue[nLen-1] == '"'))) {
                    char *zStr = sqlite3_mprintf("%.*s", (int)(nLen - 2), zValue + 1);
                    if (!zStr) return SQLITE_NOMEM;
                    cypherValueSetString(&value, zStr);
                    sqlite3_free(zStr);
                }
                /* Boolean literals */
                else if (sqlite3_stricmp(zValue, "true") == 0) {
                    cypherValueSetBoolean(&value, 1);
                }
                else if (sqlite3_stricmp(zValue, "false") == 0) {
                    cypherValueSetBoolean(&value, 0);
                }
                /* Null literal */
                else if (sqlite3_stricmp(zValue, "null") == 0) {
                    cypherValueSetNull(&value);
                }
                /* Numeric literals */
                else {
                    const char *p = zValue;
                    int bFloat = 0;

                    if (*p == '-' || *p == '+') p++;
                    while (isdigit(*p)) p++;

                    if (*p == '.') {
                        bFloat = 1;
                        p++;
                        while (isdigit(*p)) p++;
                    }

                    if (*p == 'e' || *p == 'E') {
                        bFloat = 1;
                        p++;
                        if (*p == '-' || *p == '+') p++;
                        while (isdigit(*p)) p++;
                    }

                    if (*p == '\0') {
                        if (bFloat) {
                            cypherValueSetFloat(&value, atof(zValue));
                        } else {
                            cypherValueSetInteger(&value, atoll(zValue));
                        }
                    } else {
                        /* Not a valid number, treat as unquoted string */
                        cypherValueSetString(&value, zValue);
                    }
                }
            }

            rc = cypherExpressionCreateLiteral(ppExpr, &value);
            cypherValueDestroy(&value);
            return rc;
        }

        case CYPHER_AST_IDENTIFIER: {
            /* Variable reference */
            if (!zValue) return SQLITE_ERROR;

            rc = cypherExpressionCreate(&pExpr, CYPHER_EXPR_VARIABLE);
            if (rc != SQLITE_OK) return rc;

            pExpr->u.variable.zName = sqlite3_mprintf("%s", zValue);
            if (!pExpr->u.variable.zName) {
                cypherExpressionDestroy(pExpr);
                return SQLITE_NOMEM;
            }

            *ppExpr = pExpr;
            return SQLITE_OK;
        }

        case CYPHER_AST_PROPERTY: {
            /* Property access: object.property */
            CypherExpression *pObject = NULL;
            CypherAst *pObjectAst;
            const char *zProperty;

            if (cypherAstGetChildCount(pAst) < 2) return SQLITE_ERROR;

            pObjectAst = cypherAstGetChild(pAst, 0);
            rc = cypherAstToExpression(pObjectAst, &pObject);
            if (rc != SQLITE_OK) return rc;

            zProperty = cypherAstGetValue(cypherAstGetChild(pAst, 1));
            if (!zProperty) {
                cypherExpressionDestroy(pObject);
                return SQLITE_ERROR;
            }

            rc = cypherExpressionCreate(&pExpr, CYPHER_EXPR_PROPERTY);
            if (rc != SQLITE_OK) {
                cypherExpressionDestroy(pObject);
                return rc;
            }

            pExpr->u.property.pObject = pObject;
            pExpr->u.property.zProperty = sqlite3_mprintf("%s", zProperty);
            if (!pExpr->u.property.zProperty) {
                cypherExpressionDestroy(pExpr);
                return SQLITE_NOMEM;
            }

            *ppExpr = pExpr;
            return SQLITE_OK;
        }

        case CYPHER_AST_BINARY_OP: {
            /* Binary operations: arithmetic, comparison, etc. */
            CypherExpression *pLeft = NULL, *pRight = NULL;
            CypherAst *pLeftAst, *pRightAst;
            const char *zOp;

            if (cypherAstGetChildCount(pAst) < 2) return SQLITE_ERROR;

            pLeftAst = cypherAstGetChild(pAst, 0);
            pRightAst = cypherAstGetChild(pAst, 1);
            zOp = zValue;

            if (!zOp) return SQLITE_ERROR;

            rc = cypherAstToExpression(pLeftAst, &pLeft);
            if (rc != SQLITE_OK) return rc;

            rc = cypherAstToExpression(pRightAst, &pRight);
            if (rc != SQLITE_OK) {
                cypherExpressionDestroy(pLeft);
                return rc;
            }

            /* Determine operator type */
            if (strcmp(zOp, "+") == 0) {
                rc = cypherExpressionCreateArithmetic(ppExpr, pLeft, pRight, CYPHER_OP_ADD);
            } else if (strcmp(zOp, "-") == 0) {
                rc = cypherExpressionCreateArithmetic(ppExpr, pLeft, pRight, CYPHER_OP_SUBTRACT);
            } else if (strcmp(zOp, "*") == 0) {
                rc = cypherExpressionCreateArithmetic(ppExpr, pLeft, pRight, CYPHER_OP_MULTIPLY);
            } else if (strcmp(zOp, "/") == 0) {
                rc = cypherExpressionCreateArithmetic(ppExpr, pLeft, pRight, CYPHER_OP_DIVIDE);
            } else if (strcmp(zOp, "%") == 0) {
                rc = cypherExpressionCreateArithmetic(ppExpr, pLeft, pRight, CYPHER_OP_MODULO);
            } else if (strcmp(zOp, "^") == 0) {
                rc = cypherExpressionCreateArithmetic(ppExpr, pLeft, pRight, CYPHER_OP_POWER);
            } else if (strcmp(zOp, "=") == 0) {
                rc = cypherExpressionCreateComparison(ppExpr, pLeft, pRight, CYPHER_CMP_EQUAL);
            } else if (strcmp(zOp, "<>") == 0) {
                rc = cypherExpressionCreateComparison(ppExpr, pLeft, pRight, CYPHER_CMP_NOT_EQUAL);
            } else if (strcmp(zOp, "<") == 0) {
                rc = cypherExpressionCreateComparison(ppExpr, pLeft, pRight, CYPHER_CMP_LESS);
            } else if (strcmp(zOp, "<=") == 0) {
                rc = cypherExpressionCreateComparison(ppExpr, pLeft, pRight, CYPHER_CMP_LESS_EQUAL);
            } else if (strcmp(zOp, ">") == 0) {
                rc = cypherExpressionCreateComparison(ppExpr, pLeft, pRight, CYPHER_CMP_GREATER);
            } else if (strcmp(zOp, ">=") == 0) {
                rc = cypherExpressionCreateComparison(ppExpr, pLeft, pRight, CYPHER_CMP_GREATER_EQUAL);
            } else {
                cypherExpressionDestroy(pLeft);
                cypherExpressionDestroy(pRight);
                return SQLITE_ERROR;
            }

            if (rc != SQLITE_OK) {
                cypherExpressionDestroy(pLeft);
                cypherExpressionDestroy(pRight);
            }
            return rc;
        }

        case CYPHER_AST_FUNCTION_CALL: {
            /* Function call */
            const char *zFuncName;
            int nArgs, i;
            CypherExpression **apArgs = NULL;

            if (cypherAstGetChildCount(pAst) < 1) return SQLITE_ERROR;

            zFuncName = cypherAstGetValue(cypherAstGetChild(pAst, 0));
            nArgs = cypherAstGetChildCount(pAst) - 1;

            if (nArgs > 0) {
                apArgs = sqlite3_malloc(sizeof(CypherExpression*) * nArgs);
                if (!apArgs) return SQLITE_NOMEM;

                for (i = 0; i < nArgs; i++) {
                    CypherAst *pArgAst = cypherAstGetChild(pAst, i + 1);
                    rc = cypherAstToExpression(pArgAst, &apArgs[i]);
                    if (rc != SQLITE_OK) {
                        /* Cleanup on error */
                        for (int j = 0; j < i; j++) {
                            cypherExpressionDestroy(apArgs[j]);
                        }
                        sqlite3_free(apArgs);
                        return rc;
                    }
                }
            }

            rc = cypherExpressionCreate(&pExpr, CYPHER_EXPR_FUNCTION);
            if (rc != SQLITE_OK) {
                if (apArgs) {
                    for (i = 0; i < nArgs; i++) {
                        cypherExpressionDestroy(apArgs[i]);
                    }
                    sqlite3_free(apArgs);
                }
                return rc;
            }

            pExpr->u.function.zName = sqlite3_mprintf("%s", zFuncName);
            if (!pExpr->u.function.zName) {
                if (apArgs) {
                    for (i = 0; i < nArgs; i++) {
                        cypherExpressionDestroy(apArgs[i]);
                    }
                    sqlite3_free(apArgs);
                }
                cypherExpressionDestroy(pExpr);
                return SQLITE_NOMEM;
            }

            pExpr->u.function.apArgs = apArgs;
            pExpr->u.function.nArgs = nArgs;

            *ppExpr = pExpr;
            return SQLITE_OK;
        }

        default:
            /* Unsupported AST node type */
            return SQLITE_ERROR;
    }
}

/*
** Public API: Evaluate an AST expression node.
** This is the main entry point for evaluating expressions from the parser.
*/
int cypherEvaluateAstExpression(CypherAst *pAst, ExecutionContext *pContext, CypherValue **ppResult) {
    CypherExpression *pExpr = NULL;
    CypherValue *pResult = NULL;
    int rc;

    if (!pAst || !ppResult) return SQLITE_MISUSE;

    /* Allocate result */
    pResult = sqlite3_malloc(sizeof(CypherValue));
    if (!pResult) return SQLITE_NOMEM;
    cypherValueInit(pResult);

    /* Convert AST to expression */
    rc = cypherAstToExpression(pAst, &pExpr);
    if (rc != SQLITE_OK) {
        sqlite3_free(pResult);
        return rc;
    }

    /* Evaluate expression */
    rc = cypherExpressionEvaluate(pExpr, pContext, pResult);

    /* Cleanup expression */
    cypherExpressionDestroy(pExpr);

    if (rc != SQLITE_OK) {
        cypherValueDestroy(pResult);
        sqlite3_free(pResult);
        return rc;
    }

    *ppResult = pResult;
    return SQLITE_OK;
}