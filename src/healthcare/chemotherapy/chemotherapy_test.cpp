#include <iomanip>
#include <iostream>
#include <vector>

#include "rad_ml/healthcare/chemotherapy/chemo_quantum_model.hpp"
#include "rad_ml/healthcare/chemotherapy/chemoradiation_synergy.hpp"
#include "rad_ml/healthcare/chemotherapy/drug_target_interaction.hpp"

using namespace rad_ml::healthcare;
using namespace rad_ml::healthcare::chemotherapy;

// Print a divider line
void printDivider() { std::cout << std::string(70, '=') << std::endl; }

// Print a section header
void printHeader(const std::string& header)
{
    printDivider();
    std::cout << "== " << header << " ==" << std::endl;
    printDivider();
}

// Print drug information
void printDrugInfo(const ChemotherapeuticAgent& drug)
{
    std::cout << "Drug: " << drug.name << std::endl;
    std::cout << "  Molecular weight: " << drug.molecular_weight << " Da" << std::endl;
    std::cout << "  Log P: " << drug.log_p << std::endl;
    std::cout << "  Binding energy: " << drug.binding_energy << " eV" << std::endl;
    std::cout << "  Half-life: " << drug.clearance_half_life << " hours" << std::endl;
    std::cout << "  Quantum tunneling factor: " << drug.quantum_tunneling_factor << std::endl;
    std::cout << "  Target affinities:" << std::endl;
    for (const auto& target : drug.target_affinities) {
        std::cout << "    - " << target.first << ": " << target.second << " eV" << std::endl;
    }
}

// Print tissue information
void printTissueInfo(const BiologicalSystem& tissue)
{
    std::cout << "Tissue type: ";
    switch (tissue.type) {
        case SOFT_TISSUE:
            std::cout << "Soft Tissue";
            break;
        case BONE:
            std::cout << "Bone";
            break;
        case NERVE:
            std::cout << "Nerve";
            break;
        case MUSCLE:
            std::cout << "Muscle";
            break;
        case BLOOD:
            std::cout << "Blood";
            break;
    }
    std::cout << std::endl;
    std::cout << "  Water content: " << tissue.water_content << std::endl;
    std::cout << "  Cell density: " << tissue.cell_density << " cells/mm³" << std::endl;
    std::cout << "  Effective barrier: " << tissue.effective_barrier << " eV" << std::endl;
    std::cout << "  Repair rate: " << tissue.repair_rate << std::endl;
    std::cout << "  Radiosensitivity: " << tissue.radiosensitivity << std::endl;
}

// Test drug diffusion and binding
void testDrugDiffusionAndBinding()
{
    printHeader("Testing Drug Diffusion and Binding");

    // Create drug (Paclitaxel)
    ChemotherapeuticAgent paclitaxel = createStandardDrug("Paclitaxel");
    printDrugInfo(paclitaxel);

    // Create tissue (breast)
    BiologicalSystem breast_tissue;
    breast_tissue.type = SOFT_TISSUE;
    breast_tissue.water_content = 0.70;
    breast_tissue.cell_density = 2.0e5;
    breast_tissue.effective_barrier = 0.35;
    breast_tissue.repair_rate = 0.4;
    breast_tissue.radiosensitivity = 1.2;

    std::cout << std::endl;
    printTissueInfo(breast_tissue);

    // Test quantum-corrected diffusion
    std::cout << std::endl << "Testing membrane diffusion:" << std::endl;
    double membrane_thickness = 8.0;  // nm

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  Classical diffusion (no quantum): ";
    // Temporarily reduce quantum tunneling factor to simulate classical
    ChemotherapeuticAgent classical_drug = paclitaxel;
    classical_drug.quantum_tunneling_factor = 0.0;
    double classical_diff = calculateQuantumCorrectedDiffusion(classical_drug, breast_tissue, 310.0,
                                                               membrane_thickness);
    std::cout << classical_diff << std::endl;

    std::cout << "  Quantum-corrected diffusion: ";
    double quantum_diff =
        calculateQuantumCorrectedDiffusion(paclitaxel, breast_tissue, 310.0, membrane_thickness);
    std::cout << quantum_diff << std::endl;

    std::cout << "  Quantum enhancement: " << (quantum_diff / classical_diff - 1.0) * 100.0 << "%"
              << std::endl;

    // Test binding
    std::cout << std::endl << "Testing drug-target binding:" << std::endl;
    std::cout << "  Binding to microtubules: ";
    double binding = calculateQuantumEnhancedBinding(paclitaxel, "microtubules", 310.0, 0.5);
    std::cout << binding << std::endl;
}

