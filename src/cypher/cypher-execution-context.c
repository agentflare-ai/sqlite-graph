/* Execution context - manages variables, values, and result rows */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-executor.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

/*
** Create a new execution context.
** Returns NULL on allocation failure.
*/
ExecutionContext *executionContextCreate(sqlite3 *pDb, GraphVtab *pGraph) {
  ExecutionContext *pContext;
  
  pContext = sqlite3_malloc(sizeof(ExecutionContext));
  if( !pContext ) return NULL;
  
  memset(pContext, 0, sizeof(ExecutionContext));
  pContext->pDb = pDb;
  pContext->pGraph = pGraph;
  
  return pContext;
}

/*
** Destroy an execution context and free all associated memory.
** Safe to call with NULL pointer.
*/
void executionContextDestroy(ExecutionContext *pContext) {
  int i;
  
  if( !pContext ) return;
  
  /* Free variable bindings */
  for( i = 0; i < pContext->nVariables; i++ ) {
    sqlite3_free(pContext->azVariables[i]);
    cypherValueDestroy(&pContext->aBindings[i]);
  }
  sqlite3_free(pContext->azVariables);
  sqlite3_free(pContext->aBindings);
  
  /* Free allocated memory blocks */
  for( i = 0; i < pContext->nAllocated; i++ ) {
    sqlite3_free(pContext->apAllocated[i]);
  }
  sqlite3_free(pContext->apAllocated);
  
  sqlite3_free(pContext->zErrorMsg);
  sqlite3_free(pContext);
}

/*
** Bind a variable to a value in the execution context.
** Returns SQLITE_OK on success, error code on failure.
*/
int executionContextBind(ExecutionContext *pContext, const char *zVar, CypherValue *pValue) {
  char **azNew;
  CypherValue *aNew;
  int i;
  
  if( !pContext || !zVar || !pValue ) return SQLITE_MISUSE;
  
  /* Check if variable already exists */
  for( i = 0; i < pContext->nVariables; i++ ) {
    if( strcmp(pContext->azVariables[i], zVar) == 0 ) {
      /* Update existing binding */
      cypherValueDestroy(&pContext->aBindings[i]);
      pContext->aBindings[i] = *cypherValueCopy(pValue);
      return SQLITE_OK;
    }
  }
  
  /* Resize arrays if needed */
  if( pContext->nVariables >= pContext->nVariablesAlloc ) {
    int nNew = pContext->nVariablesAlloc ? pContext->nVariablesAlloc * 2 : 8;
    
    azNew = sqlite3_realloc(pContext->azVariables, nNew * sizeof(char*));
    if( !azNew ) return SQLITE_NOMEM;
    pContext->azVariables = azNew;
    
    aNew = sqlite3_realloc(pContext->aBindings, nNew * sizeof(CypherValue));
    if( !aNew ) return SQLITE_NOMEM;
    pContext->aBindings = aNew;
    
    pContext->nVariablesAlloc = nNew;
  }
  
  /* Add new binding */
  pContext->azVariables[pContext->nVariables] = sqlite3_mprintf("%s", zVar);
  if( !pContext->azVariables[pContext->nVariables] ) return SQLITE_NOMEM;
  
  pContext->aBindings[pContext->nVariables] = *cypherValueCopy(pValue);
  pContext->nVariables++;
  
  return SQLITE_OK;
}

/*
** Get the value of a variable from the execution context.
** Returns NULL if variable not found.
*/
CypherValue *executionContextGet(ExecutionContext *pContext, const char *zVar) {
  int i;
  
  if( !pContext || !zVar ) return NULL;
  
  for( i = 0; i < pContext->nVariables; i++ ) {
    if( strcmp(pContext->azVariables[i], zVar) == 0 ) {
      return &pContext->aBindings[i];
    }
  }
  
  return NULL;
}

/*
** Create a new Cypher value.
** Returns NULL on allocation failure.
*/
CypherValue *cypherValueCreate(CypherValueType type) {
  CypherValue *pValue;
  
  pValue = sqlite3_malloc(sizeof(CypherValue));
  if( !pValue ) return NULL;
  
  memset(pValue, 0, sizeof(CypherValue));
  pValue->type = type;
  
  return pValue;
}

