/*
 * Copyright 2018-2024 Redis Labs Ltd. and Contributors
 *
 * This file is available under the Redis Source Available License 2.0,
 * applying the same terms and conditions as the Redis Source Available
 * License 2.0. You may not use this file except in compliance with the Redis
 * Source Available License 2.0.
 *
 * A copy of the Redis Source Available License 2.0 is available at
 * https://redis.io/rsal/Redis-Source-Available-License-2.0/
 *
 * The Redis Source Available License 2.0 is a copy-left license that requires
 * any derivative work to be made available under the same terms and
 * conditions.
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
#define CYPHER_MALLOC(x) CYPHER_MALLOC (x)
#define CYPHER_FREE(x) CYPHER_FREE (x)
#define CYPHER_REALLOC(x, y) CYPHER_REALLOC (x, y)
#else
// Use standard library functions for standalone/test mode
#include <sqlite3.h>
#include <stdlib.h>
#define CYPHER_MALLOC(x) sqlite3_malloc (x)
#define CYPHER_FREE(x) sqlite3_free (x)
#define CYPHER_REALLOC(x, y) sqlite3_realloc (x, y)
#endif

#include "cypher-errors.h"
#include "cypher.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for recursive descent parsing functions
static CypherAst *parseQuery (CypherLexer *pLexer, CypherParser *pParser);
static CypherAst *parseSingleQuery (CypherLexer *pLexer,
                                    CypherParser *pParser);
static CypherAst *parseMatchClause (CypherLexer *pLexer,
                                    CypherParser *pParser);
static CypherAst *parseCreateClause (CypherLexer *pLexer,
                                     CypherParser *pParser);
static CypherAst *parseMergeClause (CypherLexer *pLexer,
                                    CypherParser *pParser);
static CypherAst *parseSetClause (CypherLexer *pLexer, CypherParser *pParser);
static CypherAst *parseDeleteClause (CypherLexer *pLexer,
                                     CypherParser *pParser);
static CypherAst *parsePatternList (CypherLexer *pLexer,
                                    CypherParser *pParser);
static CypherAst *parsePattern (CypherLexer *pLexer, CypherParser *pParser);
static CypherAst *parseNodePattern (CypherLexer *pLexer,
                                    CypherParser *pParser);
static CypherAst *parseNodeLabels (CypherLexer *pLexer, CypherParser *pParser);
static CypherAst *parsePropertyMap (CypherLexer *pLexer,
                                    CypherParser *pParser);
static CypherAst *parseListLiteral (CypherLexer *pLexer,
                                    CypherParser *pParser);
static CypherAst *parseMapLiteral (CypherLexer *pLexer, CypherParser *pParser);
static CypherAst *parseFunctionCall (CypherLexer *pLexer,
                                     CypherParser *pParser,
                                     CypherAst *pFunctionName);
static CypherAst *parseRelationshipPattern (CypherLexer *pLexer,
                                            CypherParser *pParser);
static CypherAst *parseWhereClause (CypherLexer *pLexer,
                                    CypherParser *pParser);
static CypherAst *parseReturnClause (CypherLexer *pLexer,
                                     CypherParser *pParser);
static CypherAst *parseProjectionList (CypherLexer *pLexer,
                                       CypherParser *pParser);
static CypherAst *parseProjectionItem (CypherLexer *pLexer,
                                       CypherParser *pParser);
static CypherAst *parseExpression (CypherLexer *pLexer, CypherParser *pParser);
static CypherAst *parseOrExpression (CypherLexer *pLexer,
                                     CypherParser *pParser);
static CypherAst *parseAndExpression (CypherLexer *pLexer,
                                      CypherParser *pParser);
static CypherAst *parseNotExpression (CypherLexer *pLexer,
                                      CypherParser *pParser);
static CypherAst *parseComparisonExpression (CypherLexer *pLexer,
                                             CypherParser *pParser);
static CypherAst *parseAdditiveExpression (CypherLexer *pLexer,
                                           CypherParser *pParser);
static CypherAst *parseMultiplicativeExpression (CypherLexer *pLexer,
                                                 CypherParser *pParser);
static CypherAst *parseUnaryExpression (CypherLexer *pLexer,
                                        CypherParser *pParser);
static CypherAst *parsePrimaryExpression (CypherLexer *pLexer,
                                          CypherParser *pParser);
static CypherAst *parsePropertyExpression (CypherLexer *pLexer,
                                           CypherParser *pParser,
                                           CypherAst *pExpr);
static CypherAst *parseLiteral (CypherLexer *pLexer, CypherParser *pParser);

/* Static storage for peeked token to avoid returning pointer to reused memory
 */
static CypherToken s_peekedToken;

static CypherToken *
parserPeekToken (CypherLexer *pLexer)
{
  // The lexer reuses a single token structure, so we need to copy it
  // to avoid returning a pointer to memory that will be overwritten.
  int pos = pLexer->iPos;
  int line = pLexer->iLine;
  int col = pLexer->iColumn;
  CypherToken *token = cypherLexerNextToken (pLexer);
  if (token)
    {
      /* Copy token contents to static storage */
      s_peekedToken = *token;
    }
  pLexer->iPos = pos;
  pLexer->iLine = line;
  pLexer->iColumn = col;
  return token ? &s_peekedToken : NULL;
}

static CypherToken *
parserConsumeToken (CypherLexer *pLexer, CypherTokenType expectedType)
{
  CypherToken *token = cypherLexerNextToken (pLexer);
  if (token->type != expectedType)
    {
      return NULL;
    }
  return token;
}

CypherParser *
cypherParserCreate (void)
{
  CypherParser *pParser
      = (CypherParser *)CYPHER_MALLOC (sizeof (CypherParser));
  if (!pParser)
    {
      return NULL;
    }

  pParser->pAst = NULL;
  pParser->zErrorMsg = NULL;
  return pParser;
}

void
cypherParserDestroy (CypherParser *pParser)
{
  if (pParser->pAst)
    {
      cypherAstDestroy (pParser->pAst);
    }
  if (pParser->zErrorMsg)
    {
      CYPHER_FREE (pParser->zErrorMsg);
    }
  CYPHER_FREE (pParser);
}

static void
parserSetError (CypherParser *pParser, CypherLexer *pLexer,
                const char *zFormat, ...)
{
  if (pParser->zErrorMsg)
    {
      CYPHER_FREE (pParser->zErrorMsg);
    }

  va_list args;
  va_start (args, zFormat);

  // Determine required size
  va_list args_copy;
  va_copy (args_copy, args);
  int size = vsnprintf (NULL, 0, zFormat, args_copy);
  va_end (args_copy);

  if (size < 0)
    {
      va_end (args);
      // Encoding error
      pParser->zErrorMsg = NULL;
      return;
    }

  pParser->zErrorMsg
      = (char *)CYPHER_MALLOC (size + 256); // Add extra space for token info
  if (!pParser->zErrorMsg)
    {
      va_end (args);
      // Allocation failed
      return;
    }

  vsnprintf (pParser->zErrorMsg, size + 1, zFormat, args);
  va_end (args);

  // Append token information
  if (pLexer && pLexer->pLastToken)
    {
      sprintf (pParser->zErrorMsg + size, " near '%.*s' at line %d column %d",
               pLexer->pLastToken->len, pLexer->pLastToken->text,
               pLexer->pLastToken->line, pLexer->pLastToken->column);
    }
}

