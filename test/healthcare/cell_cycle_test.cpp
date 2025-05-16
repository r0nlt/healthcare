#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "rad_ml/healthcare/cell_biology/cell_cycle_model.hpp"

using namespace rad_ml::healthcare::cell_biology;

// Test constants
const double EPSILON = 1e-6;

// Helper function to compare doubles with epsilon
bool areClose(double a, double b, double epsilon = EPSILON) { return std::fabs(a - b) < epsilon; }

// Test for CellCycleModel
void testCellCycleModel()
{
    std::cout << "Testing CellCycleModel..." << std::endl;

    // Create a biological system
    BiologicalSystemExtended biosystem =
        CellCycleModel::createDefaultBiologicalSystem(TissueType::TUMOR_RAPIDLY_DIVIDING);

    // Create cell cycle model
    CellCycleModel cell_model(biosystem);

    // Test cell cycle distribution
    CellCycleDistribution distribution;
    distribution.g0_fraction = 0.1;  // 10% in G0
    distribution.g1_fraction = 0.3;  // 30% in G1
    distribution.s_fraction = 0.4;   // 40% in S
    distribution.g2_fraction = 0.1;  // 10% in G2
    distribution.m_fraction = 0.1;   // 10% in M

    // Verify the distribution is valid
    assert(distribution.isValid());

    // Set the distribution
    cell_model.setCellCycleDistribution(distribution);

    // Test phase sensitivity
    PhaseSensitivity g1_sensitivity;
    g1_sensitivity.alpha = 0.2;
    g1_sensitivity.beta = 0.02;
    cell_model.setPhaseSensitivity(CellCyclePhase::G1, g1_sensitivity);

    PhaseSensitivity s_sensitivity;
    s_sensitivity.alpha = 0.1;  // S phase is radioresistant
    s_sensitivity.beta = 0.01;
    cell_model.setPhaseSensitivity(CellCyclePhase::S, s_sensitivity);

    PhaseSensitivity g2m_sensitivity;
    g2m_sensitivity.alpha = 0.4;  // G2/M is radiosensitive
    g2m_sensitivity.beta = 0.04;
    cell_model.setPhaseSensitivity(CellCyclePhase::G2, g2m_sensitivity);
    cell_model.setPhaseSensitivity(CellCyclePhase::M, g2m_sensitivity);

    // Test survival calculation
    double dose = 2.0;  // 2 Gy
    double survival = cell_model.calculateSurvivalFraction(dose);

    std::cout << "  Survival fraction at " << dose << " Gy: " << survival << std::endl;

    // Survival should be between 0 and 1
    assert(survival > 0.0 && survival < 1.0);

    // Test phase-specific survival
    double g1_survival = cell_model.calculatePhaseSurvivalFraction(CellCyclePhase::G1, dose);
    double s_survival = cell_model.calculatePhaseSurvivalFraction(CellCyclePhase::S, dose);
    double g2_survival = cell_model.calculatePhaseSurvivalFraction(CellCyclePhase::G2, dose);

    std::cout << "  G1 phase survival: " << g1_survival << std::endl;
    std::cout << "  S phase survival:  " << s_survival << std::endl;
    std::cout << "  G2 phase survival: " << g2_survival << std::endl;

    // S phase should be more resistant than G1, and G2 should be more sensitive
    assert(s_survival > g1_survival);
    assert(g1_survival > g2_survival);

    // Test cell cycle redistribution
    CellCycleDistribution post_irradiation =
        cell_model.calculateCellCycleRedistribution(dose, 24.0);  // 24 hours after irradiation

    std::cout << "  Post-irradiation cell cycle distribution:" << std::endl;
    std::cout << "    G0: " << post_irradiation.g0_fraction << std::endl;
    std::cout << "    G1: " << post_irradiation.g1_fraction << std::endl;
    std::cout << "    S:  " << post_irradiation.s_fraction << std::endl;
    std::cout << "    G2: " << post_irradiation.g2_fraction << std::endl;
    std::cout << "    M:  " << post_irradiation.m_fraction << std::endl;

    // Verify that distribution is still valid
    assert(post_irradiation.isValid());

    // Test fractionated irradiation
    double dose_per_fraction = 2.0;
    int num_fractions = 5;
    double time_between_fractions = 24.0;  // 24 hours

    double fractionated_survival = cell_model.calculateFractionatedSurvivalFraction(
        dose_per_fraction, num_fractions, time_between_fractions);

    std::cout << "  Fractionated survival (" << num_fractions << " x " << dose_per_fraction
              << " Gy): " << fractionated_survival << std::endl;

    // Fractionated survival should be higher than single-dose equivalent
    double equivalent_single_dose = dose_per_fraction * num_fractions;
    double single_dose_survival = cell_model.calculateSurvivalFraction(equivalent_single_dose);

    std::cout << "  Equivalent single-dose survival (" << equivalent_single_dose
              << " Gy): " << single_dose_survival << std::endl;

    assert(fractionated_survival > single_dose_survival);

    // Test oxygen effect
    cell_model.setOxygenTension(21.0);  // Normoxic - 21% oxygen
    double normoxic_survival = cell_model.calculateSurvivalFraction(dose);

    cell_model.setOxygenTension(5.0);  // Hypoxic - 5% oxygen
    double hypoxic_survival = cell_model.calculateSurvivalFraction(dose);

    std::cout << "  Normoxic survival: " << normoxic_survival << std::endl;
    std::cout << "  Hypoxic survival:  " << hypoxic_survival << std::endl;

    // Hypoxic cells should be more resistant
    assert(hypoxic_survival > normoxic_survival);

    // Test fractionation schedule optimization
    auto optimal_schedule = cell_model.optimizeFractionationSchedule(60.0,   // Total dose
                                                                     1.8,    // Min fraction size
                                                                     3.0,    // Max fraction size
                                                                     0.01);  // Target survival

    double optimal_fraction_size = std::get<0>(optimal_schedule);
    int optimal_num_fractions = std::get<1>(optimal_schedule);
    double optimal_timing = std::get<2>(optimal_schedule);

    std::cout << "  Optimal fractionation:" << std::endl;
    std::cout << "    Fraction size: " << optimal_fraction_size << " Gy" << std::endl;
    std::cout << "    Number of fractions: " << optimal_num_fractions << std::endl;
    std::cout << "    Time between fractions: " << optimal_timing << " hours" << std::endl;

    // Check that the optimal schedule is within constraints
    assert(optimal_fraction_size >= 1.8 && optimal_fraction_size <= 3.0);
    assert(optimal_num_fractions > 0);
    assert(optimal_timing > 0.0);
    assert(areClose(optimal_fraction_size * optimal_num_fractions, 60.0, 0.1));

    std::cout << "CellCycleModel tests passed!" << std::endl << std::endl;
}

