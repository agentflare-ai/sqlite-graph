#!/usr/bin/env python3
"""
Cypher Query Demo - SQLite Graph Extension
Demonstrates the newly working Cypher CREATE operations

Alpha v0.1.0 - Basic CREATE queries now execute end-to-end!
"""

import sqlite3
import sys
import os

# Add parent directory to path for loading extension
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

def demo_cypher_create():
    """Demonstrate Cypher CREATE operations"""
    print("=" * 60)
    print("Cypher CREATE Query Demo")
    print("=" * 60)

    # Connect and load extension
    conn = sqlite3.connect(':memory:')
    conn.enable_load_extension(True)

    # Try to load extension from different possible locations
    extension_loaded = False
    for ext_path in ['./build/libgraph.dylib', './build/libgraph.so', '../build/libgraph.dylib', '../build/libgraph.so']:
        try:
            conn.load_extension(ext_path)
            print(f"✅ Loaded extension from {ext_path}\n")
            extension_loaded = True
            break
        except:
            continue

    if not extension_loaded:
        print("❌ Could not load graph extension. Please build first with 'make'")
        return False

    # Create graph virtual table
    conn.execute("CREATE VIRTUAL TABLE graph USING graph()")
    print("✅ Created graph virtual table\n")

    # Example 1: Create anonymous node
    print("Example 1: Create anonymous node")
    print("Query: CREATE (n)")
    try:
        conn.execute("SELECT cypher_execute('CREATE (n)')")
        count = conn.execute("SELECT graph_count_nodes()").fetchone()[0]
        print(f"✅ Success! Total nodes: {count}\n")
    except Exception as e:
        print(f"❌ Failed: {e}\n")
        return False

    # Example 2: Create node with label
    print("Example 2: Create node with label")
    print("Query: CREATE (p:Person)")
    try:
        conn.execute("SELECT cypher_execute('CREATE (p:Person)')")
        count = conn.execute("SELECT graph_count_nodes()").fetchone()[0]
        labeled_nodes = conn.execute("SELECT id, labels FROM graph_nodes WHERE labels != ''").fetchall()
        print(f"✅ Success! Total nodes: {count}")
        print(f"   Labeled nodes: {labeled_nodes}\n")
    except Exception as e:
        print(f"❌ Failed: {e}\n")
        return False

    # Example 3: Create multiple labeled nodes
    print("Example 3: Create multiple labeled nodes")
    queries = [
        "CREATE (c:Company)",
        "CREATE (d:Department)",
        "CREATE (e:Employee)"
    ]
    for query in queries:
        print(f"Query: {query}")
        try:
            conn.execute(f"SELECT cypher_execute('{query}')")
            print("✅ Success")
        except Exception as e:
            print(f"❌ Failed: {e}")

    print()

    # Show final state
    print("Final Graph State:")
    total_nodes = conn.execute("SELECT graph_count_nodes()").fetchone()[0]
    print(f"  Total nodes: {total_nodes}")

    all_labeled = conn.execute("SELECT id, labels FROM graph_nodes WHERE labels != '' ORDER BY id").fetchall()
    print(f"  Labeled nodes:")
    for node_id, label in all_labeled:
        print(f"    Node {node_id}: {label}")

    print("\n" + "=" * 60)
    print("Demo Complete!")
    print("=" * 60)
    print("\nNote: The following are not yet implemented:")
    print("  - CREATE with properties: CREATE ({name: 'Alice'})")
    print("  - CREATE relationships: CREATE (a)-[:KNOWS]->(b)")
    print("  - MATCH queries: MATCH (n:Person) RETURN n")
    print("  - RETURN clause: CREATE (n) RETURN n")
    print("\nSee ROADMAP.md for the full development timeline.")

    conn.close()
    return True

if __name__ == '__main__':
    success = demo_cypher_create()
    sys.exit(0 if success else 1)