/*
** Destroy a Cypher value and free associated memory.
** Safe to call with NULL pointer.
*/
void cypherValueDestroy(CypherValue *pValue) {
  int i;
  
  if( !pValue ) return;
  
  switch( pValue->type ) {
    case CYPHER_VALUE_STRING:
      sqlite3_free(pValue->u.zString);
      break;

    case CYPHER_VALUE_RELATIONSHIP:
      sqlite3_free(pValue->u.relationship.zType);
      break;

    case CYPHER_VALUE_LIST:
      for( i = 0; i < pValue->u.list.nValues; i++ ) {
        cypherValueDestroy(&pValue->u.list.apValues[i]);
      }
      sqlite3_free(pValue->u.list.apValues);
      break;
      
    case CYPHER_VALUE_MAP:
      for( i = 0; i < pValue->u.map.nPairs; i++ ) {
        sqlite3_free(pValue->u.map.azKeys[i]);
        cypherValueDestroy(&pValue->u.map.apValues[i]);
      }
      sqlite3_free(pValue->u.map.azKeys);
      sqlite3_free(pValue->u.map.apValues);
      break;

    case CYPHER_VALUE_PATH:
      sqlite3_free(pValue->u.path.aiNodeIds);
      sqlite3_free(pValue->u.path.aiRelIds);
      if( pValue->u.path.azRelTypes ) {
        for( i = 0; i < pValue->u.path.nRels; i++ ) {
          sqlite3_free(pValue->u.path.azRelTypes[i]);
        }
        sqlite3_free(pValue->u.path.azRelTypes);
      }
      break;

    default:
      /* No additional cleanup needed for other types */
      break;
  }
  
  /* Don't free pValue itself if it's a stack value */
}

/*
** Copy a Cypher value.
** Returns NULL on allocation failure.
*/
CypherValue *cypherValueCopy(CypherValue *pValue) {
  CypherValue *pCopy;
  int i;
  
  if( !pValue ) return NULL;
  
  pCopy = cypherValueCreate(pValue->type);
  if( !pCopy ) return NULL;
  
  switch( pValue->type ) {
    case CYPHER_VALUE_NULL:
      /* Nothing to copy */
      break;
      
    case CYPHER_VALUE_BOOLEAN:
      pCopy->u.bBoolean = pValue->u.bBoolean;
      break;
      
    case CYPHER_VALUE_INTEGER:
      pCopy->u.iInteger = pValue->u.iInteger;
      break;
      
    case CYPHER_VALUE_FLOAT:
      pCopy->u.rFloat = pValue->u.rFloat;
      break;
      
    case CYPHER_VALUE_STRING:
      if( pValue->u.zString ) {
        pCopy->u.zString = sqlite3_mprintf("%s", pValue->u.zString);
        if( !pCopy->u.zString ) {
          cypherValueDestroy(pCopy);
          sqlite3_free(pCopy);
          return NULL;
        }
      }
      break;
      
    case CYPHER_VALUE_NODE:
      pCopy->u.iNodeId = pValue->u.iNodeId;
      break;
      
    case CYPHER_VALUE_RELATIONSHIP:
      pCopy->u.relationship.iRelId = pValue->u.relationship.iRelId;
      if( pValue->u.relationship.zType ) {
        pCopy->u.relationship.zType = sqlite3_mprintf("%s", pValue->u.relationship.zType);
        if( !pCopy->u.relationship.zType ) {
          cypherValueDestroy(pCopy);
          sqlite3_free(pCopy);
          return NULL;
        }
      }
      break;
      
    case CYPHER_VALUE_LIST:
      if( pValue->u.list.nValues > 0 ) {
        pCopy->u.list.apValues = sqlite3_malloc(pValue->u.list.nValues * sizeof(CypherValue));
        if( !pCopy->u.list.apValues ) {
          cypherValueDestroy(pCopy);
          sqlite3_free(pCopy);
          return NULL;
        }

        pCopy->u.list.nValues = pValue->u.list.nValues;
        for( i = 0; i < pValue->u.list.nValues; i++ ) {
          CypherValue *pElementCopy = cypherValueCopy(&pValue->u.list.apValues[i]);
          if( !pElementCopy ) {
            cypherValueDestroy(pCopy);
            sqlite3_free(pCopy);
            return NULL;
          }
          pCopy->u.list.apValues[i] = *pElementCopy;
          sqlite3_free(pElementCopy);
        }
      }
      break;

    case CYPHER_VALUE_PATH:
      pCopy->u.path.nNodes = pValue->u.path.nNodes;
      pCopy->u.path.nRels = pValue->u.path.nRels;

      /* Copy node IDs */
      if( pValue->u.path.nNodes > 0 && pValue->u.path.aiNodeIds ) {
        pCopy->u.path.aiNodeIds = sqlite3_malloc(pValue->u.path.nNodes * sizeof(sqlite3_int64));
        if( !pCopy->u.path.aiNodeIds ) {
          cypherValueDestroy(pCopy);
          sqlite3_free(pCopy);
          return NULL;
        }
        memcpy(pCopy->u.path.aiNodeIds, pValue->u.path.aiNodeIds,
               pValue->u.path.nNodes * sizeof(sqlite3_int64));
      }

      /* Copy relationship IDs */
      if( pValue->u.path.nRels > 0 && pValue->u.path.aiRelIds ) {
        pCopy->u.path.aiRelIds = sqlite3_malloc(pValue->u.path.nRels * sizeof(sqlite3_int64));
        if( !pCopy->u.path.aiRelIds ) {
          cypherValueDestroy(pCopy);
          sqlite3_free(pCopy);
          return NULL;
        }
        memcpy(pCopy->u.path.aiRelIds, pValue->u.path.aiRelIds,
               pValue->u.path.nRels * sizeof(sqlite3_int64));
      }

      /* Copy relationship types */
      if( pValue->u.path.nRels > 0 && pValue->u.path.azRelTypes ) {
        pCopy->u.path.azRelTypes = sqlite3_malloc(pValue->u.path.nRels * sizeof(char*));
        if( !pCopy->u.path.azRelTypes ) {
          cypherValueDestroy(pCopy);
          sqlite3_free(pCopy);
          return NULL;
        }
        for( i = 0; i < pValue->u.path.nRels; i++ ) {
          if( pValue->u.path.azRelTypes[i] ) {
            pCopy->u.path.azRelTypes[i] = sqlite3_mprintf("%s", pValue->u.path.azRelTypes[i]);
          } else {
            pCopy->u.path.azRelTypes[i] = NULL;
          }
        }
      }
      break;

    default:
      /* Unsupported type for copying */
      cypherValueDestroy(pCopy);
      sqlite3_free(pCopy);
      return NULL;
  }
  
  return pCopy;
}

