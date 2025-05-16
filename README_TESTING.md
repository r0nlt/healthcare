# Rad-Tolerant ML Healthcare Module Testing Guide

This guide provides instructions for testing and validating the radiation-tolerant machine learning healthcare module, which includes quantum-enhanced modeling for biological systems.

## Overview

The healthcare module consists of several components:

1. **Quantum Enhanced Components**
   - Wave Equation Solver - Quantum wave equation solver for biological systems
   - Tunneling Model - Quantum tunneling modeling through biological barriers

2. **Cell Biology Components**
   - Cell Cycle Model - Models cell cycle-dependent radiation response
   - DNA Damage Model - Models DNA damage from radiation with track structure
   - Repair Kinetics - Models sophisticated repair kinetics

3. **Monte Carlo Components**
   - Damage Simulation - Monte Carlo simulation of radiation damage

4. **Integration Components**
   - DICOM Integration - Integration with medical imaging
   - Enhanced Radiation Therapy Model - Comprehensive model that integrates all components

## Prerequisites

- CMake 3.14 or higher
- C++14 compatible compiler
- Basic understanding of radiobiology principles

## Quick Testing Guide

We've created a validation script to quickly test the entire healthcare module:

```bash
chmod +x validate_healthcare_module.sh
./validate_healthcare_module.sh
```

This script will:
1. Configure CMake with the necessary options
2. Build the library and tests
3. Run all available healthcare tests
4. Run the healthcare examples
5. Provide a summary of the test results

## Manual Testing Steps

If you prefer to test components manually, follow these steps:

### 1. Configure and Build

```bash
mkdir -p build
cd build
cmake .. -DBUILD_HEALTHCARE=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
make -j4
```

### 2. Run Unit Tests

Run the quantum validation tests:
```bash
./test/healthcare/quantum_validation_test
```

Run DNA damage tests:
```bash
./test/healthcare/dna_damage_test
```

Run cell cycle tests:
```bash
./test/healthcare/cell_cycle_test
```

If implemented, run Monte Carlo simulation tests:
```bash
./test/healthcare/monte_carlo_test
```

If implemented, run enhanced radiation therapy model tests:
```bash
./test/healthcare/enhanced_radiation_therapy_test
```

### 3. Run Examples

Run the enhanced healthcare example:
```bash
./bin/enhanced_healthcare_example
```

## Adding New Tests

To add new tests for the healthcare module:

1. Create a new test file in `test/healthcare/` directory
2. Add the test to `test/healthcare/CMakeLists.txt`
3. Build and run the test as described above

Example for adding a test:

```cmake
# Add to test/healthcare/CMakeLists.txt
add_executable(my_new_test
    my_new_test.cpp
)
target_link_libraries(my_new_test
    rad_tolerant_ml_healthcare
)
add_test(NAME my_new_test
         COMMAND my_new_test)
```

## Validating Correctness

To validate the correctness of the healthcare module:

1. Check that all unit tests pass
2. Verify that the examples produce reasonable results
3. Compare results against published radiobiological data
4. For quantum effects, verify that:
   - Tunneling probability increases with temperature
   - Tunneling probability decreases with barrier height
   - Wave equation solutions conserve probability

## Troubleshooting

If you encounter issues during testing:

1. **Build failures**: Check that all source files exist in the `src/rad_ml/healthcare/` directory
2. **Linker errors**: Ensure implementation files match the declared functions in headers
3. **Forward declaration errors**: Ensure proper forward declarations for any incomplete types
4. **Test failures**: Review the test output to identify specific failures

## Implementation Status

The following components must be implemented in `src/rad_ml/healthcare/` for tests to pass:

- Quantum Enhanced:
  - `wave_equation_solver.cpp`
  - `tunneling_model.cpp`
  - `parameter_optimizer.cpp`

- Cell Biology:
  - `cell_cycle_model.cpp`
  - `dna_damage_model.cpp`
  - `repair_kinetics.cpp`

- Monte Carlo:
  - `damage_simulation.cpp`

- Imaging:
  - `dicom_integration.cpp`

- Integration:
  - `enhanced_radiation_therapy_model.cpp`

## Contact

If you have questions about testing the healthcare module, please contact the development team.
