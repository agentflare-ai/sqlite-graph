#!/bin/bash

# Script to systematically replace malloc/free with sqlite3_malloc/sqlite3_free
# and add memory management hardening across the extension

echo "Hardening memory management across SQLite graph extension..."

# Function to process a single file
harden_file() {
    local file="$1"
    if [ ! -f "$file" ]; then
        return
    fi
    
    echo "Processing: $file"
    
    # Create backup
    cp "$file" "${file}.backup"
    
    # Replace malloc/free patterns with SQLite equivalents
    sed -i 's/\bmalloc(/sqlite3_malloc(/g' "$file"
    sed -i 's/\bfree(/sqlite3_free(/g' "$file"
    sed -i 's/\bcalloc(/sqlite3_malloc(/g' "$file"
    sed -i 's/\brealloc(/sqlite3_realloc(/g' "$file"
    
    # Add memory.h include if not present
    if ! grep -q "graph-memory.h" "$file"; then
        sed -i '/^#include.*graph.*\.h/a #include "graph-memory.h"' "$file"
    fi
    
    echo "  - Hardened malloc/free patterns"
    echo "  - Added memory management include"
}

# Process all source files
find src -name "*.c" | while read -r file; do
    harden_file "$file"
done

echo "Memory management hardening complete!"
echo "Backup files created with .backup extension"
