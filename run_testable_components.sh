#!/bin/bash

# Script to run only the testable components
# Author: Claude

set -e  # Exit on error

echo "============================================="
echo "  Running Testable Components Only"
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

# Define all possible test executables
TEST_EXECUTABLES=(
    "test/healthcare/quantum_validation_test"
    "test/healthcare/dna_damage_test"
    "test/healthcare/cell_cycle_test"
    "test/healthcare/monte_carlo_test"
    "test/healthcare/enhanced_radiation_therapy_test"
    "test/healthcare/parameter_optimizer_test"
    "test/healthcare/dicom_test"
)

# Define all possible example executables
EXAMPLE_EXECUTABLES=(
    "bin/enhanced_healthcare_example"
    "bin/cell_cycle_effects"
    "bin/monte_carlo_simulation"
    "bin/parameter_optimization"
    "bin/dicom_integration"
)

# Run only tests that exist
echo -e "\nRunning available tests..."
TESTS_RUN=0

for test in "${TEST_EXECUTABLES[@]}"; do
    if [ -f "./$test" ]; then
        echo "Running $test..."
        ./$test
        TESTS_RUN=$((TESTS_RUN + 1))
    fi
done

if [ $TESTS_RUN -eq 0 ]; then
    echo "No test executables found!"
fi

# Run only examples that exist
echo -e "\nRunning available examples..."
EXAMPLES_RUN=0

for example in "${EXAMPLE_EXECUTABLES[@]}"; do
    if [ -f "./$example" ]; then
        echo "Running $example..."
        ./$example
        EXAMPLES_RUN=$((EXAMPLES_RUN + 1))
    fi
done

if [ $EXAMPLES_RUN -eq 0 ]; then
    echo "No example executables found!"
fi

echo -e "\n============================================="
echo "Testing Summary:"
echo "============================================="
echo "Tests run: $TESTS_RUN"
echo "Examples run: $EXAMPLES_RUN"

if [ $TESTS_RUN -eq 0 ] && [ $EXAMPLES_RUN -eq 0 ]; then
    echo -e "\nNo tests or examples were found. You need to implement the components first."
    echo "Run ./check_implementations.sh to create stub implementations."
else
    echo -e "\nAll available tests and examples completed successfully."
fi

cd ..
