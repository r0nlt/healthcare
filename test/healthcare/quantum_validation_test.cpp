#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "rad_ml/healthcare/quantum_enhanced/tunneling_model.hpp"
#include "rad_ml/healthcare/quantum_enhanced/wave_equation_solver.hpp"

using namespace rad_ml::healthcare::quantum_enhanced;

// Test constants
const double EPSILON = 1e-6;

// Helper function to compare doubles with epsilon
bool areClose(double a, double b, double epsilon = EPSILON) { return std::fabs(a - b) < epsilon; }

// Test for quantum wave equation solver
void testQuantumWaveEquationSolver()
{
    std::cout << "Testing QuantumWaveEquationSolver..." << std::endl;

    // Create configuration with known values
    WaveEquationConfig config;
    config.solver_method = SolverMethod::CRANK_NICOLSON;
    config.boundary_condition = BoundaryCondition::DIRICHLET;
    config.spatial_step = 0.1;  // nm
    config.time_step = 0.001;   // fs
    config.grid_points = 100;
    config.time_steps = 100;

    // Create solver
    QuantumWaveEquationSolver solver(config);

    // Initialize a Gaussian wave packet
    double center_x = 5.0;  // nm
    double width = 1.0;     // nm
    double k_0 = 2.0;       // Initial momentum
    solver.initializeGaussianWavePacket(center_x, width, k_0);

    // Get initial wavefunction and verify it's normalized
    const auto& wavefunction = solver.getWavefunction();

    // Check wavefunction size
    assert(wavefunction.size() == config.grid_points);

    // Check normalization
    double norm = 0.0;
    for (const auto& psi : wavefunction) {
        norm += std::norm(psi);  // |psi|^2
    }
    norm *= config.spatial_step;

    assert(areClose(norm, 1.0, 1e-4));
    std::cout << "  Initial wavefunction is normalized: " << norm << std::endl;

    // Evolve for a few time steps
    solver.evolveWavefunction(10);

    // Check that probability is conserved
    const auto& evolved_wavefunction = solver.getWavefunction();
    double evolved_norm = 0.0;
    for (const auto& psi : evolved_wavefunction) {
        evolved_norm += std::norm(psi);
    }
    evolved_norm *= config.spatial_step;

    assert(areClose(evolved_norm, 1.0, 1e-4));
    std::cout << "  Probability is conserved after evolution: " << evolved_norm << std::endl;

    // Set up a potential barrier
    double barrier_height = 0.5;     // eV
    double barrier_width = 1.0;      // nm
    double barrier_position = 10.0;  // nm
    solver.setMembranePotentialBarrier(barrier_height, barrier_width, barrier_position);

    // Evolve again with the barrier
    solver.evolveWavefunction(50);

    // Calculate tunneling probability
    double tunneling_prob = solver.calculateTunnelingProbability();
    std::cout << "  Tunneling probability: " << tunneling_prob << std::endl;

    // Tunneling probability should be between 0 and 1
    assert(tunneling_prob >= 0.0 && tunneling_prob <= 1.0);

    // Check that expected position changes during evolution
    double initial_pos = solver.calculateExpectedPosition();
    solver.evolveWavefunction(20);
    double final_pos = solver.calculateExpectedPosition();

    std::cout << "  Initial position: " << initial_pos << " nm" << std::endl;
    std::cout << "  Final position: " << final_pos << " nm" << std::endl;

    // Position should change due to momentum
    assert(initial_pos != final_pos);

    std::cout << "QuantumWaveEquationSolver tests passed!" << std::endl << std::endl;
}

