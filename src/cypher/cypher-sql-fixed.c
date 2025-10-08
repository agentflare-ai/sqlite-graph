/*
* Cypher SQL Functions - Fixed Interface Version
* Copyright 2024 SQLite Graph Extension Project
*/

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif

#include "cypher.h"
#include "graph-util.h"
#include "graph-memory.h"
#include <string.h>
#include <stdlib.h>

/*
** SQL function: cypher_parse(query_text)
** Simple placeholder that just echoes the query
*/
static void cypherParseSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  const char *zQuery;
  char *zResult;
  
  if( argc != 1 ) {
    sqlite3_result_error(context, "cypher_parse() requires exactly 1 argument", -1);
    return;
  }
  
  zQuery = (const char*)sqlite3_value_text(argv[0]);
  if( !zQuery ) {
    sqlite3_result_error(context, "Invalid query parameter", -1);
    return;
  }
  
  /* Simple echo for now */
  zResult = sqlite3_mprintf("Query: %s", zQuery);
  if( !zResult ) {
    sqlite3_result_error_nomem(context);
    return;
  }
  
  sqlite3_result_text(context, zResult, -1, sqlite3_free);
}

/*
** SQL function: cypher_validate(query_text)
** Returns 1 if valid, 0 if invalid
*/
static void cypherValidateSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  const char *zQuery;
  CypherParser *pParser = NULL;
  CypherAst *pAst = NULL;
  
  if( argc != 1 ) {
    sqlite3_result_error(context, "cypher_validate() requires exactly 1 argument", -1);
    return;
  }
  
  zQuery = (const char*)sqlite3_value_text(argv[0]);
  if( !zQuery ) {
    sqlite3_result_error(context, "Invalid query parameter", -1);
    return;
  }
  
  /* Create parser and validate query */
  pParser = cypherParserCreate();
  if( !pParser ) {
    sqlite3_result_error_nomem(context);
    return;
  }
  
  pAst = cypherParse(pParser, zQuery, NULL);
  
  if( pAst != NULL ) {
    sqlite3_result_int(context, 1); /* Valid */
  } else {
    sqlite3_result_int(context, 0); /* Invalid */
  }
  
  cypherParserDestroy(pParser);
}

/*
** SQL function: cypher_tokenize(query_text)
** Returns JSON array of tokens
*/
static void cypherTokenizeSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  const char *zQuery;
  CypherLexer *pLexer = NULL;
  CypherToken *pToken;
  char *zResult;
  int iTokenCount = 0;
  
  if( argc != 1 ) {
    sqlite3_result_error(context, "cypher_tokenize() requires exactly 1 argument", -1);
    return;
  }
  
  zQuery = (const char*)sqlite3_value_text(argv[0]);
  if( !zQuery ) {
    sqlite3_result_error(context, "Invalid query parameter", -1);
    return;
  }
  
  /* Create lexer */
  pLexer = cypherLexerCreate(zQuery);
  if( !pLexer ) {
    sqlite3_result_error_nomem(context);
    return;
  }
  
  /* Build JSON array of tokens */
  zResult = sqlite3_mprintf("[");
  if( !zResult ) {
    sqlite3_result_error_nomem(context);
    cypherLexerDestroy(pLexer);
    return;
  }
  
  /* Get tokens one by one */
  while( iTokenCount < 100 ) { /* Limit to prevent runaway */
    pToken = cypherLexerNextToken(pLexer);
    if( !pToken || pToken->type == CYPHER_TOK_EOF ) {
      /* Don't free pToken - the lexer manages token memory */
      break;
    }
    
    const char *zTypeName = cypherTokenTypeName(pToken->type);
    
    /* Build token JSON object - copy the token text since it's not null-terminated */
    char *zValue = sqlite3_mprintf("%.*s", pToken->len, pToken->text);
    char *zTokenJson = sqlite3_mprintf(
      "%s{\"type\":\"%s\",\"value\":\"%s\",\"line\":%d,\"column\":%d}",
      (iTokenCount > 0) ? "," : "",
      zTypeName ? zTypeName : "UNKNOWN",
      zValue ? zValue : "",
      pToken->line,
      pToken->column
    );
    
    /* Clean up */
    if (zValue) sqlite3_free(zValue);
    /* Don't free pToken - the lexer manages token memory */
    
    if( !zTokenJson ) {
      sqlite3_result_error_nomem(context);
      sqlite3_free(zResult);
      cypherLexerDestroy(pLexer);
      return;
    }
    
    /* Append to result */
    char *zNewResult = sqlite3_mprintf("%s%s", zResult, zTokenJson);
    sqlite3_free(zResult);
    sqlite3_free(zTokenJson);
    
    if( !zNewResult ) {
      sqlite3_result_error_nomem(context);
      cypherLexerDestroy(pLexer);
      return;
    }
    
    zResult = zNewResult;
    iTokenCount++;
  }
  
  /* Close JSON array */
  char *zFinalResult = sqlite3_mprintf("%s]", zResult);
  sqlite3_free(zResult);
  
  if( !zFinalResult ) {
    sqlite3_result_error_nomem(context);
    cypherLexerDestroy(pLexer);
    return;
  }
  
  sqlite3_result_text(context, zFinalResult, -1, sqlite3_free);
  cypherLexerDestroy(pLexer);
}

