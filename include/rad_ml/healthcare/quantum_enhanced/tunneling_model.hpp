#ifndef RAD_ML_HEALTHCARE_QUANTUM_ENHANCED_TUNNELING_MODEL_HPP
#define RAD_ML_HEALTHCARE_QUANTUM_ENHANCED_TUNNELING_MODEL_HPP

#include <complex>
#include <functional>
#include <memory>
#include <vector>

namespace rad_ml {
namespace healthcare {
namespace quantum_enhanced {

/**
 * Enumeration for different tunneling calculation methods
 */
enum class TunnelingMethod {
    SIMPLE_BARRIER,     // Simple rectangular barrier tunneling
    WKB_APPROXIMATION,  // Wentzel–Kramers–Brillouin approximation
    TRANSFER_MATRIX,    // Transfer matrix method for multi-barrier systems
    RESONANT_TUNNELING  // Resonant tunneling for quantum wells
};

/**
 * Barrier shape types for tunneling calculations
 */
enum class BarrierShape {
    RECTANGULAR,  // Simple step function
    TRIANGULAR,   // Linear ramp potential
    PARABOLIC,    // Quadratic potential
    GAUSSIAN,     // Gaussian shape barrier
    CUSTOM        // User-defined potential shape
};

/**
 * Configuration for the tunneling model
 */
struct TunnelingConfig {
    TunnelingMethod method = TunnelingMethod::WKB_APPROXIMATION;
    BarrierShape barrier_shape = BarrierShape::RECTANGULAR;
    double energy_resolution = 0.001;  // eV
    int integration_points = 1000;
    bool include_zero_point = true;   // Include zero-point energy effects
    bool include_temperature = true;  // Include temperature effects
    bool include_resonance = true;    // Include resonant tunneling effects
};

/**
 * Structure to define a single potential barrier
 */
struct PotentialBarrier {
    double position = 0.0;  // nm
    double width = 1.0;     // nm
    double height = 0.5;    // eV
    BarrierShape shape = BarrierShape::RECTANGULAR;

    // For custom shapes, provide a function that returns potential at position x
    std::function<double(double)> custom_shape_function;
};

/**
 * Class for modeling quantum tunneling in biological systems with
 * advanced methods like WKB approximation and transfer matrix method
 */
class QuantumTunnelingModel {
   public:
    /**
     * Constructor with configuration parameters
     */
    explicit QuantumTunnelingModel(const TunnelingConfig& config);

    /**
     * Set the effective mass for the tunneling particle
     * @param effective_mass Effective mass in kg
     */
    void setEffectiveMass(double effective_mass);

    /**
     * Set the temperature for thermal effects
     * @param temperature Temperature in Kelvin
     */
    void setTemperature(double temperature);

    /**
     * Add a potential barrier to the model
     * @param barrier Barrier parameters
     */
    void addPotentialBarrier(const PotentialBarrier& barrier);

    /**
     * Add a membrane barrier based on biological parameters
     * @param thickness Membrane thickness in nm
     * @param dielectric_constant Dielectric constant of the membrane
     * @param surface_charge Surface charge density
     */
    void addMembraneBarrier(double thickness, double dielectric_constant, double surface_charge);

    /**
     * Add a protein binding site potential well
     * @param position Position of the well center
     * @param width Width of the well
     * @param depth Depth of the well in eV
     */
    void addBindingSiteWell(double position, double width, double depth);

    /**
     * Configure barriers for a typical cell membrane
     * @param lipid_thickness Lipid bilayer thickness
     * @param protein_density Membrane protein density
     * @param cholesterol_content Cholesterol content percentage
     */
    void setupCellMembraneBarriers(double lipid_thickness, double protein_density,
                                   double cholesterol_content);

    /**
     * Configure DNA base pair barriers
     * @param base_pair_type Type of base pair (AT or GC)
     * @param ionic_strength Ionic strength of the solution
     */
    void setupDNABarriers(const std::string& base_pair_type, double ionic_strength);

