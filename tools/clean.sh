#!/bin/bash

echo "=== Radiation Tolerant ML Cleanup Script ==="
echo "This script will clean up build artifacts and executables."

# Clean up build directory
if [ -d "build" ]; then
  echo "Removing build directory..."
  rm -rf build
fi

# Clean up executables
echo "Removing executable files..."
executables=(
  "monte_carlo_validation"
  "space_monte_carlo_validation"
  "realistic_space_validation"
  "framework_verification_test"
  "enhanced_tmr_test"
  "scientific_validation_test"
  "radiation_stress_test"
  "systematic_fault_test"
  "modern_features_test"
  "quantum_field_validation_test"
  "neural_network_validation"
  "monte_carlo_neuralnetwork"
  "quantum_field_example"
  "architecture_test"
  "residual_network_test"
  "simple_nn"
  "mission_simulator"
  "nasa_esa_standard_test"
)

for exe in "${executables[@]}"; do
  if [ -f "$exe" ]; then
    rm -f "$exe"
    echo "Removed $exe"
  fi
done

# Remove any generated CSV files (if they exist)
find . -name "*_results.csv" -not -path "./test/*" -type f -delete

echo "Clean completed successfully!"
echo "To rebuild, use: ./tools/build_all.sh" 