CypherAst *
cypherParse (CypherParser *pParser, const char *zQuery, char **pzErrMsg)
{
  if (!zQuery)
    {
      if (pzErrMsg)
        *pzErrMsg = strdup ("Query string is NULL");
      return NULL;
    }
  CypherLexer *pLexer = cypherLexerCreate (zQuery);
  if (!pLexer)
    {
      if (pzErrMsg)
        *pzErrMsg = strdup ("Failed to create lexer");
      return NULL;
    }

  pParser->pAst = parseQuery (pLexer, pParser);

  if (pParser->zErrorMsg)
    {
      if (pzErrMsg)
        *pzErrMsg = strdup (pParser->zErrorMsg);
    }

  cypherLexerDestroy (pLexer);
  return pParser->pAst;
}

/*
** Validate a Cypher query without executing it.
** Returns SQLITE_OK if valid, error code with details if invalid.
** This function parses the query and performs basic semantic validation.
*/
int
cypherValidateQuery (const char *zQuery, char **pzError)
{
  if (!zQuery)
    {
      if (pzError)
        *pzError = sqlite3_mprintf ("Query string is NULL");
      return CYPHER_ERROR_SYNTAX_GENERAL;
    }

  CypherParser *pParser = cypherParserCreate ();
  if (!pParser)
    {
      if (pzError)
        *pzError = sqlite3_mprintf ("Failed to create parser");
      return SQLITE_NOMEM;
    }

  char *parseError = NULL;
  CypherAst *pAst = cypherParse (pParser, zQuery, &parseError);

  int result = SQLITE_OK;

  if (!pAst)
    {
      /* Parse error */
      result = CYPHER_ERROR_SYNTAX_GENERAL;
      if (pzError)
        {
          if (parseError)
            {
              *pzError = sqlite3_mprintf ("Syntax error: %s", parseError);
            }
          else
            {
              *pzError
                  = sqlite3_mprintf ("Syntax error: Invalid Cypher query");
            }
        }
    }
  else
    {
      /* Parsing succeeded, perform basic semantic validation */
      /* For now, we just check that we have a valid AST structure */
      /* TODO: Add more comprehensive semantic validation */

      if (cypherAstGetChildCount (pAst) == 0)
        {
          result = CYPHER_ERROR_SEMANTIC_UNDEFINED_VARIABLE;
          if (pzError)
            *pzError = sqlite3_mprintf ("Semantic error: Empty query");
        }

      /* Clean up AST */
      cypherAstDestroy (pAst);
    }

  if (parseError)
    {
      sqlite3_free (parseError);
    }

  cypherParserDestroy (pParser);
  return result;
}

static CypherAst *
parseQuery (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pQuery = cypherAstCreate (CYPHER_AST_QUERY, 0, 0);
  CypherAst *pSingleQuery = parseSingleQuery (pLexer, pParser);
  if (!pSingleQuery)
    {
      cypherAstDestroy (pQuery);
      return NULL;
    }
  cypherAstAddChild (pQuery, pSingleQuery);
  return pQuery;
}

static CypherAst *
parseSingleQuery (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pSingleQuery = cypherAstCreate (CYPHER_AST_SINGLE_QUERY, 0, 0);
  CypherAst *pClause = NULL;

  /* Parse the first clause - can be MATCH, CREATE, MERGE, SET, or DELETE */
  CypherToken *pPeek = parserPeekToken (pLexer);

  if (pPeek->type == CYPHER_TOK_MATCH)
    {
      pClause = parseMatchClause (pLexer, pParser);
    }
  else if (pPeek->type == CYPHER_TOK_CREATE)
    {
      pClause = parseCreateClause (pLexer, pParser);
    }
  else if (pPeek->type == CYPHER_TOK_MERGE)
    {
      pClause = parseMergeClause (pLexer, pParser);
    }
  else if (pPeek->type == CYPHER_TOK_SET)
    {
      pClause = parseSetClause (pLexer, pParser);
    }
  else if (pPeek->type == CYPHER_TOK_DELETE)
    {
      pClause = parseDeleteClause (pLexer, pParser);
    }
  else
    {
      parserSetError (pParser, pLexer,
                      "Expected MATCH, CREATE, MERGE, SET, or DELETE");
      cypherAstDestroy (pSingleQuery);
      return NULL;
    }

  if (!pClause)
    {
      cypherAstDestroy (pSingleQuery);
      return NULL;
    }
  cypherAstAddChild (pSingleQuery, pClause);

  /* Parse additional clauses: WHERE (after MATCH),
   * MATCH/CREATE/MERGE/SET/DELETE, then optional RETURN */
  for (;;)
    {
      pPeek = parserPeekToken (pLexer);
      if (!pPeek)
        break;

      if (pPeek->type == CYPHER_TOK_WHERE)
        {
          /* WHERE allowed typically after MATCH; accept and attach */
          CypherAst *pWhereClause = parseWhereClause (pLexer, pParser);
          if (pWhereClause)
            {
              cypherAstAddChild (pSingleQuery, pWhereClause);
              continue;
            }
          else
            {
              break;
            }
        }

      if (pPeek->type == CYPHER_TOK_MATCH)
        {
          CypherAst *pNext = parseMatchClause (pLexer, pParser);
          if (!pNext)
            break;
          cypherAstAddChild (pSingleQuery, pNext);
          continue;
        }
      if (pPeek->type == CYPHER_TOK_CREATE)
        {
          CypherAst *pNext = parseCreateClause (pLexer, pParser);
          if (!pNext)
            break;
          cypherAstAddChild (pSingleQuery, pNext);
          continue;
        }
      if (pPeek->type == CYPHER_TOK_MERGE)
        {
          CypherAst *pNext = parseMergeClause (pLexer, pParser);
          if (!pNext)
            break;
          cypherAstAddChild (pSingleQuery, pNext);
          continue;
        }
      if (pPeek->type == CYPHER_TOK_SET)
        {
          CypherAst *pNext = parseSetClause (pLexer, pParser);
          if (!pNext)
            break;
          cypherAstAddChild (pSingleQuery, pNext);
          continue;
        }
      if (pPeek->type == CYPHER_TOK_DELETE)
        {
          CypherAst *pNext = parseDeleteClause (pLexer, pParser);
          if (!pNext)
            break;
          cypherAstAddChild (pSingleQuery, pNext);
          continue;
        }

      /* Optional RETURN at the end */
      if (pPeek->type == CYPHER_TOK_RETURN)
        {
          CypherAst *pReturnClause = parseReturnClause (pLexer, pParser);
          if (!pReturnClause)
            {
              cypherAstDestroy (pSingleQuery);
              return NULL;
            }
          cypherAstAddChild (pSingleQuery, pReturnClause);
        }

      break;
    }

  /* Consume tokens until EOF */
  CypherToken *token;
  do
    {
      token = cypherLexerNextToken (pLexer);
    }
  while (token->type != CYPHER_TOK_EOF && token->type != CYPHER_TOK_ERROR);

  if (token->type == CYPHER_TOK_ERROR)
    {
      parserSetError (pParser, pLexer, "Syntax error");
      cypherAstDestroy (pSingleQuery);
      return NULL;
    }

  return pSingleQuery;
}