    /**
     * Calculate tunneling probability using the selected method
     * @param particle_energy Energy of the tunneling particle in eV
     * @return Tunneling probability
     */
    double calculateTunnelingProbability(double particle_energy);

    /**
     * Calculate Boltzmann-weighted tunneling probability across energy distribution
     * @return Thermally averaged tunneling probability
     */
    double calculateThermalTunnelingProbability();

    /**
     * Calculate tunneling probability with resonance effects
     * @param particle_energy Energy of the tunneling particle in eV
     * @return Resonant tunneling probability
     */
    double calculateResonantTunnelingProbability(double particle_energy);

    /**
     * Calculate tunneling rate (probability per time)
     * @param attempt_frequency Attempt frequency in Hz
     * @return Tunneling rate in Hz
     */
    double calculateTunnelingRate(double attempt_frequency);

    /**
     * Get the transmission coefficient as a function of energy
     * @param min_energy Minimum energy in eV
     * @param max_energy Maximum energy in eV
     * @param steps Number of energy steps
     * @return Vector of (energy, transmission) pairs
     */
    std::vector<std::pair<double, double>> getTransmissionSpectrum(double min_energy,
                                                                   double max_energy, int steps);

    /**
     * Get the potential profile for visualization
     * @param min_position Minimum position in nm
     * @param max_position Maximum position in nm
     * @param steps Number of position steps
     * @return Vector of (position, potential) pairs
     */
    std::vector<std::pair<double, double>> getPotentialProfile(double min_position,
                                                               double max_position, int steps);

   private:
    // Configuration
    TunnelingConfig config_;

    // Physical parameters
    double effective_mass_;
    double temperature_;
    double planck_constant_ = 6.62607015e-34;   // J⋅s
    double electron_charge_ = 1.602176634e-19;  // C
    double boltzmann_constant_ = 1.380649e-23;  // J/K

    // Potential barriers
    std::vector<PotentialBarrier> barriers_;

    // Implementation of the WKB approximation method
    double calculateWKBTunneling(double energy);

    // Implementation of the transfer matrix method
    double calculateTransferMatrixTunneling(double energy);

    // Implementation of simple barrier tunneling
    double calculateSimpleBarrierTunneling(double energy);

    // Implementation of resonant tunneling calculation
    double calculateResonantTunneling(double energy);

    // Calculate potential at a given position
    double calculatePotential(double position);

    // Find resonance energies in the system
    std::vector<double> findResonanceEnergies(double min_energy, double max_energy, int steps);

    // Calculate zero-point energy correction
    double calculateZeroPointCorrection();

    // Integrate the WKB exponent
    double integrateWKBExponent(double energy, double x1, double x2, int steps);
};

/**
 * Advanced function to calculate quantum tunneling probability through a biological barrier
 * @param barrier_height Barrier height in eV
 * @param effective_mass Effective mass in biological medium
 * @param hbar Reduced Planck constant
 * @param temperature Temperature in Kelvin
 * @return Tunneling probability
 */
double calculateBioQuantumTunnelingProbability(double barrier_height, double effective_mass,
                                               double hbar, double temperature);

/**
 * Calculate tunneling probability for drug diffusion across cell membrane
 * @param drug_radius Drug molecule radius in nm
 * @param membrane_thickness Membrane thickness in nm
 * @param temperature Temperature in K
 * @param dielectric_constant Membrane dielectric constant
 * @return Tunneling enhancement factor for drug diffusion
 */
double calculateDrugMembraneTunnelingFactor(double drug_radius, double membrane_thickness,
                                            double temperature, double dielectric_constant);

}  // namespace quantum_enhanced
}  // namespace healthcare
}  // namespace rad_ml

#endif  // RAD_ML_HEALTHCARE_QUANTUM_ENHANCED_TUNNELING_MODEL_HPP
