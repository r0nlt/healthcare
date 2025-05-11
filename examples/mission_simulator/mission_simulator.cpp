#include "rad_ml/tmr/enhanced_tmr.hpp"
#include "rad_ml/testing/radiation_simulator.hpp"
#include "rad_ml/testing/mission_simulator.hpp"
#include <array>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <string>
#include <random>
#include <cstring>

using namespace rad_ml::tmr;
using namespace rad_ml::testing;

// Simple mock neural network for demonstration
class SimpleNeuralNetwork {
private:
    // Use pointers to EnhancedTMR to avoid copy assignment issues due to mutex
    std::array<std::shared_ptr<rad_ml::tmr::EnhancedTMR<float>>, 10> weights_;
    std::chrono::milliseconds scrub_interval_{1000};
    bool recovery_enabled_ = false;
    uint32_t checkpoint_interval_s_ = 0;
    
public:
    SimpleNeuralNetwork() {
        // Initialize weights with random values
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        
        for (int i = 0; i < 10; ++i) {
            // Use the factory to create shared_ptr instances
            weights_[i] = rad_ml::tmr::TMRFactory::createEnhancedTMR<float>(dist(gen));
        }
    }
    
    // Getters and setters for configuration
    void setScrubInterval(std::chrono::milliseconds interval) {
        scrub_interval_ = interval;
        
        // Set verification interval for all weights
        for (auto& weight : weights_) {
            weight->setVerificationInterval(interval);
        }
        
        std::cout << "Scrub interval set to " << interval.count() << " ms" << std::endl;
    }
    
    void enableRecovery(uint32_t checkpoint_interval_s) {
        recovery_enabled_ = true;
        checkpoint_interval_s_ = checkpoint_interval_s;
        std::cout << "Recovery enabled with checkpoint interval of " 
                  << checkpoint_interval_s << " seconds" << std::endl;
    }
    
    // Perform a simple inference (just for demonstration)
    float runInference(float input) {
        float output = 0.0f;
        
        // Simple weighted sum with error handling for each weight
        for (int i = 0; i < 10; ++i) {
            try {
                if (weights_[i]) {
                    // Use tryGet to avoid exceptions
                    auto value = weights_[i]->tryGet();
                    if (value) {
                        output += (*value) * input;
                    }
                }
            } catch (...) {
                // Skip this weight if access fails
                std::cerr << "Warning: Skipping corrupted weight at index " << i << std::endl;
            }
        }
        
        return output;
    }
    
    // Repair all weights
    void repair() {
        for (auto& weight : weights_) {
            weight->regenerateCopies();
        }
    }
    
    // Get error statistics
    struct ErrorStats {
        size_t crc_validation_failures = 0;
        size_t voting_disagreements = 0;
    };
    
    ErrorStats getErrorStats() const {
        ErrorStats total;
        
        // In a real implementation, we would collect stats from all weights
        // For this demo we just use the first weight's stats
        try {
            // Try to get stats from the first weight that's available
            for (const auto& weight : weights_) {
                if (!weight) continue;
                
                try {
                    std::string stats_str = weight->getErrorStats();
                    
                    // Parse the error stats string to extract the values
                    if (stats_str.find("CRC validation failures: ") != std::string::npos) {
                        size_t pos = stats_str.find("CRC validation failures: ") + 
                                     std::string("CRC validation failures: ").length();
                        total.crc_validation_failures = std::stoul(stats_str.substr(pos));
                    }
                    
                    if (stats_str.find("Voting disagreements: ") != std::string::npos) {
                        size_t pos = stats_str.find("Voting disagreements: ") + 
                                     std::string("Voting disagreements: ").length();
                        total.voting_disagreements = std::stoul(stats_str.substr(pos));
                    }
                    
                    // If we successfully got stats from one weight, break the loop
                    break;
                } catch (const std::exception&) {
                    // If one weight fails, try the next one
                    continue;
                }
            }
        } catch (...) {
            // If all weights fail, return default values
            std::cerr << "Warning: Unable to read error statistics due to corrupted memory" << std::endl;
        }
        
        return total;
    }
    
    // Reset error statistics
    void resetErrorStats() {
        for (auto& weight : weights_) {
            try {
                if (weight) {
                    weight->resetErrorStats();
                }
            } catch (...) {
                // Ignore errors during reset
            }
        }
    }
    
    float getRawCopy(size_t index) const {
        try {
            if (index < weights_.size()) {
                if (weights_[index]) {
                    auto value = weights_[index]->tryGet();
                    if (value) {
                        return *value;
                    }
                }
            }
            return 0.0f; // Default value if anything goes wrong
        } catch (...) {
            return 0.0f; // Default value on any exception
        }
    }
};

// Print a horizontal line
void printSeparator() {
    std::cout << "\n" << std::setfill('-') << std::setw(80) << "" << "\n" << std::setfill(' ') << std::endl;
}

/**
 * Boost radiation rates for demonstration purposes
 * 
 * @param simulator The simulator to modify
 * @param boost_factor How much to boost radiation rates
 */
