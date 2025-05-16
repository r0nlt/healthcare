#include "rad_ml/healthcare/chemotherapy/chemoradiation_synergy.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace rad_ml {
namespace healthcare {
namespace chemotherapy {

// Standard linear-quadratic parameters
namespace standard_lq_parameters {
// Using the research values provided

// Hematopoietic system (α = 0.70 Gy⁻¹, β = 0.08 Gy⁻²)
const LinearQuadraticParameters HEMATOPOIETIC = {
    0.7,   // alpha
    0.08,  // beta
    8.75   // alpha_beta_ratio
};

// Breast tumor (α = 0.30 Gy⁻¹, β = 0.04 Gy⁻²)
const LinearQuadraticParameters BREAST_TUMOR = {
    0.3,   // alpha
    0.04,  // beta
    7.5    // alpha_beta_ratio
};

// Prostate tumor (α = 0.20 Gy⁻¹, β = 0.02 Gy⁻²)
const LinearQuadraticParameters PROSTATE_TUMOR = {
    0.2,   // alpha
    0.02,  // beta
    10.0   // alpha_beta_ratio
};

// Glioblastoma (α = 0.25 Gy⁻¹, β = 0.03 Gy⁻²)
const LinearQuadraticParameters GLIOBLASTOMA = {
    0.25,  // alpha
    0.03,  // beta
    8.33   // alpha_beta_ratio
};

// Lung tumor (α = 0.32 Gy⁻¹, β = 0.035 Gy⁻²)
const LinearQuadraticParameters LUNG_TUMOR = {
    0.32,   // alpha
    0.035,  // beta
    9.14    // alpha_beta_ratio
};
}  // namespace standard_lq_parameters

// Constructor with drug and tissue
ChemoradiationSynergyModel::ChemoradiationSynergyModel(const ChemotherapeuticAgent& drug,
                                                       const BiologicalSystem& tissue)
    : drug_(drug), tissue_(tissue)
{
    // Set default LQ parameters based on tissue type
    lq_params_ = getStandardLQParameters(tissue.type);
}

// Constructor with specific LQ parameters
ChemoradiationSynergyModel::ChemoradiationSynergyModel(const ChemotherapeuticAgent& drug,
                                                       const BiologicalSystem& tissue,
                                                       const LinearQuadraticParameters& lq_params)
    : drug_(drug), tissue_(tissue), lq_params_(lq_params)
{
}

// Calculate synergistic effect of combined treatment
double ChemoradiationSynergyModel::calculateSynergy(double radiation_dose,
                                                    double drug_concentration,
                                                    double time_between_treatments)
{
    // Base effect of radiation (linear-quadratic model)
    double radiation_effect =
        lq_params_.alpha * radiation_dose + lq_params_.beta * radiation_dose * radiation_dose;

    // Base effect of drug (simplified dose-response curve)
    double drug_effect = drug_concentration / (drug_concentration + 0.5);

    // Calculate radiosensitization factor
    double sensitization = calculateRadiosensitizationFactor(drug_concentration);

    // Calculate repair inhibition
    double repair_inhibition = calculateRepairInhibition(drug_concentration);

    // Time-dependent effect
    double time_factor = 1.0;
    if (time_between_treatments > 0.0) {
        // Exponential decay of synergy with time gap
        time_factor = std::exp(-0.05 * time_between_treatments);
    }

    // Calculate synergy
    double synergy = (radiation_effect * sensitization * repair_inhibition - radiation_effect) +
                     (drug_effect * sensitization - drug_effect) +
                     0.2 * time_factor * radiation_effect * drug_effect;

    return synergy;
}

// Adjust radiation response parameters based on drug presence
LinearQuadraticParameters ChemoradiationSynergyModel::adjustedLQParameters(
    const LinearQuadraticParameters& base_params, double drug_concentration)
{
    // Calculate radiosensitization factor
    double sensitization = calculateRadiosensitizationFactor(drug_concentration);

    // Adjust alpha and beta parameters
    LinearQuadraticParameters adjusted_params = base_params;

    // Alpha is enhanced more than beta in most drugs
    adjusted_params.alpha = base_params.alpha * sensitization;

    // Beta enhancement depends on drug mechanism
    if (drug_.name == "Paclitaxel" || drug_.name == "Doxorubicin") {
        // Cell cycle specific drugs enhance beta more
        adjusted_params.beta = base_params.beta * (sensitization + 0.2);
    }
    else {
        adjusted_params.beta = base_params.beta * sensitization;
    }

    // Recalculate alpha/beta ratio
    if (adjusted_params.beta > 0) {
        adjusted_params.alpha_beta_ratio = adjusted_params.alpha / adjusted_params.beta;
    }

    return adjusted_params;
}

// Predict efficacy with different sequencing strategies
double ChemoradiationSynergyModel::predictEfficacy(double radiation_dose, double drug_concentration,
                                                   TreatmentSequence sequence,
                                                   double time_gap_hours)
{
    // Base effects
    double radiation_effect =
        lq_params_.alpha * radiation_dose + lq_params_.beta * radiation_dose * radiation_dose;
    double drug_effect = drug_concentration / (drug_concentration + 0.5);

    // Parameters for sequence-specific effects
    double sensitization = 1.0;
    double repair_inhibition = 1.0;
    double time_factor = 1.0;
    double cycle_effect = 1.0;  // Move declaration here outside switch

    // Calculate sequence-specific factors
    switch (sequence) {
        case CONCURRENT:
            // Maximum synergy for concurrent treatment
            sensitization = calculateRadiosensitizationFactor(drug_concentration);
            repair_inhibition = calculateRepairInhibition(drug_concentration);
            time_factor = 1.0;
            break;

        case RADIATION_FIRST:
            // Reduced synergy for sequential treatment, radiation first
            // Time-dependent decay of effect
            time_factor = std::exp(-0.05 * time_gap_hours);
            sensitization =
                1.0 + (calculateRadiosensitizationFactor(drug_concentration) - 1.0) * time_factor;

            // Cell cycle redistribution effect can sometimes enhance this sequence
            cycle_effect = calculateCellCycleEffect(drug_concentration, time_gap_hours);
            sensitization *= cycle_effect;
            break;

        case DRUG_FIRST:
            // Drug first can enhance radiosensitivity
            time_factor = std::exp(-0.03 * time_gap_hours);  // Slower decay than radiation first

            // Calculate drug concentration at radiation time
            double effective_concentration =
                calculateDrugConcentrationAtTime(drug_concentration, time_gap_hours);

            sensitization = calculateRadiosensitizationFactor(effective_concentration);
            repair_inhibition = calculateRepairInhibition(effective_concentration);
            break;
    }

    // Calculate combined effect
    double combined_effect = radiation_effect * sensitization * repair_inhibition +
                             drug_effect * (1.0 + 0.2 * time_factor * radiation_effect);

    // Apply quantum corrections
    return applyQuantumCorrectionsToSynergy(combined_effect, radiation_dose, drug_concentration);
}

// Calculate cell survival fraction for combined treatment
double ChemoradiationSynergyModel::calculateSurvivalFraction(double radiation_dose,
                                                             double drug_concentration,
                                                             TreatmentSequence sequence,
                                                             double time_gap_hours)
{
    // Get adjusted LQ parameters based on drug and sequence
    LinearQuadraticParameters adjusted_params = lq_params_;

    // Adjust parameters based on sequence
    switch (sequence) {
        case CONCURRENT:
            adjusted_params = adjustedLQParameters(lq_params_, drug_concentration);
            break;

        case RADIATION_FIRST: {
            // Partial adjustment due to time gap
            double time_factor = std::exp(-0.05 * time_gap_hours);
            double effective_sensitization =
                1.0 + (calculateRadiosensitizationFactor(drug_concentration) - 1.0) * time_factor;

            adjusted_params.alpha = lq_params_.alpha * effective_sensitization;
            adjusted_params.beta = lq_params_.beta * effective_sensitization;
        } break;

        case DRUG_FIRST: {
            // Calculate effective drug concentration after time gap
            double effective_concentration =
                calculateDrugConcentrationAtTime(drug_concentration, time_gap_hours);

            adjusted_params = adjustedLQParameters(lq_params_, effective_concentration);
        } break;
    }

    // Calculate survival fraction using linear-quadratic model
    double survival = std::exp(-(adjusted_params.alpha * radiation_dose +
                                 adjusted_params.beta * radiation_dose * radiation_dose));

    // Add direct drug cytotoxicity (independent cell killing)
    double drug_survival = 1.0 - (drug_concentration / (drug_concentration + 1.0));

    // Combine radiation and drug survival (multiplicative model)
    return survival * drug_survival;
}

// Calculate therapeutic ratio for combined treatment
double ChemoradiationSynergyModel::calculateTherapeuticRatio(double tumor_radiation_dose,
                                                             double normal_tissue_radiation_dose,
                                                             double drug_concentration,
                                                             TreatmentSequence sequence,
                                                             double time_gap_hours)
{
    // Create a copy of this model with tumor parameters
    ChemoradiationSynergyModel tumor_model(*this);

    // Set normal tissue parameters based on typical late-responding tissue
    LinearQuadraticParameters normal_tissue_params;
    normal_tissue_params.alpha = 0.15;            // Lower alpha for late-responding tissue
    normal_tissue_params.beta = 0.05;             // Higher beta for late-responding tissue
    normal_tissue_params.alpha_beta_ratio = 3.0;  // Typical for late-responding tissue

    // Calculate tumor effect
    double tumor_survival = calculateSurvivalFraction(tumor_radiation_dose, drug_concentration,
                                                      sequence, time_gap_hours);

    // Create model for normal tissue
    ChemoradiationSynergyModel normal_model(drug_, tissue_, normal_tissue_params);

    // Calculate normal tissue effect
    double normal_survival = normal_model.calculateSurvivalFraction(
        normal_tissue_radiation_dose, drug_concentration * 0.8, sequence, time_gap_hours);

    // Calculate therapeutic ratio (normal survival / tumor survival)
    // Higher ratio means better therapeutic window
    if (tumor_survival > 0) {
        return normal_survival / tumor_survival;
    }
    else {
        return 1000.0;  // Avoid division by zero, large number indicates high efficacy
    }
}

// Get optimal treatment timing
double ChemoradiationSynergyModel::getOptimalTimingGap(double radiation_dose,
                                                       double drug_concentration,
                                                       bool radiation_first)
{
    // Search for optimal timing in 1-hour increments
    double best_time = 0.0;
    double best_efficacy = 0.0;

    TreatmentSequence sequence = radiation_first ? RADIATION_FIRST : DRUG_FIRST;

    for (double time = 0.0; time <= 48.0; time += 1.0) {
        double efficacy = predictEfficacy(radiation_dose, drug_concentration, sequence, time);

        if (efficacy > best_efficacy) {
            best_efficacy = efficacy;
            best_time = time;
        }
    }

    return best_time;
}

// Set tissue-specific parameters
void ChemoradiationSynergyModel::setTissueParameters(TissueType tissue_type)
{
    // Update tissue
    tissue_.type = tissue_type;

    // Set LQ parameters based on tissue type
    lq_params_ = getStandardLQParameters(tissue_type);
}

// Radiosensitization factor based on drug mechanism
double ChemoradiationSynergyModel::calculateRadiosensitizationFactor(double drug_concentration)
{
    // Base enhancement
    double base_enhancement = 1.0;

    // Drug-specific enhancement
    if (drug_.name == "Cisplatin") {
        // Platinum compounds are good radiosensitizers
        base_enhancement = 1.5;
    }
    else if (drug_.name == "Fluorouracil") {
        // 5-FU is a moderate radiosensitizer
        base_enhancement = 1.4;
    }
    else if (drug_.name == "Paclitaxel") {
        // Taxanes are good radiosensitizers
        base_enhancement = 1.3;
    }
    else if (drug_.name == "Doxorubicin") {
        // Anthracyclines can be radiosensitizers
        base_enhancement = 1.2;
    }
    else {
        // Default moderate enhancement
        base_enhancement = 1.2;
    }

    // Concentration-dependent enhancement (saturating function)
    double concentration_factor = drug_concentration / (drug_concentration + 0.5);

    return 1.0 + (base_enhancement - 1.0) * concentration_factor;
}

// DNA repair inhibition factor
double ChemoradiationSynergyModel::calculateRepairInhibition(double drug_concentration)
{
    // Base inhibition
    double base_inhibition = 1.0;

    // Drug-specific inhibition
    if (drug_.name == "Cisplatin") {
        // Platinum compounds inhibit DNA repair
        base_inhibition = 1.4;
    }
    else if (drug_.name == "Doxorubicin") {
        // Topoisomerase inhibitors affect repair
        base_inhibition = 1.3;
    }
    else {
        // Default moderate inhibition
        base_inhibition = 1.2;
    }

    // Concentration-dependent inhibition (saturating function)
    double concentration_factor = drug_concentration / (drug_concentration + 0.7);

    return 1.0 + (base_inhibition - 1.0) * concentration_factor;
}

// Cell cycle redistribution effect
double ChemoradiationSynergyModel::calculateCellCycleEffect(double drug_concentration,
                                                            double time_hours)
{
    // Default: no cell cycle effect
    double cycle_effect = 1.0;

    // Drug-specific cell cycle effects
    if (drug_.name == "Paclitaxel") {
        // Paclitaxel causes G2/M arrest which enhances radiation effect
        // Maximum effect after 6-12 hours
        double time_factor = std::exp(-0.5 * std::pow((time_hours - 9.0) / 6.0, 2.0));
        cycle_effect = 1.0 + 0.4 * time_factor;
    }
    else if (drug_.name == "Fluorouracil") {
        // 5-FU synchronizes cells in S phase
        // Maximum effect after 16-24 hours
        double time_factor = std::exp(-0.5 * std::pow((time_hours - 20.0) / 8.0, 2.0));
        cycle_effect = 1.0 + 0.3 * time_factor;
    }

    // Concentration dependence
    double concentration_factor = drug_concentration / (drug_concentration + 0.3);

    return 1.0 + (cycle_effect - 1.0) * concentration_factor;
}

// Get standard LQ parameters for tissue type
LinearQuadraticParameters ChemoradiationSynergyModel::getStandardLQParameters(
    TissueType tissue_type)
{
    // Default to breast tumor parameters
    LinearQuadraticParameters params = standard_lq_parameters::BREAST_TUMOR;

    // Adjust based on tissue type
    switch (tissue_type) {
        case SOFT_TISSUE:
            // Use breast tumor as representative for soft tissue
            params = standard_lq_parameters::BREAST_TUMOR;
            break;
        case BONE:
            // Bone has lower alpha/beta ratio (like prostate)
            params = standard_lq_parameters::PROSTATE_TUMOR;
            break;
        case NERVE:
            // Neural tissue like glioblastoma
            params = standard_lq_parameters::GLIOBLASTOMA;
            break;
        case MUSCLE:
            // Similar to soft tissue but slightly different
            params = standard_lq_parameters::BREAST_TUMOR;
            params.alpha = 0.28;  // Modified value
            params.beta = 0.035;  // Modified value
            break;
        case BLOOD:
            // Hematopoietic system
            params = standard_lq_parameters::HEMATOPOIETIC;
            break;
    }

    return params;
}

// Calculate time-dependent drug concentration
double ChemoradiationSynergyModel::calculateDrugConcentrationAtTime(double initial_concentration,
                                                                    double time_hours)
{
    // Simple exponential decay based on drug half-life
    return initial_concentration *
           std::exp(-std::log(2.0) * time_hours / drug_.clearance_half_life);
}

// Apply quantum corrections to synergy model
double ChemoradiationSynergyModel::applyQuantumCorrectionsToSynergy(double classical_synergy,
                                                                    double radiation_dose,
                                                                    double drug_concentration)
{
    // Constants based on parameters
    const double BIO_QUANTUM_ENHANCEMENT = 0.04;  // 4% enhancement

    // Dose-dependent quantum effect (higher for lower doses)
    double dose_factor = 1.0;
    if (radiation_dose < 2.0) {
        dose_factor = 1.0 + 0.2 * (2.0 - radiation_dose) / 2.0;
    }

    // Concentration-dependent quantum effect
    double concentration_factor = 1.0;
    if (drug_concentration < 0.5) {
        concentration_factor = 1.0 + 0.1 * (0.5 - drug_concentration) / 0.5;
    }

    // Calculate quantum enhancement for combined treatment
    double quantum_enhancement = 1.0 + BIO_QUANTUM_ENHANCEMENT * dose_factor * concentration_factor;

    // Apply tissue-specific factor
    double tissue_quantum_factor = 1.0;
    switch (tissue_.type) {
        case SOFT_TISSUE:
            tissue_quantum_factor = 1.0;
            break;
        case BONE:
            tissue_quantum_factor = 0.9;  // Reduced quantum effects
            break;
        case NERVE:
            tissue_quantum_factor = 1.1;  // Enhanced quantum effects
            break;
        case MUSCLE:
            tissue_quantum_factor = 0.95;
            break;
        case BLOOD:
            tissue_quantum_factor = 1.15;  // Highest quantum effects
            break;
    }

    // Apply water content effect (higher water content means stronger quantum effects)
    double water_factor = 1.0 + 0.2 * (tissue_.water_content - 0.7);

    // Combine all factors and apply to classical synergy
    double combined_quantum_factor = quantum_enhancement * tissue_quantum_factor * water_factor;
    return classical_synergy * combined_quantum_factor;
}

// Calculate dose-response curve
std::vector<double> ChemoradiationSynergyModel::calculateDoseResponseCurve(
    double max_dose, double drug_concentration, TreatmentSequence sequence, double time_gap_hours)
{
    // Calculate survival at 0.5 Gy increments
    int num_points = static_cast<int>(max_dose / 0.5) + 1;
    std::vector<double> response_curve(num_points);

    for (int i = 0; i < num_points; ++i) {
        double dose = i * 0.5;  // 0, 0.5, 1.0, 1.5, ... Gy

        response_curve[i] =
            calculateSurvivalFraction(dose, drug_concentration, sequence, time_gap_hours);
    }

    return response_curve;
}

}  // namespace chemotherapy
}  // namespace healthcare
}  // namespace rad_ml
