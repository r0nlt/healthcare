#!/bin/bash

# Build script for Quantum Monte Carlo Test
# Part of rad-tolerant-ml framework

echo "Building Quantum Monte Carlo Wave Equation Test..."

# Create build directory if it doesn't exist
mkdir -p build

# Compile the quantum MC test
g++ -std=c++17 -O3 -pthread -I./include src/quantum_mc_test.cpp -o build/quantum_mc_test

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

echo "Build successful!"

# Run the test with provided arguments
echo "Running Quantum Monte Carlo test with arguments: $@"
./build/quantum_mc_test "$@"
