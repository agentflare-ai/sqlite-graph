#!/usr/bin/env python3
"""
Basic Cypher query test to see what's working
"""
import sqlite3
import sys

conn = sqlite3.connect(':memory:')
conn.enable_load_extension(True)

# Load extension
try:
    conn.load_extension('./build/libgraph')
    print("✅ Extension loaded\n")
except Exception as e:
    print(f"❌ Failed to load extension: {e}")
    sys.exit(1)

# Create graph virtual table
try:
    conn.execute("CREATE VIRTUAL TABLE graph USING graph()")
    print("✅ Graph virtual table created\n")
except Exception as e:
    print(f"❌ Failed to create graph: {e}")
    sys.exit(1)

# Test 1: CREATE query
print("="*60)
print("Test 1: CREATE (n)")
try:
    result = conn.execute("SELECT cypher_execute('CREATE (n)')").fetchone()
    print(f"✅ CREATE SUCCESS: {result}")
    count = conn.execute("SELECT graph_count_nodes()").fetchone()[0]
    print(f"   Node count: {count}\n")
except Exception as e:
    print(f"❌ CREATE FAILED: {e}\n")

# Test 2: CREATE with label
print("="*60)
print("Test 2: CREATE (p:Person)")
try:
    result = conn.execute("SELECT cypher_execute('CREATE (p:Person)')").fetchone()
    print(f"✅ CREATE with label SUCCESS: {result}")
    count = conn.execute("SELECT graph_count_nodes()").fetchone()[0]
    print(f"   Node count: {count}\n")
except Exception as e:
    print(f"❌ CREATE with label FAILED: {e}\n")

# Test 3: MATCH query
print("="*60)
print("Test 3: MATCH (n) RETURN n")
try:
    result = conn.execute("SELECT cypher_execute('MATCH (n) RETURN n')").fetchone()
    print(f"✅ MATCH RETURN SUCCESS: {result}\n")
except Exception as e:
    print(f"❌ MATCH RETURN FAILED: {e}\n")

# Test 4: MATCH with label
print("="*60)
print("Test 4: MATCH (p:Person) RETURN p")
try:
    result = conn.execute("SELECT cypher_execute('MATCH (p:Person) RETURN p')").fetchone()
    print(f"✅ MATCH label RETURN SUCCESS: {result}\n")
except Exception as e:
    print(f"❌ MATCH label RETURN FAILED: {e}\n")

# Test 5: Simple RETURN
print("="*60)
print("Test 5: RETURN 1")
try:
    result = conn.execute("SELECT cypher_execute('RETURN 1')").fetchone()
    print(f"✅ RETURN literal SUCCESS: {result}\n")
except Exception as e:
    print(f"❌ RETURN literal FAILED: {e}\n")

conn.close()
print("Tests completed")
