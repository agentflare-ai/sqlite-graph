# src/Makefile

# Platform detection
ifeq ($(shell uname -s),Darwin)
CONFIG_DARWIN=y
else ifeq ($(OS),Windows_NT)
CONFIG_WINDOWS=y
else
CONFIG_LINUX=y
endif

ifdef CONFIG_DARWIN
LOADABLE_EXTENSION=dylib
LDFLAGS_SHARED=-undefined dynamic_lookup
endif

ifdef CONFIG_LINUX
LOADABLE_EXTENSION=so
LDFLAGS_SHARED=
endif

ifdef CONFIG_WINDOWS
LOADABLE_EXTENSION=dll
LDFLAGS_SHARED=
endif

# Compiler and flags
CC = gcc
CFLAGS = -I. -I../include -I../_deps/sqlite-src -I../_deps/Unity-2.5.2/src -I./src -g -O0 -std=gnu99 -fPIC -Wall -Wextra 
CFLAGS += $(EXTRA_CFLAGS)
LDFLAGS = -lm

# Directories
BUILD_DIR = ../build
OBJ_DIR = $(BUILD_DIR)/obj/src

# Source files - include all C files
SRCS = $(wildcard *.c) $(wildcard cypher/*.c)
SRCS += graph-vtab.c
SRCS += graph-memory.c
SRCS += graph-destructors.c
# Add graph-advanced.c to SRCS to ensure its object file is included in main libraries
SRCS += graph-advanced.c
SRCS += cypher/cypher-ast.c

# Objects for graph code only (no sqlite amalgamation)
OBJS_GRAPH = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

# Objects for static libraries (includes sqlite3.o)
OBJS_STATIC = $(OBJS_GRAPH) ../_deps/sqlite-src/sqlite3.o

# Static library for test utilities
TEST_UTIL_SRCS = graph-util.c graph-algo.c graph-advanced.c
TEST_UTIL_OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(TEST_UTIL_SRCS))

.PHONY: all clean

all: $(BUILD_DIR)/libgraph.$(LOADABLE_EXTENSION) $(BUILD_DIR)/libgraph_static.a $(BUILD_DIR)/libgraph_test_util.a

# Loadable extension does NOT include sqlite3.o
$(BUILD_DIR)/libgraph.$(LOADABLE_EXTENSION): $(OBJS_GRAPH)
	$(CC) -shared -o $@ $^ $(LDFLAGS) $(LDFLAGS_SHARED)

# Static library includes sqlite3.o for standalone testing
$(BUILD_DIR)/libgraph_static.a: $(OBJS_STATIC)
	ar rcs $@ $^

$(BUILD_DIR)/libgraph_test_util.a: $(TEST_UTIL_OBJS)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: cypher/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS_GRAPH) $(TEST_UTIL_OBJS) $(BUILD_DIR)/libgraph.$(LOADABLE_EXTENSION) $(BUILD_DIR)/libgraph.so $(BUILD_DIR)/libgraph_static.a $(BUILD_DIR)/libgraph_test_util.a
	rm -f ../_deps/sqlite-src/sqlite3.o