/*
** Set string value.
** Makes a copy of the string using sqlite3_malloc().
*/
int cypherValueSetString(CypherValue *pValue, const char *zString) {
  if( !pValue ) return SQLITE_MISUSE;
  
  /* Free existing string if any */
  if( pValue->type == CYPHER_VALUE_STRING && pValue->u.zString ) {
    sqlite3_free(pValue->u.zString);
  }
  
  pValue->type = CYPHER_VALUE_STRING;
  
  if( zString ) {
    pValue->u.zString = sqlite3_mprintf("%s", zString);
    if( !pValue->u.zString ) return SQLITE_NOMEM;
  } else {
    pValue->u.zString = NULL;
  }
  
  return SQLITE_OK;
}

/*
** Get string representation of a Cypher value.
** Caller must sqlite3_free() the returned string.
*/
char *cypherValueToString(CypherValue *pValue) {
  if( !pValue ) return sqlite3_mprintf("(null)");
  
  switch( pValue->type ) {
    case CYPHER_VALUE_NULL:
      return sqlite3_mprintf("null");
      
    case CYPHER_VALUE_BOOLEAN:
      return sqlite3_mprintf(pValue->u.bBoolean ? "true" : "false");
      
    case CYPHER_VALUE_INTEGER:
      return sqlite3_mprintf("%lld", pValue->u.iInteger);
      
    case CYPHER_VALUE_FLOAT:
      return sqlite3_mprintf("%.6g", pValue->u.rFloat);
      
    case CYPHER_VALUE_STRING:
      return sqlite3_mprintf("\"%s\"", pValue->u.zString ? pValue->u.zString : "");
      
    case CYPHER_VALUE_NODE:
      return sqlite3_mprintf("Node(%lld)", pValue->u.iNodeId);
      
    case CYPHER_VALUE_RELATIONSHIP:
      if (pValue->u.relationship.zType) {
        return sqlite3_mprintf("Relationship(id=%lld,type=%s)",
                             pValue->u.relationship.iRelId,
                             pValue->u.relationship.zType);
      } else {
        return sqlite3_mprintf("Relationship(%lld)", pValue->u.relationship.iRelId);
      }
      
    case CYPHER_VALUE_LIST:
      /* Simplified list representation */
      return sqlite3_mprintf("[List with %d elements]", pValue->u.list.nValues);

    case CYPHER_VALUE_MAP:
      /* Simplified map representation */
      return sqlite3_mprintf("{Map with %d pairs}", pValue->u.map.nPairs);

    case CYPHER_VALUE_PATH: {
      /* Serialize path as <(node1)-[rel1]->(node2)-...> */
      char *zResult = sqlite3_mprintf("<");
      if( !zResult ) return NULL;

      for( int i = 0; i < pValue->u.path.nNodes; i++ ) {
        /* Add node */
        char *zNew = sqlite3_mprintf("%s()", zResult);
        sqlite3_free(zResult);
        if( !zNew ) return NULL;
        zResult = zNew;

        /* Add relationship if not the last node */
        if( i < pValue->u.path.nRels ) {
          if( pValue->u.path.azRelTypes && pValue->u.path.azRelTypes[i] ) {
            zNew = sqlite3_mprintf("%s-[:%s]->", zResult, pValue->u.path.azRelTypes[i]);
          } else {
            zNew = sqlite3_mprintf("%s-[]->", zResult);
          }
          sqlite3_free(zResult);
          if( !zNew ) return NULL;
          zResult = zNew;
        }
      }

      /* Close path representation */
      char *zFinal = sqlite3_mprintf("%s>", zResult);
      sqlite3_free(zResult);
      return zFinal;
    }

    default:
      return sqlite3_mprintf("Unknown(%d)", pValue->type);
  }
}

