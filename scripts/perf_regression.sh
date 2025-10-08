#!/bin/bash
#
# perf_regression.sh - Performance regression testing framework
#
# This script runs automated performance tests to detect regressions
# in the SQLite Graph Extension.

set -e

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"
RESULTS_DIR="$PROJECT_DIR/perf_results"
BASELINE_FILE="$RESULTS_DIR/baseline.json"
CURRENT_FILE="$RESULTS_DIR/current.json"

# Test configuration
GRAPH_SIZES=(100 1000 10000)
WARMUP_RUNS=3
MEASURE_RUNS=10
REGRESSION_THRESHOLD=10  # 10% performance regression threshold

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Create results directory
mkdir -p "$RESULTS_DIR"

# Function to print colored output
print_status() {
    local status=$1
    local message=$2
    
    case $status in
        "PASS")
            echo -e "${GREEN}[PASS]${NC} $message"
            ;;
        "FAIL")
            echo -e "${RED}[FAIL]${NC} $message"
            ;;
        "WARN")
            echo -e "${YELLOW}[WARN]${NC} $message"
            ;;
        *)
            echo "[$status] $message"
            ;;
    esac
}

# Build the extension
build_extension() {
    echo "Building SQLite Graph Extension..."
    cd "$BUILD_DIR"
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    cd "$SCRIPT_DIR"
}

# Generate test data
generate_test_data() {
    local size=$1
    local db_file="$RESULTS_DIR/test_graph_${size}.db"
    
    echo "Generating test graph with $size nodes..."
    
    sqlite3 "$db_file" <<EOF
.load $BUILD_DIR/src/graph.so
CREATE VIRTUAL TABLE test_graph USING graph;

-- Generate nodes
WITH RECURSIVE generate_nodes(id) AS (
    SELECT 1
    UNION ALL
    SELECT id + 1 FROM generate_nodes WHERE id < $size
)
INSERT INTO test_graph_nodes 
SELECT graph_node_add(test_graph, id, 'Person', 
    printf('{"name":"Person%d","age":%d}', id, 20 + (id % 50)))
FROM generate_nodes;

-- Generate edges (each node connected to ~5 others)
WITH RECURSIVE generate_edges(from_id, to_id) AS (
    SELECT 1, 2
    UNION ALL
    SELECT 
        CASE WHEN to_id % 5 = 0 THEN from_id + 1 ELSE from_id END,
        CASE WHEN to_id % 5 = 0 THEN from_id + 2 ELSE to_id + 1 END
    FROM generate_edges 
    WHERE from_id < $size AND to_id < $size
)
INSERT INTO test_graph_edges
SELECT graph_edge_add(test_graph, from_id, to_id, 'KNOWS', 1.0, '{}')
FROM generate_edges
WHERE from_id != to_id;
EOF
}

# Run performance test
run_perf_test() {
    local test_name=$1
    local db_file=$2
    local query=$3
    local output_file=$4
    
    echo "Running test: $test_name"
    
    # Create test script
    cat > "$RESULTS_DIR/test_query.sql" <<EOF
.load $BUILD_DIR/src/graph.so
.timer on
.output /dev/null

-- Warmup runs
$(for i in $(seq 1 $WARMUP_RUNS); do echo "$query"; done)

-- Reset timer
.timer off
.timer on
.output stdout

-- Measure runs
.print "BEGIN_TIMINGS"
$(for i in $(seq 1 $MEASURE_RUNS); do echo "$query"; done)
.print "END_TIMINGS"
EOF
    
    # Run test and capture timings
    local timings=$(sqlite3 "$db_file" < "$RESULTS_DIR/test_query.sql" 2>&1 | \
        sed -n '/BEGIN_TIMINGS/,/END_TIMINGS/p' | \
        grep -E "Run Time: real [0-9.]+" | \
        awk '{print $4}')
    
    # Calculate statistics
    local sum=0
    local count=0
    local min=999999
    local max=0
    
    for time in $timings; do
        sum=$(echo "$sum + $time" | bc)
        count=$((count + 1))
        
        if (( $(echo "$time < $min" | bc -l) )); then
            min=$time
        fi
        if (( $(echo "$time > $max" | bc -l) )); then
            max=$time
        fi
    done
    
    local avg=$(echo "scale=3; $sum / $count" | bc)
    
    # Append results
    echo "{\"test\":\"$test_name\",\"avg\":$avg,\"min\":$min,\"max\":$max,\"runs\":$count}" >> "$output_file"
}

