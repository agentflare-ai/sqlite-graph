#!/usr/bin/env python3
"""
Demo script showing the complete TCK coverage system in action.
"""

import json
import subprocess
import os

def main():
    print("=== TCK Coverage Matrix Demo ===\n")
    
    # Change to tests directory
    os.chdir('tests')
    
    # Show current compliance
    print("1. Current TCK Compliance Status:")
    subprocess.run(['python3', '../scripts/update_compliance.py'])
    print()
    
    # Show sample test function names
    print("2. Sample Unity Test Function Mapping:")
    with open('coverage.json', 'r') as f:
        coverage = json.load(f)
    
    sample_groups = ['expressions.mathematical', 'clauses.match', 'expressions.string']
    for group in sample_groups:
        scenarios = [s for s in coverage['scenarios'] if s['feature_group'] == group][:3]
        print(f"   {group}:")
        for scenario in scenarios:
            print(f"     {scenario['scenario_id']} -> {scenario['unity_test_function']}()")
    print()
    
    # Show compliance by status
    print("3. Scenarios by Status:")
    stats = coverage['statistics']
    print(f"   • Passed:  {stats['passed']} ({stats['passed']/stats['total_scenarios']*100:.1f}%)")
    print(f"   • Failed:  {stats['failed']} ({stats['failed']/stats['total_scenarios']*100:.1f}%)")
    print(f"   • Skipped: {stats['skipped']} ({stats['skipped']/stats['total_scenarios']*100:.1f}%)")
    print(f"   • Pending: {stats['pending']} ({stats['pending']/stats['total_scenarios']*100:.1f}%)")
    print()
    
    # Show high priority pending tests
    print("4. High Priority Tests Needing Implementation:")
    high_priority_pending = [s for s in coverage['scenarios'] 
                           if s.get('status') == 'pending' and s.get('priority') == 'high'][:5]
    for scenario in high_priority_pending:
        print(f"   • {scenario['unity_test_function']} ({scenario['feature_group']})")
    print()
    
    # Show feature group coverage
    print("5. Coverage by Feature Group:")
    groups = {}
    for scenario in coverage['scenarios']:
        group = scenario['feature_group']
        if group not in groups:
            groups[group] = {'total': 0, 'passed': 0}
        groups[group]['total'] += 1
        if scenario.get('status') == 'pass':
            groups[group]['passed'] += 1
    
    for group, data in sorted(groups.items())[:10]:  # Show first 10 groups
        pct = (data['passed'] / data['total']) * 100 if data['total'] > 0 else 0
        print(f"   {group:30} {data['passed']:3}/{data['total']:3} ({pct:5.1f}%)")
    print(f"   ... and {len(groups)-10} more groups")
    print()
    
    print("6. Example Unity Test File Generated:")
    if os.path.exists('test_tck_expressions_mathematical.c'):
        print("   ✓ test_tck_expressions_mathematical.c")
        with open('test_tck_expressions_mathematical.c', 'r') as f:
            lines = f.readlines()[:25]  # Show first 25 lines
        for line in lines:
            print(f"   {line.rstrip()}")
        print("   ...")
    else:
        print("   (File not found - run generate_unity_tests.py)")
    
    print("\n=== Demo Complete ===")
    print("Use 'make tck-report' for quick status updates")
    print("Use 'python3 ../scripts/update_compliance.py' to recalculate compliance")

if __name__ == '__main__':
    main()
