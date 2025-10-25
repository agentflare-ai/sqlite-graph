/*
** graph-compress.c - Property compression implementation
**
** This file implements dictionary encoding and other compression
** techniques for efficient property storage in the graph database.
*/

#include <sqlite3.h>
#include "sqlite3ext.h"
#ifndef SQLITE_CORE
extern const sqlite3_api_routines *sqlite3_api;
#endif
/* SQLITE_EXTENSION_INIT1 - removed to prevent multiple definition */
#include <string.h>
#include <stdlib.h>

/* Check if zlib is available - temporarily disabled for testing */
#define HAVE_ZLIB 0

#include "graph.h"
#include "graph-memory.h"
#include "graph-performance.h"
#include "graph-memory.h"

/* Dictionary entry for string compression */
typedef struct DictEntry {
    char *zValue;                /* String value */
    sqlite3_int64 dictId;        /* Dictionary ID */
    sqlite3_int64 refCount;      /* Reference count */
    size_t length;               /* String length */
    struct DictEntry *pNext;     /* Hash chain */
} DictEntry;

/* String dictionary for property compression */
typedef struct StringDictionary {
    DictEntry **buckets;         /* Hash table */
    int nBuckets;                /* Number of buckets */
    sqlite3_int64 nEntries;      /* Total entries */
    sqlite3_int64 nextId;        /* Next dictionary ID */
    size_t totalSize;            /* Total memory used */
    size_t savedBytes;           /* Bytes saved by compression */
    sqlite3_mutex *mutex;        /* Thread safety */
} StringDictionary;

/* Compressed property storage */
typedef struct CompressedProperty {
    int type;                    /* Property type */
    union {
        sqlite3_int64 dictId;    /* Dictionary ID for strings */
        sqlite3_int64 intVal;    /* Integer value */
        double floatVal;         /* Float value */
    } value;
} CompressedProperty;

/* Property type enumeration */
enum {
    PROP_TYPE_NULL = 0,
    PROP_TYPE_BOOL = 1,
    PROP_TYPE_INT = 2,
    PROP_TYPE_FLOAT = 3,
    PROP_TYPE_STRING = 4,
    PROP_TYPE_DICT_STRING = 5,   /* Dictionary-encoded string */
    PROP_TYPE_COMPRESSED = 6      /* zlib compressed */
};

/* Global string dictionary */
static StringDictionary *g_stringDict = NULL;

/*
** Hash function for dictionary
*/
static unsigned int dictHash(const char *zStr) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *zStr++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/*
** Initialize string dictionary
*/
int graphInitStringDictionary(int initialBuckets) {
    if (g_stringDict) return SQLITE_MISUSE;
    
    g_stringDict = sqlite3_malloc(sizeof(StringDictionary));
    if (!g_stringDict) return SQLITE_NOMEM;
    
    memset(g_stringDict, 0, sizeof(StringDictionary));
    
    if (initialBuckets <= 0) initialBuckets = 1024;
    g_stringDict->nBuckets = initialBuckets;
    
    g_stringDict->buckets = sqlite3_malloc(
        initialBuckets * sizeof(DictEntry*));
    if (!g_stringDict->buckets) {
        sqlite3_free(g_stringDict);
        g_stringDict = NULL;
        return SQLITE_NOMEM;
    }
    
    memset(g_stringDict->buckets, 0, 
           initialBuckets * sizeof(DictEntry*));
    
    g_stringDict->nextId = 1;
    g_stringDict->mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_FAST);
    
    return SQLITE_OK;
}

