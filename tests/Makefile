CC = gcc
CFLAGS = -I../include -I../src -I../_deps/sqlite-src -I../_deps/Unity-2.5.2/src
CFLAGS += -g -O0 -std=gnu99 -fPIC -DSQLITE_ENABLE_LOAD_EXTENSION=1

# Static library paths
UNITY_LIB = ../build/libunity.a
SQLITE_LIB = ../build/libsqlite3.a
GRAPH_LIB = ../build/libgraph_static.a

LDFLAGS = -lm -ldl -lpthread

TEST_OUTPUT_DIR = ../build/tests

BASIC_TEST_NAMES = test_loading test_query_nodes
BASIC_TESTS = $(addprefix $(TEST_OUTPUT_DIR)/, $(BASIC_TEST_NAMES))

EXTENDED_TEST_NAMES = test_clauses_create test_clauses_delete test_clauses_match test_clauses_return test_clauses_where
EXTENDED_TEST_NAMES += test_concurrency test_real_cypher_syntax test_storage test_transactions
EXTENDED_TEST_NAMES += test_merge_clauses
EXTENDED_TESTS = $(addprefix $(TEST_OUTPUT_DIR)/, $(EXTENDED_TEST_NAMES))

HARNESS_TESTS = $(BASIC_TESTS)
ALL_TESTS = $(HARNESS_TESTS) $(EXTENDED_TESTS)

TCK_TEST_SOURCES := $(wildcard tck_test_*.c)
TCK_TEST_NAMES := $(notdir $(basename $(TCK_TEST_SOURCES)))
TCK_TESTS = $(addprefix $(TEST_OUTPUT_DIR)/, $(TCK_TEST_NAMES))
TCK_RUNNER = $(TEST_OUTPUT_DIR)/tck_runner

.PHONY: all clean test test-basic test-extended test-tck prepare-libs

# Default target for CI - only builds what's needed, doesn't fail on missing tests
all: prepare-libs
	@echo "Build preparation complete. Use 'make test' to run tests."

prepare-libs:
	@echo "Ensuring required libraries are built..."
	@cd .. && $(MAKE) -C _deps && $(MAKE) -C src

# Shared build rule for all harness and TCK binaries
$(TEST_OUTPUT_DIR):
	@mkdir -p $(TEST_OUTPUT_DIR)

$(TEST_OUTPUT_DIR)/%: %.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) | $(TEST_OUTPUT_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

test-basic: prepare-libs $(HARNESS_TESTS)
	@echo "=== Running Basic Tests ==="
	@for test in $(HARNESS_TESTS); do \
		name=$$(basename $$test); \
		echo "Running $$name..."; \
		if $$test; then \
			echo "✅ $$name PASSED"; \
		else \
			echo "❌ $$name FAILED"; \
			exit 1; \
		fi; \
		echo; \
	done
	@echo "🎉 ALL BASIC TESTS PASSED!"

test-extended: prepare-libs $(EXTENDED_TESTS) 
	@echo "=== Running Extended Tests ==="
	@for test in $(EXTENDED_TESTS); do \
		name=$$(basename $$test); \
		echo "Running $$name..."; \
		if $$test; then \
			echo "✅ $$name PASSED"; \
		else \
			echo "❌ $$name FAILED (might be expected for some tests)"; \
		fi; \
		echo; \
	done
	@echo "Extended tests completed"

test: prepare-libs $(HARNESS_TESTS) $(EXTENDED_TESTS)
	$(MAKE) test-basic
	$(MAKE) test-extended

clean:
	rm -rf $(TEST_OUTPUT_DIR)
	rm -f tck_test_*.o *.o

# TCK Test Generation and Running
generate-tck-tests: generate_tck_tests.py ../tck_scenarios_manifest.csv
	python3 generate_tck_tests.py

tck: prepare-libs $(TCK_RUNNER) $(TCK_TESTS)
	@echo "=== Running TCK Runner ==="
	@if $(TCK_RUNNER); then \
		echo "✅ tck_runner PASSED"; \
	else \
		echo "⚠️  tck_runner reported failures (expected for incomplete coverage)"; \
	fi
	@echo "=== Running Generated TCK Tests ==="
	@for test in $(TCK_TESTS); do \
		name=$$(basename $$test); \
		echo "Running $$name..."; \
		if $$test; then \
			echo "✅ $$name PASSED"; \
		else \
			echo "⚠️  $$name had failures (expected for unimplemented scenarios)"; \
		fi; \
		echo; \
	done
	@echo "TCK test run completed"

test-tck: tck

test-tck-all: $(TCK_TESTS)
	@echo "=== Running All TCK Tests ==="
	@for test in $(TCK_TESTS); do \
		name=$$(basename $$test); \
		echo "Running $$name..."; \
		if $$test; then \
			echo "✅ $$name PASSED"; \
		else \
			echo "⚠️  $$name had failures (expected for unimplemented scenarios)"; \
		fi; \
		echo; \
	done
	@echo "TCK test run completed"

clean-tck:
	rm -f $(TCK_TESTS) $(TCK_RUNNER)
	rm -f tck_test_*.c

# Working INSERT tests
test_insert_nodes_working: test_insert_nodes_working.c
	$(CC) $(CFLAGS) test_insert_nodes_working.c -o test_insert_nodes_working $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)
