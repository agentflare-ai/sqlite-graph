# Installation Guide

This guide provides detailed instructions for building and installing the SQLite Graph Database Extension.

## Prerequisites

### System Requirements

- **Operating System**: Linux, macOS, or Windows (with WSL)
- **Compiler**: GCC 4.8+ or Clang 3.8+
- **Build Tools**: Make, CMake (optional)
- **Memory**: At least 512MB RAM for building
- **Disk Space**: ~100MB for source and build files

### Dependencies

#### Required Dependencies
- **SQLite3**: Version 3.8.0 or higher
- **Standard C Library**: POSIX compliant

#### Optional Dependencies
- **Unity**: Testing framework (included in `_deps/`)
- **Python**: For Python bindings and examples
- **Valgrind**: For memory leak detection during development

## Quick Installation

### Option 1: Download Pre-built Binary (Linux x86_64) - Recommended

For most users on Linux, downloading the pre-built binary is the fastest option:

```bash
# Download the latest release
wget https://github.com/agentflare-ai/sqlite-graph/releases/latest/download/libgraph.so

# Or download a specific version (e.g., v0.1.0-alpha.0)
wget https://github.com/agentflare-ai/sqlite-graph/releases/download/v0.1.0-alpha.0/libgraph.so

# Verify the download (recommended)
wget https://github.com/agentflare-ai/sqlite-graph/releases/latest/download/checksums.txt
sha256sum -c checksums.txt

# The extension is ready to use!
```

**Supported Distributions**:
- Ubuntu 20.04, 22.04, 24.04 (x86_64)
- Debian 11, 12 (x86_64)
- Other Linux distributions with compatible glibc

#### Verifying Binary Downloads

For security, always verify the checksum of downloaded binaries:

```bash
# Download the checksum file
wget https://github.com/agentflare-ai/sqlite-graph/releases/latest/download/SHA256SUMS.txt

# Verify the libgraph.so file
sha256sum -c SHA256SUMS.txt --ignore-missing
```

Expected output: `libgraph.so: OK`

⚠️ **Security Note**: This is an alpha release. Binaries are not code-signed. For production use, we recommend building from audited source code.

### Option 2: Build from Source

If pre-built binaries aren't available for your platform, or you want to customize the build:

```bash
# Clone the repository
git clone https://github.com/agentflare-ai/sqlite-graph.git
cd sqlite-graph

# Build the extension
make

# The extension will be available at build/libgraph.so
```

### Build System

The project uses Make for a simple, reliable build process:

```bash
# Build the extension
make

# Clean build artifacts
make clean

# Run tests
make test

# The extension will be available at build/libgraph.so
```

## Detailed Build Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/agentflare-ai/sqlite-graph.git
cd sqlite-graph
```

### 2. Verify Dependencies

Check that you have the required tools:

```bash
# Check GCC version
gcc --version

# Check Make
make --version

# Check SQLite3 (if system-installed)
sqlite3 --version
```

### 3. Build Process

The build system automatically:
1. Downloads and builds SQLite3 and Unity dependencies
2. Compiles all source files with appropriate flags
3. Links the shared library extension
4. Builds test executables

```bash
# Clean any previous builds
make clean

# Build everything
make

# Build with debug symbols
make debug

# Build optimized release version
make release
```

### 4. Verify Installation

```bash
# Check that the extension was built
ls -la build/libgraph.so

# Run basic tests
make test

# Test the extension interactively
sqlite3 :memory: "
.load ./build/libgraph.so
CREATE VIRTUAL TABLE graph USING graph();
SELECT 'Extension loaded successfully' as status;
"
```

## Build Configuration

### Compiler Flags

The build system uses these flags by default:

```bash
# Debug build
CFLAGS = -g -O0 -std=gnu99 -fPIC -Wall -Wextra

# Release build  
CFLAGS = -O3 -DNDEBUG -std=gnu99 -fPIC -Wall -Wextra
```

### Customizing the Build

You can customize the build by setting environment variables:

```bash
# Use different compiler
export CC=clang
make

# Add custom flags
export CFLAGS="-g -O2 -DCUSTOM_FLAG"
make

# Build with AddressSanitizer
export CFLAGS="-g -O1 -fsanitize=address"
make
```

## Platform-Specific Instructions

### Linux (Ubuntu/Debian)

```bash
# Install build dependencies
sudo apt-get update
sudo apt-get install build-essential libsqlite3-dev

# Build
make
```

### Linux (CentOS/RHEL)

```bash
# Install build dependencies
sudo yum install gcc make sqlite-devel
# Or on newer versions:
sudo dnf install gcc make sqlite-devel

# Build
make
```

### macOS

```bash
# Install Xcode command line tools
xcode-select --install

# Install dependencies via Homebrew (optional)
brew install sqlite3

