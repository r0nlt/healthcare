#!/bin/bash

# Script to compile and validate the rad-tolerant-ml healthcare module
# Author: Claude

set -e  # Exit on error

echo "============================================="
echo "  Rad-Tolerant ML Healthcare Module Validation"
echo "============================================="

# Create a build directory
mkdir -p build
cd build

# Configure CMake
echo "Configuring CMake..."
cmake .. -DBUILD_HEALTHCARE=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON

# Build
echo "Building libraries and tests..."
make -j4

# Run the healthcare tests
echo -e "\nRunning healthcare tests..."

# Check if tests exist
if [ -f "./test/healthcare/quantum_validation_test" ]; then
    echo "Running quantum validation tests..."
    ./test/healthcare/quantum_validation_test
else
    echo "WARNING: quantum_validation_test not found."
fi

if [ -f "./test/healthcare/dna_damage_test" ]; then
    echo "Running DNA damage tests..."
    ./test/healthcare/dna_damage_test
else
    echo "WARNING: dna_damage_test not found."
fi

if [ -f "./test/healthcare/cell_cycle_test" ]; then
    echo "Running cell cycle tests..."
    ./test/healthcare/cell_cycle_test
else
    echo "WARNING: cell_cycle_test not found. You may need to implement this test."
fi

if [ -f "./test/healthcare/monte_carlo_test" ]; then
    echo "Running Monte Carlo simulation tests..."
    ./test/healthcare/monte_carlo_test
else
    echo "WARNING: monte_carlo_test not found. You may need to implement this test."
fi

if [ -f "./test/healthcare/enhanced_radiation_therapy_test" ]; then
    echo "Running enhanced radiation therapy model tests..."
    ./test/healthcare/enhanced_radiation_therapy_test
else
    echo "WARNING: enhanced_radiation_therapy_test not found. You may need to implement this test."
fi

# Run examples
echo -e "\nRunning healthcare examples..."

if [ -f "./bin/enhanced_healthcare_example" ]; then
    echo "Running enhanced healthcare example..."
    ./bin/enhanced_healthcare_example
else
    echo "WARNING: enhanced_healthcare_example not found."
fi

echo -e "\n============================================="
echo "Healthcare Module Validation Summary:"
echo "============================================="

# Print test results
echo "Tests executed:"
ls -la test/healthcare/ | grep -E 'test$' || echo "No test executables found."

echo -e "\nExamples executed:"
ls -la bin/ | grep -E 'healthcare|cell_cycle|monte_carlo|parameter|dicom' || echo "No example executables found."

echo -e "\nValidation complete!"

# Provide guidance for missing components
echo -e "\nRecommendations:"
echo "1. For any missing tests, check the implementation directories"
echo "2. Implement missing source files in src/rad_ml/healthcare/"
echo "3. Make sure all forward declarations are properly handled"
echo "4. Add unit tests for components with warnings above"

cd ..