/*
** Add string to dictionary
*/
static sqlite3_int64 dictAddString(const char *zStr) {
    if (!g_stringDict || !zStr) return 0;
    
    sqlite3_mutex_enter(g_stringDict->mutex);
    
    /* Check if already exists */
    unsigned int bucket = dictHash(zStr) % g_stringDict->nBuckets;
    DictEntry *entry = g_stringDict->buckets[bucket];
    
    while (entry) {
        if (strcmp(entry->zValue, zStr) == 0) {
            /* Found existing entry */
            entry->refCount++;
            sqlite3_int64 id = entry->dictId;
            sqlite3_mutex_leave(g_stringDict->mutex);
            return id;
        }
        entry = entry->pNext;
    }
    
    /* Create new entry */
    entry = sqlite3_malloc(sizeof(DictEntry));
    if (!entry) {
        sqlite3_mutex_leave(g_stringDict->mutex);
        return 0;
    }
    
    size_t len = strlen(zStr);
    entry->zValue = sqlite3_malloc(len + 1);
    if (!entry->zValue) {
        sqlite3_free(entry);
        sqlite3_mutex_leave(g_stringDict->mutex);
        return 0;
    }

    memcpy(entry->zValue, zStr, len + 1);
    entry->dictId = g_stringDict->nextId++;
    entry->refCount = 1;
    entry->length = len;
    
    /* Insert into hash table */
    entry->pNext = g_stringDict->buckets[bucket];
    g_stringDict->buckets[bucket] = entry;
    
    /* Update statistics */
    g_stringDict->nEntries++;
    g_stringDict->totalSize += sizeof(DictEntry) + len + 1;
    
    /* Track compression savings */
    if (entry->refCount > 1) {
        g_stringDict->savedBytes += len - sizeof(sqlite3_int64);
    }
    
    sqlite3_int64 id = entry->dictId;
    sqlite3_mutex_leave(g_stringDict->mutex);
    
    return id;
}

/*
** Get string from dictionary by ID
*/
static const char* dictGetString(sqlite3_int64 dictId) {
    if (!g_stringDict || dictId <= 0) return NULL;
    
    sqlite3_mutex_enter(g_stringDict->mutex);
    
    /* Linear search through all buckets (could optimize with reverse index) */
    for (int i = 0; i < g_stringDict->nBuckets; i++) {
        DictEntry *entry = g_stringDict->buckets[i];
        while (entry) {
            if (entry->dictId == dictId) {
                const char *value = entry->zValue;
                sqlite3_mutex_leave(g_stringDict->mutex);
                return value;
            }
            entry = entry->pNext;
        }
    }
    
    sqlite3_mutex_leave(g_stringDict->mutex);
    return NULL;
}

/*
** Compress properties using dictionary encoding
*/
char* graphCompressProperties(const char *zProperties) {
    if (!zProperties || !g_stringDict) return NULL;

    /* Parse JSON properties */
    /* This is a simplified implementation - real version would use proper JSON parser */

    /* Allocate buffer large enough for worst case */
    size_t inputLen = strlen(zProperties);
    size_t bufSize = inputLen * 2 + 256; /* Conservative estimate */
    char *compressed = sqlite3_malloc(bufSize);
    if (!compressed) return NULL;

    size_t offset = 0;
    const char *prefix = "{\"_compressed\":true,";
    size_t prefixLen = strlen(prefix);
    if (prefixLen >= bufSize) {
        sqlite3_free(compressed);
        return NULL;
    }
    memcpy(compressed + offset, prefix, prefixLen);
    offset += prefixLen;
    compressed[offset] = '\0';

    /* Simple pattern matching for demonstration */
    const char *p = zProperties;
    int n;  /* For snprintf return value */
    while (*p) {
        if (*p == '"') {
            /* Found start of string */
            p++;
            const char *start = p;
            while (*p && *p != '"') p++;

            if (*p == '"') {
                /* Extract string value */
                size_t len = p - start;
                char *value = sqlite3_malloc(len + 1);
                if (value) {
                    memcpy(value, start, len);
                    value[len] = '\0';

                    /* Check if worth compressing (length > 10) */
                    if (len > 10) {
                        sqlite3_int64 dictId = dictAddString(value);
                        if (dictId > 0) {
                            /* Replace with dictionary reference */
                            char *dictRef = sqlite3_mprintf("\"_dict\":%lld", dictId);
                            if (dictRef) {
                                size_t dictLen = strlen(dictRef);
                                if (offset + dictLen >= bufSize) {
                                    sqlite3_free(dictRef);
                                    sqlite3_free(value);
                                    sqlite3_free(compressed);
                                    return NULL;
                                }
                                memcpy(compressed + offset, dictRef, dictLen);
                                offset += dictLen;
                                compressed[offset] = '\0';
                                sqlite3_free(dictRef);
                            }
                        }
                    } else {
                        /* Keep original string - need to escape quotes properly */
                        if (offset + len + 3 >= bufSize) {
                            sqlite3_free(value);
                            sqlite3_free(compressed);
                            return NULL;
                        }
                        compressed[offset++] = '"';
                        memcpy(compressed + offset, value, len);
                        offset += len;
                        compressed[offset++] = '"';
                        compressed[offset] = '\0';
                    }

                    sqlite3_free(value);
                }
            }
        }
        p++;
    }

    if (offset + 2 >= bufSize) {
        sqlite3_free(compressed);
        return NULL;
    }
    compressed[offset++] = '}';
    compressed[offset] = '\0';

    return compressed;
}

