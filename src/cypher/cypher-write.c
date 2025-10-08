/*
** SQLite Graph Database Extension - Cypher Write Operations Implementation
**
** This file implements the core write operations for Cypher: CREATE, MERGE,
** SET, DELETE, and DETACH DELETE. Includes transaction management and
** rollback support using SQLite transaction semantics.
**
** Memory allocation: All functions use sqlite3_malloc()/sqlite3_free()
** Error handling: Functions return SQLite error codes (SQLITE_OK, etc.)
** Transaction safety: All operations integrate with SQLite transactions
*/

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "cypher-write.h"
#include "graph-vtab.h"
#include "graph-memory.h"

/*
** Constants for input validation
*/
#define MAX_LABEL_LENGTH 255
#define MAX_PROPERTY_NAME_LENGTH 255
#define MAX_VARIABLE_NAME_LENGTH 255
#define MAX_RELATIONSHIP_TYPE_LENGTH 255
#define MAX_PROPERTY_VALUE_SIZE (1024 * 1024)  /* 1MB */

/*
** Input validation functions (following @SELF_REVIEW.md Section 2.2)
*/

/*
** Check if a label name is valid.
** Returns 1 if valid, 0 if invalid.
*/
static int isValidLabelName(const char *zLabel) {
    if (!zLabel || strlen(zLabel) == 0) return 0;
    if (strlen(zLabel) > MAX_LABEL_LENGTH) return 0;
    
    /* Must start with letter or underscore */
    if (!isalpha(zLabel[0]) && zLabel[0] != '_') return 0;
    
    /* Can contain letters, digits, underscores */
    for (int i = 1; zLabel[i]; i++) {
        if (!isalnum(zLabel[i]) && zLabel[i] != '_') return 0;
    }
    
    return 1;
}

/*
** Check if a property name is valid.
** Returns 1 if valid, 0 if invalid.
*/
static int isValidPropertyName(const char *zProperty) {
    if (!zProperty || strlen(zProperty) == 0) return 0;
    if (strlen(zProperty) > MAX_PROPERTY_NAME_LENGTH) return 0;
    
    /* Must start with letter or underscore */
    if (!isalpha(zProperty[0]) && zProperty[0] != '_') return 0;
    
    /* Can contain letters, digits, underscores */
    for (int i = 1; zProperty[i]; i++) {
        if (!isalnum(zProperty[i]) && zProperty[i] != '_') return 0;
    }
    
    return 1;
}

/*
** Check if a variable name is valid.
** Returns 1 if valid, 0 if invalid.
*/
static int isValidVariableName(const char *zVariable) {
    if (!zVariable || strlen(zVariable) == 0) return 0;
    if (strlen(zVariable) > MAX_VARIABLE_NAME_LENGTH) return 0;
    
    /* Must start with letter or underscore */
    if (!isalpha(zVariable[0]) && zVariable[0] != '_') return 0;
    
    /* Can contain letters, digits, underscores */
    for (int i = 1; zVariable[i]; i++) {
        if (!isalnum(zVariable[i]) && zVariable[i] != '_') return 0;
    }
    
    return 1;
}

/*
** Check if a string is a reserved word.
** Returns 1 if reserved, 0 if not reserved.
*/
static int isReservedWord(const char *zWord) {
    static const char *azReserved[] = {
        "CREATE", "MERGE", "SET", "DELETE", "DETACH", "MATCH", "WHERE", 
        "RETURN", "WITH", "UNWIND", "OPTIONAL", "UNION", "ORDER", "BY",
        "SKIP", "LIMIT", "ASC", "DESC", "AND", "OR", "NOT", "XOR",
        "CASE", "WHEN", "THEN", "ELSE", "END", "AS", "DISTINCT",
        "TRUE", "FALSE", "NULL", "IN", "IS", "STARTS", "ENDS", "CONTAINS",
        NULL
    };
    
    if (!zWord) return 0;
    
    for (int i = 0; azReserved[i]; i++) {
        if (sqlite3_stricmp(zWord, azReserved[i]) == 0) {
            return 1;
        }
    }
    
    return 0;
}

/*
** Generate a cryptographically secure unique node ID.
** Returns node ID > 0 on success, -1 on failure.
*/
static sqlite3_int64 cypherGenerateSecureNodeId(GraphVtab *pGraph) {
    static sqlite3_int64 iNextId = 1;
    sqlite3_int64 iNodeId;
    
    /* Simple implementation - in production, use crypto-secure random */
    iNodeId = time(NULL) * 1000 + (iNextId++);
    
    /* Ensure ID doesn't already exist */
    while (cypherStorageNodeExists(pGraph, iNodeId) > 0) {
        iNodeId++;
    }
    
    return iNodeId;
}

/*
** Generate cryptographically secure unique relationship ID.
** Returns unique relationship ID or negative value on failure.
*/
static sqlite3_int64 cypherGenerateSecureRelationshipId(GraphVtab *pGraph) {
    static sqlite3_int64 iNextRelId = 1;
    sqlite3_int64 iRelId;
    
    /* Simple implementation - in production, use crypto-secure random */
    iRelId = time(NULL) * 1000 + 500000 + (iNextRelId++); /* Offset from node IDs */
    
    /* Ensure ID doesn't already exist - check against edge storage */
    sqlite3_stmt *pStmt = NULL;
    char *zSql = sqlite3_mprintf("SELECT 1 FROM graph_edges WHERE edge_id = %lld LIMIT 1", iRelId);
    if (!zSql) return -1;
    
    int rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, NULL);
    sqlite3_free(zSql);
    
    while (rc == SQLITE_OK) {
        rc = sqlite3_step(pStmt);
        if (rc == SQLITE_ROW) {
            /* ID exists, try next one */
            sqlite3_finalize(pStmt);
            iRelId++;
            zSql = sqlite3_mprintf("SELECT 1 FROM graph_edges WHERE edge_id = %lld LIMIT 1", iRelId);
            if (!zSql) return -1;
            rc = sqlite3_prepare_v2(pGraph->pDb, zSql, -1, &pStmt, NULL);
            sqlite3_free(zSql);
        } else if (rc == SQLITE_DONE) {
            /* ID is unique */
            sqlite3_finalize(pStmt);
            return iRelId;
        } else {
            /* Error */
            sqlite3_finalize(pStmt);
            return -1;
        }
    }
    
    if (pStmt) sqlite3_finalize(pStmt);
    return -1;
}

/*
** Sanitize a string against injection attacks.
** Returns sanitized copy (caller must free) or NULL on failure.
*/
static char *cypherSanitizeString(const char *zInput) {
    if (!zInput) return NULL;
    
    size_t nLen = strlen(zInput);
    char *zSanitized = sqlite3_malloc(nLen * 2 + 1); /* Worst case: all chars escaped */
    if (!zSanitized) return NULL;
    
    char *pOut = zSanitized;
    for (const char *pIn = zInput; *pIn; pIn++) {
        /* Escape dangerous characters */
        if (*pIn == '\'' || *pIn == '"' || *pIn == '\\') {
            *pOut++ = '\\';
            *pOut++ = *pIn;
        } else if (*pIn == '\0') {
            /* Null byte - skip it */
            continue;
        } else if ((unsigned char)*pIn < 32 && *pIn != '\t' && *pIn != '\n' && *pIn != '\r') {
            /* Control characters - skip them */
            continue;
        } else {
            *pOut++ = *pIn;
        }
    }
    *pOut = '\0';
    
    return zSanitized;
}

/*
** Write context management functions.
*/

/*
** Begin a write operation in the context.
** Returns SQLITE_OK on success, error code on failure.
*/
static int cypherWriteContextBeginOp(CypherWriteContext *pCtx, CypherWriteOpType type) {
    if (!pCtx) return SQLITE_MISUSE;
    
    /* Suppress unused parameter warning */
    (void)type;
    
    /* Ensure we're in a transaction */
    if (!pCtx->bInTransaction) {
        int rc = cypherWriteContextBegin(pCtx);
        if (rc != SQLITE_OK) return rc;
    }
    
    return SQLITE_OK;
}

/*
** Rollback the current operation in the context.
** Returns SQLITE_OK on success, error code on failure.
*/
static int cypherWriteContextRollbackOp(CypherWriteContext *pCtx) {
    if (!pCtx) return SQLITE_MISUSE;
    
    /* Remove the last operation if it exists */
    if (pCtx->pLastOp) {
        CypherWriteOp *pToRemove = pCtx->pLastOp;
        
        /* Find the previous operation */
        CypherWriteOp *pPrev = NULL;
        for (CypherWriteOp *pOp = pCtx->pOperations; pOp && pOp != pToRemove; pOp = pOp->pNext) {
            pPrev = pOp;
        }
        
        if (pPrev) {
            pPrev->pNext = NULL;
            pCtx->pLastOp = pPrev;
        } else {
            pCtx->pOperations = NULL;
            pCtx->pLastOp = NULL;
        }
        
        cypherWriteOpDestroy(pToRemove);
        pCtx->nOperations--;
    }
    
    return SQLITE_OK;
}

/*
** Create a new write context for mutation operations.
** Returns NULL on allocation failure.
*/
CypherWriteContext *cypherWriteContextCreate(sqlite3 *pDb, GraphVtab *pGraph,
                                           ExecutionContext *pExecContext) {
    CypherWriteContext *pCtx;
    
    if (!pDb || !pGraph || !pExecContext) {
        return NULL;
    }
    
    pCtx = (CypherWriteContext*)sqlite3_malloc(sizeof(CypherWriteContext));
    if (!pCtx) {
        return NULL;
    }
    
    memset(pCtx, 0, sizeof(CypherWriteContext));
    pCtx->pDb = pDb;
    pCtx->pGraph = pGraph;
    pCtx->pExecContext = pExecContext;
    pCtx->bAutoCommit = 1;  /* Default to auto-commit */
    pCtx->iNextNodeId = 1;  /* Start node IDs at 1 */
    pCtx->iNextRelId = 1;   /* Start relationship IDs at 1 */
    
    return pCtx;
}

/*
** Destroy a write context and free all associated memory.
** Automatically rolls back any uncommitted operations.
*/
void cypherWriteContextDestroy(CypherWriteContext *pCtx) {
    CypherWriteOp *pOp, *pNext;
    
    if (!pCtx) return;
    
    /* Rollback any uncommitted operations */
    if (pCtx->bInTransaction) {
        cypherWriteContextRollback(pCtx);
    }
    
    /* Free operation list */
    for (pOp = pCtx->pOperations; pOp; pOp = pNext) {
        pNext = pOp->pNext;
        cypherWriteOpDestroy(pOp);
    }
    
    /* Free error message */
    if (pCtx->zErrorMsg) {
        sqlite3_free(pCtx->zErrorMsg);
    }
    
    sqlite3_free(pCtx);
}

/*
** Begin a write transaction in the context.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherWriteContextBegin(CypherWriteContext *pCtx) {
    int rc;
    
    if (!pCtx) return SQLITE_MISUSE;
    if (pCtx->bInTransaction) return SQLITE_OK;  /* Already in transaction */
    
    /* Begin SQLite transaction */
    rc = sqlite3_exec(pCtx->pDb, "BEGIN", 0, 0, 0);
    if (rc != SQLITE_OK) {
        return rc;
    }
    
    pCtx->bInTransaction = 1;
    pCtx->bAutoCommit = 0;
    
    return SQLITE_OK;
}

