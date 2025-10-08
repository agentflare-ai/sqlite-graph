#!/usr/bin/env python3
"""
SQLite Graph Database Extension - Python Examples
==================================================

This file contains comprehensive examples showing how to use the SQLite Graph
Database Extension from Python. The extension provides graph database functionality
with openCypher-like query support built on top of SQLite.

Prerequisites:
- Python 3.6+
- sqlite3 module (built-in)
- Compiled graph extension (.so file)

Author: SQLite Graph Extension Team
License: MIT
"""

import sqlite3
import json
import os
from typing import List, Dict, Any, Tuple, Optional
from pathlib import Path


class GraphDB:
    """
    A Python wrapper for the SQLite Graph Database Extension.
    
    This class provides a convenient interface for working with graph data
    using the SQLite graph extension.
    """
    
    def __init__(self, db_path: str = ":memory:", extension_path: str = None):
        """
        Initialize the graph database.
        
        Args:
            db_path: Path to SQLite database file (default: in-memory)
            extension_path: Path to the graph extension .so file
        """
        self.db_path = db_path
        self.conn = sqlite3.connect(db_path)
        self.conn.row_factory = sqlite3.Row  # Enable column access by name
        
        # Load the graph extension
        if extension_path is None:
            # Try to find the extension in common locations
            possible_paths = [
                "../build/libgraph.so",
                "build/libgraph.so", 
                "./libgraph.so",
                "graph.so"
            ]
            for path in possible_paths:
                if os.path.exists(path):
                    extension_path = path
                    break
            
        if extension_path and os.path.exists(extension_path):
            self.conn.enable_load_extension(True)
            self.conn.load_extension(extension_path)
            print(f"✅ Loaded graph extension: {extension_path}")
        else:
            print("⚠️  Graph extension not found. Some features may not work.")
            
        self.cursor = self.conn.cursor()
        
    def __enter__(self):
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        
    def close(self):
        """Close the database connection."""
        if self.conn:
            self.conn.close()
            
    def execute(self, query: str, params: tuple = ()) -> sqlite3.Cursor:
        """Execute a SQL query and return the cursor."""
        return self.cursor.execute(query, params)
        
    def fetchall(self) -> List[sqlite3.Row]:
        """Fetch all results from the last query."""
        return self.cursor.fetchall()
        
    def fetchone(self) -> Optional[sqlite3.Row]:
        """Fetch one result from the last query."""
        return self.cursor.fetchone()
        
    def commit(self):
        """Commit the current transaction."""
        self.conn.commit()


def example_1_basic_setup():
    """
    Example 1: Basic Setup and Extension Loading
    ============================================
    
    Shows how to load the extension and verify it's working.
    """
    print("\n" + "="*60)
    print("EXAMPLE 1: Basic Setup and Extension Loading")
    print("="*60)
    
    with GraphDB() as db:
        # Test if extension is loaded by calling a graph function
        try:
            db.execute("SELECT graph_count_nodes() as node_count")
            result = db.fetchone()
            print(f"✅ Extension loaded successfully!")
            print(f"📊 Initial node count: {result['node_count']}")
        except sqlite3.Error as e:
            print(f"❌ Extension not loaded: {e}")
            return False
            
        # Create a virtual graph table  
        try:
            db.execute("CREATE VIRTUAL TABLE graph USING graph()")
            print("✅ Virtual graph table created successfully!")
        except sqlite3.Error as e:
            print(f"ℹ️  Virtual table might already exist: {e}")
            
        return True


