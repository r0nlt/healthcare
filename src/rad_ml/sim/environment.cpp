/**
 * @file environment.cpp
 * @brief Implementation of radiation environment simulation functions
 */

#include "rad_ml/sim/environment.hpp"
#include "rad_ml/sim/mission_environment.hpp"

#include <unordered_map>
#include <stdexcept>

namespace rad_ml {
namespace sim {

// Radiation levels for each environment (0.0-1.0)
static const std::unordered_map<Environment, double> radiation_levels = {
    {Environment::EARTH, 0.01},
    {Environment::EARTH_ORBIT, 0.2},
    {Environment::ISS, 0.25},
    {Environment::MOON, 0.4},
    {Environment::MARS, 0.3},
    {Environment::SOLAR_FLARE, 0.8},
    {Environment::DEEP_SPACE, 0.6},
    {Environment::JUPITER, 0.85},
    {Environment::SAA, 0.45},
    {Environment::EXTREME, 1.0},
    {Environment::CUSTOM, 0.5}  // Default for custom
};

// Bit error rates for each environment
static const std::unordered_map<Environment, double> bit_error_rates = {
    {Environment::EARTH, 1e-12},
    {Environment::EARTH_ORBIT, 1e-8},
    {Environment::ISS, 5e-8},
    {Environment::MOON, 1e-7},
    {Environment::MARS, 8e-8},
    {Environment::SOLAR_FLARE, 1e-6},
    {Environment::DEEP_SPACE, 5e-7},
    {Environment::JUPITER, 1e-5},
    {Environment::SAA, 2e-7},
    {Environment::EXTREME, 1e-4},
    {Environment::CUSTOM, 1e-6}  // Default for custom
};

// Expected MCU cluster sizes for each environment
static const std::unordered_map<Environment, int> mcu_sizes = {
    {Environment::EARTH, 1},
    {Environment::EARTH_ORBIT, 2},
    {Environment::ISS, 2},
    {Environment::MOON, 3},
    {Environment::MARS, 2},
    {Environment::SOLAR_FLARE, 4},
    {Environment::DEEP_SPACE, 3},
    {Environment::JUPITER, 5},
    {Environment::SAA, 3},
    {Environment::EXTREME, 8},
    {Environment::CUSTOM, 3}  // Default for custom
};

// Human-readable names for each environment
static const std::unordered_map<Environment, std::string> environment_names = {
    {Environment::EARTH, "Earth Surface"},
    {Environment::EARTH_ORBIT, "Low Earth Orbit"},
    {Environment::ISS, "International Space Station"},
    {Environment::MOON, "Lunar Surface"},
    {Environment::MARS, "Mars Surface"},
    {Environment::SOLAR_FLARE, "Solar Flare"},
    {Environment::DEEP_SPACE, "Deep Space"},
    {Environment::JUPITER, "Jupiter Orbit"},
    {Environment::SAA, "South Atlantic Anomaly"},
    {Environment::EXTREME, "Extreme Radiation"},
    {Environment::CUSTOM, "Custom Environment"}
};

double getRadiationLevel(Environment env) {
    auto it = radiation_levels.find(env);
    if (it != radiation_levels.end()) {
        return it->second;
    }
    return 0.0;  // Default
}

double getBitErrorRate(Environment env) {
    auto it = bit_error_rates.find(env);
    if (it != bit_error_rates.end()) {
        return it->second;
    }
    return 1e-12;  // Default to Earth level
}

int getExpectedMCUSize(Environment env) {
    auto it = mcu_sizes.find(env);
    if (it != mcu_sizes.end()) {
        return it->second;
    }
    return 1;  // Default
}

std::string getEnvironmentName(Environment env) {
    auto it = environment_names.find(env);
    if (it != environment_names.end()) {
        return it->second;
    }
    return "Unknown Environment";
}

RadiationEnvironment getRadiationEnvironment(Environment env) {
    RadiationEnvironment rad_env;
    
    switch (env) {
        case Environment::EARTH:
            rad_env.altitude = 0;
            rad_env.distance_from_sun = 1.0;
            rad_env.gcr_intensity = 0.01;
            rad_env.solar_activity = 0.5;
            rad_env.trapped_proton_flux = 0.0;
            rad_env.trapped_electron_flux = 0.0;
            rad_env.saa_region = false;
            rad_env.atmosphere_depth = 1000.0;
            rad_env.magnetic_field_strength = 1.0;
            rad_env.temperature = {263.0, 293.0, 24.0};
            break;
            
        case Environment::MARS:
            rad_env.altitude = 0;
            rad_env.distance_from_sun = 1.52;
            rad_env.gcr_intensity = 0.4;
            rad_env.solar_activity = 0.5;
            rad_env.trapped_proton_flux = 0.0;
            rad_env.trapped_electron_flux = 0.0;
            rad_env.saa_region = false;
            rad_env.atmosphere_depth = 20.0;
            rad_env.magnetic_field_strength = 0.01;
            rad_env.temperature = {150.0, 290.0, 24.7};
            break;
            
        case Environment::MOON:
            rad_env.altitude = 0;
            rad_env.distance_from_sun = 1.0;
            rad_env.gcr_intensity = 0.7;
            rad_env.solar_activity = 0.5;
            rad_env.trapped_proton_flux = 0.0;
            rad_env.trapped_electron_flux = 0.0;
            rad_env.saa_region = false;
            rad_env.atmosphere_depth = 0.0;
            rad_env.magnetic_field_strength = 0.0;
            rad_env.temperature = {100.0, 390.0, 655.0};
            break;
            
        case Environment::EARTH_ORBIT:
            rad_env.altitude = 400;
            rad_env.distance_from_sun = 1.0;
            rad_env.gcr_intensity = 0.2;
            rad_env.solar_activity = 0.5;
            rad_env.trapped_proton_flux = 1e3;
            rad_env.trapped_electron_flux = 1e5;
            rad_env.saa_region = false;
            rad_env.atmosphere_depth = 0.0;
            rad_env.magnetic_field_strength = 0.8;
            rad_env.temperature = {173.0, 373.0, 1.5}; // 90 min orbit
            break;
            
        // Add more environments as needed
        
        default:
            // Default to Earth conditions
            rad_env.altitude = 0;
            rad_env.distance_from_sun = 1.0;
            rad_env.gcr_intensity = 0.01;
            rad_env.solar_activity = 0.5;
            rad_env.trapped_proton_flux = 0.0;
            rad_env.trapped_electron_flux = 0.0;
            rad_env.saa_region = false;
            rad_env.atmosphere_depth = 1000.0;
            rad_env.magnetic_field_strength = 1.0;
            rad_env.temperature = {263.0, 293.0, 24.0};
            break;
    }
    
    return rad_env;
}

} // namespace sim
} // namespace rad_ml 