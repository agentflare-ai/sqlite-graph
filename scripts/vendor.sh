#!/bin/bash

echo "Setting up vendor dependencies for sqlite_graph..."

# Create _deps directory structure
mkdir -p _deps/sqlite-src
mkdir -p _deps/Unity-2.5.2/src

# Download SQLite amalgamation (same version as sqlite_vec)
echo "Downloading SQLite amalgamation..."
if curl -f -L -o sqlite-amalgamation.zip https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip 2>/dev/null; then
    unzip -q sqlite-amalgamation.zip
    mv sqlite-amalgamation-3450300/* _deps/sqlite-src/
    rmdir sqlite-amalgamation-3450300
    rm sqlite-amalgamation.zip
    echo "SQLite downloaded successfully"
else
    echo "Warning: Failed to download SQLite. Ensure sqlite3.c is in _deps/sqlite-src/"
fi

# Download Unity testing framework
echo "Downloading Unity testing framework..."
if curl -f -L -o unity.zip https://github.com/ThrowTheSwitch/Unity/archive/v2.5.2.zip 2>/dev/null; then
    unzip -q unity.zip
    mv Unity-2.5.2/* _deps/Unity-2.5.2/
    rm -rf Unity-2.5.2
    rm unity.zip
    echo "Unity downloaded successfully"
else
    echo "Warning: Failed to download Unity. Ensure unity.c is in _deps/Unity-2.5.2/src/"
fi

# Always create _deps/Makefile
cat > _deps/Makefile << 'EOF'
.PHONY: all clean

all:
	@echo "Building SQLite for sqlite_graph..."
	@if [ ! -f sqlite-src/sqlite3.o ]; then \
		$(CC) -c $(CFLAGS) \
			-DSQLITE_ENABLE_STMT_SCANSTATUS -DSQLITE_ENABLE_BYTECODE_VTAB \
			-DSQLITE_ENABLE_EXPLAIN_COMMENTS -fPIC \
			sqlite-src/sqlite3.c -o sqlite-src/sqlite3.o; \
	fi
	@if [ ! -f Unity-2.5.2/src/unity.o ]; then \
		$(CC) -c $(CFLAGS) Unity-2.5.2/src/unity.c -o Unity-2.5.2/src/unity.o; \
	fi
	@echo "Dependencies built successfully"

clean:
	rm -f sqlite-src/sqlite3.o
	rm -f Unity-2.5.2/src/unity.o
EOF

echo "Vendor dependencies setup complete!"
