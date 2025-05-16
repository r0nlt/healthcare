#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "rad_ml/healthcare/enhanced_radiation_therapy_model.hpp"

using namespace rad_ml::healthcare;
using namespace rad_ml::healthcare::cell_biology;

// Helper function to print DNA damage profile
void printDNADamageProfile(const DNADamageProfile& damage, const std::string& label)
{
    std::cout << "=== " << label << " ===" << std::endl;
    std::cout << "Single-strand breaks:  " << damage.single_strand_breaks << std::endl;
    std::cout << "Double-strand breaks:  " << damage.double_strand_breaks << std::endl;
    std::cout << "Base damages:          " << damage.base_damages << std::endl;
    std::cout << "Clustered damages:     " << damage.clustered_damages << std::endl;
    std::cout << "Complex DSBs:          " << damage.complex_dsb << std::endl;
    std::cout << "Total damage:          " << damage.getTotalDamage() << std::endl;
    std::cout << "Simple/Complex ratio:  " << damage.getSimpleToComplexRatio() << std::endl;
    std::cout << std::endl;
}

// Helper function to print cell cycle distribution
void printCellCycleDistribution(const CellCycleDistribution& dist, const std::string& label)
{
    std::cout << "=== " << label << " ===" << std::endl;
    std::cout << "G0 fraction: " << dist.g0_fraction * 100.0 << "%" << std::endl;
    std::cout << "G1 fraction: " << dist.g1_fraction * 100.0 << "%" << std::endl;
    std::cout << "S  fraction: " << dist.s_fraction * 100.0 << "%" << std::endl;
    std::cout << "G2 fraction: " << dist.g2_fraction * 100.0 << "%" << std::endl;
    std::cout << "M  fraction: " << dist.m_fraction * 100.0 << "%" << std::endl;
    std::cout << std::endl;
}

// Helper function to print radiation therapy results
void printRadiationTherapyResults(const EnhancedRadiationTherapyResult& result,
                                  const std::string& label)
{
    std::cout << "=== " << label << " ===" << std::endl;
    std::cout << "Survival fraction:             " << result.survival_fraction << std::endl;
    std::cout << "Survival fraction (no quantum): " << result.survival_fraction_no_quantum
              << std::endl;
    std::cout << "Quantum enhancement factor:     " << result.quantum_enhancement_factor
              << std::endl;
    std::cout << "Tunneling probability:          " << result.tunneling_probability << std::endl;
    std::cout << "Zero-point contribution:        " << result.zero_point_contribution << std::endl;
    std::cout << "DSBs per Gy:                    " << result.dsb_per_gy << std::endl;
    std::cout << "Complex/Simple ratio:           " << result.complex_to_simple_ratio << std::endl;
    std::cout << std::endl;
}