/*
** Get string representation of value type.
** Returns static string, do not free.
*/
const char *cypherValueTypeName(CypherValueType type) {
  switch( type ) {
    case CYPHER_VALUE_NULL:         return "NULL";
    case CYPHER_VALUE_BOOLEAN:      return "BOOLEAN";
    case CYPHER_VALUE_INTEGER:      return "INTEGER";
    case CYPHER_VALUE_FLOAT:        return "FLOAT";
    case CYPHER_VALUE_STRING:       return "STRING";
    case CYPHER_VALUE_NODE:         return "NODE";
    case CYPHER_VALUE_RELATIONSHIP: return "RELATIONSHIP";
    case CYPHER_VALUE_PATH:         return "PATH";
    case CYPHER_VALUE_LIST:         return "LIST";
    case CYPHER_VALUE_MAP:          return "MAP";
    default:                        return "UNKNOWN";
  }
}

/*
** Initialize a CypherValue to NULL.
*/
void cypherValueInit(CypherValue *pValue) {
  if( pValue ) {
    memset(pValue, 0, sizeof(CypherValue));
    pValue->type = CYPHER_VALUE_NULL;
  }
}

/*
** Set a CypherValue to NULL.
*/
void cypherValueSetNull(CypherValue *pValue) {
  if( pValue ) {
    cypherValueDestroy(pValue);
    pValue->type = CYPHER_VALUE_NULL;
    memset(&pValue->u, 0, sizeof(pValue->u));
  }
}

/*
** Set a CypherValue to an integer.
*/
void cypherValueSetInteger(CypherValue *pValue, sqlite3_int64 iValue) {
  if( pValue ) {
    cypherValueDestroy(pValue);
    pValue->type = CYPHER_VALUE_INTEGER;
    pValue->u.iInteger = iValue;
  }
}

/*
** Set a CypherValue to a float.
*/
void cypherValueSetFloat(CypherValue *pValue, double rValue) {
  if( pValue ) {
    cypherValueDestroy(pValue);
    pValue->type = CYPHER_VALUE_FLOAT;
    pValue->u.rFloat = rValue;
  }
}

/*
** Create a new result row.
** Returns NULL on allocation failure.
*/
CypherResult *cypherResultCreate(void) {
  CypherResult *pResult;
  
  pResult = sqlite3_malloc(sizeof(CypherResult));
  if( !pResult ) return NULL;
  
  memset(pResult, 0, sizeof(CypherResult));
  return pResult;
}

/*
** Destroy a result row and free all associated memory.
** Safe to call with NULL pointer.
*/
/*
** Clear the contents of a result row (for stack-allocated results).
** Does NOT free the pResult struct itself.
*/
void cypherResultClear(CypherResult *pResult) {
  int i;

  if( !pResult ) return;

  /* Free column names and values */
  for( i = 0; i < pResult->nColumns; i++ ) {
    sqlite3_free(pResult->azColumnNames[i]);
    cypherValueDestroy(&pResult->aValues[i]);
  }
  sqlite3_free(pResult->azColumnNames);
  sqlite3_free(pResult->aValues);

  /* Reset to initial state */
  pResult->azColumnNames = NULL;
  pResult->aValues = NULL;
  pResult->nColumns = 0;
  pResult->nColumnsAlloc = 0;
}

