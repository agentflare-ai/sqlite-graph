/*
** SQLite Graph Database Extension - Cypher AST Implementation
**
** This file implements the Abstract Syntax Tree (AST) structures and
** operations for the openCypher query language, based on the design
** specified in ast-generator-plan.md.
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
#define CYPHER_MPRINTF sqlite3_mprintf
#else
// Use standard library functions for standalone/test mode
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#define CYPHER_MALLOC(x) sqlite3_malloc(x)
#define CYPHER_FREE(x) sqlite3_free(x)
#define CYPHER_REALLOC(x, y) sqlite3_realloc(x, y)
static char *CYPHER_MPRINTF(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (len < 0) return NULL;
    char *result = sqlite3_malloc(len + 1);
    if (result) {
        va_start(ap, fmt);
        vsnprintf(result, len + 1, fmt, ap);
        va_end(ap);
    }
    return result;
}
#endif

#include "cypher.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define AST_INITIAL_CHILDREN 4

// Create a new AST node of the specified type.
CypherAst *cypherAstCreate(CypherAstNodeType type, int iLine, int iColumn) {
  CypherAst *pAst = CYPHER_MALLOC(sizeof(CypherAst));
  if( !pAst ) return NULL;

  memset(pAst, 0, sizeof(CypherAst));
  pAst->type = type;
  pAst->iLine = iLine;
  pAst->iColumn = iColumn;
  pAst->nChildrenAlloc = AST_INITIAL_CHILDREN;
  pAst->apChildren = CYPHER_MALLOC(sizeof(CypherAst*) * pAst->nChildrenAlloc);

  if( !pAst->apChildren ) {
    CYPHER_FREE(pAst);
    return NULL;
  }

  memset(pAst->apChildren, 0, sizeof(CypherAst*) * pAst->nChildrenAlloc);
  return pAst;
}

// Destroy an AST node and all its children.
void cypherAstDestroy(CypherAst *pAst) {
  if( !pAst ) return;
  for( int i = 0; i < pAst->nChildren; i++ ) {
    cypherAstDestroy(pAst->apChildren[i]);
  }
  CYPHER_FREE(pAst->apChildren);
  CYPHER_FREE(pAst->zValue);
  CYPHER_FREE(pAst);
}

// Add a child node to an AST node.
void cypherAstAddChild(CypherAst *pParent, CypherAst *pChild) {
  if( !pParent || !pChild ) return;

  if( pParent->nChildren >= pParent->nChildrenAlloc ) {
    int nNewMax = pParent->nChildrenAlloc * 2;
    if (nNewMax == 0) nNewMax = AST_INITIAL_CHILDREN;
    CypherAst **apNew = CYPHER_REALLOC(pParent->apChildren, sizeof(CypherAst*) * nNewMax);
    if( !apNew ) {
      // In a real scenario, we'd need more robust error handling.
      // For now, we fail silently, which is bad practice but avoids crashing.
      return;
    }
    pParent->apChildren = apNew;
    pParent->nChildrenAlloc = nNewMax;
  }
  pParent->apChildren[pParent->nChildren++] = pChild;
}

// Set the string value of an AST node.
void cypherAstSetValue(CypherAst *pAst, const char *zValue) {
  if( !pAst ) return;
  CYPHER_FREE(pAst->zValue);
  pAst->zValue = NULL;
  if( zValue ) {
    pAst->zValue = CYPHER_MPRINTF("%s", zValue);
    // No return value to check against CYPHER_MPRINTF failure in this function signature
  }
}

// Get string representation of AST node type.
const char *cypherAstNodeTypeName(CypherAstNodeType type) {
  switch( type ) {
    case CYPHER_AST_QUERY:           return "QUERY";
    case CYPHER_AST_SINGLE_QUERY:    return "SINGLE_QUERY";
    case CYPHER_AST_UNION:           return "UNION";
    case CYPHER_AST_MATCH:           return "MATCH";
    case CYPHER_AST_OPTIONAL_MATCH:  return "OPTIONAL_MATCH";
    case CYPHER_AST_WHERE:           return "WHERE";
    case CYPHER_AST_RETURN:          return "RETURN";
    case CYPHER_AST_PROJECTION_LIST: return "PROJECTION_LIST";
    case CYPHER_AST_PROJECTION_ITEM: return "PROJECTION_ITEM";
    case CYPHER_AST_ORDER_BY:        return "ORDER_BY";
    case CYPHER_AST_SORT_LIST:       return "SORT_LIST";
    case CYPHER_AST_SORT_ITEM:       return "SORT_ITEM";
    case CYPHER_AST_LIMIT:           return "LIMIT";
    case CYPHER_AST_SKIP:            return "SKIP";
    case CYPHER_AST_PATTERN:         return "PATTERN";
    case CYPHER_AST_PATTERN_LIST:    return "PATTERN_LIST";
    case CYPHER_AST_NODE_PATTERN:    return "NODE_PATTERN";
    case CYPHER_AST_REL_PATTERN:     return "REL_PATTERN";
    case CYPHER_AST_LABELS:          return "LABELS";
    case CYPHER_AST_PATH:            return "PATH";
    case CYPHER_AST_IDENTIFIER:      return "IDENTIFIER";
    case CYPHER_AST_LITERAL:         return "LITERAL";
    case CYPHER_AST_UNARY_OP:        return "UNARY_OP";
    case CYPHER_AST_BINARY_OP:       return "BINARY_OP";
    case CYPHER_AST_PROPERTY:        return "PROPERTY";
    case CYPHER_AST_MAP:             return "MAP";
    case CYPHER_AST_LIST:            return "LIST";
    case CYPHER_AST_FUNCTION_CALL:   return "FUNCTION_CALL";
    case CYPHER_AST_CASE:            return "CASE";
    case CYPHER_AST_PROPERTY_PAIR:   return "PROPERTY_PAIR";
    case CYPHER_AST_AND:             return "AND";
    case CYPHER_AST_OR:              return "OR";
    case CYPHER_AST_NOT:             return "NOT";
    case CYPHER_AST_COMPARISON:      return "COMPARISON";
    case CYPHER_AST_ADDITIVE:        return "ADDITIVE";
    case CYPHER_AST_MULTIPLICATIVE:  return "MULTIPLICATIVE";
    case CYPHER_AST_ARRAY:           return "ARRAY";
    case CYPHER_AST_OBJECT:          return "OBJECT";
    case CYPHER_AST_STARTS_WITH:     return "STARTS_WITH";
    case CYPHER_AST_ENDS_WITH:       return "ENDS_WITH";
    case CYPHER_AST_CONTAINS_OP:     return "CONTAINS_OP";
    case CYPHER_AST_REGEX:           return "REGEX";
    case CYPHER_AST_ON_CREATE:       return "ON_CREATE";
    case CYPHER_AST_ON_MATCH:        return "ON_MATCH";
    case CYPHER_AST_CREATE:          return "CREATE";
    case CYPHER_AST_MERGE:           return "MERGE";
    case CYPHER_AST_SET:             return "SET";
    case CYPHER_AST_DELETE:          return "DELETE";
    case CYPHER_AST_REMOVE:          return "REMOVE";
    case CYPHER_AST_WITH:            return "WITH";
    case CYPHER_AST_COUNT:           return "COUNT";
    default:                         return "UNKNOWN";
  }
}

// Create a literal AST node with the given value.
CypherAst *cypherAstCreateLiteral(const char *zValue, int iLine, int iColumn) {
  CypherAst *pAst = cypherAstCreate(CYPHER_AST_LITERAL, iLine, iColumn);
  if( pAst ) {
    cypherAstSetValue(pAst, zValue);
    if (!pAst->zValue && zValue) { // Check if mprintf failed
        cypherAstDestroy(pAst);
        return NULL;
    }
  }
  return pAst;
}

// Create an identifier AST node with the given name.
CypherAst *cypherAstCreateIdentifier(const char *zName, int iLine, int iColumn) {
  CypherAst *pAst = cypherAstCreate(CYPHER_AST_IDENTIFIER, iLine, iColumn);
  if( pAst ) {
    cypherAstSetValue(pAst, zName);
    if (!pAst->zValue && zName) { // Check if mprintf failed
        cypherAstDestroy(pAst);
        return NULL;
    }
  }
  return pAst;
}

// Create a binary operation AST node.
CypherAst *cypherAstCreateBinaryOp(const char *zOp, CypherAst *pLeft, CypherAst *pRight, int iLine, int iColumn) {
  CypherAst *pAst = cypherAstCreate(CYPHER_AST_BINARY_OP, iLine, iColumn);
  if( !pAst ) return NULL;

  cypherAstSetValue(pAst, zOp);
  if (!pAst->zValue && zOp) { // Check if mprintf failed
      cypherAstDestroy(pAst);
      return NULL;
  }

  cypherAstAddChild(pAst, pLeft);
  cypherAstAddChild(pAst, pRight);
  return pAst;
}

// Create a property access AST node.
CypherAst *cypherAstCreateNodeLabel(const char *zName, int iLine, int iColumn) {
  CypherAst *pAst = cypherAstCreate(CYPHER_AST_LABELS, iLine, iColumn);
  if( pAst ) {
    cypherAstSetValue(pAst, zName);
    if (!pAst->zValue && zName) {
        cypherAstDestroy(pAst);
        return NULL;
    }
  }
  return pAst;
}

CypherAst *cypherAstCreateUnaryOp(const char *zOp, CypherAst *pExpr, int iLine, int iColumn) {
  CypherAst *pAst = cypherAstCreate(CYPHER_AST_UNARY_OP, iLine, iColumn);
  if( !pAst ) return NULL;

  cypherAstSetValue(pAst, zOp);
  if (!pAst->zValue && zOp) {
      cypherAstDestroy(pAst);
      return NULL;
  }

  cypherAstAddChild(pAst, pExpr);
  return pAst;
}

CypherAst *cypherAstCreateProperty(CypherAst *pObject, const char *zProperty, int iLine, int iColumn) {
  CypherAst *pAst = cypherAstCreate(CYPHER_AST_PROPERTY, iLine, iColumn);
  if (!pAst) return NULL;

  // The property name is stored as the value of the property node itself.
  cypherAstSetValue(pAst, zProperty);
  if (!pAst->zValue && zProperty) {
      cypherAstDestroy(pAst);
      return NULL;
  }

  // The object is the first child.
  cypherAstAddChild(pAst, pObject);
  return pAst;
}

// Get the child AST node at the specified index.
CypherAst *cypherAstGetChild(CypherAst *pAst, int iChild) {
  if( !pAst || iChild < 0 || iChild >= pAst->nChildren ) {
    return NULL;
  }
  return pAst->apChildren[iChild];
}

// Get the number of children in an AST node.
int cypherAstGetChildCount(CypherAst *pAst) {
  return pAst ? pAst->nChildren : 0;
}

// Check if an AST node has the specified type.
int cypherAstIsType(CypherAst *pAst, CypherAstNodeType type) {
  return pAst && pAst->type == type;
}

// Get the value of an AST node.
const char *cypherAstGetValue(CypherAst *pAst) {
  return pAst ? pAst->zValue : NULL;
}

#ifdef CYPHER_AST_DEBUG
// Helper for printing with indentation (debug builds only)
static void cypherAstPrintIndent(int iIndent) {
    for (int i = 0; i < iIndent; i++) {
        printf("  ");
    }
}

// Print the AST for debugging (debug builds only)
void cypherAstPrint(CypherAst *pNode, int iIndent) {
    if (!pNode) return;

    cypherAstPrintIndent(iIndent);
    printf("%s", cypherAstNodeTypeName(pNode->type));
    if (pNode->zValue) {
        printf(" (%s)", pNode->zValue);
    }
    printf("\n");

    for (int i = 0; i < pNode->nChildren; i++) {
        cypherAstPrint(pNode->apChildren[i], iIndent + 1);
    }
}
#else
// Stub for production builds
void cypherAstPrint(CypherAst *pNode, int iIndent) {
    (void)pNode;
    (void)iIndent;
    /* Debug printing disabled in production builds */
}
#endif