/*
** SQL function: cypher_ast_info(query_text)
** Returns AST information
*/
static void cypherAstInfoSqlFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  const char *zQuery;
  CypherParser *pParser = NULL;
  CypherAst *pAst = NULL;
  char *zResult;
  
  if( argc != 1 ) {
    sqlite3_result_error(context, "cypher_ast_info() requires exactly 1 argument", -1);
    return;
  }
  
  zQuery = (const char*)sqlite3_value_text(argv[0]);
  if( !zQuery ) {
    sqlite3_result_error(context, "Invalid query parameter", -1);
    return;
  }
  
  /* Create parser and parse query */
  pParser = cypherParserCreate();
  if( !pParser ) {
    sqlite3_result_error_nomem(context);
    return;
  }
  
  pAst = cypherParse(pParser, zQuery, NULL);
  
  if( pAst ) {
    /* Build comprehensive result */
    zResult = sqlite3_mprintf(
      "Parse Status: SUCCESS\n"
      "AST Type: %s\n"
      "Node Count: %d\n"
      "Validation: PASSED",
      cypherAstNodeTypeName(pAst->type),
      cypherAstGetChildCount(pAst)
    );
  } else {
    zResult = sqlite3_mprintf(
      "Parse Status: FAILED\n"
      "Error: Parse error\n"
      "Validation: FAILED"
    );
  }
  
  if( !zResult ) {
    sqlite3_result_error_nomem(context);
  } else {
    sqlite3_result_text(context, zResult, -1, sqlite3_free);
  }
  
  cypherParserDestroy(pParser);
}

/*
** Register all Cypher SQL functions with the database.
*/
int cypherRegisterSqlFunctions(sqlite3 *db) {
  int rc = SQLITE_OK;
  
  /* Register cypher_parse function */
  rc = sqlite3_create_function(db, "cypher_parse", 1, 
                              SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                              0, cypherParseSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  /* Register cypher_validate function */
  rc = sqlite3_create_function(db, "cypher_validate", 1,
                              SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                              0, cypherValidateSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  /* Register cypher_tokenize function */
  rc = sqlite3_create_function(db, "cypher_tokenize", 1,
                              SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                              0, cypherTokenizeSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  /* Register cypher_ast_info function */
  rc = sqlite3_create_function(db, "cypher_ast_info", 1,
                              SQLITE_UTF8 | SQLITE_DETERMINISTIC,
                              0, cypherAstInfoSqlFunc, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  
  return SQLITE_OK;
}