// Test different tissue types
void testTissueTypes()
{
    std::cout << "Testing different tissue types..." << std::endl;

    // Array of tissue types to test
    std::vector<TissueType> tissue_types = {TissueType::SOFT_TISSUE,
                                            TissueType::BONE,
                                            TissueType::EPITHELIAL,
                                            TissueType::STEM_CELL,
                                            TissueType::TUMOR_RAPIDLY_DIVIDING,
                                            TissueType::TUMOR_HYPOXIC};

    // Test parameters for each tissue type
    for (auto tissue_type : tissue_types) {
        // Create default biological system
        BiologicalSystemExtended biosystem =
            CellCycleModel::createDefaultBiologicalSystem(tissue_type);

        // Create cell cycle model
        CellCycleModel cell_model(biosystem);

        // Get default cell cycle distribution
        CellCycleDistribution distribution =
            CellCycleModel::getDefaultCellCycleDistribution(tissue_type);

        // Check that the distribution is valid
        assert(distribution.isValid());

        // Get type name for display
        std::string type_name;
        switch (tissue_type) {
            case TissueType::SOFT_TISSUE:
                type_name = "Soft Tissue";
                break;
            case TissueType::BONE:
                type_name = "Bone";
                break;
            case TissueType::EPITHELIAL:
                type_name = "Epithelial";
                break;
            case TissueType::STEM_CELL:
                type_name = "Stem Cell";
                break;
            case TissueType::TUMOR_RAPIDLY_DIVIDING:
                type_name = "Rapidly Dividing Tumor";
                break;
            case TissueType::TUMOR_HYPOXIC:
                type_name = "Hypoxic Tumor";
                break;
            default:
                type_name = "Unknown";
                break;
        }

        std::cout << "  Testing " << type_name << ":" << std::endl;
        std::cout << "    G0 fraction: " << distribution.g0_fraction << std::endl;
        std::cout << "    G1 fraction: " << distribution.g1_fraction << std::endl;
        std::cout << "    S fraction:  " << distribution.s_fraction << std::endl;
        std::cout << "    G2 fraction: " << distribution.g2_fraction << std::endl;
        std::cout << "    M fraction:  " << distribution.m_fraction << std::endl;

        // Calculate survival at 2 Gy
        double survival_2gy = cell_model.calculateSurvivalFraction(2.0);
        std::cout << "    Survival at 2 Gy: " << survival_2gy << std::endl;

        // Get radiosensitivity parameters
        auto alphabeta = cell_model.calculateCellCycleWeightedRadiosensitivity();
        std::cout << "    Alpha: " << alphabeta.first << std::endl;
        std::cout << "    Beta:  " << alphabeta.second << std::endl;
        std::cout << "    Alpha/Beta ratio: " << alphabeta.first / alphabeta.second << std::endl;

        // Check that alpha/beta is positive and reasonable
        assert(alphabeta.first > 0.0);
        assert(alphabeta.second > 0.0);
        assert(alphabeta.first / alphabeta.second > 0.0);

        std::cout << std::endl;
    }

    std::cout << "Tissue type tests passed!" << std::endl << std::endl;
}

