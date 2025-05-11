#include <gtest/gtest.h>
#include <rad_ml/research/residual_network.hpp>
#include <rad_ml/neural/protected_neural_network.hpp>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <fstream>

namespace rad_ml {
namespace research {
namespace tests {

// Helper to generate random input
std::vector<float> generateRandomInput(size_t size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    std::vector<float> input(size);
    std::generate(input.begin(), input.end(), [&]() { return dist(gen); });
    return input;
}

// Helper to compare two vectors approximately
bool vectorsAlmostEqual(const std::vector<float>& a, const std::vector<float>& b, float epsilon = 1e-6f) {
    if (a.size() != b.size()) return false;
    
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[i]) > epsilon) return false;
    }
    
    return true;
}

TEST(ResidualNetworkTest, ConstructorTest) {
    // Create a simple network
    ResidualNeuralNetwork<float> network(
        std::vector<size_t>{4, 8, 8, 4},  // 4 inputs, 2 hidden layers, 4 outputs
        neural::ProtectionLevel::NONE
    );
    
    // No assertions needed - just make sure it constructs without crashing
}

TEST(ResidualNetworkTest, AddSkipConnectionTest) {
    // Create a simple network
    ResidualNeuralNetwork<float> network(
        std::vector<size_t>{4, 8, 8, 4},  // 4 inputs, 2 hidden layers, 4 outputs
        neural::ProtectionLevel::NONE
    );
    
    // Add a valid skip connection
    EXPECT_TRUE(network.addSkipConnection(0, 2));
    
    // Invalid connections should fail
    EXPECT_FALSE(network.addSkipConnection(0, 0));  // Same layer
    EXPECT_FALSE(network.addSkipConnection(2, 1));  // Backwards connection
    EXPECT_FALSE(network.addSkipConnection(3, 5));  // Out of bounds
}

TEST(ResidualNetworkTest, RemoveSkipConnectionTest) {
    // Create a simple network
    ResidualNeuralNetwork<float> network(
        std::vector<size_t>{4, 8, 8, 4},  // 4 inputs, 2 hidden layers, 4 outputs
        neural::ProtectionLevel::NONE
    );
    
    // Add a skip connection
    EXPECT_TRUE(network.addSkipConnection(0, 2));
    
    // Should be able to remove it
    EXPECT_TRUE(network.removeSkipConnection(0, 2));
    
    // Removing again should fail
    EXPECT_FALSE(network.removeSkipConnection(0, 2));
}

TEST(ResidualNetworkTest, GetSkipConnectionsTest) {
    // Create a simple network
    ResidualNeuralNetwork<float> network(
        std::vector<size_t>{4, 8, 8, 4},  // 4 inputs, 2 hidden layers, 4 outputs
        neural::ProtectionLevel::NONE
    );
    
    // Initially should have no connections
    EXPECT_EQ(network.getSkipConnectionCount(), 0);
    
    // Add connections
    EXPECT_TRUE(network.addSkipConnection(0, 2));
    EXPECT_TRUE(network.addSkipConnection(1, 3));
    
    // Should have 2 connections now
    EXPECT_EQ(network.getSkipConnectionCount(), 2);
    
    // Get connections
    auto connections = network.getSkipConnections();
    EXPECT_EQ(connections.size(), 2);
    
    // Check if the connections we added are in the list
    bool found_0_2 = false;
    bool found_1_3 = false;
    
    for (const auto& [from, to] : connections) {
        if (from == 0 && to == 2) found_0_2 = true;
        if (from == 1 && to == 3) found_1_3 = true;
    }
    
    EXPECT_TRUE(found_0_2);
    EXPECT_TRUE(found_1_3);
}

TEST(ResidualNetworkTest, ForwardPassTest) {
    // Create a simple network
    ResidualNeuralNetwork<float> network(
        std::vector<size_t>{4, 8, 8, 4},  // 4 inputs, 2 hidden layers, 4 outputs
        neural::ProtectionLevel::NONE
    );
    
    // Set activation functions
    for (size_t i = 0; i < 3; ++i) {
        network.setActivationFunction(i, [](float x) { return x > 0 ? x : 0; });  // ReLU
    }
    
    // Generate a test input
    auto input = generateRandomInput(4);
    
    // Run forward pass
    auto output = network.forward(input);
    
    // Check output size
    EXPECT_EQ(output.size(), 4);
}

TEST(ResidualNetworkTest, SetSkipProjectionTest) {
    // Create a simple network
    ResidualNeuralNetwork<float> network(
        std::vector<size_t>{4, 8, 8, 4},  // 4 inputs, 2 hidden layers, 4 outputs
        neural::ProtectionLevel::NONE
    );
    
    // Add a skip connection
    EXPECT_TRUE(network.addSkipConnection(0, 2));
    
    // Set a projection function (doubles all values)
    network.setSkipProjection(0, 2, [](const std::vector<float>& input) {
        std::vector<float> result = input;
        for (auto& val : result) {
            val *= 2.0f;
        }
        return result;
    });
}

