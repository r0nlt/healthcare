/**
 * @file qft_enhanced_network_example.cpp
 * @brief Example usage of QFT-enhanced neural network
 *
 * This example demonstrates how to use the QFT-enhanced neural network
 * with various radiation environments.
 */

#include <iostream>
#include <memory>
#include <rad_ml/crossdomain/qft_bridge.hpp>
#include <rad_ml/neural/protected_neural_network_mock.hpp>
#include <rad_ml/neural/qft_enhanced_network.hpp>
#include <rad_ml/neural/radiation_environment.hpp>
#include <rad_ml/physics/quantum_integration.hpp>
#include <random>
#include <string>
#include <vector>

// Helper functions implementation
namespace rad_ml {
namespace neural {

/**
 * @brief Generate synthetic data for testing
 *
 * @param inputs Output vector for inputs
 * @param targets Output vector for targets
 * @param count Number of samples to generate
 * @param input_size Size of input vector
 * @param output_size Size of output vector
 */
void generateSyntheticData(std::vector<std::vector<float>>& inputs,
                           std::vector<std::vector<float>>& targets, size_t count,
                           size_t input_size, size_t output_size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    inputs.resize(count);
    targets.resize(count);

    for (size_t i = 0; i < count; ++i) {
        // Generate random input
        inputs[i].resize(input_size);
        for (size_t j = 0; j < input_size; ++j) {
            inputs[i][j] = dist(gen);
        }

        // Generate target (simple function of inputs for demonstration)
        targets[i].resize(output_size);

        // Use first input elements to influence outputs
        size_t use_elements = std::min(input_size, output_size);

        for (size_t j = 0; j < output_size; ++j) {
            targets[i][j] = 0.0f;

            if (j < use_elements) {
                // Apply simple nonlinear transformation
                targets[i][j] = std::tanh(inputs[i][j] * 1.5f) * 0.5f + 0.5f;
            }
            else {
                // Apply a combination of input elements
                for (size_t k = 0; k < use_elements; ++k) {
                    targets[i][j] += std::sin(inputs[i][k] * (j + 1)) * (1.0f / use_elements);
                }
                targets[i][j] = targets[i][j] * 0.5f + 0.5f;
            }
        }
    }
}

/**
 * @brief Test network accuracy at a specific radiation level
 *
 * @param network Network to test
 * @param inputs Test inputs
 * @param targets Test targets
 * @param radiation_level Radiation level in Gy/s
 * @param iterations Number of iterations to average over
 * @return Accuracy (0.0-1.0)
 */
template <typename T>
float testNetworkAccuracy(const QFTEnhancedNetwork<T>& network,
                          const std::vector<std::vector<T>>& inputs,
                          const std::vector<std::vector<T>>& targets, double radiation_level,
                          int iterations)
{
    if (inputs.empty() || targets.empty() || inputs.size() != targets.size()) {
        return 0.0f;
    }

    float total_accuracy = 0.0f;

    for (int iter = 0; iter < iterations; ++iter) {
        size_t correct = 0;

        for (size_t i = 0; i < inputs.size(); ++i) {
            // Forward pass with radiation effects
            std::vector<T> output = network.forward(inputs[i], radiation_level);

            // Check if prediction is correct (thresholded for classification)
            bool all_correct = true;
            for (size_t j = 0; j < output.size(); ++j) {
                // Simple threshold-based accuracy for demonstration
                bool target_high = targets[i][j] > 0.5;
                bool output_high = output[j] > 0.5;

                if (target_high != output_high) {
                    all_correct = false;
                    break;
                }
            }

            if (all_correct) {
                correct++;
            }
        }

        float accuracy = static_cast<float>(correct) / inputs.size();
        total_accuracy += accuracy;
    }

    return total_accuracy / iterations;
}

// Explicit instantiation of template function
template float testNetworkAccuracy<float>(const QFTEnhancedNetwork<float>& network,
                                          const std::vector<std::vector<float>>& inputs,
                                          const std::vector<std::vector<float>>& targets,
                                          double radiation_level, int iterations);

}  // namespace neural
}  // namespace rad_ml

