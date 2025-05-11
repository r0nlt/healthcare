#include <rad_ml/research/architecture_tester.hpp>
#include <rad_ml/sim/environment.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

// Generate synthetic dataset for testing
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

int main(int argc, char** argv) {
    std::cout << "Neural Architecture Testing Example" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Create synthetic dataset
    std::cout << "Creating synthetic dataset..." << std::endl;
    auto [train_data, train_labels, test_data, test_labels] = 
        createSyntheticDataset(100, 20, 4, 3);
    
    // Create architecture tester
    std::cout << "Creating architecture tester..." << std::endl;
    rad_ml::research::ArchitectureTester tester(
        train_data, train_labels, test_data, test_labels,
        4,  // input size
        3,  // output size
        "architecture_results.csv"
    );
    
    // Test different architectures
    std::cout << "Testing width range..." << std::endl;
    tester.testWidthRange(
        16,    // min width
        64,    // max width
        16,    // step size
        0.2,   // dropout rate
        2,     // epochs (small for quick testing)
        rad_ml::sim::Environment::MARS
    );
    
    // Test dropout range
    std::cout << "Testing dropout range..." << std::endl;
    tester.testDropoutRange(
        {32},        // hidden layer of width 32
        0.0,         // min dropout
        0.5,         // max dropout
        0.2,         // step size
        2,           // epochs
        rad_ml::sim::Environment::MARS
    );
    
    // Get best architecture
    auto best_arch = tester.getBestArchitecture(rad_ml::sim::Environment::MARS);
    std::cout << "Best architecture for Mars environment: ";
    for (auto size : best_arch) {
        std::cout << size << "-";
    }
    std::cout << std::endl;
    
    // Get optimal dropout
    double optimal_dropout = tester.getOptimalDropout(rad_ml::sim::Environment::MARS);
    std::cout << "Optimal dropout rate: " << optimal_dropout << std::endl;
    
    std::cout << "Results saved to architecture_results.csv" << std::endl;
    
    return 0;
} 