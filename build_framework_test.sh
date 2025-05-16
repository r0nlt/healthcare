#!/bin/bash

# Exit on error
set -e

# Compile the full framework test
echo "\n===== Compiling Full Framework Test =====\n"
g++ -std=c++14 full_framework_test.cpp -o full_framework_test

# Run the test
echo "\n===== Running Full Framework Test =====\n"
./full_framework_test

echo "\n===== Full Framework Test Complete =====\n"