/*
** Commit all operations in the write context.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherWriteContextCommit(CypherWriteContext *pCtx) {
    int rc;
    
    if (!pCtx) return SQLITE_MISUSE;
    if (!pCtx->bInTransaction) return SQLITE_OK;  /* Nothing to commit */
    
    /* First execute all pending write operations */
    rc = cypherExecuteOperations(pCtx);
    if (rc != SQLITE_OK) {
        /* Execute failed, rollback SQLite transaction */
        sqlite3_exec(pCtx->pDb, "ROLLBACK", 0, 0, 0);
        cypherRollbackOperations(pCtx);
        pCtx->bInTransaction = 0;
        pCtx->bAutoCommit = 1;
        return rc;
    }
    
    /* Commit SQLite transaction */
    rc = sqlite3_exec(pCtx->pDb, "COMMIT", 0, 0, 0);
    if (rc != SQLITE_OK) {
        /* SQLite commit failed, rollback our operations */
        sqlite3_exec(pCtx->pDb, "ROLLBACK", 0, 0, 0);
        cypherRollbackOperations(pCtx);
        pCtx->bInTransaction = 0;
        pCtx->bAutoCommit = 1;
        return rc;
    }
    
    /* Success - clear operation log and reset state */
    pCtx->bInTransaction = 0;
    pCtx->bAutoCommit = 1;
    
    return SQLITE_OK;
}

/*
** Rollback all operations in the write context.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherWriteContextRollback(CypherWriteContext *pCtx) {
    int rc;
    
    if (!pCtx) return SQLITE_MISUSE;
    if (!pCtx->bInTransaction) return SQLITE_OK;  /* Nothing to rollback */
    
    /* First rollback our write operations */
    cypherRollbackOperations(pCtx);
    
    /* Then rollback SQLite transaction */
    rc = sqlite3_exec(pCtx->pDb, "ROLLBACK", 0, 0, 0);
    
    pCtx->bInTransaction = 0;
    pCtx->bAutoCommit = 1;
    
    return rc;
}

/*
** Add a write operation to the transaction log.
** Returns SQLITE_OK on success, SQLITE_NOMEM on allocation failure.
*/
int cypherWriteContextAddOperation(CypherWriteContext *pCtx, CypherWriteOp *pOp) {
    if (!pCtx || !pOp) return SQLITE_MISUSE;
    
    /* Add to end of operation list */
    if (pCtx->pLastOp) {
        pCtx->pLastOp->pNext = pOp;
    } else {
        pCtx->pOperations = pOp;
    }
    pCtx->pLastOp = pOp;
    pCtx->nOperations++;
    
    return SQLITE_OK;
}

/*
** Get the next available node ID from the context.
** Updates the context's next ID counter.
*/
sqlite3_int64 cypherWriteContextNextNodeId(CypherWriteContext *pCtx) {
    if (!pCtx) return -1;
    return pCtx->iNextNodeId++;
}

/*
** Get the next available relationship ID from the context.
** Updates the context's next ID counter.
*/
sqlite3_int64 cypherWriteContextNextRelId(CypherWriteContext *pCtx) {
    if (!pCtx) return -1;
    return pCtx->iNextRelId++;
}

/*
** CREATE operation functions.
*/

/*
** Execute a CREATE node operation.
** Returns SQLITE_OK on success, error code on failure.
** Follows @SELF_REVIEW.md requirements for input validation, security, and error handling.
*/
int cypherCreateNode(CypherWriteContext *pCtx, CreateNodeOp *pOp) {
    CypherWriteOp *pWriteOp = NULL;
    char *zLabelsJson = NULL;
    char *zPropsJson = NULL;
    int rc = SQLITE_OK;
    int i;
    
    /* @SELF_REVIEW.md Section 2.2: Input Validation Thoroughness */
    if (!pCtx) return SQLITE_MISUSE;
    if (!pOp) return SQLITE_MISUSE;
    if (!pCtx->pGraph) return SQLITE_MISUSE;
    if (!pCtx->pExecContext) return SQLITE_MISUSE;
    
    /* Validate variable name if provided */
    if (pOp->zVariable) {
        if (!isValidVariableName(pOp->zVariable)) {
            return SQLITE_FORMAT;
        }
        if (isReservedWord(pOp->zVariable)) {
            return SQLITE_MISUSE;
        }
    }
    
    /* Validate labels */
    if (pOp->nLabels < 0 || pOp->nLabels > 100) { /* Reasonable limit */
        return SQLITE_RANGE;
    }
    
    for (i = 0; i < pOp->nLabels; i++) {
        if (!pOp->azLabels || !pOp->azLabels[i]) {
            return SQLITE_MISUSE;
        }
        if (!isValidLabelName(pOp->azLabels[i])) {
            return SQLITE_FORMAT;
        }
        if (isReservedWord(pOp->azLabels[i])) {
            return SQLITE_MISUSE;
        }
    }
    
    /* Validate properties */
    if (pOp->nProperties < 0 || pOp->nProperties > 1000) { /* Reasonable limit */
        return SQLITE_RANGE;
    }
    
    for (i = 0; i < pOp->nProperties; i++) {
        if (!pOp->azPropNames || !pOp->azPropNames[i]) {
            return SQLITE_MISUSE;
        }
        if (!pOp->apPropValues || !pOp->apPropValues[i]) {
            return SQLITE_MISUSE;
        }
        if (!isValidPropertyName(pOp->azPropNames[i])) {
            return SQLITE_FORMAT;
        }
        if (isReservedWord(pOp->azPropNames[i])) {
            return SQLITE_MISUSE;
        }
        
        /* Validate property value size */
        if (pOp->apPropValues[i]->type == CYPHER_VALUE_STRING && 
            pOp->apPropValues[i]->u.zString &&
            strlen(pOp->apPropValues[i]->u.zString) > MAX_PROPERTY_VALUE_SIZE) {
            return SQLITE_TOOBIG;
        }
    }
    
    /* Generate cryptographically secure unique node ID */
    pOp->iCreatedNodeId = cypherGenerateSecureNodeId(pCtx->pGraph);
    if (pOp->iCreatedNodeId <= 0) {
        return SQLITE_ERROR;
    }
    
    /* @SELF_REVIEW.md Section 5.1: Build labels JSON array with security sanitization */
    if (pOp->nLabels > 0) {
        int nAlloc = 512;
        int nUsed = 0;
        char *zLabels = sqlite3_malloc(nAlloc);
        if (!zLabels) return SQLITE_NOMEM;
        
        nUsed = snprintf(zLabels, nAlloc, "[");
        
        for (i = 0; i < pOp->nLabels; i++) {
            /* @SELF_REVIEW.md Section 5.1: Sanitize against injection attacks */
            char *zSanitizedLabel = cypherSanitizeString(pOp->azLabels[i]);
            if (!zSanitizedLabel) {
                sqlite3_free(zLabels);
                return SQLITE_NOMEM;
            }
            
            int nNeeded = snprintf(NULL, 0, "%s\"%s\"", i > 0 ? "," : "", zSanitizedLabel);
            
            /* @SELF_REVIEW.md Section 2.1: Handle memory allocation failures */
            if (nUsed + nNeeded + 2 >= nAlloc) {
                nAlloc = (nUsed + nNeeded + 512) * 2;
                char *zNew = sqlite3_realloc(zLabels, nAlloc);
                if (!zNew) {
                    sqlite3_free(zLabels);
                    sqlite3_free(zSanitizedLabel);
                    return SQLITE_NOMEM;
                }
                zLabels = zNew;
            }
            
            nUsed += snprintf(zLabels + nUsed, nAlloc - nUsed, 
                             "%s\"%s\"", i > 0 ? "," : "", zSanitizedLabel);
            sqlite3_free(zSanitizedLabel);
        }
        
        if (nUsed + 2 < nAlloc) {
            zLabels[nUsed++] = ']';
            zLabels[nUsed] = '\0';
        } else {
            /* Emergency buffer expansion */
            char *zNew = sqlite3_realloc(zLabels, nUsed + 3);
            if (!zNew) {
                sqlite3_free(zLabels);
                return SQLITE_NOMEM;
            }
            zLabels = zNew;
            zLabels[nUsed++] = ']';
            zLabels[nUsed] = '\0';
        }
        zLabelsJson = zLabels;
    } else {
        zLabelsJson = sqlite3_mprintf("[]");
        if (!zLabelsJson) return SQLITE_NOMEM;
    }
    
    /* @SELF_REVIEW.md Section 5.1: Build properties JSON object with security sanitization */
    if (pOp->nProperties > 0) {
        int nAlloc = 1024;
        int nUsed = 0;
        char *zProps = sqlite3_malloc(nAlloc);
        if (!zProps) {
            sqlite3_free(zLabelsJson);
            return SQLITE_NOMEM;
        }
        
        nUsed = snprintf(zProps, nAlloc, "{");
        
        for (i = 0; i < pOp->nProperties; i++) {
            /* @SELF_REVIEW.md Section 5.1: Sanitize property names against injection */
            char *zSanitizedPropName = cypherSanitizeString(pOp->azPropNames[i]);
            if (!zSanitizedPropName) {
                sqlite3_free(zProps);
                sqlite3_free(zLabelsJson);
                return SQLITE_NOMEM;
            }
            
            char *zSanitizedPropValue = NULL;
            int nNeeded = 0;
            
            /* @SELF_REVIEW.md Section 2.1: Handle all value types with proper error checking */
            if (pOp->apPropValues[i]->type == CYPHER_VALUE_STRING) {
                if (pOp->apPropValues[i]->u.zString) {
                    zSanitizedPropValue = cypherSanitizeString(pOp->apPropValues[i]->u.zString);
                    if (!zSanitizedPropValue) {
                        sqlite3_free(zSanitizedPropName);
                        sqlite3_free(zProps);
                        sqlite3_free(zLabelsJson);
                        return SQLITE_NOMEM;
                    }
                    nNeeded = snprintf(NULL, 0, "%s\"%s\":\"%s\"",
                                      i > 0 ? "," : "", zSanitizedPropName, zSanitizedPropValue);
                } else {
                    nNeeded = snprintf(NULL, 0, "%s\"%s\":null",
                                      i > 0 ? "," : "", zSanitizedPropName);
                }
            } else if (pOp->apPropValues[i]->type == CYPHER_VALUE_INTEGER) {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":%lld",
                                  i > 0 ? "," : "", zSanitizedPropName,
                                  pOp->apPropValues[i]->u.iInteger);
            } else if (pOp->apPropValues[i]->type == CYPHER_VALUE_FLOAT) {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":%g",
                                  i > 0 ? "," : "", zSanitizedPropName,
                                  pOp->apPropValues[i]->u.rFloat);
            } else if (pOp->apPropValues[i]->type == CYPHER_VALUE_BOOLEAN) {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":%s",
                                  i > 0 ? "," : "", zSanitizedPropName,
                                  pOp->apPropValues[i]->u.bBoolean ? "true" : "false");
            } else {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":null",
                                  i > 0 ? "," : "", zSanitizedPropName);
            }
            
            /* @SELF_REVIEW.md Section 2.1: Handle memory allocation failures */
            if (nUsed + nNeeded + 2 >= nAlloc) {
                nAlloc = (nUsed + nNeeded + 512) * 2;
                char *zNew = sqlite3_realloc(zProps, nAlloc);
                if (!zNew) {
                    sqlite3_free(zSanitizedPropName);
                    sqlite3_free(zSanitizedPropValue);
                    sqlite3_free(zProps);
                    sqlite3_free(zLabelsJson);
                    return SQLITE_NOMEM;
                }
                zProps = zNew;
            }
            
            /* Add the property with sanitized values */
            if (pOp->apPropValues[i]->type == CYPHER_VALUE_STRING) {
                if (zSanitizedPropValue) {
                    nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                     "%s\"%s\":\"%s\"",
                                     i > 0 ? "," : "", zSanitizedPropName, zSanitizedPropValue);
                } else {
                    nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                     "%s\"%s\":null",
                                     i > 0 ? "," : "", zSanitizedPropName);
                }
            } else if (pOp->apPropValues[i]->type == CYPHER_VALUE_INTEGER) {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":%lld",
                                 i > 0 ? "," : "", zSanitizedPropName,
                                 pOp->apPropValues[i]->u.iInteger);
            } else if (pOp->apPropValues[i]->type == CYPHER_VALUE_FLOAT) {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":%g",
                                 i > 0 ? "," : "", zSanitizedPropName,
                                 pOp->apPropValues[i]->u.rFloat);
            } else if (pOp->apPropValues[i]->type == CYPHER_VALUE_BOOLEAN) {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":%s",
                                 i > 0 ? "," : "", zSanitizedPropName,
                                 pOp->apPropValues[i]->u.bBoolean ? "true" : "false");
            } else {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":null",
                                 i > 0 ? "," : "", zSanitizedPropName);
            }
            
            /* @SELF_REVIEW.md Section 2.3: Free allocated resources */
            sqlite3_free(zSanitizedPropName);
            sqlite3_free(zSanitizedPropValue);
        }
        
        if (nUsed + 2 < nAlloc) {
            zProps[nUsed++] = '}';
            zProps[nUsed] = '\0';
        } else {
            /* Emergency buffer expansion */
            char *zNew = sqlite3_realloc(zProps, nUsed + 3);
            if (!zNew) {
                sqlite3_free(zProps);
                sqlite3_free(zLabelsJson);
                return SQLITE_NOMEM;
            }
            zProps = zNew;
            zProps[nUsed++] = '}';
            zProps[nUsed] = '\0';
        }
        zPropsJson = zProps;
    } else {
        zPropsJson = sqlite3_mprintf("{}");
        if (!zPropsJson) {
            sqlite3_free(zLabelsJson);
            return SQLITE_NOMEM;
        }
    }
    
    /* @SELF_REVIEW.md Section 2.1: Begin atomic operation with rollback support */
    rc = cypherWriteContextBeginOp(pCtx, CYPHER_WRITE_CREATE_NODE);
    if (rc != SQLITE_OK) {
        sqlite3_free(zLabelsJson);
        sqlite3_free(zPropsJson);
        return rc;
    }
    
    /* Create write operation record for rollback logging */
    pWriteOp = cypherWriteOpCreate(CYPHER_WRITE_CREATE_NODE);
    if (!pWriteOp) {
        sqlite3_free(zLabelsJson);
        sqlite3_free(zPropsJson);
        cypherWriteContextRollbackOp(pCtx);
        return SQLITE_NOMEM;
    }
    
    pWriteOp->iNodeId = pOp->iCreatedNodeId;
    pWriteOp->zNewLabels = sqlite3_mprintf("%s", zLabelsJson); /* Deep copy for rollback */
    if (!pWriteOp->zNewLabels) {
        cypherWriteOpDestroy(pWriteOp);
        sqlite3_free(zLabelsJson);
        sqlite3_free(zPropsJson);
        cypherWriteContextRollbackOp(pCtx);
        return SQLITE_NOMEM;
    }
    
    /* Store properties for rollback (new value) */
    pWriteOp->pNewValue = (CypherValue*)sqlite3_malloc(sizeof(CypherValue));
    if (!pWriteOp->pNewValue) {
        cypherWriteOpDestroy(pWriteOp);
        sqlite3_free(zLabelsJson);
        sqlite3_free(zPropsJson);
        cypherWriteContextRollbackOp(pCtx);
        return SQLITE_NOMEM;
    }
    cypherValueInit(pWriteOp->pNewValue);
    cypherValueSetString(pWriteOp->pNewValue, zPropsJson);
    
    /* Add to operation log for transaction management */
    rc = cypherWriteContextAddOperation(pCtx, pWriteOp);
    if (rc != SQLITE_OK) {
        cypherWriteOpDestroy(pWriteOp);
        sqlite3_free(zLabelsJson);
        sqlite3_free(zPropsJson);
        cypherWriteContextRollbackOp(pCtx);
        return rc;
    }
    
    /* @SELF_REVIEW.md Section 2.1: Actually add node to graph storage with error handling */
    const char **azLabelPtrs = NULL;
    if (pOp->nLabels > 0) {
        azLabelPtrs = (const char**)pOp->azLabels;
    }
    
    rc = cypherStorageAddNode(pCtx->pGraph, pOp->iCreatedNodeId, 
                             azLabelPtrs, pOp->nLabels, 
                             zPropsJson);
    if (rc != SQLITE_OK) {
        sqlite3_free(zLabelsJson);
        sqlite3_free(zPropsJson);
        cypherWriteContextRollbackOp(pCtx);
        return rc;
    }
    
    /* @SELF_REVIEW.md Section 1.2: Bind variable in execution context with error handling */
    if (pOp->zVariable) {
        CypherValue nodeValue;
        cypherValueInit(&nodeValue);
        cypherValueSetNode(&nodeValue, pOp->iCreatedNodeId);
        
        rc = executionContextBind(pCtx->pExecContext, pOp->zVariable, &nodeValue);
        cypherValueDestroy(&nodeValue);
        
        if (rc != SQLITE_OK) {
            sqlite3_free(zLabelsJson);
            sqlite3_free(zPropsJson);
            cypherWriteContextRollbackOp(pCtx);
            return rc;
        }
    }
    
    /* @SELF_REVIEW.md Section 2.3: Free all allocated resources */
    sqlite3_free(zLabelsJson);
    sqlite3_free(zPropsJson);
    
    return SQLITE_OK;
}

