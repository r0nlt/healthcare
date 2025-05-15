/**
 * @file qft_enhanced_network.hpp
 * @brief QFT-enhanced neural network implementation
 *
 * This file implements a neural network that leverages quantum field theory
 * to enhance its radiation tolerance through cross-domain knowledge transfer.
 */

#ifndef RAD_ML_NEURAL_QFT_ENHANCED_NETWORK_HPP
#define RAD_ML_NEURAL_QFT_ENHANCED_NETWORK_HPP

#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "../crossdomain/qft_bridge.hpp"
#include "../physics/quantum_integration.hpp"
#include "../physics/quantum_models.hpp"
#include "protected_neural_network_mock.hpp"
#include "radiation_environment.hpp"

namespace rad_ml {
namespace neural {

// Forward declarations
template <typename T>
class ProtectedNeuralNetwork_mock;
enum class ProtectionLevel;

/**
 * @brief QFT-enhanced neural network class
 *
 * This class extends ProtectedNeuralNetwork with quantum field theory
 * enhancements for improved radiation tolerance.
 *
 * @tparam T Value type (typically float)
 */
template <typename T = float>
class QFTEnhancedNetwork {
   public:
    /**
     * @brief Constructor
     *
     * @param layer_sizes Vector containing the size of each layer (including input and output)
     * @param protection_level Protection level to apply
     */
    QFTEnhancedNetwork(const std::vector<size_t>& layer_sizes,
                       ProtectionLevel protection_level = ProtectionLevel::ADAPTIVE_TMR)
        : network_(std::make_unique<ProtectedNeuralNetwork_mock<T>>(layer_sizes, protection_level)),
          bio_params_(),
          semi_params_(),
          qft_dropout_rates_(),
          radiation_aware_learning_rate_(0.001)
    {
        // Initialize default parameters
        bio_params_ = crossdomain::BiologicalParameters();
        semi_params_ = crossdomain::SemiconductorParameters();

        // Initialize QFT-enhanced dropout rates for each layer
        qft_dropout_rates_.resize(layer_sizes.size() - 1, 0.2);

        // Create physics models
        createQFTModels();
    }

    /**
     * @brief Get the name of the network
     *
     * @return Network name
     */
    std::string getName() const { return "QFT-Enhanced Neural Network"; }

    /**
     * @brief Get the layer count
     *
     * @return Number of layers
     */
    size_t getLayerCount() const { return network_->getLayerCount(); }

    /**
     * @brief Get the input size
     *
     * @return Input layer size
     */
    size_t getInputSize() const { return network_->getLayerInputSize(0); }

    /**
     * @brief Get the output size
     *
     * @return Output layer size
     */
    size_t getOutputSize() const
    {
        return network_->getLayerOutputSize(network_->getLayerCount() - 1);
    }

    /**
     * @brief Forward pass with radiation awareness
     *
     * @param input Input tensor
     * @param radiation_level Radiation level in Gy/s
     * @return Output tensor
     */
    std::vector<T> forward(const std::vector<T>& input, double radiation_level = 0.0) const
    {
        // Apply QFT-enhanced forward pass with radiation effects
        std::vector<T> result = network_->forward(input, radiation_level);

        // Apply radiation-aware corrections if radiation is present
        if (radiation_level > 0.0) {
            applyQFTCorrections(result, radiation_level);
        }

        return result;
    }

    /**
     * @brief Apply QFT-enhanced dropout during training
     *
     * @param activations Activations to apply dropout to
     * @param layer_idx Layer index
     * @param radiation_dose Radiation dose in Gy
     * @param training Whether in training mode
     */
    void applyQFTEnhancedDropout(std::vector<T>& activations, size_t layer_idx,
                                 double radiation_dose, bool training = true)
    {
        if (!training) return;

        // Calculate radiation-aware dropout rate
        double base_rate = qft_dropout_rates_[layer_idx];
        double adjusted_rate = calculateRadiationAwareDropoutRate(base_rate, radiation_dose);

        // Apply dropout with adjusted rate
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        T scale = static_cast<T>(1.0 / (1.0 - adjusted_rate));

        for (size_t i = 0; i < activations.size(); ++i) {
            if (dis(gen) < adjusted_rate) {
                activations[i] = 0;
            }
            else {
                activations[i] *= scale;
            }
        }
    }

