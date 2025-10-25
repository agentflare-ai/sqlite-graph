#!/usr/bin/env python3
import sqlite3

conn = sqlite3.connect(':memory:')
conn.enable_load_extension(True)
conn.load_extension('./build/libgraph.dylib')
print("✅ Extension loaded")

conn.execute("CREATE VIRTUAL TABLE graph USING graph()")
print("✅ Graph table created")

try:
    conn.execute("SELECT cypher_execute('CREATE (n)')")
    count = conn.execute("SELECT graph_count_nodes()").fetchone()[0]
    print(f"✅ CREATE (n) SUCCESS! Node count: {count}")
except Exception as e:
    print(f"❌ CREATE (n) FAILED: {e}")

conn.close()
