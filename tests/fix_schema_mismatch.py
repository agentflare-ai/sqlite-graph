#!/usr/bin/env python3

import re

# Read the current CREATE test file
with open('tck_test_clauses_create.c', 'r') as f:
    content = f.read()

# Fix all instances of the edges table schema to match what the virtual table expects
old_schema = 'CREATE TABLE edges(id INTEGER PRIMARY KEY, from_id INTEGER, to_id INTEGER, type TEXT, weight REAL, properties TEXT);'
new_schema = 'CREATE TABLE edges(id INTEGER PRIMARY KEY, source INTEGER, target INTEGER, edge_type TEXT, weight REAL, properties TEXT);'

# Replace the schema in the file
content = content.replace(old_schema, new_schema)

# Write back the fixed content
with open('tck_test_clauses_create.c', 'w') as f:
    f.write(content)

print("✅ Fixed edges table schema in tck_test_clauses_create.c")
print("   Changed: from_id, to_id, type -> source, target, edge_type")
