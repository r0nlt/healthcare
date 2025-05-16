/**
 * Monte Carlo Test for Wave Equation Solver
 *
 * This file implements a parallel Monte Carlo test for the wave equation
 * solver in the rad-tolerant-ml project.
 */

#include <atomic>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <rad_ml/physics/quantum_field_theory.hpp>
#include <rad_ml/physics/quantum_integration.hpp>
#include <rad_ml/physics/quantum_models.hpp>
#include <random>
#include <string>
#include <thread>
#include <vector>

using namespace rad_ml::physics;

// Structure to hold Monte Carlo simulation parameters
struct MCSimulationParams {
    int num_samples = 10000;
    int num_threads = std::thread::hardware_concurrency();
    double temperature_min = 10.0;
    double temperature_max = 300.0;
    double feature_size_min = 2.0;
    double feature_size_max = 50.0;
    double barrier_height_min = 0.1;
    double barrier_height_max = 5.0;
};

// Structure to hold Monte Carlo simulation results
struct MCSimulationResults {
    std::vector<double> kg_values;
    std::vector<double> tunneling_values;
    std::vector<double> zpe_values;
    std::vector<double> parameters;
    double mean_kg = 0.0;
    double std_dev_kg = 0.0;
    double mean_tunneling = 0.0;
    double std_dev_tunneling = 0.0;
    double mean_zpe = 0.0;
    double std_dev_zpe = 0.0;
};

// Function to generate random parameters for the simulation
std::vector<double> generateRandomParameters(std::mt19937& gen, const MCSimulationParams& params)
{
    std::uniform_real_distribution<double> temp_dist(params.temperature_min,
                                                     params.temperature_max);
    std::uniform_real_distribution<double> size_dist(params.feature_size_min,
                                                     params.feature_size_max);
    std::uniform_real_distribution<double> barrier_dist(params.barrier_height_min,
                                                        params.barrier_height_max);

    std::vector<double> result(3);
    result[0] = temp_dist(gen);     // temperature
    result[1] = size_dist(gen);     // feature size
    result[2] = barrier_dist(gen);  // barrier height

    return result;
}

// Function to run a batch of Monte Carlo simulations in a worker thread
void runMCBatch(int start_idx, int end_idx, const MCSimulationParams& params,
                MCSimulationResults& results, std::mutex& results_mutex)
{
    // Create thread-local random number generator with unique seed
    unsigned int seed = static_cast<unsigned int>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count() + start_idx);
    std::mt19937 gen(seed);

    // Thread-local storage for results
    std::vector<double> local_kg_values;
    std::vector<double> local_tunneling_values;
    std::vector<double> local_zpe_values;
    std::vector<double> local_parameters;

    // Reserve space to avoid reallocations
    local_kg_values.reserve(end_idx - start_idx);
    local_tunneling_values.reserve(end_idx - start_idx);
    local_zpe_values.reserve(end_idx - start_idx);
    local_parameters.reserve(3 * (end_idx - start_idx));

    // Run simulations for this batch
    for (int i = start_idx; i < end_idx; ++i) {
        // Generate random parameters
        std::vector<double> rand_params = generateRandomParameters(gen, params);
        double temperature = rand_params[0];
        double feature_size = rand_params[1];
        double barrier_height = rand_params[2];

        // Create crystal and QFT parameters
        CrystalLattice crystal(CrystalLattice::DIAMOND, 5.43, barrier_height);
        QFTParameters qft_params = createQFTParameters(crystal, feature_size);

        // Run the wave equation solvers
        double kg_result = solveKleinGordonEquation(
            qft_params.hbar, qft_params.mass, qft_params.potential_coefficient,
            qft_params.coupling_constant, qft_params.lattice_spacing, qft_params.time_step);

        double tunneling_result = calculateQuantumTunnelingProbability(
            barrier_height, qft_params.mass, qft_params.hbar, temperature);

        double zpe_result = calculateZeroPointEnergyContribution(
            qft_params.hbar, qft_params.mass, crystal.lattice_constant, temperature);

        // Store results
        local_kg_values.push_back(kg_result);
        local_tunneling_values.push_back(tunneling_result);
        local_zpe_values.push_back(zpe_result);

        // Store parameters for later analysis
        local_parameters.push_back(temperature);
        local_parameters.push_back(feature_size);
        local_parameters.push_back(barrier_height);
    }

    // Lock and update the global results
    {
        std::lock_guard<std::mutex> lock(results_mutex);
        results.kg_values.insert(results.kg_values.end(), local_kg_values.begin(),
                                 local_kg_values.end());
        results.tunneling_values.insert(results.tunneling_values.end(),
                                        local_tunneling_values.begin(),
                                        local_tunneling_values.end());
        results.zpe_values.insert(results.zpe_values.end(), local_zpe_values.begin(),
                                  local_zpe_values.end());
        results.parameters.insert(results.parameters.end(), local_parameters.begin(),
                                  local_parameters.end());
    }
}

