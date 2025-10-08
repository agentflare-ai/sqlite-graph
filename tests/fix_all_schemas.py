#!/usr/bin/env python3

import os

# Fix the schema in all test files
files_to_fix = [
    'tck_test_clauses_match.c',
    'tck_test_clauses_return.c',
    'test_insert_nodes_working.c'
]

old_schema = 'CREATE TABLE edges(id INTEGER PRIMARY KEY, from_id INTEGER, to_id INTEGER, type TEXT, weight REAL, properties TEXT);'
new_schema = 'CREATE TABLE edges(id INTEGER PRIMARY KEY, source INTEGER, target INTEGER, edge_type TEXT, weight REAL, properties TEXT);'

# Also fix INSERT statements
old_insert = 'INSERT INTO edges (from_id, to_id, type, weight, properties)'
new_insert = 'INSERT INTO edges (source, target, edge_type, weight, properties)'

for filename in files_to_fix:
    if os.path.exists(filename):
        with open(filename, 'r') as f:
            content = f.read()
        
        # Fix schema
        content = content.replace(old_schema, new_schema)
        # Fix INSERT statements
        content = content.replace(old_insert, new_insert)
        
        with open(filename, 'w') as f:
            f.write(content)
        
        print(f"✅ Fixed schema in {filename}")
    else:
        print(f"❌ File not found: {filename}")

print("\n🔧 Schema fixes completed for all test files")
