#!/bin/bash

# Exit on error
set -e

# Compile the simplified test
echo "\n===== Compiling Simplified Test =====\n"
g++ -std=c++14 simplified_test.cpp -o simplified_test

# Run the test
echo "\n===== Running Simplified Test =====\n"
./simplified_test

echo "\n===== Test Complete =====\n"