/*
** Decompress properties
*/
char* graphDecompressProperties(const char *zCompressed) {
    if (!zCompressed) return NULL;

    /* Check if compressed */
    if (!strstr(zCompressed, "_compressed")) {
        /* Not compressed, return copy */
        return sqlite3_mprintf("%s", zCompressed);
    }

    size_t bufSize = strlen(zCompressed) * 4 + 256; /* Conservative estimate */
    char *decompressed = sqlite3_malloc(bufSize);
    if (!decompressed) return NULL;

    size_t offset = 0;
    decompressed[offset++] = '{';
    decompressed[offset] = '\0';

    /* Look for dictionary references */
    const char *p = strstr(zCompressed, "_dict");
    while (p) {
        /* Extract dictionary ID */
        p += 7; /* Skip "_dict":" */
        sqlite3_int64 dictId = 0;
        while (*p >= '0' && *p <= '9') {
            dictId = dictId * 10 + (*p - '0');
            p++;
        }

        /* Look up string */
        const char *value = dictGetString(dictId);
        if (value) {
            size_t valueLen = strlen(value);
            if (offset + valueLen + 3 >= bufSize) {
                sqlite3_free(decompressed);
                return NULL;
            }
            decompressed[offset++] = '"';
            memcpy(decompressed + offset, value, valueLen);
            offset += valueLen;
            decompressed[offset++] = '"';
            decompressed[offset] = '\0';
        }

        p = strstr(p, "_dict");
    }

    if (offset + 2 >= bufSize) {
        sqlite3_free(decompressed);
        return NULL;
    }
    decompressed[offset++] = '}';
    decompressed[offset] = '\0';

    return decompressed;
}

/*
** Compress using zlib for large properties
*/
char* graphCompressLarge(const char *zData, size_t *pCompressedSize) {
    if (!zData) return NULL;
    
    size_t srcLen = strlen(zData);
    
    /* Only compress if large enough */
    if (srcLen < 1024) {
        *pCompressedSize = srcLen;
        return sqlite3_mprintf("%s", zData);
    }
    
#if HAVE_ZLIB
    /* Allocate buffer for compressed data */
    uLongf destLen = compressBound(srcLen);
    unsigned char *compressed = sqlite3_malloc(destLen + 16);
    if (!compressed) return NULL;
    
    /* Add header indicating compression */
    strcpy((char*)compressed, "ZLIB:");
    
    /* Compress data */
    int rc = compress(compressed + 5, &destLen, 
                     (const unsigned char*)zData, srcLen);
    
    if (rc != Z_OK) {
        sqlite3_free(compressed);
        return NULL;
    }
    
    *pCompressedSize = destLen + 5;
    
    /* Check if compression was worthwhile */
    if (*pCompressedSize >= srcLen * 0.9) {
        /* Not worth it, return original */
        sqlite3_free(compressed);
        *pCompressedSize = srcLen;
        return sqlite3_mprintf("%s", zData);
    }
    
    return (char*)compressed;
#else
    /* Compression not available, return original */
    *pCompressedSize = srcLen;
    return sqlite3_mprintf("%s", zData);
#endif
}