# Performance test suite
run_test_suite() {
    local output_file=$1
    
    echo "[" > "$output_file"
    local first=1
    
    for size in "${GRAPH_SIZES[@]}"; do
        local db_file="$RESULTS_DIR/test_graph_${size}.db"
        
        # Generate test data if not exists
        if [ ! -f "$db_file" ]; then
            generate_test_data $size
        fi
        
        # Test 1: Simple pattern match
        if [ $first -eq 0 ]; then echo "," >> "$output_file"; fi
        run_perf_test "pattern_match_${size}" "$db_file" \
            "SELECT COUNT(*) FROM cypher_execute('MATCH (n:Person) WHERE n.age > 30 RETURN n');" \
            "$output_file"
        first=0
        
        # Test 2: Path traversal
        echo "," >> "$output_file"
        run_perf_test "path_traversal_${size}" "$db_file" \
            "SELECT COUNT(*) FROM cypher_execute('MATCH (p1:Person)-[:KNOWS]->(p2:Person) RETURN p1, p2 LIMIT 100');" \
            "$output_file"
        
        # Test 3: Shortest path
        echo "," >> "$output_file"
        run_perf_test "shortest_path_${size}" "$db_file" \
            "SELECT * FROM graph_shortest_path(test_graph, 1, $((size/2)));" \
            "$output_file"
        
        # Test 4: PageRank
        if [ $size -le 1000 ]; then
            echo "," >> "$output_file"
            run_perf_test "pagerank_${size}" "$db_file" \
                "SELECT * FROM graph_pagerank(test_graph, 0.85, 0.0001);" \
                "$output_file"
        fi
        
        # Test 5: Aggregation
        echo "," >> "$output_file"
        run_perf_test "aggregation_${size}" "$db_file" \
            "SELECT COUNT(*), AVG(CAST(json_extract(properties, '$.age') AS REAL)) FROM test_graph_nodes;" \
            "$output_file"
    done
    
    echo "]" >> "$output_file"
}

# Compare results
compare_results() {
    local baseline=$1
    local current=$2
    
    echo ""
    echo "Performance Comparison Report"
    echo "============================="
    
    # Parse JSON and compare
    python3 - <<EOF
import json
import sys

with open('$baseline', 'r') as f:
    baseline_data = json.load(f)
    
with open('$current', 'r') as f:
    current_data = json.load(f)

# Create lookup dictionary
baseline_dict = {test['test']: test for test in baseline_data}
current_dict = {test['test']: test for test in current_data}

# Compare results
regressions = []
improvements = []

for test_name in current_dict:
    if test_name in baseline_dict:
        baseline_avg = baseline_dict[test_name]['avg']
        current_avg = current_dict[test_name]['avg']
        
        change_percent = ((current_avg - baseline_avg) / baseline_avg) * 100
        
        status = "SAME"
        if change_percent > $REGRESSION_THRESHOLD:
            status = "REGRESSION"
            regressions.append((test_name, change_percent))
        elif change_percent < -$REGRESSION_THRESHOLD:
            status = "IMPROVEMENT"
            improvements.append((test_name, change_percent))
            
        print(f"{test_name:30} {baseline_avg:8.3f}ms -> {current_avg:8.3f}ms ({change_percent:+6.1f}%) [{status}]")

# Summary
print("\nSummary:")
print(f"  Regressions: {len(regressions)}")
print(f"  Improvements: {len(improvements)}")
print(f"  No change: {len(current_dict) - len(regressions) - len(improvements)}")

# Exit with error if regressions found
if regressions:
    print("\nPerformance regressions detected!")
    for test, change in regressions:
        print(f"  - {test}: {change:+.1f}%")
    sys.exit(1)
EOF
}

# Main execution
main() {
    echo "SQLite Graph Extension Performance Regression Test"
    echo "================================================"
    
    # Parse arguments
    local command=${1:-"test"}
    
    case $command in
        "baseline")
            echo "Creating performance baseline..."
            build_extension
            run_test_suite "$BASELINE_FILE"
            print_status "PASS" "Baseline created successfully"
            ;;
            
        "test")
            if [ ! -f "$BASELINE_FILE" ]; then
                print_status "WARN" "No baseline found, creating one..."
                build_extension
                run_test_suite "$BASELINE_FILE"
            fi
            
            echo "Running performance tests..."
            build_extension
            run_test_suite "$CURRENT_FILE"
            
            echo "Comparing with baseline..."
            if compare_results "$BASELINE_FILE" "$CURRENT_FILE"; then
                print_status "PASS" "No performance regressions detected"
            else
                print_status "FAIL" "Performance regressions detected"
                exit 1
            fi
            ;;
            
        "clean")
            echo "Cleaning test data..."
            rm -f "$RESULTS_DIR"/*.db
            rm -f "$RESULTS_DIR"/*.json
            rm -f "$RESULTS_DIR"/*.sql
            print_status "PASS" "Cleaned successfully"
            ;;
            
        *)
            echo "Usage: $0 [baseline|test|clean]"
            echo "  baseline - Create performance baseline"
            echo "  test     - Run tests and compare with baseline"
            echo "  clean    - Clean test data"
            exit 1
            ;;
    esac
}

# Run main function
main "$@"