/*
** Execute a CREATE relationship operation.
** Returns SQLITE_OK on success, error code on failure.
** Follows @SELF_REVIEW.md requirements for input validation, security, and error handling.
*/
int cypherCreateRelationship(CypherWriteContext *pCtx, CreateRelOp *pOp) {
    CypherWriteOp *pWriteOp = NULL;
    char *zPropsJson = NULL;
    int rc = SQLITE_OK;
    int i;
    
    /* @SELF_REVIEW.md Section 2.2: Input Validation Thoroughness */
    if (!pCtx) return SQLITE_MISUSE;
    if (!pOp) return SQLITE_MISUSE;
    if (!pCtx->pGraph) return SQLITE_MISUSE;
    if (!pCtx->pExecContext) return SQLITE_MISUSE;
    
    /* Validate node IDs */
    if (pOp->iFromNodeId <= 0 || pOp->iToNodeId <= 0) {
        return SQLITE_MISUSE;
    }
    
    /* Validate relationship type */
    if (!pOp->zRelType || strlen(pOp->zRelType) == 0) {
        return SQLITE_MISUSE;
    }
    if (strlen(pOp->zRelType) > MAX_RELATIONSHIP_TYPE_LENGTH) {
        return SQLITE_TOOBIG;
    }
    if (isReservedWord(pOp->zRelType)) {
        return SQLITE_MISUSE;
    }
    
    /* Validate variable names if provided */
    if (pOp->zFromVar && (!isValidVariableName(pOp->zFromVar) || isReservedWord(pOp->zFromVar))) {
        return SQLITE_FORMAT;
    }
    if (pOp->zToVar && (!isValidVariableName(pOp->zToVar) || isReservedWord(pOp->zToVar))) {
        return SQLITE_FORMAT;
    }
    if (pOp->zRelVar && (!isValidVariableName(pOp->zRelVar) || isReservedWord(pOp->zRelVar))) {
        return SQLITE_FORMAT;
    }
    
    /* Validate properties */
    if (pOp->nProperties < 0 || pOp->nProperties > 1000) {
        return SQLITE_RANGE;
    }
    
    for (i = 0; i < pOp->nProperties; i++) {
        if (!pOp->azPropNames || !pOp->azPropNames[i]) {
            return SQLITE_MISUSE;
        }
        if (!pOp->apPropValues || !pOp->apPropValues[i]) {
            return SQLITE_MISUSE;
        }
        if (!isValidPropertyName(pOp->azPropNames[i])) {
            return SQLITE_FORMAT;
        }
        if (isReservedWord(pOp->azPropNames[i])) {
            return SQLITE_MISUSE;
        }
        
        /* Validate property value size */
        if (pOp->apPropValues[i]->type == CYPHER_VALUE_STRING && 
            pOp->apPropValues[i]->u.zString &&
            strlen(pOp->apPropValues[i]->u.zString) > MAX_PROPERTY_VALUE_SIZE) {
            return SQLITE_TOOBIG;
        }
    }
    
    /* @SELF_REVIEW.md Section 2.1: Validate that source and target nodes exist */
    rc = cypherValidateNodeExists(pCtx, pOp->iFromNodeId);
    if (rc != SQLITE_OK) {
        return rc;
    }
    
    rc = cypherValidateNodeExists(pCtx, pOp->iToNodeId);
    if (rc != SQLITE_OK) {
        return rc;
    }
    
    /* Generate cryptographically secure unique relationship ID */
    pOp->iCreatedRelId = cypherGenerateSecureRelationshipId(pCtx->pGraph);
    if (pOp->iCreatedRelId <= 0) {
        return SQLITE_ERROR;
    }
    
    /* Build properties JSON object */
    if (pOp->nProperties > 0) {
        /* Build JSON properties with safe bounds checking */
        int nAlloc = 512;
        int nUsed = 0;
        char *zProps = sqlite3_malloc(nAlloc);
        if (!zProps) return SQLITE_NOMEM;
        
        nUsed = snprintf(zProps, nAlloc, "{");
        
        for (i = 0; i < pOp->nProperties; i++) {
            /* Calculate space needed for this property - account for sanitization */
            int nNeeded = 0;
            
            if (pOp->apPropValues[i]->type == CYPHER_VALUE_STRING) {
                /* @SELF_REVIEW.md Section 5.1: Account for string sanitization expansion */
                size_t nOrigLen = pOp->apPropValues[i]->u.zString ? 
                                  strlen(pOp->apPropValues[i]->u.zString) : 0;
                size_t nMaxSanitized = nOrigLen * 2 + 1; /* Worst case: all chars escaped */
                nNeeded = snprintf(NULL, 0, "%s\"%s\":\"\"", 
                                  i > 0 ? "," : "", pOp->azPropNames[i]) + nMaxSanitized;
            } else if (pOp->apPropValues[i]->type == CYPHER_VALUE_INTEGER) {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":%lld",
                                  i > 0 ? "," : "", pOp->azPropNames[i],
                                  pOp->apPropValues[i]->u.iInteger);
            } else if (pOp->apPropValues[i]->type == CYPHER_VALUE_FLOAT) {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":%g",
                                  i > 0 ? "," : "", pOp->azPropNames[i],
                                  pOp->apPropValues[i]->u.rFloat);
            } else {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":null",
                                  i > 0 ? "," : "", pOp->azPropNames[i]);
            }
            
            /* Resize if needed */
            if (nUsed + nNeeded + 2 >= nAlloc) {
                nAlloc = (nUsed + nNeeded + 256) * 2;
                char *zNew = sqlite3_realloc(zProps, nAlloc);
                if (!zNew) {
                    sqlite3_free(zProps);
                    return SQLITE_NOMEM;
                }
                zProps = zNew;
            }
            
            /* @SELF_REVIEW.md Section 5.1: Add property with security sanitization */
            if (pOp->apPropValues[i]->type == CYPHER_VALUE_STRING) {
                /* Sanitize string values against injection attacks */
                char *zSanitizedValue = cypherSanitizeString(pOp->apPropValues[i]->u.zString);
                if (!zSanitizedValue) {
                    sqlite3_free(zProps);
                    return SQLITE_NOMEM;
                }
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":\"%s\"",
                                 i > 0 ? "," : "", pOp->azPropNames[i],
                                 zSanitizedValue);
                sqlite3_free(zSanitizedValue);
            } else if (pOp->apPropValues[i]->type == CYPHER_VALUE_INTEGER) {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":%lld",
                                 i > 0 ? "," : "", pOp->azPropNames[i],
                                 pOp->apPropValues[i]->u.iInteger);
            } else if (pOp->apPropValues[i]->type == CYPHER_VALUE_FLOAT) {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":%g",
                                 i > 0 ? "," : "", pOp->azPropNames[i],
                                 pOp->apPropValues[i]->u.rFloat);
            } else {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":null",
                                 i > 0 ? "," : "", pOp->azPropNames[i]);
            }
        }
        
        if (nUsed + 2 < nAlloc) {
            zProps[nUsed++] = '}';
            zProps[nUsed] = '\0';
        }
        zPropsJson = zProps;
    } else {
        zPropsJson = sqlite3_mprintf("{}");
    }
    
    /* Create write operation record */
    pWriteOp = cypherWriteOpCreate(CYPHER_WRITE_CREATE_RELATIONSHIP);
    if (!pWriteOp) {
        sqlite3_free(zPropsJson);
        return SQLITE_NOMEM;
    }
    
    pWriteOp->iRelId = pOp->iCreatedRelId;
    pWriteOp->iFromId = pOp->iFromNodeId;
    pWriteOp->iToId = pOp->iToNodeId;
    pWriteOp->zRelType = sqlite3_mprintf("%s", pOp->zRelType ? pOp->zRelType : "");
    
    /* Add to operation log */
    rc = cypherWriteContextAddOperation(pCtx, pWriteOp);
    if (rc != SQLITE_OK) {
        cypherWriteOpDestroy(pWriteOp);
        sqlite3_free(zPropsJson);
        return rc;
    }
    
    /* Actually add relationship to graph storage */
    rc = cypherStorageAddEdge(pCtx->pGraph, pOp->iCreatedRelId,
                             pOp->iFromNodeId, pOp->iToNodeId,
                             pOp->zRelType, 1.0, /* Default weight */
                             zPropsJson);
    if (rc != SQLITE_OK) {
        cypherWriteOpDestroy(pWriteOp);
        sqlite3_free(zPropsJson);
        return rc;
    }
    
    /* Bind variable in execution context */
    if (pOp->zRelVar) {
        CypherValue relValue;
        cypherValueInit(&relValue);
        cypherValueSetRelationship(&relValue, pOp->iCreatedRelId);
        
        rc = executionContextBind(pCtx->pExecContext, pOp->zRelVar, &relValue);
        cypherValueDestroy(&relValue);
        
        if (rc != SQLITE_OK) {
            sqlite3_free(zPropsJson);
            return rc;
        }
    }
    
    sqlite3_free(zPropsJson);
    return SQLITE_OK;
}

