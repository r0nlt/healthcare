/**
 * @file qft_enhanced_network.cpp
 * @brief Implementation of QFT-enhanced neural network functionality
 *
 * This file provides implementation examples and utilities for the
 * QFT-enhanced neural network class.
 */

#include <iostream>
#include <memory>
#include <rad_ml/crossdomain/qft_bridge.hpp>
#include <rad_ml/neural/qft_enhanced_network.hpp>
#include <rad_ml/neural/radiation_environment.hpp>
#include <rad_ml/physics/quantum_integration.hpp>
#include <random>

namespace rad_ml {
namespace neural {

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
void demonstrateQFTEnhancedNetwork(const RadiationEnvironment& environment, double dose_rate,
                                   double training_duration, int test_iterations)
{
    std::cout << "Demonstrating QFT-Enhanced Neural Network\n";
    std::cout << "Environment: " << environment.getMissionName() << "\n";
    std::cout << "Dose rate: " << dose_rate << " Gy/s\n";

    // Create network architecture
    std::vector<size_t> layer_sizes = {10, 20, 15, 5};

    // Create QFT-enhanced network
    QFTEnhancedNetwork<float> network(layer_sizes, ProtectionLevel::ADAPTIVE_TMR);

    // Create a semiconductor-domain network for comparison
    ProtectedNeuralNetwork<float> semiconductor_network(layer_sizes, ProtectionLevel::FULL_TMR);

    // Set biological and semiconductor parameters
    crossdomain::BiologicalParameters bio_params;
    bio_params.water_content = 0.7;
    bio_params.radiosensitivity = 1.2;
    bio_params.repair_rate = 0.3;
    bio_params.temperature = 310.0;

    network.setBiologicalParameters(bio_params);

    // Calculate total radiation dose
    double total_dose = dose_rate * training_duration;

    // Generate synthetic training data
    std::vector<std::vector<float>> training_inputs;
    std::vector<std::vector<float>> training_targets;
    generateSyntheticData(training_inputs, training_targets, 1000, layer_sizes.front(),
                          layer_sizes.back());

    // Generate test data
    std::vector<std::vector<float>> test_inputs;
    std::vector<std::vector<float>> test_targets;
    generateSyntheticData(test_inputs, test_targets, 100, layer_sizes.front(), layer_sizes.back());

    // Initialize with radiation-hardened weights
    network.initializeRadiationHardenedWeights(bio_params);

    // Train the network with QFT enhancements
    std::cout << "Training QFT-enhanced network with radiation dose: " << total_dose << " Gy\n";
    float final_loss = network.trainWithQFT(training_inputs, training_targets,
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
        float accuracy =
            testNetworkAccuracy(network, test_inputs, test_targets, level, test_iterations);
        std::cout << "Radiation level: " << level << " Gy/s, Accuracy: " << (accuracy * 100.0f)
                  << "%\n";
    }

    // Demonstrate cross-domain knowledge distillation
    std::cout << "\nPerforming cross-domain knowledge distillation...\n";

    // Train semiconductor model on original data
    semiconductor_network.train(training_inputs, training_targets, 0.001f, 50, 32);

    // Distill knowledge to biological domain
    double distillation_loss =
        network.distillSemiconductorKnowledgeToBiological(semiconductor_network, test_inputs);

    std::cout << "Distillation loss: " << distillation_loss << "\n";

    // Test distilled model
    float distilled_accuracy =
        testNetworkAccuracy(network, test_inputs, test_targets, 1.0, test_iterations);

    std::cout << "Post-distillation accuracy at 1.0 Gy/s: " << (distilled_accuracy * 100.0f)
              << "%\n";

    std::cout << "QFT-Enhanced Neural Network demonstration complete.\n";
}

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

}  // namespace neural
}  // namespace rad_ml
