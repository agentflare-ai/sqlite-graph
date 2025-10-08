#!/usr/bin/env python3
"""
SQLite Graph Extension - Cypher Language Demo
==============================================

This example demonstrates the graph functionality currently available
and shows how to use it as a foundation for Cypher-like operations.

While the full Cypher SQL functions are still being integrated, this shows:
1. Working graph operations (nodes, edges, algorithms)
2. How to build Cypher-like functionality on top
3. A roadmap for full Cypher integration
"""

import sqlite3
import json
import os
from typing import List, Dict, Any, Optional


class CypherGraphDB:
    """
    A demonstration of Cypher-like operations using the SQLite Graph Extension.
    
    This class shows how the current graph functionality can be used to implement
    Cypher-like operations manually, serving as a foundation for when the full
    Cypher integration is completed.
    """
    
    def __init__(self, db_path: str = ":memory:"):
        self.conn = sqlite3.connect(db_path)
        self.conn.row_factory = sqlite3.Row
        
        # Load the graph extension
        extension_path = "../build/libgraph.so"
        if os.path.exists(extension_path):
            self.conn.enable_load_extension(True)
            self.conn.load_extension(extension_path)
            print(f"✅ Graph extension loaded successfully!")
        else:
            raise Exception(f"❌ Graph extension not found: {extension_path}")
            
        self.cursor = self.conn.cursor()
        
        # Create virtual graph table
        try:
            self.cursor.execute("CREATE VIRTUAL TABLE graph USING graph()")
            print("✅ Virtual graph table created")
        except sqlite3.Error as e:
            print(f"ℹ️  Virtual table: {e}")
            
        # Store node and relationship metadata for Cypher-like operations
        self.nodes = {}  # node_id -> {labels, properties}
        self.relationships = {}  # edge_id -> {type, properties, from_id, to_id}
        self.next_node_id = 1
        self.next_edge_id = 1
        
    def __enter__(self):
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        
    def close(self):
        if self.conn:
            self.conn.close()
            
    # Core Graph Operations (Working)
    def create_node(self, labels: List[str] = None, properties: Dict[str, Any] = None) -> int:
        """Create a node with labels and properties (Cypher: CREATE (n:Label {props}))"""
        node_id = self.next_node_id
        self.next_node_id += 1
        
        # Store metadata
        self.nodes[node_id] = {
            'labels': labels or [],
            'properties': properties or {}
        }
        
        # Create in graph database
        node_data = {
            'labels': labels or [],
            'properties': properties or {}
        }
        
        self.cursor.execute("SELECT graph_node_add(?, ?) as result", 
                           (node_id, json.dumps(node_data)))
        result = self.cursor.fetchone()
        
        print(f"📍 Created node {node_id} with labels {labels} and {len(properties or {})} properties")
        return node_id
        
    def create_relationship(self, from_node: int, to_node: int, rel_type: str, 
                          properties: Dict[str, Any] = None) -> int:
        """Create a relationship (Cypher: CREATE (a)-[r:TYPE {props}]->(b))"""
        edge_id = self.next_edge_id
        self.next_edge_id += 1
        
        # Store metadata
        self.relationships[edge_id] = {
            'type': rel_type,
            'properties': properties or {},
            'from_id': from_node,
            'to_id': to_node
        }
        
        # Create in graph database
        edge_data = {
            'type': rel_type,
            'properties': properties or {}
        }
        
        self.cursor.execute("SELECT graph_edge_add(?, ?, ?, ?) as result",
                           (from_node, to_node, rel_type, json.dumps(edge_data)))
        result = self.cursor.fetchone()
        
        print(f"🔗 Created relationship {edge_id}: ({from_node})-[:{rel_type}]->({to_node})")
        return edge_id
        
    def match_nodes(self, labels: List[str] = None, properties: Dict[str, Any] = None) -> List[int]:
        """Find nodes matching criteria (Cypher: MATCH (n:Label {props}))"""
        matching_nodes = []
        
        for node_id, node_data in self.nodes.items():
            # Check labels
            if labels:
                if not all(label in node_data['labels'] for label in labels):
                    continue
                    
            # Check properties
            if properties:
                node_props = node_data['properties']
                if not all(node_props.get(k) == v for k, v in properties.items()):
                    continue
                    
            matching_nodes.append(node_id)
            
        print(f"🔍 Found {len(matching_nodes)} nodes matching criteria")
        return matching_nodes
        
    def get_relationships(self, from_node: int = None, to_node: int = None, 
                         rel_type: str = None) -> List[Dict]:
        """Find relationships matching criteria (Cypher: MATCH ()-[r:TYPE]->())"""
        matching_rels = []
        
        for edge_id, rel_data in self.relationships.items():
            if from_node and rel_data['from_id'] != from_node:
                continue
            if to_node and rel_data['to_id'] != to_node:
                continue
            if rel_type and rel_data['type'] != rel_type:
                continue
                
            matching_rels.append({
                'id': edge_id,
                'from': rel_data['from_id'],
                'to': rel_data['to_id'],
                'type': rel_data['type'],
                'properties': rel_data['properties']
            })
            
        print(f"🔍 Found {len(matching_rels)} relationships matching criteria")
        return matching_rels
        
    # Graph Algorithms (Working)
    def shortest_path(self, from_node: int, to_node: int) -> Optional[str]:
        """Find shortest path between nodes (Cypher: shortestPath((a)-[*]-(b)))"""
        try:
            self.cursor.execute("SELECT graph_shortest_path(?, ?) as path", (from_node, to_node))
            result = self.cursor.fetchone()
            return result['path'] if result['path'] else None
        except sqlite3.Error as e:
            print(f"ℹ️  Shortest path: {e}")
            return None
            
    def node_degree_centrality(self, node_id: int) -> float:
        """Calculate node centrality (Cypher extension)"""
        try:
            self.cursor.execute("SELECT graph_degree_centrality(?) as centrality", (node_id,))
            result = self.cursor.fetchone()
            return float(result['centrality'])
        except sqlite3.Error as e:
            print(f"ℹ️  Centrality: {e}")
            return 0.0
            
    def graph_stats(self) -> Dict[str, Any]:
        """Get graph statistics (Cypher: RETURN count(n), count(r))"""
        stats = {}
        
        try:
            self.cursor.execute("SELECT graph_count_nodes() as nodes, graph_count_edges() as edges")
            result = self.cursor.fetchone()
            stats['nodes'] = result['nodes']
            stats['edges'] = result['edges']
        except sqlite3.Error as e:
            print(f"ℹ️  Graph stats: {e}")
            stats['nodes'] = len(self.nodes)
            stats['edges'] = len(self.relationships)
            
        try:
            self.cursor.execute("SELECT graph_density() as density")
            result = self.cursor.fetchone()
            stats['density'] = float(result['density'])
        except sqlite3.Error as e:
            stats['density'] = 0.0
            
        try:
            self.cursor.execute("SELECT graph_is_connected() as connected")
            result = self.cursor.fetchone()
            stats['connected'] = bool(result['connected'])
        except sqlite3.Error as e:
            stats['connected'] = False
            
        return stats
        
    # Cypher-like Query Interface (Demonstration)
    def cypher_query(self, query: str) -> List[Dict]:
        """
        A demonstration of how Cypher queries could be implemented.
        
        This is a simplified parser for common Cypher patterns.
        In the full implementation, this would use the Cypher parser and executor.
        """
        query = query.strip()
        
        # Simple CREATE node pattern
        if query.startswith("CREATE (") and "):" in query and "{" in query:
            # Example: CREATE (n:Person {name: 'Alice', age: 30})
            return self._parse_create_node(query)
            
        # Simple MATCH pattern  
        elif query.startswith("MATCH (") and "RETURN" in query:
            # Example: MATCH (n:Person) RETURN n
            return self._parse_match_return(query)
            
        # Simple CREATE relationship pattern
        elif query.startswith("CREATE (") and ")-[" in query and "]->(" in query:
            # Example: CREATE (a)-[r:KNOWS]->(b)  
            return self._parse_create_relationship(query)
            
        else:
            print(f"⚠️  Query pattern not yet implemented: {query}")
            return []
            
    def _parse_create_node(self, query: str) -> List[Dict]:
        """Parse CREATE (n:Label {props}) pattern"""
        try:
            # Extract label
            label_start = query.find(":") + 1
            label_end = query.find(" ", label_start)
            if label_end == -1:
                label_end = query.find("{", label_start)
            label = query[label_start:label_end].strip()
            
            # Extract properties (simplified)
            props_start = query.find("{") + 1
            props_end = query.rfind("}")
            props_str = query[props_start:props_end]
            
            # Parse simple properties (name: 'value', age: 30)
            properties = {}
            if props_str.strip():
                for prop in props_str.split(","):
                    if ":" in prop:
                        key, value = prop.split(":", 1)
                        key = key.strip().strip("'\"")
                        value = value.strip().strip("'\"")
                        # Try to convert to appropriate type
                        try:
                            if value.isdigit():
                                properties[key] = int(value)
                            elif value.replace(".", "").isdigit():
                                properties[key] = float(value)
                            else:
                                properties[key] = value
                        except:
                            properties[key] = value
            
            # Create the node
            node_id = self.create_node([label], properties)
            return [{'node_id': node_id, 'action': 'created'}]
            
        except Exception as e:
            print(f"❌ Error parsing CREATE query: {e}")
            return []
            
    def _parse_match_return(self, query: str) -> List[Dict]:
        """Parse MATCH (n:Label) RETURN n pattern"""
        try:
            # Extract label if present
            labels = []
            if ":" in query:
                label_start = query.find(":") + 1
                label_end = query.find(")", label_start)
                label = query[label_start:label_end].strip()
                if label:
                    labels = [label]
            
            # Find matching nodes
            node_ids = self.match_nodes(labels)
            
            # Return node data
            results = []
            for node_id in node_ids:
                node_data = self.nodes[node_id]
                results.append({
                    'node_id': node_id,
                    'labels': node_data['labels'],
                    'properties': node_data['properties']
                })
                
            return results
            
        except Exception as e:
            print(f"❌ Error parsing MATCH query: {e}")
            return []
            
    def _parse_create_relationship(self, query: str) -> List[Dict]:
        """Parse CREATE (a)-[r:TYPE]->(b) pattern (simplified)"""
        print(f"⚠️  CREATE relationship parsing not fully implemented yet")
        return []


