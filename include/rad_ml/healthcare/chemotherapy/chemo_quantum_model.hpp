#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "rad_ml/healthcare/bio_quantum_integration.hpp"

namespace rad_ml {
namespace healthcare {
namespace chemotherapy {

// Core drug properties structure
struct ChemotherapeuticAgent {
    std::string name;
    double molecular_weight;                          // Da
    double log_p;                                     // Octanol-water partition coefficient
    double binding_energy;                            // eV
    double clearance_half_life;                       // hours
    double quantum_tunneling_factor;                  // dimensionless
    std::map<std::string, double> target_affinities;  // eV
};

// Tissue-specific drug response
struct ChemoDrugResponse {
    double intracellular_concentration;             // μmol/L
    double dna_binding_rate;                        // % bound
    double membrane_permeability;                   // cm/s
    double metabolic_degradation_rate;              // 1/h
    double quantum_enhanced_binding;                // dimensionless factor
    std::map<std::string, double> cellular_damage;  // by mechanism
};

// Parameters for drug-specific quantum effects
struct DrugQuantumParameters {
    double tunneling_probability;     // across membrane
    double binding_enhancement;       // quantum enhancement
    double zero_point_contribution;   // zero-point fluctuation impact
    double thermal_contribution;      // thermal effects
    double quantum_coherence_length;  // coherence length in nm
};

// Configuration for drug-specific quantum effects
struct DrugQuantumConfig {
    bool enable_tunneling_effects;
    bool enable_binding_enhancement;
    bool enable_zero_point_contribution;
    double temperature;          // K
    double ph_value;             // pH of environment
    double ionic_strength;       // mol/L
    double dielectric_constant;  // dimensionless
};

// Struct for storing combined chemoradiation parameters
struct ChemoradiationParameters {
    double radiation_dose;             // Gy
    double drug_concentration;         // μmol/L
    double time_gap;                   // hours between treatments
    bool radiation_first;              // true if radiation before drug
    double radiosensitization_factor;  // enhancement due to drug
    double repair_inhibition_factor;   // repair inhibition due to drug
};

// Treatment scheduling enum for the new DrugSpecificQuantumModel
enum TreatmentSchedule { CONCURRENT, RADIATION_FIRST, DRUG_FIRST };

/**
 * Drug-specific quantum modeling for chemoradiation synergy
 * based on QM-MM docking and QM-QSAR approaches
 */
class DrugSpecificQuantumModel {
   public:
    DrugSpecificQuantumModel();

    // Calculate drug-specific binding with QM corrections
    double calculateQMEnhancedBinding(const std::string& drugName, double temperature,
                                      bool enableQuantumEffects = true);

    // Calculate synergy between radiation and drug with QM corrections
    double calculateChemoRadiationSynergy(const std::string& drugName, double radiationDose,
                                          TreatmentSchedule schedule, double temperature);

   private:
    struct DrugParameters {
        double baseBindingAffinity;
        double concurrentSynergyFactor;
        double radiationFirstSynergyFactor;
        double drugFirstSynergyFactor;
        bool containsMetal;
    };

    std::map<std::string, DrugParameters> drugParams_;

    void initializeDrugParameters();
};

// Create a therapeutic drug based on parameters
ChemotherapeuticAgent createStandardDrug(const std::string& drug_name);

// Calculate quantum-corrected drug diffusion across membrane
double calculateQuantumCorrectedDiffusion(const ChemotherapeuticAgent& drug,
                                          const BiologicalSystem& tissue, double temperature,
                                          double membrane_thickness_nm);

// Calculate quantum-enhanced binding of drug to target
double calculateQuantumEnhancedBinding(const ChemotherapeuticAgent& drug,
                                       const std::string& target_name, double temperature,
                                       double concentration);

// Apply quantum corrections to drug efficacy
ChemoDrugResponse applyQuantumCorrectionsToChemotherapy(const ChemotherapeuticAgent& drug,
                                                        const BiologicalSystem& tissue,
                                                        double concentration, double temperature,
                                                        const DrugQuantumConfig& config);

// Calculate combined effect of chemotherapy and radiation
double calculateChemoradiationSynergy(const ChemotherapeuticAgent& drug,
                                      const BiologicalSystem& tissue,
                                      const ChemoradiationParameters& params);

// The standard drugs we support
namespace standard_drugs {
extern const ChemotherapeuticAgent PACLITAXEL;
extern const ChemotherapeuticAgent CISPLATIN;
extern const ChemotherapeuticAgent DOXORUBICIN;
extern const ChemotherapeuticAgent TEMOZOLOMIDE;
extern const ChemotherapeuticAgent FLUOROURACIL;
}  // namespace standard_drugs

}  // namespace chemotherapy
}  // namespace healthcare
}  // namespace rad_ml
