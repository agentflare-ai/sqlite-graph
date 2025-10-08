import re

def patch_file(file_path, patches):
    with open(file_path, 'r') as f:
        content = f.read()

    for old_code, new_code in patches.items():
        if re.search(re.escape(old_code), content, re.DOTALL):
            content = content.replace(old_code, new_code)
            print(f"✅ Patched {file_path}")
        else:
            print(f"❌ Pattern not found in {file_path}: {old_code}")

    with open(file_path, 'w') as f:
        f.write(content)

# Patch for graphBestIndex in src/graph-vtab.c
patch1 = {
'static int graphBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info *pIdxInfo)':
'static int graphBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info *pIdxInfo) {\n  int i;\n  for (i = 0; i < pIdxInfo->nConstraint; i++) {\n    if (pIdxInfo->aConstraint[i].iColumn == 0 &&\n        pIdxInfo->aConstraint[i].op == SQLITE_INDEX_CONSTRAINT_EQ &&\n        pIdxInfo->aConstraint[i].usable) {\n      pIdxInfo->aConstraintUsage[i].argvIndex = 1;\n      pIdxInfo->aConstraintUsage[i].omit = 1;\n      pIdxInfo->idxNum = 1; // 1 for type filter\n      return SQLITE_OK;\n    }\n  }\n  pIdxInfo->idxNum = 0; // 0 for full scan\n  return SQLITE_OK;\n}'
}

# Patch for graphFilter in src/graph-vtab.c
patch2 = {
'static int graphFilter(sqlite3_vtab_cursor *pCur, int idxNum, const char *idxStr, int argc, sqlite3_value **argv)':
'static int graphFilter(sqlite3_vtab_cursor *pCur, int idxNum, const char *idxStr, int argc, sqlite3_value **argv) {\n  GraphCursor *pCsr = (GraphCursor *)pCur;\n  pCsr->iRowid = 0;\n\n  if (idxNum == 1) { // We have a type constraint\n    const char *type = (const char *)sqlite3_value_text(argv[0]);\n    if (strcmp(type, "node") == 0) {\n      pCsr->mode = 0; // Node mode\n    } else if (strcmp(type, "edge") == 0) {\n      pCsr->mode = 1; // Edge mode\n    } else {\n      // Invalid type, return no rows\n      pCsr->mode = -1;\n    }\n  } else { // Full scan\n    pCsr->mode = 0; // Default to nodes\n  }\n\n  return SQLITE_OK;\n}'
}

# Patch for graphNext in src/graph-vtab.c to handle different modes
patch3 = {
'static int graphNext(sqlite3_vtab_cursor *pCur)':
'static int graphNext(sqlite3_vtab_cursor *pCur) {\n  GraphCursor *pCsr = (GraphCursor *)pCur;\n  pCsr->iRowid++;\n  return SQLITE_OK;\n}'
}


patch_file('../src/graph-vtab.c', patch1)
patch_file('../src/graph-vtab.c', patch2)
patch_file('../src/graph-vtab.c', patch3)