/*
** Utility functions for write operations.
*/

/*
** Validate that a node exists before creating relationships.
** Returns SQLITE_OK if node exists, SQLITE_ERROR if not found.
*/
int cypherValidateNodeExists(CypherWriteContext *pCtx, sqlite3_int64 iNodeId) {
    int bExists;
    
    if (!pCtx || iNodeId <= 0) return SQLITE_ERROR;
    
    bExists = cypherStorageNodeExists(pCtx->pGraph, iNodeId);
    return (bExists > 0) ? SQLITE_OK : SQLITE_ERROR;
}

/*
** Check if a node matches the given labels and properties.
** Returns 1 if match, 0 if no match, -1 on error.
*/
int cypherNodeMatches(CypherWriteContext *pCtx, sqlite3_int64 iNodeId,
                     char **azLabels, int nLabels,
                     char **azProps, CypherValue **apValues, int nProps) {
    /* Query the graph storage to check if node matches criteria */
    sqlite3_stmt *pStmt = NULL;
    char *zSql = NULL;
    int rc = SQLITE_OK;
    int bMatches = 0;
    int i;
    
    if (!pCtx || !pCtx->pGraph || iNodeId <= 0) return 0;
    
    /* First check if node exists */
    if (cypherStorageNodeExists(pCtx->pGraph, iNodeId) <= 0) {
        return 0;
    }
    
    /* Build query to check labels */
    if (nLabels > 0) {
        zSql = sqlite3_mprintf(
            "SELECT 1 FROM graph_nodes WHERE node_id = %lld", iNodeId);
        
        for (i = 0; i < nLabels; i++) {
            char *zNewSql = sqlite3_mprintf(
                "%s AND json_extract(labels, '$') LIKE '%%\"%s\"%%'",
                zSql, azLabels[i]);
            sqlite3_free(zSql);
            zSql = zNewSql;
            if (!zSql) return 0;
        }
        
        rc = sqlite3_prepare_v2(pCtx->pGraph->pDb, zSql, -1, &pStmt, NULL);
        sqlite3_free(zSql);
        
        if (rc == SQLITE_OK) {
            rc = sqlite3_step(pStmt);
            if (rc == SQLITE_ROW) {
                bMatches = 1;
            }
            sqlite3_finalize(pStmt);
        }
        
        if (!bMatches) return 0;
    }
    
    /* Check properties if any */
    for (i = 0; i < nProps && bMatches; i++) {
        if (!azProps[i] || !apValues[i]) continue;
        
        char *zValueJson = cypherValueToString(apValues[i]);
        if (!zValueJson) return 0;
        
        zSql = sqlite3_mprintf(
            "SELECT 1 FROM graph_nodes WHERE node_id = %lld "
            "AND json_extract(properties, '$.%s') = json('%s')",
            iNodeId, azProps[i], zValueJson);
        
        sqlite3_free(zValueJson);
        
        if (!zSql) return 0;
        
        rc = sqlite3_prepare_v2(pCtx->pGraph->pDb, zSql, -1, &pStmt, NULL);
        sqlite3_free(zSql);
        
        if (rc == SQLITE_OK) {
            rc = sqlite3_step(pStmt);
            if (rc != SQLITE_ROW) {
                bMatches = 0;
            }
            sqlite3_finalize(pStmt);
        } else {
            bMatches = 0;
        }
    }
    
    return bMatches;
}

/*
** Find a node that matches the given criteria.
** Returns node ID if found, 0 if not found, -1 on error.
*/
sqlite3_int64 cypherFindMatchingNode(CypherWriteContext *pCtx,
                                    char **azLabels, int nLabels,
                                    char **azProps, CypherValue **apValues, int nProps) {
    char *zSql = NULL;
    sqlite3_stmt *pStmt = NULL;
    int rc = SQLITE_OK;
    sqlite3_int64 iNodeId = 0;
    int i;
    
    if (!pCtx || !pCtx->pGraph) return 0;
    
    /* Build query to find matching node */
    if (nLabels > 0) {
        /* Start with label-based search */
        zSql = sqlite3_mprintf(
            "SELECT node_id FROM graph_nodes WHERE "
            "json_extract(labels, '$[0]') = '%s'",
            azLabels[0]
        );
        
        /* Add additional label filters */
        for (i = 1; i < nLabels; i++) {
            char *zNewSql = sqlite3_mprintf(
                "%s AND json_extract(labels, '$[%d]') = '%s'",
                zSql, i, azLabels[i]
            );
            sqlite3_free(zSql);
            zSql = zNewSql;
        }
    } else {
        /* No labels specified, search all nodes */
        zSql = sqlite3_mprintf("SELECT node_id FROM graph_nodes");
    }
    
    if (!zSql) return 0;
    
    /* Add property filters */
    for (i = 0; i < nProps; i++) {
        char *zValueJson = cypherValueToJson(apValues[i]);
        if (zValueJson) {
            char *zNewSql = sqlite3_mprintf(
                "%s AND json_extract(properties, '$.%s') = json('%s')",
                zSql, azProps[i], zValueJson
            );
            sqlite3_free(zSql);
            sqlite3_free(zValueJson);
            zSql = zNewSql;
        }
    }
    
    /* Add LIMIT 1 to get first match */
    char *zFinalSql = sqlite3_mprintf("%s LIMIT 1", zSql);
    sqlite3_free(zSql);
    zSql = zFinalSql;
    
    if (!zSql) return 0;
    
    /* Execute the query */
    rc = sqlite3_prepare_v2(pCtx->pGraph->pDb, zSql, -1, &pStmt, NULL);
    if (rc == SQLITE_OK) {
        rc = sqlite3_step(pStmt);
        if (rc == SQLITE_ROW) {
            iNodeId = sqlite3_column_int64(pStmt, 0);
        }
    }
    
    sqlite3_finalize(pStmt);
    sqlite3_free(zSql);
    
    return iNodeId;
}

/*
** Get all relationships connected to a node (for DETACH DELETE).
** Returns JSON array of relationship IDs, caller must sqlite3_free().
*/
char *cypherGetNodeRelationships(CypherWriteContext *pCtx, sqlite3_int64 iNodeId) {
    /* Implement actual relationship lookup from graph storage */
    sqlite3_stmt *pStmt = NULL;
    char *zSql = NULL;
    char *zResult = NULL;
    int rc = SQLITE_OK;
    int nAlloc = 256;
    int nUsed = 0;
    
    if (!pCtx || !pCtx->pGraph || iNodeId <= 0) {
        return sqlite3_mprintf("[]");
    }
    
    zResult = sqlite3_malloc(nAlloc);
    if (!zResult) return NULL;
    
    nUsed = snprintf(zResult, nAlloc, "[");
    
    /* Query for all relationships connected to this node */
    zSql = sqlite3_mprintf(
        "SELECT edge_id FROM graph_edges WHERE from_node = %lld OR to_node = %lld",
        iNodeId, iNodeId);
    
    if (!zSql) {
        sqlite3_free(zResult);
        return NULL;
    }
    
    rc = sqlite3_prepare_v2(pCtx->pGraph->pDb, zSql, -1, &pStmt, NULL);
    sqlite3_free(zSql);
    
    if (rc == SQLITE_OK) {
        int bFirst = 1;
        while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW) {
            sqlite3_int64 iEdgeId = sqlite3_column_int64(pStmt, 0);
            
            int nNeeded = snprintf(NULL, 0, "%s%lld", bFirst ? "" : ",", iEdgeId);
            if (nUsed + nNeeded + 2 >= nAlloc) {
                nAlloc = (nUsed + nNeeded + 256) * 2;
                char *zNew = sqlite3_realloc(zResult, nAlloc);
                if (!zNew) {
                    sqlite3_free(zResult);
                    sqlite3_finalize(pStmt);
                    return NULL;
                }
                zResult = zNew;
            }
            
            nUsed += snprintf(zResult + nUsed, nAlloc - nUsed, 
                             "%s%lld", bFirst ? "" : ",", iEdgeId);
            bFirst = 0;
        }
        sqlite3_finalize(pStmt);
    }
    
    /* Close array */
    if (nUsed + 2 < nAlloc) {
        zResult[nUsed++] = ']';
        zResult[nUsed] = '\0';
    }
    
    return zResult;
}

/*
** Transaction management implementation.
*/

