#!/bin/bash

# Simple build script for QM/MM integration test
set -e  # Exit on error

echo "Building QM/MM Integration for Healthcare Module (Simple Version)..."

# Create output directory
mkdir -p build_simple

# Set compiler flags
CXX=${CXX:-g++}
CXXFLAGS="-std=c++14 -I. -I./include"
LDFLAGS=""

# List of source files to compile
SOURCES=(
    "src/healthcare/bio_quantum_integration.cpp"
    "src/healthcare/chemotherapy/chemo_quantum_model.cpp"
    "src/healthcare/qmmm_integration_test.cpp"
)

# Compile and link
echo "Compiling QM/MM integration test..."
$CXX $CXXFLAGS ${SOURCES[@]} -o build_simple/qmmm_integration_test $LDFLAGS

echo "Build completed successfully."

# Make executable
chmod +x build_simple/qmmm_integration_test

# Check if the executable was created
if [ -f "build_simple/qmmm_integration_test" ]; then
    echo "Running QM/MM integration test..."

    # Default drug and radiation dose
    DRUG=${1:-"CISPLATIN"}
    DOSE=${2:-"2.0"}

    # Run the test
    ./build_simple/qmmm_integration_test "$DRUG" "$DOSE"

    echo "Test completed."
else
    echo "Error: Executable not found. Build may have failed."
    exit 1
fi