// Test for cell cycle-weighted radiosensitivity function
void testCellCycleWeightedRadiosensitivity()
{
    std::cout << "Testing calculateCellCycleWeightedRadiosensitivity function..." << std::endl;

    // Create a biological system
    BiologicalSystemExtended biosystem;
    biosystem.type = TissueType::TUMOR_RAPIDLY_DIVIDING;

    // Set cell cycle distribution
    CellCycleDistribution distribution;
    distribution.g0_fraction = 0.1;
    distribution.g1_fraction = 0.3;
    distribution.s_fraction = 0.4;
    distribution.g2_fraction = 0.1;
    distribution.m_fraction = 0.1;
    biosystem.cycle_distribution = distribution;

    // Set phase sensitivity
    std::map<CellCyclePhase, PhaseSensitivity> phase_sensitivity;

    PhaseSensitivity g0_sensitivity;
    g0_sensitivity.alpha = 0.1;
    g0_sensitivity.beta = 0.01;
    phase_sensitivity[CellCyclePhase::G0] = g0_sensitivity;

    PhaseSensitivity g1_sensitivity;
    g1_sensitivity.alpha = 0.2;
    g1_sensitivity.beta = 0.02;
    phase_sensitivity[CellCyclePhase::G1] = g1_sensitivity;

    PhaseSensitivity s_sensitivity;
    s_sensitivity.alpha = 0.1;
    s_sensitivity.beta = 0.01;
    phase_sensitivity[CellCyclePhase::S] = s_sensitivity;

    PhaseSensitivity g2_sensitivity;
    g2_sensitivity.alpha = 0.3;
    g2_sensitivity.beta = 0.03;
    phase_sensitivity[CellCyclePhase::G2] = g2_sensitivity;

    PhaseSensitivity m_sensitivity;
    m_sensitivity.alpha = 0.4;
    m_sensitivity.beta = 0.04;
    phase_sensitivity[CellCyclePhase::M] = m_sensitivity;

    biosystem.phase_sensitivity = phase_sensitivity;

    // Calculate weighted radiosensitivity
    auto weighted_params = calculateCellCycleWeightedRadiosensitivity(biosystem);

    std::cout << "  Weighted alpha: " << weighted_params.first << std::endl;
    std::cout << "  Weighted beta:  " << weighted_params.second << std::endl;

    // Calculate expected values manually for verification
    double expected_alpha = 0.1 * g0_sensitivity.alpha + 0.3 * g1_sensitivity.alpha +
                            0.4 * s_sensitivity.alpha + 0.1 * g2_sensitivity.alpha +
                            0.1 * m_sensitivity.alpha;

    double expected_beta = 0.1 * g0_sensitivity.beta + 0.3 * g1_sensitivity.beta +
                           0.4 * s_sensitivity.beta + 0.1 * g2_sensitivity.beta +
                           0.1 * m_sensitivity.beta;

    std::cout << "  Expected alpha: " << expected_alpha << std::endl;
    std::cout << "  Expected beta:  " << expected_beta << std::endl;

    // Check that the calculated values match the expected values
    assert(areClose(weighted_params.first, expected_alpha));
    assert(areClose(weighted_params.second, expected_beta));

    std::cout << "calculateCellCycleWeightedRadiosensitivity test passed!" << std::endl
              << std::endl;
}

// Main test function
int main()
{
    std::cout << "Running Cell Cycle Model Tests" << std::endl;
    std::cout << "=============================" << std::endl << std::endl;

    // Run tests
    testCellCycleModel();
    testTissueTypes();
    testCellCycleWeightedRadiosensitivity();

    std::cout << "All cell cycle tests passed successfully!" << std::endl;
    return 0;
}