static CypherAst *
parseMatchClause (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_MATCH))
    {
      parserSetError (pParser, pLexer, "Expected MATCH");
      return NULL;
    }
  CypherAst *pMatchClause = cypherAstCreate (CYPHER_AST_MATCH, 0, 0);
  CypherAst *pPatternList = parsePatternList (pLexer, pParser);
  if (!pPatternList)
    {
      cypherAstDestroy (pMatchClause);
      return NULL;
    }
  cypherAstAddChild (pMatchClause, pPatternList);
  return pMatchClause;
}

static CypherAst *
parseCreateClause (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_CREATE))
    {
      parserSetError (pParser, pLexer, "Expected CREATE");
      return NULL;
    }
  CypherAst *pCreateClause = cypherAstCreate (CYPHER_AST_CREATE, 0, 0);
  CypherAst *pPatternList = parsePatternList (pLexer, pParser);
  if (!pPatternList)
    {
      cypherAstDestroy (pCreateClause);
      return NULL;
    }
  cypherAstAddChild (pCreateClause, pPatternList);
  return pCreateClause;
}

static CypherAst *
parseMergeClause (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_MERGE))
    {
      parserSetError (pParser, pLexer, "Expected MERGE");
      return NULL;
    }
  CypherAst *pMergeClause = cypherAstCreate (CYPHER_AST_MERGE, 0, 0);
  CypherAst *pPatternList = parsePatternList (pLexer, pParser);
  if (!pPatternList)
    {
      cypherAstDestroy (pMergeClause);
      return NULL;
    }
  /* Minimal validation: disallow parameter tokens in MERGE property maps */
  if (pLexer->pLastToken && pLexer->pLastToken->type == CYPHER_TOK_DOLLAR)
    {
      parserSetError (pParser, pLexer,
                      "Parameters are not allowed in MERGE property maps");
      cypherAstDestroy (pMergeClause);
      return NULL;
    }
  cypherAstAddChild (pMergeClause, pPatternList);
  /* Optional: ON CREATE SET / ON MATCH SET sections */
  for (;;)
    {
      CypherToken *pTok = parserPeekToken (pLexer);
      if (!pTok || pTok->type != CYPHER_TOK_ON)
        break;
      /* Consume ON */
      if (!parserConsumeToken (pLexer, CYPHER_TOK_ON))
        break;

      CypherAst *pOnNode = NULL;
      CypherToken *pKind = parserPeekToken (pLexer);
      if (!pKind)
        break;
      if (pKind->type == CYPHER_TOK_CREATE)
        {
          parserConsumeToken (pLexer, CYPHER_TOK_CREATE);
          /* Expect SET */
          if (!parserConsumeToken (pLexer, CYPHER_TOK_SET))
            {
              parserSetError (pParser, pLexer, "Expected SET after ON CREATE");
              cypherAstDestroy (pMergeClause);
              return NULL;
            }
          /* Parse assignment list: a.b = expr [, a.c = expr]* */
          CypherAst *pSetList = cypherAstCreate (CYPHER_AST_SET, 0, 0);
          if (!pSetList)
            {
              cypherAstDestroy (pMergeClause);
              return NULL;
            }
          /* Parse first assignment */
          {
            /* Left side: property expression (restrict to primary to avoid
             * consuming '=') */
            CypherAst *pLeft = parsePrimaryExpression (pLexer, pParser);
            if (!pLeft)
              {
                cypherAstDestroy (pSetList);
                cypherAstDestroy (pMergeClause);
                return NULL;
              }
            if (!parserConsumeToken (pLexer, CYPHER_TOK_EQ))
              {
                parserSetError (pParser, pLexer,
                                "Expected '=' in SET assignment");
                cypherAstDestroy (pSetList);
                cypherAstDestroy (pMergeClause);
                return NULL;
              }
            CypherAst *pRight = parseExpression (pLexer, pParser);
            if (!pRight)
              {
                cypherAstDestroy (pSetList);
                cypherAstDestroy (pMergeClause);
                return NULL;
              }
            /* Disallow parameters ($) on RHS in MERGE SET */
            if (pLexer->pLastToken
                && pLexer->pLastToken->type == CYPHER_TOK_DOLLAR)
              {
                parserSetError (pParser, pLexer,
                                "Parameters are not allowed in MERGE SET");
                cypherAstDestroy (pSetList);
                cypherAstDestroy (pMergeClause);
                return NULL;
              }
            CypherAst *pAssign
                = cypherAstCreateBinaryOp ("=", pLeft, pRight, 0, 0);
            cypherAstAddChild (pSetList, pAssign);
          }
          /* Parse additional assignments separated by commas */
          for (;;)
            {
              CypherToken *pSep = parserPeekToken (pLexer);
              if (!pSep || pSep->type != CYPHER_TOK_COMMA)
                break;
              parserConsumeToken (pLexer, CYPHER_TOK_COMMA);
              CypherAst *pLeft = parsePrimaryExpression (pLexer, pParser);
              if (!pLeft)
                {
                  cypherAstDestroy (pSetList);
                  cypherAstDestroy (pMergeClause);
                  return NULL;
                }
              if (!parserConsumeToken (pLexer, CYPHER_TOK_EQ))
                {
                  parserSetError (pParser, pLexer,
                                  "Expected '=' in SET assignment");
                  cypherAstDestroy (pSetList);
                  cypherAstDestroy (pMergeClause);
                  return NULL;
                }
              CypherAst *pRight = parseExpression (pLexer, pParser);
              if (!pRight)
                {
                  cypherAstDestroy (pSetList);
                  cypherAstDestroy (pMergeClause);
                  return NULL;
                }
              if (pLexer->pLastToken
                  && pLexer->pLastToken->type == CYPHER_TOK_DOLLAR)
                {
                  parserSetError (pParser, pLexer,
                                  "Parameters are not allowed in MERGE SET");
                  cypherAstDestroy (pSetList);
                  cypherAstDestroy (pMergeClause);
                  return NULL;
                }
              CypherAst *pAssign
                  = cypherAstCreateBinaryOp ("=", pLeft, pRight, 0, 0);
              cypherAstAddChild (pSetList, pAssign);
            }
          pOnNode = cypherAstCreate (CYPHER_AST_ON_CREATE, 0, 0);
          if (!pOnNode)
            {
              cypherAstDestroy (pSetList);
              cypherAstDestroy (pMergeClause);
              return NULL;
            }
          cypherAstAddChild (pOnNode, pSetList);
          cypherAstAddChild (pMergeClause, pOnNode);
        }
      else if (pKind->type == CYPHER_TOK_MATCH)
        {
          parserConsumeToken (pLexer, CYPHER_TOK_MATCH);
          if (!parserConsumeToken (pLexer, CYPHER_TOK_SET))
            {
              parserSetError (pParser, pLexer, "Expected SET after ON MATCH");
              cypherAstDestroy (pMergeClause);
              return NULL;
            }
          CypherAst *pSetList = cypherAstCreate (CYPHER_AST_SET, 0, 0);
          if (!pSetList)
            {
              cypherAstDestroy (pMergeClause);
              return NULL;
            }
          {
            CypherAst *pLeft = parsePrimaryExpression (pLexer, pParser);
            if (!pLeft)
              {
                cypherAstDestroy (pSetList);
                cypherAstDestroy (pMergeClause);
                return NULL;
              }
            if (!parserConsumeToken (pLexer, CYPHER_TOK_EQ))
              {
                parserSetError (pParser, pLexer,
                                "Expected '=' in SET assignment");
                cypherAstDestroy (pSetList);
                cypherAstDestroy (pMergeClause);
                return NULL;
              }
            CypherAst *pRight = parseExpression (pLexer, pParser);
            if (!pRight)
              {
                cypherAstDestroy (pSetList);
                cypherAstDestroy (pMergeClause);
                return NULL;
              }
            if (pLexer->pLastToken
                && pLexer->pLastToken->type == CYPHER_TOK_DOLLAR)
              {
                parserSetError (pParser, pLexer,
                                "Parameters are not allowed in MERGE SET");
                cypherAstDestroy (pSetList);
                cypherAstDestroy (pMergeClause);
                return NULL;
              }
            CypherAst *pAssign
                = cypherAstCreateBinaryOp ("=", pLeft, pRight, 0, 0);
            cypherAstAddChild (pSetList, pAssign);
          }
          for (;;)
            {
              CypherToken *pSep = parserPeekToken (pLexer);
              if (!pSep || pSep->type != CYPHER_TOK_COMMA)
                break;
              parserConsumeToken (pLexer, CYPHER_TOK_COMMA);
              CypherAst *pLeft = parsePrimaryExpression (pLexer, pParser);
              if (!pLeft)
                {
                  cypherAstDestroy (pSetList);
                  cypherAstDestroy (pMergeClause);
                  return NULL;
                }
              if (!parserConsumeToken (pLexer, CYPHER_TOK_EQ))
                {
                  parserSetError (pParser, pLexer,
                                  "Expected '=' in SET assignment");
                  cypherAstDestroy (pSetList);
                  cypherAstDestroy (pMergeClause);
                  return NULL;
                }
              CypherAst *pRight = parseExpression (pLexer, pParser);
              if (!pRight)
                {
                  cypherAstDestroy (pSetList);
                  cypherAstDestroy (pMergeClause);
                  return NULL;
                }
              if (pLexer->pLastToken
                  && pLexer->pLastToken->type == CYPHER_TOK_DOLLAR)
                {
                  parserSetError (pParser, pLexer,
                                  "Parameters are not allowed in MERGE SET");
                  cypherAstDestroy (pSetList);
                  cypherAstDestroy (pMergeClause);
                  return NULL;
                }
              CypherAst *pAssign
                  = cypherAstCreateBinaryOp ("=", pLeft, pRight, 0, 0);
              cypherAstAddChild (pSetList, pAssign);
            }
          pOnNode = cypherAstCreate (CYPHER_AST_ON_MATCH, 0, 0);
          if (!pOnNode)
            {
              cypherAstDestroy (pSetList);
              cypherAstDestroy (pMergeClause);
              return NULL;
            }
          cypherAstAddChild (pOnNode, pSetList);
          cypherAstAddChild (pMergeClause, pOnNode);
        }
      else
        {
          parserSetError (pParser, pLexer,
                          "Expected CREATE or MATCH after ON");
          cypherAstDestroy (pMergeClause);
          return NULL;
        }
    }
  return pMergeClause;
}

