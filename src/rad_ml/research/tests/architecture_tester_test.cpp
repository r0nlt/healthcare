#include <gtest/gtest.h>
#include <rad_ml/research/architecture_tester.hpp>
#include <rad_ml/sim/environment.hpp>
#include <vector>
#include <random>
#include <algorithm>

namespace rad_ml {
namespace research {
namespace tests {

// Generate simple synthetic dataset for testing
std::tuple<std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>> 
createSyntheticDataset(size_t num_train, size_t num_test, size_t input_size, size_t output_size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    // Training data
    std::vector<float> train_data(num_train * input_size);
    std::generate(train_data.begin(), train_data.end(), [&]() { return dist(gen); });
    
    // Training labels (one-hot encoded)
    std::vector<float> train_labels(num_train * output_size, 0.0f);
    for (size_t i = 0; i < num_train; ++i) {
        size_t label = i % output_size;
        train_labels[i * output_size + label] = 1.0f;
    }
    
    // Test data
    std::vector<float> test_data(num_test * input_size);
    std::generate(test_data.begin(), test_data.end(), [&]() { return dist(gen); });
    
    // Test labels (one-hot encoded)
    std::vector<float> test_labels(num_test * output_size, 0.0f);
    for (size_t i = 0; i < num_test; ++i) {
        size_t label = i % output_size;
        test_labels[i * output_size + label] = 1.0f;
    }
    
    return {train_data, train_labels, test_data, test_labels};
}

TEST(ArchitectureTesterTest, ConstructorTest) {
    // Generate a small synthetic dataset
    auto [train_data, train_labels, test_data, test_labels] = 
        createSyntheticDataset(10, 5, 4, 3);
    
    // Create tester
    ArchitectureTester tester(
        train_data, train_labels, test_data, test_labels,
        4, 3, "test_results.csv"
    );
    
    // No assertions needed - just make sure it constructs without crashing
}

TEST(ArchitectureTesterTest, TestArchitectureTest) {
    // Generate a small synthetic dataset
    auto [train_data, train_labels, test_data, test_labels] = 
        createSyntheticDataset(10, 5, 4, 3);
    
    // Create tester
    ArchitectureTester tester(
        train_data, train_labels, test_data, test_labels,
        4, 3, "test_results.csv"
    );
    
    // Test a specific architecture
    auto result = tester.testArchitecture(
        {4, 8, 3},                         // simple architecture
        0.1,                               // dropout rate
        false,                             // no residual connections
        neural::ProtectionLevel::NONE,     // no protection
        1,                                 // epochs
        sim::Environment::EARTH            // Earth environment
    );
    
    // Check result fields have been set
    EXPECT_EQ(result.layer_sizes.size(), 3);
    EXPECT_EQ(result.layer_sizes[0], 4);  // input size
    EXPECT_EQ(result.layer_sizes[1], 8);  // hidden size
    EXPECT_EQ(result.layer_sizes[2], 3);  // output size
    EXPECT_EQ(result.dropout_rate, 0.1);
    EXPECT_FALSE(result.has_residual_connections);
    EXPECT_EQ(result.protection_level, neural::ProtectionLevel::NONE);
    EXPECT_EQ(result.environment, sim::Environment::EARTH);
    
    // Check metrics have some values
    EXPECT_GT(result.baseline_accuracy, 0.0);
    EXPECT_GT(result.radiation_accuracy, 0.0);
}

TEST(ArchitectureTesterTest, TestWidthRangeTest) {
    // Generate a small synthetic dataset
    auto [train_data, train_labels, test_data, test_labels] = 
        createSyntheticDataset(10, 5, 4, 3);
    
    // Create tester
    ArchitectureTester tester(
        train_data, train_labels, test_data, test_labels,
        4, 3, "width_results.csv"
    );
    
    // Test with small range
    tester.testWidthRange(
        4,     // min width
        8,     // max width
        4,     // step size
        0.2,   // dropout rate
        1,     // epochs
        sim::Environment::EARTH
    );
    
    // Get best architecture - should return something
    auto best_arch = tester.getBestArchitecture(sim::Environment::EARTH);
    EXPECT_FALSE(best_arch.empty());
}

TEST(ArchitectureTesterTest, TestDropoutRangeTest) {
    // Generate a small synthetic dataset
    auto [train_data, train_labels, test_data, test_labels] = 
        createSyntheticDataset(10, 5, 4, 3);
    
    // Create tester
    ArchitectureTester tester(
        train_data, train_labels, test_data, test_labels,
        4, 3, "dropout_results.csv"
    );
    
    // Test dropout range
    tester.testDropoutRange(
        {8},        // simple architecture with one hidden layer
        0.0,        // min dropout
        0.3,        // max dropout
        0.3,        // step size (just test 0.0 and 0.3)
        1,          // epochs
        sim::Environment::EARTH
    );
    
    // Get optimal dropout - should be in the range we tested
    double optimal_dropout = tester.getOptimalDropout(sim::Environment::EARTH);
    EXPECT_GE(optimal_dropout, 0.0);
    EXPECT_LE(optimal_dropout, 0.5);
}

} // namespace tests
} // namespace research
} // namespace rad_ml 