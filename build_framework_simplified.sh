#!/bin/bash

# Exit on error
set -e

# Compile the simplified framework test
echo "\n===== Compiling Simplified Framework Test =====\n"
g++ -std=c++14 framework_test_simplified.cpp -o framework_test_simplified

# Run the test
echo "\n===== Running Simplified Framework Test =====\n"
./framework_test_simplified

echo "\n===== Framework Test Complete =====\n"