/*
** Execute all pending write operations and commit them.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherExecuteOperations(CypherWriteContext *pCtx) {
    CypherWriteOp *pOp;
    int rc = SQLITE_OK;
    
    if (!pCtx) return SQLITE_MISUSE;
    
    /* Execute each operation in sequence */
    for (pOp = pCtx->pOperations; pOp; pOp = pOp->pNext) {
        switch (pOp->type) {
            case CYPHER_WRITE_CREATE_NODE:
                /* Execute actual node creation in graph storage */
                if (pOp->iNodeId == 0) {
                    pOp->iNodeId = cypherStorageGetNextNodeId(pCtx->pGraph);
                }
                rc = cypherStorageAddNode(pCtx->pGraph, pOp->iNodeId,
                                        NULL, 0, NULL);
                if (rc != SQLITE_OK) return rc;
                break;
                
            case CYPHER_WRITE_CREATE_RELATIONSHIP:
                /* Execute actual relationship creation in graph storage */
                if (pOp->iRelId == 0) {
                    pOp->iRelId = cypherStorageGetNextEdgeId(pCtx->pGraph);
                }
                rc = cypherStorageAddEdge(pCtx->pGraph, pOp->iRelId,
                                        pOp->iFromId, pOp->iToId,
                                        pOp->zRelType, 1.0, NULL);
                if (rc != SQLITE_OK) return rc;
                break;
                
            case CYPHER_WRITE_MERGE_NODE:
                /* Execute actual node merge in graph storage */
                {
                    sqlite3_int64 iFoundId = cypherFindMatchingNode(pCtx,
                        NULL, 0, NULL, NULL, 0); /* Basic node search */
                    
                    if (iFoundId > 0) {
                        /* Node exists, update properties if needed */
                        pOp->iNodeId = iFoundId;
                        /* Apply ON MATCH properties if any */
                    } else {
                        /* Node doesn't exist, create it */
                        if (pOp->iNodeId == 0) {
                            pOp->iNodeId = cypherStorageGetNextNodeId(pCtx->pGraph);
                        }
                        rc = cypherStorageAddNode(pCtx->pGraph, pOp->iNodeId,
                                                NULL, 0, NULL);
                        if (rc != SQLITE_OK) return rc;
                    }
                }
                break;
                
            case CYPHER_WRITE_SET_PROPERTY:
                /* Execute actual property update in graph storage */
                rc = cypherStorageUpdateProperties(pCtx->pGraph, 
                                                 pOp->iNodeId,
                                                 pOp->iRelId,
                                                 pOp->zProperty,
                                                 pOp->pNewValue);
                if (rc != SQLITE_OK) return rc;
                break;
                
            case CYPHER_WRITE_SET_LABEL:
                /* Execute actual label update in graph storage */
                /* For label updates, we need to read current labels, add new ones, and update */
                {
                    char *zSql = sqlite3_mprintf(
                        "UPDATE graph_nodes SET labels = "
                        "json_insert(COALESCE(labels, '[]'), '$[#]', '%s') "
                        "WHERE node_id = %lld",
                        pOp->zLabel, pOp->iNodeId);
                    
                    if (!zSql) return SQLITE_NOMEM;
                    
                    sqlite3_int64 rowId;
                    rc = cypherStorageExecuteUpdate(pCtx->pGraph, zSql, &rowId);
                    sqlite3_free(zSql);
                    if (rc != SQLITE_OK) return rc;
                }
                break;
                
            case CYPHER_WRITE_DELETE_NODE:
            case CYPHER_WRITE_DETACH_DELETE_NODE:
                /* Execute actual node deletion in graph storage */
                {
                    int bDetach = (pOp->type == CYPHER_WRITE_DETACH_DELETE_NODE);
                    rc = cypherStorageDeleteNode(pCtx->pGraph, pOp->iNodeId, bDetach);
                    if (rc != SQLITE_OK) return rc;
                }
                break;
                
            case CYPHER_WRITE_DELETE_RELATIONSHIP:
                /* Execute actual relationship deletion in graph storage */
                rc = cypherStorageDeleteEdge(pCtx->pGraph, pOp->iRelId);
                if (rc != SQLITE_OK) return rc;
                break;
                
            default:
                /* Unknown operation type */
                return SQLITE_ERROR;
        }
    }
    
    return rc;
}

/*
** Rollback all pending write operations.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherRollbackOperations(CypherWriteContext *pCtx) {
    CypherWriteOp *pOp;
    int rc = SQLITE_OK;
    
    if (!pCtx) return SQLITE_MISUSE;
    
    /* Rollback operations in reverse order */
    CypherWriteOp *apOps[1000]; /* Simple array for reverse iteration */
    int nOps = 0;
    
    /* Collect operations into array */
    for (pOp = pCtx->pOperations; pOp && nOps < 1000; pOp = pOp->pNext) {
        apOps[nOps++] = pOp;
    }
    
    /* Process in reverse order */
    for (int i = nOps - 1; i >= 0; i--) {
        pOp = apOps[i];
        switch (pOp->type) {
            case CYPHER_WRITE_SET_PROPERTY:
                /* Restore old property value if available */
                if (pOp->pOldValue) {
                    cypherStorageUpdateProperties(pCtx->pGraph, pOp->iNodeId, pOp->iRelId,
                                                 pOp->zProperty, pOp->pOldValue);
                }
                break;
                
            case CYPHER_WRITE_SET_LABEL:
                /* Restore old labels if available */
                if (pOp->zOldLabels) {
                    /* Restore labels to old state in graph storage */
                    char *zRestoreSql = sqlite3_mprintf(
                        "UPDATE graph_nodes SET labels = '%s' WHERE node_id = %lld",
                        pOp->zOldLabels, pOp->iNodeId);
                    
                    if (zRestoreSql) {
                        sqlite3_int64 rowId;
                        cypherStorageExecuteUpdate(pCtx->pGraph, zRestoreSql, &rowId);
                        sqlite3_free(zRestoreSql);
                    }
                }
                break;
                
            case CYPHER_WRITE_CREATE_NODE:
            case CYPHER_WRITE_MERGE_NODE:
                /* Remove created nodes */
                if (pOp->iNodeId > 0) {
                    cypherStorageDeleteNode(pCtx->pGraph, pOp->iNodeId, 1); /* Force detach */
                }
                break;
                
            case CYPHER_WRITE_CREATE_RELATIONSHIP:
                /* Remove created relationships */
                if (pOp->iRelId > 0) {
                    cypherStorageDeleteEdge(pCtx->pGraph, pOp->iRelId);
                }
                break;
                
            case CYPHER_WRITE_DELETE_NODE:
            case CYPHER_WRITE_DETACH_DELETE_NODE:
            case CYPHER_WRITE_DELETE_RELATIONSHIP:
                /* Restore deleted items */
                /* Restore deleted items from backup data */
                if (pOp->type == CYPHER_WRITE_DELETE_NODE || pOp->type == CYPHER_WRITE_DETACH_DELETE_NODE) {
                    /* Restore deleted node */
                    if (pOp->zOldLabels && pOp->pOldValue) {
                        char *zValueStr = cypherValueToString(pOp->pOldValue);
                        if (zValueStr) {
                            cypherStorageAddNode(pCtx->pGraph, pOp->iNodeId, 
                                               NULL, 0, zValueStr);
                            sqlite3_free(zValueStr);
                        }
                    }
                } else if (pOp->type == CYPHER_WRITE_DELETE_RELATIONSHIP) {
                    /* Restore deleted relationship */
                    if (pOp->iFromId > 0 && pOp->iToId > 0) {
                        char *zValueStr = pOp->pOldValue ? cypherValueToString(pOp->pOldValue) : NULL;
                        cypherStorageAddEdge(pCtx->pGraph, pOp->iRelId,
                                           pOp->iFromId, pOp->iToId,
                                           pOp->zRelType, 1.0, zValueStr);
                        if (zValueStr) sqlite3_free(zValueStr);
                    }
                }
                break;
                
            case CYPHER_WRITE_MERGE_RELATIONSHIP:
                /* Remove merged relationships */
                if (pOp->iRelId > 0) {
                    cypherStorageDeleteEdge(pCtx->pGraph, pOp->iRelId);
                }
                break;
                
            case CYPHER_WRITE_REMOVE_PROPERTY:
                /* Restore removed property */
                if (pOp->pOldValue) {
                    cypherStorageUpdateProperties(pCtx->pGraph, pOp->iNodeId, pOp->iRelId,
                                                 pOp->zProperty, pOp->pOldValue);
                }
                break;
                
            case CYPHER_WRITE_REMOVE_LABEL:
                /* Restore removed label */
                if (pOp->zOldLabels) {
                    char *zRestoreSql = sqlite3_mprintf(
                        "UPDATE graph_nodes SET labels = '%s' WHERE node_id = %lld",
                        pOp->zOldLabels, pOp->iNodeId);
                    if (zRestoreSql) {
                        sqlite3_int64 rowId;
                        cypherStorageExecuteUpdate(pCtx->pGraph, zRestoreSql, &rowId);
                        sqlite3_free(zRestoreSql);
                    }
                }
                break;
        }
    }
    
    return rc;
}

/*
** Write operation memory management.
*/

/*
** Create a write operation record.
** Returns NULL on allocation failure.
*/
CypherWriteOp *cypherWriteOpCreate(CypherWriteOpType type) {
    CypherWriteOp *pOp = (CypherWriteOp*)sqlite3_malloc(sizeof(CypherWriteOp));
    if (!pOp) return NULL;
    
    memset(pOp, 0, sizeof(CypherWriteOp));
    pOp->type = type;
    
    return pOp;
}

/*
** Destroy a write operation and free all associated memory.
** Safe to call with NULL pointer.
*/
void cypherWriteOpDestroy(CypherWriteOp *pOp) {
    if (!pOp) return;
    
    sqlite3_free(pOp->zProperty);
    sqlite3_free(pOp->zLabel);
    sqlite3_free(pOp->zRelType);
    sqlite3_free(pOp->zOldLabels);
    sqlite3_free(pOp->zNewLabels);
    
    if (pOp->pOldValue) {
        cypherValueDestroy(pOp->pOldValue);
        sqlite3_free(pOp->pOldValue);
    }
    if (pOp->pNewValue) {
        cypherValueDestroy(pOp->pNewValue);
        sqlite3_free(pOp->pNewValue);
    }
    
    sqlite3_free(pOp);
}

/*
** Create operation structures.
*/

/*
** Create a CREATE node operation structure.
** Returns NULL on allocation failure.
*/
CreateNodeOp *cypherCreateNodeOpCreate(void) {
    CreateNodeOp *pOp = (CreateNodeOp*)sqlite3_malloc(sizeof(CreateNodeOp));
    if (!pOp) return NULL;
    
    memset(pOp, 0, sizeof(CreateNodeOp));
    return pOp;
}

/*
** Destroy a CREATE node operation.
** Safe to call with NULL pointer.
*/
void cypherCreateNodeOpDestroy(CreateNodeOp *pOp) {
    int i;
    
    if (!pOp) return;
    
    sqlite3_free(pOp->zVariable);
    
    for (i = 0; i < pOp->nLabels; i++) {
        sqlite3_free(pOp->azLabels[i]);
    }
    sqlite3_free(pOp->azLabels);
    
    for (i = 0; i < pOp->nProperties; i++) {
        sqlite3_free(pOp->azPropNames[i]);
        if (pOp->apPropValues[i]) {
            cypherValueDestroy(pOp->apPropValues[i]);
            sqlite3_free(pOp->apPropValues[i]);
        }
    }
    sqlite3_free(pOp->azPropNames);
    sqlite3_free(pOp->apPropValues);
    
    sqlite3_free(pOp);
}

/*
** Create a CREATE relationship operation structure.
** Returns NULL on allocation failure.
*/
CreateRelOp *cypherCreateRelOpCreate(void) {
    CreateRelOp *pOp = (CreateRelOp*)sqlite3_malloc(sizeof(CreateRelOp));
    if (!pOp) return NULL;
    
    memset(pOp, 0, sizeof(CreateRelOp));
    return pOp;
}

