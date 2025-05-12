#!/bin/bash
set -e  # Exit on any error

echo "=== Radiation Tolerant ML Build Script ==="
echo "This script will build all components of the project."

# Simply use the root Makefile since it's already properly configured
echo "Building all targets with the project's Makefile..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

echo "Build completed successfully!"
echo "Executable files are located in the project root directory."
echo "To run tests, use: ./tools/run_tests.sh" 