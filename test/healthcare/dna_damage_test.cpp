#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "rad_ml/healthcare/cell_biology/cell_cycle_model.hpp"
#include "rad_ml/healthcare/cell_biology/dna_damage_model.hpp"
#include "rad_ml/healthcare/cell_biology/repair_kinetics.hpp"

using namespace rad_ml::healthcare::cell_biology;

// Test constants
const double EPSILON = 1e-6;

// Helper function to compare doubles with epsilon
bool areClose(double a, double b, double epsilon = EPSILON) { return std::fabs(a - b) < epsilon; }

// Test for DNA damage model
void testDNADamageModel()
{
    std::cout << "Testing DNADamageModel..." << std::endl;

    // Create a biological system
    BiologicalSystemExtended biosystem;
    biosystem.type = TissueType::TUMOR_RAPIDLY_DIVIDING;
    biosystem.water_content = 0.7;      // 70% water
    biosystem.cell_density = 1.0e6;     // 1 million cells per mm³
    biosystem.effective_barrier = 0.3;  // eV
    biosystem.repair_rate = 0.3;        // cellular repair capability
    biosystem.radiosensitivity = 1.2;   // radiation sensitivity factor
    biosystem.oxygen_tension = 5.0;     // 5% O₂ (hypoxic)

    // Create DNA damage model
    DNADamageModel damage_model(biosystem);

    // Set radiation parameters
    RadiationParams params;
    params.type = RadiationType::PHOTON;
    params.dose = 2.0;       // 2 Gy
    params.energy = 6.0;     // 6 MV
    params.let = 0.2;        // 0.2 keV/µm
    params.rbe = 1.0;        // Relative biological effectiveness
    params.oer = 1.0;        // Oxygen enhancement ratio
    params.dose_rate = 2.0;  // 2 Gy/min

    damage_model.setRadiationParams(params);

    // Set DNA repair kinetics
    DNARepairKinetics repair_kinetics;
    repair_kinetics.fast_repair_half_time = 0.5;  // 0.5 hours
    repair_kinetics.slow_repair_half_time = 8.0;  // 8 hours
    repair_kinetics.fast_repair_fraction = 0.7;   // 70% fast repair
    repair_kinetics.repair_fidelity_dsb = 0.95;   // 95% correct DSB repair
    repair_kinetics.repair_fidelity_ssb = 0.999;  // 99.9% correct SSB repair

    damage_model.setDNARepairKinetics(repair_kinetics);

    // Calculate DNA damage
    DNADamageProfile damage = damage_model.calculateDamage();

    std::cout << "  Initial damage:" << std::endl;
    std::cout << "    SSBs: " << damage.single_strand_breaks << std::endl;
    std::cout << "    DSBs: " << damage.double_strand_breaks << std::endl;
    std::cout << "    Base damages: " << damage.base_damages << std::endl;
    std::cout << "    Clustered damages: " << damage.clustered_damages << std::endl;
    std::cout << "    Complex DSBs: " << damage.complex_dsb << std::endl;
    std::cout << "    Total: " << damage.getTotalDamage() << std::endl;
    std::cout << "    Simple/Complex ratio: " << damage.getSimpleToComplexRatio() << std::endl;

    // Check that damage values are reasonable
    assert(damage.single_strand_breaks > 0);
    assert(damage.double_strand_breaks > 0);
    assert(damage.base_damages > 0);

    // SSBs should be more frequent than DSBs
    assert(damage.single_strand_breaks > damage.double_strand_breaks);

    // Calculate track structure damage
    DNADamageProfile track_damage = damage_model.calculateTrackStructureDamage(2.0, 0.2);

    std::cout << "  Track structure damage:" << std::endl;
    std::cout << "    SSBs: " << track_damage.single_strand_breaks << std::endl;
    std::cout << "    DSBs: " << track_damage.double_strand_breaks << std::endl;
    std::cout << "    Base damages: " << track_damage.base_damages << std::endl;
    std::cout << "    Clustered damages: " << track_damage.clustered_damages << std::endl;
    std::cout << "    Complex DSBs: " << track_damage.complex_dsb << std::endl;

    // Track structure should include some clustered damage
    assert(track_damage.clustered_damages > 0);
    assert(track_damage.complex_dsb > 0);

    // Time-evolving damage with repair
    double repair_time = 6.0;  // 6 hours
    DNADamageProfile residual_damage =
        damage_model.calculateTimeEvolvingDamage(damage, repair_time);

    std::cout << "  Residual damage after " << repair_time << " hours:" << std::endl;
    std::cout << "    SSBs: " << residual_damage.single_strand_breaks << std::endl;
    std::cout << "    DSBs: " << residual_damage.double_strand_breaks << std::endl;
    std::cout << "    Base damages: " << residual_damage.base_damages << std::endl;
    std::cout << "    Clustered damages: " << residual_damage.clustered_damages << std::endl;
    std::cout << "    Complex DSBs: " << residual_damage.complex_dsb << std::endl;
    std::cout << "    Total: " << residual_damage.getTotalDamage() << std::endl;

    // Residual damage should be less than initial damage
    assert(residual_damage.single_strand_breaks < damage.single_strand_breaks);
    assert(residual_damage.double_strand_breaks < damage.double_strand_breaks);
    assert(residual_damage.getTotalDamage() < damage.getTotalDamage());

    // Complex damage should be repaired more slowly
    double simple_repair_fraction =
        1.0 -
        static_cast<double>(residual_damage.single_strand_breaks) / damage.single_strand_breaks;
    double complex_repair_fraction =
        1.0 - static_cast<double>(residual_damage.complex_dsb) / damage.complex_dsb;

    std::cout << "  Simple damage repair fraction: " << simple_repair_fraction << std::endl;
    std::cout << "  Complex damage repair fraction: " << complex_repair_fraction << std::endl;

    assert(simple_repair_fraction > complex_repair_fraction);

    // Calculate survival
    double survival = damage_model.calculateSurvivalFromDamage(damage);

    std::cout << "  Survival fraction: " << survival << std::endl;

    // Survival should be between 0 and 1
    assert(survival >= 0.0 && survival <= 1.0);

    // Calculate survival after repair
    double repaired_survival = damage_model.calculateSurvivalFromDamage(residual_damage);

    std::cout << "  Survival fraction after repair: " << repaired_survival << std::endl;

    // Survival should increase after repair
    assert(repaired_survival > survival);

    // Test different radiation types
    RadiationParams proton_params = params;
    proton_params.type = RadiationType::PROTON;
    proton_params.let = 5.0;  // 5 keV/µm
    proton_params.rbe = 1.1;  // RBE for protons

    damage_model.setRadiationParams(proton_params);
    DNADamageProfile proton_damage = damage_model.calculateDamage();

    std::cout << "  Proton damage:" << std::endl;
    std::cout << "    DSBs: " << proton_damage.double_strand_breaks << std::endl;
    std::cout << "    Complex DSBs: " << proton_damage.complex_dsb << std::endl;
    std::cout << "    Simple/Complex ratio: " << proton_damage.getSimpleToComplexRatio()
              << std::endl;

    // Higher LET should produce more complex damage
    assert(proton_damage.complex_dsb > damage.complex_dsb);
    assert(proton_damage.getSimpleToComplexRatio() < damage.getSimpleToComplexRatio());

    std::cout << "DNADamageModel tests passed!" << std::endl << std::endl;
}