// Test drug target interaction model
void testDrugTargetInteraction()
{
    printHeader("Testing Drug Target Interaction Model");

    // Create drug (Cisplatin)
    ChemotherapeuticAgent cisplatin = createStandardDrug("Cisplatin");
    printDrugInfo(cisplatin);

    // Create tissue (soft tissue)
    BiologicalSystem tissue;
    tissue.type = SOFT_TISSUE;
    tissue.water_content = 0.70;
    tissue.cell_density = 2.0e5;
    tissue.effective_barrier = 0.35;
    tissue.repair_rate = 0.4;
    tissue.radiosensitivity = 1.0;

    std::cout << std::endl;
    printTissueInfo(tissue);

    // Create interaction model
    DrugTargetInteractionModel model(cisplatin, tissue);

    // Configure quantum effects
    DrugQuantumConfig config;
    config.enable_tunneling_effects = true;
    config.enable_binding_enhancement = true;
    config.enable_zero_point_contribution = true;
    config.temperature = 310.0;
    config.ph_value = 7.4;
    model.setDrugQuantumConfig(config);

    // Test binding
    std::cout << std::endl << "Testing binding probability:" << std::endl;
    std::cout << std::fixed << std::setprecision(6);

    double temp = 310.0;
    std::vector<double> cell_sizes = {8.0, 15.0, 25.0};
    std::vector<double> concentrations = {0.1, 0.5, 1.0};

    std::cout << "  Cell size effects:" << std::endl;
    for (double size : cell_sizes) {
        double binding = model.calculateQuantumEnhancedBinding(temp, size, 0.5);
        std::cout << "    - " << size << " µm: " << binding << std::endl;
    }

    std::cout << "  Concentration effects:" << std::endl;
    for (double conc : concentrations) {
        double binding = model.calculateQuantumEnhancedBinding(temp, 15.0, conc);
        std::cout << "    - " << conc << " µmol/L: " << binding << std::endl;
    }

    // Test time-dependent binding
    std::cout << std::endl << "Testing time-dependent binding:" << std::endl;
    std::vector<double> binding_curve = model.modelBindingDynamics(1.0, 24.0, 4.0);

    std::cout << "  Binding curve over 24 hours:" << std::endl;
    for (size_t i = 0; i < binding_curve.size(); ++i) {
        double time = i * 4.0;
        std::cout << "    - " << time << " hours: " << binding_curve[i] << std::endl;
    }

    // Get complete drug response
    std::cout << std::endl << "Drug response at 0.5 µmol/L after 12 hours:" << std::endl;
    ChemoDrugResponse response = model.getDrugResponse(0.5, 12.0, temp);

    std::cout << "  Intracellular concentration: " << response.intracellular_concentration
              << " µmol/L" << std::endl;
    std::cout << "  DNA binding rate: " << response.dna_binding_rate << std::endl;
    std::cout << "  Membrane permeability: " << response.membrane_permeability << " cm/s"
              << std::endl;
    std::cout << "  Metabolic degradation rate: " << response.metabolic_degradation_rate << " 1/h"
              << std::endl;
    std::cout << "  Quantum enhanced binding: " << response.quantum_enhanced_binding << std::endl;

    std::cout << "  Cellular damage:" << std::endl;
    for (const auto& damage : response.cellular_damage) {
        std::cout << "    - " << damage.first << ": " << damage.second << std::endl;
    }
}