    /**
     * @brief Train the network with QFT-enhanced learning
     *
     * @param inputs Training inputs
     * @param targets Training targets
     * @param learning_rate Base learning rate
     * @param epochs Number of epochs
     * @param batch_size Batch size
     * @param radiation_dose Radiation dose in Gy
     * @return Final training loss
     */
    T trainWithQFT(const std::vector<std::vector<T>>& inputs,
                   const std::vector<std::vector<T>>& targets, T learning_rate, size_t epochs,
                   size_t batch_size, double radiation_dose = 0.0)
    {
        // Store base learning rate
        radiation_aware_learning_rate_ = learning_rate;

        T final_loss = 0.0;

        // Run training with radiation-aware adjustments
        for (size_t epoch = 0; epoch < epochs; ++epoch) {
            // Calculate adaptive learning rate based on radiation
            T adjusted_lr = calculateRadiationAwareLearningRate(learning_rate, radiation_dose);

            // Train for this epoch with the adjusted learning rate
            final_loss = network_->train(inputs, targets, adjusted_lr, 1, batch_size);

            // Periodically apply QFT corrections to weights
            if (epoch % 5 == 0 && radiation_dose > 0.0) {
                applyQFTCorrectionsToWeights(radiation_dose);
            }
        }

        return final_loss;
    }

    /**
     * @brief Calculate radiation-aware loss
     *
     * @param predictions Model predictions
     * @param targets Target values
     * @param radiation_dose Radiation dose in Gy
     * @return Radiation-aware loss value
     */
    T calculateRadiationAwareLoss(const std::vector<std::vector<T>>& predictions,
                                  const std::vector<std::vector<T>>& targets, double radiation_dose)
    {
        // Start with standard MSE loss
        T standard_loss = 0.0;
        size_t count = 0;

        for (size_t i = 0; i < predictions.size(); ++i) {
            for (size_t j = 0; j < predictions[i].size(); ++j) {
                T error = predictions[i][j] - targets[i][j];
                standard_loss += error * error;
                count++;
            }
        }

        if (count > 0) {
            standard_loss /= count;
        }

        // Get quantum enhancement factor based on dose
        physics::CrystalLattice crystal;
        crystal.type = physics::CrystalLattice::DIAMOND;
        crystal.lattice_constant = 5.431;
        crystal.barrier_height = semi_params_.barrier_height;

        double qft_enhancement = physics::calculateQuantumEnhancementFactor(
            bio_params_.temperature, bio_params_.feature_size);

        // Scale loss by radiation effects
        return standard_loss * (1.0 + qft_enhancement * radiation_dose);
    }

    /**
     * @brief Initialize with radiation-hardened weight initialization
     *
     * @param bio_params Biological parameters for QFT model
     */
    void initializeRadiationHardenedWeights(const crossdomain::BiologicalParameters& bio_params)
    {
        bio_params_ = bio_params;

        // Initialize each layer with radiation-aware scaling
        for (size_t layer = 0; layer < network_->getLayerCount() - 1; ++layer) {
            size_t inputs = network_->getLayerInputSize(layer);
            size_t outputs = network_->getLayerOutputSize(layer);

            // Get standard initialization range (Xavier/Glorot)
            double range = std::sqrt(6.0 / (inputs + outputs));

            // Scale based on radiation tolerance factors
            double scaling = 1.0 / (1.0 + bio_params.radiosensitivity * 0.5);

            // Initialize layer weights
            auto weights = network_->getLayerWeights(layer);
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<T> dist(-range * scaling, range * scaling);

            for (size_t i = 0; i < inputs; ++i) {
                for (size_t j = 0; j < outputs; ++j) {
                    weights[i][j] = dist(gen);
                }
            }

            network_->setLayerWeights(layer, weights);
        }
    }

    /**
     * @brief Set the biological parameters for the QFT model
     *
     * @param params Biological parameters
     */
    void setBiologicalParameters(const crossdomain::BiologicalParameters& params)
    {
        bio_params_ = params;

        // Update semiconductor parameters through conversion
        semi_params_ = crossdomain::convertToSemiconductor(bio_params_);
    }

    /**
     * @brief Set the semiconductor parameters for the QFT model
     *
     * @param params Semiconductor parameters
     */
    void setSemiconductorParameters(const crossdomain::SemiconductorParameters& params)
    {
        semi_params_ = params;

        // Update biological parameters through conversion
        bio_params_ = crossdomain::convertToBiological(semi_params_);
    }

