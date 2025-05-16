// Implementation file for cell_biology/cell_cycle_model.cpp
#include "rad_ml/healthcare/cell_biology/cell_cycle_model.hpp"

#include <algorithm>
#include <cmath>

namespace rad_ml {
namespace healthcare {
namespace cell_biology {

// Constructor for CellCycleModel
CellCycleModel::CellCycleModel(const BiologicalSystemExtended& biosystem) : biosystem_(biosystem)
{
    // Constructor just initializes the biosystem_ member
}

// Set cell cycle distribution
void CellCycleModel::setCellCycleDistribution(const CellCycleDistribution& distribution)
{
    if (distribution.isValid()) {
        biosystem_.cycle_distribution = distribution;
    }
}

// Set phase sensitivity parameters
void CellCycleModel::setPhaseSensitivity(CellCyclePhase phase, const PhaseSensitivity& sensitivity)
{
    biosystem_.phase_sensitivity[phase] = sensitivity;
}

// Set oxygen tension
void CellCycleModel::setOxygenTension(double oxygen_tension)
{
    biosystem_.oxygen_tension = oxygen_tension;
}

// Calculate survival fraction for a given dose
double CellCycleModel::calculateSurvivalFraction(double dose) const
{
    // Calculate weighted radiosensitivity parameters
    auto params = calculateCellCycleWeightedRadiosensitivity();
    double alpha = params.first;
    double beta = params.second;

    // Apply oxygen effect
    double oer = calculateOxygenEnhancementFactor();
    alpha /= oer;
    beta /= (oer * oer);

    // Linear-quadratic survival
    return std::exp(-(alpha * dose + beta * dose * dose));
}

// Calculate survival fraction for a specific phase
double CellCycleModel::calculatePhaseSurvivalFraction(CellCyclePhase phase, double dose) const
{
    // Get phase-specific sensitivity
    auto it = biosystem_.phase_sensitivity.find(phase);
    if (it == biosystem_.phase_sensitivity.end()) {
        // If not found, use a default
        PhaseSensitivity default_sensitivity;
        default_sensitivity.alpha = 0.2;
        default_sensitivity.beta = 0.02;

        // Apply oxygen effect
        double oer = calculateOxygenEnhancementFactor();
        return std::exp(-(default_sensitivity.alpha * dose / oer +
                          default_sensitivity.beta * dose * dose / (oer * oer)));
    }

    // Apply oxygen effect
    double oer = calculateOxygenEnhancementFactor();
    double alpha = it->second.alpha / oer;
    double beta = it->second.beta / (oer * oer);

    // Linear-quadratic survival
    return std::exp(-(alpha * dose + beta * dose * dose));
}

// Calculate cell cycle redistribution after irradiation
CellCycleDistribution CellCycleModel::calculateCellCycleRedistribution(
    double dose, double time_after_irradiation) const
{
    CellCycleDistribution redistribution = biosystem_.cycle_distribution;

    // Calculate survival for each phase
    double g0_survival = calculatePhaseSurvivalFraction(CellCyclePhase::G0, dose);
    double g1_survival = calculatePhaseSurvivalFraction(CellCyclePhase::G1, dose);
    double s_survival = calculatePhaseSurvivalFraction(CellCyclePhase::S, dose);
    double g2_survival = calculatePhaseSurvivalFraction(CellCyclePhase::G2, dose);
    double m_survival = calculatePhaseSurvivalFraction(CellCyclePhase::M, dose);

    // Adjust fractions based on survival
    double total_before = redistribution.g0_fraction + redistribution.g1_fraction +
                          redistribution.s_fraction + redistribution.g2_fraction +
                          redistribution.m_fraction;

    double g0_after = redistribution.g0_fraction * g0_survival;
    double g1_after = redistribution.g1_fraction * g1_survival;
    double s_after = redistribution.s_fraction * s_survival;
    double g2_after = redistribution.g2_fraction * g2_survival;
    double m_after = redistribution.m_fraction * m_survival;

    double total_after = g0_after + g1_after + s_after + g2_after + m_after;

    // Normalize
    if (total_after > 0) {
        redistribution.g0_fraction = g0_after / total_after;
        redistribution.g1_fraction = g1_after / total_after;
        redistribution.s_fraction = s_after / total_after;
        redistribution.g2_fraction = g2_after / total_after;
        redistribution.m_fraction = m_after / total_after;
    }

    // Apply cell cycle progression over time
    if (time_after_irradiation > 0) {
        return progressCellCycle(redistribution, time_after_irradiation);
    }

    return redistribution;
}

// Calculate survival for fractionated irradiation
double CellCycleModel::calculateFractionatedSurvivalFraction(double dose_per_fraction,
                                                             int num_fractions,
                                                             double time_between_fractions) const
{
    double total_survival = 1.0;
    CellCycleDistribution current_distribution = biosystem_.cycle_distribution;

    // For each fraction
    for (int i = 0; i < num_fractions; ++i) {
        // Create a temporary model with current distribution
        BiologicalSystemExtended temp_biosystem = biosystem_;
        temp_biosystem.cycle_distribution = current_distribution;

        CellCycleModel temp_model(temp_biosystem);

        // Calculate survival for this fraction
        double fraction_survival = temp_model.calculateSurvivalFraction(dose_per_fraction);
        total_survival *= fraction_survival;

        // Update distribution for next fraction
        if (i < num_fractions - 1) {  // Don't need to update after last fraction
            current_distribution = temp_model.calculateCellCycleRedistribution(
                dose_per_fraction, time_between_fractions);
        }
    }

    return total_survival;
}

// Calculate cell-cycle weighted radiosensitivity
std::pair<double, double> CellCycleModel::calculateCellCycleWeightedRadiosensitivity() const
{
    return calculateCellCycleWeightedRadiosensitivity(biosystem_);
}

// Optimize fractionation schedule
std::tuple<double, int, double> CellCycleModel::optimizeFractionationSchedule(
    double total_dose, double min_fraction_size, double max_fraction_size,
    double target_survival) const
{
    // Default values
    double optimal_fraction_size = 2.0;
    int optimal_num_fractions = static_cast<int>(std::round(total_dose / optimal_fraction_size));
    double optimal_timing = 24.0;  // 24 hours between fractions

    // Simple optimization: try different fraction sizes
    double best_survival_difference = 1.0;

    for (double fraction_size = min_fraction_size; fraction_size <= max_fraction_size;
         fraction_size += 0.2) {
        int num_fractions = static_cast<int>(std::round(total_dose / fraction_size));
        if (num_fractions < 1) num_fractions = 1;

        double actual_fraction_size = total_dose / num_fractions;

        // Try different timings
        for (double timing = 6.0; timing <= 36.0; timing += 6.0) {
            double survival =
                calculateFractionatedSurvivalFraction(actual_fraction_size, num_fractions, timing);

            double survival_difference = std::fabs(survival - target_survival);

            if (survival_difference < best_survival_difference) {
                best_survival_difference = survival_difference;
                optimal_fraction_size = actual_fraction_size;
                optimal_num_fractions = num_fractions;
                optimal_timing = timing;
            }
        }
    }

    return std::make_tuple(optimal_fraction_size, optimal_num_fractions, optimal_timing);
}

// Get default cell cycle distribution for a tissue type
CellCycleDistribution CellCycleModel::getDefaultCellCycleDistribution(TissueType tissue_type)
{
    CellCycleDistribution distribution;

    switch (tissue_type) {
        case TissueType::SOFT_TISSUE:
            distribution.g0_fraction = 0.6;
            distribution.g1_fraction = 0.2;
            distribution.s_fraction = 0.1;
            distribution.g2_fraction = 0.05;
            distribution.m_fraction = 0.05;
            break;

        case TissueType::BONE:
            distribution.g0_fraction = 0.8;
            distribution.g1_fraction = 0.1;
            distribution.s_fraction = 0.05;
            distribution.g2_fraction = 0.025;
            distribution.m_fraction = 0.025;
            break;

        case TissueType::EPITHELIAL:
            distribution.g0_fraction = 0.4;
            distribution.g1_fraction = 0.3;
            distribution.s_fraction = 0.2;
            distribution.g2_fraction = 0.05;
            distribution.m_fraction = 0.05;
            break;

        case TissueType::STEM_CELL:
            distribution.g0_fraction = 0.2;
            distribution.g1_fraction = 0.3;
            distribution.s_fraction = 0.3;
            distribution.g2_fraction = 0.1;
            distribution.m_fraction = 0.1;
            break;

        case TissueType::TUMOR_RAPIDLY_DIVIDING:
            distribution.g0_fraction = 0.1;
            distribution.g1_fraction = 0.3;
            distribution.s_fraction = 0.4;
            distribution.g2_fraction = 0.1;
            distribution.m_fraction = 0.1;
            break;

        case TissueType::TUMOR_HYPOXIC:
            distribution.g0_fraction = 0.3;
            distribution.g1_fraction = 0.3;
            distribution.s_fraction = 0.2;
            distribution.g2_fraction = 0.1;
            distribution.m_fraction = 0.1;
            break;

        default:
            // Default distribution
            distribution.g0_fraction = 0.2;
            distribution.g1_fraction = 0.3;
            distribution.s_fraction = 0.3;
            distribution.g2_fraction = 0.1;
            distribution.m_fraction = 0.1;
            break;
    }

    return distribution;
}

// Create a default biological system for a tissue type
BiologicalSystemExtended CellCycleModel::createDefaultBiologicalSystem(TissueType tissue_type)
{
    BiologicalSystemExtended biosystem;
    biosystem.type = tissue_type;

    // Set cell cycle distribution
    biosystem.cycle_distribution = getDefaultCellCycleDistribution(tissue_type);

    // Set default oxygen tension
    if (tissue_type == TissueType::TUMOR_HYPOXIC) {
        biosystem.oxygen_tension = 2.5;  // mmHg, hypoxic
    }
    else {
        biosystem.oxygen_tension = 40.0;  // mmHg, normoxic
    }

    // Set default phase sensitivities
    std::map<CellCyclePhase, PhaseSensitivity> phase_sensitivity;

    // G0 phase - usually radioresistant
    PhaseSensitivity g0_sensitivity;
    g0_sensitivity.alpha = 0.1;
    g0_sensitivity.beta = 0.01;
    phase_sensitivity[CellCyclePhase::G0] = g0_sensitivity;

    // G1 phase - moderate sensitivity
    PhaseSensitivity g1_sensitivity;
    g1_sensitivity.alpha = 0.2;
    g1_sensitivity.beta = 0.02;
    phase_sensitivity[CellCyclePhase::G1] = g1_sensitivity;

    // S phase - most radioresistant
    PhaseSensitivity s_sensitivity;
    s_sensitivity.alpha = 0.1;
    s_sensitivity.beta = 0.01;
    phase_sensitivity[CellCyclePhase::S] = s_sensitivity;

    // G2 phase - radiosensitive
    PhaseSensitivity g2_sensitivity;
    g2_sensitivity.alpha = 0.3;
    g2_sensitivity.beta = 0.03;
    phase_sensitivity[CellCyclePhase::G2] = g2_sensitivity;

    // M phase - most radiosensitive
    PhaseSensitivity m_sensitivity;
    m_sensitivity.alpha = 0.4;
    m_sensitivity.beta = 0.04;
    phase_sensitivity[CellCyclePhase::M] = m_sensitivity;

    biosystem.phase_sensitivity = phase_sensitivity;

    return biosystem;
}

// Implementation of CellCycleDistribution::isValid
bool CellCycleDistribution::isValid() const
{
    // Check that all fractions are non-negative
    if (g0_fraction < 0.0 || g1_fraction < 0.0 || s_fraction < 0.0 || g2_fraction < 0.0 ||
        m_fraction < 0.0) {
        return false;
    }

    // Check that fractions sum to 1.0 (with small tolerance)
    double sum = g0_fraction + g1_fraction + s_fraction + g2_fraction + m_fraction;
    return std::fabs(sum - 1.0) < 0.001;
}

// Global function to calculate cell cycle weighted radiosensitivity
std::pair<double, double> calculateCellCycleWeightedRadiosensitivity(
    const BiologicalSystemExtended& biosystem)
{
    double alpha = 0.0;
    double beta = 0.0;

    // Get distribution
    const CellCycleDistribution& dist = biosystem.cycle_distribution;

    // Get phase sensitivities
    const auto& phase_sensitivity = biosystem.phase_sensitivity;

    // Weight by cell cycle phase
    if (phase_sensitivity.find(CellCyclePhase::G0) != phase_sensitivity.end()) {
        alpha += dist.g0_fraction * phase_sensitivity.at(CellCyclePhase::G0).alpha;
        beta += dist.g0_fraction * phase_sensitivity.at(CellCyclePhase::G0).beta;
    }

    if (phase_sensitivity.find(CellCyclePhase::G1) != phase_sensitivity.end()) {
        alpha += dist.g1_fraction * phase_sensitivity.at(CellCyclePhase::G1).alpha;
        beta += dist.g1_fraction * phase_sensitivity.at(CellCyclePhase::G1).beta;
    }

    if (phase_sensitivity.find(CellCyclePhase::S) != phase_sensitivity.end()) {
        alpha += dist.s_fraction * phase_sensitivity.at(CellCyclePhase::S).alpha;
        beta += dist.s_fraction * phase_sensitivity.at(CellCyclePhase::S).beta;
    }

    if (phase_sensitivity.find(CellCyclePhase::G2) != phase_sensitivity.end()) {
        alpha += dist.g2_fraction * phase_sensitivity.at(CellCyclePhase::G2).alpha;
        beta += dist.g2_fraction * phase_sensitivity.at(CellCyclePhase::G2).beta;
    }

    if (phase_sensitivity.find(CellCyclePhase::M) != phase_sensitivity.end()) {
        alpha += dist.m_fraction * phase_sensitivity.at(CellCyclePhase::M).alpha;
        beta += dist.m_fraction * phase_sensitivity.at(CellCyclePhase::M).beta;
    }

    return std::make_pair(alpha, beta);
}

// Cell cycle progression model
CellCycleDistribution CellCycleModel::progressCellCycle(
    const CellCycleDistribution& initial_distribution, double time_hours) const
{
    CellCycleDistribution result = initial_distribution;

    // Simplified model: cells progress through cycle with average doubling time of 24h
    double progression_fraction =
        time_hours / biosystem_.doubling_time;  // Fraction of cycle completed

    // Simple progression model (cells move from one phase to next)
    if (progression_fraction > 0) {
        double g1_to_s = result.g1_fraction * progression_fraction;
        double s_to_g2 = result.s_fraction * progression_fraction;
        double g2_to_m = result.g2_fraction * progression_fraction;
        double m_to_g1 = result.m_fraction * progression_fraction;
        double g0_to_g1 =
            result.g0_fraction * progression_fraction * 0.1;  // Only 10% of G0 reenter

        result.g0_fraction -= g0_to_g1;
        result.g1_fraction = result.g1_fraction - g1_to_s + m_to_g1 + g0_to_g1;
        result.s_fraction = result.s_fraction - s_to_g2 + g1_to_s;
        result.g2_fraction = result.g2_fraction - g2_to_m + s_to_g2;
        result.m_fraction = result.m_fraction - m_to_g1 + g2_to_m;
    }

    // Ensure valid distribution
    if (!result.isValid()) {
        double total = result.g0_fraction + result.g1_fraction + result.s_fraction +
                       result.g2_fraction + result.m_fraction;

        if (total > 0) {
            result.g0_fraction /= total;
            result.g1_fraction /= total;
            result.s_fraction /= total;
            result.g2_fraction /= total;
            result.m_fraction /= total;
        }
        else {
            // If all zero, reset to default
            result.g0_fraction = 0.1;
            result.g1_fraction = 0.3;
            result.s_fraction = 0.4;
            result.g2_fraction = 0.1;
            result.m_fraction = 0.1;
        }
    }

    return result;
}

// Calculate oxygen enhancement factor
double CellCycleModel::calculateOxygenEnhancementFactor() const
{
    // OER formula based on oxygen tension
    double k = 3.0;  // max OER
    double m = 3.0;  // mmHg at which OER is half maximal

    return 1.0 + (k - 1.0) * biosystem_.oxygen_tension / (m + biosystem_.oxygen_tension);
}

// These methods are stubs for now
double CellCycleModel::calculateRepopulationFactor(double time_hours) const { return 1.0; }

double CellCycleModel::calculateRepairFactor(double time_hours) const { return 1.0; }

void CellCycleModel::setCellDoublingTime(double doubling_time_hours)
{
    biosystem_.doubling_time = doubling_time_hours;
}

double CellCycleModel::getTissueRegenerationCapacity() const
{
    return biosystem_.growth_fraction * biosystem_.stem_cell_fraction;
}

}  // namespace cell_biology
}  // namespace healthcare
}  // namespace rad_ml
