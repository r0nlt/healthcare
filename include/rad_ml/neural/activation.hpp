/**
 * @file activation.hpp
 * @brief Defines activation functions for neural networks
 */

#pragma once

#include <functional>
#include <cmath>

namespace rad_ml {
namespace neural {

/**
 * @brief Enumeration of activation function types
 */
enum class Activation {
    LINEAR,    ///< Linear activation: f(x) = x
    RELU,      ///< Rectified Linear Unit: f(x) = max(0, x)
    SIGMOID,   ///< Sigmoid activation: f(x) = 1 / (1 + exp(-x))
    TANH,      ///< Hyperbolic tangent: f(x) = tanh(x)
    SOFTMAX,   ///< Softmax activation (for output layer)
    LEAKY_RELU ///< Leaky ReLU: f(x) = max(0.01x, x)
};

/**
 * @brief Get activation function by type
 * 
 * @tparam T Value type
 * @param type Activation type
 * @return Function implementing the activation
 */
template <typename T>
std::function<T(T)> getActivationFunction(Activation type) {
    switch (type) {
        case Activation::RELU:
            return [](T x) -> T { return x > 0 ? x : 0; };
        case Activation::SIGMOID:
            return [](T x) -> T { return 1 / (1 + std::exp(-x)); };
        case Activation::TANH:
            return [](T x) -> T { return std::tanh(x); };
        case Activation::LEAKY_RELU:
            return [](T x) -> T { return x > 0 ? x : 0.01f * x; };
        case Activation::LINEAR:
        default:
            return [](T x) -> T { return x; };
    }
}

} // namespace neural
} // namespace rad_ml 