import os
import sqlite3
import click
import sqlite_utils


@sqlite_utils.hookimpl
def prepare_connection(conn):
    """Load the SQLite graph extension when a connection is created."""
    # Try to load the extension from various possible locations
    extension_paths = [
        "./build/libgraph.so",  # Local build
        "./libgraph.so",  # Current directory
        "/usr/local/lib/libgraph.so",  # System-wide install
        "/usr/lib/libgraph.so",  # Alternative system location
    ]
    
    for path in extension_paths:
        if os.path.exists(path):
            try:
                conn.enable_load_extension(True)
                conn.load_extension(path)
                return
            except Exception as e:
                continue
    
    # If no extension found, warn the user
    click.echo(
        "Warning: SQLite graph extension not found. "
        "Please ensure libgraph.so is in your path or build directory.",
        err=True
    )


@sqlite_utils.hookimpl
def register_commands(cli):
    """Register graph-related commands with sqlite-utils."""
    
    @cli.command()
    @click.argument("database")
    @click.option(
        "--cypher", "-c",
        help="Execute a Cypher query"
    )
    @click.option(
        "--output", "-o",
        type=click.Choice(["table", "json", "csv"]),
        default="table",
        help="Output format"
    )
    @click.pass_context
    def graph(ctx, database, cypher, output):
        """Execute Cypher queries on a SQLite graph database."""
        db = sqlite_utils.Database(database)
        
        # Ensure the graph extension is loaded
        prepare_connection(db.conn)
        
        # Create the graph virtual table if it doesn't exist
        try:
            db.conn.execute("CREATE VIRTUAL TABLE IF NOT EXISTS graph USING graph()")
        except sqlite3.OperationalError as e:
            click.echo(f"Error creating graph table: {e}", err=True)
            return
        
        if cypher:
            try:
                # Execute the Cypher query using the cypher_execute function
                cursor = db.conn.execute("SELECT cypher_execute(?) as result", (cypher,))
                results = cursor.fetchall()
                
                if output == "json":
                    import json
                    # Convert results to JSON format
                    column_names = [desc[0] for desc in cursor.description] if cursor.description else []
                    json_results = [dict(zip(column_names, row)) for row in results]
                    click.echo(json.dumps(json_results, indent=2))
                elif output == "csv":
                    import csv
                    import sys
                    writer = csv.writer(sys.stdout)
                    if cursor.description:
                        writer.writerow([desc[0] for desc in cursor.description])
                    writer.writerows(results)
                else:
                    # Table format (default)
                    if results:
                        # Print column headers if available
                        if cursor.description:
                            headers = [desc[0] for desc in cursor.description]
                            click.echo("\t".join(headers))
                            click.echo("\t".join(["-" * len(h) for h in headers]))
                        
                        # Print rows
                        for row in results:
                            click.echo("\t".join(str(cell) for cell in row))
                    else:
                        click.echo("No results returned.")
                        
            except Exception as e:
                click.echo(f"Error executing Cypher query: {e}", err=True)
        else:
            # Interactive mode or show graph info
            try:
                # Show some basic graph statistics using graph functions
                node_count = db.conn.execute("SELECT graph_count_nodes() as count").fetchone()[0]
                edge_count = db.conn.execute("SELECT graph_count_edges() as count").fetchone()[0]
                
                click.echo(f"Graph Database: {database}")
                click.echo(f"Nodes: {node_count}")
                click.echo(f"Edges: {edge_count}")
                click.echo("\nUse --cypher to execute Cypher queries.")
                
            except Exception as e:
                click.echo(f"Graph table not initialized. Use CREATE VIRTUAL TABLE graph USING graph() first.", err=True)

    @cli.command()
    @click.argument("database")
    @click.option(
        "--nodes", "-n",
        type=int,
        help="Number of sample nodes to create"
    )
    @click.option(
        "--edges", "-e", 
        type=int,
        help="Number of sample edges to create"
    )
    def graph_sample(database, nodes, edges):
        """Create sample graph data for testing."""
        db = sqlite_utils.Database(database)
        
        # Ensure the graph extension is loaded
        prepare_connection(db.conn)
        
        # Create the graph virtual table
        try:
            db.conn.execute("CREATE VIRTUAL TABLE IF NOT EXISTS graph USING graph()")
        except sqlite3.OperationalError as e:
            click.echo(f"Error creating graph table: {e}", err=True)
            return
        
        # Create sample nodes
        if nodes:
            import json
            click.echo(f"Creating {nodes} sample nodes...")
            for i in range(nodes):
                node_data = {"name": f"Person_{i}", "id": i}
                db.conn.execute("SELECT graph_node_add(?, ?) as result", (i, json.dumps(node_data)))
        
        # Create sample edges
        if edges and nodes:
            import random
            click.echo(f"Creating {edges} sample edges...")
            for i in range(edges):
                from_id = random.randint(0, nodes - 1)
                to_id = random.randint(0, nodes - 1)
                if from_id != to_id:
                    edge_data = {"relationship": "KNOWS"}
                    try:
                        db.conn.execute("SELECT graph_edge_add(?, ?, ?, ?) as result", 
                                      (from_id, to_id, "KNOWS", json.dumps(edge_data)))
                    except:
                        pass  # Ignore duplicate edges
        
        click.echo("Sample data created successfully!")

    @cli.command()
    @click.argument("database")
    def graph_info(database):
        """Show information about the graph database."""
        db = sqlite_utils.Database(database)
        
        # Ensure the graph extension is loaded
        prepare_connection(db.conn)
        
        try:
            # Check if graph table exists
            tables = db.table_names()
            if "graph" not in tables:
                click.echo("No graph table found. Create one with: CREATE VIRTUAL TABLE graph USING graph()")
                return
            
            # Get basic statistics using graph functions
            node_count = db.conn.execute("SELECT graph_count_nodes() as count").fetchone()[0]
            edge_count = db.conn.execute("SELECT graph_count_edges() as count").fetchone()[0]
            
            click.echo(f"Graph Database: {database}")
            click.echo("-" * 40)
            click.echo(f"Nodes: {node_count}")
            click.echo(f"Edges: {edge_count}")
            
            # Show additional graph properties
            try:
                is_connected = db.conn.execute("SELECT graph_is_connected() as connected").fetchone()[0]
                click.echo(f"Connected: {bool(is_connected)}")
            except Exception:
                pass  # Function might not be available
            
            try:
                density = db.conn.execute("SELECT graph_density() as density").fetchone()[0]
                click.echo(f"Density: {density:.3f}")
            except Exception:
                pass  # Function might not be available
            
        except Exception as e:
            click.echo(f"Error getting graph info: {e}", err=True)