void boostRadiationRates(RadiationSimulator& simulator, float boost_factor) {
    // Get current environment params
    auto params = simulator.getSimulationEnvironment();
    
    // Artificially boost radiation by increasing solar activity
    params.solar_activity *= boost_factor;
    
    // Decrease shielding for even more radiation
    params.shielding_thickness_mm = std::max(0.5, params.shielding_thickness_mm / boost_factor);
    
    // Set South Atlantic Anomaly to true for boosted radiation
    params.inside_saa = true;
    
    // Update the simulator
    simulator.updateEnvironment(params);
    
    std::cout << "BOOSTED RADIATION RATES FOR DEMONSTRATION:" << std::endl;
    std::cout << simulator.getEnvironmentDescription() << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        std::cout << "Radiation-Tolerant Machine Learning Mission Simulator\n" 
                << "=====================================================" << std::endl;
        
        // Parse command line arguments for mission
        std::string mission_name = "LEO";
        if (argc > 1) {
            mission_name = argv[1];
        }
        
        // Create mission profile and adaptive protection configuration
        MissionProfile profile = MissionProfile::createStandard(mission_name);
        AdaptiveProtectionConfig protection_config = AdaptiveProtectionConfig::createStandard(mission_name);
        
        std::cout << "Creating mission profile for: " << mission_name << std::endl;
        
        // Create mission simulator
        MissionSimulator simulator(profile, protection_config);
        
        // Boost radiation rates for demonstration purposes
        boostRadiationRates(simulator.getRadiationSimulator(), 50.0f);
        
        // Print mission details
        printSeparator();
        std::cout << "Mission Environment Description:" << std::endl;
        std::cout << simulator.getRadiationSimulator().getEnvironmentDescription() << std::endl;
        printSeparator();
        
        // Create neural network
        std::cout << "Initializing neural network..." << std::endl;
        SimpleNeuralNetwork network;
        
        // Configure network based on mission profile
        std::string protection_level = simulator.getProtectionConfig().enable_tmr_medium ?
                            "MEDIUM" : (simulator.getProtectionConfig().enable_tmr_high ? "HIGH" : "LOW");
        
        std::cout << "Configuring network with protection level: " << protection_level << std::endl;
        
        // Set network protection based on mission profile
        network.setScrubInterval(std::chrono::milliseconds(5000));
        if (protection_level == "MEDIUM" || protection_level == "HIGH") {
            network.enableRecovery(300);
        }
        
        // Register memory regions with the simulator
        SimpleNeuralNetwork* network_ptr = &network;
        
        // Create static memory for weights to simulate
        static std::array<float, 10> weight_copies;
        
        // Instead of registering the entire network (which includes mutexes),
        // register each weight individually with static storage
        for (size_t i = 0; i < 10; i++) {
            try {
                // Store a copy of the value in our static array
                weight_copies[i] = network.getRawCopy(i);
                
                // Register the address of the static copy
                simulator.registerMemoryRegion(
                    static_cast<void*>(&weight_copies[i]),
                    sizeof(float),
                    true
                );
            } catch (...) {
                std::cerr << "Warning: Error registering weight " << i << " for simulation" << std::endl;
            }
        }
        
        // Run simulated mission
        printSeparator();
        std::cout << "Beginning mission simulation...\n" << std::endl;
        
        // Run the simulation with a callback for environment changes
        auto stats = simulator.runSimulation(
            std::chrono::seconds(30),  // Simulate 30 seconds of mission time
            std::chrono::seconds(3),   // Time step of 3 seconds
            [&network](const RadiationSimulator::EnvironmentParams& env) {
                try {
                    std::cout << "\nEnvironment changed to: " << env.mission_name << std::endl;
                    
                    // In a real system, we would adapt protection based on environment
                    if (env.inside_saa || env.solar_activity > 5.0) {
                        std::cout << "High radiation detected - increasing protection" << std::endl;
                        network.setScrubInterval(std::chrono::milliseconds(500));
                        network.enableRecovery(60);
                    } else {
                        std::cout << "Normal radiation levels - standard protection" << std::endl;
                        network.setScrubInterval(std::chrono::milliseconds(5000));
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error adapting to environment change: " << e.what() << std::endl;
                } catch (...) {
                    std::cerr << "Unknown error during environment adaptation" << std::endl;
                }
            }
        );
        
        // Print mission statistics
        printSeparator();
        std::cout << "Mission Complete - Final Statistics" << std::endl;
        printSeparator();
        std::cout << stats.getReport() << std::endl;
        
        // Test the neural network after the mission
        std::cout << "Testing neural network after mission..." << std::endl;
        try {
            float input = 0.5f;
            float output = network.runInference(input);
            std::cout << "Inference result: " << input << " → " << output << std::endl;
            
            // Print network error statistics
            auto network_stats = network.getErrorStats();
            std::cout << "Neural network error statistics:" << std::endl;
            std::cout << "  CRC validation failures: " << network_stats.crc_validation_failures << std::endl;
            std::cout << "  Voting disagreements: " << network_stats.voting_disagreements << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error testing neural network: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error occurred when testing neural network" << std::endl;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error in simulator: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error in simulator" << std::endl;
        return 1;
    }
} 