// Test for repair kinetics model
void testRepairKineticsModel()
{
    std::cout << "Testing RepairKineticsModel..." << std::endl;

    // Create repair pathways configuration
    RepairKineticsConfig config;

    // Configure NHEJ pathway (for DSB repair)
    RepairPathwayKinetics nhej_kinetics;
    nhej_kinetics.half_time = 2.0;            // 2 hours
    nhej_kinetics.fidelity = 0.95;            // 95% correct repair
    nhej_kinetics.saturation_threshold = 50;  // Saturates at 50 DSBs

    // Configure BER pathway (for SSB and base damage repair)
    RepairPathwayKinetics ber_kinetics;
    ber_kinetics.half_time = 0.5;             // 0.5 hours
    ber_kinetics.fidelity = 0.999;            // 99.9% correct repair
    ber_kinetics.saturation_threshold = 200;  // Saturates at 200 damages

    // Add pathways to config
    config.pathway_kinetics[RepairPathway::NHEJ] = nhej_kinetics;
    config.pathway_kinetics[RepairPathway::BER] = ber_kinetics;

    // Create repair kinetics model
    RepairKineticsModel repair_model(config);

    // Create initial damage profile
    DNADamageProfile initial_damage;
    initial_damage.single_strand_breaks = 1000;
    initial_damage.double_strand_breaks = 40;
    initial_damage.base_damages = 500;
    initial_damage.clustered_damages = 20;
    initial_damage.complex_dsb = 10;

    // Calculate repair probability for different damage types
    double ssb_repair_1h = repair_model.calculateRepairProbability(DNADamageType::SSB, 1.0);
    double dsb_repair_1h = repair_model.calculateRepairProbability(DNADamageType::DSB, 1.0);

    std::cout << "  Repair probability after 1 hour:" << std::endl;
    std::cout << "    SSB: " << ssb_repair_1h << std::endl;
    std::cout << "    DSB: " << dsb_repair_1h << std::endl;

    // SSB repair should be faster than DSB repair
    assert(ssb_repair_1h > dsb_repair_1h);

    // Both probabilities should be between 0 and 1
    assert(ssb_repair_1h >= 0.0 && ssb_repair_1h <= 1.0);
    assert(dsb_repair_1h >= 0.0 && dsb_repair_1h <= 1.0);

    // Calculate repair over time
    std::vector<double> times = {0.5, 1.0, 2.0, 4.0, 8.0, 24.0};

    std::cout << "  Time-dependent repair:" << std::endl;
    std::cout << "    Time (h) | SSB Repair | DSB Repair" << std::endl;
    std::cout << "    --------|-----------|-----------" << std::endl;

    for (double time : times) {
        double ssb_repair = repair_model.calculateRepairProbability(DNADamageType::SSB, time);
        double dsb_repair = repair_model.calculateRepairProbability(DNADamageType::DSB, time);
        std::cout << "    " << time << "      | " << ssb_repair << "    | " << dsb_repair
                  << std::endl;

        // Repair probability should increase with time
        if (time > 1.0) {
            assert(ssb_repair > ssb_repair_1h);
            assert(dsb_repair > dsb_repair_1h);
        }
    }

    // Calculate time-evolving damage
    DNADamageProfile damage_2h = repair_model.calculateTimeEvolvingDamage(initial_damage, 2.0);
    DNADamageProfile damage_8h = repair_model.calculateTimeEvolvingDamage(initial_damage, 8.0);
    DNADamageProfile damage_24h = repair_model.calculateTimeEvolvingDamage(initial_damage, 24.0);

    std::cout << "  Residual damage:" << std::endl;
    std::cout << "    Time | SSBs  | DSBs | Complex DSBs" << std::endl;
    std::cout << "    -----|-------|------|------------" << std::endl;
    std::cout << "    0h   | " << initial_damage.single_strand_breaks << " | "
              << initial_damage.double_strand_breaks << "   | " << initial_damage.complex_dsb
              << std::endl;
    std::cout << "    2h   | " << damage_2h.single_strand_breaks << "  | "
              << damage_2h.double_strand_breaks << "   | " << damage_2h.complex_dsb << std::endl;
    std::cout << "    8h   | " << damage_8h.single_strand_breaks << "   | "
              << damage_8h.double_strand_breaks << "   | " << damage_8h.complex_dsb << std::endl;
    std::cout << "    24h  | " << damage_24h.single_strand_breaks << "    | "
              << damage_24h.double_strand_breaks << "    | " << damage_24h.complex_dsb << std::endl;

    // Damage should decrease over time
    assert(damage_2h.single_strand_breaks < initial_damage.single_strand_breaks);
    assert(damage_8h.single_strand_breaks < damage_2h.single_strand_breaks);
    assert(damage_24h.single_strand_breaks < damage_8h.single_strand_breaks);

    assert(damage_2h.double_strand_breaks < initial_damage.double_strand_breaks);
    assert(damage_8h.double_strand_breaks < damage_2h.double_strand_breaks);
    assert(damage_24h.double_strand_breaks < damage_8h.double_strand_breaks);

    // Test repair saturation
    double saturation_factor = repair_model.calculateRepairSaturationFactor(initial_damage);
    std::cout << "  Repair saturation factor: " << saturation_factor << std::endl;

    // Saturation factor should be <= 1.0
    assert(saturation_factor <= 1.0);
    assert(saturation_factor > 0.0);

    // Test survival calculation
    double survival_0h = repair_model.calculateSurvivalFromRepair(initial_damage, 0.0);
    double survival_2h = repair_model.calculateSurvivalFromRepair(initial_damage, 2.0);
    double survival_8h = repair_model.calculateSurvivalFromRepair(initial_damage, 8.0);
    double survival_24h = repair_model.calculateSurvivalFromRepair(initial_damage, 24.0);

    std::cout << "  Survival probability:" << std::endl;
    std::cout << "    0h:  " << survival_0h << std::endl;
    std::cout << "    2h:  " << survival_2h << std::endl;
    std::cout << "    8h:  " << survival_8h << std::endl;
    std::cout << "    24h: " << survival_24h << std::endl;

    // Survival should increase over time due to repair
    assert(survival_2h > survival_0h);
    assert(survival_8h > survival_2h);
    assert(survival_24h > survival_8h);

    // Survival should be between 0 and 1
    assert(survival_0h >= 0.0 && survival_0h <= 1.0);
    assert(survival_24h >= 0.0 && survival_24h <= 1.0);

    std::cout << "RepairKineticsModel tests passed!" << std::endl << std::endl;
}

