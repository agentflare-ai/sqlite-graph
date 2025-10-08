#!/bin/bash
# Build thread safety test
gcc -pthread -o test_concurrent \
    tests/test_concurrent.c \
    -I./include -I./build/_deps/sqlite3-src \
    build/src/libgraph.a build/libsqlite3_lib.a \
    -lm -ldl

# Run with helgrind
valgrind --tool=helgrind \
         --error-exitcode=1 \
         ./test_concurrent

# Check result
if [ $? -eq 0 ]; then
  echo "PASS: No race conditions detected"
else
  echo "FAIL: Race conditions found"
  exit 1
fi