#ifndef RAD_ML_QUANTUM_MONTE_CARLO_HPP
#define RAD_ML_QUANTUM_MONTE_CARLO_HPP

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

#include "rad_ml/quantum/zpe_model.hpp"

namespace rad_ml {
namespace quantum {

/**
 * Parameters for quantum field theory calculations
 */
struct QFTParameters {
    double hbar = 1.054571817e-34;   // Reduced Planck's constant
    double c = 299792458.0;          // Speed of light
    double mass = 9.1093837015e-31;  // Electron mass
    double kb = 1.380649e-23;        // Boltzmann constant
};

/**
 * Parameters for a crystal lattice model
 */
struct CrystalParameters {
    double lattice_constant = 5.43e-10;  // Si lattice constant in meters
    MaterialModelFactory::MaterialType material_type = MaterialModelFactory::SILICON;
    double youngs_modulus = 1.3e11;  // Young's modulus in Pa
};

/**
 * Parameters for the Monte Carlo simulation
 */
struct MCSimulationParameters {
    size_t num_samples = 10000;
    size_t num_threads = std::thread::hardware_concurrency();
    double temp_min = 10.0;              // Minimum temperature in K
    double temp_max = 300.0;             // Maximum temperature in K
    double size_min = 2.0e-9;            // Minimum feature size in m (2 nm)
    double size_max = 50.0e-9;           // Maximum feature size in m (50 nm)
    double barrier_min = 0.1;            // Minimum barrier height in eV
    double barrier_max = 5.0;            // Maximum barrier height in eV
    bool use_detailed_zpe_model = true;  // Whether to use the refined ZPE model
    bool enable_env_effects = false;     // Whether to enable environmental effects
};

/**
 * Results from a Monte Carlo simulation
 */
struct MCSimulationResults {
    // Raw data vectors
    std::vector<double> temperatures;
    std::vector<double> feature_sizes;
    std::vector<double> barrier_heights;
    std::vector<double> kg_results;
    std::vector<double> tunneling_results;
    std::vector<double> zpe_results;

    // New refined model data vectors
    std::vector<double> pure_zpe_values;
    std::vector<double> thermal_quantum_values;

    // Statistical results
    double mean_kg = 0.0;
    double std_dev_kg = 0.0;
    double mean_tunneling = 0.0;
    double std_dev_tunneling = 0.0;
    double mean_zpe = 0.0;
    double std_dev_zpe = 0.0;

    // New refined model statistics
    double mean_pure_zpe = 0.0;
    double std_dev_pure_zpe = 0.0;
    double mean_thermal_quantum = 0.0;
    double std_dev_thermal_quantum = 0.0;

    // Correlation analysis
    double corr_kg_temp = 0.0;
    double corr_tunneling_barrier = 0.0;
    double corr_zpe_temp = 0.0;

    // New correlations for refined model
    double corr_pure_zpe_temp = 0.0;
    double corr_thermal_temp = 0.0;

