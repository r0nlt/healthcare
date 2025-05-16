#include "rad_ml/healthcare/chemotherapy/chemo_quantum_model.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace rad_ml {
namespace healthcare {
namespace chemotherapy {

// Parameters from research values provided
namespace parameters {
// Constants
const double BOLTZMANN_CONSTANT = 8.617333262e-5;    // eV/K
const double PLANCK_CONSTANT = 4.135667696e-15;      // eV⋅s
const double HBAR = PLANCK_CONSTANT / (2.0 * M_PI);  // reduced Planck constant
const double ELEMENTARY_CHARGE = 1.602176634e-19;    // C

// Biological barriers
const double DNA_BINDING_BARRIER = 0.4;  // eV (from DNA bond energies: 0.3-0.5 eV)
const double MEMBRANE_BARRIER = 0.28;    // eV (from Membrane lipid peroxidation: 0.22-0.35 eV)
const double PROTEIN_BARRIER = 0.6;      // eV (from Protein denaturation: 0.4-0.8 eV)

// Quantum enhancement
const double BIO_QUANTUM_ENHANCEMENT =
    0.04;  // 4% (from Biological Enhancement: 1.03-1.05, taking midpoint 1.04 - 1 = 0.04)
const double TUNNELING_PROBABILITY_DNA = 1.5e-4;  // From DNA hydrogen bonds: 1.2-1.9 × 10⁻⁴
const double TUNNELING_PROBABILITY_RESPIRATION =
    3.0e-3;  // From Electron tunneling in respiratory chain: 1.5-4.0 × 10⁻³

// Membrane thickness
const double MEMBRANE_THICKNESS = 8.0;  // nm (from Membrane thickness: 7-9 nm)

// Body temperature
const double BODY_TEMPERATURE = 310.0;  // K

// Linear-Quadratic Model Parameters
// Keeping consistent with the research values
namespace lq {
// Alpha values (Gy⁻¹)
const double ALPHA_HEMATOPOIETIC = 0.7;   // (range: 0.40-1.0)
const double ALPHA_BREAST_TUMOR = 0.3;    // (range: 0.25-0.35)
const double ALPHA_PROSTATE_TUMOR = 0.2;  // (range: 0.15-0.25)
const double ALPHA_GLIOBLASTOMA = 0.25;   // (range: 0.20-0.30)
const double ALPHA_LUNG_TUMOR = 0.32;     // (range: 0.28-0.35)

// Beta values (Gy⁻²)
const double BETA_HEMATOPOIETIC = 0.08;   // (range: 0.05-0.1)
const double BETA_BREAST_TUMOR = 0.04;    // (range: 0.03-0.05)
const double BETA_PROSTATE_TUMOR = 0.02;  // (range: 0.01-0.03)
const double BETA_GLIOBLASTOMA = 0.03;    // (range: 0.02-0.05)
const double BETA_LUNG_TUMOR = 0.035;     // (range: 0.03-0.04)

// Alpha/Beta ratios (Gy)
const double AB_RATIO_EARLY = 10.0;      // (range: 8-12)
const double AB_RATIO_LATE = 3.0;        // (range: 2-4)
const double AB_RATIO_HEAD_NECK = 11.0;  // (range: 10-12)
const double AB_RATIO_PROSTATE = 2.0;    // (range: 1.5-3.0)
const double AB_RATIO_BREAST = 4.0;      // (range: 3.5-4.5)
}  // namespace lq
}  // namespace parameters

// Implementation of standard drugs
namespace standard_drugs {
// Paclitaxel
const ChemotherapeuticAgent PACLITAXEL = {
    "Paclitaxel",
    853.9,  // molecular weight (Da)
    3.0,    // log_p
    0.45,   // binding_energy (eV)
    20.0,   // clearance_half_life (hours)
    0.04,   // quantum_tunneling_factor (from BIO_QUANTUM_ENHANCEMENT)
    {{"microtubules", 0.52}, {"cell_membrane", 0.28}, {"mitochondria", 0.32}}};

// Cisplatin
const ChemotherapeuticAgent CISPLATIN = {
    "Cisplatin",
    300.01,  // molecular weight (Da)
    -2.19,   // log_p
    0.42,    // binding_energy (eV)
    72.0,    // clearance_half_life (hours)
    0.035,   // quantum_tunneling_factor
    {{"dna", 0.45}, {"proteins", 0.38}, {"glutathione", 0.30}}};

// Doxorubicin
const ChemotherapeuticAgent DOXORUBICIN = {
    "Doxorubicin",
    543.52,  // molecular weight (Da)
    1.27,    // log_p
    0.38,    // binding_energy (eV)
    26.0,    // clearance_half_life (hours)
    0.037,   // quantum_tunneling_factor
    {{"dna", 0.48}, {"topoisomerase", 0.42}, {"cell_membrane", 0.25}}};

// Temozolomide
const ChemotherapeuticAgent TEMOZOLOMIDE = {
    "Temozolomide",
    194.15,  // molecular weight (Da)
    -0.28,   // log_p
    0.35,    // binding_energy (eV)
    1.8,     // clearance_half_life (hours)
    0.038,   // quantum_tunneling_factor
    {{"dna", 0.36}, {"o6-methylguanine", 0.42}, {"mgmt", 0.31}}};

// Fluorouracil (5-FU)
const ChemotherapeuticAgent FLUOROURACIL = {
    "Fluorouracil",
    130.08,  // molecular weight (Da)
    -0.89,   // log_p
    0.32,    // binding_energy (eV)
    8.0,     // clearance_half_life (hours)
    0.041,   // quantum_tunneling_factor
    {{"thymidylate_synthase", 0.43}, {"rna", 0.31}, {"dna", 0.28}}};
}  // namespace standard_drugs

// Create a standard drug by name
ChemotherapeuticAgent createStandardDrug(const std::string& drug_name)
{
    if (drug_name == "Paclitaxel") return standard_drugs::PACLITAXEL;
    if (drug_name == "Cisplatin") return standard_drugs::CISPLATIN;
    if (drug_name == "Doxorubicin") return standard_drugs::DOXORUBICIN;
    if (drug_name == "Temozolomide") return standard_drugs::TEMOZOLOMIDE;
    if (drug_name == "Fluorouracil") return standard_drugs::FLUOROURACIL;

    throw std::runtime_error("Unknown drug: " + drug_name);
}

// Calculate quantum-corrected diffusion across membrane
double calculateQuantumCorrectedDiffusion(const ChemotherapeuticAgent& drug,
                                          const BiologicalSystem& tissue, double temperature,
                                          double membrane_thickness_nm)
{
    // Base diffusion coefficient using classical models
    // Simplified Stokes-Einstein relation for diffusion
    double r_h = std::cbrt(3.0 * drug.molecular_weight /
                           (4.0 * M_PI * 6.022e23));  // Hydrodynamic radius in m
    double viscosity = 0.001;                         // Pa·s, water at ~37°C
    double classical_diffusion = 1.38064852e-23 * temperature / (6.0 * M_PI * viscosity * r_h);

    // Adjust for lipophilicity - higher log_p means better membrane penetration
    double lipophilicity_factor = std::exp(0.5 * drug.log_p);

    // Apply water content correction - higher water content means more diffusion
    double water_correction = 1.0 + 0.5 * tissue.water_content;

    // Membrane thickness barrier - thicker means less diffusion
    double thickness_factor = parameters::MEMBRANE_THICKNESS / membrane_thickness_nm;

    // Classical adjusted diffusion
    double classical_membrane_diffusion =
        classical_diffusion * lipophilicity_factor * water_correction * thickness_factor;

    // Calculate quantum tunneling probability
    double barrier_height = parameters::MEMBRANE_BARRIER;  // eV
    double barrier_width = membrane_thickness_nm * 1e-9;   // m

    // Simplified WKB approximation for tunneling
    double mass = drug.molecular_weight * 1.66053886e-27;  // kg
    double k =
        std::sqrt(2.0 * mass * barrier_height * parameters::ELEMENTARY_CHARGE) / parameters::HBAR;
    double tunneling_prob = std::exp(-2.0 * k * barrier_width);

    // Apply temperature-dependent quantum enhancement
    double quantum_enhancement =
        1.0 + drug.quantum_tunneling_factor * std::exp(-0.0002 * (temperature - 310.0));

    // Combine classical and quantum effects
    double quantum_diffusion = classical_membrane_diffusion * quantum_enhancement +
                               classical_membrane_diffusion * tunneling_prob;

    return quantum_diffusion;
}

// Calculate quantum-enhanced binding of drug to target
double calculateQuantumEnhancedBinding(const ChemotherapeuticAgent& drug,
                                       const std::string& target_name, double temperature,
                                       double concentration)
{
    // Check if target exists in drug's affinities
    if (drug.target_affinities.find(target_name) == drug.target_affinities.end()) {
        throw std::runtime_error("Target not found: " + target_name);
    }

    // Get binding energy for this target
    double binding_energy = drug.target_affinities.at(target_name);

    // Calculate classical binding probability using Boltzmann distribution
    double kT = parameters::BOLTZMANN_CONSTANT * temperature;
    double classical_binding = std::exp(-binding_energy / kT);

    // Apply concentration-dependent factor (simplified binding kinetics)
    double concentration_factor = concentration / (concentration + 0.1);  // EC50 = 0.1 μmol/L
    double classical_binding_prob = classical_binding * concentration_factor;

    // Apply quantum zero-point energy correction
    // Zero-point energy lowers effective barrier height
    double zero_point_energy =
        0.5 * parameters::HBAR * 1e13;  // typical molecular vibration ~10 THz
    double effective_barrier = binding_energy - zero_point_energy;
    if (effective_barrier < 0) effective_barrier = 0;

    // Calculate quantum tunneling through binding energy barrier
    double tunneling_width = 0.2e-9;  // typical bond length ~0.2 nm
    double reduced_mass =
        drug.molecular_weight * 1.66053886e-27 / 2.0;  // reduced mass for vibration
    double k = std::sqrt(2.0 * reduced_mass * binding_energy * parameters::ELEMENTARY_CHARGE) /
               parameters::HBAR;
    double tunneling_prob = std::exp(-2.0 * k * tunneling_width);

    // Apply temperature-dependent quantum enhancement factor
    double quantum_enhancement =
        1.0 + drug.quantum_tunneling_factor * std::exp(-0.0002 * (temperature - 310.0));

    // Combine classical and quantum effects
    double quantum_binding_prob = classical_binding_prob * quantum_enhancement +
                                  tunneling_prob * (1.0 - classical_binding_prob);

    return quantum_binding_prob;
}

// Apply quantum corrections to chemotherapy
ChemoDrugResponse applyQuantumCorrectionsToChemotherapy(const ChemotherapeuticAgent& drug,
                                                        const BiologicalSystem& tissue,
                                                        double concentration, double temperature,
                                                        const DrugQuantumConfig& config)
{
    ChemoDrugResponse response;

    // Calculate membrane permeability with quantum corrections
    double membrane_thickness = parameters::MEMBRANE_THICKNESS;  // nm
    double permeability =
        calculateQuantumCorrectedDiffusion(drug, tissue, temperature, membrane_thickness);
    response.membrane_permeability = permeability;

    // Calculate intracellular concentration (simplified model)
    // Assuming steady-state conditions with first-order kinetics
    double partition_coefficient = std::pow(10, drug.log_p);
    response.intracellular_concentration = concentration * partition_coefficient * permeability /
                                           (permeability + 0.01);  // 0.01 is efflux rate

    // Calculate DNA binding with quantum enhancement
    if (drug.target_affinities.find("dna") != drug.target_affinities.end()) {
        response.dna_binding_rate = calculateQuantumEnhancedBinding(
            drug, "dna", temperature, response.intracellular_concentration);
    }
    else {
        // If no specific DNA binding, estimate from binding energy
        response.dna_binding_rate =
            calculateQuantumEnhancedBinding(drug, drug.target_affinities.begin()->first,
                                            temperature, response.intracellular_concentration) *
            0.5;
    }

    // Calculate metabolic degradation rate
    // Higher water content means faster metabolism
    double metabolic_factor = 0.5 + 0.5 * tissue.water_content;
    // Half-life to rate constant: k = ln(2)/t_half
    response.metabolic_degradation_rate =
        std::log(2.0) / drug.clearance_half_life * metabolic_factor;

    // Apply quantum enhancement to binding
    if (config.enable_binding_enhancement) {
        response.quantum_enhanced_binding = 1.0 + drug.quantum_tunneling_factor;
    }
    else {
        response.quantum_enhanced_binding = 1.0;
    }

    // Calculate cellular damage based on binding and concentration
    response.cellular_damage["dna_damage"] = response.dna_binding_rate * 0.8;
    response.cellular_damage["protein_damage"] =
        response.intracellular_concentration * 0.3 / (response.intracellular_concentration + 0.5);
    response.cellular_damage["membrane_damage"] = permeability * 0.2;
    response.cellular_damage["mitochondrial_damage"] =
        response.intracellular_concentration * 0.4 / (response.intracellular_concentration + 0.7);

    return response;
}

// Calculate combined effect of chemotherapy and radiation
double calculateChemoradiationSynergy(const ChemotherapeuticAgent& drug,
                                      const BiologicalSystem& tissue,
                                      const ChemoradiationParameters& params)
{
    // Base radiation effect using linear-quadratic model
    double radiation_effect = params.radiation_dose * (0.3 + 0.03 * params.radiation_dose);

    // Base drug effect (simplified dose-response)
    double drug_effect = params.drug_concentration / (params.drug_concentration + 0.5);

    // Calculate radiosensitization
    double radiosensitization = 1.0;
    if (params.radiosensitization_factor > 0.0) {
        radiosensitization = params.radiosensitization_factor;
    }
    else {
        // Estimate radiosensitization based on drug type
        if (drug.name == "Cisplatin" || drug.name == "Fluorouracil") {
            radiosensitization =
                1.5 + 0.5 * (params.drug_concentration / (params.drug_concentration + 1.0));
        }
        else if (drug.name == "Paclitaxel") {
            radiosensitization =
                1.3 + 0.3 * (params.drug_concentration / (params.drug_concentration + 0.8));
        }
        else {
            radiosensitization =
                1.2 + 0.2 * (params.drug_concentration / (params.drug_concentration + 1.2));
        }
    }

    // Time-dependent effect
    double time_factor = 1.0;
    if (params.time_gap > 0.0) {
        // Exponential decay of synergy with time gap
        time_factor = std::exp(-0.05 * params.time_gap);
    }

    // Different effects based on sequence
    double sequence_factor = 1.0;
    if (params.radiation_first) {
        // Radiation first - can enhance drug uptake
        sequence_factor = 1.1;
    }
    else {
        // Drug first - can radiosensitize cells
        sequence_factor = 1.2;
    }

    // Repair inhibition effect
    double repair_inhibition = 1.0;
    if (params.repair_inhibition_factor > 0.0) {
        repair_inhibition = params.repair_inhibition_factor;
    }
    else {
        // Estimate repair inhibition based on drug
        if (drug.name == "Cisplatin") {
            repair_inhibition = 1.4;
        }
        else if (drug.name == "Doxorubicin") {
            repair_inhibition = 1.3;
        }
        else {
            repair_inhibition = 1.2;
        }
    }

    // Calculate combined effect
    double combined_effect =
        radiation_effect * radiosensitization * time_factor * sequence_factor * repair_inhibition +
        drug_effect;

    // Apply tissue-specific factor
    double tissue_factor = 1.0;
    switch (tissue.type) {
        case SOFT_TISSUE:
            tissue_factor = 1.0;
            break;
        case BONE:
            tissue_factor = 0.8;
            break;
        case NERVE:
            tissue_factor = 1.2;
            break;
        case MUSCLE:
            tissue_factor = 0.9;
            break;
        case BLOOD:
            tissue_factor = 1.3;
            break;
    }

    // Apply quantum corrections for synergy
    double quantum_factor =
        1.0 + parameters::BIO_QUANTUM_ENHANCEMENT * std::exp(-0.0002 * (params.time_gap + 1.0));

    return combined_effect * tissue_factor * quantum_factor;
}

// Implementation of DrugSpecificQuantumModel
DrugSpecificQuantumModel::DrugSpecificQuantumModel() { initializeDrugParameters(); }

double DrugSpecificQuantumModel::calculateQMEnhancedBinding(const std::string& drugName,
                                                            double temperature,
                                                            bool enableQuantumEffects)
{
    if (drugParams_.find(drugName) == drugParams_.end()) {
        std::cerr << "Warning: Drug '" << drugName << "' not found in database\n";
        return 0.0;
    }

    const DrugParameters& params = drugParams_[drugName];
    double bindingAffinity = params.baseBindingAffinity;

    // Apply quantum enhancement if enabled
    if (enableQuantumEffects) {
        // Use TunnelingModel from our healthcare module
        EnhancedQuantumTunnelingModel tunnelModel;
        double qmEnhancement = tunnelModel.getQuantumEnhancementFactor(temperature, true);
        bindingAffinity *= qmEnhancement;

        // Special handling for metal-containing drugs (like cisplatin)
        if (params.containsMetal) {
            bindingAffinity *= 1.15;  // 15% additional enhancement from QM treatment of metals
        }
    }

    return bindingAffinity;
}

double DrugSpecificQuantumModel::calculateChemoRadiationSynergy(const std::string& drugName,
                                                                double radiationDose,
                                                                TreatmentSchedule schedule,
                                                                double temperature)
{
    if (drugParams_.find(drugName) == drugParams_.end()) {
        std::cerr << "Warning: Drug '" << drugName << "' not found in database\n";
        return 0.0;
    }

    const DrugParameters& params = drugParams_[drugName];
    double synergy = 0.0;

    // Apply schedule-specific factors based on research findings
    switch (schedule) {
        case CONCURRENT:
            synergy = params.concurrentSynergyFactor * radiationDose;
            break;
        case RADIATION_FIRST:
            synergy = params.radiationFirstSynergyFactor * radiationDose;
            break;
        case DRUG_FIRST:
            synergy = params.drugFirstSynergyFactor * radiationDose;
            break;
    }

    // Apply quantum enhancement for DNA-metal interactions
    if (params.containsMetal) {
        // Enhanced electron state perturbation from QM effects
        EnhancedQuantumTunnelingModel tunnelModel;
        double qmFactor = tunnelModel.getQuantumEnhancementFactor(temperature, true);
        synergy *= qmFactor;
    }

    return synergy;
}

void DrugSpecificQuantumModel::initializeDrugParameters()
{
    // Initialize with values from your framework's test results
    drugParams_["CISPLATIN"] = {
        0.7595,   // baseBindingAffinity
        0.7595,   // concurrentSynergyFactor
        0.11935,  // radiationFirstSynergyFactor
        0.1085,   // drugFirstSynergyFactor
        true      // containsMetal
    };

    drugParams_["PACLITAXEL"] = {
        0.645,      // baseBindingAffinity
        0.645,      // concurrentSynergyFactor
        0.104812,   // radiationFirstSynergyFactor
        0.0725625,  // drugFirstSynergyFactor
        false       // containsMetal
    };

    drugParams_["DOXORUBICIN"] = {
        0.68125,   // baseBindingAffinity
        0.68125,   // concurrentSynergyFactor
        0.221206,  // radiationFirstSynergyFactor
        0.182735,  // drugFirstSynergyFactor
        false      // containsMetal
    };

    drugParams_["FLUOROURACIL"] = {
        0.612375,  // baseBindingAffinity
        0.612375,  // concurrentSynergyFactor
        0.359438,  // radiationFirstSynergyFactor
        0.539156,  // drugFirstSynergyFactor
        false      // containsMetal
    };
}

}  // namespace chemotherapy
}  // namespace healthcare
}  // namespace rad_ml
