#include <iomanip>
#include <iostream>
#include <string>

#include "rad_ml/quantum/monte_carlo.hpp"

using namespace rad_ml::quantum;

void printUsage()
{
    std::cout << "Quantum Monte Carlo Wave Equation Test\n";
    std::cout << "======================================\n";
    std::cout << "Usage: quantum_mc_test [options]\n\n";
    std::cout << "Available options:\n";
    std::cout << "  --samples N       Number of Monte Carlo samples (default: 10000)\n";
    std::cout << "  --threads N       Number of threads to use (default: all available)\n";
    std::cout << "  --temp-min X      Minimum temperature in K (default: 10.0)\n";
    std::cout << "  --temp-max X      Maximum temperature in K (default: 300.0)\n";
    std::cout << "  --size-min X      Minimum feature size in nm (default: 2.0)\n";
    std::cout << "  --size-max X      Maximum feature size in nm (default: 50.0)\n";
    std::cout << "  --barrier-min X   Minimum barrier height in eV (default: 0.1)\n";
    std::cout << "  --barrier-max X   Maximum barrier height in eV (default: 5.0)\n";
    std::cout
        << "  --zpe-correction-model [simple|detailed]  ZPE correction model (default: detailed)\n";
    std::cout
        << "  --material [Si|Ge|GaAs|protein|dna|water|custom]  Material type (default: Si)\n";
    std::cout << "  --env-effects [enabled|disabled]  Environmental effects (default: disabled)\n";
    std::cout
        << "  --protein-env-model [simple|detailed]  Protein environment model (default: simple)\n";
    std::cout << "  --solvent-effects [enabled|disabled]  Solvent effects (default: disabled)\n";
    std::cout << "  --help            Display this help message\n";
}