// Test for the calculateTrackStructureDamage function
void testCalculateTrackStructureDamage()
{
    std::cout << "Testing calculateTrackStructureDamage function..." << std::endl;

    // Create a biological system
    BiologicalSystemExtended biosystem;
    biosystem.type = TissueType::TUMOR_RAPIDLY_DIVIDING;
    biosystem.water_content = 0.7;
    biosystem.oxygen_tension = 5.0;  // Hypoxic

    // Test different radiation types with varying LET
    struct TestCase {
        double dose;
        double let;
        std::string description;
    };

    std::vector<TestCase> test_cases = {{2.0, 0.2, "Photons (low LET)"},
                                        {2.0, 5.0, "Protons (medium LET)"},
                                        {2.0, 80.0, "Carbon ions (high LET)"}};

    for (const auto& test : test_cases) {
        DNADamageProfile damage = calculateTrackStructureDamage(test.dose, test.let, biosystem);

        std::cout << "  " << test.description << " (LET = " << test.let << " keV/µm):" << std::endl;
        std::cout << "    SSBs: " << damage.single_strand_breaks << std::endl;
        std::cout << "    DSBs: " << damage.double_strand_breaks << std::endl;
        std::cout << "    Clustered damages: " << damage.clustered_damages << std::endl;
        std::cout << "    Complex DSBs: " << damage.complex_dsb << std::endl;
        std::cout << "    Simple/Complex ratio: " << damage.getSimpleToComplexRatio() << std::endl;

        // Check that damage scales reasonably with LET
        if (test.let > 0.5) {
            // Higher LET should produce more clustered and complex damage
            assert(damage.clustered_damages > damage.single_strand_breaks / 10);
            assert(damage.complex_dsb > damage.double_strand_breaks / 5);
        }

        // Total damage should be reasonable
        assert(damage.getTotalDamage() > 0);
    }

    // Same dose but different LET
    DNADamageProfile low_let_damage = calculateTrackStructureDamage(2.0, 0.2, biosystem);
    DNADamageProfile high_let_damage = calculateTrackStructureDamage(2.0, 80.0, biosystem);

    // Higher LET should produce more complex damage
    assert(high_let_damage.complex_dsb > low_let_damage.complex_dsb);
    assert(high_let_damage.getSimpleToComplexRatio() < low_let_damage.getSimpleToComplexRatio());

    // Different doses at same LET
    DNADamageProfile low_dose_damage = calculateTrackStructureDamage(1.0, 0.2, biosystem);
    DNADamageProfile high_dose_damage = calculateTrackStructureDamage(4.0, 0.2, biosystem);

    // Damage should scale with dose
    assert(high_dose_damage.single_strand_breaks > low_dose_damage.single_strand_breaks);
    assert(high_dose_damage.double_strand_breaks > low_dose_damage.double_strand_breaks);

    std::cout << "calculateTrackStructureDamage tests passed!" << std::endl << std::endl;
}

