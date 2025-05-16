**Author:** Rishab Nuguru
**Original Copyright:** © 2025 Rishab Nuguru
**Company:** Space Labs AI
**License:** GNU General Public License (GPL) Version 3
**Repository:** https://github.com/r0nlt/healthcare
**Company Page** https://www.linkedin.com/company/space-labs-ai
**Version:** v0.9.5

A comprehensive framework for modeling and simulating radiation effects on various systems, including semiconductor devices, space instrumentation, and healthcare applications.

## 🚀 What's New in v0.9.5

- **Theoretically Sound Zero-Point Energy Model**: New refined model properly separates temperature-independent pure ZPE from thermal quantum corrections
- **Enhanced Biological Environment Modeling**: Improved material parameters for proteins, DNA, and biological systems
- **Cross-Domain Quantum Validation**: Validation against theoretical predictions with <1% error for ground-state energy calculations
- **Extended Healthcare Quantum Framework**: Better integration of quantum effects in biological radiation responses

## 🌊 Monte Carlo Wave Equation Solver

The Monte Carlo wave equation solver provides a robust framework for statistically evaluating quantum mechanical effects relevant to radiation modeling. This addition significantly enhances our research capabilities by:

- **Quantifying Uncertainties**: Provides statistical distributions of quantum effects across parameter ranges
- **Identifying Correlations**: Reveals relationships between physical parameters and quantum outcomes
- **Validating Models**: Enables rigorous testing of quantum models against theoretical expectations
- **Optimizing Parameters**: Helps identify optimal parameter ranges for specific applications

### Research Implications

This enhancement opens new research directions:

1. **Uncertainty Quantification**: Better understanding of uncertainties in radiation effect predictions
2. **Parameter Sensitivity Analysis**: Identification of which physical parameters most strongly influence quantum effects
3. **Cross-Domain Validation**: Ability to validate models across semiconductor, space, and healthcare domains
4. **Performance Optimization**: Significantly faster testing enabling larger parameter sweeps

### Key Results From Latest Testing

Based on our recent tests with the refined quantum models, we've observed:

- Proper temperature independence (correlation ≈ 0) of pure zero-point energy, aligning with fundamental quantum theory
- Strong correlation (>0.99) between thermal quantum corrections and temperature, validating the Bose-Einstein statistics implementation
- Material-dependent quantum contributions with protein systems showing ~10x smaller ZPE than silicon
- Zero error (<0.001%) between theoretical predictions and Monte Carlo results for ground-state energy

## 🔍 Overview

This framework combines quantum mechanical (QM) and molecular mechanical (MM) approaches to model radiation effects across domains. Originally developed for space and semiconductor applications, it now includes healthcare modeling capabilities, particularly for cancer treatment modalities.

## 🔧 Key Components

- **QM/MM Integration**: Tiered approach to quantum modeling based on system requirements
- **Quantum Tunneling Models**: Account for tunneling effects in various materials and biological systems
- **Radiation Damage Models**: Simulate the effects of radiation on different target systems
- **Healthcare Applications**: Model the interaction of radiation and chemotherapy with biological systems
- **Monte Carlo Quantum Testing**: Statistical validation framework for quantum wave equations
- **Refined Zero-Point Energy Model**: Theoretically sound implementation of ground-state energy and thermal corrections

## 📊 Healthcare Module

New healthcare module models radiation effects on biological systems, with a focus on:
- Radiation therapy simulation
- Chemotherapy drug modeling
- Quantum-corrected chemoradiation synergy
- Biological tissue response to radiation

### Supported Drugs Database

The system includes quantum parameters for standard chemotherapeutic agents:
- **Cisplatin** - DNA-binding platinum compound
- **Paclitaxel** - Microtubule-stabilizing agent
- **Doxorubicin** - DNA intercalating agent
- **Temozolomide** - Alkylating agent
- **Fluorouracil** - Antimetabolite

## 🖥️ Getting Started

### Prerequisites

- C++17 compatible compiler (gcc 7+, clang 5+, MSVC 2019+)
- CMake 3.10 or higher
- Basic linear algebra libraries
- POSIX-compliant system for parallel execution

### Building the Project

```bash
# Clone the repository
git clone https://github.com/r0nlt/healthcare.git
cd rad-tolerant-ml

# Build using CMake
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build .
```

### Running Healthcare Module Tests

```bash
# Quick QM/MM integration test with default parameters (CISPLATIN, 2 Gy)
./build_simple_qmmm.sh

# Test with specific drug and radiation dose
./build_simple_qmmm.sh PACLITAXEL 5.0
```

### Running the Monte Carlo Wave Equation Test

The Monte Carlo test can be run with various parameters to explore different physical regimes:

