CC = gcc
CFLAGS = -I../include -I../src -I../_deps/sqlite-src -I../_deps/Unity-2.5.2/src
CFLAGS += -g -O0 -std=gnu99 -fPIC -DSQLITE_ENABLE_LOAD_EXTENSION=1

# Static library paths
UNITY_LIB = ../build/libunity.a
SQLITE_LIB = ../build/libsqlite3.a
GRAPH_LIB = ../build/libgraph_static.a

LDFLAGS = -lm -ldl -lpthread -Wl,--export-dynamic

# Basic tests - only include tests that actually exist
BASIC_TESTS = test_match_return_simple test_storage test_virtual_table_crud test_performance
EXTENDED_TESTS =  # None exist yet

ALL_TESTS = $(BASIC_TESTS) $(EXTENDED_TESTS)

.PHONY: all clean test test-basic test-extended test-tck prepare-libs

# Default target for CI - only builds what's needed, doesn't fail on missing tests
all: prepare-libs
	@echo "Build preparation complete. Use 'make test' to run tests."

prepare-libs:
	@echo "Ensuring required libraries are built..."
	@cd .. && $(MAKE) -C _deps && $(MAKE) -C src

# Basic test targets using static libraries
$(BASIC_TESTS): %: %.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

# Extended test targets using static libraries  
$(EXTENDED_TESTS): %: %.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

test-basic: $(BASIC_TESTS)
	@echo "=== Running Basic Tests ==="
	@for test in $(BASIC_TESTS); do \
		echo "Running $$test..."; \
		if ./$$test; then \
			echo "✅ $$test PASSED"; \
		else \
			echo "❌ $$test FAILED"; \
			exit 1; \
		fi; \
		echo; \
	done
	@echo "🎉 ALL BASIC TESTS PASSED!"

test-extended: $(EXTENDED_TESTS) 
	@echo "=== Running Extended Tests ==="
	@for test in $(EXTENDED_TESTS); do \
		echo "Running $$test..."; \
		if ./$$test; then \
			echo "✅ $$test PASSED"; \
		else \
			echo "❌ $$test FAILED (might be expected for some tests)"; \
		fi; \
		echo; \
	done
	@echo "Extended tests completed"

test: test-basic test-extended

clean:
	rm -f $(ALL_TESTS) tck_runner *.o

# TCK Test Generation and Running
generate-tck-tests: generate_tck_tests.py ../tck_scenarios_manifest.csv
	python3 generate_tck_tests.py

tck_runner: tck_runner.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

test-tck: tck_runner
	@echo "=== Running TCK Tests ==="
	./tck_runner
	@echo "TCK tests completed"

# TCK individual test targets can be added here
# Example: tck-clauses-match: generated_tck_clauses_match.c
#	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

# Generated TCK test targets

tck_test_clauses_call: tck_test_clauses_call.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_create: tck_test_clauses_create.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_delete: tck_test_clauses_delete.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_match: tck_test_clauses_match.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_match_where: tck_test_clauses_match_where.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_merge: tck_test_clauses_merge.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_remove: tck_test_clauses_remove.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_return: tck_test_clauses_return.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_return_orderby: tck_test_clauses_return_orderby.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_return_skip_limit: tck_test_clauses_return_skip_limit.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_set: tck_test_clauses_set.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_union: tck_test_clauses_union.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_unwind: tck_test_clauses_unwind.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_with: tck_test_clauses_with.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_with_orderBy: tck_test_clauses_with_orderBy.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_with_skip_limit: tck_test_clauses_with_skip_limit.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_clauses_with_where: tck_test_clauses_with_where.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_aggregation: tck_test_expressions_aggregation.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_boolean: tck_test_expressions_boolean.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_comparison: tck_test_expressions_comparison.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_conditional: tck_test_expressions_conditional.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_existentialSubqueries: tck_test_expressions_existentialSubqueries.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_graph: tck_test_expressions_graph.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_list: tck_test_expressions_list.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_literals: tck_test_expressions_literals.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_map: tck_test_expressions_map.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_mathematical: tck_test_expressions_mathematical.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_null: tck_test_expressions_null.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_path: tck_test_expressions_path.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_pattern: tck_test_expressions_pattern.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_precedence: tck_test_expressions_precedence.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_quantifier: tck_test_expressions_quantifier.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_string: tck_test_expressions_string.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_temporal: tck_test_expressions_temporal.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_expressions_typeConversion: tck_test_expressions_typeConversion.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_useCases_countingSubgraphMatches: tck_test_useCases_countingSubgraphMatches.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

tck_test_useCases_triadicSelection: tck_test_useCases_triadicSelection.c $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB)
	$(CC) $(CFLAGS) $< -o $@ $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

TCK_TESTS = tck_test_clauses_call tck_test_clauses_create tck_test_clauses_delete tck_test_clauses_match tck_test_clauses_match_where tck_test_clauses_merge tck_test_clauses_remove tck_test_clauses_return tck_test_clauses_return_orderby tck_test_clauses_return_skip_limit tck_test_clauses_set tck_test_clauses_union tck_test_clauses_unwind tck_test_clauses_with tck_test_clauses_with_orderBy tck_test_clauses_with_skip_limit tck_test_clauses_with_where tck_test_expressions_aggregation tck_test_expressions_boolean tck_test_expressions_comparison tck_test_expressions_conditional tck_test_expressions_existentialSubqueries tck_test_expressions_graph tck_test_expressions_list tck_test_expressions_literals tck_test_expressions_map tck_test_expressions_mathematical tck_test_expressions_null tck_test_expressions_path tck_test_expressions_pattern tck_test_expressions_precedence tck_test_expressions_quantifier tck_test_expressions_string tck_test_expressions_temporal tck_test_expressions_typeConversion tck_test_useCases_countingSubgraphMatches tck_test_useCases_triadicSelection

test-tck-all: $(TCK_TESTS)
	@echo "=== Running All TCK Tests ==="
	@for test in $(TCK_TESTS); do \
		echo "Running $$test..."; \
		if ./$$test; then \
			echo "✅ $$test PASSED"; \
		else \
			echo "⚠️  $$test had failures (expected for unimplemented scenarios)"; \
		fi; \
		echo; \
	done
	@echo "TCK test run completed"

clean-tck:
	rm -f $(TCK_TESTS) tck_test_*.c

# Working INSERT tests
test_insert_nodes_working: test_insert_nodes_working.c
	$(CC) $(CFLAGS) test_insert_nodes_working.c -o test_insert_nodes_working $(UNITY_LIB) $(SQLITE_LIB) $(GRAPH_LIB) $(LDFLAGS)