void cypherResultDestroy(CypherResult *pResult) {
  if( !pResult ) return;

  cypherResultClear(pResult);
  sqlite3_free(pResult);
}

/*
** Add a column to a result row.
** Returns SQLITE_OK on success, error code on failure.
*/
int cypherResultAddColumn(CypherResult *pResult, const char *zName, CypherValue *pValue) {
  char **azNew;
  CypherValue *aNew;
  
  if( !pResult || !zName || !pValue ) return SQLITE_MISUSE;
  
  /* Resize arrays if needed */
  if( pResult->nColumns >= pResult->nColumnsAlloc ) {
    int nNew = pResult->nColumnsAlloc ? pResult->nColumnsAlloc * 2 : 4;
    
    azNew = sqlite3_realloc(pResult->azColumnNames, nNew * sizeof(char*));
    if( !azNew ) return SQLITE_NOMEM;
    pResult->azColumnNames = azNew;
    
    aNew = sqlite3_realloc(pResult->aValues, nNew * sizeof(CypherValue));
    if( !aNew ) return SQLITE_NOMEM;
    pResult->aValues = aNew;
    
    pResult->nColumnsAlloc = nNew;
  }
  
  /* Add column */
  pResult->azColumnNames[pResult->nColumns] = sqlite3_mprintf("%s", zName);
  if( !pResult->azColumnNames[pResult->nColumns] ) return SQLITE_NOMEM;

  CypherValue *pCopy = cypherValueCopy(pValue);
  if( !pCopy ) {
    sqlite3_free(pResult->azColumnNames[pResult->nColumns]);
    pResult->azColumnNames[pResult->nColumns] = NULL;
    return SQLITE_NOMEM;
  }

  pResult->aValues[pResult->nColumns] = *pCopy;
  /* Don't call cypherValueDestroy() on pCopy because we did a struct copy above.
   * The pointers inside (zString, zType, etc.) are now owned by aValues[nColumns].
   * Just free the wrapper itself. */
  sqlite3_free(pCopy);
  pResult->nColumns++;

  return SQLITE_OK;
}

/*
** Set a CypherValue to a boolean.
*/
void cypherValueSetBoolean(CypherValue *pValue, int bValue) {
  if( pValue ) {
    cypherValueDestroy(pValue);
    pValue->type = CYPHER_VALUE_BOOLEAN;
    pValue->u.bBoolean = bValue ? 1 : 0;
  }
}

/*
** Get boolean value from a CypherValue.
** Returns 0 if value is not boolean or is NULL.
*/
int cypherValueGetBoolean(const CypherValue *pValue) {
  if( !pValue || pValue->type != CYPHER_VALUE_BOOLEAN ) return 0;
  return pValue->u.bBoolean;
}

/*
** Get integer value from a CypherValue.
** Returns 0 if value is not integer or is NULL.
*/
sqlite3_int64 cypherValueGetInteger(const CypherValue *pValue) {
  if( !pValue || pValue->type != CYPHER_VALUE_INTEGER ) return 0;
  return pValue->u.iInteger;
}

/*
** Get float value from a CypherValue.
** Returns 0.0 if value is not float or is NULL.
*/
double cypherValueGetFloat(const CypherValue *pValue) {
  if( !pValue || pValue->type != CYPHER_VALUE_FLOAT ) return 0.0;
  return pValue->u.rFloat;
}

/*
** Get string value from a CypherValue.
** Returns NULL if value is not string or is NULL.
*/
const char *cypherValueGetString(const CypherValue *pValue) {
  if( !pValue || pValue->type != CYPHER_VALUE_STRING ) return NULL;
  return pValue->u.zString;
}

