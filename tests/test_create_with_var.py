#!/usr/bin/env python3
"""
Test CREATE (n) - with variable
"""

import sqlite3

conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)

try:
    conn.load_extension("./build/libgraph")
except:
    conn.load_extension("./build/libgraph")

conn.execute("CREATE TABLE nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT)")
conn.execute("CREATE TABLE edges(id INTEGER PRIMARY KEY, source INTEGER, target INTEGER, edge_type TEXT, properties TEXT)")

print("Testing: CREATE (n)")

# Test AST
try:
    result = conn.execute("SELECT cypher_ast_info('CREATE (n)')").fetchone()
    print(f"AST info: {result[0]}")
except Exception as e:
    print(f"❌ AST info failed: {e}")

# Test logical plan
try:
    result = conn.execute("SELECT cypher_logical_plan('CREATE (n)')").fetchone()
    print(f"✅ Logical plan: {result[0][:200]}")
except Exception as e:
    print(f"❌ Logical plan failed: {e}")

# Test execution
try:
    result = conn.execute("SELECT cypher_execute('CREATE (n)')").fetchone()
    print(f"✅ Execute result: {result[0]}")

    # Check if node was created
    count = conn.execute("SELECT COUNT(*) FROM nodes").fetchone()[0]
    print(f"✅ Nodes in database: {count}")
except Exception as e:
    print(f"ℹ️  Execute failed (expected for now): {e}")

conn.close()