// Test for quantum tunneling model
void testQuantumTunnelingModel()
{
    std::cout << "Testing QuantumTunnelingModel..." << std::endl;

    // Create configuration
    TunnelingConfig config;
    config.method = TunnelingMethod::WKB_APPROXIMATION;
    config.barrier_shape = BarrierShape::RECTANGULAR;
    config.include_zero_point = true;
    config.include_temperature = true;

    // Create tunneling model
    QuantumTunnelingModel tunneling_model(config);

    // Set parameters
    tunneling_model.setEffectiveMass(9.109e-31);  // Electron mass
    tunneling_model.setTemperature(310.0);        // Body temperature (K)

    // Add a potential barrier
    PotentialBarrier barrier;
    barrier.position = 0.0;
    barrier.width = 1.0;   // nm
    barrier.height = 0.5;  // eV
    barrier.shape = BarrierShape::RECTANGULAR;

    tunneling_model.addPotentialBarrier(barrier);

    // Calculate tunneling probability for different energies
    double energy1 = 0.1;  // eV (below barrier)
    double energy2 = 0.5;  // eV (equal to barrier)
    double energy3 = 1.0;  // eV (above barrier)

    double prob1 = tunneling_model.calculateTunnelingProbability(energy1);
    double prob2 = tunneling_model.calculateTunnelingProbability(energy2);
    double prob3 = tunneling_model.calculateTunnelingProbability(energy3);

    std::cout << "  Tunneling probability (E = 0.1 eV): " << prob1 << std::endl;
    std::cout << "  Tunneling probability (E = 0.5 eV): " << prob2 << std::endl;
    std::cout << "  Tunneling probability (E = 1.0 eV): " << prob3 << std::endl;

    // Probabilities should be between 0 and 1
    assert(prob1 >= 0.0 && prob1 <= 1.0);
    assert(prob2 >= 0.0 && prob2 <= 1.0);
    assert(prob3 >= 0.0 && prob3 <= 1.0);

    // Higher energy should have higher tunneling probability
    assert(prob1 < prob2);
    assert(prob2 < prob3);

    // Calculate thermal tunneling
    double thermal_prob = tunneling_model.calculateThermalTunnelingProbability();
    std::cout << "  Thermal tunneling probability: " << thermal_prob << std::endl;
    assert(thermal_prob >= 0.0 && thermal_prob <= 1.0);

    // Add a second barrier to test multi-barrier tunneling
    PotentialBarrier barrier2;
    barrier2.position = 2.0;
    barrier2.width = 0.5;
    barrier2.height = 0.7;
    barrier2.shape = BarrierShape::RECTANGULAR;

    tunneling_model.addPotentialBarrier(barrier2);

    // Calculate tunneling through multiple barriers
    double multi_barrier_prob = tunneling_model.calculateTunnelingProbability(energy2);
    std::cout << "  Multi-barrier tunneling probability: " << multi_barrier_prob << std::endl;

    // Multi-barrier probability should be lower than single barrier
    assert(multi_barrier_prob <= prob2);

    // Calculate resonant tunneling (if available)
    if (config.include_resonance) {
        double resonant_prob = tunneling_model.calculateResonantTunnelingProbability(energy1);
        std::cout << "  Resonant tunneling probability: " << resonant_prob << std::endl;
        assert(resonant_prob >= 0.0 && resonant_prob <= 1.0);
    }

    // Get transmission spectrum
    auto spectrum = tunneling_model.getTransmissionSpectrum(0.1, 1.0, 10);
    assert(spectrum.size() == 10);

    std::cout << "  Transmission spectrum:" << std::endl;
    for (const auto& point : spectrum) {
        std::cout << "    E = " << point.first << " eV, T = " << point.second << std::endl;
        assert(point.second >= 0.0 && point.second <= 1.0);
    }

    // Test membrane barrier setup
    tunneling_model.addMembraneBarrier(5.0, 2.0, 0.01);

    double membrane_prob = tunneling_model.calculateTunnelingProbability(0.3);
    std::cout << "  Membrane barrier tunneling probability: " << membrane_prob << std::endl;
    assert(membrane_prob >= 0.0 && membrane_prob <= 1.0);

    std::cout << "QuantumTunnelingModel tests passed!" << std::endl << std::endl;
}