def example_2_creating_nodes_and_edges():
    """
    Example 2: Creating Nodes and Edges
    ===================================
    
    Shows how to create nodes and edges using SQL functions.
    """
    print("\n" + "="*60)
    print("EXAMPLE 2: Creating Nodes and Edges")
    print("="*60)
    
    with GraphDB() as db:
        # Ensure virtual table exists
        try:
            db.execute("CREATE VIRTUAL TABLE graph USING graph()")
        except sqlite3.Error:
            pass  # Table might already exist
            
        # Create nodes
        print("Creating nodes...")
        
        # Add person nodes
        alice_id = 1
        bob_id = 2
        charlie_id = 3
        
        db.execute("SELECT graph_node_add(?, ?) as result", (alice_id, '{"name": "Alice", "age": 30, "city": "New York"}'))
        result = db.fetchone()
        print(f"✅ Created node Alice (ID: {alice_id}): {result['result']}")
        
        db.execute("SELECT graph_node_add(?, ?) as result", (bob_id, '{"name": "Bob", "age": 25, "city": "San Francisco"}'))
        result = db.fetchone()
        print(f"✅ Created node Bob (ID: {bob_id}): {result['result']}")
        
        db.execute("SELECT graph_node_add(?, ?) as result", (charlie_id, '{"name": "Charlie", "age": 35, "city": "Chicago"}'))
        result = db.fetchone()
        print(f"✅ Created node Charlie (ID: {charlie_id}): {result['result']}")
        
        # Create edges (relationships)
        print("\nCreating relationships...")
        
        # Alice knows Bob
        db.execute("SELECT graph_edge_add(?, ?, ?, ?) as result", 
                  (alice_id, bob_id, "KNOWS", '{"since": "2020", "strength": 0.8}'))
        result = db.fetchone()
        print(f"✅ Created KNOWS relationship: Alice -> Bob: {result['result']}")
        
        # Bob knows Charlie  
        db.execute("SELECT graph_edge_add(?, ?, ?, ?) as result",
                  (bob_id, charlie_id, "KNOWS", '{"since": "2019", "strength": 0.9}'))
        result = db.fetchone()
        print(f"✅ Created KNOWS relationship: Bob -> Charlie: {result['result']}")
        
        # Alice works with Charlie
        db.execute("SELECT graph_edge_add(?, ?, ?, ?) as result",
                  (alice_id, charlie_id, "WORKS_WITH", '{"project": "GraphDB", "since": "2021"}'))
        result = db.fetchone()
        print(f"✅ Created WORKS_WITH relationship: Alice -> Charlie: {result['result']}")
        
        # Check counts
        db.execute("SELECT graph_count_nodes() as nodes, graph_count_edges() as edges")
        counts = db.fetchone()
        print(f"\n📊 Graph statistics:")
        print(f"   Nodes: {counts['nodes']}")
        print(f"   Edges: {counts['edges']}")


def example_3_graph_algorithms():
    """
    Example 3: Graph Algorithms
    ============================
    
    Shows how to use built-in graph algorithms.
    """
    print("\n" + "="*60)
    print("EXAMPLE 3: Graph Algorithms")
    print("="*60)
    
    with GraphDB() as db:
        # Set up a small graph first
        example_2_creating_nodes_and_edges()
        
        print("\nRunning graph algorithms...")
        
        # Check if graph is connected
        try:
            db.execute("SELECT graph_is_connected() as connected")
            result = db.fetchone()
            print(f"🔗 Graph is connected: {bool(result['connected'])}")
        except sqlite3.Error as e:
            print(f"ℹ️  graph_is_connected: {e}")
        
        # Calculate graph density
        try:
            db.execute("SELECT graph_density() as density")
            result = db.fetchone()
            print(f"📏 Graph density: {result['density']:.3f}")
        except sqlite3.Error as e:
            print(f"ℹ️  graph_density: {e}")
            
        # Check for cycles
        try:
            db.execute("SELECT graph_has_cycle() as has_cycle")
            result = db.fetchone()
            print(f"🔄 Graph has cycles: {bool(result['has_cycle'])}")
        except sqlite3.Error as e:
            print(f"ℹ️  graph_has_cycle: {e}")
            
        # Calculate degree centrality for Alice (node 1)
        try:
            db.execute("SELECT graph_degree_centrality(1) as centrality")
            result = db.fetchone()
            print(f"📊 Alice's degree centrality: {result['centrality']:.3f}")
        except sqlite3.Error as e:
            print(f"ℹ️  graph_degree_centrality: {e}")
            
        # Find shortest path from Alice to Charlie
        try:
            db.execute("SELECT graph_shortest_path(1, 3) as path")
            result = db.fetchone()
            if result['path']:
                print(f"🛤️  Shortest path Alice->Charlie: {result['path']}")
            else:
                print("🛤️  No path found from Alice to Charlie")
        except sqlite3.Error as e:
            print(f"ℹ️  graph_shortest_path: {e}")


def example_4_cypher_queries():
    """
    Example 4: Cypher-like Queries
    ===============================
    
    Shows how to use Cypher query syntax with the extension.
    """
    print("\n" + "="*60)
    print("EXAMPLE 4: Cypher-like Queries") 
    print("="*60)
    
    with GraphDB() as db:
        print("Testing Cypher query parsing and validation...")
        
        # Test basic Cypher parsing
        cypher_queries = [
            "RETURN 42",
            "RETURN 'hello world'", 
            "MATCH (n) RETURN n",
            "MATCH (n:Person) RETURN n.name",
            "CREATE (n:Person {name: 'Alice'})",
            "MATCH (a)-[r:KNOWS]->(b) RETURN a.name, b.name"
        ]
        
        for query in cypher_queries:
            try:
                db.execute("SELECT cypher_parse(?) as result", (query,))
                result = db.fetchone()
                print(f"✅ '{query}' -> Valid syntax")
            except sqlite3.Error as e:
                print(f"❌ '{query}' -> {e}")
                
        # Test Cypher validation
        print("\nValidating Cypher queries...")
        for query in cypher_queries:
            try:
                db.execute("SELECT cypher_validate(?) as valid", (query,))
                result = db.fetchone()
                status = "✅ Valid" if result['valid'] else "❌ Invalid"
                print(f"{status}: {query}")
            except sqlite3.Error as e:
                print(f"ℹ️  cypher_validate: {e}")


