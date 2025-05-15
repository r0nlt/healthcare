#include <iostream>
#include <map>
#include <string>

#include "../include/rad_ml/healthcare/bio_quantum_integration.hpp"

using namespace rad_ml::healthcare;

// A simple test function to demonstrate biological quantum effects
void basicBioQuantumTest()
{
    std::cout << "Running basic bio-quantum effects test..." << std::endl;

    // Create a biological system (soft tissue)
    BiologicalSystem tissue;
    tissue.type = SOFT_TISSUE;
    tissue.water_content = 0.7;      // 70% water
    tissue.cell_density = 1.0e6;     // 1 million cells per mm³
    tissue.effective_barrier = 0.3;  // eV
    tissue.repair_rate = 0.3;        // cellular repair capability
    tissue.radiosensitivity = 1.2;   // radiation sensitivity factor

    // Create basic quantum configuration
    BioQuantumConfig config;
    config.enable_quantum_corrections = true;
    config.force_quantum_corrections = false;
    config.temperature_threshold = 305.0;   // K
    config.cell_size_threshold = 12.0;      // µm
    config.radiation_dose_threshold = 2.0;  // Gy
    config.dna_damage_threshold = 0.2;
    config.radiolysis_factor = 0.3;

    // Create a sample cellular damage distribution
    CellularDamageDistribution damage;
    damage["dna_strand_break"] = 0.5;
    damage["protein_damage"] = 0.3;
    damage["membrane_lipid_peroxidation"] = 0.2;
    damage["mitochondrial_damage"] = 0.1;

    // Test parameters
    double temperature = 293.0;   // K (room temperature)
    double cell_size = 10.0;      // µm
    double radiation_dose = 2.5;  // Gy

    // Print initial damage
    std::cout << "Initial damage values:" << std::endl;
    for (const auto& pair : damage) {
        std::cout << "  - " << pair.first << ": " << pair.second << std::endl;
    }

    // Calculate QFT parameters
    BiologicalQFTParameters qft_params = createBioQFTParameters(tissue, cell_size);

    std::cout << "\nCalculated QFT parameters:" << std::endl;
    std::cout << "  - Effective mass: " << qft_params.effective_mass << std::endl;
    std::cout << "  - Coupling constant: " << qft_params.coupling_constant << std::endl;
    std::cout << "  - Water coupling: " << qft_params.water_coupling << std::endl;
    std::cout << "  - Cell spacing: " << qft_params.cell_spacing << std::endl;

    // Test quantum enhancement factor
    double enhancement = calculateBioQuantumEnhancementFactor(temperature, cell_size);
    std::cout << "\nQuantum enhancement factor: " << enhancement << std::endl;

    // Test quantum tunneling probability
    double tunneling = calculateBioQuantumTunnelingProbability(
        tissue.effective_barrier, qft_params.effective_mass, qft_params.hbar, temperature);
    std::cout << "Quantum tunneling probability: " << tunneling << std::endl;

    // Test wave equation correction
    double wave_correction = solveBioQuantumWaveEquation(
        qft_params.hbar, qft_params.effective_mass, 0.3, qft_params.coupling_constant,
        qft_params.cell_spacing, qft_params.time_step);
    std::cout << "Quantum wave correction: " << wave_correction << std::endl;

    // Test zero-point energy contribution
    double zpe = calculateBioZeroPointEnergyContribution(qft_params.hbar, qft_params.effective_mass,
                                                         tissue.water_content, temperature);
    std::cout << "Zero-point energy contribution: " << zpe << std::endl;

    // Check if quantum corrections should be applied
    bool apply_corrections =
        shouldApplyBioQuantumCorrections(temperature, cell_size, radiation_dose, config);
    std::cout << "\nShould apply quantum corrections: " << (apply_corrections ? "Yes" : "No")
              << std::endl;

    // Apply quantum corrections
    CellularDamageDistribution corrected_damage =
        applyBioQuantumFieldCorrections(damage, tissue, qft_params, temperature);

    // Print corrected damage
    std::cout << "\nQuantum-corrected damage values:" << std::endl;
    for (const auto& pair : corrected_damage) {
        std::cout << "  - " << pair.first << ": " << pair.second << std::endl;
    }

    std::cout << "\nTest completed successfully!" << std::endl;
}

int main()
{
    std::cout << "Testing Radiation-Tolerant ML Healthcare Module" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    // Run the basic test
    basicBioQuantumTest();

    return 0;
}
