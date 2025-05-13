/**
 * @file auto_arch_search_example.cpp
 * @brief Example demonstrating the automatic architecture search functionality
 * 
 * This example shows how to use the AutoArchSearch class to find optimal
 * neural network architectures for radiation environments.
 */

#include <rad_ml/research/auto_arch_search.hpp>
// Removing logger include that's causing build errors
#include <iostream>
#include <vector>
#include <random>
#include <cmath>

// Helper function to create a synthetic dataset for testing
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>>
createSyntheticDataset(size_t train_size, size_t test_size, size_t input_size, size_t num_classes) {
    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    // Create vectors to hold data
    std::vector<float> train_data(train_size * input_size);
    std::vector<float> train_labels(train_size * num_classes, 0.0f);
    std::vector<float> test_data(test_size * input_size);
    std::vector<float> test_labels(test_size * num_classes, 0.0f);
    
    // Generate training data
    for (size_t i = 0; i < train_size; ++i) {
        // Generate input features
        for (size_t j = 0; j < input_size; ++j) {
            train_data[i * input_size + j] = dist(gen);
        }
        
        // Generate one-hot encoded label
        size_t class_idx = i % num_classes;
        train_labels[i * num_classes + class_idx] = 1.0f;
    }
    
    // Generate test data
    for (size_t i = 0; i < test_size; ++i) {
        // Generate input features
        for (size_t j = 0; j < input_size; ++j) {
            test_data[i * input_size + j] = dist(gen);
        }
        
        // Generate one-hot encoded label
        size_t class_idx = i % num_classes;
        test_labels[i * num_classes + class_idx] = 1.0f;
    }
    
    return {train_data, train_labels, test_data, test_labels};
}

int main() {
    std::cout << "Automatic Architecture Search Example - Low Earth Orbit Environment" << std::endl;
    std::cout << "===================================================" << std::endl;
    
    // Create synthetic dataset (small for this example)
    std::cout << "Creating synthetic dataset..." << std::endl;
    auto [train_data, train_labels, test_data, test_labels] = 
        createSyntheticDataset(100, 30, 8, 4);  // Changed input size to 8 instead of 10 to be distinct
    
    // Create AutoArchSearch instance with Earth Orbit environment
    std::cout << "Creating architecture searcher for Earth Orbit environment..." << std::endl;
    rad_ml::research::AutoArchSearch searcher(
        train_data, train_labels, test_data, test_labels,
        rad_ml::sim::Environment::EARTH_ORBIT,  // Using Earth Orbit environment instead of LEO
        {32, 64, 128, 256},                     // Width options to test
        {0.3, 0.4, 0.5, 0.6},                   // Dropout options to test
        "leo_arch_search_results.csv"           // Output file specific to LEO
    );
    
    // Configure the search parameters
    std::cout << "Configuring search parameters..." << std::endl;
    searcher.setFixedParameters(
        10,  // Input size
        4,   // Output size (number of classes)
        2    // Try 2 hidden layers for this example
    );
    
    // Set protection levels to test - using the correct enum values from ProtectedNeuralNetwork
    searcher.setProtectionLevels({
        rad_ml::neural::ProtectionLevel::NONE,
        rad_ml::neural::ProtectionLevel::CHECKSUM_ONLY,
        rad_ml::neural::ProtectionLevel::SELECTIVE_TMR,
        rad_ml::neural::ProtectionLevel::FULL_TMR,
        rad_ml::neural::ProtectionLevel::ADAPTIVE_TMR,
        rad_ml::neural::ProtectionLevel::SPACE_OPTIMIZED
    });
    
    // Enable residual connections testing
    searcher.setTestResidualConnections(true);
    
    // Perform evolutionary search (better for complex search space)
    std::cout << "Starting evolutionary search for LEO-optimal architecture..." << std::endl;
    auto result = searcher.evolutionarySearch(
        10,   // 10 individuals in population
        5,    // 5 generations
        0.2,  // 20% mutation rate
        5,    // 5 training epochs per architecture
        true, // Use Monte Carlo testing
        3     // 3 Monte Carlo trials per architecture (using a small number for quick testing)
    );
    
    // Print the best architecture found
    std::cout << "\nBest LEO-optimized architecture found:" << std::endl;
    std::cout << "Layer sizes: ";
    for (auto size : result.config.layer_sizes) {
        std::cout << size << "-";
    }
    std::cout << std::endl;
    
    std::cout << "Dropout rate: " << result.config.dropout_rate << std::endl;
    std::cout << "Has residual connections: " 
              << (result.config.has_residual_connections ? "Yes" : "No") << std::endl;
    
    std::cout << "Protection level: ";
    switch (result.config.protection_level) {
        case rad_ml::neural::ProtectionLevel::NONE: 
            std::cout << "None"; 
            break;
        case rad_ml::neural::ProtectionLevel::CHECKSUM_ONLY: 
            std::cout << "Checksum Only"; 
            break;
        case rad_ml::neural::ProtectionLevel::SELECTIVE_TMR: 
            std::cout << "Selective TMR"; 
            break;
        case rad_ml::neural::ProtectionLevel::FULL_TMR: 
            std::cout << "Full TMR"; 
            break;
        case rad_ml::neural::ProtectionLevel::ADAPTIVE_TMR: 
            std::cout << "Adaptive TMR"; 
            break;
        case rad_ml::neural::ProtectionLevel::SPACE_OPTIMIZED: 
            std::cout << "Space Optimized"; 
            break;
        default: 
            std::cout << "Unknown";
    }
    std::cout << std::endl;
    
    std::cout << "Baseline accuracy: " << result.baseline_accuracy << "%" << std::endl;
    std::cout << "Radiation accuracy: " << result.radiation_accuracy << "%" << std::endl;
    std::cout << "Accuracy preservation: " << result.accuracy_preservation << "%" << std::endl;
    std::cout << "Found after " << result.iterations << " total architecture evaluations" << std::endl;
    
    std::cout << "\nDetailed results saved to 'leo_arch_search_results.csv'" << std::endl;
    
    // Optionally, compare with random search approach
    std::cout << "\nComparing with random search approach..." << std::endl;
    auto random_result = searcher.randomSearch(
        20,   // 20 iterations
        5,    // 5 training epochs 
        true, // Use Monte Carlo testing
        3     // 3 Monte Carlo trials per architecture
    );
    
    std::cout << "\nBest architecture found (random search):" << std::endl;
    std::cout << "Layer sizes: ";
    for (auto size : random_result.config.layer_sizes) {
        std::cout << size << "-";
    }
    std::cout << std::endl;
    std::cout << "Accuracy preservation: " << random_result.accuracy_preservation << "%" << std::endl;
    
    // Export combined results
    searcher.exportResults("leo_combined_results.csv");
    std::cout << "Combined results exported to 'leo_combined_results.csv'" << std::endl;
    
    return 0;
} 