def example_5_write_operations():
    """
    Example 5: Write Operations (CREATE, MERGE, SET, DELETE)
    ========================================================
    
    Shows how to use Cypher write operations.
    """
    print("\n" + "="*60)
    print("EXAMPLE 5: Write Operations")
    print("="*60)
    
    with GraphDB() as db:
        print("Testing Cypher write operations...")
        
        # Begin a write transaction
        try:
            db.execute("SELECT cypher_begin_write() as result")
            result = db.fetchone()
            print(f"✅ Write transaction started: {result['result']}")
        except sqlite3.Error as e:
            print(f"ℹ️  cypher_begin_write: {e}")
            
        # Create a node
        try:
            db.execute("SELECT cypher_create_node(?, ?, ?) as node_id", 
                      (None, "Person", '{"name": "David", "age": 28}'))
            result = db.fetchone()
            david_id = result['node_id']
            print(f"✅ Created node David with ID: {david_id}")
        except sqlite3.Error as e:
            print(f"ℹ️  cypher_create_node: {e}")
            david_id = 4  # fallback
            
        # Merge a node (create if not exists, or match existing)
        try:
            db.execute("SELECT cypher_merge_node(?, ?, ?, ?, ?) as node_id",
                      (None, "Person", '["name"]', '{"name": "David"}', '{"age": 29}'))
            result = db.fetchone()
            print(f"✅ Merged node (should match existing David): {result['node_id']}")
        except sqlite3.Error as e:
            print(f"ℹ️  cypher_merge_node: {e}")
            
        # Set a property
        try:
            db.execute("SELECT cypher_set_property(?, ?, ?, ?) as result",
                      ("node", david_id, "location", "Seattle"))
            result = db.fetchone()
            print(f"✅ Set location property for David: {result['result']}")
        except sqlite3.Error as e:
            print(f"ℹ️  cypher_set_property: {e}")
            
        # Commit the transaction
        try:
            db.execute("SELECT cypher_commit_write() as result")
            result = db.fetchone()
            print(f"✅ Write transaction committed: {result['result']}")
        except sqlite3.Error as e:
            print(f"ℹ️  cypher_commit_write: {e}")


def example_6_social_network():
    """
    Example 6: Building a Social Network Graph
    ==========================================
    
    A practical example building a social network with users, posts, and relationships.
    """
    print("\n" + "="*60)
    print("EXAMPLE 6: Social Network Graph")
    print("="*60)
    
    with GraphDB() as db:
        print("Building a social network graph...")
        
        # Create users
        users = [
            (1, {"name": "Alice Johnson", "age": 28, "location": "NYC", "occupation": "Developer"}),
            (2, {"name": "Bob Smith", "age": 32, "location": "SF", "occupation": "Designer"}),
            (3, {"name": "Carol Davis", "age": 26, "location": "LA", "occupation": "Manager"}),
            (4, {"name": "David Wilson", "age": 30, "location": "Chicago", "occupation": "Analyst"}),
            (5, {"name": "Eve Brown", "age": 24, "location": "Austin", "occupation": "Student"})
        ]
        
        print("Creating user nodes...")
        for user_id, props in users:
            db.execute("SELECT graph_node_add(?, ?) as result", 
                      (user_id, json.dumps(props)))
            print(f"  ✅ User {props['name']} (ID: {user_id})")
            
        # Create friendships
        friendships = [
            (1, 2, {"since": "2020-01-15", "closeness": 0.8}),
            (1, 3, {"since": "2019-06-20", "closeness": 0.6}),
            (2, 4, {"since": "2021-03-10", "closeness": 0.9}),
            (3, 4, {"since": "2020-11-05", "closeness": 0.7}),
            (3, 5, {"since": "2021-08-12", "closeness": 0.5}),
            (4, 5, {"since": "2021-09-01", "closeness": 0.8})
        ]
        
        print("\nCreating friendship relationships...")
        for from_id, to_id, props in friendships:
            # Create bidirectional friendship
            db.execute("SELECT graph_edge_add(?, ?, ?, ?) as result",
                      (from_id, to_id, "FRIENDS", json.dumps(props)))
            db.execute("SELECT graph_edge_add(?, ?, ?, ?) as result", 
                      (to_id, from_id, "FRIENDS", json.dumps(props)))
            from_name = next(u[1]['name'] for u in users if u[0] == from_id)
            to_name = next(u[1]['name'] for u in users if u[0] == to_id)
            print(f"  🤝 {from_name} ↔ {to_name}")
            
        # Add some posts (using higher node IDs)
        posts = [
            (101, {"title": "Learning SQLite Extensions", "author_id": 1, "likes": 15}),
            (102, {"title": "Graph Database Benefits", "author_id": 2, "likes": 23}),
            (103, {"title": "Network Analysis Tips", "author_id": 3, "likes": 8})
        ]
        
        print("\nCreating post nodes...")
        for post_id, props in posts:
            db.execute("SELECT graph_node_add(?, ?) as result",
                      (post_id, json.dumps(props)))
            print(f"  📝 Post: {props['title']} (ID: {post_id})")
            
        # Connect posts to authors
        print("\nConnecting posts to authors...")
        for post_id, props in posts:
            author_id = props['author_id']
            db.execute("SELECT graph_edge_add(?, ?, ?, ?) as result",
                      (author_id, post_id, "AUTHORED", '{"created_at": "2023-01-01"}'))
            author_name = next(u[1]['name'] for u in users if u[0] == author_id)
            print(f"  ✍️  {author_name} authored post {post_id}")
            
        # Calculate network statistics
        print("\n📊 Social Network Statistics:")
        db.execute("SELECT graph_count_nodes() as nodes, graph_count_edges() as edges")
        stats = db.fetchone()
        print(f"   Total nodes: {stats['nodes']}")
        print(f"   Total edges: {stats['edges']}")
        
        # Find most connected user
        print("\n🌟 Most Connected Users:")
        for user_id, _ in users:
            try:
                db.execute("SELECT graph_degree_centrality(?) as centrality", (user_id,))
                result = db.fetchone()
                user_name = next(u[1]['name'] for u in users if u[0] == user_id)
                print(f"   {user_name}: {result['centrality']:.3f}")
            except sqlite3.Error:
                pass