// Forward declaration of demonstration function
void demonstrateQFTEnhancedNetwork(const rad_ml::neural::RadiationEnvironment& environment,
                                   double dose_rate, double training_duration, int test_iterations);

/**
 * @brief Demonstrate QFT-enhanced neural network with radiation effects
 *
 * This function shows how to use the QFT-enhanced neural network in a
 * radiation environment with different doses.
 *
 * @param environment Radiation environment to simulate
 * @param dose_rate Radiation dose rate in Gy/s
 * @param training_duration Duration of training in seconds
 * @param test_iterations Number of test iterations
 */
void demonstrateQFTEnhancedNetwork(const rad_ml::neural::RadiationEnvironment& environment,
                                   double dose_rate, double training_duration, int test_iterations)
{
    std::cout << "Demonstrating QFT-Enhanced Neural Network\n";
    std::cout << "Environment: " << environment.getMissionName() << "\n";
    std::cout << "Dose rate: " << dose_rate << " Gy/s\n";

    // Create network architecture
    std::vector<size_t> layer_sizes = {10, 20, 15, 5};

    // Create models with different protection levels
    rad_ml::neural::ProtectedNeuralNetwork_mock<float> semiconductor_model(
        layer_sizes, rad_ml::neural::ProtectionLevel::FULL_TMR);
    rad_ml::neural::QFTEnhancedNetwork<float> space_model(
        layer_sizes, rad_ml::neural::ProtectionLevel::ADAPTIVE_TMR);

    // Set biological parameters
    rad_ml::crossdomain::BiologicalParameters bio_params;
    bio_params.water_content = 0.7;
    bio_params.radiosensitivity = 1.2;
    bio_params.repair_rate = 0.3;
    bio_params.temperature = 310.0;

    space_model.setBiologicalParameters(bio_params);

    // Calculate total radiation dose
    double total_dose = dose_rate * training_duration;

    // Generate synthetic training data
    std::vector<std::vector<float>> train_data;
    std::vector<std::vector<float>> train_targets;
    rad_ml::neural::generateSyntheticData(train_data, train_targets, 500, layer_sizes.front(),
                                          layer_sizes.back());

    // Generate test data
    std::vector<std::vector<float>> test_data;
    std::vector<std::vector<float>> test_targets;
    rad_ml::neural::generateSyntheticData(test_data, test_targets, 100, layer_sizes.front(),
                                          layer_sizes.back());

    // Initialize with radiation-hardened weights
    space_model.initializeRadiationHardenedWeights(bio_params);

    // Train the network with QFT enhancements
    std::cout << "Training QFT-enhanced network with radiation dose: " << total_dose << " Gy\n";
    float final_loss = space_model.trainWithQFT(train_data, train_targets,
                                                0.001f,     // learning rate
                                                100,        // epochs
                                                32,         // batch size
                                                total_dose  // radiation dose
    );

    std::cout << "Final loss: " << final_loss << "\n";

    // Test the network at different radiation levels
    std::cout << "\nTesting with different radiation levels:\n";

    std::vector<double> radiation_levels = {0.0, 0.1, 0.5, 1.0, 2.0, 5.0};

    for (double level : radiation_levels) {
        float accuracy = rad_ml::neural::testNetworkAccuracy(space_model, test_data, test_targets,
                                                             level, test_iterations);
        std::cout << "Radiation level: " << level << " Gy/s, Accuracy: " << (accuracy * 100.0f)
                  << "%\n";
    }

    // Demonstrate cross-domain knowledge distillation
    std::cout << "\nPerforming cross-domain knowledge distillation...\n";

    // Train semiconductor model on original data
    semiconductor_model.train(train_data, train_targets, 0.001f, 50, 32);

    // Distill knowledge to biological domain
    double distillation_loss =
        space_model.distillSemiconductorKnowledgeToBiological(semiconductor_model, test_data);

    std::cout << "Distillation loss: " << distillation_loss << "\n";

    // Test distilled model
    float distilled_accuracy = rad_ml::neural::testNetworkAccuracy(
        space_model, test_data, test_targets, 1.0, test_iterations);

    std::cout << "Post-distillation accuracy at 1.0 Gy/s: " << (distilled_accuracy * 100.0f)
              << "%\n";

    std::cout << "QFT-Enhanced Neural Network demonstration complete.\n";
}