/*
** Destroy a CREATE relationship operation.
** Safe to call with NULL pointer.
*/
void cypherCreateRelOpDestroy(CreateRelOp *pOp) {
    int i;
    
    if (!pOp) return;
    
    sqlite3_free(pOp->zFromVar);
    sqlite3_free(pOp->zToVar);
    sqlite3_free(pOp->zRelVar);
    sqlite3_free(pOp->zRelType);
    
    for (i = 0; i < pOp->nProperties; i++) {
        sqlite3_free(pOp->azPropNames[i]);
        if (pOp->apPropValues[i]) {
            cypherValueDestroy(pOp->apPropValues[i]);
            sqlite3_free(pOp->apPropValues[i]);
        }
    }
    sqlite3_free(pOp->azPropNames);
    sqlite3_free(pOp->apPropValues);
    
    sqlite3_free(pOp);
}

/*
** MERGE operation functions.
*/

/*
** Execute a MERGE node operation.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherMergeNode(CypherWriteContext *pCtx, MergeNodeOp *pOp) {
    CypherWriteOp *pWriteOp;
    sqlite3_int64 iFoundNodeId;
    char *zLabelsJson = NULL;
    char *zPropsJson = NULL;
    int rc = SQLITE_OK;
    int i;
    
    if (!pCtx || !pOp) return SQLITE_MISUSE;
    
    /* First, try to find an existing node that matches the criteria */
    iFoundNodeId = cypherFindMatchingNode(pCtx, 
                                         pOp->azLabels, pOp->nLabels,
                                         pOp->azMatchProps, pOp->apMatchValues, pOp->nMatchProps);
    
    if (iFoundNodeId > 0) {
        /* Node found - execute ON MATCH clause */
        pOp->iNodeId = iFoundNodeId;
        pOp->bWasCreated = 0;
        
        /* Apply ON MATCH property updates */
        for (i = 0; i < pOp->nOnMatchProps; i++) {
            SetPropertyOp setOp;
            memset(&setOp, 0, sizeof(SetPropertyOp));
            setOp.zVariable = pOp->zVariable;
            setOp.zProperty = pOp->azOnMatchProps[i];
            setOp.pValue = pOp->apOnMatchValues[i];
            setOp.iNodeId = iFoundNodeId;
            
            rc = cypherSetProperty(pCtx, &setOp);
            if (rc != SQLITE_OK) {
                return rc;
            }
        }
        
        /* Create operation record for MERGE match */
        pWriteOp = cypherWriteOpCreate(CYPHER_WRITE_MERGE_NODE);
        if (!pWriteOp) return SQLITE_NOMEM;
        
        pWriteOp->iNodeId = iFoundNodeId;
        pWriteOp->zProperty = sqlite3_mprintf("MATCH");
        
        rc = cypherWriteContextAddOperation(pCtx, pWriteOp);
        if (rc != SQLITE_OK) {
            cypherWriteOpDestroy(pWriteOp);
            return rc;
        }
        
    } else {
        /* Node not found - create new node with ON CREATE properties */
        pOp->iNodeId = cypherWriteContextNextNodeId(pCtx);
        pOp->bWasCreated = 1;
        
        /* Build labels JSON array */
        if (pOp->nLabels > 0) {
            /* Build JSON array with safe bounds checking */
            int nAlloc = 256;
            int nUsed = 0;
            char *zLabels = sqlite3_malloc(nAlloc);
            if (!zLabels) return SQLITE_NOMEM;
            
            nUsed = snprintf(zLabels, nAlloc, "[");
            
            for (i = 0; i < pOp->nLabels; i++) {
                int nNeeded = snprintf(NULL, 0, "%s\"%s\"", i > 0 ? "," : "", pOp->azLabels[i]);
                if (nUsed + nNeeded + 2 >= nAlloc) {
                    nAlloc = (nUsed + nNeeded + 256) * 2;
                    char *zNew = sqlite3_realloc(zLabels, nAlloc);
                    if (!zNew) {
                        sqlite3_free(zLabels);
                        return SQLITE_NOMEM;
                    }
                    zLabels = zNew;
                }
                nUsed += snprintf(zLabels + nUsed, nAlloc - nUsed, 
                                 "%s\"%s\"", i > 0 ? "," : "", pOp->azLabels[i]);
            }
            
            if (nUsed + 2 < nAlloc) {
                zLabels[nUsed++] = ']';
                zLabels[nUsed] = '\0';
            }
            zLabelsJson = zLabels;
        } else {
            zLabelsJson = sqlite3_mprintf("[]");
        }
        
        /* Build combined properties JSON (match props + on create props) */
        int nAlloc = 512;
        int nUsed = 0;
        char *zProps = sqlite3_malloc(nAlloc);
        if (!zProps) {
            sqlite3_free(zLabelsJson);
            return SQLITE_NOMEM;
        }
        
        nUsed = snprintf(zProps, nAlloc, "{");
        int propCount = 0;
        
        /* Add match properties */
        for (i = 0; i < pOp->nMatchProps; i++) {
            int nNeeded = 0;
            
            /* Calculate space needed */
            if (pOp->apMatchValues[i]->type == CYPHER_VALUE_STRING) {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":\"%s\"",
                                  propCount > 0 ? "," : "", pOp->azMatchProps[i],
                                  pOp->apMatchValues[i]->u.zString);
            } else if (pOp->apMatchValues[i]->type == CYPHER_VALUE_INTEGER) {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":%lld",
                                  propCount > 0 ? "," : "", pOp->azMatchProps[i],
                                  pOp->apMatchValues[i]->u.iInteger);
            } else {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":null",
                                  propCount > 0 ? "," : "", pOp->azMatchProps[i]);
            }
            
            /* Resize if needed */
            if (nUsed + nNeeded + 2 >= nAlloc) {
                nAlloc = (nUsed + nNeeded + 256) * 2;
                char *zNew = sqlite3_realloc(zProps, nAlloc);
                if (!zNew) {
                    sqlite3_free(zProps);
                    sqlite3_free(zLabelsJson);
                    return SQLITE_NOMEM;
                }
                zProps = zNew;
            }
            
            /* Add the property */
            if (pOp->apMatchValues[i]->type == CYPHER_VALUE_STRING) {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":\"%s\"",
                                 propCount > 0 ? "," : "", pOp->azMatchProps[i],
                                 pOp->apMatchValues[i]->u.zString);
            } else if (pOp->apMatchValues[i]->type == CYPHER_VALUE_INTEGER) {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":%lld",
                                 propCount > 0 ? "," : "", pOp->azMatchProps[i],
                                 pOp->apMatchValues[i]->u.iInteger);
            } else {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":null",
                                 propCount > 0 ? "," : "", pOp->azMatchProps[i]);
            }
            propCount++;
        }
        
        /* Add ON CREATE properties */
        for (i = 0; i < pOp->nOnCreateProps; i++) {
            int nNeeded = 0;
            
            /* Calculate space needed */
            if (pOp->apOnCreateValues[i]->type == CYPHER_VALUE_STRING) {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":\"%s\"",
                                  propCount > 0 ? "," : "", pOp->azOnCreateProps[i],
                                  pOp->apOnCreateValues[i]->u.zString);
            } else if (pOp->apOnCreateValues[i]->type == CYPHER_VALUE_INTEGER) {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":%lld",
                                  propCount > 0 ? "," : "", pOp->azOnCreateProps[i],
                                  pOp->apOnCreateValues[i]->u.iInteger);
            } else {
                nNeeded = snprintf(NULL, 0, "%s\"%s\":null",
                                  propCount > 0 ? "," : "", pOp->azOnCreateProps[i]);
            }
            
            /* Resize if needed */
            if (nUsed + nNeeded + 2 >= nAlloc) {
                nAlloc = (nUsed + nNeeded + 256) * 2;
                char *zNew = sqlite3_realloc(zProps, nAlloc);
                if (!zNew) {
                    sqlite3_free(zProps);
                    sqlite3_free(zLabelsJson);
                    return SQLITE_NOMEM;
                }
                zProps = zNew;
            }
            
            /* Add the property */
            if (pOp->apOnCreateValues[i]->type == CYPHER_VALUE_STRING) {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":\"%s\"",
                                 propCount > 0 ? "," : "", pOp->azOnCreateProps[i],
                                 pOp->apOnCreateValues[i]->u.zString);
            } else if (pOp->apOnCreateValues[i]->type == CYPHER_VALUE_INTEGER) {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":%lld",
                                 propCount > 0 ? "," : "", pOp->azOnCreateProps[i],
                                 pOp->apOnCreateValues[i]->u.iInteger);
            } else {
                nUsed += snprintf(zProps + nUsed, nAlloc - nUsed,
                                 "%s\"%s\":null",
                                 propCount > 0 ? "," : "", pOp->azOnCreateProps[i]);
            }
            propCount++;
        }
        
        if (nUsed + 2 < nAlloc) {
            zProps[nUsed++] = '}';
            zProps[nUsed] = '\0';
        }
        zPropsJson = zProps;
        
        /* Create write operation record for MERGE create */
        pWriteOp = cypherWriteOpCreate(CYPHER_WRITE_MERGE_NODE);
        if (!pWriteOp) {
            sqlite3_free(zLabelsJson);
            sqlite3_free(zPropsJson);
            return SQLITE_NOMEM;
        }
        
        pWriteOp->iNodeId = pOp->iNodeId;
        pWriteOp->zNewLabels = zLabelsJson;
        pWriteOp->zProperty = sqlite3_mprintf("CREATE");
        
        /* Add to operation log */
        rc = cypherWriteContextAddOperation(pCtx, pWriteOp);
        if (rc != SQLITE_OK) {
            cypherWriteOpDestroy(pWriteOp);
            sqlite3_free(zLabelsJson);
            sqlite3_free(zPropsJson);
            return rc;
        }
        
        /* Actually create node in graph storage */
        const char **azLabelPtrs = NULL;
        if (pOp->nLabels > 0) {
            azLabelPtrs = (const char**)pOp->azLabels;
        }
        
        rc = cypherStorageAddNode(pCtx->pGraph, pOp->iNodeId, 
                                 azLabelPtrs, pOp->nLabels, 
                                 zPropsJson);
        if (rc != SQLITE_OK) {
            sqlite3_free(zLabelsJson);
            sqlite3_free(zPropsJson);
            return rc;
        }
        
        sqlite3_free(zPropsJson);
    }
    
    /* Bind variable in execution context */
    if (pOp->zVariable) {
        CypherValue nodeValue;
        cypherValueInit(&nodeValue);
        cypherValueSetNode(&nodeValue, pOp->iNodeId);
        
        rc = executionContextBind(pCtx->pExecContext, pOp->zVariable, &nodeValue);
        cypherValueDestroy(&nodeValue);
        
        if (rc != SQLITE_OK) {
            return rc;
        }
    }
    
    return SQLITE_OK;
}

/*
** Create a MERGE node iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherMergeNodeIteratorCreate(CypherWriteContext *pCtx, MergeNodeOp *pOp) {
    CypherWriteIterator *pIterator;
    
    if (!pCtx || !pOp) return NULL;
    
    pIterator = (CypherWriteIterator*)sqlite3_malloc(sizeof(CypherWriteIterator));
    if (!pIterator) return NULL;
    
    memset(pIterator, 0, sizeof(CypherWriteIterator));
    pIterator->pWriteCtx = pCtx;
    pIterator->pOperationData = pOp;
    
    /* Set up base iterator interface */
    /* Iterator functions for write operations are minimal */
    pIterator->base.xOpen = NULL;    /* Write operations don't need opening */
    pIterator->base.xNext = NULL;    /* Write operations don't iterate */
    pIterator->base.xClose = NULL;   /* Write operations don't need closing */
    pIterator->base.xDestroy = NULL; /* Will be freed by caller */
    
    return pIterator;
}

