#!/bin/bash

# Build script for QM/MM integration test
set -e  # Exit on error

echo "Building QM/MM Integration for Healthcare Module..."

# First, build from the main directory
cmake -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_HEALTHCARE=ON \
      -DBUILD_TESTS=OFF \
      -DBUILD_EXAMPLES=OFF \
      .

# Build the project
cmake --build . -- -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

echo "Build completed successfully."

# Check if the test executable was built
if [ -f "src/healthcare/qmmm_integration_test" ]; then
    echo "Running QM/MM integration test..."

    # Default drug and radiation dose
    DRUG=${1:-"CISPLATIN"}
    DOSE=${2:-"2.0"}

    # Run the test
    ./src/healthcare/qmmm_integration_test "$DRUG" "$DOSE"

    echo "Test completed."
else
    echo "Test executable not found at src/healthcare/qmmm_integration_test"
    echo "Searching for the executable..."
    find . -name "qmmm_integration_test" -type f -executable
    echo "Build may have completed, but the test executable location is different than expected."
fi
