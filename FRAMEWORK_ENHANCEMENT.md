# Radiation-Tolerant ML Framework Enhancements

This document describes the enhancements made to the rad-tolerant-ml framework, expanding its capabilities for modeling quantum effects in biological systems under radiation exposure.

## Enhanced Testing Framework

The enhancement introduces a comprehensive testing framework that builds upon the simplified tests to include:

1. **Advanced Quantum Effects Testing** - Tests quantum tunneling in biological membranes with temperature and water content variations

2. **Cross-Domain Bridge Testing** - Validates the connection between semiconductor physics and biological systems

3. **Chemoradiation Synergy Testing** - Tests the synergistic effects between radiation and various chemotherapy agents

4. **Clinical Validation Testing** - Compares model predictions with published clinical data

5. **Therapeutic Ratio Optimization** - Tests optimization of treatment plans accounting for quantum effects

## New Components

The enhancement adds several new components to the framework:

- **Bio-Quantum Integration** - Bridges quantum physics and cellular biology
- **Radiation Therapy Model** - Models radiation therapy effects with quantum corrections
- **Chemo-Quantum Model** - Models chemotherapy effects with quantum effects
- **Chemoradiation Synergy** - Models combined effects of radiation and chemotherapy
- **QFT Bridge** - Connects quantum field theory to biological applications

## Using the Enhanced Framework

### Building and Running Tests

```bash
# Make the test script executable
chmod +x run_framework_tests.sh

# Run the tests
./run_framework_tests.sh
```

### Adding the Full Framework Test

To include the full framework test in your build:

1. Move `full_framework_test.cpp` to the `test/healthcare/` directory
2. Build the project with CMake

```bash
mkdir -p build
cd build
cmake .. -DBUILD_HEALTHCARE=ON -DBUILD_TESTS=ON
make -j$(nproc)
```

## Next Steps

1. **Implement Visualization Functions** - Develop the visualization functions to generate plots of dose-response curves, quantum effects, and chemoradiation synergy

2. **Complete Component Implementation** - Complete the implementation of cross-domain bridge and chemoradiation synergy components

3. **Clinical Data Integration** - Integrate real clinical data for validation testing

4. **Performance Optimization** - Optimize the quantum simulation components for performance

## Contributing

Contributions to extend these enhancements are welcome. Please ensure all changes maintain compatibility with the existing framework and pass all tests.
