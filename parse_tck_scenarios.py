#!/usr/bin/env python3

import os
import re
import json
import csv
from pathlib import Path
from typing import Dict, List, Set, Tuple

def parse_feature_file(file_path: str) -> List[Dict]:
    """Parse a single feature file and extract scenario information."""
    scenarios = []
    
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Extract feature name
    feature_match = re.search(r'^Feature:\s*(.+)$', content, re.MULTILINE)
    feature_name = feature_match.group(1).strip() if feature_match else "Unknown"
    
    # Find all scenarios (including scenario outlines)
    scenario_pattern = r'^  (Scenario|Scenario Outline):\s*(.+)$'
    scenarios_found = re.findall(scenario_pattern, content, re.MULTILINE)
    
    for i, (scenario_type, scenario_name) in enumerate(scenarios_found):
        # Generate a scenario ID
        relative_path = os.path.relpath(file_path, 'openCypher-master/tck/features')
        path_parts = relative_path.replace('.feature', '').split('/')
        scenario_id = f"{'-'.join(path_parts)}-{i+1:02d}"
        
        # Determine language area based on directory structure
        language_area = determine_language_area(relative_path)
        
        # Check if scenario requires runtime execution
        requires_runtime = check_requires_runtime(content, scenario_name)
        
        scenario_info = {
            'scenario_id': scenario_id,
            'scenario_name': scenario_name.strip(),
            'scenario_type': scenario_type,
            'feature_name': feature_name,
            'file_path': relative_path,
            'language_area': language_area,
            'requires_runtime': requires_runtime
        }
        
        scenarios.append(scenario_info)
    
    return scenarios

def determine_language_area(file_path: str) -> str:
    """Determine the language area based on file path."""
    parts = file_path.split('/')
    
    if len(parts) >= 2:
        main_area = parts[0]
        if len(parts) >= 3:
            sub_area = parts[1]
            return f"{main_area}.{sub_area}"
        return main_area
    
    return "unknown"

def check_requires_runtime(content: str, scenario_name: str) -> bool:
    """
    Check if a scenario requires runtime execution vs pure parse/validate.
    This is a heuristic based on common patterns.
    """
    runtime_indicators = [
        r'\bAnd the result should be\b',
        r'\bThen the result should be\b',
        r'\bAnd the side effects should be\b',
        r'\bThen the side effects should be\b',
        r'\bWhen executing query\b',
        r'\bAnd executing query\b',
        r'\bThen a \w+Error should be raised\b',
        r'\bAnd a \w+Error should be raised\b',
        r'\bno side effects\b',
        r'\brows in any order\b',
        r'\brows in order\b'
    ]
    
    parse_validate_indicators = [
        r'\bThen a SyntaxError should be raised\b',
        r'\bAnd a SyntaxError should be raised\b',
        r'\bshould fail to parse\b',
        r'\bparse error\b'
    ]
    
    # Check for parse/validate indicators first (they take precedence)
    for indicator in parse_validate_indicators:
        if re.search(indicator, content, re.IGNORECASE):
            return False
    
    # Check for runtime indicators
    for indicator in runtime_indicators:
        if re.search(indicator, content, re.IGNORECASE):
            return True
    
    # Default assumption: if it has database setup or query execution, it needs runtime
    if re.search(r'\bGiven an empty graph\b|\bGiven any graph\b|\bWhen executing query\b', content, re.IGNORECASE):
        return True
    
    return False

def main():
    """Main function to process all feature files."""
    tck_features_dir = 'openCypher-master/tck/features'
    
    if not os.path.exists(tck_features_dir):
        print(f"Error: TCK features directory not found: {tck_features_dir}")
        return
    
    all_scenarios = []
    
    # Walk through all feature files
    for root, dirs, files in os.walk(tck_features_dir):
        for file in files:
            if file.endswith('.feature'):
                file_path = os.path.join(root, file)
                try:
                    scenarios = parse_feature_file(file_path)
                    all_scenarios.extend(scenarios)
                except Exception as e:
                    print(f"Error processing {file_path}: {e}")
    
    # Generate commit info
    commit_info = {
        'repository': 'opencypher/openCypher',
        'commit_hash': '7db2677dd3c6c87cc8d6b35fa7a32e5054ef6ebf',
        'total_scenarios': len(all_scenarios)
    }
    
    # Group scenarios by language area
    scenarios_by_area = {}
    for scenario in all_scenarios:
        area = scenario['language_area']
        if area not in scenarios_by_area:
            scenarios_by_area[area] = []
        scenarios_by_area[area].append(scenario)
    
    # Create summary statistics
    summary = {
        'commit_info': commit_info,
        'language_areas': {},
        'scenarios_by_area': scenarios_by_area
    }
    
    for area, scenarios in scenarios_by_area.items():
        runtime_count = sum(1 for s in scenarios if s['requires_runtime'])
        parse_count = len(scenarios) - runtime_count
        
        summary['language_areas'][area] = {
            'total_scenarios': len(scenarios),
            'runtime_execution': runtime_count,
            'parse_validate_only': parse_count
        }
    
    # Write JSON manifest
    with open('tck_scenarios_manifest.json', 'w') as f:
        json.dump(summary, f, indent=2)
    
    # Write CSV manifest
    with open('tck_scenarios_manifest.csv', 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([
            'scenario_id', 'scenario_name', 'scenario_type', 'feature_name',
            'file_path', 'language_area', 'requires_runtime'
        ])
        
        for scenario in sorted(all_scenarios, key=lambda x: (x['language_area'], x['scenario_id'])):
            writer.writerow([
                scenario['scenario_id'],
                scenario['scenario_name'],
                scenario['scenario_type'],
                scenario['feature_name'],
                scenario['file_path'],
                scenario['language_area'],
                scenario['requires_runtime']
            ])
    
    # Print summary
    print(f"OpenCypher TCK Scenarios Analysis")
    print(f"==================================")
    print(f"Repository: {commit_info['repository']}")
    print(f"Commit: {commit_info['commit_hash']}")
    print(f"Total scenarios: {commit_info['total_scenarios']}")
    print()
    
    print("Language Area Summary:")
    print("----------------------")
    for area, stats in sorted(summary['language_areas'].items()):
        print(f"{area:30} | Total: {stats['total_scenarios']:3d} | Runtime: {stats['runtime_execution']:3d} | Parse: {stats['parse_validate_only']:3d}")
    
    print()
    print("Output files generated:")
    print("- tck_scenarios_manifest.json")
    print("- tck_scenarios_manifest.csv")

if __name__ == "__main__":
    main()