static CypherAst *
parseSetClause (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_SET))
    {
      parserSetError (pParser, pLexer, "Expected SET");
      return NULL;
    }
  CypherAst *pSetClause = cypherAstCreate (CYPHER_AST_SET, 0, 0);

  /* Parse SET items: variable.property = value */
  /* For now, parse as expression - full implementation would parse property
   * assignments */
  CypherAst *pExpr = parseExpression (pLexer, pParser);
  if (!pExpr)
    {
      cypherAstDestroy (pSetClause);
      return NULL;
    }
  cypherAstAddChild (pSetClause, pExpr);
  return pSetClause;
}

static CypherAst *
parseDeleteClause (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_DELETE))
    {
      parserSetError (pParser, pLexer, "Expected DELETE");
      return NULL;
    }
  CypherAst *pDeleteClause = cypherAstCreate (CYPHER_AST_DELETE, 0, 0);

  /* Parse identifiers or expressions to delete */
  CypherAst *pExpr = parseExpression (pLexer, pParser);
  if (!pExpr)
    {
      cypherAstDestroy (pDeleteClause);
      return NULL;
    }
  cypherAstAddChild (pDeleteClause, pExpr);
  return pDeleteClause;
}

static CypherAst *
parsePatternList (CypherLexer *pLexer, CypherParser *pParser)
{
  /* Pattern list: one or more pattern elements separated by commas */
  CypherAst *pPatternList = cypherAstCreate (CYPHER_AST_PATTERN, 0, 0);
  if (!pPatternList)
    return NULL;

  /* Parse first pattern */
  CypherAst *pPattern = parsePattern (pLexer, pParser);
  if (!pPattern)
    {
      cypherAstDestroy (pPatternList);
      return NULL;
    }
  cypherAstAddChild (pPatternList, pPattern);

  /* Parse subsequent patterns separated by commas */
  while (1)
    {
      CypherToken *pComma = parserPeekToken (pLexer);
      if (!pComma || pComma->type != CYPHER_TOK_COMMA)
        break;
      /* Consume comma and parse next pattern */
      parserConsumeToken (pLexer, CYPHER_TOK_COMMA);
      CypherAst *pNext = parsePattern (pLexer, pParser);
      if (!pNext)
        {
          cypherAstDestroy (pPatternList);
          return NULL;
        }
      cypherAstAddChild (pPatternList, pNext);
    }

  return pPatternList;
}