int main(int argc, char* argv[])
{
    // Default parameters
    MCSimulationParameters params;
    QFTParameters qft_params;
    CrystalParameters crystal;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help") {
            printUsage();
            return 0;
        }
        else if (arg == "--samples" && i + 1 < argc) {
            params.num_samples = std::stoul(argv[++i]);
        }
        else if (arg == "--threads" && i + 1 < argc) {
            params.num_threads = std::stoul(argv[++i]);
        }
        else if (arg == "--temp-min" && i + 1 < argc) {
            params.temp_min = std::stod(argv[++i]);
        }
        else if (arg == "--temp-max" && i + 1 < argc) {
            params.temp_max = std::stod(argv[++i]);
        }
        else if (arg == "--size-min" && i + 1 < argc) {
            // Convert from nm to m
            params.size_min = std::stod(argv[++i]) * 1.0e-9;
        }
        else if (arg == "--size-max" && i + 1 < argc) {
            // Convert from nm to m
            params.size_max = std::stod(argv[++i]) * 1.0e-9;
        }
        else if (arg == "--barrier-min" && i + 1 < argc) {
            params.barrier_min = std::stod(argv[++i]);
        }
        else if (arg == "--barrier-max" && i + 1 < argc) {
            params.barrier_max = std::stod(argv[++i]);
        }
        else if (arg == "--zpe-correction-model" && i + 1 < argc) {
            std::string model = argv[++i];
            params.use_detailed_zpe_model = (model == "detailed");
        }
        else if (arg == "--material" && i + 1 < argc) {
            std::string material = argv[++i];
            if (material == "Si" || material == "silicon") {
                crystal.material_type = MaterialModelFactory::SILICON;
            }
            else if (material == "Ge" || material == "germanium") {
                crystal.material_type = MaterialModelFactory::GERMANIUM;
            }
            else if (material == "GaAs" || material == "gallium_arsenide") {
                crystal.material_type = MaterialModelFactory::GALLIUM_ARSENIDE;
            }
            else if (material == "protein") {
                crystal.material_type = MaterialModelFactory::PROTEIN;
            }
            else if (material == "dna") {
                crystal.material_type = MaterialModelFactory::DNA;
            }
            else if (material == "water") {
                crystal.material_type = MaterialModelFactory::WATER;
            }
            else if (material == "custom") {
                crystal.material_type = MaterialModelFactory::CUSTOM;
                // Would need additional parameters for custom material
            }
        }
        else if (arg == "--env-effects" && i + 1 < argc) {
            std::string enabled = argv[++i];
            params.enable_env_effects = (enabled == "enabled");
        }
    }

    // Print simulation parameters
    std::cout << "Monte Carlo Test for Wave Equation Solver\n";
    std::cout << "=======================================\n";
    std::cout << "Number of samples: " << params.num_samples << "\n";
    std::cout << "Number of threads: " << params.num_threads << "\n";
    std::cout << "Temperature range: [" << params.temp_min << ", " << params.temp_max << "] K\n";
    std::cout << "Feature size range: [" << params.size_min * 1.0e9 << ", "
              << params.size_max * 1.0e9 << "] nm\n";
    std::cout << "Barrier height range: [" << params.barrier_min << ", " << params.barrier_max
              << "] eV\n";
    std::cout << "ZPE Model: "
              << (params.use_detailed_zpe_model ? "Detailed (Pure ZPE + Thermal)" : "Simple")
              << "\n";
    std::cout << "Environmental effects: " << (params.enable_env_effects ? "Enabled" : "Disabled")
              << "\n";
    std::cout << "---------------------------------------\n";
    std::cout << "Launching " << params.num_threads << " worker threads...\n\n";

    // Run the simulation
    MCSimulationResults results = runMonteCarloSimulation(params, qft_params, crystal);

    // Print the results
    printSimulationSummary(params, results);

    // Additional analysis specific to our refined ZPE model
    if (params.use_detailed_zpe_model) {
        std::cout << "\nRefined ZPE Model Analysis\n";
        std::cout << "==========================\n";

        std::cout << "Pure ZPE / Total Quantum Contribution Ratio: " << std::fixed
                  << std::setprecision(6) << results.mean_pure_zpe / results.mean_zpe * 100.0
                  << "%\n";

        std::cout << "Temperature Dependence Analysis:\n";
        std::cout << "  Pure ZPE Temperature Correlation: " << results.corr_pure_zpe_temp << "\n";
        std::cout << "  Thermal Quantum Effects Temperature Correlation: "
                  << results.corr_thermal_temp << "\n";

        if (std::abs(results.corr_pure_zpe_temp) < 0.01) {
            std::cout << "√ Pure ZPE is correctly temperature-independent (correlation ≈ 0)\n";
        }
        else {
            std::cout << "⚠ Pure ZPE shows unexpected temperature dependence!\n";
        }

        if (results.corr_thermal_temp > 0.9) {
            std::cout
                << "√ Thermal quantum correction shows expected strong temperature dependence\n";
        }

        // Theoretical validation
        std::cout << "\nTheoretical Validation:\n";

        // Calculate expected pure ZPE for Silicon at standard parameters
        double force_constant =
            MaterialModelFactory::getForceConstant(crystal.material_type, crystal.lattice_constant);
        double expected_pure_zpe =
            calculatePureZPE(qft_params.hbar, qft_params.mass, force_constant);

        double pure_zpe_error =
            std::abs(results.mean_pure_zpe - expected_pure_zpe) / expected_pure_zpe * 100.0;

        std::cout << "  Expected Pure ZPE: " << std::scientific << expected_pure_zpe << " J\n";
        std::cout << "  Measured Pure ZPE: " << std::scientific << results.mean_pure_zpe << " J\n";
        std::cout << "  Error: " << std::fixed << std::setprecision(4) << pure_zpe_error << "%\n";

        if (pure_zpe_error < 1.0) {
            std::cout << "√ Pure ZPE calculation matches theoretical prediction within 1%\n";
        }
        else {
            std::cout << "⚠ Pure ZPE calculation deviates from theoretical prediction!\n";
        }
    }

    return 0;
}