// Function to calculate statistics for the Monte Carlo results
void calculateStatistics(MCSimulationResults& results)
{
    // Calculate means
    double sum_kg = 0.0, sum_tunneling = 0.0, sum_zpe = 0.0;
    for (size_t i = 0; i < results.kg_values.size(); ++i) {
        sum_kg += results.kg_values[i];
        sum_tunneling += results.tunneling_values[i];
        sum_zpe += results.zpe_values[i];
    }

    results.mean_kg = sum_kg / results.kg_values.size();
    results.mean_tunneling = sum_tunneling / results.tunneling_values.size();
    results.mean_zpe = sum_zpe / results.zpe_values.size();

    // Calculate standard deviations
    double sum_sq_kg = 0.0, sum_sq_tunneling = 0.0, sum_sq_zpe = 0.0;
    for (size_t i = 0; i < results.kg_values.size(); ++i) {
        sum_sq_kg +=
            (results.kg_values[i] - results.mean_kg) * (results.kg_values[i] - results.mean_kg);
        sum_sq_tunneling += (results.tunneling_values[i] - results.mean_tunneling) *
                            (results.tunneling_values[i] - results.mean_tunneling);
        sum_sq_zpe +=
            (results.zpe_values[i] - results.mean_zpe) * (results.zpe_values[i] - results.mean_zpe);
    }

    results.std_dev_kg = std::sqrt(sum_sq_kg / results.kg_values.size());
    results.std_dev_tunneling = std::sqrt(sum_sq_tunneling / results.tunneling_values.size());
    results.std_dev_zpe = std::sqrt(sum_sq_zpe / results.zpe_values.size());
}

// Function to analyze correlations between parameters and results
void analyzeParameterCorrelations(const MCSimulationResults& results)
{
    std::cout << "Parameter Correlation Analysis:\n";
    std::cout << "-------------------------------\n";

    // Simple correlation coefficients
    size_t n = results.kg_values.size();

    // For Klein-Gordon equation vs temperature
    double sum_kg_temp = 0.0, sum_kg = 0.0, sum_temp = 0.0, sum_sq_kg = 0.0, sum_sq_temp = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double temp = results.parameters[i * 3];
        double kg = results.kg_values[i];

        sum_kg_temp += kg * temp;
        sum_kg += kg;
        sum_temp += temp;
        sum_sq_kg += kg * kg;
        sum_sq_temp += temp * temp;
    }

    double correlation_kg_temp =
        (n * sum_kg_temp - sum_kg * sum_temp) /
        std::sqrt((n * sum_sq_kg - sum_kg * sum_kg) * (n * sum_sq_temp - sum_temp * sum_temp));

    std::cout << "Correlation between Klein-Gordon solution and temperature: "
              << correlation_kg_temp << std::endl;

    // For tunneling probability vs barrier height
    double sum_tun_barrier = 0.0, sum_tun = 0.0, sum_barrier = 0.0;
    double sum_sq_tun = 0.0, sum_sq_barrier = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double barrier = results.parameters[i * 3 + 2];
        double tun = results.tunneling_values[i];

        sum_tun_barrier += tun * barrier;
        sum_tun += tun;
        sum_barrier += barrier;
        sum_sq_tun += tun * tun;
        sum_sq_barrier += barrier * barrier;
    }

    double correlation_tun_barrier = (n * sum_tun_barrier - sum_tun * sum_barrier) /
                                     std::sqrt((n * sum_sq_tun - sum_tun * sum_tun) *
                                               (n * sum_sq_barrier - sum_barrier * sum_barrier));

    std::cout << "Correlation between tunneling probability and barrier height: "
              << correlation_tun_barrier << std::endl;

    // For ZPE contribution vs temperature
    double sum_zpe_temp = 0.0, sum_zpe = 0.0, sum_sq_zpe = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double temp = results.parameters[i * 3];
        double zpe = results.zpe_values[i];

        sum_zpe_temp += zpe * temp;
        sum_zpe += zpe;
        sum_sq_zpe += zpe * zpe;
    }

    double correlation_zpe_temp =
        (n * sum_zpe_temp - sum_zpe * sum_temp) /
        std::sqrt((n * sum_sq_zpe - sum_zpe * sum_zpe) * (n * sum_sq_temp - sum_temp * sum_temp));

    std::cout << "Correlation between zero-point energy and temperature: " << correlation_zpe_temp
              << std::endl;
}

