#!/usr/bin/env python3
"""
TCK Plan Generator
Generates comprehensive implementation plan files for all TCK features
"""

import os
import re
from pathlib import Path
from datetime import datetime
from typing import List, Dict, Tuple

class Scenario:
    def __init__(self, number: int, name: str):
        self.number = number
        self.name = name
        self.query = ""
        self.expected = ""
        self.given = ""
        self.is_error_test = False
        self.error_type = ""

class Feature:
    def __init__(self, name: str, description: str):
        self.name = name
        self.description = description
        self.scenarios: List[Scenario] = []
        self.category = ""
        self.subcategory = ""

def parse_feature_file(file_path: Path) -> Feature:
    """Parse a .feature file and extract structure"""
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    feature = None
    current_scenario = None
    in_query = False
    in_result = False
    query_buffer = []

    for line in lines:
        line_stripped = line.strip()

        # Parse feature line
        if line_stripped.startswith('Feature:'):
            match = re.match(r'Feature:\s+(\w+)\s*-\s*(.+)', line_stripped)
            if match:
                feature = Feature(match.group(1), match.group(2))

        # Parse scenario line
        elif line_stripped.startswith('Scenario:'):
            match = re.match(r'Scenario:\s+\[(\d+)\]\s*(.+)', line_stripped)
            if match and feature:
                current_scenario = Scenario(int(match.group(1)), match.group(2))
                feature.scenarios.append(current_scenario)

        # Parse Given
        elif line_stripped.startswith('Given') and current_scenario:
            current_scenario.given = line_stripped

        # Parse When executing query
        elif 'When executing query:' in line_stripped:
            in_query = True
            query_buffer = []
        elif in_query and '"""' in line_stripped:
            if query_buffer:  # Closing quote
                in_query = False
                if current_scenario:
                    current_scenario.query = '\n'.join(query_buffer).strip()
                    query_buffer = []
        elif in_query:
            query_buffer.append(line.rstrip())

        # Parse Then (error tests)
        elif line_stripped.startswith('Then a ') and 'Error should be raised' in line_stripped:
            if current_scenario:
                current_scenario.is_error_test = True
                match = re.search(r'(\w+)Error', line_stripped)
                if match:
                    current_scenario.error_type = match.group(1) + 'Error'

        # Parse Then (result tests)
        elif line_stripped.startswith('Then the result should be'):
            in_result = True
            if current_scenario:
                current_scenario.expected = "Result table (see feature file)"

    return feature

def get_priority(category: str, feature_name: str) -> str:
    """Determine priority based on category and feature"""
    # Critical: Foundation clauses
    if category in ['match', 'create', 'return'] and '1' in feature_name:
        return 'CRITICAL'
    # High: Common operations
    if category in ['match', 'with', 'where', 'return', 'create']:
        return 'HIGH'
    # Medium: Advanced features
    if category in ['merge', 'delete', 'set', 'orderby', 'skip', 'limit']:
        return 'MEDIUM'
    # Low: Everything else
    return 'LOW'

