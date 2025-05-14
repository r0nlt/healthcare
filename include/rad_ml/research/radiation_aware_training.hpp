/**
 * @file radiation_aware_training.hpp
 * @brief Radiation-aware training for neural networks
 * 
 * This file implements radiation-aware training techniques that inject
 * bit flips during training to improve inherent resilience to radiation.
 */

#pragma once

#include <random>
#include <vector>
#include <functional>
#include <optional>
#include <variant>
#include <memory>
#include <filesystem>

#include "../neural/training_config.hpp"
#include "../sim/environment.hpp"

namespace rad_ml {
namespace research {

// Forward declarations
// Forward declaration of existing neural network classes
namespace neural = ::rad_ml::neural;  // Alias to avoid namespace confusion

// Forward declare templated ResidualNeuralNetwork
template <typename T = float>
class ResidualNeuralNetwork;

/**
 * @brief Class for radiation-aware training of neural networks
 * 
 * This class implements techniques for training neural networks with
 * simulated radiation effects, improving their inherent resilience
 * to radiation-induced errors.
 */
class RadiationAwareTraining {
public:
    /**
     * @brief Training statistics collected during training
     */
    struct TrainingStats {
        int total_bit_flips{0};         ///< Total number of bit flips injected
        float avg_accuracy_drop{0.0f};  ///< Average accuracy drop after bit flips
        float recovery_rate{0.0f};      ///< Average recovery rate after continued training
        
        // Support for structured binding
        template <std::size_t I>
        auto get() const {
            if constexpr (I == 0) return total_bit_flips;
            else if constexpr (I == 1) return avg_accuracy_drop;
            else if constexpr (I == 2) return recovery_rate;
        }
    };
    
    /**
     * @brief Result of training, either statistics or error message
     */
    using TrainingResult = std::variant<TrainingStats, std::string>;
    
    /**
     * @brief Constructor
     * 
     * @param bit_flip_probability Probability of bit flips during training
     * @param target_critical_weights Whether to target critical weights
     * @param environment Radiation environment to simulate
     */
    explicit RadiationAwareTraining(
        float bit_flip_probability = 0.01f,
        bool target_critical_weights = false,
        sim::Environment environment = sim::Environment::MARS
    );
    
    // Disable copy operations
    RadiationAwareTraining(const RadiationAwareTraining&) = delete;
    RadiationAwareTraining& operator=(const RadiationAwareTraining&) = delete;
    
    // Enable move operations
    RadiationAwareTraining(RadiationAwareTraining&&) noexcept = default;
    RadiationAwareTraining& operator=(RadiationAwareTraining&&) noexcept = default;
    
    /**
     * @brief Train a neural network with radiation awareness
     * 
     * @param network Neural network to train
     * @param train_data Training data
     * @param train_labels Training labels
     * @param config Training configuration
     * @return Training result (statistics or error message)
     */
    template <typename Network>
    TrainingResult train(
        Network& network,
        const std::vector<float>& train_data,
        const std::vector<float>& train_labels,
        const ::rad_ml::neural::TrainingConfig& config
    );
    
    /**
     * @brief Set bit flip probability
     * 
     * @param probability Probability of bit flips
     */
    void setBitFlipProbability(float probability);
    
    /**
     * @brief Set whether to target critical weights
     * 
     * @param target Whether to target critical weights
     */
    void setTargetCriticalWeights(bool target);
    
    /**
     * @brief Set radiation environment
     * 
     * @param env Radiation environment
     */
    void setEnvironment(sim::Environment env);
    
    /**
     * @brief Get criticality map
     * 
     * @return Criticality map if available
     */
    std::optional<std::vector<std::vector<std::vector<float>>>> getCriticalityMap() const;
    
    /**
     * @brief Get training statistics
     * 
     * @return Training statistics
     */
    TrainingStats getStats() const;
    
    /**
     * @brief Save results to a file
     * 
     * @param path Path to save results
     * @return Whether saving was successful
     */
    bool saveResults(const std::filesystem::path& path) const;

private:
    float bit_flip_probability_;
    bool target_critical_weights_;
    std::mt19937 generator_;
    std::uniform_real_distribution<float> uniform_dist_;
    std::uniform_int_distribution<int> bit_dist_;
    sim::Environment environment_;
    TrainingStats stats_{};
    std::optional<std::vector<std::vector<std::vector<float>>>> weight_criticality_{};
    
    /**
     * @brief Inject radiation effects into network
     * 
     * @param network Neural network
     */
    template <typename Network>
    void injectRadiationEffects(Network& network);
    
    /**
     * @brief Flip a bit in a float value
     * 
     * @param value Value to modify
     * @param bit_position Bit position to flip
     * @return Modified value
     */
    float flipBit(float value, int bit_position) const;
    
    /**
     * @brief Update criticality map for weights
     * 
     * @param network Neural network
     * @param data Training data
     * @param labels Training labels
     */
    template <typename Network>
    void updateCriticalityMap(
        Network& network,
        const std::vector<float>& data,
        const std::vector<float>& labels
    );
};

} // namespace research
} // namespace rad_ml

// Template specialization for tuple-like access (for structured binding support)
namespace std {
template<> struct tuple_size<rad_ml::research::RadiationAwareTraining::TrainingStats> 
    : std::integral_constant<size_t, 3> {};

template<size_t I> struct tuple_element<I, rad_ml::research::RadiationAwareTraining::TrainingStats> {
    using type = decltype(std::declval<rad_ml::research::RadiationAwareTraining::TrainingStats>().template get<I>());
};
} 