// Test chemoradiation synergy model
void testChemoradiationSynergy()
{
    printHeader("Testing Chemoradiation Synergy Model");

    // Create drug (Cisplatin)
    ChemotherapeuticAgent cisplatin = createStandardDrug("Cisplatin");

    // Create tissue (lung tumor)
    BiologicalSystem tissue;
    tissue.type = SOFT_TISSUE;
    tissue.water_content = 0.70;
    tissue.cell_density = 2.0e5;
    tissue.effective_barrier = 0.35;
    tissue.repair_rate = 0.4;
    tissue.radiosensitivity = 1.2;

    // Set LQ parameters for lung tumor
    LinearQuadraticParameters lung_params = standard_lq_parameters::LUNG_TUMOR;

    // Create synergy model
    ChemoradiationSynergyModel model(cisplatin, tissue, lung_params);

    // Test synergy
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Testing synergy for Cisplatin + Radiation:" << std::endl;

    double radiation_dose = 2.0;      // Gy
    double drug_concentration = 0.5;  // µmol/L

    double synergy = model.calculateSynergy(radiation_dose, drug_concentration, 0.0);
    std::cout << "  Synergy (concurrent): " << synergy << std::endl;

    // Test different sequencing strategies
    std::cout << std::endl << "Testing treatment sequencing:" << std::endl;
    std::cout << "  Concurrent treatment efficacy: "
              << model.predictEfficacy(radiation_dose, drug_concentration, CONCURRENT, 0.0)
              << std::endl;

    std::cout << "  Radiation first (6h gap) efficacy: "
              << model.predictEfficacy(radiation_dose, drug_concentration, RADIATION_FIRST, 6.0)
              << std::endl;

    std::cout << "  Drug first (6h gap) efficacy: "
              << model.predictEfficacy(radiation_dose, drug_concentration, DRUG_FIRST, 6.0)
              << std::endl;

    // Test survival fraction
    std::cout << std::endl << "Testing cell survival fraction:" << std::endl;
    std::cout << "  Radiation alone (2 Gy): "
              << std::exp(-(lung_params.alpha * 2.0 + lung_params.beta * 2.0 * 2.0)) << std::endl;

    std::cout << "  Drug alone (0.5 µmol/L): "
              << 1.0 - (drug_concentration / (drug_concentration + 1.0)) << std::endl;

    std::cout << "  Combined treatment: "
              << model.calculateSurvivalFraction(radiation_dose, drug_concentration, CONCURRENT,
                                                 0.0)
              << std::endl;

    // Test therapeutic ratio
    std::cout << std::endl << "Testing therapeutic ratio:" << std::endl;
    double tumor_dose = 2.0;   // Gy
    double normal_dose = 1.0;  // Gy

    std::cout << "  Therapeutic ratio (concurrent): "
              << model.calculateTherapeuticRatio(tumor_dose, normal_dose, drug_concentration,
                                                 CONCURRENT, 0.0)
              << std::endl;

    // Find optimal timing
    std::cout << std::endl << "Finding optimal timing:" << std::endl;
    double optimal_time_radiation_first =
        model.getOptimalTimingGap(radiation_dose, drug_concentration, true);
    std::cout << "  Optimal gap (radiation first): " << optimal_time_radiation_first << " hours"
              << std::endl;

    double optimal_time_drug_first =
        model.getOptimalTimingGap(radiation_dose, drug_concentration, false);
    std::cout << "  Optimal gap (drug first): " << optimal_time_drug_first << " hours" << std::endl;

    // Compare drugs
    std::cout << std::endl << "Comparing different drugs:" << std::endl;
    std::vector<std::string> drug_names = {"Cisplatin", "Paclitaxel", "Doxorubicin",
                                           "Fluorouracil"};

    for (const auto& name : drug_names) {
        ChemotherapeuticAgent drug = createStandardDrug(name);
        ChemoradiationSynergyModel drug_model(drug, tissue, lung_params);

        double drug_efficacy =
            drug_model.predictEfficacy(radiation_dose, drug_concentration, CONCURRENT, 0.0);
        std::cout << "  " << name << ": " << drug_efficacy << std::endl;
    }
}