static CypherAst *
parsePattern (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pPattern = cypherAstCreate (CYPHER_AST_PATTERN, 0, 0);
  CypherAst *pNodePattern = parseNodePattern (pLexer, pParser);
  if (!pNodePattern)
    {
      cypherAstDestroy (pPattern);
      return NULL;
    }
  cypherAstAddChild (pPattern, pNodePattern);
  /* Optional single-hop relationship pattern: -[r:TYPE {..}]->(n) or
   * undirected */
  CypherToken *pTok = parserPeekToken (pLexer);
  if (!pTok)
    return pPattern;
  if (pTok->type == CYPHER_TOK_MINUS || pTok->type == CYPHER_TOK_ARROW_LEFT)
    {
      CypherAst *pRel = parseRelationshipPattern (pLexer, pParser);
      if (!pRel && pParser->zErrorMsg)
        {
          cypherAstDestroy (pPattern);
          return NULL;
        }
      if (pRel)
        {
          cypherAstAddChild (pPattern, pRel);
          CypherAst *pRight = parseNodePattern (pLexer, pParser);
          if (!pRight)
            {
              cypherAstDestroy (pPattern);
              return NULL;
            }
          cypherAstAddChild (pPattern, pRight);
        }
    }
  return pPattern;
}

static CypherAst *
parseNodePattern (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_LPAREN))
    {
      parserSetError (pParser, pLexer, "Expected (");
      return NULL;
    }
  CypherAst *pNodePattern = cypherAstCreate (CYPHER_AST_NODE_PATTERN, 0, 0);
  CypherToken *pId = parserConsumeToken (pLexer, CYPHER_TOK_IDENTIFIER);
  if (pId)
    {
      cypherAstAddChild (pNodePattern, cypherAstCreateIdentifier (
                                           pId->text, pId->line, pId->column));
    }

  CypherAst *pLabels = parseNodeLabels (pLexer, pParser);
  if (pLabels)
    {
      cypherAstAddChild (pNodePattern, pLabels);
    }
  else if (pParser->zErrorMsg)
    {
      cypherAstDestroy (pNodePattern);
      return NULL;
    }

  // Check for property map
  CypherToken *pBrace = parserPeekToken (pLexer);
  if (pBrace && pBrace->type == CYPHER_TOK_LBRACE)
    {
      CypherAst *pProperties = parsePropertyMap (pLexer, pParser);
      if (pProperties)
        {
          cypherAstAddChild (pNodePattern, pProperties);
        }
    }

  if (!parserConsumeToken (pLexer, CYPHER_TOK_RPAREN))
    {
      parserSetError (pParser, pLexer, "Expected )");
      cypherAstDestroy (pNodePattern);
      return NULL;
    }
  return pNodePattern;
}

static CypherAst *
parseNodeLabels (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherToken *pColon = parserPeekToken (pLexer);
  if (!pColon || pColon->type != CYPHER_TOK_COLON)
    {
      return NULL;
    }
  parserConsumeToken (pLexer, CYPHER_TOK_COLON);

  CypherToken *pLabel = parserConsumeToken (pLexer, CYPHER_TOK_IDENTIFIER);
  if (!pLabel)
    {
      parserSetError (pParser, pLexer, "Expected node label after ':'");
      return NULL;
    }

  /* Create null-terminated copy of label text using token length */
  char *zLabel = sqlite3_mprintf ("%.*s", pLabel->len, pLabel->text);
  if (!zLabel)
    {
      parserSetError (pParser, pLexer, "Out of memory");
      return NULL;
    }

  CypherAst *pAst
      = cypherAstCreateNodeLabel (zLabel, pLabel->line, pLabel->column);
  sqlite3_free (zLabel);
  return pAst;
}

static CypherAst *
parsePropertyMap (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_LBRACE))
    {
      return NULL;
    }

  CypherAst *pMap = cypherAstCreate (CYPHER_AST_MAP, 0, 0);

  // Handle empty map
  CypherToken *pRBrace = parserPeekToken (pLexer);
  if (pRBrace && pRBrace->type == CYPHER_TOK_RBRACE)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_RBRACE);
      return pMap;
    }

  // Parse property pairs
  do
    {
      // Parse key (identifier)
      CypherToken *pKey = parserConsumeToken (pLexer, CYPHER_TOK_IDENTIFIER);
      if (!pKey)
        {
          parserSetError (pParser, pLexer, "Expected property name");
          cypherAstDestroy (pMap);
          return NULL;
        }

      /* Copy key text immediately to avoid token reuse freeing it */
      char *zKeyCopy = sqlite3_mprintf ("%.*s", pKey->len, pKey->text);
      if (!zKeyCopy)
        {
          parserSetError (pParser, pLexer, "Out of memory");
          cypherAstDestroy (pMap);
          return NULL;
        }

      // Parse colon
      if (!parserConsumeToken (pLexer, CYPHER_TOK_COLON))
        {
          parserSetError (pParser, pLexer, "Expected ':' after property name");
          cypherAstDestroy (pMap);
          sqlite3_free (zKeyCopy);
          return NULL;
        }

      // Parse value - support expressions
      CypherAst *pValue = parseExpression (pLexer, pParser);
      if (!pValue)
        {
          parserSetError (pParser, pLexer,
                          "Expected property value expression");
          cypherAstDestroy (pMap);
          sqlite3_free (zKeyCopy);
          return NULL;
        }

      // Create property pair
      CypherAst *pPair = cypherAstCreate (CYPHER_AST_PROPERTY_PAIR, pKey->line,
                                          pKey->column);
      cypherAstSetValue (pPair, zKeyCopy);
      sqlite3_free (zKeyCopy);
      cypherAstAddChild (pPair, pValue);
      cypherAstAddChild (pMap, pPair);

      // Check for comma
      CypherToken *pComma = parserPeekToken (pLexer);
      if (pComma && pComma->type == CYPHER_TOK_COMMA)
        {
          parserConsumeToken (pLexer, CYPHER_TOK_COMMA);
        }
      else
        {
          break;
        }
    }
  while (1);

  if (!parserConsumeToken (pLexer, CYPHER_TOK_RBRACE))
    {
      parserSetError (pParser, pLexer, "Expected '}' to close property map");
      cypherAstDestroy (pMap);
      return NULL;
    }

  return pMap;
}

