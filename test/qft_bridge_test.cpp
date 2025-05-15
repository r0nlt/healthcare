/**
 * QFT Bridge Test Program
 *
 * This program tests the quantum field theory bridge implementation
 * with the scientifically validated parameters.
 */

#include <iomanip>
#include <iostream>
#include <rad_ml/crossdomain/qft_bridge.hpp>
#include <rad_ml/healthcare/bio_quantum_integration.hpp>
#include <rad_ml/physics/quantum_integration.hpp>

using namespace rad_ml;

// Test basic parameter conversion
void testParameterConversion()
{
    std::cout << "=== Testing Parameter Conversion ===" << std::endl;

    // Create silicon parameters with scientifically validated values
    crossdomain::SemiconductorParameters silicon;
    silicon.energy_gap = 1.12;      // Silicon bandgap at 300K [1]
    silicon.effective_mass = 0.26;  // Longitudinal effective mass for Si [2]
    silicon.feature_size = 15.0;    // nm
    silicon.temperature = 300.0;    // K, room temperature
    silicon.barrier_height = 3.1;   // Si-SiO₂ barrier height [3]

    // Convert to biological parameters
    crossdomain::BiologicalParameters bio_params = crossdomain::convertToBiological(silicon);

    // Print and validate the conversion
    std::cout << "Silicon parameters:" << std::endl;
    std::cout << "  - Energy gap:     " << silicon.energy_gap << " eV" << std::endl;
    std::cout << "  - Barrier height: " << silicon.barrier_height << " eV" << std::endl;
    std::cout << "  - Feature size:   " << silicon.feature_size << " nm" << std::endl;

    std::cout << "Converted biological parameters:" << std::endl;
    std::cout << "  - Bond energy:     " << bio_params.bond_energy << " eV (expected: 0.3-0.5)"
              << std::endl;
    std::cout << "  - Barrier height:  " << bio_params.barrier_height << " eV (expected: 0.2-0.5)"
              << std::endl;
    std::cout << "  - Feature size:    " << bio_params.feature_size << " nm (expected: 7-9)"
              << std::endl;
    std::cout << "  - α/β ratio:       " << bio_params.alpha_over_beta
              << " (expected: tissue-specific value)" << std::endl;
    std::cout << "  - α coefficient:   " << bio_params.alpha << " Gy⁻¹" << std::endl;
    std::cout << "  - β coefficient:   " << bio_params.beta << " Gy⁻²" << std::endl;

    // Validate the parameter conversion
    bool valid = true;
    if (bio_params.bond_energy < 0.3 || bio_params.bond_energy > 0.5) {
        std::cout << "ERROR: Bond energy outside expected range (0.3-0.5 eV)" << std::endl;
        valid = false;
    }

    if (bio_params.barrier_height < 0.2 || bio_params.barrier_height > 0.5) {
        std::cout << "ERROR: Barrier height outside expected range (0.2-0.5 eV)" << std::endl;
        valid = false;
    }

    if (bio_params.feature_size < 7.0 || bio_params.feature_size > 9.0) {
        std::cout << "ERROR: Feature size outside expected range (7-9 nm)" << std::endl;
        valid = false;
    }

    std::cout << "Parameter conversion " << (valid ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
}

// Test quantum enhancement factors
void testQuantumEnhancement()
{
    std::cout << "=== Testing Quantum Enhancement Factors ===" << std::endl;

    // Create processors
    crossdomain::SemiconductorQFTProcessor semi_processor;
    crossdomain::BiologicalQFTProcessor bio_processor;

    // Create parameters
    crossdomain::SemiconductorParameters silicon;
    silicon.energy_gap = 1.12;      // Silicon bandgap at 300K
    silicon.effective_mass = 0.26;  // Longitudinal effective mass for Si
    silicon.feature_size = 15.0;    // nm
    silicon.temperature = 300.0;    // K, room temperature
    silicon.barrier_height = 3.1;   // Si-SiO₂ barrier height

    crossdomain::BiologicalParameters bio_params;
    bio_params.bond_energy = 0.4;       // DNA/RNA bond energy
    bio_params.temperature = 310.0;     // K, body temperature
    bio_params.feature_size = 8.0;      // nm, cell membrane thickness
    bio_params.water_content = 0.7;     // 70% water content
    bio_params.radiosensitivity = 1.0;  // Normal sensitivity

    // Test silicon enhancement - should be in 8-12% range
    double si_enhancement = semi_processor.calculateEnhancementFactor(silicon, silicon.temperature);
    std::cout << "Silicon quantum enhancement: " << std::fixed << std::setprecision(4)
              << si_enhancement << " (expected range: 1.08-1.12)" << std::endl;

    // Test biological enhancement - should be in 3-5% range
    double bio_enhancement =
        bio_processor.calculateEnhancementFactor(bio_params, bio_params.temperature);
    std::cout << "Biological quantum enhancement: " << std::fixed << std::setprecision(4)
              << bio_enhancement << " (expected range: 1.03-1.05)" << std::endl;

    // Test tunneling probability
    double si_tunneling =
        semi_processor.calculateTunnelingProbability(silicon, silicon.temperature);
    std::cout << "Silicon tunneling probability: " << std::scientific << std::setprecision(6)
              << si_tunneling << std::endl;

    double bio_tunneling =
        bio_processor.calculateTunnelingProbability(bio_params, bio_params.temperature);
    std::cout << "Biological tunneling probability: " << std::scientific << std::setprecision(6)
              << bio_tunneling << std::endl;

    // Validate the enhancement factors
    bool valid = true;
    if (si_enhancement < 1.08 || si_enhancement > 1.12) {
        std::cout << "ERROR: Silicon enhancement outside expected range (1.08-1.12)" << std::endl;
        valid = false;
    }

    if (bio_enhancement < 1.03 || bio_enhancement > 1.05) {
        std::cout << "ERROR: Biological enhancement outside expected range (1.03-1.05)"
                  << std::endl;
        valid = false;
    }

    std::cout << "Quantum enhancement " << (valid ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
}

// Test radiation damage prediction
void testRadiationDamage()
{
    std::cout << "=== Testing Radiation Damage Prediction ===" << std::endl;

    // Create parameters
    crossdomain::SemiconductorParameters silicon;
    silicon.energy_gap = 1.12;      // Silicon bandgap at 300K
    silicon.effective_mass = 0.26;  // Longitudinal effective mass for Si
    silicon.feature_size = 15.0;    // nm
    silicon.temperature = 300.0;    // K
    silicon.barrier_height = 3.1;   // Si-SiO₂ barrier height

    // Convert to biological parameters
    crossdomain::BiologicalParameters bio_params = crossdomain::convertToBiological(silicon);

    // Test radiation doses
    double doses[] = {0.5, 1.0, 2.0, 5.0};  // Gy

    for (double dose : doses) {
        std::cout << "Radiation dose: " << dose << " Gy" << std::endl;

        // Test cross-domain equivalence
        bool within_tolerance = crossdomain::testCrossDomainEquivalence(silicon, bio_params, dose);

        // Count passes and fails
        std::cout << "Cross-domain validation " << (within_tolerance ? "PASSED" : "FAILED")
                  << " at " << dose << " Gy" << std::endl
                  << std::endl;
    }
}

// Main function
int main()
{
    std::cout << "==== QFT Bridge Test Program ====" << std::endl;
    std::cout << std::string(60, '=') << std::endl << std::endl;

    // Run tests
    testParameterConversion();
    testQuantumEnhancement();
    testRadiationDamage();

    return 0;
}
