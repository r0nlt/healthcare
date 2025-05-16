// Implementation file for cell_biology/dna_damage_model.cpp
#include "rad_ml/healthcare/cell_biology/dna_damage_model.hpp"

#include <cmath>
#include <random>

namespace rad_ml {
namespace healthcare {
namespace cell_biology {

// Constructor for DNADamageModel
DNADamageModel::DNADamageModel(const BiologicalSystemExtended& biosystem) : biosystem_(biosystem)
{
    // Set default damage induction parameters
    damage_params_.alpha_ssb = 30.0;                   // SSBs per Gy per cell
    damage_params_.alpha_dsb = 40.0;                   // DSBs per Gy per cell
    damage_params_.let_factor = 1.0;                   // Scaling factor for LET effects
    damage_params_.o2_enhancement_ratio = 3.0;         // OER for DSB induction
    damage_params_.complex_damage_probability = 0.3;   // Probability of complex damage
    damage_params_.clustered_damage_threshold = 10.0;  // nm for clustered damage

    // Set default repair parameters
    repair_params_.fast_repair_half_time = 0.5;    // hours
    repair_params_.slow_repair_half_time = 2.5;    // hours
    repair_params_.fast_repair_fraction = 0.7;     // fraction of damage repaired by fast component
    repair_params_.mis_repair_probability = 0.05;  // probability of misrepair
    repair_params_.lethal_mis_repair_probability = 0.01;  // probability of lethal misrepair
}

// Set damage induction parameters
void DNADamageModel::setDamageInductionParameters(const DamageInductionParameters& params)
{
    damage_params_ = params;
}

// Set repair parameters
void DNADamageModel::setRepairParameters(const RepairParameters& params)
{
    repair_params_ = params;
}

// Calculate DNA damage for a given dose
DNADamageProfile DNADamageModel::calculateDamage(double dose, RadiationType rad_type) const
{
    DNADamageProfile damage;

    // Calculate LET factor based on radiation type
    double let_factor = getLETFactor(rad_type);

    // Calculate oxygen enhancement ratio based on oxygen tension
    double oer = calculateOER(biosystem_.oxygen_tension);

    // Calculate base damage values
    double ssb_base = damage_params_.alpha_ssb * dose;
    double dsb_base = damage_params_.alpha_dsb * dose;

    // Apply modifiers
    double ssb_count = ssb_base * let_factor;
    double dsb_count = dsb_base * let_factor / oer;  // Lower OER means higher damage in hypoxia

    // Calculate base damages (typically 10x more than SSBs)
    double base_damage_count = ssb_count * 10.0;

    // Calculate clustered damages
    double clustered_damage_count = dsb_count * damage_params_.complex_damage_probability * 1.5;

    // Calculate complex DSBs (subset of total DSBs)
    double complex_dsb_count = dsb_count * damage_params_.complex_damage_probability;

    // Round to integers
    damage.single_strand_breaks = static_cast<int>(std::round(ssb_count));
    damage.double_strand_breaks = static_cast<int>(std::round(dsb_count));
    damage.base_damages = static_cast<int>(std::round(base_damage_count));
    damage.clustered_damages = static_cast<int>(std::round(clustered_damage_count));
    damage.complex_dsb = static_cast<int>(std::round(complex_dsb_count));

    return damage;
}

// Calculate residual DNA damage after repair
DNADamageProfile DNADamageModel::calculateResidualDamage(const DNADamageProfile& initial_damage,
                                                         double repair_time) const
{
    DNADamageProfile residual_damage;

    // Calculate repair fractions for fast and slow components
    double fast_repair_constant = std::log(2.0) / repair_params_.fast_repair_half_time;
    double slow_repair_constant = std::log(2.0) / repair_params_.slow_repair_half_time;

    double fast_repair_fraction = repair_params_.fast_repair_fraction;
    double slow_repair_fraction = 1.0 - fast_repair_fraction;

    double fast_repair_factor = std::exp(-fast_repair_constant * repair_time);
    double slow_repair_factor = std::exp(-slow_repair_constant * repair_time);

    // Calculate residual damage for each type
    residual_damage.single_strand_breaks =
        calculateResidualCount(initial_damage.single_strand_breaks, fast_repair_fraction,
                               slow_repair_fraction, fast_repair_factor, slow_repair_factor);

    residual_damage.double_strand_breaks =
        calculateResidualCount(initial_damage.double_strand_breaks, fast_repair_fraction * 0.9,
                               slow_repair_fraction * 1.1, fast_repair_factor, slow_repair_factor);

    residual_damage.base_damages =
        calculateResidualCount(initial_damage.base_damages, fast_repair_fraction * 1.1,
                               slow_repair_fraction * 0.9, fast_repair_factor, slow_repair_factor);

    residual_damage.clustered_damages =
        calculateResidualCount(initial_damage.clustered_damages, fast_repair_fraction * 0.7,
                               slow_repair_fraction * 1.3, fast_repair_factor, slow_repair_factor);

    residual_damage.complex_dsb =
        calculateResidualCount(initial_damage.complex_dsb, fast_repair_fraction * 0.6,
                               slow_repair_fraction * 1.4, fast_repair_factor, slow_repair_factor);

    return residual_damage;
}

// Calculate survival fraction based on damage
double DNADamageModel::calculateSurvivalFraction(const DNADamageProfile& damage) const
{
    // Use a simple model based on DSBs and complex damage
    double lethal_lesions = damage.double_strand_breaks * 0.05 + damage.complex_dsb * 0.2 +
                            damage.clustered_damages * 0.01;

    // Survival fraction using Poisson statistics
    return std::exp(-lethal_lesions);
}

// Calculate cell survival for a given dose
double DNADamageModel::calculateCellSurvival(double dose, RadiationType rad_type) const
{
    // Calculate initial damage
    DNADamageProfile initial_damage = calculateDamage(dose, rad_type);

    // Assume repair time of 24 hours
    double repair_time = 24.0;

    // Calculate residual damage after repair
    DNADamageProfile residual_damage = calculateResidualDamage(initial_damage, repair_time);

    // Calculate survival fraction
    return calculateSurvivalFraction(residual_damage);
}

// Calculate linear-quadratic parameters
std::pair<double, double> DNADamageModel::calculateLinearQuadraticParameters(
    RadiationType rad_type) const
{
    double alpha, beta;

    // Base values
    alpha = 0.1;  // Gy^-1
    beta = 0.01;  // Gy^-2

    // Modify based on radiation type
    double let_factor = getLETFactor(rad_type);
    alpha *= let_factor;
    beta *= std::sqrt(let_factor);  // Beta changes less with LET

    // Modify based on oxygen
    double oer = calculateOER(biosystem_.oxygen_tension);
    alpha /= oer;
    beta /= (oer * oer);

    return std::make_pair(alpha, beta);
}

// Helper function to get LET factor based on radiation type
double DNADamageModel::getLETFactor(RadiationType rad_type) const
{
    // Rough approximation of relative biological effectiveness
    switch (rad_type) {
        case RadiationType::ELECTRON:
            return 1.0;
        case RadiationType::PHOTON:
            return 1.0;
        case RadiationType::PROTON:
            return 1.1;  // Depends on energy, using average
        case RadiationType::ALPHA:
            return 20.0;
        case RadiationType::CARBON_ION:
            return 3.0;
        case RadiationType::NEUTRON:
            return 10.0;
        default:
            return 1.0;
    }
}

// Helper function to calculate oxygen enhancement ratio
double DNADamageModel::calculateOER(double oxygen_tension) const
{
    // OER formula based on oxygen tension
    double k = 3.0;  // max OER
    double m = 3.0;  // mmHg at which OER is half maximal

    return 1.0 + (k - 1.0) * oxygen_tension / (m + oxygen_tension);
}

// Helper function to calculate residual damage counts
int DNADamageModel::calculateResidualCount(int initial_count, double fast_fraction,
                                           double slow_fraction, double fast_factor,
                                           double slow_factor) const
{
    double residual = initial_count * (fast_fraction * fast_factor + slow_fraction * slow_factor);
    return static_cast<int>(std::round(residual));
}

// Calculate damage using track structure model
DNADamageProfile calculateDamageTrackStructure(const BiologicalSystemExtended& biosystem,
                                               double dose, RadiationType rad_type)
{
    // Create a DNA damage model
    DNADamageModel model(biosystem);

    // Set track structure specific parameters
    DamageInductionParameters params;
    params.alpha_ssb = 35.0;  // Higher value for track structure
    params.alpha_dsb = 45.0;
    params.let_factor = 1.2;
    params.complex_damage_probability = 0.35;

    model.setDamageInductionParameters(params);

    // Calculate damage profile
    return model.calculateDamage(dose, rad_type);
}

// Implementations for DNADamageProfile methods
double DNADamageProfile::getSimpleToComplexRatio() const
{
    if (complex_dsb == 0) return 0.0;
    return static_cast<double>(double_strand_breaks - complex_dsb) / complex_dsb;
}

double DNADamageProfile::getSsbToDsbRatio() const
{
    if (double_strand_breaks == 0) return 0.0;
    return static_cast<double>(single_strand_breaks) / double_strand_breaks;
}

DNADamageProfile DNADamageProfile::operator+(const DNADamageProfile& other) const
{
    DNADamageProfile sum;
    sum.single_strand_breaks = single_strand_breaks + other.single_strand_breaks;
    sum.double_strand_breaks = double_strand_breaks + other.double_strand_breaks;
    sum.base_damages = base_damages + other.base_damages;
    sum.clustered_damages = clustered_damages + other.clustered_damages;
    sum.complex_dsb = complex_dsb + other.complex_dsb;
    return sum;
}

}  // namespace cell_biology
}  // namespace healthcare
}  // namespace rad_ml
