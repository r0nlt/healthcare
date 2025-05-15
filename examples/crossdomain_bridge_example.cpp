/**
 * Cross-Domain Quantum Field Theory Bridge Example
 *
 * This example demonstrates the unified quantum field theory bridge
 * between semiconductor and biological systems.
 */

#include <iomanip>
#include <iostream>
#include <rad_ml/crossdomain/qft_bridge.hpp>
#include <rad_ml/healthcare/bio_quantum_integration.hpp>
#include <rad_ml/physics/quantum_integration.hpp>

using namespace rad_ml;

// Print semiconductor parameters
void printSemiconductorParams(const crossdomain::SemiconductorParameters& params)
{
    std::cout << "Semiconductor Parameters:" << std::endl;
    std::cout << "  - Energy gap:              " << params.energy_gap << " eV" << std::endl;
    std::cout << "  - Effective mass:          " << params.effective_mass << " m₀" << std::endl;
    std::cout << "  - Feature size:            " << params.feature_size << " nm" << std::endl;
    std::cout << "  - Temperature:             " << params.temperature << " K" << std::endl;
    std::cout << "  - Defect formation energy: " << params.defect_formation_energy << " eV"
              << std::endl;
    std::cout << "  - Barrier height:          " << params.barrier_height << " eV" << std::endl;
    std::cout << std::endl;
}

// Print biological parameters
void printBiologicalParams(const crossdomain::BiologicalParameters& params)
{
    std::cout << "Biological Parameters:" << std::endl;
    std::cout << "  - Bond energy:             " << params.bond_energy << " eV" << std::endl;
    std::cout << "  - Effective mass:          " << params.effective_mass << " kg" << std::endl;
    std::cout << "  - Cell size:               " << params.cell_size << " μm" << std::endl;
    std::cout << "  - Feature size:            " << params.feature_size << " nm" << std::endl;
    std::cout << "  - Temperature:             " << params.temperature << " K" << std::endl;
    std::cout << "  - Water content:           " << params.water_content * 100.0 << "%"
              << std::endl;
    std::cout << "  - Radiosensitivity:        " << params.radiosensitivity << std::endl;
    std::cout << "  - Repair rate:             " << params.repair_rate << std::endl;
    std::cout << "  - Barrier height:          " << params.barrier_height << " eV" << std::endl;
    std::cout << std::endl;
}

// Print quantum effects in both domains
void compareQuantumEffects(const crossdomain::SemiconductorParameters& semi_params,
                           const crossdomain::BiologicalParameters& bio_params)
{
    // Create domain-specific processors
    crossdomain::SemiconductorQFTProcessor semi_processor;
    crossdomain::BiologicalQFTProcessor bio_processor;

    // Calculate quantum effects
    double semi_enhancement =
        semi_processor.calculateEnhancementFactor(semi_params, semi_params.temperature);
    double semi_tunneling =
        semi_processor.calculateTunnelingProbability(semi_params, semi_params.temperature);
    double semi_zpe =
        semi_processor.calculateZeroPointEnergyContribution(semi_params, semi_params.temperature);

    double bio_enhancement =
        bio_processor.calculateEnhancementFactor(bio_params, bio_params.temperature);
    double bio_tunneling =
        bio_processor.calculateTunnelingProbability(bio_params, bio_params.temperature);
    double bio_zpe =
        bio_processor.calculateZeroPointEnergyContribution(bio_params, bio_params.temperature);

    // Print comparison
    std::cout << "Quantum Effects Comparison:" << std::endl;
    std::cout << std::setprecision(4) << std::fixed;
    std::cout << "  Factor                      Semiconductor      Biological       Ratio"
              << std::endl;
    std::cout << "  -----------------------------------------------------------------" << std::endl;
    std::cout << "  Enhancement factor:         " << std::setw(8) << semi_enhancement
              << "          " << std::setw(8) << bio_enhancement << "        " << std::setw(6)
              << bio_enhancement / semi_enhancement << std::endl;

    std::cout << "  Tunneling probability:      " << std::setw(8) << semi_tunneling << "          "
              << std::setw(8) << bio_tunneling << "        " << std::setw(6)
              << bio_tunneling / semi_tunneling << std::endl;

    std::cout << "  Zero-point energy:          " << std::setw(8) << semi_zpe << "          "
              << std::setw(8) << bio_zpe << "        " << std::setw(6) << bio_zpe / semi_zpe
              << std::endl;
    std::cout << std::endl;
}

