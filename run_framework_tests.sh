#!/bin/bash

# Exit on error
set -e

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake .. -DBUILD_HEALTHCARE=ON -DBUILD_TESTS=ON

# Build the project
make -j$(nproc)

# Run the simplified test
echo "\n===== Running Simplified Test =====\n"
./simplified_test | tee ../simplified_test_results.txt

# Run the full framework test if it exists
if [ -f "./test/healthcare/full_framework_test" ]; then
    echo "\n===== Running Full Framework Test =====\n"
    ./test/healthcare/full_framework_test | tee ../full_framework_test_results.txt
else
    echo "\n===== Full Framework Test not built =====\n"
    echo "Move full_framework_test.cpp to test/healthcare/ directory and rebuild"
fi

echo "\n===== Tests Complete =====\n"
echo "Results saved to simplified_test_results.txt and full_framework_test_results.txt"
