#ifndef RAD_ML_QUANTUM_ZPE_MODEL_HPP
#define RAD_ML_QUANTUM_ZPE_MODEL_HPP

#include <cmath>

namespace rad_ml {
namespace quantum {

/**
 * Calculates the pure zero-point energy (temperature-independent)
 *
 * @param hbar Reduced Planck constant
 * @param mass Particle mass
 * @param force_constant Spring constant for the harmonic oscillator model
 * @return Pure ZPE value
 */
double calculatePureZPE(double hbar, double mass, double force_constant)
{
    double omega = std::sqrt(force_constant / mass);
    return 0.5 * hbar * omega;
}

/**
 * Calculates the thermal quantum correction (temperature-dependent)
 *
 * @param hbar Reduced Planck constant
 * @param mass Particle mass
 * @param force_constant Spring constant for the harmonic oscillator model
 * @param temperature System temperature in Kelvin
 * @param kb Boltzmann constant
 * @return Thermal quantum correction value
 */
double calculateThermalQuantumCorrection(double hbar, double mass, double force_constant,
                                         double temperature, double kb = 1.380649e-23)
{
    double omega = std::sqrt(force_constant / mass);
    double beta = 1.0 / (kb * temperature);

    // Avoid division by zero for very low temperatures
    if (temperature < 0.01) {
        return 0.0;
    }

    // Bose-Einstein distribution contribution
    return hbar * omega / (std::exp(hbar * omega * beta) - 1.0);
}

/**
 * Calculates the total quantum contribution by combining pure ZPE and thermal corrections
 *
 * @param hbar Reduced Planck constant
 * @param mass Particle mass
 * @param force_constant Spring constant for the harmonic oscillator model
 * @param temperature System temperature in Kelvin
 * @param kb Boltzmann constant
 * @return Total quantum contribution (ZPE + thermal)
 */
double getTotalQuantumContribution(double hbar, double mass, double force_constant,
                                   double temperature, double kb = 1.380649e-23)
{
    return calculatePureZPE(hbar, mass, force_constant) +
           calculateThermalQuantumCorrection(hbar, mass, force_constant, temperature, kb);
}

/**
 * Calculate force constant from lattice constant for crystalline systems
 *
 * @param lattice_constant Lattice constant in meters
 * @param youngs_modulus Young's modulus in Pa
 * @return Force constant in N/m
 */
double calculateForceConstantFromLattice(double lattice_constant, double youngs_modulus)
{
    // Simple approximation: k ≈ E·A/L where E is Young's modulus, A is cross-section, L is length
    // For unit cube: A = L² so k ≈ E·L
    return youngs_modulus * lattice_constant;
}

/**
 * Factory class for different material models to calculate force constants
 */
class MaterialModelFactory {
   public:
    enum MaterialType { SILICON, GERMANIUM, GALLIUM_ARSENIDE, PROTEIN, DNA, WATER, CUSTOM };

    /**
     * Get appropriate force constant for a material
     *
     * @param material_type Type of material
     * @param lattice_constant or molecular size in meters
     * @return Force constant in N/m
     */
    static double getForceConstant(MaterialType material_type, double characteristic_length)
    {
        switch (material_type) {
            case SILICON:
                return 1.3e11 * characteristic_length;  // Approximation using Si Young's modulus
            case GERMANIUM:
                return 1.0e11 * characteristic_length;
            case GALLIUM_ARSENIDE:
                return 8.5e10 * characteristic_length;
            case PROTEIN:
                return 1.0e9 * characteristic_length;  // Softer than crystalline materials
            case DNA:
                return 3.0e8 * characteristic_length;
            case WATER:
                return 2.2e9 * characteristic_length;
            case CUSTOM:
            default:
                return 0.0;  // Must be set explicitly
        }
    }
};

}  // namespace quantum
}  // namespace rad_ml

#endif  // RAD_ML_QUANTUM_ZPE_MODEL_HPP
