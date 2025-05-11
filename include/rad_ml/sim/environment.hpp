/**
 * @file environment.hpp
 * @brief Radiation environment definitions for simulation
 * 
 * This file defines the standard radiation environments used for testing
 * neural network architectures under different radiation conditions.
 */

#pragma once

#include <string>
#include <map>

namespace rad_ml {
namespace sim {

/**
 * Standard radiation environments for testing
 */
enum class Environment {
    EARTH,             // Earth surface conditions
    EARTH_ORBIT,       // Low Earth Orbit
    ISS,               // International Space Station environment
    MOON,              // Lunar surface
    MARS,              // Mars surface
    SOLAR_FLARE,       // Solar flare condition
    DEEP_SPACE,        // Deep space interplanetary
    JUPITER,           // Jupiter orbit
    SAA,               // South Atlantic Anomaly
    EXTREME,           // Extreme radiation testing
    CUSTOM             // Custom environment (for advanced testing)
};

/**
 * Get the radiation level for a specific environment (0.0-1.0 scale)
 */
double getRadiationLevel(Environment env);

/**
 * Get the bit error rate for a specific environment
 */
double getBitErrorRate(Environment env);

/**
 * Get the expected MCU (Multiple Cell Upset) cluster size for an environment
 */
int getExpectedMCUSize(Environment env);

/**
 * Get the human-readable name of an environment
 */
std::string getEnvironmentName(Environment env);

/**
 * Get the radiation environment parameters for an environment
 */
struct RadiationEnvironment getRadiationEnvironment(Environment env);

} // namespace sim
} // namespace rad_ml 