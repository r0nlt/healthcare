# Quantum Monte Carlo Wave Equation Testing

This document provides detailed information about the Monte Carlo testing framework for quantum wave equations implemented in the rad-tolerant-ml project.

## Overview

The Monte Carlo wave equation solver employs statistical sampling to evaluate the behavior of quantum mechanical equations across a range of physical parameters. By running thousands of simulations with randomized input parameters, it provides insights into:

1. Statistical distributions of quantum effects
2. Correlations between physical parameters and quantum outcomes
3. Sensitivity of quantum models to parameter variations
4. Performance characteristics of the quantum solvers

## Physical Theory

The framework currently tests three key quantum mechanical elements:

### 1. Klein-Gordon Equation

The Klein-Gordon equation is a relativistic wave equation that describes spinless particles. In our framework, it is used to model quantum field effects in materials exposed to radiation. The equation in its simplest form is:

```
(∂²/∂t² - ∇² + m²)φ(x,t) = 0
```

where:
- φ(x,t) is the quantum field
- m is the mass parameter
- ∇² is the Laplacian operator

Our implementation uses a simplified solver that accounts for:
- Mass/energy relationship
- Coupling constants
- Potential energy coefficients
- Lattice spacing effects

### 2. Quantum Tunneling

Quantum tunneling is a phenomenon where particles penetrate energy barriers that would be insurmountable in classical physics. The tunneling probability is calculated using the WKB approximation:

```
P ≈ exp(-2∫√(2m(V(x)-E))/ħ dx)
```

where:
- V(x) is the potential barrier
- E is the particle energy
- m is the particle mass
- ħ is the reduced Planck constant

Our implementation includes:
- Temperature dependence via thermal energy distribution
- Barrier height and width effects
- Mass-dependent tunneling rates

### 3. Zero-Point Energy

Zero-point energy (ZPE) is the lowest possible energy that a quantum mechanical system can have. For a simple harmonic oscillator:

```
E₀ = (1/2)ħω
```

where:
- ω is the angular frequency
- ħ is the reduced Planck constant

Our implementation models:
- Temperature dependence of ZPE effects
- Lattice constant influence
- Material-specific contributions

## Implementation Details

### Parallel Processing Architecture

The Monte Carlo framework employs a thread-based parallelization approach:

1. The simulation space is divided into equal-sized batches
2. Each batch is assigned to a separate worker thread
3. Worker threads maintain thread-local storage to minimize lock contention
4. Results are aggregated using mutex-protected operations
5. Final statistical analysis is performed on the aggregated results

This architecture enables near-linear scaling with the number of available cores.

### Random Parameter Generation

Parameters are generated using the Mersenne Twister engine (std::mt19937) with:

- Uniformly distributed sampling across parameter ranges
- Unique seed generation per thread using high-resolution clock
- Thread-safe random number generation

### Statistical Analysis

The framework performs:

- Mean and standard deviation calculations for each quantum measure
- Pearson correlation coefficient computation between parameters and results
- Performance metrics (samples per second)

## Using the Framework for Research

### Parameter Space Exploration

The framework can be used to explore how quantum effects vary across different parameter spaces by adjusting:

```bash
--temp-min X      # Minimum temperature in K
--temp-max X      # Maximum temperature in K
--size-min X      # Minimum feature size in nm
--size-max X      # Maximum feature size in nm
--barrier-min X   # Minimum barrier height in eV
--barrier-max X   # Maximum barrier height in eV
```

Example research questions:
- How does tunneling probability change in nano-scale vs. micro-scale features?
- What is the effect of cryogenic vs. room temperature on quantum corrections?
- At what barrier heights do tunneling effects become negligible?

### Scaling Tests

The framework can evaluate computational performance and scaling behavior:

```bash
--samples N       # Number of Monte Carlo samples
--threads N       # Number of threads to use
```

This can help optimize computational resources for larger studies and understand the computational complexity of the quantum models.

### Validation Studies

By comparing Monte Carlo results against known theoretical predictions, the framework can validate the implementation of quantum models:

1. Run with parameters matching analytical solutions
2. Compare statistical distributions with theoretical expectations
3. Verify correlation coefficients against theoretical models

## Extending the Framework

### Adding New Quantum Equations

To add a new quantum equation to the testing framework:

1. Implement the solver function in the appropriate namespace
2. Add result storage to the `MCSimulationResults` structure
3. Update the `runMCBatch` function to calculate and store the new results
4. Add statistical analysis for the new measure in `calculateStatistics`
5. Implement correlation analysis in `analyzeParameterCorrelations`

### Custom Parameter Distributions

The current implementation uses uniform distributions for parameter sampling. To implement more sophisticated sampling:

1. Modify the `generateRandomParameters` function
2. Implement custom distribution generators (e.g., Gaussian, exponential)
3. Add new command-line parameters to control distribution characteristics

### Extended Analysis

The framework can be extended with more sophisticated analysis:

1. Multi-dimensional correlation analysis
2. Polynomial regression to identify non-linear relationships
3. Visualization capabilities for parameter spaces
4. Sensitivity analysis to identify critical parameters

## Performance Considerations

- **Memory Usage**: For very large sample sizes (>10⁶), consider monitoring memory usage
- **Thread Count**: Performance typically scales well up to the number of physical cores
- **Parameter Ranges**: Extremely wide parameter ranges may need more samples for statistical significance
- **Simulation Time**: Most runs complete in seconds, but complex extensions may increase runtime

## Troubleshooting

Common issues:

1. **Build Failures**: Ensure C++17 support is enabled and threading library is available
2. **Runtime Crashes**: Check for potential race conditions in custom extensions
3. **Statistical Anomalies**: Verify parameter distributions and increase sample size for better statistics
4. **Performance Issues**: Monitor thread count and system resources during execution

## Future Directions

Planned enhancements to the framework include:

1. GPU acceleration for massively parallel Monte Carlo simulations
2. Integration with machine learning for parameter optimization
3. Support for more complex quantum models (Dirac equation, quantum field theory)
4. Enhanced visualization and reporting capabilities
5. Integration with experimental validation datasets
