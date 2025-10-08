#!/bin/bash
# Build with coverage
cd "$(dirname "$0")/.."
mkdir -p build
cd build

# Configure with coverage flags
cmake -DCMAKE_BUILD_TYPE=Coverage ..
make clean && make

# Run tests
./tests/test_graph

# Generate report
gcovr --root .. \
      --exclude build/ \
      --exclude tests/ \
      --print-summary \
      --fail-under-line=95 \
      --fail-under-branch=90

echo "Coverage report generated in build/coverage/index.html"