#ifndef QUANTUM_WAVE_MC_TEST_NO_MAIN
// Main function to run the Monte Carlo simulation
int main(int argc, char** argv)
{
    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();

    // Parse command line arguments
    MCSimulationParams params;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--samples" && i + 1 < argc) {
            params.num_samples = std::stoi(argv[++i]);
        }
        else if (arg == "--threads" && i + 1 < argc) {
            params.num_threads = std::stoi(argv[++i]);
        }
        else if (arg == "--temp-min" && i + 1 < argc) {
            params.temperature_min = std::stod(argv[++i]);
        }
        else if (arg == "--temp-max" && i + 1 < argc) {
            params.temperature_max = std::stod(argv[++i]);
        }
        else if (arg == "--size-min" && i + 1 < argc) {
            params.feature_size_min = std::stod(argv[++i]);
        }
        else if (arg == "--size-max" && i + 1 < argc) {
            params.feature_size_max = std::stod(argv[++i]);
        }
        else if (arg == "--barrier-min" && i + 1 < argc) {
            params.barrier_height_min = std::stod(argv[++i]);
        }
        else if (arg == "--barrier-max" && i + 1 < argc) {
            params.barrier_height_max = std::stod(argv[++i]);
        }
        else if (arg == "--help") {
            std::cout << "Monte Carlo Test for Wave Equation Solver\n";
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "  --samples N       Number of Monte Carlo samples (default: 10000)\n";
            std::cout
                << "  --threads N       Number of threads to use (default: hardware concurrency)\n";
            std::cout << "  --temp-min X      Minimum temperature in K (default: 10)\n";
            std::cout << "  --temp-max X      Maximum temperature in K (default: 300)\n";
            std::cout << "  --size-min X      Minimum feature size in nm (default: 2)\n";
            std::cout << "  --size-max X      Maximum feature size in nm (default: 50)\n";
            std::cout << "  --barrier-min X   Minimum barrier height in eV (default: 0.1)\n";
            std::cout << "  --barrier-max X   Maximum barrier height in eV (default: 5.0)\n";
            std::cout << "  --help            Display this help message\n";
            return 0;
        }
    }

    // Cap the number of threads to something reasonable
    params.num_threads = std::min(params.num_threads, 32);

    // Print simulation parameters
    std::cout << "Monte Carlo Test for Wave Equation Solver\n";
    std::cout << "=======================================\n";
    std::cout << "Number of samples: " << params.num_samples << std::endl;
    std::cout << "Number of threads: " << params.num_threads << std::endl;
    std::cout << "Temperature range: [" << params.temperature_min << ", " << params.temperature_max
              << "] K\n";
    std::cout << "Feature size range: [" << params.feature_size_min << ", "
              << params.feature_size_max << "] nm\n";
    std::cout << "Barrier height range: [" << params.barrier_height_min << ", "
              << params.barrier_height_max << "] eV\n";
    std::cout << "---------------------------------------\n";

    // Initialize results
    MCSimulationResults results;
    results.kg_values.reserve(params.num_samples);
    results.tunneling_values.reserve(params.num_samples);
    results.zpe_values.reserve(params.num_samples);
    results.parameters.reserve(3 * params.num_samples);

    // Create and launch worker threads
    std::vector<std::thread> threads;
    std::mutex results_mutex;

    int samples_per_thread = params.num_samples / params.num_threads;

    std::cout << "Launching " << params.num_threads << " worker threads...\n";
    for (int i = 0; i < params.num_threads; ++i) {
        int start_idx = i * samples_per_thread;
        int end_idx =
            (i == params.num_threads - 1) ? params.num_samples : (i + 1) * samples_per_thread;

        threads.emplace_back(runMCBatch, start_idx, end_idx, std::ref(params), std::ref(results),
                             std::ref(results_mutex));
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // Calculate statistics
    calculateStatistics(results);

    // Print results
    std::cout << "Monte Carlo Simulation Complete\n";
    std::cout << "===============================\n";
    std::cout << "Klein-Gordon Equation Results:\n";
    std::cout << "  Mean: " << results.mean_kg << std::endl;
    std::cout << "  Std Dev: " << results.std_dev_kg << std::endl;

    std::cout << "Quantum Tunneling Probability Results:\n";
    std::cout << "  Mean: " << results.mean_tunneling << std::endl;
    std::cout << "  Std Dev: " << results.std_dev_tunneling << std::endl;

    std::cout << "Zero-Point Energy Contribution Results:\n";
    std::cout << "  Mean: " << results.mean_zpe << std::endl;
    std::cout << "  Std Dev: " << results.std_dev_zpe << std::endl;

    // Analyze parameter correlations
    analyzeParameterCorrelations(results);

    // Print timing information
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Total execution time: " << duration.count() / 1000.0 << " seconds\n";
    std::cout << "Samples per second: " << params.num_samples / (duration.count() / 1000.0)
              << std::endl;

    return 0;
}
#endif
