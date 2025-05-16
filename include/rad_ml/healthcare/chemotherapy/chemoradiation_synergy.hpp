#pragma once

#include <map>
#include <string>
#include <vector>

#include "rad_ml/healthcare/bio_quantum_integration.hpp"
#include "rad_ml/healthcare/chemotherapy/chemo_quantum_model.hpp"
#include "rad_ml/healthcare/radiation_therapy_model.hpp"

namespace rad_ml {
namespace healthcare {
namespace chemotherapy {

// Linear-Quadratic model parameters for radiation response
struct LinearQuadraticParameters {
    double alpha;             // Gy⁻¹
    double beta;              // Gy⁻²
    double alpha_beta_ratio;  // Gy
};

// Using TreatmentSchedule from chemo_quantum_model.hpp instead
// enum TreatmentSequence { CONCURRENT, RADIATION_FIRST, DRUG_FIRST };

class ChemoradiationSynergyModel {
   public:
    // Constructor with drug and tissue
    ChemoradiationSynergyModel(const ChemotherapeuticAgent& drug, const BiologicalSystem& tissue);

    // Initialize with specific LQ parameters
    ChemoradiationSynergyModel(const ChemotherapeuticAgent& drug, const BiologicalSystem& tissue,
                               const LinearQuadraticParameters& lq_params);

    // Calculate synergistic effect of combined treatment
    double calculateSynergy(double radiation_dose, double drug_concentration,
                            double time_between_treatments);

    // Adjust radiation response parameters based on drug presence
    LinearQuadraticParameters adjustedLQParameters(const LinearQuadraticParameters& base_params,
                                                   double drug_concentration);

    // Predict efficacy with different sequencing strategies
    double predictEfficacy(double radiation_dose, double drug_concentration,
                           TreatmentSchedule sequence, double time_gap_hours);

    // Calculate cell survival fraction for combined treatment
    double calculateSurvivalFraction(double radiation_dose, double drug_concentration,
                                     TreatmentSchedule sequence, double time_gap_hours);

    // Calculate therapeutic ratio for combined treatment
    double calculateTherapeuticRatio(double tumor_radiation_dose,
                                     double normal_tissue_radiation_dose, double drug_concentration,
                                     TreatmentSchedule sequence, double time_gap_hours);

    // Get optimal treatment timing
    double getOptimalTimingGap(double radiation_dose, double drug_concentration,
                               bool radiation_first);

    // Set tissue-specific parameters
    void setTissueParameters(TissueType tissue_type);

   private:
    ChemotherapeuticAgent drug_;
    BiologicalSystem tissue_;
    LinearQuadraticParameters lq_params_;

    // Radiosensitization factor based on drug mechanism
    double calculateRadiosensitizationFactor(double drug_concentration);

    // DNA repair inhibition factor
    double calculateRepairInhibition(double drug_concentration);

    // Cell cycle redistribution effect
    double calculateCellCycleEffect(double drug_concentration, double time_hours);

    // Get standard LQ parameters for tissue type
    LinearQuadraticParameters getStandardLQParameters(TissueType tissue_type);

    // Calculate time-dependent drug concentration
    double calculateDrugConcentrationAtTime(double initial_concentration, double time_hours);

    // Apply quantum corrections to synergy model
    double applyQuantumCorrectionsToSynergy(double classical_synergy, double radiation_dose,
                                            double drug_concentration);

    // Calculate dose-response curve
    std::vector<double> calculateDoseResponseCurve(double max_dose, double drug_concentration,
                                                   TreatmentSchedule sequence,
                                                   double time_gap_hours);
};

// Standard linear-quadratic parameters for different tissues
namespace standard_lq_parameters {
extern const LinearQuadraticParameters HEMATOPOIETIC;
extern const LinearQuadraticParameters BREAST_TUMOR;
extern const LinearQuadraticParameters PROSTATE_TUMOR;
extern const LinearQuadraticParameters GLIOBLASTOMA;
extern const LinearQuadraticParameters LUNG_TUMOR;
}  // namespace standard_lq_parameters

}  // namespace chemotherapy
}  // namespace healthcare
}  // namespace rad_ml