    // Performance metrics
    double execution_time_seconds = 0.0;
    double samples_per_second = 0.0;
};

/**
 * Generates random parameters for a Monte Carlo simulation.
 *
 * @param params The simulation parameters containing ranges
 * @param gen Random number generator
 * @param temperature Output parameter for temperature
 * @param feature_size Output parameter for feature size
 * @param barrier_height Output parameter for barrier height
 */
inline void generateRandomParameters(const MCSimulationParameters& params, std::mt19937& gen,
                                     double& temperature, double& feature_size,
                                     double& barrier_height)
{
    std::uniform_real_distribution<double> temp_dist(params.temp_min, params.temp_max);
    std::uniform_real_distribution<double> size_dist(params.size_min, params.size_max);
    std::uniform_real_distribution<double> barrier_dist(params.barrier_min, params.barrier_max);

    temperature = temp_dist(gen);
    feature_size = size_dist(gen);
    barrier_height = barrier_dist(gen);
}

/**
 * Calculates the Klein-Gordon equation solution for given parameters.
 * This is a simplified model for demonstration purposes.
 */
inline double calculateKleinGordonSolution(double mass, double c, double feature_size)
{
    // Simplified K-G solution for demonstration
    double wave_number = M_PI / feature_size;
    double energy = std::sqrt(std::pow(mass * c * c, 2) + std::pow(c * wave_number, 2));
    return std::sin(wave_number * feature_size) / energy;
}

/**
 * Calculates quantum tunneling probability using WKB approximation.
 */
inline double calculateTunnelingProbability(double barrier_height, double feature_size, double mass,
                                            double hbar)
{
    // Convert barrier from eV to Joules
    double barrier_joules = barrier_height * 1.602176634e-19;

    // WKB approximation for rectangular barrier
    double exponent = -2.0 * feature_size * std::sqrt(2.0 * mass * barrier_joules) / hbar;
    return std::exp(exponent);
}

/**
 * Runs a batch of Monte Carlo simulations on a single thread.
 *
 * @param params Simulation parameters
 * @param qft_params Quantum field theory parameters
 * @param crystal Crystal parameters
 * @param thread_id ID of the current thread
 * @param start_index Starting index for this batch
 * @param end_index Ending index for this batch
 * @param temperatures Output vector for temperatures
 * @param feature_sizes Output vector for feature sizes
 * @param barrier_heights Output vector for barrier heights
 * @param kg_results Output vector for Klein-Gordon results
 * @param tunneling_results Output vector for tunneling results
 * @param zpe_results Output vector for ZPE results
 * @param pure_zpe_values Output vector for pure ZPE values
 * @param thermal_quantum_values Output vector for thermal quantum contributions
 */
inline void runMCBatch(const MCSimulationParameters& params, const QFTParameters& qft_params,
                       const CrystalParameters& crystal, int thread_id, size_t start_index,
                       size_t end_index, std::vector<double>& temperatures,
                       std::vector<double>& feature_sizes, std::vector<double>& barrier_heights,
                       std::vector<double>& kg_results, std::vector<double>& tunneling_results,
                       std::vector<double>& zpe_results, std::vector<double>& pure_zpe_values,
                       std::vector<double>& thermal_quantum_values)
{
    // Thread-local random generator with unique seed
    std::random_device rd;
    std::mt19937 gen(rd() + static_cast<unsigned int>(thread_id));

    // Thread-local storage for results
    std::vector<double> local_temperatures;
    std::vector<double> local_feature_sizes;
    std::vector<double> local_barrier_heights;
    std::vector<double> local_kg_results;
    std::vector<double> local_tunneling_results;
    std::vector<double> local_zpe_results;
    std::vector<double> local_pure_zpe_values;
    std::vector<double> local_thermal_quantum_values;

    // Reserve space for efficiency
    size_t batch_size = end_index - start_index;
    local_temperatures.reserve(batch_size);
    local_feature_sizes.reserve(batch_size);
    local_barrier_heights.reserve(batch_size);
    local_kg_results.reserve(batch_size);
    local_tunneling_results.reserve(batch_size);
    local_zpe_results.reserve(batch_size);
    local_pure_zpe_values.reserve(batch_size);
    local_thermal_quantum_values.reserve(batch_size);

    // Run simulations for this batch
    for (size_t i = start_index; i < end_index; ++i) {
        double temperature, feature_size, barrier_height;
        generateRandomParameters(params, gen, temperature, feature_size, barrier_height);

        // Store the parameters
        local_temperatures.push_back(temperature);
        local_feature_sizes.push_back(feature_size);
        local_barrier_heights.push_back(barrier_height);

        // Calculate quantum results
        double kg_result =
            calculateKleinGordonSolution(qft_params.mass, qft_params.c, feature_size);
        local_kg_results.push_back(kg_result);

        double tunneling_result = calculateTunnelingProbability(barrier_height, feature_size,
                                                                qft_params.mass, qft_params.hbar);
        local_tunneling_results.push_back(tunneling_result);

        // Calculate force constant from crystal parameters
        double force_constant =
            MaterialModelFactory::getForceConstant(crystal.material_type, crystal.lattice_constant);

        if (force_constant <= 0.0) {
            force_constant =
                calculateForceConstantFromLattice(crystal.lattice_constant, crystal.youngs_modulus);
        }

        // Calculate ZPE components with refined model
        double pure_zpe = calculatePureZPE(qft_params.hbar, qft_params.mass, force_constant);

        double thermal_quantum = calculateThermalQuantumCorrection(
            qft_params.hbar, qft_params.mass, force_constant, temperature, qft_params.kb);

        local_pure_zpe_values.push_back(pure_zpe);
        local_thermal_quantum_values.push_back(thermal_quantum);

        // Total ZPE contribution (for backward compatibility)
        double zpe_result = pure_zpe + thermal_quantum;
        local_zpe_results.push_back(zpe_result);
    }

    // Use a mutex to safely update the shared results
    static std::mutex results_mutex;
    {
        std::lock_guard<std::mutex> lock(results_mutex);
        temperatures.insert(temperatures.end(), local_temperatures.begin(),
                            local_temperatures.end());
        feature_sizes.insert(feature_sizes.end(), local_feature_sizes.begin(),
                             local_feature_sizes.end());
        barrier_heights.insert(barrier_heights.end(), local_barrier_heights.begin(),
                               local_barrier_heights.end());
        kg_results.insert(kg_results.end(), local_kg_results.begin(), local_kg_results.end());
        tunneling_results.insert(tunneling_results.end(), local_tunneling_results.begin(),
                                 local_tunneling_results.end());
        zpe_results.insert(zpe_results.end(), local_zpe_results.begin(), local_zpe_results.end());
        pure_zpe_values.insert(pure_zpe_values.end(), local_pure_zpe_values.begin(),
                               local_pure_zpe_values.end());
        thermal_quantum_values.insert(thermal_quantum_values.end(),
                                      local_thermal_quantum_values.begin(),
                                      local_thermal_quantum_values.end());
    }
}

/**
 * Calculate the Pearson correlation coefficient between two vectors.
 */
inline double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y)
{
    if (x.size() != y.size() || x.empty()) {
        return 0.0;
    }

    double sum_x = std::accumulate(x.begin(), x.end(), 0.0);
    double sum_y = std::accumulate(y.begin(), y.end(), 0.0);

    double mean_x = sum_x / x.size();
    double mean_y = sum_y / y.size();

    double numerator = 0.0;
    double denominator_x = 0.0;
    double denominator_y = 0.0;

    for (size_t i = 0; i < x.size(); ++i) {
        double x_diff = x[i] - mean_x;
        double y_diff = y[i] - mean_y;
        numerator += x_diff * y_diff;
        denominator_x += x_diff * x_diff;
        denominator_y += y_diff * y_diff;
    }

    if (denominator_x <= 0.0 || denominator_y <= 0.0) {
        return 0.0;
    }

    return numerator / std::sqrt(denominator_x * denominator_y);
}

/**
 * Calculate statistical metrics for the simulation results.
 */
inline void calculateStatistics(MCSimulationResults& results)
{
    // Calculate means
    results.mean_kg = std::accumulate(results.kg_results.begin(), results.kg_results.end(), 0.0) /
                      results.kg_results.size();
    results.mean_tunneling =
        std::accumulate(results.tunneling_results.begin(), results.tunneling_results.end(), 0.0) /
        results.tunneling_results.size();
    results.mean_zpe =
        std::accumulate(results.zpe_results.begin(), results.zpe_results.end(), 0.0) /
        results.zpe_results.size();
    results.mean_pure_zpe =
        std::accumulate(results.pure_zpe_values.begin(), results.pure_zpe_values.end(), 0.0) /
        results.pure_zpe_values.size();
    results.mean_thermal_quantum = std::accumulate(results.thermal_quantum_values.begin(),
                                                   results.thermal_quantum_values.end(), 0.0) /
                                   results.thermal_quantum_values.size();

    // Calculate standard deviations
    double kg_var = 0.0, tunneling_var = 0.0, zpe_var = 0.0, pure_zpe_var = 0.0, thermal_var = 0.0;

    for (double val : results.kg_results) {
        kg_var += std::pow(val - results.mean_kg, 2);
    }
    results.std_dev_kg = std::sqrt(kg_var / results.kg_results.size());

    for (double val : results.tunneling_results) {
        tunneling_var += std::pow(val - results.mean_tunneling, 2);
    }
    results.std_dev_tunneling = std::sqrt(tunneling_var / results.tunneling_results.size());

    for (double val : results.zpe_results) {
        zpe_var += std::pow(val - results.mean_zpe, 2);
    }
    results.std_dev_zpe = std::sqrt(zpe_var / results.zpe_results.size());

    for (double val : results.pure_zpe_values) {
        pure_zpe_var += std::pow(val - results.mean_pure_zpe, 2);
    }
    results.std_dev_pure_zpe = std::sqrt(pure_zpe_var / results.pure_zpe_values.size());

    for (double val : results.thermal_quantum_values) {
        thermal_var += std::pow(val - results.mean_thermal_quantum, 2);
    }
    results.std_dev_thermal_quantum =
        std::sqrt(thermal_var / results.thermal_quantum_values.size());
}

/**
 * Analyze parameter correlations in the simulation results.
 */
inline void analyzeParameterCorrelations(MCSimulationResults& results)
{
    results.corr_kg_temp = calculateCorrelation(results.temperatures, results.kg_results);
    results.corr_tunneling_barrier =
        calculateCorrelation(results.barrier_heights, results.tunneling_results);
    results.corr_zpe_temp = calculateCorrelation(results.temperatures, results.zpe_results);

    // New correlations for the refined model
    results.corr_pure_zpe_temp =
        calculateCorrelation(results.temperatures, results.pure_zpe_values);
    results.corr_thermal_temp =
        calculateCorrelation(results.temperatures, results.thermal_quantum_values);
}

/**
 * Run a Monte Carlo simulation with the given parameters.
 *
 * @param params Simulation parameters
 * @param qft_params Quantum field theory parameters (optional)
 * @param crystal Crystal parameters (optional)
 * @return Simulation results
 */
inline MCSimulationResults runMonteCarloSimulation(
    const MCSimulationParameters& params, const QFTParameters& qft_params = QFTParameters{},
    const CrystalParameters& crystal = CrystalParameters{})
{
    MCSimulationResults results;

    // Prepare result vectors
    results.temperatures.reserve(params.num_samples);
    results.feature_sizes.reserve(params.num_samples);
    results.barrier_heights.reserve(params.num_samples);
    results.kg_results.reserve(params.num_samples);
    results.tunneling_results.reserve(params.num_samples);
    results.zpe_results.reserve(params.num_samples);
    results.pure_zpe_values.reserve(params.num_samples);
    results.thermal_quantum_values.reserve(params.num_samples);

    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();

    // Fix the type mismatch by making both arguments the same type (size_t)
    size_t hw_threads = static_cast<size_t>(std::thread::hardware_concurrency());
    size_t num_threads = params.num_threads < hw_threads ? params.num_threads : hw_threads;

    size_t batch_size = params.num_samples / num_threads;

    std::vector<std::thread> workers;
    workers.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start_index = i * batch_size;
        size_t end_index = (i == num_threads - 1) ? params.num_samples : (i + 1) * batch_size;

        workers.emplace_back(runMCBatch, std::ref(params), std::ref(qft_params), std::ref(crystal),
                             static_cast<int>(i), start_index, end_index,
                             std::ref(results.temperatures), std::ref(results.feature_sizes),
                             std::ref(results.barrier_heights), std::ref(results.kg_results),
                             std::ref(results.tunneling_results), std::ref(results.zpe_results),
                             std::ref(results.pure_zpe_values),
                             std::ref(results.thermal_quantum_values));
    }

