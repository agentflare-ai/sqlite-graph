/* JSON parsing and serialization for Cypher values */

#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include "cypher-executor.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
** Parse JSON properties string and populate a CypherValue map.
** Input: zJson - JSON string like '{"name": "John", "age": 30}'
** Output: pResult - CypherValue initialized as a map
** Returns: SQLITE_OK on success, error code on failure
*/
int cypherParseJsonProperties(const char *zJson, CypherValue *pResult) {
    const char *p;
    char **azKeys = NULL;
    CypherValue *apValues = NULL;
    int nPairs = 0;
    int nAllocated = 0;
    int rc = SQLITE_OK;
    
    if( !zJson || !pResult ) return SQLITE_MISUSE;
    
    /* Initialize result as null first */
    cypherValueInit(pResult);
    
    /* Skip whitespace */
    p = zJson;
    while( isspace(*p) ) p++;
    
    /* Handle null or empty input */
    if( *p == '\0' || strcmp(p, "null") == 0 ) {
        cypherValueSetNull(pResult);
        return SQLITE_OK;
    }
    
    /* Must start with { */
    if( *p != '{' ) {
        return SQLITE_FORMAT;
    }
    p++;
    
    /* Skip whitespace after { */
    while( isspace(*p) ) p++;
    
    /* Handle empty object */
    if( *p == '}' ) {
        cypherValueSetMap(pResult, NULL, NULL, 0);
        return SQLITE_OK;
    }
    
    /* Parse key-value pairs */
    while( *p != '}' && *p != '\0' ) {
        char *zKey = NULL;
        CypherValue value;
        const char *keyStart, *keyEnd;
        const char *valueStart, *valueEnd;
        
        /* Skip whitespace */
        while( isspace(*p) ) p++;
        
        /* Parse key (must be quoted string) */
        if( *p != '"' ) {
            rc = SQLITE_FORMAT;
            goto parse_error;
        }
        p++; /* Skip opening quote */
        keyStart = p;
        
        /* Find end of key */
        while( *p != '"' && *p != '\0' ) {
            if( *p == '\\' ) p++; /* Skip escaped character */
            if( *p != '\0' ) p++;
        }
        
        if( *p != '"' ) {
            rc = SQLITE_FORMAT;
            goto parse_error;
        }
        keyEnd = p;
        p++; /* Skip closing quote */
        
        /* Extract key */
        int keyLen = keyEnd - keyStart;
        zKey = sqlite3_malloc(keyLen + 1);
        if( !zKey ) {
            rc = SQLITE_NOMEM;
            goto parse_error;
        }
        memcpy(zKey, keyStart, keyLen);
        zKey[keyLen] = '\0';
        
        /* Skip whitespace and expect : */
        while( isspace(*p) ) p++;
        if( *p != ':' ) {
            sqlite3_free(zKey);
            rc = SQLITE_FORMAT;
            goto parse_error;
        }
        p++;
        while( isspace(*p) ) p++;
        
        /* Parse value */
        cypherValueInit(&value);
        valueStart = p;
        
        if( *p == '"' ) {
            /* String value */
            p++; /* Skip opening quote */
            valueStart = p;
            while( *p != '"' && *p != '\0' ) {
                if( *p == '\\' ) p++; /* Skip escaped character */
                if( *p != '\0' ) p++;
            }
            if( *p != '"' ) {
                sqlite3_free(zKey);
                rc = SQLITE_FORMAT;
                goto parse_error;
            }
            valueEnd = p;
            p++; /* Skip closing quote */
            
            /* Extract string value */
            int valueLen = valueEnd - valueStart;
            char *zValue = sqlite3_malloc(valueLen + 1);
            if( !zValue ) {
                sqlite3_free(zKey);
                rc = SQLITE_NOMEM;
                goto parse_error;
            }
            memcpy(zValue, valueStart, valueLen);
            zValue[valueLen] = '\0';
            
            cypherValueSetString(&value, zValue);
            sqlite3_free(zValue);
            
        } else if( isdigit(*p) || *p == '-' || *p == '+' ) {
            /* Numeric value */
            valueStart = p;
            if( *p == '-' || *p == '+' ) p++;
            while( isdigit(*p) ) p++;
            
            if( *p == '.' ) {
                /* Float */
                p++;
                while( isdigit(*p) ) p++;
                valueEnd = p;
                
                int valueLen = valueEnd - valueStart;
                char *zValue = sqlite3_malloc(valueLen + 1);
                if( !zValue ) {
                    sqlite3_free(zKey);
                    rc = SQLITE_NOMEM;
                    goto parse_error;
                }
                memcpy(zValue, valueStart, valueLen);
                zValue[valueLen] = '\0';
                
                double rValue = atof(zValue);
                cypherValueSetFloat(&value, rValue);
                sqlite3_free(zValue);
                
            } else {
                /* Integer */
                valueEnd = p;
                
                int valueLen = valueEnd - valueStart;
                char *zValue = sqlite3_malloc(valueLen + 1);
                if( !zValue ) {
                    sqlite3_free(zKey);
                    rc = SQLITE_NOMEM;
                    goto parse_error;
                }
                memcpy(zValue, valueStart, valueLen);
                zValue[valueLen] = '\0';
                
                sqlite3_int64 iValue = atoll(zValue);
                cypherValueSetInteger(&value, iValue);
                sqlite3_free(zValue);
            }
            
        } else if( strncmp(p, "true", 4) == 0 ) {
            /* Boolean true */
            cypherValueSetBoolean(&value, 1);
            p += 4;
            
        } else if( strncmp(p, "false", 5) == 0 ) {
            /* Boolean false */
            cypherValueSetBoolean(&value, 0);
            p += 5;
            
        } else if( strncmp(p, "null", 4) == 0 ) {
            /* Null value */
            cypherValueSetNull(&value);
            p += 4;
            
        } else {
            /* Unsupported value type */
            sqlite3_free(zKey);
            rc = SQLITE_FORMAT;
            goto parse_error;
        }
        
        /* Expand arrays if needed */
        if( nPairs >= nAllocated ) {
            int nNewAlloc = nAllocated ? nAllocated * 2 : 4;
            
            char **azNewKeys = sqlite3_realloc(azKeys, nNewAlloc * sizeof(char*));
            if( !azNewKeys ) {
                sqlite3_free(zKey);
                cypherValueDestroy(&value);
                rc = SQLITE_NOMEM;
                goto parse_error;
            }
            azKeys = azNewKeys;
            
            CypherValue *apNewValues = sqlite3_realloc(apValues, nNewAlloc * sizeof(CypherValue));
            if( !apNewValues ) {
                sqlite3_free(zKey);
                cypherValueDestroy(&value);
                rc = SQLITE_NOMEM;
                goto parse_error;
            }
            apValues = apNewValues;
            
            nAllocated = nNewAlloc;
        }
        
        /* Add key-value pair */
        azKeys[nPairs] = zKey;
        apValues[nPairs] = value;
        nPairs++;
        
        /* Skip whitespace */
        while( isspace(*p) ) p++;
        
        /* Check for comma or end */
        if( *p == ',' ) {
            p++;
            while( isspace(*p) ) p++;
        } else if( *p != '}' ) {
            rc = SQLITE_FORMAT;
            goto parse_error;
        }
    }
    
    /* Should end with } */
    if( *p != '}' ) {
        rc = SQLITE_FORMAT;
        goto parse_error;
    }
    
    /* Success - set the map value */
    cypherValueSetMap(pResult, azKeys, apValues, nPairs);
    return SQLITE_OK;
    
parse_error:
    /* Cleanup on error */
    if( azKeys ) {
        for( int i = 0; i < nPairs; i++ ) {
            sqlite3_free(azKeys[i]);
        }
        sqlite3_free(azKeys);
    }
    if( apValues ) {
        for( int i = 0; i < nPairs; i++ ) {
            cypherValueDestroy(&apValues[i]);
        }
        sqlite3_free(apValues);
    }
    return rc;
}

