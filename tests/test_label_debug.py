#!/usr/bin/env python3
"""
Debug label extraction
"""

import sqlite3

conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)

try:
    conn.load_extension("./build/libgraph.dylib")
except:
    conn.load_extension("./build/libgraph.so")

print("Testing label extraction in logical plan")

queries = [
    "CREATE (p)",
    "CREATE (p:Person)",
    "CREATE (p:Person:Employee)",
]

for query in queries:
    print(f"\nQuery: {query}")
    try:
        result = conn.execute("SELECT cypher_logical_plan(?)", (query,)).fetchone()
        print(f"  Logical plan: {result[0]}")
    except Exception as e:
        print(f"  Error: {e}")

conn.close()
