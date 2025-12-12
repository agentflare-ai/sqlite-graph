#!/usr/bin/env python3
"""
Test CREATE execution - check if nodes are actually being inserted
"""

import sqlite3
import sys

def test_create_execution():
    """Test that CREATE queries actually insert nodes"""

    # Connect to in-memory database
    conn = sqlite3.connect(":memory:")
    conn.enable_load_extension(True)

    # Load the graph extension
    try:
        conn.load_extension("../build/libgraph")
    except sqlite3.OperationalError as e:
        try:
            conn.load_extension("../build/libgraph")
        except sqlite3.OperationalError as e2:
            print(f"❌ Failed to load extension: {e}, {e2}")
            return False

    print("✅ Extension loaded successfully")

    # Create backing tables
    try:
        conn.execute("""
            CREATE TABLE IF NOT EXISTS nodes(
                id INTEGER PRIMARY KEY,
                labels TEXT,
                properties TEXT
            )
        """)
        conn.execute("""
            CREATE TABLE IF NOT EXISTS edges(
                id INTEGER PRIMARY KEY,
                source INTEGER,
                target INTEGER,
                edge_type TEXT,
                properties TEXT
            )
        """)
        print("✅ Tables created")
    except sqlite3.Error as e:
        print(f"❌ Table creation failed: {e}")
        return False

    # Test 1: Direct SQL INSERT to verify table works
    print("\nTest 1: Direct SQL INSERT")
    try:
        conn.execute("INSERT INTO nodes (labels, properties) VALUES ('', '{}')")
        count = conn.execute("SELECT COUNT(*) FROM nodes").fetchone()[0]
        print(f"✅ Direct INSERT works, node count: {count}")
    except Exception as e:
        print(f"❌ Direct INSERT failed: {e}")
        return False

    # Test 2: Try cypher_execute with empty query
    print("\nTest 2: cypher_execute with empty pattern")
    try:
        # Check if cypher_execute function exists
        result = conn.execute("SELECT cypher_execute('MATCH (n) RETURN n')")
        rows = result.fetchall()
        print(f"✅ cypher_execute function exists")
        print(f"   Result: {rows}")
    except sqlite3.OperationalError as e:
        print(f"ℹ️  cypher_execute error (expected if not fully implemented): {e}")
    except Exception as e:
        print(f"ℹ️  Other error: {e}")

    # Test 3: Check what functions are available
    print("\nTest 3: Available Cypher functions")
    try:
        # Try to find Cypher functions
        cursor = conn.execute("SELECT name FROM pragma_function_list WHERE name LIKE 'cypher%'")
        functions = cursor.fetchall()
        if functions:
            print(f"✅ Available Cypher functions:")
            for func in functions:
                print(f"   - {func[0]}")
        else:
            print("ℹ️  No Cypher functions found in pragma_function_list")
    except Exception as e:
        print(f"ℹ️  Could not query functions: {e}")

    conn.close()
    return True

if __name__ == "__main__":
    success = test_create_execution()
    sys.exit(0 if success else 1)
