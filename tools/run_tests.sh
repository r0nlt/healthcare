#!/bin/bash
set -e  # Exit on any error

echo "=== Radiation Tolerant ML Test Runner ==="

# Function to run a single test
run_test() {
  local test_name="$1"
  echo "Running test: $test_name"
  if [ -f "$test_name" ]; then
    ./$test_name
    echo "✓ Test $test_name completed successfully"
  else
    echo "✗ Test executable not found: $test_name"
    echo "Make sure you've built the project with ./tools/build_all.sh first"
  fi
  echo ""
}

# Function to run all tests
run_all_tests() {
  echo "Running all tests..."
  
  # These are the test executables we saw in the CMakeLists.txt
  run_test monte_carlo_validation
  run_test space_monte_carlo_validation
  run_test realistic_space_validation
  run_test framework_verification_test
  run_test enhanced_tmr_test
  run_test scientific_validation_test
  run_test radiation_stress_test
  run_test systematic_fault_test
  run_test modern_features_test
  run_test quantum_field_validation_test
  run_test neural_network_validation
  run_test monte_carlo_neuralnetwork
  
  echo "All tests completed!"
}

# Function to run a specific example
run_example() {
  local example_name="$1"
  echo "Running example: $example_name"
  if [ -f "$example_name" ]; then
    ./$example_name
    echo "✓ Example $example_name completed successfully"
  else
    echo "✗ Example executable not found: $example_name"
    echo "Make sure you've built the project with ./tools/build_all.sh first"
  fi
  echo ""
}

# Function to run all examples
run_all_examples() {
  echo "Running all examples..."
  
  # Examples from the examples directory
  run_example quantum_field_example
  run_example architecture_test
  run_example residual_network_test
  run_example simple_nn
  run_example mission_simulator
  
  echo "All examples completed!"
}

# Handle command-line arguments
if [ "$1" == "all" ] || [ -z "$1" ]; then
  run_all_tests
  run_all_examples
elif [ "$1" == "tests" ]; then
  run_all_tests
elif [ "$1" == "examples" ]; then
  run_all_examples
else
  # Run a specific test or example
  run_test "$1" || run_example "$1" || echo "Unknown test or example: $1"
fi 