/*
** Convert a CypherValue to JSON string representation.
** Caller must sqlite3_free() the returned string.
** Returns NULL on allocation failure.
*/
char *cypherValueToJson(const CypherValue *pValue) {
    if( !pValue ) return sqlite3_mprintf("null");
    
    switch( pValue->type ) {
        case CYPHER_VALUE_NULL:
            return sqlite3_mprintf("null");
            
        case CYPHER_VALUE_BOOLEAN:
            return sqlite3_mprintf(pValue->u.bBoolean ? "true" : "false");
            
        case CYPHER_VALUE_INTEGER:
            return sqlite3_mprintf("%lld", pValue->u.iInteger);
            
        case CYPHER_VALUE_FLOAT:
            return sqlite3_mprintf("%.15g", pValue->u.rFloat);
            
        case CYPHER_VALUE_STRING: {
            /* Escape string for JSON */
            const char *zStr = pValue->u.zString ? pValue->u.zString : "";
            int nLen = strlen(zStr);
            int nAlloc = nLen * 2 + 16; /* Worst case: every char escaped */
            char *zResult = sqlite3_malloc(nAlloc);
            if( !zResult ) return NULL;
            
            int nResult = 0;
            zResult[nResult++] = '"';
            
            for( int i = 0; i < nLen && nResult < nAlloc - 2; i++ ) {
                char c = zStr[i];
                switch( c ) {
                    case '"':
                        zResult[nResult++] = '\\';
                        zResult[nResult++] = '"';
                        break;
                    case '\\':
                        zResult[nResult++] = '\\';
                        zResult[nResult++] = '\\';
                        break;
                    case '\n':
                        zResult[nResult++] = '\\';
                        zResult[nResult++] = 'n';
                        break;
                    case '\r':
                        zResult[nResult++] = '\\';
                        zResult[nResult++] = 'r';
                        break;
                    case '\t':
                        zResult[nResult++] = '\\';
                        zResult[nResult++] = 't';
                        break;
                    default:
                        zResult[nResult++] = c;
                        break;
                }
            }
            
            zResult[nResult++] = '"';
            zResult[nResult] = '\0';
            return zResult;
        }
        
        case CYPHER_VALUE_LIST: {
            /* Convert list to JSON array */
            char *zResult = sqlite3_mprintf("[");
            if( !zResult ) return NULL;
            
            for( int i = 0; i < pValue->u.list.nValues; i++ ) {
                char *zElement = cypherValueToJson(&pValue->u.list.apValues[i]);
                if( !zElement ) {
                    sqlite3_free(zResult);
                    return NULL;
                }
                
                char *zNew = sqlite3_mprintf("%s%s%s", zResult, 
                                           i > 0 ? "," : "", zElement);
                sqlite3_free(zResult);
                sqlite3_free(zElement);
                
                if( !zNew ) return NULL;
                zResult = zNew;
            }
            
            char *zFinal = sqlite3_mprintf("%s]", zResult);
            sqlite3_free(zResult);
            return zFinal;
        }
        
        case CYPHER_VALUE_MAP: {
            /* Convert map to JSON object */
            char *zResult = sqlite3_mprintf("{");
            if( !zResult ) return NULL;
            
            for( int i = 0; i < pValue->u.map.nPairs; i++ ) {
                char *zValue = cypherValueToJson(&pValue->u.map.apValues[i]);
                if( !zValue ) {
                    sqlite3_free(zResult);
                    return NULL;
                }
                
                char *zNew = sqlite3_mprintf("%s%s\"%s\":%s", zResult,
                                           i > 0 ? "," : "",
                                           pValue->u.map.azKeys[i],
                                           zValue);
                sqlite3_free(zResult);
                sqlite3_free(zValue);
                
                if( !zNew ) return NULL;
                zResult = zNew;
            }
            
            char *zFinal = sqlite3_mprintf("%s}", zResult);
            sqlite3_free(zResult);
            return zFinal;
        }
        
        case CYPHER_VALUE_NODE:
            return sqlite3_mprintf("{\"_type\":\"node\",\"_id\":%lld}", pValue->u.iNodeId);
            
        case CYPHER_VALUE_RELATIONSHIP:
            return sqlite3_mprintf("{\"_type\":\"relationship\",\"_id\":%lld}", pValue->u.iRelId);
            
        default:
            return sqlite3_mprintf("null");
    }
}