    /**
     * @brief Perform cross-domain knowledge distillation
     *
     * @param semiconductor_model Source model from semiconductor domain
     * @param test_data Test data for distillation
     * @param temperature Softmax temperature for distillation
     * @return Distillation loss
     */
    double distillSemiconductorKnowledgeToBiological(
        const ProtectedNeuralNetwork_mock<T>& semiconductor_model,
        const std::vector<std::vector<T>>& test_data, double temperature = 2.0)
    {
        double total_loss = 0.0;

        // Process each test sample
        for (const auto& sample : test_data) {
            // Get semiconductor model predictions
            auto semi_predictions = semiconductor_model.forward(sample, 0.0);

            // Apply domain adaptation factor (110x amplification from QFT bridge)
            std::vector<T> adapted_predictions;
            adapted_predictions.reserve(semi_predictions.size());

            for (const auto& pred : semi_predictions) {
                adapted_predictions.push_back(pred * 110.0);
            }

            // Get biological model predictions
            auto bio_predictions = forward(sample, 0.0);

            // Calculate KL divergence loss
            total_loss += calculateKLDivergence(adapted_predictions, bio_predictions, temperature);
        }

        return total_loss / test_data.size();
    }

   private:
    /**
     * @brief Create QFT models for the network
     */
    void createQFTModels()
    {
        // Configure crystal lattice for silicon
        crystal_.type = physics::CrystalLattice::DIAMOND;
        crystal_.lattice_constant = 5.431;  // Silicon
        crystal_.barrier_height = semi_params_.barrier_height;

        // Create QFT parameters
        qft_params_ = physics::createQFTParameters(crystal_, semi_params_.feature_size);
    }

    /**
     * @brief Apply QFT corrections to the output
     *
     * @param output Output tensor to correct
     * @param radiation_level Radiation level in Gy/s
     */
    void applyQFTCorrections(std::vector<T>& output, double radiation_level) const
    {
        // Get quantum enhancement factor
        double qft_enhancement = physics::calculateQuantumEnhancementFactor(
            bio_params_.temperature, bio_params_.feature_size);

        // Calculate correction factor based on radiation level
        double correction_factor = 1.0 + (qft_enhancement * radiation_level * 0.1);

        // Apply correction to outputs
        for (auto& val : output) {
            // Adjust towards more conservative predictions under radiation
            if (val > 0.5) {
                val = static_cast<T>(0.5 + (val - 0.5) / correction_factor);
            }
            else if (val < 0.5) {
                val = static_cast<T>(0.5 - (0.5 - val) / correction_factor);
            }
        }
    }

    /**
     * @brief Apply QFT corrections to weights
     *
     * @param radiation_dose Radiation dose in Gy
     */
    void applyQFTCorrectionsToWeights(double radiation_dose)
    {
        // Calculate tunneling probability
        double tunneling_prob = physics::calculateQuantumTunnelingProbability(
            bio_params_.barrier_height, bio_params_.effective_mass, 6.582119569e-16,
            bio_params_.temperature);

        // Calculate zero-point energy contribution
        double zpe_contribution = physics::calculateZeroPointEnergyContribution(
            6.582119569e-16, bio_params_.effective_mass, crystal_.lattice_constant,
            bio_params_.temperature);

        // Iterate through all layers and apply corrections
        for (size_t layer = 0; layer < network_->getLayerCount() - 1; ++layer) {
            auto weights = network_->getLayerWeights(layer);
            auto biases = network_->getLayerBiases(layer);

            // Apply corrections to weights based on QFT effects
            for (auto& row : weights) {
                for (auto& w : row) {
                    // Regularization effect that increases with radiation
                    double reg_factor = 1.0 - (radiation_dose * tunneling_prob * 0.1);
                    reg_factor = std::max(0.9, reg_factor);  // Don't reduce too much

                    // Apply regularization
                    w = static_cast<T>(w * reg_factor);

                    // Add small noise from zero-point energy
                    if (zpe_contribution > 0.001) {
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::normal_distribution<T> noise(0,
                                                          static_cast<T>(zpe_contribution * 0.01));
                        w += noise(gen);
                    }
                }
            }

            // Apply corrections to biases
            for (auto& b : biases) {
                // Similar regularization for biases
                double reg_factor = 1.0 - (radiation_dose * tunneling_prob * 0.05);
                reg_factor = std::max(0.95, reg_factor);

                b = static_cast<T>(b * reg_factor);
            }

            // Update the layer
            network_->setLayerWeights(layer, weights);
            network_->setLayerBiases(layer, biases);
        }
    }