def generate_plan_content(feature: Feature, feature_file: Path) -> str:
    """Generate markdown content for a plan file"""

    date_str = datetime.now().strftime('%Y-%m-%d')
    scenario_count = len(feature.scenarios)
    priority = get_priority(feature.category, feature.name)

    # Build scenario status table
    scenario_list = ', '.join([f"{s.number:02d}" for s in feature.scenarios])

    content = f"""# {feature.name} - {feature.description} - Task List

**Feature:** {feature.name} - {feature.description}
**Status:** 🔴 NOT STARTED (0/{scenario_count} scenarios)
**Priority:** {priority}
**Last Updated:** {date_str}

---

## Overview

{feature.description}

**Feature File:** `{feature_file}`
**Total Scenarios:** {scenario_count}

**Key Capabilities:**
"""

    # Add key capabilities based on scenarios
    for i, scenario in enumerate(feature.scenarios[:5]):  # First 5 scenarios
        content += f"- {scenario.name}\n"
    if len(feature.scenarios) > 5:
        content += f"- ... and {len(feature.scenarios) - 5} more scenarios\n"

    content += f"""
---

## Quick Status

| Status | Count | Scenarios |
|--------|-------|-----------|
| 🔴 Not Started | {scenario_count} | {scenario_list} |

---

## Test Scenarios

"""

    # Add each scenario
    for scenario in feature.scenarios:
        content += f"""### 🔴 [Scenario {scenario.number:02d}] {scenario.name}
**Status:** 🔴 NOT STARTED
**Priority:** {priority}

**Test:**
```cypher
{scenario.query if scenario.query else '(See feature file)'}
```

"""
        if scenario.is_error_test:
            content += f"**Expected:** {scenario.error_type}\n\n"
        else:
            content += f"**Expected:** {scenario.expected}\n\n"

        content += """**TODO:**
- [ ] Implement test case
- [ ] Parse and compile query
- [ ] Execute query
- [ ] Validate results
- [ ] Handle edge cases

"""

        if scenario.is_error_test:
            content += "**Dependencies:** Error handling infrastructure\n"
        else:
            content += "**Dependencies:** Basic query execution\n"

        content += "**Estimated Effort:** TBD\n\n---\n\n"

    # Implementation plan
    content += f"""## Implementation Plan

### Phase 1: Foundation
**Goal:** Implement basic {feature.name} functionality
**Timeline:** TBD

**Tasks:**
1. Study feature requirements
2. Design implementation approach
3. Implement core functionality
4. Write initial tests
5. Debug and validate

### Phase 2: Complete Implementation
**Goal:** All scenarios passing
**Timeline:** TBD

**Tasks:**
1. Implement remaining scenarios
2. Handle edge cases
3. Error validation
4. Performance optimization
5. Complete test coverage

---

## Files to Modify

**Likely files to modify:**
- [ ] `src/cypher/cypher-parser.c` - Query parsing
- [ ] `src/cypher/cypher-planner.c` - Query planning
- [ ] `src/cypher/cypher-physical-plan.c` - Physical execution plan
- [ ] `src/cypher/cypher-iterators.c` - Execution iterators
- [ ] `tests/tck_test_*.c` - Test implementation

---

## Success Criteria

### Minimum Viable
- [ ] At least 50% of scenarios passing
- [ ] Basic functionality working
- [ ] Core use cases covered

### Production Ready
- [ ] All scenarios passing
- [ ] Error handling complete
- [ ] Edge cases handled
- [ ] Performance acceptable

---

## Dependencies

**Prerequisites:**
- Basic query parsing infrastructure
- Execution engine
- Result formatting

**Blocks:**
- Features that depend on this implementation

---

## Notes

- Feature file: `{feature_file}`
- Total scenarios: {scenario_count}
- This plan was auto-generated on {date_str}
- Review and update priorities as implementation progresses

---

## Quick Reference

**Run Tests:**
```bash
./build/tests/tck_test_{feature.category}
```

**Feature File Location:**
```
{feature_file}
```
"""

    return content

def generate_category_readme(category: str, features: List[Tuple[str, str]], plans_dir: Path) -> str:
    """Generate README.md for a category directory"""

    content = f"""# {category.upper()} TCK Implementation Plans

This directory contains implementation plans for all {category.upper()} TCK features.

## Features

"""

    for i, (feature_name, description) in enumerate(features, 1):
        plan_file = f"{i:02d}_TCK_{feature_name}.md"
        content += f"{i}. **{feature_name}** - {description}\n"
        content += f"   - Plan: [{plan_file}]({plan_file})\n"
        content += f"   - Status: 🔴 NOT STARTED\n\n"

    content += f"""
## Progress Tracking

| Feature | Status | Scenarios | Pass Rate |
|---------|--------|-----------|-----------|
"""

    for i, (feature_name, _) in enumerate(features, 1):
        content += f"| {feature_name} | 🔴 NOT STARTED | 0/? | 0% |\n"

    content += f"""
## Quick Commands

**Run all {category} tests:**
```bash
./build/tests/tck_test_{category}
```

**View specific plan:**
```bash
cat .agents/plans/{category}/NN_TCK_FeatureName.md
```
"""

    return content