# Build
make
```

### Windows (WSL)

```bash
# Install WSL and Ubuntu
# Then follow Ubuntu instructions above
```

## Installation Options

### Option 1: Local Installation

Keep the extension in the build directory and load it using the full path:

```python
import sqlite3
conn = sqlite3.connect(":memory:")
conn.enable_load_extension(True)
conn.load_extension("./build/libgraph.so")
```

### Option 2: System Installation

Copy the extension to a system directory:

```bash
# Copy to system extension directory
sudo cp build/libgraph.so /usr/local/lib/

# Or create a symbolic link
sudo ln -s $(pwd)/build/libgraph.so /usr/local/lib/libgraph.so

# Load without path
conn.load_extension("libgraph")
```

### Option 3: Python Package Installation

For Python development, you can install the extension as a package:

```bash
# Create setup.py (example)
cat > setup.py << 'EOF'
from setuptools import setup, Extension

setup(
    name="sqlite-graph",
    version="1.0.0",
    ext_modules=[
        Extension(
            "sqlite_graph",
            ["build/libgraph.so"],
            include_dirs=["include/"]
        )
    ]
)
EOF

# Install
pip install -e .
```

## Testing the Installation

### Basic Functionality Test

```bash
# Run the test suite
make test

# Run specific test
cd build/tests && ./test_cypher_basic
```

### Interactive Test

```bash
sqlite3 :memory: "
.load ./build/libgraph.so
CREATE VIRTUAL TABLE graph USING graph();
.schema

-- Add some test data
INSERT INTO graph (command) VALUES ('CREATE (p:Person {name: \"Alice\"})');
INSERT INTO graph (command) VALUES ('CREATE (p:Person {name: \"Bob\"})');
INSERT INTO graph (command) VALUES ('MATCH (a:Person {name: \"Alice\"}), (b:Person {name: \"Bob\"}) CREATE (a)-[:KNOWS]->(b)');

-- Query the graph
SELECT * FROM graph WHERE command = 'MATCH (p:Person) RETURN p.name';
"
```

### Python Integration Test

```python
import sqlite3
import os

# Test script
def test_extension():
    # Check if extension exists
    ext_path = "./build/libgraph.so"
    if not os.path.exists(ext_path):
        print(f"Extension not found at {ext_path}")
        return False
    
    try:
        # Load extension
        conn = sqlite3.connect(":memory:")
        conn.enable_load_extension(True)
        conn.load_extension(ext_path)
        
        # Test basic functionality
        conn.execute("CREATE VIRTUAL TABLE graph USING graph()")
        conn.execute("INSERT INTO graph (command) VALUES ('CREATE (p:Person {name: \"Test\"})')")
        
        result = conn.execute("SELECT * FROM graph WHERE command = 'MATCH (p:Person) RETURN p.name'").fetchall()
        print(f"Test passed: {result}")
        return True
        
    except Exception as e:
        print(f"Test failed: {e}")
        return False
    finally:
        conn.close()

# Run test
if __name__ == "__main__":
    test_extension()
```

## Troubleshooting

### Common Issues

#### 1. "libgraph.so not found"

```bash
# Check if file exists
ls -la build/libgraph.so

# Rebuild if missing
make clean && make
```

#### 2. "Extension loading failed"

```bash
# Check permissions
chmod +x build/libgraph.so

# Check dependencies
ldd build/libgraph.so
```

#### 3. Compilation errors

```bash
# Check compiler version
gcc --version

# Try with different flags
export CFLAGS="-g -O0 -std=c99"
make clean && make
```

#### 4. SQLite version mismatch

```bash
# Check SQLite version
sqlite3 --version

# Use bundled SQLite
make clean && make  # Uses bundled SQLite automatically
```

### Getting Help

If you encounter issues:

1. Check the [GitHub Issues](https://github.com/agentflare-ai/sqlite-graph/issues)
2. Search existing discussions
3. Create a new issue with:
   - Your operating system and version
   - Compiler version
   - Complete error messages
   - Steps to reproduce

## Performance Optimization

### Build Optimizations

```bash
# Maximum optimization
export CFLAGS="-O3 -DNDEBUG -march=native -flto"
make clean && make

# Profile-guided optimization
export CFLAGS="-O3 -fprofile-generate"
make clean && make
# Run typical workload
export CFLAGS="-O3 -fprofile-use"
make clean && make
```

### Runtime Optimizations

```bash
# Increase SQLite cache size
sqlite3 :memory: "
.load ./build/libgraph.so
PRAGMA cache_size = 10000;
PRAGMA synchronous = OFF;
PRAGMA journal_mode = MEMORY;
"
```

## Development Setup

For development work, see the [Contributing Guide](CONTRIBUTING.md) for additional setup instructions including:

- Code formatting tools
- Testing frameworks
- Debugging configurations
- Continuous integration setup

---

**Need help?** Check our [GitHub Issues](https://github.com/agentflare-ai/sqlite-graph/issues) or [Discussions](https://github.com/agentflare-ai/sqlite-graph/discussions).

**Part of the AgentFlare AI ecosystem**