static CypherAst *
parseRelationshipPattern (CypherLexer *pLexer, CypherParser *pParser)
{
  /* Parse left connector: '-' or '<-' */
  int dirLeft = 0; /* -1 for '<-', 0 for '-', +1 to be decided by right part */
  CypherToken *pFirst = parserPeekToken (pLexer);
  if (!pFirst)
    return NULL;
  if (pFirst->type == CYPHER_TOK_ARROW_LEFT)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_ARROW_LEFT);
      dirLeft = -1;
    }
  else if (pFirst->type == CYPHER_TOK_MINUS)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_MINUS);
      dirLeft = 0;
    }
  else
    {
      return NULL; /* No relationship begins here */
    }

  /* Expect relationship bracket */
  if (!parserConsumeToken (pLexer, CYPHER_TOK_LBRACKET))
    {
      parserSetError (pParser, pLexer,
                      "Expected '[' to start relationship pattern");
      return NULL;
    }

  CypherAst *pRelPattern = cypherAstCreate (CYPHER_AST_REL_PATTERN, 0, 0);
  if (!pRelPattern)
    return NULL;

  /* Inside [ ... ]: optional variable, optional :TYPE, optional {props} */
  CypherToken *pTok = parserPeekToken (pLexer);
  /* Capture variable name safely (copy text now to avoid UAF when tokens advance) */
  char *zVarCopy = NULL; int varLine = 0; int varColumn = 0;
  if (pTok && pTok->type == CYPHER_TOK_IDENTIFIER)
    {
      /* Tentatively treat as variable; if ':' follows, we still keep it as var */
      CypherToken *pVarTok = cypherLexerNextToken (pLexer);
      if (pVarTok && pVarTok->text)
        {
          zVarCopy = sqlite3_mprintf ("%s", pVarTok->text);
          varLine = pVarTok->line;
          varColumn = pVarTok->column;
        }
    }

  /* Optional :TYPE */
  pTok = parserPeekToken (pLexer);
  if (pTok && pTok->type == CYPHER_TOK_COLON)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_COLON);
      CypherToken *pType = parserConsumeToken (pLexer, CYPHER_TOK_IDENTIFIER);
      if (!pType)
        {
          parserSetError (pParser, pLexer,
                          "Expected relationship type after ':'");
          cypherAstDestroy (pRelPattern);
          return NULL;
        }
      /* Store type as first child so planner can find it easily */
      cypherAstAddChild (
          pRelPattern,
          cypherAstCreateIdentifier (pType->text, pType->line, pType->column));
    }

  /* Optional properties map inside relationship */
  pTok = parserPeekToken (pLexer);
  if (pTok && pTok->type == CYPHER_TOK_LBRACE)
    {
      CypherAst *pProps = parsePropertyMap (pLexer, pParser);
      if (!pProps)
        {
          cypherAstDestroy (pRelPattern);
          return NULL;
        }
      cypherAstAddChild (pRelPattern, pProps);
    }

  /* If we captured a variable and did NOT add it yet, add it after type so
   * type remains first */
  if (zVarCopy)
    {
      CypherAst *pId = cypherAstCreateIdentifier (zVarCopy, varLine, varColumn);
      cypherAstAddChild (pRelPattern, pId);
      sqlite3_free (zVarCopy);
      zVarCopy = NULL;
    }

  if (!parserConsumeToken (pLexer, CYPHER_TOK_RBRACKET))
    {
      parserSetError (pParser, pLexer,
                      "Expected ']' to close relationship pattern");
      cypherAstDestroy (pRelPattern);
      return NULL;
    }

  /* Parse right connector: '->' or '-' (if left was '<-') */
  CypherToken *pAfter = parserPeekToken (pLexer);
  const char *zDir = "-"; /* Default undirected */
  if (pAfter && pAfter->type == CYPHER_TOK_ARROW_RIGHT)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_ARROW_RIGHT);
      zDir = "->";
    }
  else if (pAfter && pAfter->type == CYPHER_TOK_MINUS)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_MINUS);
      zDir = "-";
    }
  else
    {
      /* If left consumed '<-', we may have already got direction; otherwise
       * error */
      if (dirLeft == -1)
        {
          zDir = "<-";
        }
      else
        {
          parserSetError (pParser, pLexer,
                          "Expected '-' or '->' after relationship pattern");
          cypherAstDestroy (pRelPattern);
          return NULL;
        }
    }

  /* If initial was '<-', direction is '<-' regardless of trailing '-' */
  if (dirLeft == -1)
    zDir = "<-";
  cypherAstSetValue (pRelPattern, zDir);
  return pRelPattern;
}

static CypherAst *
parseWhereClause (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_WHERE))
    {
      return NULL;
    }
  CypherAst *pWhereClause = cypherAstCreate (CYPHER_AST_WHERE, 0, 0);
  CypherAst *pExpr = parseExpression (pLexer, pParser);
  if (!pExpr)
    {
      cypherAstDestroy (pWhereClause);
      return NULL;
    }
  cypherAstAddChild (pWhereClause, pExpr);
  return pWhereClause;
}

static CypherAst *
parseReturnClause (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_RETURN))
    {
      parserSetError (pParser, pLexer, "Expected RETURN");
      return NULL;
    }
  CypherAst *pReturnClause = cypherAstCreate (CYPHER_AST_RETURN, 0, 0);
  CypherAst *pProjectionList = parseProjectionList (pLexer, pParser);
  if (!pProjectionList)
    {
      cypherAstDestroy (pReturnClause);
      return NULL;
    }
  cypherAstAddChild (pReturnClause, pProjectionList);
  return pReturnClause;
}

static CypherAst *
parseProjectionList (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pProjectionList
      = cypherAstCreate (CYPHER_AST_PROJECTION_LIST, 0, 0);
  CypherAst *pProjectionItem = parseProjectionItem (pLexer, pParser);
  if (!pProjectionItem)
    {
      cypherAstDestroy (pProjectionList);
      return NULL;
    }
  cypherAstAddChild (pProjectionList, pProjectionItem);
  return pProjectionList;
}

static CypherAst *
parseProjectionItem (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pProjectionItem
      = cypherAstCreate (CYPHER_AST_PROJECTION_ITEM, 0, 0);
  CypherAst *pExpr = parseExpression (pLexer, pParser);
  if (!pExpr)
    {
      cypherAstDestroy (pProjectionItem);
      return NULL;
    }
  cypherAstAddChild (pProjectionItem, pExpr);
  return pProjectionItem;
}

static CypherAst *
parseExpression (CypherLexer *pLexer, CypherParser *pParser)
{
  return parseOrExpression (pLexer, pParser);
}

static CypherAst *
parseOrExpression (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pLeft = parseAndExpression (pLexer, pParser);
  if (!pLeft)
    return NULL;

  while (parserPeekToken (pLexer)->type == CYPHER_TOK_OR)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_OR);
      CypherAst *pRight = parseAndExpression (pLexer, pParser);
      if (!pRight)
        {
          cypherAstDestroy (pLeft);
          parserSetError (pParser, pLexer, "Expected expression after OR");
          return NULL;
        }
      CypherAst *pOrExpr = cypherAstCreateBinaryOp ("OR", pLeft, pRight, 0, 0);
      cypherAstAddChild (pOrExpr, pLeft);
      cypherAstAddChild (pOrExpr, pRight);
      pLeft = pOrExpr;
    }
  return pLeft;
}

static CypherAst *
parseAndExpression (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pLeft = parseNotExpression (pLexer, pParser);
  if (!pLeft)
    return NULL;

  while (parserPeekToken (pLexer)->type == CYPHER_TOK_AND)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_AND);
      CypherAst *pRight = parseNotExpression (pLexer, pParser);
      if (!pRight)
        {
          cypherAstDestroy (pLeft);
          parserSetError (pParser, pLexer, "Expected expression after AND");
          return NULL;
        }
      CypherAst *pAndExpr = cypherAstCreate (CYPHER_AST_AND, 0, 0);
      cypherAstAddChild (pAndExpr, pLeft);
      cypherAstAddChild (pAndExpr, pRight);
      pLeft = pAndExpr;
    }
  return pLeft;
}

