**Author:** Rishab Nuguru
**Original Copyright:** © 2025 Rishab Nuguru
**Company:** Space Labs AI
**License:** GNU General Public License (GPL) Version 3
**Repository:** https://github.com/r0nlt/healthcare
**Company Page** https://www.linkedin.com/company/space-labs-ai
**Version:** v0.9.3

A comprehensive framework for modeling and simulating radiation effects on various systems, including semiconductor devices, space instrumentation, and healthcare applications.

## 🚀 What's New in v0.9.3

- **Healthcare Module Integration**: Newly implemented QM/MM integration for radiation-based healthcare applications
- **Chemoradiation Synergy Modeling**: Advanced models to predict combined effects of chemotherapy and radiation therapy
- **Drug-Specific Quantum Corrections**: Specialized handling for different chemotherapeutic agents
- **Temperature-Dependent Quantum Effects**: Enhanced modeling of quantum tunneling with temperature dependency

## 🔍 Overview

This framework combines quantum mechanical (QM) and molecular mechanical (MM) approaches to model radiation effects across domains. Originally developed for space and semiconductor applications, it now includes healthcare modeling capabilities, particularly for cancer treatment modalities.

## 🔧 Key Components

- **QM/MM Integration**: Tiered approach to quantum modeling based on system requirements
- **Quantum Tunneling Models**: Account for tunneling effects in various materials and biological systems
- **Radiation Damage Models**: Simulate the effects of radiation on different target systems
- **Healthcare Applications**: Model the interaction of radiation and chemotherapy with biological systems

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

- C++14 compatible compiler (gcc 5+, clang 3.4+, MSVC 2017+)
- CMake 3.14 or higher
- Basic linear algebra libraries

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

## 📖 Documentation

For detailed documentation on specific modules:

- [QM/MM Integration for Healthcare](QMMM_INTEGRATION_README.md)
- [Semiconductor Radiation Models](docs/semiconductor_models.md)
- [Space Radiation Effects](docs/space_radiation.md)

## 📊 Example Usage

```cpp
// Example: Using the healthcare module to simulate chemoradiation
#include <rad_ml/healthcare/bio_quantum_integration.hpp>
#include <rad_ml/healthcare/chemotherapy/chemo_quantum_model.hpp>
#include <rad_ml/healthcare/molecule.hpp>

namespace rh = rad_ml::healthcare;
namespace rhc = rad_ml::healthcare::chemotherapy;

int main() {
    // Create drug molecule
    auto drug = rh::createStandardDrug("CISPLATIN");

    // Create biological target
    rh::BiologicalSystem dnaTarget;
    dnaTarget.type = rh::NUCLEIC_ACID;
    dnaTarget.water_content = 0.7;

    // Calculate quantum-enhanced binding
    rhc::DrugSpecificQuantumModel model;
    double binding = model.calculateQMEnhancedBinding("CISPLATIN", 310.0, true);

    // Calculate chemoradiation synergy
    double synergy = model.calculateChemoRadiationSynergy(
        "CISPLATIN", 2.0, rhc::CONCURRENT, 310.0);

    return 0;
}
```

## 📞 Contact

For questions or support, please contact rnuguruworkspace@gmail.com.