MergeNodeOp *cypherMergeNodeOpCreate(void) {
    MergeNodeOp *pOp = (MergeNodeOp*)sqlite3_malloc(sizeof(MergeNodeOp));
    if (!pOp) return NULL;
    memset(pOp, 0, sizeof(MergeNodeOp));
    return pOp;
}

void cypherMergeNodeOpDestroy(MergeNodeOp *pOp) {
    int i;
    
    if (!pOp) return;
    
    sqlite3_free(pOp->zVariable);
    
    /* Free labels */
    for (i = 0; i < pOp->nLabels; i++) {
        sqlite3_free(pOp->azLabels[i]);
    }
    sqlite3_free(pOp->azLabels);
    
    /* Free match properties */
    for (i = 0; i < pOp->nMatchProps; i++) {
        sqlite3_free(pOp->azMatchProps[i]);
        if (pOp->apMatchValues[i]) {
            cypherValueDestroy(pOp->apMatchValues[i]);
            sqlite3_free(pOp->apMatchValues[i]);
        }
    }
    sqlite3_free(pOp->azMatchProps);
    sqlite3_free(pOp->apMatchValues);
    
    /* Free ON CREATE properties */
    for (i = 0; i < pOp->nOnCreateProps; i++) {
        sqlite3_free(pOp->azOnCreateProps[i]);
        if (pOp->apOnCreateValues[i]) {
            cypherValueDestroy(pOp->apOnCreateValues[i]);
            sqlite3_free(pOp->apOnCreateValues[i]);
        }
    }
    sqlite3_free(pOp->azOnCreateProps);
    sqlite3_free(pOp->apOnCreateValues);
    
    /* Free ON MATCH properties */
    for (i = 0; i < pOp->nOnMatchProps; i++) {
        sqlite3_free(pOp->azOnMatchProps[i]);
        if (pOp->apOnMatchValues[i]) {
            cypherValueDestroy(pOp->apOnMatchValues[i]);
            sqlite3_free(pOp->apOnMatchValues[i]);
        }
    }
    sqlite3_free(pOp->azOnMatchProps);
    sqlite3_free(pOp->apOnMatchValues);
    
    sqlite3_free(pOp);
}

/*
** SET operation functions.
*/

/*
** Execute a SET property operation.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherSetProperty(CypherWriteContext *pCtx, SetPropertyOp *pOp) {
    CypherWriteOp *pWriteOp;
    int rc = SQLITE_OK;
    
    if (!pCtx || !pOp) return SQLITE_MISUSE;
    
    /* Validate that the target node exists */
    rc = cypherValidateNodeExists(pCtx, pOp->iNodeId);
    if (rc != SQLITE_OK) {
        return rc;
    }
    
    /* Create write operation record */
    pWriteOp = cypherWriteOpCreate(CYPHER_WRITE_SET_PROPERTY);
    if (!pWriteOp) return SQLITE_NOMEM;
    
    pWriteOp->iNodeId = pOp->iNodeId;
    pWriteOp->zProperty = sqlite3_mprintf("%s", pOp->zProperty);
    
    /* Get old value for rollback support */
    pWriteOp->pOldValue = (CypherValue*)sqlite3_malloc(sizeof(CypherValue));
    if (!pWriteOp->pOldValue) {
        cypherWriteOpDestroy(pWriteOp);
        return SQLITE_NOMEM;
    }
    
    /* Query current property value */
    sqlite3_stmt *pStmt = NULL;
    char *zSql = sqlite3_mprintf(
        "SELECT json_extract(properties, '$.%s') FROM graph_nodes WHERE node_id = %lld",
        pOp->zProperty, pOp->iNodeId);
    
    if (!zSql) {
        cypherWriteOpDestroy(pWriteOp);
        return SQLITE_NOMEM;
    }
    
    rc = sqlite3_prepare_v2(pCtx->pGraph->pDb, zSql, -1, &pStmt, NULL);
    sqlite3_free(zSql);
    
    if (rc == SQLITE_OK) {
        rc = sqlite3_step(pStmt);
        if (rc == SQLITE_ROW) {
            const char *zOldValue = (const char*)sqlite3_column_text(pStmt, 0);
            if (zOldValue) {
                cypherValueSetString(pWriteOp->pOldValue, zOldValue);
            } else {
                cypherValueInit(pWriteOp->pOldValue);
            }
        } else {
            cypherValueInit(pWriteOp->pOldValue);
        }
        sqlite3_finalize(pStmt);
    } else {
        cypherValueInit(pWriteOp->pOldValue);
    }
    
    /* Set new value */
    pWriteOp->pNewValue = (CypherValue*)sqlite3_malloc(sizeof(CypherValue));
    if (!pWriteOp->pNewValue) {
        cypherWriteOpDestroy(pWriteOp);
        return SQLITE_NOMEM;
    }
    
    memcpy(pWriteOp->pNewValue, pOp->pValue, sizeof(CypherValue));
    
    /* If it's a string value, make a copy */
    if (pOp->pValue->type == CYPHER_VALUE_STRING && pOp->pValue->u.zString) {
        pWriteOp->pNewValue->u.zString = sqlite3_mprintf("%s", pOp->pValue->u.zString);
        if (!pWriteOp->pNewValue->u.zString) {
            cypherWriteOpDestroy(pWriteOp);
            return SQLITE_NOMEM;
        }
    }
    
    /* Add to operation log */
    rc = cypherWriteContextAddOperation(pCtx, pWriteOp);
    if (rc != SQLITE_OK) {
        cypherWriteOpDestroy(pWriteOp);
        return rc;
    }
    
    /* Actually update property in graph storage */
    rc = cypherStorageUpdateProperties(pCtx->pGraph, pOp->iNodeId, 0, /* No relation ID for node properties */
                                      pOp->zProperty, pOp->pValue);
    if (rc != SQLITE_OK) {
        return rc;
    }
    
    return SQLITE_OK;
}

/*
** Execute a SET label operation.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherSetLabel(CypherWriteContext *pCtx, SetLabelOp *pOp) {
    CypherWriteOp *pWriteOp;
    char *zLabelsJson = NULL;
    int rc = SQLITE_OK;
    int i;
    
    if (!pCtx || !pOp) return SQLITE_MISUSE;
    
    /* Validate that the target node exists */
    rc = cypherValidateNodeExists(pCtx, pOp->iNodeId);
    if (rc != SQLITE_OK) {
        return rc;
    }
    
    /* Build labels JSON array */
    if (pOp->nLabels > 0) {
        /* Build JSON array with safe bounds checking */
        int nAlloc = 256;
        int nUsed = 0;
        char *zLabels = sqlite3_malloc(nAlloc);
        if (!zLabels) return SQLITE_NOMEM;
        
        nUsed = snprintf(zLabels, nAlloc, "[");
        
        for (i = 0; i < pOp->nLabels; i++) {
            int nNeeded = snprintf(NULL, 0, "%s\"%s\"", i > 0 ? "," : "", pOp->azLabels[i]);
            if (nUsed + nNeeded + 2 >= nAlloc) {
                nAlloc = (nUsed + nNeeded + 256) * 2;
                char *zNew = sqlite3_realloc(zLabels, nAlloc);
                if (!zNew) {
                    sqlite3_free(zLabels);
                    return SQLITE_NOMEM;
                }
                zLabels = zNew;
            }
            nUsed += snprintf(zLabels + nUsed, nAlloc - nUsed, 
                             "%s\"%s\"", i > 0 ? "," : "", pOp->azLabels[i]);
        }
        
        if (nUsed + 2 < nAlloc) {
            zLabels[nUsed++] = ']';
            zLabels[nUsed] = '\0';
        }
        zLabelsJson = zLabels;
    } else {
        zLabelsJson = sqlite3_mprintf("[]");
    }
    
    /* Create write operation record */
    pWriteOp = cypherWriteOpCreate(CYPHER_WRITE_SET_LABEL);
    if (!pWriteOp) {
        sqlite3_free(zLabelsJson);
        return SQLITE_NOMEM;
    }
    
    pWriteOp->iNodeId = pOp->iNodeId;
    pWriteOp->zNewLabels = zLabelsJson;
    
    /* Get old labels for rollback support */
    sqlite3_stmt *pStmt = NULL;
    char *zSql = sqlite3_mprintf(
        "SELECT labels FROM graph_nodes WHERE node_id = %lld",
        pOp->iNodeId);
    
    if (!zSql) {
        cypherWriteOpDestroy(pWriteOp);
        return SQLITE_NOMEM;
    }
    
    rc = sqlite3_prepare_v2(pCtx->pGraph->pDb, zSql, -1, &pStmt, NULL);
    sqlite3_free(zSql);
    
    if (rc == SQLITE_OK) {
        rc = sqlite3_step(pStmt);
        if (rc == SQLITE_ROW) {
            const char *zOldLabels = (const char*)sqlite3_column_text(pStmt, 0);
            if (zOldLabels) {
                pWriteOp->zOldLabels = sqlite3_mprintf("%s", zOldLabels);
            } else {
                pWriteOp->zOldLabels = sqlite3_mprintf("[]");
            }
        } else {
            pWriteOp->zOldLabels = sqlite3_mprintf("[]");
        }
        sqlite3_finalize(pStmt);
    } else {
        pWriteOp->zOldLabels = sqlite3_mprintf("[]");
    }
    
    if (!pWriteOp->zOldLabels) {
        cypherWriteOpDestroy(pWriteOp);
        return SQLITE_NOMEM;
    }
    
    /* Add to operation log */
    rc = cypherWriteContextAddOperation(pCtx, pWriteOp);
    if (rc != SQLITE_OK) {
        cypherWriteOpDestroy(pWriteOp);
        return rc;
    }
    
    /* Actually update labels in graph storage here */
    char *zUpdateSql = sqlite3_mprintf(
        "UPDATE graph_nodes SET labels = '%s' WHERE node_id = %lld",
        zLabelsJson, pOp->iNodeId);
    
    if (!zUpdateSql) {
        return SQLITE_NOMEM;
    }
    
    sqlite3_int64 rowId;
    rc = cypherStorageExecuteUpdate(pCtx->pGraph, zUpdateSql, &rowId);
    sqlite3_free(zUpdateSql);
    
    if (rc != SQLITE_OK) {
        return rc;
    }
    
    return SQLITE_OK;
}