def main():
    """Main execution"""

    base_dir = Path("/Users/gabrielwillen/VSCode/agentflare/sqlite-graph")
    tck_dir = base_dir / "tck" / "features"
    plans_dir = base_dir / ".agents" / "plans"

    # Track what we already have
    existing_categories = {'create', 'delete', 'match', 'merge', 'with'}

    print("=== TCK Plan Generator ===\n")

    # Collect all feature files
    all_features = {}

    for root, dirs, files in os.walk(tck_dir):
        for filename in files:
            if not filename.endswith('.feature'):
                continue

            file_path = Path(root) / filename
            rel_path = Path(root).relative_to(tck_dir)

            # Determine category and subcategory
            if 'clauses' in rel_path.parts:
                category = rel_path.parts[1]  # e.g., 'match', 'return'
                subcategory = 'clauses'
            elif 'expressions' in rel_path.parts:
                category = f"expressions_{rel_path.parts[1]}"
                subcategory = 'expressions'
            elif 'useCases' in rel_path.parts:
                category = f"usecases_{rel_path.parts[1]}"
                subcategory = 'useCases'
            else:
                category = 'other'
                subcategory = 'other'

            # Skip if category already has plans (unless it's incomplete)
            base_category = category.replace('expressions_', '').replace('usecases_', '')

            if category not in all_features:
                all_features[category] = []

            # Parse feature file
            try:
                feature = parse_feature_file(file_path)
                if feature and feature.name:
                    feature.category = category
                    feature.subcategory = subcategory
                    all_features[category].append((feature, file_path))
            except Exception as e:
                print(f"Warning: Could not parse {file_path}: {e}")

    # Generate plans for each category
    total_generated = 0

    for category, features in sorted(all_features.items()):
        # Create category directory
        category_dir = plans_dir / category

        # Skip categories that already have complete plans
        base_cat = category.split('_')[-1]
        if base_cat in existing_categories:
            print(f"Skipping {category} (already has plans)")
            continue

        category_dir.mkdir(parents=True, exist_ok=True)
        print(f"\n{'='*60}")
        print(f"Category: {category}")
        print(f"Features: {len(features)}")
        print(f"{'='*60}")

        # Generate plan for each feature
        feature_list = []
        for i, (feature, feature_file) in enumerate(sorted(features, key=lambda x: x[0].name), 1):
            plan_filename = f"{i:02d}_TCK_{feature.name}_{feature.description.replace(' ', '_')[:40]}.md"
            plan_filename = re.sub(r'[^a-zA-Z0-9_.-]', '', plan_filename)
            plan_path = category_dir / plan_filename

            print(f"  {i:2d}. Generating: {plan_filename}")

            # Generate plan content
            rel_feature_file = feature_file.relative_to(base_dir)
            content = generate_plan_content(feature, rel_feature_file)

            # Write plan file
            with open(plan_path, 'w', encoding='utf-8') as f:
                f.write(content)

            feature_list.append((feature.name, feature.description))
            total_generated += 1

        # Generate README for category
        readme_content = generate_category_readme(category, feature_list, category_dir)
        readme_path = category_dir / "README.md"
        with open(readme_path, 'w', encoding='utf-8') as f:
            f.write(readme_content)

        print(f"  ✓ Generated README.md")

    print(f"\n{'='*60}")
    print(f"SUMMARY")
    print(f"{'='*60}")
    print(f"Total plans generated: {total_generated}")
    print(f"Total categories: {len([c for c in all_features.keys() if c.split('_')[-1] not in existing_categories])}")
    print(f"\nAll plans generated successfully!")

if __name__ == '__main__':
    main()