def example_7_performance_testing():
    """
    Example 7: Performance Testing and Bulk Operations
    ==================================================
    
    Shows how to handle large graphs and performance considerations.
    """
    print("\n" + "="*60)
    print("EXAMPLE 7: Performance Testing")
    print("="*60)
    
    with GraphDB() as db:
        import time
        
        print("Creating a larger graph for performance testing...")
        
        # Create many nodes efficiently
        start_time = time.time()
        node_count = 1000
        
        print(f"Creating {node_count} nodes...")
        for i in range(1, node_count + 1):
            if i % 100 == 0:
                print(f"  Progress: {i}/{node_count}")
            db.execute("SELECT graph_node_add(?, ?) as result",
                      (i, json.dumps({"id": i, "type": "test_node", "value": i * 2})))
            
        node_time = time.time() - start_time
        print(f"✅ Created {node_count} nodes in {node_time:.2f} seconds")
        
        # Create edges in a ring topology
        start_time = time.time()
        edge_count = node_count - 1
        
        print(f"Creating {edge_count} edges...")
        for i in range(1, node_count):
            if i % 100 == 0:
                print(f"  Progress: {i}/{edge_count}")
            db.execute("SELECT graph_edge_add(?, ?, ?, ?) as result",
                      (i, i + 1, "NEXT", '{"weight": 1.0}'))
            
        edge_time = time.time() - start_time
        print(f"✅ Created {edge_count} edges in {edge_time:.2f} seconds")
        
        # Test query performance
        print(f"\n⏱️  Performance Results:")
        print(f"   Node creation: {node_count/node_time:.0f} nodes/second")
        print(f"   Edge creation: {edge_count/edge_time:.0f} edges/second")
        
        # Test algorithm performance on larger graph
        start_time = time.time()
        try:
            db.execute("SELECT graph_is_connected() as connected")
            result = db.fetchone()
            algo_time = time.time() - start_time
            print(f"   Connectivity check: {algo_time:.3f} seconds (connected: {result['connected']})")
        except sqlite3.Error as e:
            print(f"ℹ️  Connectivity check: {e}")


def main():
    """
    Main function to run all examples.
    """
    print("🗄️  SQLite Graph Database Extension - Python Examples")
    print("=" * 60)
    print("This script demonstrates how to use the SQLite Graph Extension from Python.")
    print("Make sure the graph extension (.so file) is compiled and available.")
    
    # Run all examples
    examples = [
        example_1_basic_setup,
        example_2_creating_nodes_and_edges, 
        example_3_graph_algorithms,
        example_4_cypher_queries,
        example_5_write_operations,
        example_6_social_network,
        example_7_performance_testing
    ]
    
    for i, example_func in enumerate(examples, 1):
        try:
            example_func()
        except Exception as e:
            print(f"\n❌ Example {i} failed: {e}")
            continue
    
    print(f"\n🎉 Examples completed! Check the output above for results.")
    print("💡 Tip: Modify these examples to experiment with your own graph data.")


if __name__ == "__main__":
    main()