static CypherAst *
parseNotExpression (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherToken *pToken = parserPeekToken (pLexer);
  if (pToken->type == CYPHER_TOK_NOT)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_NOT);
      CypherAst *pExpr = parseNotExpression (pLexer, pParser);
      if (!pExpr)
        {
          parserSetError (pParser, pLexer, "Expected expression after NOT");
          return NULL;
        }
      CypherAst *pNotExpr = cypherAstCreate (CYPHER_AST_NOT, 0, 0);
      cypherAstAddChild (pNotExpr, pExpr);
      return pNotExpr;
    }
  return parseComparisonExpression (pLexer, pParser);
}

static CypherAst *
parseComparisonExpression (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pLeft = parseAdditiveExpression (pLexer, pParser);
  if (!pLeft)
    return NULL;

  CypherToken *pToken = parserPeekToken (pLexer);
  while (pToken->type == CYPHER_TOK_EQ || pToken->type == CYPHER_TOK_NE
         || pToken->type == CYPHER_TOK_LT || pToken->type == CYPHER_TOK_LE
         || pToken->type == CYPHER_TOK_GT || pToken->type == CYPHER_TOK_GE
         || pToken->type == CYPHER_TOK_STARTS_WITH
         || pToken->type == CYPHER_TOK_ENDS_WITH
         || pToken->type == CYPHER_TOK_CONTAINS
         || pToken->type == CYPHER_TOK_IN)
    {
      parserConsumeToken (pLexer, pToken->type);
      CypherAst *pRight = parseAdditiveExpression (pLexer, pParser);
      if (!pRight)
        {
          cypherAstDestroy (pLeft);
          parserSetError (pParser, pLexer,
                          "Expected expression after comparison operator");
          return NULL;
        }
      CypherAst *pCompExpr = cypherAstCreate (CYPHER_AST_COMPARISON, 0, 0);
      cypherAstSetValue (pCompExpr, pToken->text);
      cypherAstAddChild (pCompExpr, pLeft);
      cypherAstAddChild (pCompExpr, pRight);
      pLeft = pCompExpr;
      pToken = parserPeekToken (pLexer);
    }
  return pLeft;
}

static CypherAst *
parseAdditiveExpression (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pLeft = parseMultiplicativeExpression (pLexer, pParser);
  if (!pLeft)
    return NULL;

  CypherToken *pToken = parserPeekToken (pLexer);
  while (pToken->type == CYPHER_TOK_PLUS || pToken->type == CYPHER_TOK_MINUS)
    {
      parserConsumeToken (pLexer, pToken->type);
      CypherAst *pRight = parseMultiplicativeExpression (pLexer, pParser);
      if (!pRight)
        {
          cypherAstDestroy (pLeft);
          parserSetError (pParser, pLexer,
                          "Expected expression after additive operator");
          return NULL;
        }
      CypherAst *pAddExpr = cypherAstCreate (CYPHER_AST_ADDITIVE, 0, 0);
      cypherAstSetValue (pAddExpr, pToken->text);
      cypherAstAddChild (pAddExpr, pLeft);
      cypherAstAddChild (pAddExpr, pRight);
      pLeft = pAddExpr;
      pToken = parserPeekToken (pLexer);
    }
  return pLeft;
}

static CypherAst *
parseMultiplicativeExpression (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pLeft = parseUnaryExpression (pLexer, pParser);
  if (!pLeft)
    return NULL;

  CypherToken *pToken = parserPeekToken (pLexer);
  while (pToken->type == CYPHER_TOK_MULT || pToken->type == CYPHER_TOK_DIV
         || pToken->type == CYPHER_TOK_MOD)
    {
      parserConsumeToken (pLexer, pToken->type);
      CypherAst *pRight = parseUnaryExpression (pLexer, pParser);
      if (!pRight)
        {
          cypherAstDestroy (pLeft);
          parserSetError (pParser, pLexer,
                          "Expected expression after multiplicative operator");
          return NULL;
        }
      CypherAst *pMulExpr = cypherAstCreate (CYPHER_AST_MULTIPLICATIVE, 0, 0);
      cypherAstSetValue (pMulExpr, pToken->text);
      cypherAstAddChild (pMulExpr, pLeft);
      cypherAstAddChild (pMulExpr, pRight);
      pLeft = pMulExpr;
      pToken = parserPeekToken (pLexer);
    }
  return pLeft;
}

static CypherAst *
parseUnaryExpression (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherToken *pToken = parserPeekToken (pLexer);
  if (pToken->type == CYPHER_TOK_PLUS || pToken->type == CYPHER_TOK_MINUS)
    {
      parserConsumeToken (pLexer, pToken->type);
      CypherAst *pExpr = parseUnaryExpression (pLexer, pParser);
      if (!pExpr)
        {
          parserSetError (pParser, pLexer,
                          "Expected expression after unary operator");
          return NULL;
        }
      CypherAst *pUnaryExpr
          = cypherAstCreateUnaryOp (pToken->text, pExpr, 0, 0);
      cypherAstSetValue (pUnaryExpr, pToken->text);
      cypherAstAddChild (pUnaryExpr, pExpr);
      return pUnaryExpr;
    }
  return parsePrimaryExpression (pLexer, pParser);
}

static CypherAst *
parsePrimaryExpression (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherAst *pExpr = parseLiteral (pLexer, pParser);
  if (pExpr)
    {
      // Check if this is a function call (identifier followed by parentheses)
      if (pExpr->type == CYPHER_AST_IDENTIFIER
          && parserPeekToken (pLexer)->type == CYPHER_TOK_LPAREN)
        {
          CypherAst *pFunctionCall
              = parseFunctionCall (pLexer, pParser, pExpr);
          if (pFunctionCall)
            {
              return parsePropertyExpression (pLexer, pParser, pFunctionCall);
            }
        }
      return parsePropertyExpression (pLexer, pParser, pExpr);
    }
  return NULL;
}

static CypherAst *
parsePropertyExpression (CypherLexer *pLexer, CypherParser *pParser,
                         CypherAst *pExpr)
{
  CypherToken *pToken = parserPeekToken (pLexer);
  while (pToken->type == CYPHER_TOK_DOT)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_DOT);
      CypherToken *pProperty
          = parserConsumeToken (pLexer, CYPHER_TOK_IDENTIFIER);
      if (!pProperty)
        {
          cypherAstDestroy (pExpr);
          parserSetError (pParser, pLexer, "Expected property name after '.'");
          return NULL;
        }
      CypherAst *pPropExpr = cypherAstCreate (CYPHER_AST_PROPERTY, 0, 0);
      cypherAstAddChild (pPropExpr, pExpr);
      cypherAstAddChild (
          pPropExpr, cypherAstCreateIdentifier (
                         pProperty->text, pProperty->line, pProperty->column));
      pExpr = pPropExpr;
      pToken = parserPeekToken (pLexer);
    }
  return pExpr;
}