/*
** Compare two CypherValues.
** Returns:
**   -1 if pLeft < pRight
**    0 if pLeft == pRight  
**    1 if pLeft > pRight
**   SQLITE_MISMATCH if types are incompatible
*/
int cypherValueCompare(const CypherValue *pLeft, const CypherValue *pRight) {
  if( !pLeft && !pRight ) return 0;
  if( !pLeft ) return -1;
  if( !pRight ) return 1;
  
  /* NULL values */
  if( pLeft->type == CYPHER_VALUE_NULL && pRight->type == CYPHER_VALUE_NULL ) return 0;
  if( pLeft->type == CYPHER_VALUE_NULL ) return -1;
  if( pRight->type == CYPHER_VALUE_NULL ) return 1;
  
  /* Different types cannot be compared directly */
  if( pLeft->type != pRight->type ) return SQLITE_MISMATCH;
  
  switch( pLeft->type ) {
    case CYPHER_VALUE_BOOLEAN:
      return (pLeft->u.bBoolean == pRight->u.bBoolean) ? 0 : 
             (pLeft->u.bBoolean ? 1 : -1);
             
    case CYPHER_VALUE_INTEGER:
      if( pLeft->u.iInteger < pRight->u.iInteger ) return -1;
      if( pLeft->u.iInteger > pRight->u.iInteger ) return 1;
      return 0;
      
    case CYPHER_VALUE_FLOAT:
      if( pLeft->u.rFloat < pRight->u.rFloat ) return -1;
      if( pLeft->u.rFloat > pRight->u.rFloat ) return 1;
      return 0;
      
    case CYPHER_VALUE_STRING:
      if( !pLeft->u.zString && !pRight->u.zString ) return 0;
      if( !pLeft->u.zString ) return -1;
      if( !pRight->u.zString ) return 1;
      return strcmp(pLeft->u.zString, pRight->u.zString);
      
    case CYPHER_VALUE_NODE:
      if( pLeft->u.iNodeId < pRight->u.iNodeId ) return -1;
      if( pLeft->u.iNodeId > pRight->u.iNodeId ) return 1;
      return 0;
      
    case CYPHER_VALUE_RELATIONSHIP:
      if( pLeft->u.relationship.iRelId < pRight->u.relationship.iRelId ) return -1;
      if( pLeft->u.relationship.iRelId > pRight->u.relationship.iRelId ) return 1;
      return 0;
      
    default:
      /* Complex types not directly comparable */
      return SQLITE_MISMATCH;
  }
}

/*
** Set a CypherValue to a list.
** Takes ownership of the values array.
*/
void cypherValueSetList(CypherValue *pValue, CypherValue *apValues, int nValues) {
  if( !pValue ) return;
  
  cypherValueDestroy(pValue);
  pValue->type = CYPHER_VALUE_LIST;
  pValue->u.list.apValues = apValues;
  pValue->u.list.nValues = nValues;
}

/*
** Set a CypherValue to a map.
** Takes ownership of the keys and values arrays.
*/
void cypherValueSetMap(CypherValue *pValue, char **azKeys, CypherValue *apValues, int nPairs) {
  if( !pValue ) return;
  
  cypherValueDestroy(pValue);
  pValue->type = CYPHER_VALUE_MAP;
  pValue->u.map.azKeys = azKeys;
  pValue->u.map.apValues = apValues;
  pValue->u.map.nPairs = nPairs;
}

/*
** Check if a CypherValue is NULL.
*/
int cypherValueIsNull(const CypherValue *pValue) {
  return !pValue || pValue->type == CYPHER_VALUE_NULL;
}

/*
** Check if a CypherValue is a list.
*/
int cypherValueIsList(const CypherValue *pValue) {
  return pValue && pValue->type == CYPHER_VALUE_LIST;
}

/*
** Check if a CypherValue is a map.
*/
int cypherValueIsMap(const CypherValue *pValue) {
  return pValue && pValue->type == CYPHER_VALUE_MAP;
}

/*
** Set a CypherValue to a node ID.
*/
void cypherValueSetNode(CypherValue *pValue, sqlite3_int64 iNodeId) {
  if( pValue ) {
    cypherValueDestroy(pValue);
    pValue->type = CYPHER_VALUE_NODE;
    pValue->u.iNodeId = iNodeId;
  }
}

/*
** Set a CypherValue to a relationship ID.
*/
void cypherValueSetRelationship(CypherValue *pValue, sqlite3_int64 iRelId) {
  if( pValue ) {
    cypherValueDestroy(pValue);
    pValue->type = CYPHER_VALUE_RELATIONSHIP;
    pValue->u.relationship.iRelId = iRelId;
    pValue->u.relationship.zType = NULL;  /* Type not provided in this function */
  }
}