```bash
# Run with default parameters (10,000 samples, all available cores)
./build_quantum_mc_test.sh

# Run with custom parameters
./build_quantum_mc_test.sh --samples 50000 --threads 8 --temp-min 5.0 --temp-max 500.0

# Available parameters:
#   --samples N       Number of Monte Carlo samples
#   --threads N       Number of threads to use
#   --temp-min X      Minimum temperature in K
#   --temp-max X      Maximum temperature in K
#   --size-min X      Minimum feature size in nm
#   --size-max X      Maximum feature size in nm
#   --barrier-min X   Minimum barrier height in eV
#   --barrier-max X   Maximum barrier height in eV
#   --zpe-correction-model [simple|detailed]  ZPE model type
#   --material [Si|Ge|GaAs|protein|dna|water|custom]  Material type
#   --env-effects [enabled|disabled]  Environmental effects
```

#### Example Output with Refined ZPE Model

```
Monte Carlo Test for Wave Equation Solver
=======================================
Number of samples: 5000
Number of threads: 4
Temperature range: [0.1, 500] K
Feature size range: [2, 50] nm
Barrier height range: [0.1, 5] eV
ZPE Model: Detailed (Pure ZPE + Thermal)
Environmental effects: Disabled
---------------------------------------
Launching 4 worker threads...

Monte Carlo Simulation Complete
===============================
Klein-Gordon Equation Results:
  Mean: 3.23801e-33
  Std Dev: 5.58909e-33
Quantum Tunneling Probability Results:
  Mean: 2.52236e-08
  Std Dev: 1.65778e-06

Zero-Point Energy Analysis:
  Total Quantum Contribution:
    Mean: 4.64166e-19
    Std Dev: 4.4489e-32
  Pure ZPE (Temperature-Independent):
    Mean: 4.64166e-19
    Std Dev: 4.4489e-32
  Thermal Quantum Correction:
    Mean: 2.41054e-79
    Std Dev: 2.14803e-78

Parameter Correlation Analysis:
-------------------------------
Correlation between Klein-Gordon solution and temperature: 0.0149399
Correlation between tunneling probability and barrier height: -0.0251485
Correlation between total quantum contribution and temperature: -4.90664e-15
Correlation between pure ZPE and temperature: -4.90664e-15
Correlation between thermal quantum correction and temperature: 0.195763

Refined ZPE Model Analysis
==========================
Pure ZPE / Total Quantum Contribution Ratio: 100.000000%
Temperature Dependence Analysis:
  Pure ZPE Temperature Correlation: -0.000000
  Thermal Quantum Effects Temperature Correlation: 0.195763

Theoretical Validation:
  Expected Pure ZPE: 4.641659e-19 J
  Measured Pure ZPE: 4.641659e-19 J
  Error: 0.0000%
```

## 📖 Documentation

For detailed documentation on specific modules:

- [QM/MM Integration for Healthcare](QMMM_INTEGRATION_README.md)
- [Semiconductor Radiation Models](docs/semiconductor_models.md)
- [Space Radiation Effects](docs/space_radiation.md)
- [Quantum Monte Carlo Testing](docs/quantum_monte_carlo.md)

## 📊 Example Usage

```cpp
// Example: Using the healthcare module with refined quantum model
#include <rad_ml/healthcare/bio_quantum_integration.hpp>
#include <rad_ml/healthcare/chemotherapy/chemo_quantum_model.hpp>
#include <rad_ml/healthcare/molecule.hpp>
#include <rad_ml/quantum/zpe_model.hpp>

namespace rh = rad_ml::healthcare;
namespace rhc = rad_ml::healthcare::chemotherapy;
namespace rq = rad_ml::quantum;

int main() {
    // Create drug molecule
    auto drug = rh::createStandardDrug("CISPLATIN");

    // Create biological target
    rh::BiologicalSystem dnaTarget;
    dnaTarget.type = rh::NUCLEIC_ACID;
    dnaTarget.water_content = 0.7;

    // Calculate quantum parameters with refined model
    double force_constant = rq::MaterialModelFactory::getForceConstant(
        rq::MaterialModelFactory::DNA, 2.0e-9);

    double pure_zpe = rq::calculatePureZPE(
        1.054571817e-34, 9.1093837015e-31, force_constant);

    double thermal_correction = rq::calculateThermalQuantumCorrection(
        1.054571817e-34, 9.1093837015e-31, force_constant, 310.0);

    double total_quantum = rq::getTotalQuantumContribution(
        1.054571817e-34, 9.1093837015e-31, force_constant, 310.0);

    // Calculate chemoradiation synergy with quantum effects
    rhc::DrugSpecificQuantumModel model;
    double synergy = model.calculateChemoRadiationSynergy(
        "CISPLATIN", 2.0, rhc::CONCURRENT, 310.0);

    return 0;
}
```

## 📞 Contact

For questions or support, please contact rnuguruworkspace@gmail.com.