// Test for calculating time-evolving damage
void testTimeEvolvingDamage()
{
    std::cout << "Testing calculateTimeEvolvingDamage function..." << std::endl;

    // Create initial damage profile
    DNADamageProfile initial_damage;
    initial_damage.single_strand_breaks = 1000;
    initial_damage.double_strand_breaks = 40;
    initial_damage.base_damages = 500;
    initial_damage.clustered_damages = 20;
    initial_damage.complex_dsb = 10;

    // Create repair kinetics
    DNARepairKinetics repair_kinetics;
    repair_kinetics.fast_repair_half_time = 0.5;  // 0.5 hours
    repair_kinetics.slow_repair_half_time = 8.0;  // 8 hours
    repair_kinetics.fast_repair_fraction = 0.7;   // 70% fast repair
    repair_kinetics.repair_fidelity_dsb = 0.95;   // 95% correct DSB repair
    repair_kinetics.repair_fidelity_ssb = 0.999;  // 99.9% correct SSB repair

    // Test different time points
    std::vector<double> time_points = {0.0, 0.5, 1.0, 2.0, 4.0, 8.0, 12.0, 24.0};

    std::cout << "  Time-dependent damage:" << std::endl;
    std::cout << "    Time (h) | SSBs | DSBs | Complex DSBs" << std::endl;
    std::cout << "    ---------|------|------|------------" << std::endl;

    DNADamageProfile prev_damage = initial_damage;

    for (double time : time_points) {
        DNADamageProfile damage =
            calculateTimeEvolvingDamage(initial_damage, repair_kinetics, time);

        std::cout << "    " << time << "       | " << damage.single_strand_breaks << "  | "
                  << damage.double_strand_breaks << "   | " << damage.complex_dsb << std::endl;

        // At time 0, damage should equal initial damage
        if (areClose(time, 0.0)) {
            assert(damage.single_strand_breaks == initial_damage.single_strand_breaks);
            assert(damage.double_strand_breaks == initial_damage.double_strand_breaks);
        }
        else {
            // Damage should decrease over time
            assert(damage.single_strand_breaks <= prev_damage.single_strand_breaks);
            assert(damage.double_strand_breaks <= prev_damage.double_strand_breaks);

            // SSBs should be repaired faster than DSBs
            if (time <= 4.0) {
                double ssb_repair_fraction =
                    1.0 - static_cast<double>(damage.single_strand_breaks) /
                              initial_damage.single_strand_breaks;
                double dsb_repair_fraction =
                    1.0 - static_cast<double>(damage.double_strand_breaks) /
                              initial_damage.double_strand_breaks;

                assert(ssb_repair_fraction >= dsb_repair_fraction);
            }
        }

        prev_damage = damage;
    }

    // After a long time, most damage should be repaired
    DNADamageProfile late_damage =
        calculateTimeEvolvingDamage(initial_damage, repair_kinetics, 72.0);

    double ssb_repair_fraction = 1.0 - static_cast<double>(late_damage.single_strand_breaks) /
                                           initial_damage.single_strand_breaks;
    double dsb_repair_fraction = 1.0 - static_cast<double>(late_damage.double_strand_breaks) /
                                           initial_damage.double_strand_breaks;

    std::cout << "  After 72 hours:" << std::endl;
    std::cout << "    SSB repair fraction: " << ssb_repair_fraction << std::endl;
    std::cout << "    DSB repair fraction: " << dsb_repair_fraction << std::endl;

    assert(ssb_repair_fraction > 0.99);  // >99% of SSBs repaired
    assert(dsb_repair_fraction > 0.9);   // >90% of DSBs repaired

    std::cout << "calculateTimeEvolvingDamage tests passed!" << std::endl << std::endl;
}

// Main test function
int main()
{
    std::cout << "Running DNA Damage Model Tests" << std::endl;
    std::cout << "=============================" << std::endl << std::endl;

    // Run tests
    testDNADamageModel();
    testRepairKineticsModel();
    testCalculateTrackStructureDamage();
    testTimeEvolvingDamage();

    std::cout << "All DNA damage model tests passed successfully!" << std::endl;
    return 0;
}
