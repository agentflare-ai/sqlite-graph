#!/usr/bin/env python3
"""
Simple test for CREATE () - just parse and show result
"""

import sqlite3

conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)

try:
    conn.load_extension("./build/libgraph")
except:
    conn.load_extension("./build/libgraph")

print("Testing: CREATE ()")

# Just parse
try:
    result = conn.execute("SELECT cypher_parse('CREATE ()')").fetchone()
    print(f"✅ Parse result: {result[0][:100]}")
except Exception as e:
    print(f"❌ Parse failed: {e}")

# Try AST info
try:
    result = conn.execute("SELECT cypher_ast_info('CREATE ()')").fetchone()
    print(f"✅ AST info: {result[0][:200]}")
except Exception as e:
    print(f"❌ AST info failed: {e}")

# Now try logical plan
try:
    result = conn.execute("SELECT cypher_logical_plan('CREATE ()')").fetchone()
    print(f"✅ Logical plan: {result[0][:200]}")
except Exception as e:
    print(f"❌ Logical plan failed: {e}")

conn.close()
