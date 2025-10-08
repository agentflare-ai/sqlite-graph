#!/usr/bin/env python3
"""
Update TCK compliance percentage in coverage.json
This script calculates the running compliance percentage based on test results.
"""

import json
import sys
from typing import Dict, Any

def calculate_compliance(coverage_data: Dict[str, Any]) -> float:
    """Calculate compliance percentage from scenario results."""
    stats = coverage_data.get('statistics', {})
    total = stats.get('total_scenarios', 0)
    passed = stats.get('passed', 0)
    
    if total == 0:
        return 0.0
    
    return (passed / total) * 100.0

def update_statistics(coverage_data: Dict[str, Any]) -> None:
    """Update statistics based on current scenario statuses."""
    scenarios = coverage_data.get('scenarios', [])
    
    stats = {
        'total_scenarios': len(scenarios),
        'passed': 0,
        'failed': 0,
        'skipped': 0,
        'pending': 0
    }
    
    for scenario in scenarios:
        status = scenario.get('status', 'pending')
        if status == 'pass':
            stats['passed'] += 1
        elif status == 'fail':
            stats['failed'] += 1
        elif status == 'skip':
            stats['skipped'] += 1
        else:
            stats['pending'] += 1
    
    stats['compliance_percentage'] = calculate_compliance({'statistics': stats})
    coverage_data['statistics'] = stats

def main():
    try:
        with open('coverage.json', 'r') as f:
            coverage_data = json.load(f)
        
        update_statistics(coverage_data)
        
        with open('coverage.json', 'w') as f:
            json.dump(coverage_data, f, indent=2)
        
        stats = coverage_data['statistics']
        print(f"TCK Compliance Report:")
        print(f"  Total Scenarios: {stats['total_scenarios']}")
        print(f"  Passed: {stats['passed']}")
        print(f"  Failed: {stats['failed']}")
        print(f"  Skipped: {stats['skipped']}")
        print(f"  Pending: {stats['pending']}")
        print(f"  Compliance: {stats['compliance_percentage']:.2f}%")
        
    except Exception as e:
        print(f"Error updating compliance: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()