// Test quantum effects on chemoradiation
void testQuantumEffects()
{
    printHeader("Testing Quantum Effects on Chemoradiation");

    // Create drug (Paclitaxel)
    ChemotherapeuticAgent paclitaxel = createStandardDrug("Paclitaxel");

    // Create tissue (breast tumor)
    BiologicalSystem tissue;
    tissue.type = SOFT_TISSUE;
    tissue.water_content = 0.70;
    tissue.cell_density = 2.0e5;
    tissue.effective_barrier = 0.35;
    tissue.repair_rate = 0.4;
    tissue.radiosensitivity = 1.0;

    // Test drug interaction with and without quantum effects
    std::cout << "Testing drug interaction with/without quantum effects:" << std::endl;

    // Create interaction model with quantum effects
    DrugTargetInteractionModel quantum_model(paclitaxel, tissue);
    DrugQuantumConfig quantum_config;
    quantum_config.enable_tunneling_effects = true;
    quantum_config.enable_binding_enhancement = true;
    quantum_config.enable_zero_point_contribution = true;
    quantum_model.setDrugQuantumConfig(quantum_config);

    // Create interaction model without quantum effects
    DrugTargetInteractionModel classical_model(paclitaxel, tissue);
    DrugQuantumConfig classical_config;
    classical_config.enable_tunneling_effects = false;
    classical_config.enable_binding_enhancement = false;
    classical_config.enable_zero_point_contribution = false;
    classical_model.setDrugQuantumConfig(classical_config);

    // Compare binding for different cell sizes
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  Binding comparison for different cell sizes:" << std::endl;
    std::vector<double> cell_sizes = {8.0, 15.0, 25.0};

    for (double size : cell_sizes) {
        double quantum_binding = quantum_model.calculateQuantumEnhancedBinding(310.0, size, 0.5);
        double classical_binding =
            classical_model.calculateQuantumEnhancedBinding(310.0, size, 0.5);
        double enhancement = (quantum_binding / classical_binding - 1.0) * 100.0;

        std::cout << "    - " << size << " µm: "
                  << "Q: " << quantum_binding << ", C: " << classical_binding << " (" << enhancement
                  << "% enhancement)" << std::endl;
    }

    // Compare drug responses
    std::cout << std::endl << "  Drug response comparison at 12 hours:" << std::endl;
    ChemoDrugResponse quantum_response = quantum_model.getDrugResponse(0.5, 12.0, 310.0);
    ChemoDrugResponse classical_response = classical_model.getDrugResponse(0.5, 12.0, 310.0);

    std::cout << "    - Intracellular concentration: "
              << "Q: " << quantum_response.intracellular_concentration
              << ", C: " << classical_response.intracellular_concentration << " ("
              << (quantum_response.intracellular_concentration /
                      classical_response.intracellular_concentration -
                  1.0) *
                     100.0
              << "% diff)" << std::endl;

    std::cout << "    - DNA binding rate: "
              << "Q: " << quantum_response.dna_binding_rate
              << ", C: " << classical_response.dna_binding_rate << " ("
              << (quantum_response.dna_binding_rate / classical_response.dna_binding_rate - 1.0) *
                     100.0
              << "% diff)" << std::endl;

    // Test chemoradiation with varying water content
    std::cout << std::endl
              << "Testing quantum effects in tissues with different water content:" << std::endl;
    std::vector<double> water_contents = {0.1, 0.3, 0.5, 0.7, 0.9};

    for (double water : water_contents) {
        tissue.water_content = water;

        ChemoradiationSynergyModel synergy_model(paclitaxel, tissue);
        double efficacy = synergy_model.predictEfficacy(2.0, 0.5, CONCURRENT, 0.0);

        std::cout << "  Water content " << water << ": Efficacy = " << efficacy << std::endl;
    }
}

int main()
{
    std::cout << "==== Radiation-Based Healthcare Quantum Modeling Framework ====" << std::endl;
    std::cout << "==== Chemotherapy Extension Test Program ====" << std::endl;
    printDivider();

    // Test drug diffusion and binding
    testDrugDiffusionAndBinding();

    // Test drug target interaction
    testDrugTargetInteraction();

    // Test chemoradiation synergy
    testChemoradiationSynergy();

    // Test quantum effects on chemoradiation
    testQuantumEffects();

    printDivider();
    std::cout << "Tests completed successfully!" << std::endl;

    return 0;
}