int main(int argc, char* argv[])
{
    std::cout << "QFT-Enhanced Neural Network Example\n";
    std::cout << "===================================\n\n";

    // Select radiation environment (default: LEO_POLAR)
    rad_ml::neural::SpaceMission mission = rad_ml::neural::SpaceMission::LEO_POLAR;

    // Parse command line args if provided
    if (argc > 1) {
        std::string mission_arg = argv[1];
        if (mission_arg == "leo_equatorial") {
            mission = rad_ml::neural::SpaceMission::LEO_EQUATORIAL;
        }
        else if (mission_arg == "leo_polar") {
            mission = rad_ml::neural::SpaceMission::LEO_POLAR;
        }
        else if (mission_arg == "meo") {
            mission = rad_ml::neural::SpaceMission::MEO;
        }
        else if (mission_arg == "geo") {
            mission = rad_ml::neural::SpaceMission::GEO;
        }
        else if (mission_arg == "lunar") {
            mission = rad_ml::neural::SpaceMission::LUNAR;
        }
        else if (mission_arg == "mars") {
            mission = rad_ml::neural::SpaceMission::MARS;
        }
        else if (mission_arg == "jupiter") {
            mission = rad_ml::neural::SpaceMission::JUPITER;
        }
        else if (mission_arg == "solar") {
            mission = rad_ml::neural::SpaceMission::SOLAR_PROBE;
        }
    }

    // Create radiation environment
    rad_ml::neural::RadiationEnvironment environment(mission);

    // Set solar activity level (0.0-1.0)
    environment.setSolarActivity(0.5);  // Medium solar activity

    // Set spacecraft shielding
    environment.setShielding(5.0);  // 5mm aluminum equivalent

    // Define test parameters
    double dose_rate = 0.01;         // Gy/s
    double training_duration = 100;  // seconds
    int test_iterations = 10;        // Number of test iterations to average

    // Run demonstration
    demonstrateQFTEnhancedNetwork(environment, dose_rate, training_duration, test_iterations);

    // Demonstrate with solar flare
    std::cout << "\n\nTesting with solar flare:\n";
    std::cout << "===========================\n\n";

    // Simulate solar flare
    double flare_intensity = environment.simulateSolarFlare(2.0);

    std::cout << "Solar flare intensity: " << flare_intensity << "x baseline\n";

    // Update dose rate based on solar flare
    double flare_dose_rate = dose_rate * flare_intensity;

    // Run demonstration with solar flare conditions
    demonstrateQFTEnhancedNetwork(environment, flare_dose_rate, training_duration, test_iterations);

    // Compare between different shielding levels
    std::cout << "\n\nComparing different shielding levels:\n";
    std::cout << "====================================\n\n";

    std::vector<double> shielding_levels = {1.0, 5.0, 10.0, 20.0};  // mm of aluminum

    for (double shield : shielding_levels) {
        std::cout << "\nTesting with " << shield << "mm aluminum shielding:\n";
        environment.setShielding(shield);

        // Calculate position for SAA (South Atlantic Anomaly)
        rad_ml::neural::OrbitalPosition saa_position;
        saa_position.latitude = -30.0;   // South Atlantic
        saa_position.longitude = -40.0;  // Anomaly region
        saa_position.altitude = 400.0;   // km

        // Calculate radiation flux at position
        auto flux = environment.calculateRadiationFlux(saa_position);

        std::cout << "Radiation flux at SAA:\n";
        std::cout << "  - Total flux: " << flux.getTotalFlux() << " particles/cm²/s\n";
        std::cout << "  - Total dose rate: " << flux.total_dose_rate << " rad/hour\n";

        // Run short demonstration
        demonstrateQFTEnhancedNetwork(environment, flux.total_dose_rate / 3600.0, 50.0, 5);
    }

    // Demonstrate cross-domain knowledge transfer
    std::cout << "\n\nDemonstrating Cross-Domain Knowledge Transfer:\n";
    std::cout << "===========================================\n\n";

    // Create networks
    std::vector<size_t> layer_sizes = {10, 15, 8, 4};

    // Create models with different protection levels
    rad_ml::neural::ProtectedNeuralNetwork_mock<float> semiconductor_model(
        layer_sizes, rad_ml::neural::ProtectionLevel::FULL_TMR);
    rad_ml::neural::QFTEnhancedNetwork<float> space_model(
        layer_sizes, rad_ml::neural::ProtectionLevel::ADAPTIVE_TMR);

    // Configure models
    rad_ml::crossdomain::BiologicalParameters bio_params;
    bio_params.water_content = 0.7;
    bio_params.radiosensitivity = 1.2;
    bio_params.repair_rate = 0.3;
    bio_params.temperature = 310.0;

    space_model.setBiologicalParameters(bio_params);

    // Generate synthetic test data
    std::vector<std::vector<float>> train_data;
    std::vector<std::vector<float>> train_targets;
    rad_ml::neural::generateSyntheticData(train_data, train_targets, 500, layer_sizes.front(),
                                          layer_sizes.back());

    // Train each model separately
    std::cout << "Training semiconductor model in low-radiation environment...\n";
    semiconductor_model.train(train_data, train_targets, 0.001f, 50, 32);

    std::cout << "Training space model with radiation awareness...\n";
    space_model.trainWithQFT(train_data, train_targets, 0.001f, 50, 32, 0.5);

    // Test data
    std::vector<std::vector<float>> test_data;
    std::vector<std::vector<float>> test_targets;
    rad_ml::neural::generateSyntheticData(test_data, test_targets, 100, layer_sizes.front(),
                                          layer_sizes.back());

    // Test radiation levels
    std::vector<double> test_rad_levels = {0.0, 0.5, 1.0, 2.0, 5.0};

    std::cout << "\nComparison of models under radiation:\n";
    std::cout << "------------------------------------\n";
    std::cout << "Rad Level | Standard  | QFT-Enhanced | Improvement\n";
    std::cout << "--------------------------------------------\n";

    for (double rad : test_rad_levels) {
        // Test standard model
        float std_acc = 0.0f;
        size_t correct = 0;

        for (size_t i = 0; i < test_data.size(); ++i) {
            // Forward pass
            std::vector<float> output = semiconductor_model.forward(test_data[i], rad);

            // Simple accuracy check
            bool match = true;
            for (size_t j = 0; j < output.size(); ++j) {
                bool target_high = test_targets[i][j] > 0.5;
                bool output_high = output[j] > 0.5;

                if (target_high != output_high) {
                    match = false;
                    break;
                }
            }

            if (match) correct++;
        }

        std_acc = static_cast<float>(correct) / test_data.size();

        // Test QFT-enhanced model
        float qft_acc =
            rad_ml::neural::testNetworkAccuracy(space_model, test_data, test_targets, rad, 5);

        float improvement = 0.0f;
        if (std_acc > 0.0f) {
            improvement = (qft_acc - std_acc) / std_acc * 100.0f;
        }

        printf("%8.2f | %8.2f%% | %12.2f%% | %+10.2f%%\n", rad, std_acc * 100.0f, qft_acc * 100.0f,
               improvement);
    }

    std::cout << "\nQFT-Enhanced Neural Network Example Complete\n";

    return 0;
}
