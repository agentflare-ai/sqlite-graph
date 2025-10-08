# SQLite Graph Extension Tutorials

**Part of the AgentFlare AI ecosystem**

## Table of Contents

1. [Getting Started](#getting-started)
2. [Tutorial 1: Building a Social Network](#tutorial-1-building-a-social-network)
3. [Tutorial 2: Creating a Knowledge Graph](#tutorial-2-creating-a-knowledge-graph)
4. [Tutorial 3: Route Planning System](#tutorial-3-route-planning-system)
5. [Tutorial 4: Recommendation Engine](#tutorial-4-recommendation-engine)
6. [Tutorial 5: Fraud Detection](#tutorial-5-fraud-detection)
7. [Tutorial 6: Organization Hierarchy](#tutorial-6-organization-hierarchy)
8. [Advanced Topics](#advanced-topics)

## Getting Started

### Installation

```bash
# Build the extension
make

# Or use CMake
mkdir build && cd build
cmake ..
make

# The extension file will be at:
# ./graph_extension.so (Linux)
# ./graph_extension.dylib (macOS)
# ./graph_extension.dll (Windows)
```

### Loading the Extension

```sql
-- Start SQLite
sqlite3 mydatabase.db

-- Load the extension
.load ./graph_extension

-- Verify installation
SELECT sqlite_version();
SELECT graph_version();
```

### Basic Concepts

1. **Nodes**: Entities in your graph (people, places, things)
2. **Edges**: Relationships between nodes
3. **Labels**: Categories for nodes (Person, Company, Product)
4. **Properties**: Key-value data on nodes and edges
5. **Cypher**: Query language for graph operations

## Tutorial 1: Building a Social Network

### Objective
Create a social network graph to analyze friendships, find mutual friends, and identify influencers.

### Step 1: Create the Graph

```sql
-- Create a virtual table for our social network
CREATE VIRTUAL TABLE social_network USING graph();

-- Verify creation
.tables
```

### Step 2: Add Users (Nodes)

```sql
-- Insert users with properties
INSERT INTO social_network (node_id, labels, properties) VALUES
    (1, '["Person", "User"]', '{"name": "Alice Johnson", "age": 28, "city": "New York", "joined": "2020-01-15"}'),
    (2, '["Person", "User"]', '{"name": "Bob Smith", "age": 32, "city": "San Francisco", "joined": "2019-06-20"}'),
    (3, '["Person", "User"]', '{"name": "Charlie Brown", "age": 25, "city": "New York", "joined": "2021-03-10"}'),
    (4, '["Person", "User"]', '{"name": "Diana Prince", "age": 30, "city": "Los Angeles", "joined": "2020-11-05"}'),
    (5, '["Person", "User"]', '{"name": "Eve Wilson", "age": 27, "city": "San Francisco", "joined": "2021-01-20"}'),
    (6, '["Person", "User"]', '{"name": "Frank Miller", "age": 35, "city": "New York", "joined": "2018-09-12"}'),
    (7, '["Person", "User"]', '{"name": "Grace Lee", "age": 29, "city": "Los Angeles", "joined": "2020-07-18"}'),
    (8, '["Person", "User"]', '{"name": "Henry Ford", "age": 31, "city": "San Francisco", "joined": "2019-12-01"}');

-- Verify users were added
SELECT node_id, json_extract(properties, '$.name') as name,
       json_extract(properties, '$.city') as city
FROM social_network
WHERE edge_id IS NULL;
```

### Step 3: Create Friendships (Edges)

```sql
-- Add friendship relationships
INSERT INTO social_network (edge_id, from_id, to_id, edge_type, weight, properties) VALUES
    -- Alice's friends
    (101, 1, 2, 'FRIEND', 0.9, '{"since": "2020-02-01", "interaction": "high"}'),
    (102, 1, 3, 'FRIEND', 0.7, '{"since": "2021-04-15", "interaction": "medium"}'),
    (103, 1, 4, 'FRIEND', 0.8, '{"since": "2020-12-20", "interaction": "high"}'),
    
    -- Bob's friends
    (104, 2, 1, 'FRIEND', 0.9, '{"since": "2020-02-01", "interaction": "high"}'),
    (105, 2, 5, 'FRIEND', 0.6, '{"since": "2021-02-10", "interaction": "low"}'),
    (106, 2, 8, 'FRIEND', 0.8, '{"since": "2020-01-05", "interaction": "medium"}'),
    
    -- Charlie's friends
    (107, 3, 1, 'FRIEND', 0.7, '{"since": "2021-04-15", "interaction": "medium"}'),
    (108, 3, 6, 'FRIEND', 0.5, '{"since": "2021-05-20", "interaction": "low"}'),
    
    -- Diana's friends
    (109, 4, 1, 'FRIEND', 0.8, '{"since": "2020-12-20", "interaction": "high"}'),
    (110, 4, 7, 'FRIEND', 0.9, '{"since": "2020-11-10", "interaction": "high"}'),
    
    -- Additional friendships
    (111, 5, 2, 'FRIEND', 0.6, '{"since": "2021-02-10", "interaction": "low"}'),
    (112, 5, 8, 'FRIEND', 0.7, '{"since": "2021-03-01", "interaction": "medium"}'),
    (113, 6, 3, 'FRIEND', 0.5, '{"since": "2021-05-20", "interaction": "low"}'),
    (114, 6, 7, 'FRIEND', 0.6, '{"since": "2019-01-15", "interaction": "medium"}'),
    (115, 7, 4, 'FRIEND', 0.9, '{"since": "2020-11-10", "interaction": "high"}'),
    (116, 7, 6, 'FRIEND', 0.6, '{"since": "2019-01-15", "interaction": "medium"}'),
    (117, 8, 2, 'FRIEND', 0.8, '{"since": "2020-01-05", "interaction": "medium"}'),
    (118, 8, 5, 'FRIEND', 0.7, '{"since": "2021-03-01", "interaction": "medium"}');
```

### Step 4: Query the Social Network

#### Find Direct Friends
```sql
-- Find Alice's friends using SQL
SELECT 
    n2.node_id,
    json_extract(n2.properties, '$.name') as friend_name,
    e.weight as friendship_strength,
    json_extract(e.properties, '$.since') as friends_since
FROM social_network e
JOIN social_network n1 ON n1.node_id = e.from_id
JOIN social_network n2 ON n2.node_id = e.to_id
WHERE n1.node_id = 1 AND e.edge_type = 'FRIEND'
ORDER BY e.weight DESC;

-- Using Cypher
SELECT cypher_query('social_network', '
    MATCH (alice:Person {name: "Alice Johnson"})-[f:FRIEND]->(friend:Person)
    RETURN friend.name as friend_name, 
           f.weight as strength,
           f.since as friends_since
    ORDER BY f.weight DESC
');
```

#### Find Mutual Friends
```sql
-- Find mutual friends between Alice and Bob
WITH alice_friends AS (
    SELECT to_id as friend_id 
    FROM social_network 
    WHERE from_id = 1 AND edge_type = 'FRIEND'
),
bob_friends AS (
    SELECT to_id as friend_id 
    FROM social_network 
    WHERE from_id = 2 AND edge_type = 'FRIEND'
)
SELECT 
    n.node_id,
    json_extract(n.properties, '$.name') as mutual_friend
FROM alice_friends a
JOIN bob_friends b ON a.friend_id = b.friend_id
JOIN social_network n ON n.node_id = a.friend_id
WHERE n.edge_id IS NULL;
```

#### Find Friend Recommendations
```sql
-- Find friend-of-friend recommendations for Alice
WITH RECURSIVE friend_network AS (
    -- Start with Alice
    SELECT 1 as user_id, 0 as distance
    
    UNION ALL
    
    -- Find friends at each level
    SELECT 
        CASE 
            WHEN e.from_id = fn.user_id THEN e.to_id
            ELSE e.from_id
        END as user_id,
        fn.distance + 1 as distance
    FROM friend_network fn
    JOIN social_network e ON (
        e.edge_type = 'FRIEND' AND 
        (e.from_id = fn.user_id OR e.to_id = fn.user_id)
    )
    WHERE fn.distance < 2
)
SELECT DISTINCT
    n.node_id,
    json_extract(n.properties, '$.name') as recommended_friend,
    json_extract(n.properties, '$.city') as city,
    MIN(distance) as connection_distance
FROM friend_network fn
JOIN social_network n ON n.node_id = fn.user_id
WHERE fn.user_id != 1  -- Exclude Alice
  AND fn.distance = 2  -- Only friend-of-friends
  AND n.edge_id IS NULL
  -- Exclude existing friends
  AND NOT EXISTS (
      SELECT 1 FROM social_network e 
      WHERE e.edge_type = 'FRIEND' 
        AND ((e.from_id = 1 AND e.to_id = fn.user_id) 
          OR (e.from_id = fn.user_id AND e.to_id = 1))
  )
GROUP BY n.node_id
ORDER BY connection_distance;
```

#### Identify Influencers (PageRank)
```sql
-- Calculate influence scores
SELECT 
    n.node_id,
    json_extract(n.properties, '$.name') as name,
    pr.rank as influence_score,
    pr.outgoing_edges as connections
FROM graph_pagerank('social_network', 0.85, 100) pr
JOIN social_network n ON n.node_id = pr.node_id
WHERE n.edge_id IS NULL
ORDER BY pr.rank DESC
LIMIT 5;
```

#### Find Shortest Path Between Users
```sql
-- Find shortest path from Alice to Henry
SELECT 
    d.*,
    (
        SELECT json_group_array(json_extract(n.properties, '$.name'))
        FROM json_each(d.path) p
        JOIN social_network n ON n.node_id = p.value
    ) as path_names
FROM graph_dijkstra('social_network', 1, 8) d
WHERE d.node_id = 8;
```

### Step 5: Analyze Network Statistics

```sql
-- Overall network statistics
SELECT 
    (SELECT COUNT(*) FROM social_network WHERE edge_id IS NULL) as total_users,
    (SELECT COUNT(*) FROM social_network WHERE edge_id IS NOT NULL) as total_friendships,
    (SELECT AVG(weight) FROM social_network WHERE edge_id IS NOT NULL) as avg_friendship_strength,
    (SELECT COUNT(DISTINCT json_extract(properties, '$.city')) 
     FROM social_network WHERE edge_id IS NULL) as cities_represented;

-- User statistics
SELECT 
    json_extract(n.properties, '$.city') as city,
    COUNT(*) as user_count,
    AVG(CAST(json_extract(n.properties, '$.age') as INTEGER)) as avg_age
FROM social_network n
WHERE n.edge_id IS NULL
GROUP BY city
ORDER BY user_count DESC;

-- Friendship patterns
SELECT 
    json_extract(e.properties, '$.interaction') as interaction_level,
    COUNT(*) as friendship_count,
    AVG(e.weight) as avg_strength
FROM social_network e
WHERE e.edge_id IS NOT NULL
GROUP BY interaction_level;
```

## Tutorial 2: Creating a Knowledge Graph

### Objective
Build a knowledge graph to represent concepts, their relationships, and enable semantic queries.

### Step 1: Create the Knowledge Graph

```sql
CREATE VIRTUAL TABLE knowledge_graph USING graph();
```

### Step 2: Add Concepts (Nodes)

```sql
-- Programming languages
INSERT INTO knowledge_graph (node_id, labels, properties) VALUES
    (1001, '["Concept", "ProgrammingLanguage"]', 
     '{"name": "Python", "paradigm": "multi-paradigm", "year": 1991, "creator": "Guido van Rossum"}'),
    (1002, '["Concept", "ProgrammingLanguage"]', 
     '{"name": "JavaScript", "paradigm": "multi-paradigm", "year": 1995, "creator": "Brendan Eich"}'),
    (1003, '["Concept", "ProgrammingLanguage"]', 
     '{"name": "Java", "paradigm": "object-oriented", "year": 1995, "creator": "James Gosling"}'),
    (1004, '["Concept", "ProgrammingLanguage"]', 
     '{"name": "Rust", "paradigm": "multi-paradigm", "year": 2010, "creator": "Graydon Hoare"}');

-- Technology domains
INSERT INTO knowledge_graph (node_id, labels, properties) VALUES
    (2001, '["Concept", "Domain"]', '{"name": "Web Development", "description": "Building web applications"}'),
    (2002, '["Concept", "Domain"]', '{"name": "Data Science", "description": "Analyzing and interpreting data"}'),
    (2003, '["Concept", "Domain"]', '{"name": "Systems Programming", "description": "Low-level programming"}'),
    (2004, '["Concept", "Domain"]', '{"name": "Machine Learning", "description": "Building intelligent systems"}');

-- Frameworks and libraries
INSERT INTO knowledge_graph (node_id, labels, properties) VALUES
    (3001, '["Concept", "Framework"]', '{"name": "Django", "type": "web", "language": "Python"}'),
    (3002, '["Concept", "Framework"]', '{"name": "React", "type": "frontend", "language": "JavaScript"}'),
    (3003, '["Concept", "Framework"]', '{"name": "TensorFlow", "type": "ml", "language": "Python"}'),
    (3004, '["Concept", "Framework"]', '{"name": "Spring", "type": "web", "language": "Java"}');

-- Companies
INSERT INTO knowledge_graph (node_id, labels, properties) VALUES
    (4001, '["Concept", "Company"]', '{"name": "Google", "founded": 1998, "domain": "technology"}'),
    (4002, '["Concept", "Company"]', '{"name": "Mozilla", "founded": 1998, "domain": "open-source"}'),
    (4003, '["Concept", "Company"]', '{"name": "Facebook", "founded": 2004, "domain": "social-media"}');
```

### Step 3: Create Relationships

```sql
-- Language relationships to domains
INSERT INTO knowledge_graph (edge_id, from_id, to_id, edge_type, weight, properties) VALUES
    -- Python relationships
    (5001, 1001, 2001, 'USED_FOR', 0.9, '{"popularity": "high"}'),
    (5002, 1001, 2002, 'USED_FOR', 1.0, '{"popularity": "very high"}'),
    (5003, 1001, 2004, 'USED_FOR', 0.95, '{"popularity": "very high"}'),
    
    -- JavaScript relationships
    (5004, 1002, 2001, 'USED_FOR', 1.0, '{"popularity": "very high"}'),
    
    -- Java relationships
    (5005, 1003, 2001, 'USED_FOR', 0.8, '{"popularity": "high"}'),
    (5006, 1003, 2003, 'USED_FOR', 0.7, '{"popularity": "medium"}'),
    
    -- Rust relationships
    (5007, 1004, 2003, 'USED_FOR', 0.9, '{"popularity": "growing"}');

-- Framework relationships
INSERT INTO knowledge_graph (edge_id, from_id, to_id, edge_type, weight, properties) VALUES
    (5101, 3001, 1001, 'IMPLEMENTED_IN', 1.0, '{}'),
    (5102, 3002, 1002, 'IMPLEMENTED_IN', 1.0, '{}'),
    (5103, 3003, 1001, 'IMPLEMENTED_IN', 1.0, '{}'),
    (5104, 3004, 1003, 'IMPLEMENTED_IN', 1.0, '{}');

-- Company relationships
INSERT INTO knowledge_graph (edge_id, from_id, to_id, edge_type, weight, properties) VALUES
    (5201, 4001, 3003, 'CREATED', 1.0, '{"year": 2015}'),
    (5202, 4002, 1004, 'SPONSORS', 0.8, '{"since": 2015}'),
    (5203, 4003, 3002, 'CREATED', 1.0, '{"year": 2013}');

-- Language evolution relationships
INSERT INTO knowledge_graph (edge_id, from_id, to_id, edge_type, weight, properties) VALUES
    (5301, 1003, 1001, 'INFLUENCED', 0.6, '{"aspects": "OOP concepts"}'),
    (5302, 1001, 1004, 'INFLUENCED', 0.5, '{"aspects": "syntax, memory safety focus"}');
```

### Step 4: Query the Knowledge Graph

#### Find Technologies for a Domain
```cypher
-- Using Cypher to find all technologies for Web Development
SELECT cypher_query('knowledge_graph', '
    MATCH (lang:ProgrammingLanguage)-[r:USED_FOR]->(domain:Domain {name: "Web Development"})
    RETURN lang.name as language, 
           lang.paradigm as paradigm,
           r.popularity as popularity
    ORDER BY r.weight DESC
');
```

#### Technology Stack Recommendations
```sql
-- Find complete technology stacks
WITH web_languages AS (
    SELECT DISTINCT l.node_id, json_extract(l.properties, '$.name') as language
    FROM knowledge_graph e
    JOIN knowledge_graph l ON l.node_id = e.from_id
    JOIN knowledge_graph d ON d.node_id = e.to_id
    WHERE e.edge_type = 'USED_FOR'
      AND json_extract(d.properties, '$.name') = 'Web Development'
)
SELECT 
    wl.language,
    json_extract(f.properties, '$.name') as framework,
    json_extract(f.properties, '$.type') as framework_type
FROM web_languages wl
LEFT JOIN knowledge_graph fe ON fe.to_id = wl.node_id AND fe.edge_type = 'IMPLEMENTED_IN'
LEFT JOIN knowledge_graph f ON f.node_id = fe.from_id
WHERE f.node_id IS NOT NULL
ORDER BY wl.language, framework_type;
```

#### Knowledge Path Finding
```sql
-- Find learning path from Java to Machine Learning
SELECT 
    path,
    (
        SELECT json_group_array(
            json_extract(n.properties, '$.name')
        )
        FROM json_each(path) p
        JOIN knowledge_graph n ON n.node_id = p.value
    ) as concept_path
FROM graph_path('knowledge_graph', 1003, 2004)
LIMIT 5;
```

## Tutorial 3: Route Planning System

### Objective
Create a transportation network for route planning and optimization.

### Step 1: Create Transportation Network

```sql
CREATE VIRTUAL TABLE transport_network USING graph();

-- Add cities (nodes)
INSERT INTO transport_network (node_id, labels, properties) VALUES
    (1, '["City", "Airport"]', '{"name": "New York", "code": "NYC", "lat": 40.7128, "lon": -74.0060, "population": 8336817}'),
    (2, '["City", "Airport"]', '{"name": "Los Angeles", "code": "LAX", "lat": 34.0522, "lon": -118.2437, "population": 3979576}'),
    (3, '["City", "Airport"]', '{"name": "Chicago", "code": "ORD", "lat": 41.8781, "lon": -87.6298, "population": 2693976}'),
    (4, '["City", "Airport"]', '{"name": "Houston", "code": "IAH", "lat": 29.7604, "lon": -95.3698, "population": 2320268}'),
    (5, '["City", "Airport"]', '{"name": "Phoenix", "code": "PHX", "lat": 33.4484, "lon": -112.0740, "population": 1680992}'),
    (6, '["City", "Airport"]', '{"name": "Philadelphia", "code": "PHL", "lat": 39.9526, "lon": -75.1652, "population": 1584064}'),
    (7, '["City", "Airport"]', '{"name": "San Antonio", "code": "SAT", "lat": 29.4241, "lon": -98.4936, "population": 1547253}'),
    (8, '["City", "Airport"]', '{"name": "San Diego", "code": "SAN", "lat": 32.7157, "lon": -117.1611, "population": 1423851}'),
    (9, '["City", "Airport"]', '{"name": "Dallas", "code": "DFW", "lat": 32.7767, "lon": -96.7970, "population": 1343573}'),
    (10, '["City", "Airport"]', '{"name": "San Jose", "code": "SJC", "lat": 37.3382, "lon": -121.8863, "population": 1021795}');
```

### Step 2: Add Routes (Edges)

```sql
-- Add flight routes with distance as weight (in miles)
INSERT INTO transport_network (edge_id, from_id, to_id, edge_type, weight, properties) VALUES
    -- From New York
    (101, 1, 2, 'FLIGHT', 2451, '{"duration": 5.5, "daily_flights": 45, "airline": "Multiple"}'),
    (102, 1, 3, 'FLIGHT', 740, '{"duration": 2.5, "daily_flights": 65, "airline": "Multiple"}'),
    (103, 1, 6, 'FLIGHT', 95, '{"duration": 1.5, "daily_flights": 80, "airline": "Multiple"}'),
    
    -- From Los Angeles  
    (104, 2, 1, 'FLIGHT', 2451, '{"duration": 5.0, "daily_flights": 45, "airline": "Multiple"}'),
    (105, 2, 3, 'FLIGHT', 1744, '{"duration": 4.0, "daily_flights": 35, "airline": "Multiple"}'),
    (106, 2, 5, 'FLIGHT', 369, '{"duration": 1.5, "daily_flights": 40, "airline": "Multiple"}'),
    (107, 2, 8, 'FLIGHT', 120, '{"duration": 1.0, "daily_flights": 55, "airline": "Multiple"}'),
    (108, 2, 10, 'FLIGHT', 337, '{"duration": 1.25, "daily_flights": 50, "airline": "Multiple"}'),
    
    -- From Chicago
    (109, 3, 1, 'FLIGHT', 740, '{"duration": 2.0, "daily_flights": 65, "airline": "Multiple"}'),
    (110, 3, 2, 'FLIGHT', 1744, '{"duration": 4.5, "daily_flights": 35, "airline": "Multiple"}'),
    (111, 3, 4, 'FLIGHT', 941, '{"duration": 2.75, "daily_flights": 25, "airline": "Multiple"}'),
    (112, 3, 9, 'FLIGHT', 802, '{"duration": 2.5, "daily_flights": 30, "airline": "Multiple"}'),
    
    -- Additional routes
    (113, 4, 3, 'FLIGHT', 941, '{"duration": 2.75, "daily_flights": 25, "airline": "Multiple"}'),
    (114, 4, 7, 'FLIGHT', 192, '{"duration": 1.0, "daily_flights": 20, "airline": "Multiple"}'),
    (115, 4, 9, 'FLIGHT', 224, '{"duration": 1.0, "daily_flights": 35, "airline": "Multiple"}'),
    (116, 5, 2, 'FLIGHT', 369, '{"duration": 1.5, "daily_flights": 40, "airline": "Multiple"}'),
    (117, 5, 8, 'FLIGHT', 298, '{"duration": 1.25, "daily_flights": 15, "airline": "Multiple"}'),
    (118, 6, 1, 'FLIGHT', 95, '{"duration": 1.0, "daily_flights": 80, "airline": "Multiple"}'),
    (119, 7, 4, 'FLIGHT', 192, '{"duration": 1.0, "daily_flights": 20, "airline": "Multiple"}'),
    (120, 8, 2, 'FLIGHT', 120, '{"duration": 1.0, "daily_flights": 55, "airline": "Multiple"}'),
    (121, 8, 5, 'FLIGHT', 298, '{"duration": 1.25, "daily_flights": 15, "airline": "Multiple"}'),
    (122, 9, 3, 'FLIGHT', 802, '{"duration": 2.5, "daily_flights": 30, "airline": "Multiple"}'),
    (123, 9, 4, 'FLIGHT', 224, '{"duration": 1.0, "daily_flights": 35, "airline": "Multiple"}'),
    (124, 10, 2, 'FLIGHT', 337, '{"duration": 1.25, "daily_flights": 50, "airline": "Multiple"}');
```

### Step 3: Route Planning Queries

#### Find Shortest Route
```sql
-- Find shortest route from New York to San Diego
SELECT 
    d.distance as total_miles,
    d.path,
    (
        SELECT json_group_array(
            json_extract(n.properties, '$.name') || ' (' || 
            json_extract(n.properties, '$.code') || ')'
        )
        FROM json_each(d.path) p
        JOIN transport_network n ON n.node_id = p.value
    ) as route,
    ROUND(d.distance * 0.20, 2) as estimated_cost_usd
FROM graph_dijkstra('transport_network', 1, 8) d
WHERE d.node_id = 8;
```

#### Find All Routes with Constraints
```sql
-- Find routes with maximum 2 stops
WITH RECURSIVE routes AS (
    -- Start from New York
    SELECT 
        1 as current_city,
        8 as destination,
        json_array(1) as path,
        0 as total_distance,
        0 as stops
    
    UNION ALL
    
    -- Explore routes
    SELECT 
        e.to_id as current_city,
        r.destination,
        json_insert(r.path, '$[#]', e.to_id) as path,
        r.total_distance + e.weight as total_distance,
        r.stops + 1 as stops
    FROM routes r
    JOIN transport_network e ON e.from_id = r.current_city
    WHERE e.edge_type = 'FLIGHT'
      AND r.stops < 2
      AND NOT EXISTS (
          SELECT 1 FROM json_each(r.path) p 
          WHERE p.value = e.to_id
      )
)
SELECT 
    path,
    total_distance,
    stops,
    (
        SELECT json_group_array(json_extract(n.properties, '$.name'))
        FROM json_each(path) p
        JOIN transport_network n ON n.node_id = p.value
    ) as route_names
FROM routes
WHERE current_city = destination
ORDER BY total_distance
LIMIT 5;
```

#### Hub Analysis
```sql
-- Find major hubs by connectivity
WITH hub_stats AS (
    SELECT 
        node_id,
        (
            SELECT COUNT(*) 
            FROM transport_network e1 
            WHERE e1.from_id = n.node_id AND e1.edge_type = 'FLIGHT'
        ) as outgoing_flights,
        (
            SELECT COUNT(*) 
            FROM transport_network e2 
            WHERE e2.to_id = n.node_id AND e2.edge_type = 'FLIGHT'
        ) as incoming_flights
    FROM transport_network n
    WHERE n.edge_id IS NULL
)
SELECT 
    n.node_id,
    json_extract(n.properties, '$.name') as city,
    json_extract(n.properties, '$.code') as airport_code,
    hs.outgoing_flights,
    hs.incoming_flights,
    hs.outgoing_flights + hs.incoming_flights as total_connections
FROM hub_stats hs
JOIN transport_network n ON n.node_id = hs.node_id
ORDER BY total_connections DESC;
```

## Tutorial 4: Recommendation Engine

### Objective
Build a movie recommendation system using collaborative filtering.

### Step 1: Create Recommendation Graph

```sql
CREATE VIRTUAL TABLE movie_recommendations USING graph();

-- Add users
INSERT INTO movie_recommendations (node_id, labels, properties) VALUES
    (1, '["User"]', '{"name": "John", "age": 25, "preferences": ["action", "sci-fi"]}'),
    (2, '["User"]', '{"name": "Sarah", "age": 30, "preferences": ["drama", "romance"]}'),
    (3, '["User"]', '{"name": "Mike", "age": 28, "preferences": ["action", "comedy"]}'),
    (4, '["User"]', '{"name": "Emma", "age": 35, "preferences": ["drama", "thriller"]}'),
    (5, '["User"]', '{"name": "David", "age": 22, "preferences": ["sci-fi", "fantasy"]}');

-- Add movies
INSERT INTO movie_recommendations (node_id, labels, properties) VALUES
    (101, '["Movie"]', '{"title": "The Matrix", "year": 1999, "genres": ["sci-fi", "action"], "rating": 8.7}'),
    (102, '["Movie"]', '{"title": "Inception", "year": 2010, "genres": ["sci-fi", "thriller"], "rating": 8.8}'),
    (103, '["Movie"]', '{"title": "The Godfather", "year": 1972, "genres": ["drama", "crime"], "rating": 9.2}'),
    (104, '["Movie"]', '{"title": "Titanic", "year": 1997, "genres": ["drama", "romance"], "rating": 7.9}'),
    (105, '["Movie"]', '{"title": "The Dark Knight", "year": 2008, "genres": ["action", "thriller"], "rating": 9.0}'),
    (106, '["Movie"]', '{"title": "Forrest Gump", "year": 1994, "genres": ["drama", "romance"], "rating": 8.8}'),
    (107, '["Movie"]', '{"title": "Star Wars", "year": 1977, "genres": ["sci-fi", "adventure"], "rating": 8.6}'),
    (108, '["Movie"]', '{"title": "The Avengers", "year": 2012, "genres": ["action", "adventure"], "rating": 8.0}');

-- Add ratings (edges)
INSERT INTO movie_recommendations (edge_id, from_id, to_id, edge_type, weight, properties) VALUES
    -- John's ratings
    (201, 1, 101, 'RATED', 5.0, '{"date": "2023-01-15"}'),
    (202, 1, 105, 'RATED', 5.0, '{"date": "2023-02-20"}'),
    (203, 1, 107, 'RATED', 4.5, '{"date": "2023-03-10"}'),
    (204, 1, 108, 'RATED', 4.0, '{"date": "2023-04-05"}'),
    
    -- Sarah's ratings
    (205, 2, 103, 'RATED', 5.0, '{"date": "2023-01-20"}'),
    (206, 2, 104, 'RATED', 4.5, '{"date": "2023-02-15"}'),
    (207, 2, 106, 'RATED', 5.0, '{"date": "2023-03-25"}'),
    
    -- Mike's ratings
    (208, 3, 101, 'RATED', 4.5, '{"date": "2023-01-10"}'),
    (209, 3, 105, 'RATED', 5.0, '{"date": "2023-02-28"}'),
    (210, 3, 108, 'RATED', 4.5, '{"date": "2023-04-15"}'),
    
    -- Emma's ratings
    (211, 4, 102, 'RATED', 5.0, '{"date": "2023-01-25"}'),
    (212, 4, 103, 'RATED', 4.5, '{"date": "2023-02-10"}'),
    (213, 4, 105, 'RATED', 4.0, '{"date": "2023-03-30"}'),
    
    -- David's ratings
    (214, 5, 101, 'RATED', 5.0, '{"date": "2023-01-05"}'),
    (215, 5, 102, 'RATED', 5.0, '{"date": "2023-02-25"}'),
    (216, 5, 107, 'RATED', 5.0, '{"date": "2023-03-15"}');
```

### Step 2: Generate Recommendations

#### Collaborative Filtering
```sql
-- Find similar users based on common movie ratings
WITH user_similarity AS (
    SELECT 
        u1.from_id as user1,
        u2.from_id as user2,
        COUNT(*) as common_movies,
        AVG(ABS(u1.weight - u2.weight)) as rating_difference
    FROM movie_recommendations u1
    JOIN movie_recommendations u2 ON u1.to_id = u2.to_id
    WHERE u1.edge_type = 'RATED' 
      AND u2.edge_type = 'RATED'
      AND u1.from_id < u2.from_id
    GROUP BY u1.from_id, u2.from_id
)
SELECT 
    n1.node_id as user1_id,
    json_extract(n1.properties, '$.name') as user1_name,
    n2.node_id as user2_id,
    json_extract(n2.properties, '$.name') as user2_name,
    us.common_movies,
    ROUND(us.rating_difference, 2) as avg_rating_diff,
    ROUND(us.common_movies * (1.0 / (1.0 + us.rating_difference)), 2) as similarity_score
FROM user_similarity us
JOIN movie_recommendations n1 ON n1.node_id = us.user1
JOIN movie_recommendations n2 ON n2.node_id = us.user2
WHERE n1.edge_id IS NULL AND n2.edge_id IS NULL
ORDER BY similarity_score DESC;
```

#### Get Personalized Recommendations
```sql
-- Get movie recommendations for John (user_id = 1)
WITH john_movies AS (
    SELECT to_id as movie_id
    FROM movie_recommendations
    WHERE from_id = 1 AND edge_type = 'RATED'
),
similar_users AS (
    -- Find users who liked the same movies as John
    SELECT DISTINCT
        r2.from_id as similar_user,
        COUNT(*) as common_likes
    FROM movie_recommendations r1
    JOIN movie_recommendations r2 ON r1.to_id = r2.to_id
    WHERE r1.from_id = 1 
      AND r2.from_id != 1
      AND r1.edge_type = 'RATED'
      AND r2.edge_type = 'RATED'
      AND r1.weight >= 4.0
      AND r2.weight >= 4.0
    GROUP BY r2.from_id
)
SELECT DISTINCT
    m.node_id,
    json_extract(m.properties, '$.title') as movie_title,
    json_extract(m.properties, '$.year') as year,
    json_extract(m.properties, '$.genres') as genres,
    json_extract(m.properties, '$.rating') as imdb_rating,
    AVG(r.weight) as avg_user_rating,
    COUNT(r.edge_id) as rating_count
FROM similar_users su
JOIN movie_recommendations r ON r.from_id = su.similar_user
JOIN movie_recommendations m ON m.node_id = r.to_id
WHERE r.edge_type = 'RATED'
  AND r.weight >= 4.0
  AND m.node_id NOT IN (SELECT movie_id FROM john_movies)
  AND m.edge_id IS NULL
GROUP BY m.node_id
ORDER BY avg_user_rating DESC, rating_count DESC;
```

## Tutorial 5: Fraud Detection

### Objective
Create a financial transaction network to detect suspicious patterns.

### Step 1: Create Transaction Network

```sql
CREATE VIRTUAL TABLE transaction_network USING graph();

-- Add accounts
INSERT INTO transaction_network (node_id, labels, properties) VALUES
    -- Regular accounts
    (1, '["Account", "Personal"]', '{"owner": "Alice Smith", "created": "2020-01-15", "status": "active", "risk": "low"}'),
    (2, '["Account", "Personal"]', '{"owner": "Bob Johnson", "created": "2019-06-20", "status": "active", "risk": "low"}'),
    (3, '["Account", "Business"]', '{"owner": "Tech Corp", "created": "2018-03-10", "status": "active", "risk": "low"}'),
    (4, '["Account", "Personal"]', '{"owner": "Charlie Brown", "created": "2021-11-05", "status": "active", "risk": "medium"}'),
    
    -- Suspicious accounts
    (5, '["Account", "Personal"]', '{"owner": "David Wilson", "created": "2023-01-01", "status": "active", "risk": "high"}'),
    (6, '["Account", "Shell"]', '{"owner": "XYZ Holdings", "created": "2023-01-05", "status": "active", "risk": "high"}'),
    (7, '["Account", "Shell"]', '{"owner": "ABC Trading", "created": "2023-01-10", "status": "active", "risk": "high"}'),
    
    -- Merchant accounts
    (8, '["Account", "Merchant"]', '{"owner": "Online Store", "created": "2017-05-20", "status": "active", "risk": "low"}'),
    (9, '["Account", "Merchant"]', '{"owner": "Coffee Shop", "created": "2016-08-15", "status": "active", "risk": "low"}');

-- Add transactions
INSERT INTO transaction_network (edge_id, from_id, to_id, edge_type, weight, properties) VALUES
    -- Normal transactions
    (101, 1, 8, 'TRANSFER', 150.00, '{"date": "2023-06-01", "time": "10:30:00", "method": "online", "status": "completed"}'),
    (102, 1, 9, 'TRANSFER', 5.50, '{"date": "2023-06-01", "time": "08:15:00", "method": "card", "status": "completed"}'),
    (103, 2, 3, 'TRANSFER', 2500.00, '{"date": "2023-06-02", "time": "14:20:00", "method": "wire", "status": "completed"}'),
    (104, 3, 2, 'TRANSFER', 5000.00, '{"date": "2023-06-03", "time": "09:45:00", "method": "wire", "status": "completed"}'),
    
    -- Suspicious pattern: rapid transfers through shell companies
    (105, 4, 5, 'TRANSFER', 9500.00, '{"date": "2023-06-04", "time": "23:45:00", "method": "online", "status": "completed"}'),
    (106, 5, 6, 'TRANSFER', 9400.00, '{"date": "2023-06-05", "time": "00:15:00", "method": "online", "status": "completed"}'),
    (107, 6, 7, 'TRANSFER', 9300.00, '{"date": "2023-06-05", "time": "00:45:00", "method": "online", "status": "completed"}'),
    (108, 7, 5, 'TRANSFER', 9200.00, '{"date": "2023-06-05", "time": "01:15:00", "method": "online", "status": "completed"}'),
    
    -- More suspicious transactions
    (109, 5, 4, 'TRANSFER', 4500.00, '{"date": "2023-06-05", "time": "02:00:00", "method": "online", "status": "completed"}'),
    (110, 6, 4, 'TRANSFER', 4500.00, '{"date": "2023-06-05", "time": "02:30:00", "method": "online", "status": "completed"}');
```

### Step 2: Detect Suspicious Patterns

#### Find Circular Transactions
```sql
-- Detect money cycling patterns
WITH RECURSIVE transaction_paths AS (
    SELECT 
        from_id as start_account,
        to_id as current_account,
        json_array(from_id, to_id) as path,
        weight as total_amount,
        1 as hops,
        edge_id as first_transaction
    FROM transaction_network
    WHERE edge_type = 'TRANSFER'
    
    UNION ALL
    
    SELECT 
        tp.start_account,
        t.to_id as current_account,
        json_insert(tp.path, '$[#]', t.to_id) as path,
        tp.total_amount + t.weight as total_amount,
        tp.hops + 1 as hops,
        tp.first_transaction
    FROM transaction_paths tp
    JOIN transaction_network t ON t.from_id = tp.current_account
    WHERE t.edge_type = 'TRANSFER'
      AND tp.hops < 5
      AND NOT EXISTS (
          SELECT 1 FROM json_each(tp.path) p 
          WHERE p.value = t.to_id AND p.value != tp.start_account
      )
)
SELECT 
    path,
    total_amount,
    hops,
    (
        SELECT json_group_array(json_extract(n.properties, '$.owner'))
        FROM json_each(path) p
        JOIN transaction_network n ON n.node_id = p.value
    ) as account_owners
FROM transaction_paths
WHERE current_account = start_account
  AND hops > 2
ORDER BY total_amount DESC;
```

#### Identify High-Risk Accounts
```sql
-- Find accounts involved in rapid transactions
WITH transaction_velocity AS (
    SELECT 
        from_id as account_id,
        COUNT(*) as outgoing_count,
        SUM(weight) as outgoing_total,
        AVG(weight) as avg_transaction,
        MIN(json_extract(properties, '$.date') || ' ' || json_extract(properties, '$.time')) as first_transaction,
        MAX(json_extract(properties, '$.date') || ' ' || json_extract(properties, '$.time')) as last_transaction
    FROM transaction_network
    WHERE edge_type = 'TRANSFER'
    GROUP BY from_id
)
SELECT 
    n.node_id,
    json_extract(n.properties, '$.owner') as account_owner,
    json_extract(n.properties, '$.risk') as current_risk,
    tv.outgoing_count,
    ROUND(tv.outgoing_total, 2) as total_outgoing,
    ROUND(tv.avg_transaction, 2) as avg_transaction_size,
    tv.first_transaction,
    tv.last_transaction,
    ROUND(
        julianday(tv.last_transaction) - julianday(tv.first_transaction), 
        2
    ) as days_active,
    CASE 
        WHEN tv.outgoing_count > 5 
         AND julianday(tv.last_transaction) - julianday(tv.first_transaction) < 1
        THEN 'HIGH - Rapid transactions'
        WHEN tv.avg_transaction > 5000
        THEN 'MEDIUM - Large transactions'
        ELSE 'LOW'
    END as risk_assessment
FROM transaction_velocity tv
JOIN transaction_network n ON n.node_id = tv.account_id
WHERE n.edge_id IS NULL
ORDER BY risk_assessment DESC, total_outgoing DESC;
```

## Tutorial 6: Organization Hierarchy

### Objective
Model and query an organizational structure with reporting relationships.

### Step 1: Create Organization Graph

```sql
CREATE VIRTUAL TABLE org_structure USING graph();

-- Add employees
INSERT INTO org_structure (node_id, labels, properties) VALUES
    -- C-Suite
    (1, '["Employee", "Executive"]', '{"name": "John CEO", "title": "Chief Executive Officer", "department": "Executive", "hire_date": "2015-01-01", "salary": 500000}'),
    (2, '["Employee", "Executive"]', '{"name": "Sarah CTO", "title": "Chief Technology Officer", "department": "Technology", "hire_date": "2016-03-15", "salary": 350000}'),
    (3, '["Employee", "Executive"]', '{"name": "Mike CFO", "title": "Chief Financial Officer", "department": "Finance", "hire_date": "2015-06-01", "salary": 350000}'),
    
    -- Directors
    (4, '["Employee", "Director"]', '{"name": "Emma Director", "title": "Engineering Director", "department": "Engineering", "hire_date": "2017-02-01", "salary": 200000}'),
    (5, '["Employee", "Director"]', '{"name": "David Director", "title": "Product Director", "department": "Product", "hire_date": "2018-04-15", "salary": 180000}'),
    (6, '["Employee", "Director"]', '{"name": "Lisa Director", "title": "Finance Director", "department": "Finance", "hire_date": "2016-08-01", "salary": 170000}'),
    
    -- Managers
    (7, '["Employee", "Manager"]', '{"name": "Tom Manager", "title": "Backend Manager", "department": "Engineering", "hire_date": "2019-01-15", "salary": 130000}'),
    (8, '["Employee", "Manager"]', '{"name": "Amy Manager", "title": "Frontend Manager", "department": "Engineering", "hire_date": "2019-03-01", "salary": 130000}'),
    (9, '["Employee", "Manager"]', '{"name": "Bob Manager", "title": "Product Manager", "department": "Product", "hire_date": "2019-06-01", "salary": 120000}'),
    
    -- Senior Engineers
    (10, '["Employee", "Senior"]', '{"name": "Alice Senior", "title": "Senior Backend Engineer", "department": "Engineering", "hire_date": "2020-01-01", "salary": 110000}'),
    (11, '["Employee", "Senior"]', '{"name": "Charlie Senior", "title": "Senior Frontend Engineer", "department": "Engineering", "hire_date": "2020-02-15", "salary": 110000}'),
    
    -- Engineers
    (12, '["Employee", "Engineer"]', '{"name": "Diana Engineer", "title": "Backend Engineer", "department": "Engineering", "hire_date": "2021-03-01", "salary": 85000}'),
    (13, '["Employee", "Engineer"]', '{"name": "Eve Engineer", "title": "Frontend Engineer", "department": "Engineering", "hire_date": "2021-06-15", "salary": 85000}'),
    (14, '["Employee", "Engineer"]', '{"name": "Frank Engineer", "title": "Backend Engineer", "department": "Engineering", "hire_date": "2022-01-01", "salary": 80000}');

-- Add reporting relationships
INSERT INTO org_structure (edge_id, from_id, to_id, edge_type, weight, properties) VALUES
    -- CEO's direct reports
    (101, 2, 1, 'REPORTS_TO', 1.0, '{"since": "2016-03-15"}'),
    (102, 3, 1, 'REPORTS_TO', 1.0, '{"since": "2015-06-01"}'),
    
    -- CTO's reports
    (103, 4, 2, 'REPORTS_TO', 1.0, '{"since": "2017-02-01"}'),
    (104, 5, 2, 'REPORTS_TO', 1.0, '{"since": "2018-04-15"}'),
    
    -- CFO's reports
    (105, 6, 3, 'REPORTS_TO', 1.0, '{"since": "2016-08-01"}'),
    
    -- Engineering Director's reports
    (106, 7, 4, 'REPORTS_TO', 1.0, '{"since": "2019-01-15"}'),
    (107, 8, 4, 'REPORTS_TO', 1.0, '{"since": "2019-03-01"}'),
    
    -- Product Director's reports
    (108, 9, 5, 'REPORTS_TO', 1.0, '{"since": "2019-06-01"}'),
    
    -- Backend Manager's reports
    (109, 10, 7, 'REPORTS_TO', 1.0, '{"since": "2020-01-01"}'),
    (110, 12, 7, 'REPORTS_TO', 1.0, '{"since": "2021-03-01"}'),
    (111, 14, 7, 'REPORTS_TO', 1.0, '{"since": "2022-01-01"}'),
    
    -- Frontend Manager's reports
    (112, 11, 8, 'REPORTS_TO', 1.0, '{"since": "2020-02-15"}'),
    (113, 13, 8, 'REPORTS_TO', 1.0, '{"since": "2021-06-15"}');
```

### Step 2: Query Organization Structure

#### Find Reporting Chain
```sql
-- Find complete reporting chain for an employee
WITH RECURSIVE reporting_chain AS (
    SELECT 
        node_id as employee_id,
        json_extract(properties, '$.name') as employee_name,
        NULL as manager_id,
        NULL as manager_name,
        0 as level
    FROM org_structure
    WHERE node_id = 13  -- Eve Engineer
    
    UNION ALL
    
    SELECT 
        rc.employee_id,
        rc.employee_name,
        m.node_id as manager_id,
        json_extract(m.properties, '$.name') as manager_name,
        rc.level + 1 as level
    FROM reporting_chain rc
    JOIN org_structure r ON r.from_id = 
        CASE WHEN rc.level = 0 THEN rc.employee_id ELSE rc.manager_id END
    JOIN org_structure m ON m.node_id = r.to_id
    WHERE r.edge_type = 'REPORTS_TO'
)
SELECT * FROM reporting_chain
ORDER BY level;
```

#### Find All Reports
```sql
-- Find all direct and indirect reports for a manager
WITH RECURSIVE all_reports AS (
    SELECT 
        4 as manager_id,  -- Emma Director
        node_id as report_id,
        json_extract(properties, '$.name') as report_name,
        json_extract(properties, '$.title') as report_title,
        1 as level
    FROM org_structure e
    JOIN org_structure r ON r.from_id = e.node_id
    WHERE r.to_id = 4 AND r.edge_type = 'REPORTS_TO'
    
    UNION ALL
    
    SELECT 
        ar.manager_id,
        e.node_id as report_id,
        json_extract(e.properties, '$.name') as report_name,
        json_extract(e.properties, '$.title') as report_title,
        ar.level + 1 as level
    FROM all_reports ar
    JOIN org_structure r ON r.to_id = ar.report_id
    JOIN org_structure e ON e.node_id = r.from_id
    WHERE r.edge_type = 'REPORTS_TO'
)
SELECT 
    level,
    report_name,
    report_title
FROM all_reports
ORDER BY level, report_name;
```

#### Department Analysis
```sql
-- Analyze department statistics
SELECT 
    json_extract(properties, '$.department') as department,
    COUNT(*) as employee_count,
    AVG(CAST(json_extract(properties, '$.salary') as INTEGER)) as avg_salary,
    MIN(CAST(json_extract(properties, '$.salary') as INTEGER)) as min_salary,
    MAX(CAST(json_extract(properties, '$.salary') as INTEGER)) as max_salary,
    SUM(CAST(json_extract(properties, '$.salary') as INTEGER)) as total_payroll
FROM org_structure
WHERE edge_id IS NULL
GROUP BY department
ORDER BY employee_count DESC;
```

## Advanced Topics

### Performance Optimization

```sql
-- Create indexes for better performance
CREATE INDEX idx_edge_type ON social_network(edge_type) WHERE edge_id IS NOT NULL;
CREATE INDEX idx_node_labels ON social_network(labels) WHERE edge_id IS NULL;
CREATE INDEX idx_from_to ON social_network(from_id, to_id) WHERE edge_id IS NOT NULL;

-- Analyze tables
ANALYZE social_network;
ANALYZE knowledge_graph;
ANALYZE transport_network;
```

### Batch Operations

```sql
-- Batch insert with transaction
BEGIN TRANSACTION;

-- Insert 1000 nodes
WITH RECURSIVE generate_nodes(n) AS (
    SELECT 1
    UNION ALL
    SELECT n + 1 FROM generate_nodes WHERE n < 1000
)
INSERT INTO social_network (node_id, labels, properties)
SELECT 
    1000 + n,
    '["Person", "User"]',
    json_object(
        'name', 'User' || n,
        'age', 20 + (n % 40),
        'city', CASE (n % 4) 
            WHEN 0 THEN 'New York'
            WHEN 1 THEN 'Los Angeles'
            WHEN 2 THEN 'Chicago'
            ELSE 'Houston'
        END
    )
FROM generate_nodes;

COMMIT;
```

### Graph Export/Import

```sql
-- Export graph to JSON
.mode json
.output my_graph_export.json
SELECT * FROM social_network;
.output stdout
.mode list

-- Import from CSV
.mode csv
.import nodes.csv temp_nodes
.import edges.csv temp_edges

INSERT INTO social_network 
SELECT * FROM temp_nodes;

INSERT INTO social_network 
SELECT * FROM temp_edges;
```

### Monitoring and Maintenance

```sql
-- Check graph statistics
SELECT 
    'Nodes' as entity_type,
    COUNT(*) as count
FROM social_network 
WHERE edge_id IS NULL
UNION ALL
SELECT 
    'Edges' as entity_type,
    COUNT(*) as count
FROM social_network 
WHERE edge_id IS NOT NULL;

-- Vacuum and optimize
VACUUM social_network;
REINDEX social_network;
```

## Conclusion

These tutorials demonstrate the versatility of the SQLite Graph Extension for various real-world applications. Key takeaways:

1. **Flexibility**: Supports both SQL and Cypher queries
2. **Performance**: Optimized algorithms for graph operations
3. **Integration**: Works seamlessly with existing SQLite features
4. **Scalability**: Handles graphs from small to millions of nodes

For more advanced features and optimizations, refer to the API Reference and Performance Tuning Guide.