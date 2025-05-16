// Implementation file for cell_biology/repair_kinetics.cpp
// TODO: Implement this component

#include "rad_ml/healthcare/cell_biology/repair_kinetics.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace rad_ml {
namespace healthcare {
namespace cell_biology {

// Constructor
RepairKinetics::RepairKinetics(const BiologicalSystemExtended& biosystem) : biosystem_(biosystem)
{
    // Set default parameters
    repair_params_.fast_repair_half_time = 0.5;           // hours
    repair_params_.slow_repair_half_time = 2.5;           // hours
    repair_params_.fast_repair_fraction = 0.7;            // fraction repaired by fast component
    repair_params_.mis_repair_probability = 0.05;         // probability of misrepair
    repair_params_.lethal_mis_repair_probability = 0.01;  // probability of lethal misrepair
}

// Calculate repair fraction as a function of time
double RepairKinetics::calculateRepairFraction(double time) const
{
    // Calculate repair fractions for fast and slow components
    double fast_repair_constant = std::log(2.0) / repair_params_.fast_repair_half_time;
    double slow_repair_constant = std::log(2.0) / repair_params_.slow_repair_half_time;

    double fast_repair_fraction = repair_params_.fast_repair_fraction;
    double slow_repair_fraction = 1.0 - fast_repair_fraction;

    double fast_repair_factor = 1.0 - std::exp(-fast_repair_constant * time);
    double slow_repair_factor = 1.0 - std::exp(-slow_repair_constant * time);

    // Total repair fraction
    return fast_repair_fraction * fast_repair_factor + slow_repair_fraction * slow_repair_factor;
}

// Calculate residual damage over time
DNADamageProfile RepairKinetics::calculateResidualDamage(const DNADamageProfile& initial_damage,
                                                         double time) const
{
    DNADamageProfile residual_damage;

    // Calculate repair fraction
    double repair_fraction = calculateRepairFraction(time);

    // Calculate residual DSBs
    residual_damage.double_strand_breaks =
        static_cast<int>(std::round(initial_damage.double_strand_breaks * (1.0 - repair_fraction)));

    // DSBs are harder to repair, so adjust repair fraction for complex DSBs
    double complex_repair_fraction = repair_fraction * 0.8;  // 20% less repair for complex DSBs
    residual_damage.complex_dsb =
        static_cast<int>(std::round(initial_damage.complex_dsb * (1.0 - complex_repair_fraction)));

    // SSBs are easier to repair
    double ssb_repair_fraction = std::min(1.0, repair_fraction * 1.2);  // 20% more repair for SSBs
    residual_damage.single_strand_breaks = static_cast<int>(
        std::round(initial_damage.single_strand_breaks * (1.0 - ssb_repair_fraction)));

    // Base damages
    double base_repair_fraction = std::min(1.0, repair_fraction * 1.1);  // 10% more repair
    residual_damage.base_damages =
        static_cast<int>(std::round(initial_damage.base_damages * (1.0 - base_repair_fraction)));

    // Clustered damages are harder to repair
    double clustered_repair_fraction = repair_fraction * 0.7;  // 30% less repair
    residual_damage.clustered_damages = static_cast<int>(
        std::round(initial_damage.clustered_damages * (1.0 - clustered_repair_fraction)));

    return residual_damage;
}

// Calculate survival fraction based on residual damage
double RepairKinetics::calculateSurvivalFraction(const DNADamageProfile& initial_damage,
                                                 double repair_time) const
{
    // Calculate residual damage
    DNADamageProfile residual_damage = calculateResidualDamage(initial_damage, repair_time);

    // Calculate correct repairs, misrepairs, and lethal misrepairs
    double total_dsb = initial_damage.double_strand_breaks;
    double dsb_repaired =
        initial_damage.double_strand_breaks - residual_damage.double_strand_breaks;

    double correct_repairs = dsb_repaired * (1.0 - repair_params_.mis_repair_probability);
    double misrepairs = dsb_repaired * repair_params_.mis_repair_probability;
    double lethal_misrepairs = misrepairs * repair_params_.lethal_mis_repair_probability;

    // Each residual DSB has a probability of being lethal
    double lethal_residual_dsb = residual_damage.double_strand_breaks * 0.2;

    // Complex DSBs are more likely to be lethal
    double lethal_complex_dsb = residual_damage.complex_dsb * 0.5;

    // Total lethal lesions
    double lethal_lesions = lethal_misrepairs + lethal_residual_dsb + lethal_complex_dsb;

    // Survival using Poisson statistics
    return std::exp(-lethal_lesions);
}

// Set repair parameters
void RepairKinetics::setRepairParameters(const RepairParameters& params)
{
    repair_params_ = params;
}

// Modify repair parameters based on modifiers
void RepairKinetics::applyRepairModifiers(double oxygen_modifier, double temperature_modifier)
{
    // Oxygen affects repair (hypoxia can reduce repair efficiency)
    double oxygen_tension = biosystem_.oxygen_tension;
    double oxygen_effect = 1.0;

    if (oxygen_tension < 5.0) {
        // Hypoxia reduces repair efficiency
        oxygen_effect = 0.7 + 0.3 * (oxygen_tension / 5.0);
    }

    // Temperature affects repair enzyme activity
    double temperature_effect = 1.0;
    if (temperature_modifier != 1.0) {
        // 37°C is normal - higher temps increase repair up to a point, then decrease
        if (temperature_modifier > 1.0 && temperature_modifier < 1.1) {
            // Higher temperature increases repair up to 10% above normal
            temperature_effect = temperature_modifier;
        }
        else if (temperature_modifier >= 1.1) {
            // Too high temperatures impair repair
            temperature_effect = 1.1 - 0.5 * (temperature_modifier - 1.1);
        }
        else {
            // Lower temperatures decrease repair
            temperature_effect = temperature_modifier;
        }
    }

    // Apply modifiers
    double total_modifier = oxygen_effect * temperature_effect * oxygen_modifier;

    // Adjust repair parameters
    repair_params_.fast_repair_half_time /= total_modifier;  // Faster repair = lower half-time
    repair_params_.slow_repair_half_time /= total_modifier;

    // More efficient repair = lower misrepair probability
    repair_params_.mis_repair_probability /= std::sqrt(total_modifier);
    repair_params_.lethal_mis_repair_probability /= std::sqrt(total_modifier);
}

// Calculate maximum repair capacity
double RepairKinetics::calculateRepairCapacity() const
{
    // Simple model of repair capacity
    double base_capacity = 100.0;  // DSBs per hour

    // Adjust based on cell type
    switch (biosystem_.type) {
        case TissueType::STEM_CELL:
            base_capacity *= 1.5;  // Stem cells have enhanced repair
            break;
        case TissueType::TUMOR_RAPIDLY_DIVIDING:
            base_capacity *= 1.2;  // Tumors often upregulate repair
            break;
        case TissueType::TUMOR_HYPOXIC:
            base_capacity *= 0.8;  // Hypoxia impairs repair
            break;
        default:
            break;
    }

    // Adjust based on oxygen tension
    double oxygen_tension = biosystem_.oxygen_tension;
    if (oxygen_tension < 5.0) {
        // Hypoxia reduces repair capacity
        base_capacity *= (0.7 + 0.3 * (oxygen_tension / 5.0));
    }

    return base_capacity;
}

// Calculate repair kinetics curve
std::vector<double> RepairKinetics::calculateRepairCurve(double max_time, double time_step) const
{
    std::vector<double> repair_curve;

    for (double time = 0.0; time <= max_time; time += time_step) {
        double repair_fraction = calculateRepairFraction(time);
        repair_curve.push_back(repair_fraction);
    }

    return repair_curve;
}

// External function to calculate repair with modifiers
double calculateRepairWithModifiers(const BiologicalSystemExtended& biosystem,
                                    const DNADamageProfile& damage, double repair_time,
                                    double oxygen_modifier, double temperature_modifier)
{
    // Create model
    RepairKinetics model(biosystem);

    // Apply modifiers
    model.applyRepairModifiers(oxygen_modifier, temperature_modifier);

    // Calculate survival
    return model.calculateSurvivalFraction(damage, repair_time);
}

}  // namespace cell_biology
}  // namespace healthcare
}  // namespace rad_ml