int main()
{
    std::cout << "=== Cross-Domain Quantum Field Theory Bridge Example ===" << std::endl;
    std::cout << std::string(60, '=') << std::endl << std::endl;

    // Example 1: Silicon to Bio-equivalent conversion
    std::cout << "Example 1: Silicon to Biological Equivalent" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    // Create silicon parameters
    crossdomain::SemiconductorParameters silicon;
    silicon.energy_gap = 1.12;      // eV
    silicon.effective_mass = 0.26;  // m₀
    silicon.feature_size = 15.0;    // nm
    silicon.temperature = 300.0;    // K
    silicon.barrier_height = 0.5;   // eV

    // Print silicon parameters
    printSemiconductorParams(silicon);

    // Convert to biological parameters
    crossdomain::BiologicalParameters bio_equiv = crossdomain::convertToBiological(silicon);

    // Print biological parameters
    printBiologicalParams(bio_equiv);

    // Compare quantum effects
    compareQuantumEffects(silicon, bio_equiv);

    // Example 2: Biological to Semiconductor conversion
    std::cout << "Example 2: Biological to Semiconductor Equivalent" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    // Create biological parameters
    crossdomain::BiologicalParameters tissue;
    tissue.bond_energy = 0.3;         // eV
    tissue.effective_mass = 1.5e-29;  // kg
    tissue.cell_size = 10.0;          // μm
    tissue.feature_size = 8.0;        // nm
    tissue.temperature = 310.0;       // K (body temperature)
    tissue.water_content = 0.8;       // 80% water
    tissue.radiosensitivity = 1.2;    // More sensitive than average
    tissue.repair_rate = 0.5;         // Good repair capability
    tissue.barrier_height = 0.2;      // eV

    // Print biological parameters
    printBiologicalParams(tissue);

    // Convert to semiconductor parameters
    crossdomain::SemiconductorParameters semi_equiv = crossdomain::convertToSemiconductor(tissue);

    // Print semiconductor parameters
    printSemiconductorParams(semi_equiv);

    // Compare quantum effects
    compareQuantumEffects(semi_equiv, tissue);

    // Example 3: Radiation damage prediction across domains
    std::cout << "Example 3: Cross-Domain Radiation Damage Prediction" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    // Different radiation doses
    double doses[] = {0.5, 1.0, 2.0, 5.0};  // Gy

    for (double dose : doses) {
        std::cout << "Radiation dose: " << dose << " Gy" << std::endl;
        std::cout << "--------------------------" << std::endl;

        // Test cross-domain equivalence
        bool within_tolerance = crossdomain::testCrossDomainEquivalence(silicon, bio_equiv, dose);

        std::cout << "Validation " << (within_tolerance ? "PASSED" : "FAILED") << " at " << dose
                  << " Gy" << std::endl
                  << std::endl;
    }

    // Example 4: Temperature-dependent quantum effects
    std::cout << "Example 4: Temperature-Dependent Quantum Effects" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    double temperatures[] = {77.0, 150.0, 300.0, 400.0};  // K

    std::cout << "Temperature  |  Silicon Enhancement  |  Bio Enhancement  |  Ratio" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;

    crossdomain::SemiconductorQFTProcessor semi_processor;
    crossdomain::BiologicalQFTProcessor bio_processor;

    for (double temp : temperatures) {
        // Update temperature
        silicon.temperature = temp;
        bio_equiv.temperature = temp;

        // Calculate enhancements
        double semi_enhancement = semi_processor.calculateEnhancementFactor(silicon, temp);
        double bio_enhancement = bio_processor.calculateEnhancementFactor(bio_equiv, temp);
        double ratio = bio_enhancement / semi_enhancement;

        // Print results
        std::cout << std::setw(8) << temp << " K  |  " << std::setw(14) << std::fixed
                  << std::setprecision(4) << semi_enhancement << "  |  " << std::setw(14)
                  << bio_enhancement << "  |  " << std::setw(6) << ratio << std::endl;
    }
    std::cout << std::endl;

    // Example 5: Feature size-dependent quantum effects
    std::cout << "Example 5: Feature Size-Dependent Quantum Effects" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    double feature_sizes[] = {5.0, 10.0, 20.0, 45.0};  // nm

    std::cout << "Feature Size  |  Silicon Enhancement  |  Bio Enhancement  |  Ratio" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;

    // Reset temperature
    silicon.temperature = 300.0;
    bio_equiv.temperature = 300.0;

    for (double size : feature_sizes) {
        // Update feature size
        silicon.feature_size = size;
        bio_equiv.feature_size = size;

        // Calculate enhancements
        double semi_enhancement =
            semi_processor.calculateEnhancementFactor(silicon, silicon.temperature);
        double bio_enhancement =
            bio_processor.calculateEnhancementFactor(bio_equiv, bio_equiv.temperature);
        double ratio = bio_enhancement / semi_enhancement;

        // Print results
        std::cout << std::setw(8) << size << " nm  |  " << std::setw(14) << std::fixed
                  << std::setprecision(4) << semi_enhancement << "  |  " << std::setw(14)
                  << bio_enhancement << "  |  " << std::setw(6) << ratio << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Cross-domain bridge example completed successfully." << std::endl;
    std::cout << "The unified quantum field theory foundation successfully bridges " << std::endl;
    std::cout << "semiconductor and biological systems with consistent physics principles."
              << std::endl;

    return 0;
}
