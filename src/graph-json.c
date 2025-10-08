/*
** SQLite Graph Database Extension - JSON Property Handling
**
** This file implements JSON property validation and manipulation
** for graph nodes and edges. Uses SQLite's built-in JSON functions
** where possible for consistency.
**
** Functions: JSON validation, property extraction, property updates
** Integration: Leverages SQLite JSON1 extension functionality
*/

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "graph.h"
#include "graph-memory.h"
#include <string.h>

/* JSON property handling implementation */

/*
** Validate JSON string format.
** Returns SQLITE_OK if valid JSON, SQLITE_ERROR if invalid.
** Uses basic validation - can be enhanced with full JSON parser.
*/
int graphValidateJSON(const char *zJson){
  /* Basic validation - check for balanced braces */
  if( zJson==0 || zJson[0]==0 ){
    return SQLITE_ERROR;
  }
  
  /* Basic JSON validation */
  /* Check for basic JSON structure - starts with { or [ and ends with } or ] */
  int len = strlen(zJson);
  if (len >= 2) {
    if ((zJson[0] == '{' && zJson[len-1] == '}') ||
        (zJson[0] == '[' && zJson[len-1] == ']')) {
      /* Basic structure is valid */
      /* Full JSON parsing would validate entire structure */
      return SQLITE_OK;
    }
  }
  return SQLITE_ERROR;
}

/*
** Extract property value from JSON object.
** Returns allocated string that caller must sqlite3_free().
** Uses simple string search - can be enhanced with JSON parser.
*/
int graphGetJSONProperty(const char *zJson, const char *zKey, 
                        char **pzValue){
  /* Basic JSON property extraction */
  /* This is a simplified implementation for basic key-value pairs */
  char *zPattern;
  char *zPos;
  char *zEnd;
  int nLen;
  
  if (!zJson || !zKey || !pzValue) return SQLITE_MISUSE;
  
  *pzValue = NULL;
  
  /* Search for "key":"value" pattern */
  zPattern = sqlite3_mprintf("\"%s\":", zKey);
  if (!zPattern) return SQLITE_NOMEM;
  
  zPos = strstr(zJson, zPattern);
  sqlite3_free(zPattern);
  
  if (!zPos) return SQLITE_NOTFOUND;
  
  /* Skip past key and colon */
  zPos += strlen(zKey) + 3;
  
  /* Skip whitespace */
  while (*zPos == ' ' || *zPos == '\t') zPos++;
  
  /* Extract value */
  if (*zPos == '"') {
    /* String value */
    zPos++;
    zEnd = strchr(zPos, '"');
    if (zEnd) {
      nLen = zEnd - zPos;
      *pzValue = sqlite3_malloc(nLen + 1);
      if (*pzValue) {
        memcpy(*pzValue, zPos, nLen);
        (*pzValue)[nLen] = '\0';
        return SQLITE_OK;
      }
      return SQLITE_NOMEM;
    }
  }
  
  return SQLITE_NOTFOUND;
}