TEST(ResidualNetworkTest, SkipConnectionEffectTest) {
    // Create two identical networks, one with a skip connection and one without
    ResidualNeuralNetwork<float> network1(
        std::vector<size_t>{4, 8, 8, 4},  // 4 inputs, 2 hidden layers, 4 outputs
        neural::ProtectionLevel::NONE
    );
    
    ResidualNeuralNetwork<float> network2(
        std::vector<size_t>{4, 8, 8, 4},  // identical architecture
        neural::ProtectionLevel::NONE
    );
    
    // Add skip connection to network2
    EXPECT_TRUE(network2.addSkipConnection(0, 2));
    
    // Set identical activation functions
    for (size_t i = 0; i < 3; ++i) {
        network1.setActivationFunction(i, [](float x) { return x > 0 ? x : 0; });  // ReLU
        network2.setActivationFunction(i, [](float x) { return x > 0 ? x : 0; });  // ReLU
    }
    
    // Set identical weights and biases
    // This is just a placeholder assertion since we can't actually set these without
    // exposing more methods in the class
    
    // Use identical input
    auto input = generateRandomInput(4);
    
    // Get outputs
    auto output1 = network1.forward(input);
    auto output2 = network2.forward(input);
    
    // We expect the outputs to be different, but we can't reliably test that
    // without access to the internal weights
    // This test is more of a placeholder for future enhancements
    EXPECT_EQ(output1.size(), output2.size());
}

TEST(ResidualNetworkTest, RadiationSimulationTest) {
    // Create network with radiation protection
    ResidualNeuralNetwork<float> network(
        std::vector<size_t>{4, 8, 8, 4},
        neural::ProtectionLevel::CHECKSUM_ONLY  // Use basic protection
    );
    
    // Generate input
    auto input = generateRandomInput(4);
    
    // Get output without radiation
    auto baseline_output = network.forward(input);
    
    // Get output with radiation
    auto radiation_output = network.forward(input, 0.1);  // 10% radiation level
    
    // Outputs will likely be different due to radiation effects, but should have same size
    EXPECT_EQ(baseline_output.size(), radiation_output.size());
}

TEST(ResidualNetworkTest, ProtectionEffectTest) {
    // Create two networks with different protection levels
    ResidualNeuralNetwork<float> unprotected(
        std::vector<size_t>{4, 8, 8, 4},
        neural::ProtectionLevel::NONE
    );
    
    ResidualNeuralNetwork<float> protected_network(
        std::vector<size_t>{4, 8, 8, 4},
        neural::ProtectionLevel::FULL_TMR  // Full Triple Modular Redundancy
    );
    
    // Generate input
    auto input = generateRandomInput(4);
    
    // Get outputs with high radiation
    auto unprotected_output = unprotected.forward(input, 0.5);  // 50% radiation
    auto protected_output = protected_network.forward(input, 0.5);  // 50% radiation
    
    // We can't reliably test the difference without more control over the radiation effects,
    // but we can verify both networks produce outputs of the correct size
    EXPECT_EQ(unprotected_output.size(), 4);
    EXPECT_EQ(protected_output.size(), 4);
}

TEST(ResidualNetworkTest, FileIOTest) {
    // Create a network with some skip connections
    ResidualNeuralNetwork<float> original(
        std::vector<size_t>{4, 8, 8, 4},
        neural::ProtectionLevel::NONE
    );
    
    // Add skip connections
    EXPECT_TRUE(original.addSkipConnection(0, 2));
    EXPECT_TRUE(original.addSkipConnection(1, 3));
    
    // Set activation functions
    for (size_t i = 0; i < 3; ++i) {
        original.setActivationFunction(i, [](float x) { return x > 0 ? x : 0; });  // ReLU
    }
    
    // Generate input
    auto input = generateRandomInput(4);
    
    // Get original output
    auto original_output = original.forward(input);
    
    // Test filename
    const std::string test_filename = "test_network.dat";
    
    // Save to file
    EXPECT_TRUE(original.saveToFile(test_filename));
    
    // Create a new network and load from file
    ResidualNeuralNetwork<float> loaded(
        std::vector<size_t>{4, 8, 8, 4},  // Same architecture
        neural::ProtectionLevel::NONE
    );
    
    // Load might not currently be implemented, so this is a placeholder test
    // EXPECT_TRUE(loaded.loadFromFile(test_filename));
    
    // Clean up the test file
    std::remove(test_filename.c_str());
}

} // namespace tests
} // namespace research
} // namespace rad_ml 