static CypherAst *
parseLiteral (CypherLexer *pLexer, CypherParser *pParser)
{
  CypherToken *pToken = parserPeekToken (pLexer);

  // Handle list literals [1, 2, 3]
  if (pToken->type == CYPHER_TOK_LBRACKET)
    {
      return parseListLiteral (pLexer, pParser);
    }

  // Handle map literals {key: value}
  if (pToken->type == CYPHER_TOK_LBRACE)
    {
      return parseMapLiteral (pLexer, pParser);
    }

  // Handle parenthesized expressions
  if (pToken->type == CYPHER_TOK_LPAREN)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_LPAREN);
      CypherAst *pExpr = parseExpression (pLexer, pParser);
      if (!pExpr)
        return NULL;
      if (!parserConsumeToken (pLexer, CYPHER_TOK_RPAREN))
        {
          cypherAstDestroy (pExpr);
          parserSetError (pParser, pLexer, "Expected closing parenthesis");
          return NULL;
        }
      return pExpr;
    }

  // Handle identifiers separately from literals
  if (pToken->type == CYPHER_TOK_IDENTIFIER)
    {
      pToken = cypherLexerNextToken (pLexer);
      return cypherAstCreateIdentifier (pToken->text, pToken->line,
                                        pToken->column);
    }

  // Handle basic literals
  if (pToken->type == CYPHER_TOK_INTEGER || pToken->type == CYPHER_TOK_FLOAT
      || pToken->type == CYPHER_TOK_STRING
      || pToken->type == CYPHER_TOK_BOOLEAN || pToken->type == CYPHER_TOK_NULL)
    {
      pToken = cypherLexerNextToken (pLexer);
      return cypherAstCreateLiteral (pToken->text, pToken->line,
                                     pToken->column);
    }

  return NULL;
}

static CypherAst *
parseListLiteral (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_LBRACKET))
    {
      return NULL;
    }

  CypherAst *pList = cypherAstCreate (CYPHER_AST_ARRAY, 0, 0);

  // Handle empty list []
  CypherToken *pToken = parserPeekToken (pLexer);
  if (pToken->type == CYPHER_TOK_RBRACKET)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_RBRACKET);
      return pList;
    }

  // Parse list elements
  do
    {
      CypherAst *pElement = parseExpression (pLexer, pParser);
      if (!pElement)
        {
          cypherAstDestroy (pList);
          parserSetError (pParser, pLexer, "Expected expression in list");
          return NULL;
        }
      cypherAstAddChild (pList, pElement);

      pToken = parserPeekToken (pLexer);
      if (pToken->type == CYPHER_TOK_COMMA)
        {
          parserConsumeToken (pLexer, CYPHER_TOK_COMMA);
        }
      else
        {
          break;
        }
    }
  while (1);

  if (!parserConsumeToken (pLexer, CYPHER_TOK_RBRACKET))
    {
      cypherAstDestroy (pList);
      parserSetError (pParser, pLexer, "Expected closing bracket");
      return NULL;
    }

  return pList;
}

static CypherAst *
parseMapLiteral (CypherLexer *pLexer, CypherParser *pParser)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_LBRACE))
    {
      return NULL;
    }

  CypherAst *pMap = cypherAstCreate (CYPHER_AST_OBJECT, 0, 0);

  // Handle empty map {}
  CypherToken *pToken = parserPeekToken (pLexer);
  if (pToken->type == CYPHER_TOK_RBRACE)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_RBRACE);
      return pMap;
    }

  // Parse key-value pairs
  do
    {
      // Parse key (identifier or string)
      pToken = parserPeekToken (pLexer);
      if (pToken->type != CYPHER_TOK_IDENTIFIER
          && pToken->type != CYPHER_TOK_STRING)
        {
          cypherAstDestroy (pMap);
          parserSetError (pParser, pLexer, "Expected property name");
          return NULL;
        }
      pToken = cypherLexerNextToken (pLexer);
      CypherAst *pKey = cypherAstCreateLiteral (pToken->text, pToken->line,
                                                pToken->column);

      // Expect colon
      if (!parserConsumeToken (pLexer, CYPHER_TOK_COLON))
        {
          cypherAstDestroy (pMap);
          cypherAstDestroy (pKey);
          parserSetError (pParser, pLexer,
                          "Expected colon after property name");
          return NULL;
        }

      // Parse value
      CypherAst *pValue = parseExpression (pLexer, pParser);
      if (!pValue)
        {
          cypherAstDestroy (pMap);
          cypherAstDestroy (pKey);
          parserSetError (pParser, pLexer, "Expected expression after colon");
          return NULL;
        }

      // Create property pair
      CypherAst *pPair = cypherAstCreate (CYPHER_AST_PROPERTY_PAIR, 0, 0);
      cypherAstAddChild (pPair, pKey);
      cypherAstAddChild (pPair, pValue);
      cypherAstAddChild (pMap, pPair);

      pToken = parserPeekToken (pLexer);
      if (pToken->type == CYPHER_TOK_COMMA)
        {
          parserConsumeToken (pLexer, CYPHER_TOK_COMMA);
        }
      else
        {
          break;
        }
    }
  while (1);

  if (!parserConsumeToken (pLexer, CYPHER_TOK_RBRACE))
    {
      cypherAstDestroy (pMap);
      parserSetError (pParser, pLexer, "Expected closing brace");
      return NULL;
    }

  return pMap;
}

static CypherAst *
parseFunctionCall (CypherLexer *pLexer, CypherParser *pParser,
                   CypherAst *pFunctionName)
{
  if (!parserConsumeToken (pLexer, CYPHER_TOK_LPAREN))
    {
      return NULL;
    }

  CypherAst *pFunctionCall = cypherAstCreate (CYPHER_AST_FUNCTION_CALL, 0, 0);
  cypherAstAddChild (pFunctionCall, pFunctionName);

  // Handle empty function call func()
  CypherToken *pToken = parserPeekToken (pLexer);
  if (pToken->type == CYPHER_TOK_RPAREN)
    {
      parserConsumeToken (pLexer, CYPHER_TOK_RPAREN);
      return pFunctionCall;
    }

  // Parse function arguments
  do
    {
      CypherAst *pArg = parseExpression (pLexer, pParser);
      if (!pArg)
        {
          cypherAstDestroy (pFunctionCall);
          parserSetError (pParser, pLexer,
                          "Expected expression in function call");
          return NULL;
        }
      cypherAstAddChild (pFunctionCall, pArg);

      pToken = parserPeekToken (pLexer);
      if (pToken->type == CYPHER_TOK_COMMA)
        {
          parserConsumeToken (pLexer, CYPHER_TOK_COMMA);
        }
      else
        {
          break;
        }
    }
  while (1);

  if (!parserConsumeToken (pLexer, CYPHER_TOK_RPAREN))
    {
      cypherAstDestroy (pFunctionCall);
      parserSetError (pParser, pLexer, "Expected closing parenthesis");
      return NULL;
    }

  return pFunctionCall;
}