/*
** Decompress zlib data
*/
char* graphDecompressLarge(const char *zCompressed, size_t compressedSize) {
    if (!zCompressed || compressedSize < 6) return NULL;
    
    /* Check for zlib header */
    if (strncmp(zCompressed, "ZLIB:", 5) != 0) {
        /* Not compressed */
        return sqlite3_mprintf("%s", zCompressed);
    }
    
#if HAVE_ZLIB
    /* Estimate decompressed size (could be stored in header) */
    uLongf destLen = compressedSize * 10;
    unsigned char *decompressed = sqlite3_malloc(destLen);
    if (!decompressed) return NULL;
    
    /* Decompress */
    int rc = uncompress(decompressed, &destLen,
                       (const unsigned char*)zCompressed + 5,
                       compressedSize - 5);
    
    if (rc != Z_OK) {
        sqlite3_free(decompressed);
        return NULL;
    }
    
    /* Null terminate */
    decompressed[destLen] = '\0';
    
    return (char*)decompressed;
#else
    /* Decompression not available, return error */
    return NULL;
#endif
}

/*
** Get compression statistics
*/
void graphCompressionStats(sqlite3_int64 *pDictEntries,
                          size_t *pDictMemory,
                          size_t *pSavedBytes) {
    if (!g_stringDict) return;
    
    sqlite3_mutex_enter(g_stringDict->mutex);
    
    if (pDictEntries) *pDictEntries = g_stringDict->nEntries;
    if (pDictMemory) *pDictMemory = g_stringDict->totalSize;
    if (pSavedBytes) *pSavedBytes = g_stringDict->savedBytes;
    
    sqlite3_mutex_leave(g_stringDict->mutex);
}

/*
** SQL function to get compression statistics
*/
static void compressionStatsFunc(
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
) {
    (void)argc;
    (void)argv;
    sqlite3_int64 dictEntries = 0;
    size_t dictMemory = 0, savedBytes = 0;
    
    graphCompressionStats(&dictEntries, &dictMemory, &savedBytes);
    
    double ratio = 0.0;
    if (dictMemory > 0) {
        ratio = (double)savedBytes / dictMemory * 100.0;
    }
    
    char *result = sqlite3_mprintf(
        "{\"dict_entries\":%lld,\"dict_memory\":%zu,"
        "\"saved_bytes\":%zu,\"compression_ratio\":%.1f}",
        dictEntries, dictMemory, savedBytes, ratio
    );
    
    sqlite3_result_text(context, result, -1, sqlite3_free);
}

/*
** Shutdown compression system
*/
void graphCompressionShutdown(void) {
    if (!g_stringDict) return;
    
    /* Free all dictionary entries */
    for (int i = 0; i < g_stringDict->nBuckets; i++) {
        DictEntry *entry = g_stringDict->buckets[i];
        while (entry) {
            DictEntry *next = entry->pNext;
            sqlite3_free(entry->zValue);
            sqlite3_free(entry);
            entry = next;
        }
    }
    
    sqlite3_free(g_stringDict->buckets);
    sqlite3_mutex_free(g_stringDict->mutex);
    sqlite3_free(g_stringDict);
    g_stringDict = NULL;
}

/*
** Register compression SQL functions
*/
int graphRegisterCompressionFunctions(sqlite3 *db) {
    return sqlite3_create_function(db, "graph_compression_stats", 0,
                                  SQLITE_UTF8, NULL,
                                  compressionStatsFunc, NULL, NULL);
}