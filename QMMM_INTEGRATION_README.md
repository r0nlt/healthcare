# QM/MM Integration for Radiation-Based Healthcare v0.9.2

This module integrates Quantum Mechanics and Molecular Mechanics (QM/MM) approaches for modeling radiation effects in biological systems, with a focus on healthcare applications like chemotherapy and radiation therapy.

## Overview

The QM/MM integration is based on the implementation strategies from Arodola & Soliman (2017). It provides sophisticated models for:

1. **Tiered QM/MM Partitioning** - Applies appropriate QM methods based on system size and required accuracy
2. **Temperature-dependent Quantum Effects** - Models quantum tunneling with enhanced temperature dependency
3. **Drug-specific Quantum Modeling** - Specialized handling for chemoradiation synergy
4. **Integrated Chemoradiation Synergy** - Models combined treatment effects with quantum corrections

## Key Components

- `TieredQMMMPartitioner` - Intelligently partitions systems into QM and MM regions
- `EnhancedQuantumTunnelingModel` - Calculates quantum effects with temperature dependence
- `DrugSpecificQuantumModel` - Provides drug-specific quantum corrections
- `QMIntegratedWorkflow` - Manages the workflow from screening to detailed analysis
- `ChemoradiationSynergyModel` - Calculates synergistic effects of combined treatments

## Building and Running

A simple build script is provided:

```bash
./build_simple_qmmm.sh [DRUG_NAME] [RADIATION_DOSE]
```

Where:
- `DRUG_NAME` is one of: CISPLATIN, PACLITAXEL, DOXORUBICIN, FLUOROURACIL (default: CISPLATIN)
- `RADIATION_DOSE` is the radiation dose in Gy (default: 2.0)

## Key Features

### Tiered QM Approach

The system automatically selects the appropriate QM method based on region size:
- **COUPLED_CLUSTER** - For regions up to 20 atoms (high accuracy)
- **DENSITY_FUNCTIONAL** - For regions up to 500 atoms (medium accuracy)
- **SEMIEMPIRICAL** - For regions up to 2000 atoms (low accuracy)

### Special Metal Handling

Metal-containing drugs (like cisplatin) receive special treatment:
- Larger QM regions (6.0Å cutoff vs. 5.0Å standard)
- Preference for higher-quality QM methods
- Enhanced quantum effects (15% additional enhancement)

### Temperature-Dependent Quantum Effects

Quantum effects are modeled with temperature dependence:
- Biological systems: 3-5% enhancement
- Semiconductor systems: 8-12% enhancement
- Temperature adjustments based on physiological relevance

### Staged Computational Approach

The workflow follows a staged approach:
1. **Preprocessing** - Fast MM-based screening
2. **Refinement** - Mixed MM/QM for promising candidates
3. **Final Analysis** - Full QM/MM for lead compounds

### Chemoradiation Synergy Modeling

The system models combined chemoradiation effects:
- Three treatment schedules: concurrent, radiation-first, or drug-first
- Linear-quadratic model with drug-specific adjustments
- Tissue-specific quantum correction factors
- Treatment timing optimization

## Example Output

```
QM/MM Integration for Radiation-Based Healthcare Quantum Modeling
================================================================

Creating drug molecule: CISPLATIN
Drug properties:
- Name: Cisplatin
- Molecular weight: 300.01 Da
- LogP: -2.19
- H-bond donors: 0
- H-bond acceptors: 4
- Contains metals: Yes
- Atom count: 9

Running QM/MM integrated workflow
--------------------------------
Performing initial MM-based screening for drug: Cisplatin
Initial screening result: PASSED
Advancing from stage 0 to stage 1
Performing QM/MM refinement analysis for drug: Cisplatin
QM Enhancement factor: 1.04
Refinement analysis result: PASSED
Advancing from stage 1 to stage 2
Performing final QM/MM analysis for drug: Cisplatin
Using specialized metal-system QM region definition
QM region contains 9 atoms with method: COUPLED_CLUSTER
Final QM Enhancement factor: 1.04

Computing drug-specific quantum effects
-------------------------------------
Binding affinity:
- With quantum effects: 0.908362
- Without quantum effects: 0.7595
- Quantum enhancement: 0.196 (factor)

Chemoradiation synergy (dose: 2 Gy):
- Concurrent treatment: 1.57976
- Radiation first: 0.248248
- Drug first: 0.22568

Optimal treatment schedule: CONCURRENT
```

## Code Structure

- `include/rad_ml/healthcare/bio_quantum_integration.hpp` - Core QM/MM classes
- `include/rad_ml/healthcare/chemotherapy/chemo_quantum_model.hpp` - Drug-specific models
- `include/rad_ml/healthcare/chemotherapy/chemoradiation_synergy.hpp` - Synergy modeling
- `include/rad_ml/healthcare/molecule.hpp` - Molecular structure representation
- `include/rad_ml/healthcare/qmmm_simulation.hpp` - Simulation components
- `src/healthcare/qmmm_integration_test.cpp` - Test program

## Supported Drug Database

The system includes a database of standard chemotherapeutic agents with quantum parameters:
- **Cisplatin** - DNA-binding platinum compound
- **Paclitaxel** - Microtubule-stabilizing agent
- **Doxorubicin** - DNA intercalating agent
- **Temozolomide** - Alkylating agent
- **Fluorouracil** - Antimetabolite

## Modern C++ Features Used

This implementation uses several modern C++ features:
- Smart pointers (`std::shared_ptr`) for resource management
- Namespaces and using-declarations to avoid naming conflicts
- Type aliasing for complex types via `using`
- Default member initializers in class definitions
- Strongly-typed enums for better type safety
- Constexpr for compile-time constants

## References

Arodola, O.A. and Soliman, M.E.S. (2017). Quantum mechanics implementation in drug-design workflows: Does it really help? *Drug Design, Development and Therapy*, 11, 2551-2564.
