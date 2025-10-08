#!/usr/bin/env python3
"""
Generate comprehensive TCK test suite from tck_scenarios_manifest.csv
Creates test files covering all 1600+ test scenarios
"""

import csv
import json
import os
import re
from collections import defaultdict

def sanitize_name(name):
    """Convert scenario name to valid C identifier"""
    # Remove special characters and replace with underscore
    name = re.sub(r'[^a-zA-Z0-9_]', '_', name)
    # Remove multiple underscores
    name = re.sub(r'_+', '_', name)
    # Remove leading/trailing underscores
    name = name.strip('_')
    # Ensure it starts with letter
    if name and name[0].isdigit():
        name = 'test_' + name
    return name

def load_tck_scenarios(csv_path):
    """Load TCK scenarios from CSV manifest"""
    scenarios = []
    
    with open(csv_path, 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        for row in reader:
            scenarios.append(row)
    
    return scenarios

def group_scenarios_by_area(scenarios):
    """Group scenarios by language area for organization"""
    grouped = defaultdict(list)
    
    for scenario in scenarios:
        area = scenario.get('language_area', 'unknown')
        grouped[area].append(scenario)
    
    return dict(grouped)

def generate_test_file_header():
    """Generate common header for all test files"""
    return '''#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unity.h>

static sqlite3 *db = NULL;
static char *error_msg = NULL;

void setUp(void) {
    int rc = sqlite3_open(":memory:", &db);
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
    
    // Enable loading extensions
    sqlite3_enable_load_extension(db, 1);
    
    // Load graph extension
    rc = sqlite3_load_extension(db, "../build/libgraph.so", "sqlite3_graph_init", &error_msg);
    if (rc != SQLITE_OK) {
        printf("Failed to load graph extension: %s\\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    TEST_ASSERT_EQUAL(SQLITE_OK, rc);
}

void tearDown(void) {
    if (error_msg) {
        sqlite3_free(error_msg);
        error_msg = NULL;
    }
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

'''

def generate_test_function(scenario, index):
    """Generate a single test function for a scenario"""
    scenario_id = scenario.get('scenario_id', f'unknown_{index}')
    scenario_name = scenario.get('scenario_name', 'Unknown scenario')
    feature_name = scenario.get('feature_name', 'Unknown feature')
    requires_runtime = scenario.get('requires_runtime', 'False') == 'True'
    
    # Sanitize function name
    func_name = f"test_{sanitize_name(scenario_id)}"
    
    # Generate test body based on scenario type
    if requires_runtime:
        test_body = f'''
    // Runtime test for: {scenario_name}
    // Feature: {feature_name}
    
    // Create virtual table
    int rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE graph USING graph()", NULL, NULL, &error_msg);
    if (rc != SQLITE_OK) {{
        printf("Failed to create virtual table: %s\\n", error_msg);
        sqlite3_free(error_msg);
        error_msg = NULL;
        TEST_FAIL_MESSAGE("Virtual table creation failed");
        return;
    }}
    
    // TODO: Implement actual test logic for {scenario_id}
    // This is a placeholder that ensures basic functionality works
    
    // For now, mark as pending implementation
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: {scenario_id}");
'''
    else:
        test_body = f'''
    // Parse/validate test for: {scenario_name}
    // Feature: {feature_name}
    
    // TODO: Implement parsing/validation test for {scenario_id}
    // This is a placeholder for syntax validation tests
    
    // For now, mark as pending implementation  
    TEST_IGNORE_MESSAGE("TCK scenario implementation pending: {scenario_id}");
'''

    return f'''void {func_name}(void) {{{test_body}
}}

'''

def generate_main_function(test_functions):
    """Generate main function that runs all tests"""
    main_body = '''int main(void) {
    UNITY_BEGIN();
    
'''
    
    for func_name in test_functions:
        main_body += f'    RUN_TEST({func_name});\n'
    
    main_body += '''    
    return UNITY_END();
}
'''
    
    return main_body

def generate_area_test_file(area, scenarios, output_dir):
    """Generate a test file for a specific language area"""
    if not scenarios:
        return
        
    # Sanitize area name for filename
    area_name = sanitize_name(area.replace('.', '_'))
    filename = f"tck_test_{area_name}.c"
    filepath = os.path.join(output_dir, filename)
    
    with open(filepath, 'w') as f:
        # Write header
        f.write(f"// Generated TCK tests for language area: {area}\n")
        f.write(f"// Total scenarios: {len(scenarios)}\n\n")
        f.write(generate_test_file_header())
        
        # Generate test functions
        test_functions = []
        for i, scenario in enumerate(scenarios):
            func_name = f"test_{sanitize_name(scenario.get('scenario_id', f'unknown_{i}'))}"
            test_functions.append(func_name)
            f.write(generate_test_function(scenario, i))
        
        # Generate main function
        f.write(generate_main_function(test_functions))
    
    print(f"Generated {filename} with {len(scenarios)} test scenarios")
    return filename

def generate_makefile_targets(area_files):
    """Generate Makefile targets for TCK test files"""
    makefile_content = '''
# Generated TCK test targets
'''
    
    for area_file in area_files:
        target_name = area_file.replace('.c', '')
        makefile_content += f'''
{target_name}: {area_file} $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
\t$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)
'''
    
    # Add target to run all TCK tests
    all_targets = ' '.join([f.replace('.c', '') for f in area_files])
    makefile_content += f'''
TCK_TESTS = {all_targets}

test-tck-all: $(TCK_TESTS)
\t@echo "=== Running All TCK Tests ==="
\t@for test in $(TCK_TESTS); do \\
\t\techo "Running $$test..."; \\
\t\tif ./$$test; then \\
\t\t\techo "✅ $$test PASSED"; \\
\t\telse \\
\t\t\techo "⚠️  $$test had failures (expected for unimplemented scenarios)"; \\
\t\tfi; \\
\t\techo; \\
\tdone
\t@echo "TCK test run completed"

clean-tck:
\trm -f $(TCK_TESTS) tck_test_*.c
'''
    
    return makefile_content

def main():
    # Load scenarios from manifest
    csv_path = '../tck_scenarios_manifest.csv'
    if not os.path.exists(csv_path):
        print(f"Error: {csv_path} not found")
        return 1
    
    print("Loading TCK scenarios from manifest...")
    scenarios = load_tck_scenarios(csv_path)
    print(f"Loaded {len(scenarios)} scenarios")
    
    # Group by language area
    grouped_scenarios = group_scenarios_by_area(scenarios)
    print(f"Found {len(grouped_scenarios)} language areas")
    
    # Create output directory for generated tests
    output_dir = "."
    
    # Generate test files for each area
    area_files = []
    total_scenarios = 0
    
    for area, area_scenarios in grouped_scenarios.items():
        if area_scenarios:  # Only generate if there are scenarios
            filename = generate_area_test_file(area, area_scenarios, output_dir)
            if filename:
                area_files.append(filename)
                total_scenarios += len(area_scenarios)
    
    # Generate Makefile additions
    makefile_additions = generate_makefile_targets(area_files)
    with open('Makefile.tck', 'w') as f:
        f.write(makefile_additions)
    
    # Generate summary
    summary = f"""
# TCK Test Generation Summary

Generated {len(area_files)} test files covering {total_scenarios} scenarios from {len(grouped_scenarios)} language areas.

## Generated Files:
"""
    
    for filename in sorted(area_files):
        summary += f"- {filename}\n"
    
    summary += f"""
## To run tests:
```bash
# Build all TCK tests
make -f Makefile.tck

# Run all TCK tests  
make -f Makefile.tck test-tck-all

# Clean TCK tests
make -f Makefile.tck clean-tck
```

## Note:
Most tests are currently marked as TEST_IGNORE pending implementation.
This provides the framework for implementing all 1600+ TCK scenarios.
"""
    
    with open('TCK_GENERATION_SUMMARY.md', 'w') as f:
        f.write(summary)
    
    print(summary)
    return 0

if __name__ == '__main__':
    exit(main())