    /**
     * @brief Calculate radiation-aware dropout rate
     *
     * @param base_rate Base dropout rate
     * @param radiation_dose Radiation dose in Gy
     * @return Adjusted dropout rate
     */
    double calculateRadiationAwareDropoutRate(double base_rate, double radiation_dose)
    {
        if (radiation_dose <= 0.0) return base_rate;

        // Calculate tunneling probability for this environment
        double tunneling_prob = physics::calculateQuantumTunnelingProbability(
            bio_params_.barrier_height, bio_params_.effective_mass, 6.582119569e-16,
            bio_params_.temperature);

        // More dropout in high-radiation conditions
        double adjusted_rate = base_rate * (1.0 + radiation_dose * tunneling_prob * 10.0);

        // Ensure rate stays reasonable
        return std::min(0.8, adjusted_rate);
    }

    /**
     * @brief Calculate radiation-aware learning rate
     *
     * @param base_lr Base learning rate
     * @param radiation_dose Radiation dose in Gy
     * @return Adjusted learning rate
     */
    T calculateRadiationAwareLearningRate(T base_lr, double radiation_dose)
    {
        if (radiation_dose <= 0.0) return base_lr;

        // Scale learning rate inversely with radiation to improve stability
        double factor = 1.0 / (1.0 + radiation_dose * 2.0);

        // Ensure learning rate doesn't drop too low
        factor = std::max(0.1, factor);

        return static_cast<T>(base_lr * factor);
    }

    /**
     * @brief Calculate KL divergence between probability distributions
     *
     * @param p First distribution
     * @param q Second distribution
     * @param temperature Softmax temperature
     * @return KL divergence
     */
    double calculateKLDivergence(const std::vector<T>& p, const std::vector<T>& q,
                                 double temperature = 1.0)
    {
        if (p.size() != q.size() || p.empty()) {
            return 0.0;
        }

        // Apply softmax with temperature to both distributions
        auto p_softmax = applySoftmax(p, temperature);
        auto q_softmax = applySoftmax(q, temperature);

        // Calculate KL divergence: KL(p||q) = sum(p_i * log(p_i / q_i))
        double kl_divergence = 0.0;
        for (size_t i = 0; i < p_softmax.size(); ++i) {
            // Avoid division by zero
            double q_val = std::max(q_softmax[i], 1e-7);

            if (p_softmax[i] > 0) {
                kl_divergence += p_softmax[i] * std::log(p_softmax[i] / q_val);
            }
        }

        return kl_divergence;
    }

    /**
     * @brief Apply softmax function to a vector
     *
     * @param x Input vector
     * @param temperature Softmax temperature
     * @return Softmax output
     */
    std::vector<double> applySoftmax(const std::vector<T>& x, double temperature = 1.0)
    {
        std::vector<double> result(x.size());

        // Find maximum value for numerical stability
        double max_val = *std::max_element(x.begin(), x.end());

        // Calculate exp of each element with temperature scaling
        double sum = 0.0;
        for (size_t i = 0; i < x.size(); ++i) {
            result[i] = std::exp((x[i] - max_val) / temperature);
            sum += result[i];
        }

        // Normalize
        if (sum > 0) {
            for (auto& val : result) {
                val /= sum;
            }
        }

        return result;
    }

   private:
    // Core network implementation
    std::unique_ptr<ProtectedNeuralNetwork_mock<T>> network_;

    // QFT model components
    crossdomain::BiologicalParameters bio_params_;
    crossdomain::SemiconductorParameters semi_params_;
    physics::CrystalLattice crystal_;
    physics::QFTParameters qft_params_;

    // Network parameters
    std::vector<double> qft_dropout_rates_;
    T radiation_aware_learning_rate_;
};

}  // namespace neural
}  // namespace rad_ml

#endif  // RAD_ML_NEURAL_QFT_ENHANCED_NETWORK_HPP