// Test the solveBioQuantumWaveEquation function
void testSolveBioQuantumWaveEquation()
{
    std::cout << "Testing solveBioQuantumWaveEquation..." << std::endl;

    double hbar = 1.0545718e-34;        // J·s
    double effective_mass = 9.109e-31;  // kg
    double potential_coeff = 0.3;       // eV
    double coupling_constant = 0.1;
    double cell_spacing = 10.0;  // nm
    double time_step = 1.0;      // fs

    double result = solveBioQuantumWaveEquation(hbar, effective_mass, potential_coeff,
                                                coupling_constant, cell_spacing, time_step);

    std::cout << "  Result: " << result << std::endl;

    // Result should be finite
    assert(std::isfinite(result));

    std::cout << "solveBioQuantumWaveEquation test passed!" << std::endl << std::endl;
}

// Test the calculateBioQuantumTunnelingProbability function
void testCalculateBioQuantumTunnelingProbability()
{
    std::cout << "Testing calculateBioQuantumTunnelingProbability..." << std::endl;

    double barrier_height = 0.5;        // eV
    double effective_mass = 9.109e-31;  // kg
    double hbar = 1.0545718e-34;        // J·s
    double temperature = 310.0;         // K

    double result =
        calculateBioQuantumTunnelingProbability(barrier_height, effective_mass, hbar, temperature);

    std::cout << "  Result: " << result << std::endl;

    // Result should be between 0 and 1
    assert(result >= 0.0 && result <= 1.0);

    // Test with different barrier heights
    double result_high =
        calculateBioQuantumTunnelingProbability(1.0, effective_mass, hbar, temperature);
    double result_low =
        calculateBioQuantumTunnelingProbability(0.1, effective_mass, hbar, temperature);

    std::cout << "  Result (high barrier): " << result_high << std::endl;
    std::cout << "  Result (low barrier): " << result_low << std::endl;

    // Higher barrier should have lower tunneling probability
    assert(result_high <= result);
    assert(result_low >= result);

    std::cout << "calculateBioQuantumTunnelingProbability test passed!" << std::endl << std::endl;
}

// Test drug membrane tunneling factor
void testCalculateDrugMembraneTunnelingFactor()
{
    std::cout << "Testing calculateDrugMembraneTunnelingFactor..." << std::endl;

    double drug_radius = 0.5;         // nm
    double membrane_thickness = 5.0;  // nm
    double temperature = 310.0;       // K
    double dielectric_constant = 2.0;

    double result = calculateDrugMembraneTunnelingFactor(drug_radius, membrane_thickness,
                                                         temperature, dielectric_constant);

    std::cout << "  Result: " << result << std::endl;

    // Result should be positive
    assert(result >= 0.0);

    // Test with different drug sizes
    double result_large = calculateDrugMembraneTunnelingFactor(1.0, membrane_thickness, temperature,
                                                               dielectric_constant);
    double result_small = calculateDrugMembraneTunnelingFactor(0.1, membrane_thickness, temperature,
                                                               dielectric_constant);

    std::cout << "  Result (large drug): " << result_large << std::endl;
    std::cout << "  Result (small drug): " << result_small << std::endl;

    // Larger drugs should have lower tunneling factor
    assert(result_large <= result);
    assert(result_small >= result);

    std::cout << "calculateDrugMembraneTunnelingFactor test passed!" << std::endl << std::endl;
}

// Main test function
int main()
{
    std::cout << "Running Quantum Healthcare Framework Tests" << std::endl;
    std::cout << "==========================================" << std::endl << std::endl;

    // Run tests
    testQuantumWaveEquationSolver();
    testQuantumTunnelingModel();
    testSolveBioQuantumWaveEquation();
    testCalculateBioQuantumTunnelingProbability();
    testCalculateDrugMembraneTunnelingFactor();

    std::cout << "All quantum healthcare tests passed successfully!" << std::endl;
    return 0;
}