int main()
{
    std::cout << "Enhanced Healthcare Framework Example" << std::endl;
    std::cout << "=====================================" << std::endl << std::endl;

    // 1. Create configuration
    EnhancedRadiationTherapyConfig config;
    config.enable_quantum_effects = true;
    config.enable_cell_cycle_dependence = true;
    config.enable_monte_carlo_simulation = true;
    config.enable_dna_track_structure = true;
    config.temperature = 310.0;         // K (body temperature)
    config.cell_size_threshold = 10.0;  // µm

    // 2. Create the radiation therapy model
    EnhancedRadiationTherapyModel model(config);

    // 3. Create a tumor biological system
    BiologicalSystemExtended tumor_system =
        CellCycleModel::createDefaultBiologicalSystem(TissueType::TUMOR_RAPIDLY_DIVIDING);

    // 4. Add cell cycle distribution
    CellCycleDistribution cycle_dist;
    cycle_dist.g0_fraction = 0.1;  // Quiescent cells
    cycle_dist.g1_fraction = 0.3;  // G1 phase
    cycle_dist.s_fraction = 0.4;   // S phase (DNA synthesis)
    cycle_dist.g2_fraction = 0.1;  // G2 phase
    cycle_dist.m_fraction = 0.1;   // Mitosis
    tumor_system.cycle_distribution = cycle_dist;

    // 5. Set phase-specific radiosensitivity
    std::map<CellCyclePhase, PhaseSensitivity> phase_sensitivity;

    // G1 phase sensitivity
    PhaseSensitivity g1_sensitivity;
    g1_sensitivity.alpha = 0.2;
    g1_sensitivity.beta = 0.02;
    phase_sensitivity[CellCyclePhase::G1] = g1_sensitivity;

    // S phase sensitivity (most resistant)
    PhaseSensitivity s_sensitivity;
    s_sensitivity.alpha = 0.1;
    s_sensitivity.beta = 0.01;
    phase_sensitivity[CellCyclePhase::S] = s_sensitivity;

    // G2/M phase sensitivity (most sensitive)
    PhaseSensitivity g2m_sensitivity;
    g2m_sensitivity.alpha = 0.4;
    g2m_sensitivity.beta = 0.04;
    phase_sensitivity[CellCyclePhase::G2] = g2m_sensitivity;
    phase_sensitivity[CellCyclePhase::M] = g2m_sensitivity;

    // Add to biological system
    tumor_system.phase_sensitivity = phase_sensitivity;

    // 6. Configure oxygen level
    tumor_system.oxygen_tension = 5.0;  // 5% O2 (hypoxic)

    // 7. Set the biological system in the model
    model.setBiologicalSystem(tumor_system);

    // 8. Set radiation parameters
    RadiationParams radiation_params;
    radiation_params.type = RadiationType::PHOTON;
    radiation_params.dose = 0.0;       // Will be set later
    radiation_params.energy = 6.0;     // 6 MV photons
    radiation_params.dose_rate = 2.0;  // 2 Gy/min
    model.setRadiationParams(radiation_params);

    // 9. Print initial conditions
    std::cout << "Tumor System Parameters:" << std::endl;
    std::cout << "- Tissue Type: Rapidly Dividing Tumor" << std::endl;
    std::cout << "- Oxygen Tension: " << tumor_system.oxygen_tension << "%" << std::endl;
    std::cout << "- Radiation Type: "
              << (radiation_params.type == RadiationType::PHOTON ? "Photon" : "Other") << std::endl;
    std::cout << "- Energy: " << radiation_params.energy << " MV" << std::endl;
    std::cout << "- Dose Rate: " << radiation_params.dose_rate << " Gy/min" << std::endl;
    std::cout << std::endl;

    // 10. Print initial cell cycle distribution
    printCellCycleDistribution(tumor_system.cycle_distribution, "Initial Cell Cycle Distribution");

    // 11. Single dose radiation effect
    double single_dose = 2.0;  // 2 Gy
    std::cout << "Calculating radiation effect for " << single_dose << " Gy..." << std::endl;
    EnhancedRadiationTherapyResult single_result = model.predictRadiationEffect(single_dose);

    // 12. Print results
    printRadiationTherapyResults(single_result, "Radiation Effects (2 Gy)");
    printDNADamageProfile(single_result.initial_damage, "Initial DNA Damage (2 Gy)");
    printDNADamageProfile(single_result.residual_damage, "Residual DNA Damage (24h, 2 Gy)");
    printCellCycleDistribution(single_result.final_cycle_distribution,
                               "Post-Radiation Cell Cycle Distribution (24h, 2 Gy)");

    // 13. Compare with and without quantum effects
    std::cout << "Comparing results with and without quantum effects:" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Survival fraction with quantum:    " << single_result.survival_fraction
              << std::endl;
    std::cout << "Survival fraction without quantum: " << single_result.survival_fraction_no_quantum
              << std::endl;
    std::cout << "Quantum enhancement factor:        " << single_result.quantum_enhancement_factor
              << std::endl;
    std::cout << std::endl;

    // 14. Fractionated radiation effect
    double dose_per_fraction = 2.0;        // 2 Gy per fraction
    int num_fractions = 5;                 // 5 fractions
    double time_between_fractions = 24.0;  // 24 hours between fractions

    std::cout << "Calculating fractionated radiation effect..." << std::endl;
    std::cout << "- Dose per fraction: " << dose_per_fraction << " Gy" << std::endl;
    std::cout << "- Number of fractions: " << num_fractions << std::endl;
    std::cout << "- Time between fractions: " << time_between_fractions << " hours" << std::endl;

    EnhancedRadiationTherapyResult fractionated_result = model.predictFractionatedRadiationEffect(
        dose_per_fraction, num_fractions, time_between_fractions);

    // 15. Print fractionated results
    printRadiationTherapyResults(fractionated_result, "Fractionated Radiation Effects (5 x 2 Gy)");

    // 16. Print fraction-by-fraction survival
    std::cout << "Fraction-by-fraction survival:" << std::endl;
    std::cout << "-----------------------------" << std::endl;
    for (int i = 0; i < fractionated_result.fraction_survival.size(); ++i) {
        std::cout << "Fraction " << (i + 1) << ": " << fractionated_result.fraction_survival[i]
                  << std::endl;
    }
    std::cout << std::endl;

    // 17. Calculate BED and EQD2
    double bed = model.calculateBED(dose_per_fraction * num_fractions, num_fractions);
    double eqd2 = model.calculateEQD2(dose_per_fraction * num_fractions, num_fractions);

    std::cout << "Biological Effective Dose (BED): " << bed << " Gy" << std::endl;
    std::cout << "Equivalent Dose in 2 Gy fractions (EQD2): " << eqd2 << " Gy" << std::endl;
    std::cout << std::endl;

    // 18. Calculate TCP
    double tcp = model.calculateTCP(dose_per_fraction * num_fractions, num_fractions);
    std::cout << "Tumor Control Probability (TCP): " << tcp * 100.0 << "%" << std::endl;
    std::cout << std::endl;

    // 19. Compare treatment schedules
    std::cout << "Comparing different treatment schedules:" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    std::cout << std::setw(15) << "Schedule" << std::setw(10) << "BED (Gy)" << std::setw(10)
              << "EQD2 (Gy)" << std::setw(10) << "TCP (%)" << std::endl;
    std::cout << "----------------------------------" << std::endl;

    struct TreatmentSchedule {
        double dose_per_fraction;
        int num_fractions;
        std::string name;
    };

    std::vector<TreatmentSchedule> schedules = {{2.0, 5, "5 x 2 Gy"},
                                                {4.0, 5, "5 x 4 Gy"},
                                                {8.0, 5, "5 x 8 Gy"},
                                                {3.0, 10, "10 x 3 Gy"},
                                                {1.8, 30, "30 x 1.8 Gy"}};

    for (const auto& schedule : schedules) {
        double total_dose = schedule.dose_per_fraction * schedule.num_fractions;
        double bed = model.calculateBED(total_dose, schedule.num_fractions);
        double eqd2 = model.calculateEQD2(total_dose, schedule.num_fractions);
        double tcp = model.calculateTCP(total_dose, schedule.num_fractions);

        std::cout << std::setw(15) << schedule.name << std::setw(10) << std::fixed
                  << std::setprecision(1) << bed << std::setw(10) << std::fixed
                  << std::setprecision(1) << eqd2 << std::setw(10) << std::fixed
                  << std::setprecision(1) << tcp * 100.0 << std::endl;
    }
    std::cout << std::endl;

    // 20. Print time-dependent survival and damage
    std::cout << "Time-dependent survival after 2 Gy:" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    std::cout << std::setw(10) << "Time (h)" << std::setw(20) << "Survival Fraction" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    for (const auto& point : single_result.time_survival_curve) {
        std::cout << std::setw(10) << std::fixed << std::setprecision(1) << point.first
                  << std::setw(20) << std::fixed << std::setprecision(6) << point.second
                  << std::endl;
    }
    std::cout << std::endl;

    // 21. Normal tissue comparison
    BiologicalSystemExtended normal_system =
        CellCycleModel::createDefaultBiologicalSystem(TissueType::SOFT_TISSUE);
    normal_system.oxygen_tension = 21.0;  // 21% O2 (normoxic)

    // Set normal tissue cell cycle
    CellCycleDistribution normal_cycle_dist;
    normal_cycle_dist.g0_fraction = 0.8;  // Mostly quiescent
    normal_cycle_dist.g1_fraction = 0.1;
    normal_cycle_dist.s_fraction = 0.05;
    normal_cycle_dist.g2_fraction = 0.03;
    normal_cycle_dist.m_fraction = 0.02;
    normal_system.cycle_distribution = normal_cycle_dist;

    // Calculate therapeutic ratio
    double tumor_dose = 2.0;
    double normal_dose = 2.0 * 0.7;  // Assume 70% of tumor dose reaches normal tissue

    double therapeutic_ratio =
        model.calculateTherapeuticRatio(tumor_dose, normal_dose, tumor_system, normal_system);

    std::cout << "Therapeutic Ratio: " << therapeutic_ratio << std::endl;
    std::cout << std::endl;

    // 22. Find optimal dose
    double optimal_dose = model.optimizeRadiationDose(1.0, 4.0, 0.1, tumor_system, normal_system);

    std::cout << "Optimal Single Dose: " << optimal_dose << " Gy" << std::endl;
    std::cout << std::endl;

    // 23. Find optimal fractionation
    auto optimal_fractionation = model.optimizeFractionationSchedule(60.0,  // Total dose
                                                                     1.8,   // Min fraction size
                                                                     4.0,   // Max fraction size
                                                                     10,    // Min fractions
                                                                     30);   // Max fractions

    std::cout << "Optimal Fractionation Schedule:" << std::endl;
    std::cout << "- Dose per fraction: " << std::get<0>(optimal_fractionation) << " Gy"
              << std::endl;
    std::cout << "- Number of fractions: " << std::get<1>(optimal_fractionation) << std::endl;
    std::cout << "- Time between fractions: " << std::get<2>(optimal_fractionation) << " hours"
              << std::endl;
    std::cout << std::endl;

    std::cout << "Enhanced Healthcare Example Completed Successfully" << std::endl;
    return 0;
}
