# Main Makefile - Updated with Memory Management Hardening

# Compiler and flags
CC = gcc
CFLAGS = -I$(CURDIR)/include -I$(CURDIR)/src -I$(CURDIR)/_deps/sqlite-src -I$(CURDIR)/_deps/Unity-2.5.2/src -g -O0 -std=gnu99 -fPIC
LDFLAGS = -lm -ldl -lpthread

# Sanitizer flags when SANITIZE=1
ifeq ($(SANITIZE),1)
    CFLAGS += -fsanitize=address,undefined -Wall -Wextra
    LDFLAGS += -fsanitize=address,undefined
endif

# Enable SQLite extensions for all builds
CFLAGS += -DSQLITE_ENABLE_LOAD_EXTENSION=1

# Add the SQLite source to the CFLAGS
CFLAGS += -I$(CURDIR)/_deps/sqlite-src

# Directories
BUILD_DIR = build
SRC_DIR = src
TESTS_DIR = tests

.PHONY: all clean test rebuild deps test_tck sanitize harden

all: deps
	@mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(SRC_DIR) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"
	$(MAKE) -C $(TESTS_DIR) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

deps:
	$(MAKE) -C _deps CFLAGS="$(CFLAGS)"

clean:
	$(MAKE) -C $(SRC_DIR) clean
	$(MAKE) -C $(TESTS_DIR) clean
	@if [ -d _deps ]; then $(MAKE) -C _deps clean; fi
	rm -rf $(BUILD_DIR)

test:
	$(MAKE) -C $(TESTS_DIR) test

# TCK target - compiles and runs all TCK test files with extension support
test_tck: deps
	@mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(SRC_DIR) CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"
	$(MAKE) -C $(TESTS_DIR) tck CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

rebuild: clean all

# Sanitizer build with memory hardening
sanitize:
	@echo "Building with sanitizers and memory hardening enabled..."
	$(MAKE) clean
	$(MAKE) all SANITIZE=1
	@echo "Sanitizer build complete. Use for testing to detect memory issues."

# Apply memory management hardening
harden:
	@echo "Applying memory management hardening..."
	@./scripts/harden_memory.sh
	@echo "Memory hardening applied. Test with 'make test SANITIZE=1'"

# Test with memory validation
test_hardened: harden
	@echo "Testing hardened memory management..."
	$(MAKE) test SANITIZE=1
	@echo "Hardened memory tests complete!"

# Test Go operations in C
test_go_operations: $(BUILD_DIR)/tests/test_go_operations
	@echo "Running Go operations test in C..."
	@$(BUILD_DIR)/tests/test_go_operations

$(BUILD_DIR)/tests/test_go_operations: tests/test_go_operations.c $(BUILD_DIR)/libgraph_static.a
	@mkdir -p $(BUILD_DIR)/tests
	gcc -I./include -I./src -I./_deps/sqlite-src -I./_deps/Unity-2.5.2/src -g -O0 -std=gnu99 \
		-o $@ tests/test_go_operations.c -lsqlite3 -lm -ldl -lpthread
