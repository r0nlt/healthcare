#!/bin/bash

# Run all tests and examples for the rad-tolerant-ml project
echo "==============================================="
echo "Running tests for rad-tolerant-ml"
echo "==============================================="

# Create results directory if it doesn't exist
mkdir -p results

# Function to run a test and check its exit code
run_test() {
    local test_name=$1
    local test_cmd=$2

    echo -n "Running $test_name... "
    $test_cmd > "results/${test_name}_output.txt" 2>&1
    local exit_code=$?

    if [ $exit_code -eq 0 ]; then
        echo "PASSED"
        return 0
    else
        echo "FAILED (exit code: $exit_code)"
        return 1
    fi
}

# Track overall status
overall_status=0

# Run the healthcare test
run_test "healthcare_test" "./bin/healthcare_test"
healthcare_status=$?
overall_status=$((overall_status + healthcare_status))

# Run the QFT bridge test
run_test "qft_bridge_test" "./bin/qft_bridge_test"
qft_bridge_status=$?
overall_status=$((overall_status + qft_bridge_status))

# Run cross-domain simplified example
run_test "crossdomain_simplified" "./bin/crossdomain_simplified"
crossdomain_status=$?
overall_status=$((overall_status + crossdomain_status))

# Run QFT bridge analysis
run_test "qft_bridge_analysis" "./bin/qft_bridge_analysis"
bridge_analysis_status=$?
overall_status=$((overall_status + bridge_analysis_status))

# Run QFT enhanced network example
run_test "qft_enhanced_network" "./bin/qft_enhanced_network_example"
network_status=$?
overall_status=$((overall_status + network_status))

# Print summary
echo ""
echo "==============================================="
echo "Test Summary"
echo "==============================================="
echo "Healthcare Test:          $([[ $healthcare_status -eq 0 ]] && echo "PASSED" || echo "FAILED")"
echo "QFT Bridge Test:          $([[ $qft_bridge_status -eq 0 ]] && echo "PASSED" || echo "FAILED")"
echo "Cross-domain Simplified:  $([[ $crossdomain_status -eq 0 ]] && echo "PASSED" || echo "FAILED")"
echo "QFT Bridge Analysis:      $([[ $bridge_analysis_status -eq 0 ]] && echo "PASSED" || echo "FAILED")"
echo "QFT Enhanced Network:     $([[ $network_status -eq 0 ]] && echo "PASSED" || echo "FAILED")"
echo ""

if [ $overall_status -eq 0 ]; then
    echo "All tests PASSED"
    exit 0
else
    echo "$overall_status tests FAILED"
    exit 1
fi
