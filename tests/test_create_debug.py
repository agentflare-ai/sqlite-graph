#!/usr/bin/env python3
"""
Debug CREATE parsing - see what AST is generated
"""

import sqlite3
import sys

def test_create_debug():
    """Test CREATE parsing output"""

    conn = sqlite3.connect(":memory:")
    conn.enable_load_extension(True)

    try:
        conn.load_extension("./build/libgraph.dylib")
    except:
        try:
            conn.load_extension("./build/libgraph.so")
        except Exception as e:
            print(f"❌ Failed to load extension: {e}")
            return False

    print("✅ Extension loaded")

    # Create tables
    conn.execute("CREATE TABLE nodes(id INTEGER PRIMARY KEY, labels TEXT, properties TEXT)")
    conn.execute("CREATE TABLE edges(id INTEGER PRIMARY KEY, source INTEGER, target INTEGER, edge_type TEXT, properties TEXT)")

    # Test what cypher_parse returns for different queries
    queries = [
        ("MATCH (n) RETURN n", "Basic MATCH"),
        ("CREATE (n)", "CREATE with variable"),
        ("CREATE ()", "CREATE without variable"),
    ]

    for query, description in queries:
        print(f"\n{description}: {query}")
        try:
            result = conn.execute("SELECT cypher_parse(?)", (query,)).fetchone()
            if result:
                print(f"✅ Parsed successfully:")
                print(f"   {result[0][:200]}...")  # First 200 chars
        except Exception as e:
            print(f"❌ Parse failed: {e}")

    # Test cypher_logical_plan
    print("\n\nTesting logical plan generation:")
    for query, description in queries:
        print(f"\n{description}: {query}")
        try:
            result = conn.execute("SELECT cypher_logical_plan(?)", (query,)).fetchone()
            if result:
                print(f"✅ Logical plan created:")
                print(f"   {result[0][:200]}...")
        except Exception as e:
            print(f"❌ Logical plan failed: {e}")

    conn.close()
    return True

if __name__ == "__main__":
    test_create_debug()
