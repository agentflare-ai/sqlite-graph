.load ./build/libgraph.dylib
CREATE VIRTUAL TABLE graph USING graph();
SELECT cypher_execute('CREATE (n)');
SELECT graph_count_nodes();
SELECT cypher_execute('MATCH (n) RETURN n');
