#include "rad_ml/healthcare/chemotherapy/drug_target_interaction.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace rad_ml {
namespace healthcare {
namespace chemotherapy {

// Constructor
DrugTargetInteractionModel::DrugTargetInteractionModel(const ChemotherapeuticAgent& drug,
                                                       const BiologicalSystem& tissue)
    : drug_(drug), tissue_(tissue)
{
    // Default quantum configuration
    quantum_config_.enable_tunneling_effects = true;
    quantum_config_.enable_binding_enhancement = true;
    quantum_config_.enable_zero_point_contribution = true;
    quantum_config_.temperature = 310.0;         // Body temperature in K
    quantum_config_.ph_value = 7.4;              // Physiological pH
    quantum_config_.ionic_strength = 0.15;       // Physiological ionic strength
    quantum_config_.dielectric_constant = 80.0;  // Water at body temperature

    // Initialize quantum parameters with default temperature
    initializeQuantumParameters(quantum_config_.temperature);
}

// Set drug quantum configuration
void DrugTargetInteractionModel::setDrugQuantumConfig(const DrugQuantumConfig& config)
{
    quantum_config_ = config;
    initializeQuantumParameters(quantum_config_.temperature);
}

// Initialize quantum parameters
void DrugTargetInteractionModel::initializeQuantumParameters(double temperature)
{
    // Physical constants
    const double BOLTZMANN_CONSTANT = 8.617333262e-5;    // eV/K
    const double PLANCK_CONSTANT = 4.135667696e-15;      // eV⋅s
    const double HBAR = PLANCK_CONSTANT / (2.0 * M_PI);  // reduced Planck constant
    const double ELEMENTARY_CHARGE = 1.602176634e-19;    // C

    // Calculate tunneling probability - using membrane as default barrier
    double barrier_height = 0.28;   // eV - membrane barrier
    double barrier_width = 8.0e-9;  // m - typical membrane thickness
    quantum_parameters_.tunneling_probability =
        calculateTunnelingProbability(barrier_height, barrier_width, temperature);

    // Calculate quantum enhancement factor
    quantum_parameters_.binding_enhancement =
        1.0 + drug_.quantum_tunneling_factor * std::exp(-0.0002 * (temperature - 310.0));

    // Calculate zero-point contribution
    // Zero-point energy for typical molecular vibration (~10 THz)
    double zero_point_energy = 0.5 * HBAR * 1e13;
    double thermal_energy = BOLTZMANN_CONSTANT * temperature;
    quantum_parameters_.zero_point_contribution = zero_point_energy / thermal_energy;

    // Calculate thermal contribution
    quantum_parameters_.thermal_contribution =
        std::sqrt(2.0 * M_PI * thermal_energy / (drug_.binding_energy * ELEMENTARY_CHARGE));

    // Calculate quantum coherence length
    double mass = drug_.molecular_weight * 1.66053886e-27;  // kg
    double velocity = std::sqrt(3.0 * BOLTZMANN_CONSTANT * temperature * ELEMENTARY_CHARGE / mass);
    quantum_parameters_.quantum_coherence_length = HBAR / (mass * velocity) * 1e9;  // nm
}

// Calculate quantum-enhanced binding probability
double DrugTargetInteractionModel::calculateQuantumEnhancedBinding(double temperature,
                                                                   double cell_size,
                                                                   double drug_concentration)
{
    // If temperature changed, recalculate quantum parameters
    if (std::abs(temperature - quantum_config_.temperature) > 0.1) {
        initializeQuantumParameters(temperature);
    }

    // Cell size effect on binding
    double size_factor = 1.0;
    if (cell_size < 15.0) {  // For smaller cells, quantum effects are more pronounced
        size_factor = 1.0 + 0.2 * ((15.0 - cell_size) / 15.0);
    }

    // Get average binding energy across all targets
    double avg_binding_energy = 0.0;
    for (const auto& target : drug_.target_affinities) {
        avg_binding_energy += target.second;
    }
    avg_binding_energy /= drug_.target_affinities.size();

    // Classical binding probability
    double classical_binding =
        calculateClassicalBinding(drug_concentration, avg_binding_energy, temperature);

    // Apply quantum corrections
    double quantum_binding =
        applyQuantumCorrections(classical_binding, temperature, avg_binding_energy);

    return quantum_binding * size_factor;
}

// Apply quantum corrections to classical binding
double DrugTargetInteractionModel::applyQuantumCorrections(double classical_binding_probability,
                                                           double temperature,
                                                           double target_barrier_height)
{
    // If quantum effects disabled, return classical result
    if (!quantum_config_.enable_binding_enhancement && !quantum_config_.enable_tunneling_effects &&
        !quantum_config_.enable_zero_point_contribution) {
        return classical_binding_probability;
    }

    double quantum_binding = classical_binding_probability;

    // Apply binding enhancement if enabled
    if (quantum_config_.enable_binding_enhancement) {
        quantum_binding *= quantum_parameters_.binding_enhancement;
    }

    // Apply tunneling effects if enabled
    if (quantum_config_.enable_tunneling_effects) {
        // Calculate tunneling probability through this specific barrier
        double tunneling_width = 0.2e-9;  // typical bond length ~0.2 nm
        double tunneling_prob =
            calculateTunnelingProbability(target_barrier_height, tunneling_width, temperature);

        // Add tunneling contribution to binding
        quantum_binding += tunneling_prob * (1.0 - classical_binding_probability);
    }

    // Apply zero-point energy contribution if enabled
    if (quantum_config_.enable_zero_point_contribution) {
        // Zero-point energy effectively lowers the barrier
        double zpe_factor = 1.0 + 0.5 * quantum_parameters_.zero_point_contribution;
        quantum_binding *= zpe_factor;
    }

    // Ensure binding probability is in valid range [0,1]
    return std::max(0.0, std::min(1.0, quantum_binding));
}

// Model the time-dependent binding dynamics
std::vector<double> DrugTargetInteractionModel::modelBindingDynamics(double initial_concentration,
                                                                     double time_hours,
                                                                     double time_step_hours)
{
    // Calculate number of time steps
    int num_steps = static_cast<int>(time_hours / time_step_hours) + 1;
    std::vector<double> binding_curve(num_steps, 0.0);

    for (int i = 0; i < num_steps; ++i) {
        double current_time = i * time_step_hours;

        // Calculate concentration at this time point (accounting for clearance)
        double current_concentration =
            initial_concentration *
            std::exp(-std::log(2.0) * current_time / drug_.clearance_half_life);

        // Calculate intracellular concentration at this time
        double intracellular_conc =
            calculateIntracellularConcentration(current_concentration, current_time);

        // Calculate binding at this time point
        if (drug_.target_affinities.find("dna") != drug_.target_affinities.end()) {
            binding_curve[i] = calculateQuantumEnhancedBinding(quantum_config_.temperature, 15.0,
                                                               intracellular_conc);
        }
        else {
            // Use first available target if no DNA target
            binding_curve[i] = calculateQuantumEnhancedBinding(quantum_config_.temperature, 15.0,
                                                               intracellular_conc) *
                               0.8;
        }
    }

    return binding_curve;
}

// Calculate intracellular drug concentration
double DrugTargetInteractionModel::calculateIntracellularConcentration(
    double extracellular_concentration, double time_hours)
{
    // Calculate membrane permeability
    double membrane_thickness = 8.0;  // nm
    double permeability =
        calculateMembraneDiffusion(1.0, membrane_thickness, quantum_config_.temperature);

    // Parameters for simplified intracellular kinetics
    double k_in = permeability;                    // Rate of drug entry
    double k_out = 0.01;                           // Rate of drug efflux
    double k_bind = 0.1;                           // Rate of binding to intracellular targets
    double partition = std::pow(10, drug_.log_p);  // Partition coefficient from log_p

    // Calculate equilibrium ratio (would be reached at infinite time)
    double equilibrium_ratio = partition * k_in / (k_out + k_bind);

    // Time-dependent approach to equilibrium
    double rate_constant = k_in + k_out + k_bind;
    double approach_factor = 1.0 - std::exp(-rate_constant * time_hours);

    return extracellular_concentration * equilibrium_ratio * approach_factor;
}

// Calculate drug transport across membrane
double DrugTargetInteractionModel::calculateMembraneDiffusion(double concentration_gradient,
                                                              double membrane_thickness_nm,
                                                              double temperature)
{
    // Base diffusion coefficient using Stokes-Einstein
    double r_h =
        std::cbrt(3.0 * drug_.molecular_weight / (4.0 * M_PI * 6.022e23));  // Hydrodynamic radius
    double viscosity = 0.001;                                               // Pa·s, water at ~37°C
    double d_coeff = 1.38064852e-23 * temperature / (6.0 * M_PI * viscosity * r_h);

    // Membrane permeability using diffusion and partitioning
    double partition_coeff = std::pow(10, drug_.log_p);
    double classical_perm = d_coeff * partition_coeff / (membrane_thickness_nm * 1e-9);

    // Add quantum effects if enabled
    if (quantum_config_.enable_tunneling_effects) {
        double barrier_height = 0.28;  // eV - membrane barrier
        double barrier_width = membrane_thickness_nm * 1e-9;
        double tunneling_prob =
            calculateTunnelingProbability(barrier_height, barrier_width, temperature);

        double quantum_perm =
            classical_perm * (1.0 + drug_.quantum_tunneling_factor) + d_coeff * tunneling_prob;

        return quantum_perm * concentration_gradient;
    }
    else {
        return classical_perm * concentration_gradient;
    }
}

// Get drug response for specific tissue
ChemoDrugResponse DrugTargetInteractionModel::getDrugResponse(double extracellular_concentration,
                                                              double time_hours, double temperature)
{
    ChemoDrugResponse response;

    // Calculate membrane permeability
    double membrane_thickness = 8.0;  // nm
    double permeability = calculateMembraneDiffusion(1.0, membrane_thickness, temperature);
    response.membrane_permeability = permeability;

    // Calculate intracellular concentration
    response.intracellular_concentration =
        calculateIntracellularConcentration(extracellular_concentration, time_hours);

    // Calculate binding rate to DNA or primary target
    if (drug_.target_affinities.find("dna") != drug_.target_affinities.end()) {
        double binding_energy = drug_.target_affinities.at("dna");
        double classical_binding = calculateClassicalBinding(response.intracellular_concentration,
                                                             binding_energy, temperature);
        response.dna_binding_rate =
            applyQuantumCorrections(classical_binding, temperature, binding_energy);
    }
    else {
        // Use first target
        const auto& first_target = *drug_.target_affinities.begin();
        double binding_energy = first_target.second;
        double classical_binding = calculateClassicalBinding(response.intracellular_concentration,
                                                             binding_energy, temperature);
        response.dna_binding_rate =
            applyQuantumCorrections(classical_binding, temperature, binding_energy) * 0.5;
    }

    // Calculate metabolic degradation rate
    double water_factor = 1.0 + 0.5 * tissue_.water_content;
    response.metabolic_degradation_rate = std::log(2.0) / drug_.clearance_half_life * water_factor;

    // Quantum enhancement factor for binding
    response.quantum_enhanced_binding = quantum_parameters_.binding_enhancement;

    // Calculate damage to various cellular components
    response.cellular_damage =
        calculateCellularDamage(response.dna_binding_rate, response.intracellular_concentration);

    return response;
}

// Tunneling probability through cellular barriers
double DrugTargetInteractionModel::calculateTunnelingProbability(double barrier_height,
                                                                 double barrier_width,
                                                                 double temperature)
{
    // Physical constants
    const double BOLTZMANN_CONSTANT = 8.617333262e-5;    // eV/K
    const double PLANCK_CONSTANT = 4.135667696e-15;      // eV⋅s
    const double HBAR = PLANCK_CONSTANT / (2.0 * M_PI);  // reduced Planck constant
    const double ELEMENTARY_CHARGE = 1.602176634e-19;    // C

    // Convert energy from eV to J
    double barrier_energy = barrier_height * ELEMENTARY_CHARGE;

    // Calculate mass
    double mass = drug_.molecular_weight * 1.66053886e-27;  // kg

    // Calculate wavenumber
    double k = std::sqrt(2.0 * mass * barrier_energy) / HBAR;

    // WKB approximation for tunneling probability
    double tunneling_prob = std::exp(-2.0 * k * barrier_width);

    // Add thermal contribution
    double thermal_energy = BOLTZMANN_CONSTANT * temperature * ELEMENTARY_CHARGE;
    double thermal_factor = std::exp(-barrier_energy / thermal_energy);

    // Ensure probability is in valid range [0,1]
    return std::min(1.0, tunneling_prob + 0.1 * thermal_factor);
}

// Quantum correction factor based on molecular properties
double DrugTargetInteractionModel::quantumCorrectionFactor(double molecular_weight,
                                                           double temperature)
{
    // Heavier molecules have smaller quantum effects
    double weight_factor = std::exp(-molecular_weight / 1000.0);

    // Temperature effect - higher temperature reduces quantum effects
    double temp_factor = std::exp(-0.0002 * (temperature - 310.0));

    // Combine factors
    return drug_.quantum_tunneling_factor * weight_factor * temp_factor;
}

// Calculate binding energy for specific target
double DrugTargetInteractionModel::getTargetBindingEnergy(const std::string& target)
{
    auto it = drug_.target_affinities.find(target);
    if (it != drug_.target_affinities.end()) {
        return it->second;
    }

    // If target not found, use average binding energy
    double sum = 0.0;
    for (const auto& pair : drug_.target_affinities) {
        sum += pair.second;
    }
    return sum / drug_.target_affinities.size();
}

// Calculate classical binding using standard models
double DrugTargetInteractionModel::calculateClassicalBinding(double concentration,
                                                             double binding_energy,
                                                             double temperature)
{
    // Physical constants
    const double BOLTZMANN_CONSTANT = 8.617333262e-5;  // eV/K

    // Simplified binding model based on Boltzmann distribution and concentration
    double kT = BOLTZMANN_CONSTANT * temperature;
    double boltzmann_factor = std::exp(-binding_energy / kT);

    // Concentration-dependent binding (simplified Hill equation)
    double k_d = 0.1;  // Dissociation constant (μmol/L)
    double binding = concentration / (concentration + k_d);

    // Combined binding probability
    return binding * boltzmann_factor;
}

// Calculate cellular damage from bound drug
std::map<std::string, double> DrugTargetInteractionModel::calculateCellularDamage(
    double bound_fraction, double intracellular_concentration)
{
    std::map<std::string, double> damage;

    // DNA damage - depends on binding and specific targeting
    bool targets_dna = (drug_.target_affinities.find("dna") != drug_.target_affinities.end());
    if (targets_dna) {
        damage["dna_damage"] = bound_fraction * 0.9;  // Higher damage if directly targeting DNA
    }
    else {
        damage["dna_damage"] = bound_fraction * 0.4;  // Lower indirect DNA damage
    }

    // Protein damage
    damage["protein_damage"] =
        intracellular_concentration * 0.3 / (intracellular_concentration + 0.5);

    // Membrane damage
    bool targets_membrane =
        (drug_.target_affinities.find("cell_membrane") != drug_.target_affinities.end());
    if (targets_membrane) {
        damage["membrane_damage"] = bound_fraction * 0.7;
    }
    else {
        damage["membrane_damage"] =
            intracellular_concentration * 0.2 / (intracellular_concentration + 1.0);
    }

    // Mitochondrial damage
    bool targets_mitochondria =
        (drug_.target_affinities.find("mitochondria") != drug_.target_affinities.end());
    if (targets_mitochondria) {
        damage["mitochondrial_damage"] = bound_fraction * 0.8;
    }
    else {
        damage["mitochondrial_damage"] =
            intracellular_concentration * 0.25 / (intracellular_concentration + 0.7);
    }

    return damage;
}

}  // namespace chemotherapy
}  // namespace healthcare
}  // namespace rad_ml