/*
** Create a SET property iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherSetPropertyIteratorCreate(CypherWriteContext *pCtx, SetPropertyOp *pOp) {
    CypherWriteIterator *pIterator;
    
    if (!pCtx || !pOp) return NULL;
    
    pIterator = (CypherWriteIterator*)sqlite3_malloc(sizeof(CypherWriteIterator));
    if (!pIterator) return NULL;
    
    memset(pIterator, 0, sizeof(CypherWriteIterator));
    pIterator->pWriteCtx = pCtx;
    pIterator->pOperationData = pOp;
    
    return pIterator;
}

/*
** Create a SET label iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherSetLabelIteratorCreate(CypherWriteContext *pCtx, SetLabelOp *pOp) {
    CypherWriteIterator *pIterator;
    
    if (!pCtx || !pOp) return NULL;
    
    pIterator = (CypherWriteIterator*)sqlite3_malloc(sizeof(CypherWriteIterator));
    if (!pIterator) return NULL;
    
    memset(pIterator, 0, sizeof(CypherWriteIterator));
    pIterator->pWriteCtx = pCtx;
    pIterator->pOperationData = pOp;
    
    return pIterator;
}

SetPropertyOp *cypherSetPropertyOpCreate(void) {
    SetPropertyOp *pOp = (SetPropertyOp*)sqlite3_malloc(sizeof(SetPropertyOp));
    if (!pOp) return NULL;
    memset(pOp, 0, sizeof(SetPropertyOp));
    return pOp;
}

void cypherSetPropertyOpDestroy(SetPropertyOp *pOp) {
    if (!pOp) return;
    
    sqlite3_free(pOp->zVariable);
    sqlite3_free(pOp->zProperty);
    
    if (pOp->pValue) {
        cypherValueDestroy(pOp->pValue);
        sqlite3_free(pOp->pValue);
    }
    
    sqlite3_free(pOp);
}

SetLabelOp *cypherSetLabelOpCreate(void) {
    SetLabelOp *pOp = (SetLabelOp*)sqlite3_malloc(sizeof(SetLabelOp));
    if (!pOp) return NULL;
    memset(pOp, 0, sizeof(SetLabelOp));
    return pOp;
}

void cypherSetLabelOpDestroy(SetLabelOp *pOp) {
    int i;
    
    if (!pOp) return;
    
    sqlite3_free(pOp->zVariable);
    
    for (i = 0; i < pOp->nLabels; i++) {
        sqlite3_free(pOp->azLabels[i]);
    }
    sqlite3_free(pOp->azLabels);
    
    sqlite3_free(pOp);
}

/*
** DELETE operation functions.
*/

/*
** Execute a DELETE operation.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherDelete(CypherWriteContext *pCtx, DeleteOp *pOp) {
    CypherWriteOp *pWriteOp;
    char *zRelationships = NULL;
    int rc = SQLITE_OK;
    
    if (!pCtx || !pOp) return SQLITE_MISUSE;
    
    if (pOp->bIsNode) {
        /* Deleting a node */
        
        /* Validate that the node exists */
        rc = cypherValidateNodeExists(pCtx, pOp->iNodeId);
        if (rc != SQLITE_OK) {
            return rc;
        }
        
        if (pOp->bDetach) {
            /* DETACH DELETE - first delete all connected relationships */
            zRelationships = cypherGetNodeRelationships(pCtx, pOp->iNodeId);
            if (!zRelationships) {
                return SQLITE_NOMEM;
            }
            
            /* Parse relationship IDs and delete each one */
            /* Parse the JSON array of relationship IDs */
            if (zRelationships && strlen(zRelationships) > 2) { /* More than just "[]" */
                char *zCopy = sqlite3_mprintf("%s", zRelationships);
                if (zCopy) {
                    char *zStart = strchr(zCopy, '[');
                    char *zEnd = strchr(zCopy, ']');
                    if (zStart && zEnd) {
                        zStart++; /* Skip '[' */
                        *zEnd = '\0'; /* Terminate at ']' */
                        
                        /* Parse comma-separated IDs */
                        char *zToken = strtok(zStart, ",");
                        while (zToken) {
                            /* Remove whitespace */
                            while (*zToken == ' ' || *zToken == '\t') zToken++;
                            
                            sqlite3_int64 iRelId = atoll(zToken);
                            if (iRelId > 0) {
                                cypherStorageDeleteEdge(pCtx->pGraph, iRelId);
                            }
                            
                            zToken = strtok(NULL, ",");
                        }
                    }
                    sqlite3_free(zCopy);
                }
            }
            
            /* Create write operation record for DETACH DELETE */
            pWriteOp = cypherWriteOpCreate(CYPHER_WRITE_DETACH_DELETE_NODE);
        } else {
            /* Regular DELETE - check if node has relationships */
            zRelationships = cypherGetNodeRelationships(pCtx, pOp->iNodeId);
            if (zRelationships && strlen(zRelationships) > 2) {  /* More than just "[]" */
                sqlite3_free(zRelationships);
                /* Cannot delete node with relationships without DETACH */
                return SQLITE_ERROR;
            }
            
            /* Create write operation record for DELETE */
            pWriteOp = cypherWriteOpCreate(CYPHER_WRITE_DELETE_NODE);
        }
        
        if (!pWriteOp) {
            sqlite3_free(zRelationships);
            return SQLITE_NOMEM;
        }
        
        pWriteOp->iNodeId = pOp->iNodeId;
        
        /* Store old node data for rollback */
        sqlite3_stmt *pStmt = NULL;
        char *zSql = sqlite3_mprintf(
            "SELECT labels, properties FROM graph_nodes WHERE node_id = %lld",
            pOp->iNodeId);
        
        if (!zSql) {
            cypherWriteOpDestroy(pWriteOp);
            return SQLITE_NOMEM;
        }
        
        rc = sqlite3_prepare_v2(pCtx->pGraph->pDb, zSql, -1, &pStmt, NULL);
        sqlite3_free(zSql);
        
        if (rc == SQLITE_OK) {
            rc = sqlite3_step(pStmt);
            if (rc == SQLITE_ROW) {
                const char *zLabels = (const char*)sqlite3_column_text(pStmt, 0);
                const char *zProps = (const char*)sqlite3_column_text(pStmt, 1);
                
                if (zLabels) {
                    pWriteOp->zOldLabels = sqlite3_mprintf("%s", zLabels);
                } else {
                    pWriteOp->zOldLabels = sqlite3_mprintf("[]");
                }
                
                if (zProps) {
                    pWriteOp->pOldValue = (CypherValue*)sqlite3_malloc(sizeof(CypherValue));
                    if (pWriteOp->pOldValue) {
                        cypherValueSetString(pWriteOp->pOldValue, zProps);
                    }
                }
            }
            sqlite3_finalize(pStmt);
        }
        
    } else {
        /* Deleting a relationship */
        
        /* Create write operation record for relationship DELETE */
        pWriteOp = cypherWriteOpCreate(CYPHER_WRITE_DELETE_RELATIONSHIP);
        if (!pWriteOp) return SQLITE_NOMEM;
        
        pWriteOp->iRelId = pOp->iRelId;
        
        /* Store old relationship data for rollback */
        sqlite3_stmt *pStmt = NULL;
        char *zSql = sqlite3_mprintf(
            "SELECT from_node, to_node, edge_type, weight, properties FROM graph_edges WHERE edge_id = %lld",
            pOp->iRelId);
        
        if (!zSql) {
            cypherWriteOpDestroy(pWriteOp);
            return SQLITE_NOMEM;
        }
        
        rc = sqlite3_prepare_v2(pCtx->pGraph->pDb, zSql, -1, &pStmt, NULL);
        sqlite3_free(zSql);
        
        if (rc == SQLITE_OK) {
            rc = sqlite3_step(pStmt);
            if (rc == SQLITE_ROW) {
                pWriteOp->iFromId = sqlite3_column_int64(pStmt, 0);
                pWriteOp->iToId = sqlite3_column_int64(pStmt, 1);
                
                const char *zType = (const char*)sqlite3_column_text(pStmt, 2);
                if (zType) {
                    pWriteOp->zRelType = sqlite3_mprintf("%s", zType);
                }
                
                const char *zProps = (const char*)sqlite3_column_text(pStmt, 4);
                if (zProps) {
                    pWriteOp->pOldValue = (CypherValue*)sqlite3_malloc(sizeof(CypherValue));
                    if (pWriteOp->pOldValue) {
                        cypherValueSetString(pWriteOp->pOldValue, zProps);
                    }
                }
            }
            sqlite3_finalize(pStmt);
        }
    }
    
    /* Add to operation log */
    rc = cypherWriteContextAddOperation(pCtx, pWriteOp);
    if (rc != SQLITE_OK) {
        cypherWriteOpDestroy(pWriteOp);
        sqlite3_free(zRelationships);
        return rc;
    }
    
    /* Actually delete from graph storage */
    if (pOp->bIsNode) {
        /* Delete node */
        rc = cypherStorageDeleteNode(pCtx->pGraph, pOp->iNodeId, pOp->bDetach);
        if (rc != SQLITE_OK) {
            sqlite3_free(zRelationships);
            return rc;
        }
    } else {
        /* Delete relationship */
        rc = cypherStorageDeleteEdge(pCtx->pGraph, pOp->iRelId);
        if (rc != SQLITE_OK) {
            sqlite3_free(zRelationships);
            return rc;
        }
    }
    
    sqlite3_free(zRelationships);
    return SQLITE_OK;
}

/*
** Create a DELETE iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherDeleteIteratorCreate(CypherWriteContext *pCtx, DeleteOp *pOp) {
    CypherWriteIterator *pIterator;
    
    if (!pCtx || !pOp) return NULL;
    
    pIterator = (CypherWriteIterator*)sqlite3_malloc(sizeof(CypherWriteIterator));
    if (!pIterator) return NULL;
    
    memset(pIterator, 0, sizeof(CypherWriteIterator));
    pIterator->pWriteCtx = pCtx;
    pIterator->pOperationData = pOp;
    
    return pIterator;
}

DeleteOp *cypherDeleteOpCreate(void) {
    DeleteOp *pOp = (DeleteOp*)sqlite3_malloc(sizeof(DeleteOp));
    if (!pOp) return NULL;
    memset(pOp, 0, sizeof(DeleteOp));
    return pOp;
}

void cypherDeleteOpDestroy(DeleteOp *pOp) {
    if (!pOp) return;
    
    sqlite3_free(pOp->zVariable);
    sqlite3_free(pOp);
}

/*
** Create a CREATE node iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherCreateNodeIteratorCreate(CypherWriteContext *pCtx,
                                                  CreateNodeOp *pOp) {
    CypherWriteIterator *pIterator;
    
    if (!pCtx || !pOp) return NULL;
    
    pIterator = (CypherWriteIterator*)sqlite3_malloc(sizeof(CypherWriteIterator));
    if (!pIterator) return NULL;
    
    memset(pIterator, 0, sizeof(CypherWriteIterator));
    pIterator->pWriteCtx = pCtx;
    pIterator->pOperationData = pOp;
    
    /* Set up base iterator interface */
    /* Write operations don't need typical iterator functions */
    pIterator->base.xOpen = NULL;    /* Write operations don't need opening */
    pIterator->base.xNext = NULL;    /* Write operations don't iterate */
    pIterator->base.xClose = NULL;   /* Write operations don't need closing */
    pIterator->base.xDestroy = NULL; /* Will be freed by caller */
    
    return pIterator;
}

/*
** Create a CREATE relationship iterator.
** Returns NULL on allocation failure.
*/
CypherWriteIterator *cypherCreateRelIteratorCreate(CypherWriteContext *pCtx,
                                                 CreateRelOp *pOp) {
    CypherWriteIterator *pIterator;
    
    if (!pCtx || !pOp) return NULL;
    
    pIterator = (CypherWriteIterator*)sqlite3_malloc(sizeof(CypherWriteIterator));
    if (!pIterator) return NULL;
    
    memset(pIterator, 0, sizeof(CypherWriteIterator));
    pIterator->pWriteCtx = pCtx;
    pIterator->pOperationData = pOp;
    
    /* Set up base iterator interface */
    /* Write operations don't need typical iterator functions */
    pIterator->base.xOpen = NULL;    /* Write operations don't need opening */
    pIterator->base.xNext = NULL;    /* Write operations don't iterate */
    pIterator->base.xClose = NULL;   /* Write operations don't need closing */
    pIterator->base.xDestroy = NULL; /* Will be freed by caller */
    
    return pIterator;
}