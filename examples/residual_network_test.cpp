#include <rad_ml/research/residual_network.hpp>
#include <rad_ml/neural/protected_neural_network.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>

// Generate random input
std::vector<float> generateRandomInput(size_t size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    std::vector<float> input(size);
    std::generate(input.begin(), input.end(), [&]() { return dist(gen); });
    return input;
}

// Print vector
void printVector(const std::vector<float>& vec, const std::string& label) {
    std::cout << label << ": ";
    for (auto val : vec) {
        std::cout << std::fixed << std::setprecision(4) << val << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "Residual Neural Network Example" << std::endl;
    std::cout << "===============================" << std::endl;
    
    // Create standard network
    std::cout << "Creating standard protected neural network..." << std::endl;
    rad_ml::neural::ProtectedNeuralNetwork<float> standard_network(
        {4, 8, 8, 4},  // 4 inputs, 2 hidden layers (8 neurons each), 4 outputs
        rad_ml::neural::ProtectionLevel::NONE
    );
    
    // Set activation functions (ReLU for hidden, sigmoid for output)
    for (size_t i = 0; i < 2; ++i) {
        standard_network.setActivationFunction(i, [](float x) { return x > 0 ? x : 0; });
    }
    standard_network.setActivationFunction(2, [](float x) { return 1.0f / (1.0f + std::exp(-x)); });
    
    // Create residual network with same architecture
    std::cout << "Creating residual neural network..." << std::endl;
    rad_ml::research::ResidualNeuralNetwork<float> residual_network(
        {4, 8, 8, 4},  // Same architecture as standard network
        rad_ml::neural::ProtectionLevel::NONE
    );
    
    // Set the same activation functions
    for (size_t i = 0; i < 2; ++i) {
        residual_network.setActivationFunction(i, [](float x) { return x > 0 ? x : 0; });
    }
    residual_network.setActivationFunction(2, [](float x) { return 1.0f / (1.0f + std::exp(-x)); });
    
    // Add skip connections
    std::cout << "Adding skip connections..." << std::endl;
    residual_network.addSkipConnection(0, 2);  // Connect input to second hidden layer
    
    // Generate random input
    auto input = generateRandomInput(4);
    printVector(input, "Input");
    
    // Forward pass through both networks
    std::cout << "Running forward pass..." << std::endl;
    auto standard_output = standard_network.forward(input);
    auto residual_output = residual_network.forward(input);
    
    // Print outputs
    printVector(standard_output, "Standard Network Output");
    printVector(residual_output, "Residual Network Output");
    
    // Add radiation to the networks
    std::cout << "\nTesting with radiation effects..." << std::endl;
    auto standard_output_rad = standard_network.forward(input, 0.01);
    auto residual_output_rad = residual_network.forward(input, 0.01);
    
    // Print outputs with radiation
    printVector(standard_output_rad, "Standard Network Output (with radiation)");
    printVector(residual_output_rad, "Residual Network Output (with radiation)");
    
    // Get error statistics
    auto [detected_std, corrected_std] = standard_network.getErrorStats();
    auto [detected_res, corrected_res] = residual_network.getErrorStats();
    
    std::cout << "\nError Statistics:" << std::endl;
    std::cout << "Standard Network: " << detected_std << " detected, " 
              << corrected_std << " corrected" << std::endl;
    
    std::cout << "Residual Network: " << detected_res << " detected, " 
              << corrected_res << " corrected" << std::endl;
    
    return 0;
} 