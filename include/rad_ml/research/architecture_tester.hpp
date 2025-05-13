// File: include/rad_ml/research/architecture_tester.hpp

#pragma once

#include <rad_ml/neural/protected_neural_network.hpp>
#include <rad_ml/sim/environment.hpp>
#include <rad_ml/neural/multi_bit_protection.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <memory>

namespace rad_ml {
namespace research {

/**
 * Structure to hold test results for a particular architecture
 */
struct ArchitectureTestResult {
    // Architecture details
    std::vector<size_t> layer_sizes;
    double dropout_rate;
    bool has_residual_connections;
    neural::ProtectionLevel protection_level;
    
    // Environment details
    sim::Environment environment;
    
    // Performance metrics
    double baseline_accuracy;
    double radiation_accuracy;
    double accuracy_preservation;
    double execution_time_ms;
    
    // Error statistics
    size_t errors_detected;
    size_t errors_corrected;
    size_t uncorrectable_errors;
    
    // Monte Carlo statistics
    double baseline_accuracy_stddev = 0.0;  // Standard deviation of baseline accuracy
    double radiation_accuracy_stddev = 0.0; // Standard deviation of radiation accuracy
    double accuracy_preservation_stddev = 0.0; // Standard deviation of preservation
    size_t monte_carlo_trials = 1;          // Number of Monte Carlo trials performed
    
    // Constructor for single test results
    ArchitectureTestResult() = default;
    
    // Constructor for Monte Carlo aggregated results
    ArchitectureTestResult(
        const std::vector<size_t>& sizes,
        double dropout,
        bool residual,
        neural::ProtectionLevel protection,
        sim::Environment env,
        double base_acc, double base_stddev,
        double rad_acc, double rad_stddev,
        double preservation, double preservation_stddev,
        double exec_time,
        size_t errors_det, size_t errors_corr, size_t uncorr_errors,
        size_t num_trials = 1
    ) : layer_sizes(sizes),
        dropout_rate(dropout),
        has_residual_connections(residual),
        protection_level(protection),
        environment(env),
        baseline_accuracy(base_acc),
        radiation_accuracy(rad_acc),
        accuracy_preservation(preservation),
        execution_time_ms(exec_time),
        errors_detected(errors_det),
        errors_corrected(errors_corr),
        uncorrectable_errors(uncorr_errors),
        baseline_accuracy_stddev(base_stddev),
        radiation_accuracy_stddev(rad_stddev),
        accuracy_preservation_stddev(preservation_stddev),
        monte_carlo_trials(num_trials) {}
};

/**
 * Class for systematic testing of neural network architectures
 * under radiation conditions
 */
class ArchitectureTester {
public:
    /**
     * Constructor with dataset and output file
     */
    ArchitectureTester(
        const std::vector<float>& train_data,
        const std::vector<float>& train_labels,
        const std::vector<float>& test_data,
        const std::vector<float>& test_labels,
        size_t input_size,
        size_t output_size,
        const std::string& results_file);
    
    /**
     * Test networks with different widths
     */
    void testWidthRange(
        int min_width, 
        int max_width, 
        int width_step,
        double dropout_rate = 0.5,
        int epochs = 100,
        sim::Environment env = sim::Environment::MARS);
    
    /**
     * Test networks with different dropout rates
     */
    void testDropoutRange(
        const std::vector<int>& architecture,
        double min_dropout, 
        double max_dropout, 
        double dropout_step,
        int epochs = 100,
        sim::Environment env = sim::Environment::MARS);
    
    /**
     * Test a specific network architecture
     */
    ArchitectureTestResult testArchitecture(
        const std::vector<size_t>& architecture,
        double dropout_rate,
        bool use_residual_connections,
        neural::ProtectionLevel protection_level,
        int epochs,
        sim::Environment env,
        unsigned int trial_num = 0);
    
    /**
     * Test a specific network architecture using Monte Carlo method
     * @param architecture Layer sizes of the neural network
     * @param dropout_rate Dropout rate to use
     * @param use_residual_connections Whether to use residual connections
     * @param protection_level Protection level to use
     * @param epochs Number of training epochs
     * @param env Radiation environment to test in
     * @param num_trials Number of Monte Carlo trials to run
     * @param seed_offset Starting seed offset for random number generation
     * @return Aggregated test results with statistical data
     */
    ArchitectureTestResult testArchitectureMonteCarlo(
        const std::vector<size_t>& architecture,
        double dropout_rate,
        bool use_residual_connections,
        neural::ProtectionLevel protection_level,
        int epochs,
        sim::Environment env,
        size_t num_trials = 50,
        unsigned int seed_offset = 0);
    
    /**
     * Get the best architecture for a specific environment
     */
    std::vector<size_t> getBestArchitecture(sim::Environment env) const;
    
    /**
     * Get the optimal dropout rate for a specific environment
     */
    double getOptimalDropout(sim::Environment env) const;
    
    /**
     * Visualize test results
     */
    void visualizeResults(const std::string& output_file = "architecture_comparison.png") const;

private:
    // Dataset fields
    std::vector<float> train_data_;
    std::vector<float> train_labels_;
    std::vector<float> test_data_;
    std::vector<float> test_labels_;
    size_t input_size_;
    size_t output_size_;
    std::string results_file_;
    
    // Store test results
    std::vector<ArchitectureTestResult> results_;
    
    // Train a network with the given architecture
    void trainNetwork(
        neural::ProtectedNeuralNetwork<float>& network,
        int epochs);
    
    // Evaluate network under radiation
    ArchitectureTestResult evaluateNetwork(
        neural::ProtectedNeuralNetwork<float>& network,
        const std::vector<size_t>& architecture,
        double dropout_rate,
        bool has_residual,
        neural::ProtectionLevel protection_level,
        sim::Environment env);
    
    // Calculate statistical results from multiple Monte Carlo trials
    ArchitectureTestResult calculateMonteCarloStatistics(
        const std::vector<ArchitectureTestResult>& trial_results);
    
    // Save results to CSV
    void saveResultsToFile();
    
    // Convert vectors to appropriate format for training
    std::vector<std::vector<float>> prepareTrainingData() const;
    std::vector<std::vector<float>> prepareTrainingLabels() const;
    std::vector<std::vector<float>> prepareTestData() const;
    std::vector<std::vector<float>> prepareTestLabels() const;
};

} // namespace research
} // namespace rad_ml 