/*
** Set a CypherValue to a path.
** Makes copies of the arrays.
*/
void cypherValueSetPath(CypherValue *pValue, sqlite3_int64 *aiNodeIds, int nNodes,
                        sqlite3_int64 *aiRelIds, char **azRelTypes, int nRels) {
  if( !pValue ) return;

  cypherValueDestroy(pValue);
  pValue->type = CYPHER_VALUE_PATH;
  pValue->u.path.nNodes = nNodes;
  pValue->u.path.nRels = nRels;

  /* Copy node IDs */
  if( nNodes > 0 && aiNodeIds ) {
    pValue->u.path.aiNodeIds = sqlite3_malloc(nNodes * sizeof(sqlite3_int64));
    if( pValue->u.path.aiNodeIds ) {
      memcpy(pValue->u.path.aiNodeIds, aiNodeIds, nNodes * sizeof(sqlite3_int64));
    }
  } else {
    pValue->u.path.aiNodeIds = NULL;
  }

  /* Copy relationship IDs */
  if( nRels > 0 && aiRelIds ) {
    pValue->u.path.aiRelIds = sqlite3_malloc(nRels * sizeof(sqlite3_int64));
    if( pValue->u.path.aiRelIds ) {
      memcpy(pValue->u.path.aiRelIds, aiRelIds, nRels * sizeof(sqlite3_int64));
    }
  } else {
    pValue->u.path.aiRelIds = NULL;
  }

  /* Copy relationship types */
  if( nRels > 0 && azRelTypes ) {
    pValue->u.path.azRelTypes = sqlite3_malloc(nRels * sizeof(char*));
    if( pValue->u.path.azRelTypes ) {
      for( int i = 0; i < nRels; i++ ) {
        if( azRelTypes[i] ) {
          pValue->u.path.azRelTypes[i] = sqlite3_mprintf("%s", azRelTypes[i]);
        } else {
          pValue->u.path.azRelTypes[i] = NULL;
        }
      }
    }
  } else {
    pValue->u.path.azRelTypes = NULL;
  }
}

/*
** Get JSON representation of a result row.
** Caller must sqlite3_free() the returned string.
*/
char *cypherResultToJson(CypherResult *pResult) {
  char *zResult, *zValue;
  int i, nResult = 0, nAlloc = 256;
  
  if( !pResult ) return sqlite3_mprintf("null");
  
  zResult = sqlite3_malloc(nAlloc);
  if( !zResult ) return NULL;
  
  /* Start JSON object */
  nResult = snprintf(zResult, nAlloc, "{");

  for( i = 0; i < pResult->nColumns; i++ ) {
    zValue = cypherValueToString(&pResult->aValues[i]);
    if( !zValue ) {
      sqlite3_free(zResult);
      return NULL;
    }
    
    /* Calculate space needed */
    int nNeeded = snprintf(NULL, 0, "%s\"%s\":%s", 
                          i > 0 ? "," : "", 
                          pResult->azColumnNames[i], 
                          zValue);
    
    /* Resize if needed */
    if( nResult + nNeeded + 2 >= nAlloc ) {
      nAlloc = (nResult + nNeeded + 256) * 2;
      char *zNew = sqlite3_realloc(zResult, nAlloc);
      if( !zNew ) {
        sqlite3_free(zResult);
        sqlite3_free(zValue);
        return NULL;
      }
      zResult = zNew;
    }
    
    /* Add column to JSON */
    nResult += snprintf(zResult + nResult, nAlloc - nResult,
                       "%s\"%s\":%s", 
                       i > 0 ? "," : "", 
                       pResult->azColumnNames[i], 
                       zValue);
    
    sqlite3_free(zValue);
  }
  
  /* Close JSON object */
  if( nResult + 2 < nAlloc ) {
    zResult[nResult++] = '}';
    zResult[nResult] = '\0';
  }
  
  return zResult;
}

