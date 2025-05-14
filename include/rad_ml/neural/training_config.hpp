/**
 * @file training_config.hpp
 * @brief Configuration for neural network training
 * 
 * This file defines the configuration parameters for training neural networks,
 * including epochs, batch size, learning rate, and other hyperparameters.
 */

#pragma once

#include <cstddef>
#include <string>
#include <functional>

namespace rad_ml {
namespace neural {

/**
 * @brief Configuration for neural network training
 */
struct TrainingConfig {
    int epochs = 10;              ///< Number of training epochs
    int batch_size = 32;          ///< Batch size for training
    float learning_rate = 0.01f;  ///< Learning rate for optimization
    float momentum = 0.9f;        ///< Momentum factor for optimization
    float weight_decay = 0.0001f; ///< Weight decay factor for regularization
    float dropout_rate = 0.0f;    ///< Dropout rate during training
    bool shuffle = true;          ///< Whether to shuffle data during training
    std::string optimizer = "sgd"; ///< Optimizer type (sgd, adam, etc.)
    
    // Early stopping parameters
    bool early_stopping = false;   ///< Whether to use early stopping
    int patience = 5;              ///< Patience for early stopping
    float min_delta = 0.001f;      ///< Minimum change to qualify as improvement
    
    // Callbacks for training
    std::function<void(int, float, float)> epoch_callback = nullptr; ///< Callback after each epoch
    
    /**
     * @brief Default constructor
     */
    TrainingConfig() = default;
    
    /**
     * @brief Constructor with basic parameters
     * 
     * @param epochs Number of training epochs
     * @param batch_size Batch size for training
     * @param learning_rate Learning rate for optimization
     */
    TrainingConfig(int epochs, int batch_size, float learning_rate)
        : epochs(epochs), batch_size(batch_size), learning_rate(learning_rate) {}
};

} // namespace neural
} // namespace rad_ml 