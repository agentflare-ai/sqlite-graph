#!/usr/bin/env python3
"""
Test CREATE (:Person) - with label
"""

import sqlite3

conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)

try:
    conn.load_extension("./build/libgraph.dylib")
except:
    conn.load_extension("./build/libgraph.so")

conn.execute("CREATE TABLE nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT)")
conn.execute("CREATE TABLE edges(id INTEGER PRIMARY KEY, source INTEGER, target INTEGER, edge_type TEXT, properties TEXT)")

print("Testing: CREATE (:Person)")

# Test logical plan
try:
    result = conn.execute("SELECT cypher_logical_plan('CREATE (:Person)')").fetchone()
    print(f"✅ Logical plan: {result[0][:300]}")
except Exception as e:
    print(f"❌ Logical plan failed: {e}")

# Test execution
try:
    result = conn.execute("SELECT cypher_execute('CREATE (:Person)')").fetchone()
    print(f"✅ Execute result: {result[0]}")

    # Check if node was created with label
    rows = conn.execute("SELECT id, labels, properties FROM nodes").fetchall()
    print(f"✅ Nodes in database: {len(rows)}")
    for row in rows:
        print(f"   Node {row[0]}: labels='{row[1]}', properties='{row[2]}'")
except Exception as e:
    print(f"❌ Execute failed: {e}")

# Test with variable and label
print("\nTesting: CREATE (p:Person)")
try:
    result = conn.execute("SELECT cypher_execute('CREATE (p:Person)')").fetchone()
    print(f"✅ Execute result: {result[0]}")

    rows = conn.execute("SELECT id, labels, properties FROM nodes").fetchall()
    print(f"✅ Total nodes: {len(rows)}")
    for row in rows:
        print(f"   Node {row[0]}: labels='{row[1]}', properties='{row[2]}'")
except Exception as e:
    print(f"❌ Execute failed: {e}")

conn.close()