def demo_cypher_operations():
    """Demonstrate Cypher-like operations using the graph extension."""
    print("🎯 SQLite Graph Extension - Cypher Operations Demo")
    print("=" * 60)
    
    with CypherGraphDB() as db:
        # Create nodes using Cypher-like syntax
        print("\n1. Creating nodes with Cypher-like operations:")
        
        # Direct API calls
        alice_id = db.create_node(['Person'], {'name': 'Alice', 'age': 30, 'city': 'NYC'})
        bob_id = db.create_node(['Person'], {'name': 'Bob', 'age': 25, 'city': 'SF'})
        carol_id = db.create_node(['Person', 'Developer'], {'name': 'Carol', 'age': 28, 'city': 'LA'})
        
        # Using Cypher query interface (demonstration)
        print("\n2. Using Cypher query interface:")
        result = db.cypher_query("CREATE (d:Person {name: 'David', age: 32})")
        print(f"   Query result: {result}")
        
        # Create relationships
        print("\n3. Creating relationships:")
        db.create_relationship(alice_id, bob_id, 'FRIENDS', {'since': '2020'})
        db.create_relationship(alice_id, carol_id, 'WORKS_WITH', {'project': 'GraphDB'})
        db.create_relationship(bob_id, carol_id, 'KNOWS', {'met_at': 'Conference'})
        
        # Query nodes
        print("\n4. Querying nodes:")
        result = db.cypher_query("MATCH (n:Person) RETURN n")
        print(f"   Found {len(result)} Person nodes:")
        for node in result[:3]:  # Show first 3
            print(f"     Node {node['node_id']}: {node['properties']['name']}")
        
        # Find specific nodes
        print("\n5. Finding specific nodes:")
        developers = db.match_nodes(['Developer'])
        print(f"   Found {len(developers)} developers: {developers}")
        
        # Analyze relationships
        print("\n6. Analyzing relationships:")
        friendships = db.get_relationships(rel_type='FRIENDS')
        print(f"   Found {len(friendships)} friendships")
        
        # Graph algorithms
        print("\n7. Running graph algorithms:")
        stats = db.graph_stats()
        print(f"   Graph statistics: {stats}")
        
        # Path finding
        print(f"\n8. Path finding:")
        path = db.shortest_path(alice_id, carol_id)
        print(f"   Shortest path from Alice to Carol: {path}")
        
        # Centrality analysis
        print(f"\n9. Centrality analysis:")
        alice_centrality = db.node_degree_centrality(alice_id)
        bob_centrality = db.node_degree_centrality(bob_id)
        carol_centrality = db.node_degree_centrality(carol_id)
        
        print(f"   Alice centrality: {alice_centrality:.3f}")
        print(f"   Bob centrality: {bob_centrality:.3f}")
        print(f"   Carol centrality: {carol_centrality:.3f}")