    // Wait for all threads to complete
    for (auto& worker : workers) {
        worker.join();
    }

    // Calculate execution time
    auto end_time = std::chrono::high_resolution_clock::now();
    results.execution_time_seconds = std::chrono::duration<double>(end_time - start_time).count();
    results.samples_per_second = params.num_samples / results.execution_time_seconds;

    // Calculate statistics and correlations
    calculateStatistics(results);
    analyzeParameterCorrelations(results);

    return results;
}

/**
 * Print a summary of the Monte Carlo simulation results to the console.
 */
inline void printSimulationSummary(const MCSimulationParameters& params,
                                   const MCSimulationResults& results)
{
    std::cout << "\nMonte Carlo Simulation Complete\n";
    std::cout << "===============================\n";
    std::cout << "Klein-Gordon Equation Results:\n";
    std::cout << "  Mean: " << results.mean_kg << "\n";
    std::cout << "  Std Dev: " << results.std_dev_kg << "\n";
    std::cout << "Quantum Tunneling Probability Results:\n";
    std::cout << "  Mean: " << results.mean_tunneling << "\n";
    std::cout << "  Std Dev: " << results.std_dev_tunneling << "\n";

    std::cout << "\nZero-Point Energy Analysis:\n";
    std::cout << "  Total Quantum Contribution:\n";
    std::cout << "    Mean: " << results.mean_zpe << "\n";
    std::cout << "    Std Dev: " << results.std_dev_zpe << "\n";

    if (params.use_detailed_zpe_model) {
        std::cout << "  Pure ZPE (Temperature-Independent):\n";
        std::cout << "    Mean: " << results.mean_pure_zpe << "\n";
        std::cout << "    Std Dev: " << results.std_dev_pure_zpe << "\n";
        std::cout << "  Thermal Quantum Correction:\n";
        std::cout << "    Mean: " << results.mean_thermal_quantum << "\n";
        std::cout << "    Std Dev: " << results.std_dev_thermal_quantum << "\n";
    }

    std::cout << "\nParameter Correlation Analysis:\n";
    std::cout << "-------------------------------\n";
    std::cout << "Correlation between Klein-Gordon solution and temperature: "
              << results.corr_kg_temp << "\n";
    std::cout << "Correlation between tunneling probability and barrier height: "
              << results.corr_tunneling_barrier << "\n";
    std::cout << "Correlation between total quantum contribution and temperature: "
              << results.corr_zpe_temp << "\n";

    if (params.use_detailed_zpe_model) {
        std::cout << "Correlation between pure ZPE and temperature: " << results.corr_pure_zpe_temp
                  << "\n";
        std::cout << "Correlation between thermal quantum correction and temperature: "
                  << results.corr_thermal_temp << "\n";
    }

    std::cout << "\nTotal execution time: " << results.execution_time_seconds << " seconds\n";
    std::cout << "Samples per second: " << results.samples_per_second << "\n";
}

}  // namespace quantum
}  // namespace rad_ml

#endif  // RAD_ML_QUANTUM_MONTE_CARLO_HPP