/*
** Convert a Cypher result set to pretty-formatted JSON with proper indentation.
** This enhanced serializer handles complex nested structures better.
*/
char *cypherResultToFormattedJson(CypherResult *pResult, int nIndent) {
  char *zResult, *zValue;
  int i, nResult = 0, nAlloc = 512;
  char *zIndent = sqlite3_malloc(nIndent + 1);
  
  if (!pResult || !zIndent) {
    sqlite3_free(zIndent);
    return sqlite3_mprintf("null");
  }
  
  /* Create indentation string */
  memset(zIndent, ' ', nIndent);
  zIndent[nIndent] = '\0';
  
  zResult = sqlite3_malloc(nAlloc);
  if (!zResult) {
    sqlite3_free(zIndent);
    return NULL;
  }
  
  /* Start JSON object with formatting */
  nResult = snprintf(zResult, nAlloc, "{\n");
  
  for (i = 0; i < pResult->nColumns; i++) {
    /* Format column value based on type */
    if (pResult->aValues[i].type == CYPHER_VALUE_LIST) {
      zValue = cypherValueToFormattedJson(&pResult->aValues[i], nIndent + 2);
    } else if (pResult->aValues[i].type == CYPHER_VALUE_MAP) {
      zValue = cypherValueToFormattedJson(&pResult->aValues[i], nIndent + 2);
    } else {
      zValue = cypherValueToString(&pResult->aValues[i]);
    }
    
    if (!zValue) {
      sqlite3_free(zResult);
      sqlite3_free(zIndent);
      return NULL;
    }
    
    /* Calculate space needed for formatted output */
    int nNeeded = snprintf(NULL, 0, "%s  \"%s\": %s%s\n", 
                          zIndent,
                          pResult->azColumnNames[i], 
                          zValue,
                          i < pResult->nColumns - 1 ? "," : "");
    
    /* Resize if needed */
    if (nResult + nNeeded + 10 >= nAlloc) {
      nAlloc = (nResult + nNeeded + 10) * 2;
      char *zNewResult = sqlite3_realloc(zResult, nAlloc);
      if (!zNewResult) {
        sqlite3_free(zResult);
        sqlite3_free(zValue);
        sqlite3_free(zIndent);
        return NULL;
      }
      zResult = zNewResult;
    }
    
    /* Append formatted column */
    nResult += snprintf(zResult + nResult, nAlloc - nResult,
                       "%s  \"%s\": %s%s\n", 
                       zIndent,
                       pResult->azColumnNames[i], 
                       zValue,
                       i < pResult->nColumns - 1 ? "," : "");
    
    sqlite3_free(zValue);
  }
  
  /* Close JSON object */
  if (nResult + (int)strlen(zIndent) + 3 < nAlloc) {
    nResult += snprintf(zResult + nResult, nAlloc - nResult, "%s}", zIndent);
  }
  
  sqlite3_free(zIndent);
  return zResult;
}

/*
** Convert a Cypher value to formatted JSON string with indentation.
** Handles complex nested structures like lists and maps.
*/
char *cypherValueToFormattedJson(const CypherValue *pValue, int nIndent) {
  if (!pValue) return sqlite3_mprintf("null");
  
  switch (pValue->type) {
    case CYPHER_VALUE_LIST: {
      char *zResult = sqlite3_malloc(256);
      int nUsed = snprintf(zResult, 256, "[\n");
      
      for (int i = 0; i < pValue->u.list.nValues; i++) {
        char *zItem = cypherValueToFormattedJson(&pValue->u.list.apValues[i], nIndent + 2);
        /* Add indentation and item */
        char *zIndent = sqlite3_malloc(nIndent + 3);
        memset(zIndent, ' ', nIndent + 2);
        zIndent[nIndent + 2] = '\0';
        
        /* Resize if needed and append */
        int nNeeded = strlen(zItem) + strlen(zIndent) + 10;
        if (nUsed + nNeeded > 256) {
          zResult = sqlite3_realloc(zResult, nUsed + nNeeded + 100);
        }
        
        nUsed += snprintf(zResult + nUsed, nNeeded + 50, "%s%s%s\n",
                         zIndent, zItem, i < pValue->u.list.nValues - 1 ? "," : "");
        
        sqlite3_free(zItem);
        sqlite3_free(zIndent);
      }
      
      /* Add closing bracket with proper indentation */
      char *zIndent = sqlite3_malloc(nIndent + 1);
      memset(zIndent, ' ', nIndent);
      zIndent[nIndent] = '\0';
      
      snprintf(zResult + nUsed, 50, "%s]", zIndent);
      sqlite3_free(zIndent);
      
      return zResult;
    }
    
    case CYPHER_VALUE_MAP: {
      /* Similar logic for maps - would be implemented based on map structure */
      return sqlite3_mprintf("{...}");  /* Simplified for now */
    }
    
    default:
      /* Use regular string conversion for primitive types */
      /* Cast away const since cypherValueToString doesn't modify the value */
      return cypherValueToString((CypherValue*)pValue);
  }
}