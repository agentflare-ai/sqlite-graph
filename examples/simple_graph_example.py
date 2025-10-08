#!/usr/bin/env python3
"""
Simple SQLite Graph Extension Example
=====================================

A focused example showing the most common graph operations:
- Loading the extension
- Creating nodes and edges  
- Running graph algorithms
- Querying the graph

This is a great starting point for new users.
"""

import sqlite3
import json
import os


def main():
    # Connect to SQLite (in-memory database)
    conn = sqlite3.connect(":memory:")
    conn.row_factory = sqlite3.Row
    
    # Load the graph extension
    extension_path = "../build/libgraph.so"  # Adjust path as needed
    if os.path.exists(extension_path):
        conn.enable_load_extension(True)
        conn.load_extension(extension_path)
        print("✅ Graph extension loaded successfully!")
    else:
        print("❌ Graph extension not found. Please compile it first.")
        return
    
    cursor = conn.cursor()
    
    # Create virtual graph table
    try:
        cursor.execute("CREATE VIRTUAL TABLE graph USING graph()")
        print("✅ Virtual graph table created")
    except sqlite3.Error as e:
        print(f"ℹ️  Virtual table: {e}")
    
    # Test basic functionality
    print("\n🧪 Testing basic graph functionality...")
    cursor.execute("SELECT graph_count_nodes() as count")
    result = cursor.fetchone()
    print(f"Initial node count: {result['count']}")
    
    # Create some nodes
    print("\n👥 Creating people nodes...")
    people = [
        (1, {"name": "Alice", "age": 30, "city": "New York"}),
        (2, {"name": "Bob", "age": 25, "city": "San Francisco"}), 
        (3, {"name": "Carol", "age": 28, "city": "Chicago"}),
        (4, {"name": "Dave", "age": 32, "city": "Austin"})
    ]
    
    for person_id, data in people:
        cursor.execute("SELECT graph_node_add(?, ?) as result", 
                      (person_id, json.dumps(data)))
        result = cursor.fetchone()
        print(f"Created {data['name']} (ID: {person_id}): {result['result']}")
    
    # Create relationships
    print("\n🤝 Creating relationships...")
    relationships = [
        (1, 2, "FRIENDS", {"since": "2020"}),
        (1, 3, "WORKS_WITH", {"project": "GraphDB"}),
        (2, 4, "FRIENDS", {"since": "2019"}),
        (3, 4, "FRIENDS", {"since": "2021"})
    ]
    
    for from_id, to_id, rel_type, data in relationships:
        cursor.execute("SELECT graph_edge_add(?, ?, ?, ?) as result",
                      (from_id, to_id, rel_type, json.dumps(data)))
        result = cursor.fetchone()
        from_name = next(p[1]['name'] for p in people if p[0] == from_id)
        to_name = next(p[1]['name'] for p in people if p[0] == to_id)
        print(f"{from_name} -> {to_name} ({rel_type}): {result['result']}")
    
    # Check graph statistics
    print("\n📊 Graph Statistics:")
    cursor.execute("SELECT graph_count_nodes() as nodes, graph_count_edges() as edges")
    stats = cursor.fetchone()
    print(f"Nodes: {stats['nodes']}, Edges: {stats['edges']}")
    
    # Run graph algorithms
    print("\n🧮 Running graph algorithms...")
    
    try:
        cursor.execute("SELECT graph_is_connected() as connected")
        result = cursor.fetchone()
        print(f"Graph is connected: {bool(result['connected'])}")
    except sqlite3.Error as e:
        print(f"Connectivity check: {e}")
    
    try:
        cursor.execute("SELECT graph_density() as density")
        result = cursor.fetchone()
        print(f"Graph density: {result['density']:.3f}")
    except sqlite3.Error as e:
        print(f"Density calculation: {e}")
    
    try:
        cursor.execute("SELECT graph_degree_centrality(1) as centrality")
        result = cursor.fetchone()
        print(f"Alice's centrality: {result['centrality']:.3f}")
    except sqlite3.Error as e:
        print(f"Centrality calculation: {e}")
    
    # Test Cypher parsing
    print("\n🔍 Testing Cypher query parsing...")
    cypher_queries = [
        "RETURN 42",
        "MATCH (n) RETURN n", 
        "CREATE (n:Person {name: 'Alice'})"
    ]
    
    for query in cypher_queries:
        try:
            cursor.execute("SELECT cypher_parse(?) as result", (query,))
            result = cursor.fetchone()
            print(f"✅ '{query}' -> Valid")
        except sqlite3.Error as e:
            print(f"❌ '{query}' -> {e}")
    
    conn.close()
    print("\n🎉 Example completed successfully!")
    print("\n💡 Next steps:")
    print("   - Try modifying the node/edge data")
    print("   - Experiment with larger graphs")
    print("   - Explore more graph algorithms")
    print("   - Check out python_examples.py for advanced features")


if __name__ == "__main__":
    main()