def demo_roadmap():
    """Show the roadmap for full Cypher integration."""
    print("\n" + "🛣️" + "="*59)
    print("ROADMAP: Full Cypher Language Integration")
    print("="*61)
    
    print("\n✅ CURRENTLY WORKING:")
    print("   • Graph node and edge creation")
    print("   • Graph algorithms (shortest path, centrality, etc.)")
    print("   • Basic graph statistics and analysis")
    print("   • Virtual table interface")
    print("   • Memory management and persistence")
    
    print("\n🚧 IN PROGRESS:")
    print("   • Cypher parser integration (function interface fixes needed)")
    print("   • Cypher query executor (interface alignment)")
    print("   • Write operations (CREATE, MERGE, SET, DELETE)")
    print("   • Query planner optimization")
    
    print("\n🎯 COMING NEXT:")
    print("   • Full Cypher query support:")
    print("     - SELECT cypher_parse('MATCH (n) RETURN n')")
    print("     - SELECT cypher_execute('CREATE (n:Person {name: \"Alice\"})')")
    print("   • Advanced pattern matching")
    print("   • Cypher aggregation functions")
    print("   • Query optimization and indexing")
    
    print("\n💡 HOW TO HELP:")
    print("   • Test the current graph operations")
    print("   • Report issues with function interfaces")
    print("   • Contribute to the Cypher parser integration")
    print("   • Help with performance optimization")
    
    print("\n📚 RESOURCES:")
    print("   • Check tests/ directory for working examples")
    print("   • See src/cypher/ for parser implementation")
    print("   • Review include/cypher.h for API definitions")


if __name__ == "__main__":
    try:
        demo_cypher_operations()
        demo_roadmap()
        
        print("\n🎉 Demo completed successfully!")
        print("\n💡 Next steps:")
        print("   • Try modifying the node/relationship data")
        print("   • Experiment with the graph algorithms")
        print("   • Help integrate the full Cypher functionality")
        print("   • Build your own graph applications!")
        
    except Exception as e:
        print(f"\n❌ Demo failed: {e}")
        print("\n🔧 Troubleshooting:")
        print("   • Make sure the graph extension is compiled")
        print("   • Check that libgraph.so is in the build/ directory")
        print("   • Verify you're running from the examples/ directory")
