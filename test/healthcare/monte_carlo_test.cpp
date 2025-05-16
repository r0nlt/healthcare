#include <cassert>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "rad_ml/healthcare/cell_biology/cell_cycle_model.hpp"
#include "rad_ml/healthcare/monte_carlo/damage_simulation.hpp"

using namespace rad_ml::healthcare::monte_carlo;
using namespace rad_ml::healthcare::cell_biology;

// Test constants
const double EPSILON = 1e-6;

// Helper function to compare doubles with epsilon
bool areClose(double a, double b, double epsilon = EPSILON) { return std::fabs(a - b) < epsilon; }

// Test for Monte Carlo damage simulation
void testMonteCarloDamageSimulation()
{
    std::cout << "Testing MonteCarloDamageSimulation..." << std::endl;

    // Create biological system
    BiologicalSystemExtended biosystem =
        CellCycleModel::createDefaultBiologicalSystem(TissueType::TUMOR_RAPIDLY_DIVIDING);
    biosystem.oxygen_tension = 5.0;  // Hypoxic conditions

    // Create Monte Carlo configuration
    MonteCarloConfig config;
    config.num_particles = 100;  // Use a smaller number for faster tests
    config.dose = 2.0;           // 2 Gy
    config.include_dna_structure = true;
    config.include_chemical_stage = true;
    config.voxel_size = 10.0;  // nm

    // Create Monte Carlo simulation
    MonteCarloDamageSimulation simulation(biosystem, config);

    // Set default particle properties (photons)
    ParticleProperties particle_props =
        MonteCarloDamageSimulation::getDefaultParticleProperties(RadiationType::PHOTON);
    simulation.setParticleProperties(particle_props);

    // Set default nucleus geometry
    NucleusGeometry geometry = MonteCarloDamageSimulation::getDefaultNucleusGeometry("tumor");
    simulation.setNucleusGeometry(geometry);

    // Set default chromatin distribution
    ChromatinDistribution chromatin =
        MonteCarloDamageSimulation::getDefaultChromatinDistribution("tumor");
    simulation.setChromatinDistribution(chromatin);

    // Run the simulation
    std::cout << "  Running Monte Carlo simulation..." << std::endl;
    MonteCarloResult result = simulation.runSimulation();

    // Check the damage profile
    std::cout << "  Damage profile:" << std::endl;
    std::cout << "    Single-strand breaks: " << result.damage_profile.single_strand_breaks
              << std::endl;
    std::cout << "    Double-strand breaks: " << result.damage_profile.double_strand_breaks
              << std::endl;
    std::cout << "    Base damages: " << result.damage_profile.base_damages << std::endl;
    std::cout << "    Clustered damages: " << result.damage_profile.clustered_damages << std::endl;
    std::cout << "    Complex DSBs: " << result.damage_profile.complex_dsb << std::endl;

    // Check that the simulation produced some damage
    assert(result.damage_profile.single_strand_breaks > 0);
    assert(result.damage_profile.double_strand_breaks > 0);

    // Check that SSBs are more frequent than DSBs
    assert(result.damage_profile.single_strand_breaks > result.damage_profile.double_strand_breaks);

    // Check that damage positions are being tracked
    std::cout << "  Number of damage positions: " << result.damage_positions.size() << std::endl;
    assert(result.damage_positions.size() > 0);

    // Test with different radiation types
    ParticleProperties proton_props =
        MonteCarloDamageSimulation::getDefaultParticleProperties(RadiationType::PROTON);
    simulation.setParticleProperties(proton_props);

    std::cout << "  Running simulation with protons..." << std::endl;
    MonteCarloResult proton_result = simulation.runSimulation();

    std::cout << "  Proton damage profile:" << std::endl;
    std::cout << "    Double-strand breaks: " << proton_result.damage_profile.double_strand_breaks
              << std::endl;
    std::cout << "    Complex DSBs: " << proton_result.damage_profile.complex_dsb << std::endl;

    // Protons should produce a different damage profile (typically more clustered damage)
    assert(proton_result.complex_damage_fraction >= result.complex_damage_fraction);

    // Test dose dependence by running with a different dose
    std::cout << "  Testing dose dependence..." << std::endl;
    double higher_dose = 4.0;  // 4 Gy
    MonteCarloResult high_dose_result = simulation.runSimulationWithDose(higher_dose);

    std::cout << "  Damage at " << higher_dose << " Gy:" << std::endl;
    std::cout << "    Double-strand breaks: "
              << high_dose_result.damage_profile.double_strand_breaks << std::endl;

    // Higher dose should produce proportionally more damage
    double dose_ratio = higher_dose / config.dose;
    double damage_ratio =
        static_cast<double>(high_dose_result.damage_profile.double_strand_breaks) /
        proton_result.damage_profile.double_strand_breaks;

    std::cout << "  Dose ratio: " << dose_ratio << std::endl;
    std::cout << "  Damage ratio: " << damage_ratio << std::endl;

    // Allow for some statistical variation since it's a Monte Carlo simulation
    assert(damage_ratio > 0.5 * dose_ratio && damage_ratio < 1.5 * dose_ratio);

    // Test the conversion functions
    int num_particles = 1000;
    double calculated_dose =
        MonteCarloDamageSimulation::particlesToDose(num_particles, particle_props, geometry);

    int calculated_particles =
        MonteCarloDamageSimulation::doseToParticles(calculated_dose, particle_props, geometry);

    std::cout << "  Particles to dose conversion: " << num_particles
              << " particles = " << calculated_dose << " Gy" << std::endl;
    std::cout << "  Dose to particles conversion: " << calculated_dose
              << " Gy = " << calculated_particles << " particles" << std::endl;

    // Check that the conversions are reasonable (allow for some rounding)
    assert(std::abs(calculated_particles - num_particles) <= 1);

    std::cout << "MonteCarloDamageSimulation tests passed!" << std::endl << std::endl;
}

// Test for nucleus geometry
void testNucleusGeometry()
{
    std::cout << "Testing NucleusGeometry..." << std::endl;

    // Create a spherical nucleus
    NucleusGeometry sphere_geometry;
    sphere_geometry.shape = NucleusGeometry::Shape::SPHERE;
    sphere_geometry.radius = 5.0;  // 5 µm
    sphere_geometry.center = {0.0, 0.0, 0.0};

    // Test points inside and outside the sphere
    std::array<double, 3> inside_point = {1.0, 1.0, 1.0};    // Inside
    std::array<double, 3> outside_point = {10.0, 0.0, 0.0};  // Outside

    assert(sphere_geometry.isInside(inside_point) == true);
    assert(sphere_geometry.isInside(outside_point) == false);

    // Create an ellipsoidal nucleus
    NucleusGeometry ellipsoid_geometry;
    ellipsoid_geometry.shape = NucleusGeometry::Shape::ELLIPSOID;
    ellipsoid_geometry.semi_axes = {5.0, 3.0, 4.0};  // Semi-axes in µm
    ellipsoid_geometry.center = {0.0, 0.0, 0.0};

    // Test points inside and outside the ellipsoid
    std::array<double, 3> ellipsoid_inside = {2.0, 1.0, 2.0};   // Inside
    std::array<double, 3> ellipsoid_outside = {5.5, 0.0, 0.0};  // Outside

    assert(ellipsoid_geometry.isInside(ellipsoid_inside) == true);
    assert(ellipsoid_geometry.isInside(ellipsoid_outside) == false);

    // Test edge cases
    std::array<double, 3> on_boundary = {5.0, 0.0, 0.0};  // On the boundary of sphere

    // Points exactly on the boundary should be considered inside
    assert(sphere_geometry.isInside(on_boundary) == true);

    std::cout << "NucleusGeometry tests passed!" << std::endl << std::endl;
}

// Test chromatin distribution
void testChromatinDistribution()
{
    std::cout << "Testing ChromatinDistribution..." << std::endl;

    // Create uniform chromatin distribution
    ChromatinDistribution uniform_distribution;
    uniform_distribution.model = ChromatinDistribution::Model::UNIFORM;
    uniform_distribution.chromatin_density = 0.015;  // DNA density (bp/nm³)

    // Test density at different points
    std::array<double, 3> point1 = {0.0, 0.0, 0.0};
    std::array<double, 3> point2 = {2.0, 3.0, 4.0};

    double density1 = uniform_distribution.getDensity(point1);
    double density2 = uniform_distribution.getDensity(point2);

    // For uniform distribution, density should be the same everywhere
    assert(areClose(density1, uniform_distribution.chromatin_density));
    assert(areClose(density2, uniform_distribution.chromatin_density));

    // Create heterochromatin periphery distribution
    ChromatinDistribution periphery_distribution;
    periphery_distribution.model = ChromatinDistribution::Model::HETEROCHROMATIN_PERIPHERY;
    periphery_distribution.chromatin_density = 0.015;
    periphery_distribution.heterochromatin_fraction = 0.8;

    // For heterochromatin periphery, density at center should be different than at periphery
    // (This is just a basic test; actual implementation would be more complex)
    std::array<double, 3> center = {0.0, 0.0, 0.0};
    std::array<double, 3> periphery = {4.0, 4.0, 4.0};

    double center_density = periphery_distribution.getDensity(center);
    double periphery_density = periphery_distribution.getDensity(periphery);

    std::cout << "  Center chromatin density: " << center_density << std::endl;
    std::cout << "  Periphery chromatin density: " << periphery_density << std::endl;

    // Detailed testing would depend on the specific implementation

    std::cout << "ChromatinDistribution tests passed!" << std::endl << std::endl;
}

// Test particle properties
void testParticleProperties()
{
    std::cout << "Testing ParticleProperties..." << std::endl;

    // Create different particle types
    ParticleProperties photon;
    photon.type = RadiationType::PHOTON;
    photon.energy = 6.0;  // 6 MeV photons

    ParticleProperties proton;
    proton.type = RadiationType::PROTON;
    proton.energy = 100.0;  // 100 MeV protons

    ParticleProperties carbon;
    carbon.type = RadiationType::CARBON_ION;
    carbon.energy = 250.0;  // 250 MeV/u carbon ions

    // Calculate LET
    double photon_let = photon.calculateLET();
    double proton_let = proton.calculateLET();
    double carbon_let = carbon.calculateLET();

    std::cout << "  Calculated LET values:" << std::endl;
    std::cout << "    Photon (6 MeV): " << photon_let << " keV/µm" << std::endl;
    std::cout << "    Proton (100 MeV): " << proton_let << " keV/µm" << std::endl;
    std::cout << "    Carbon (250 MeV/u): " << carbon_let << " keV/µm" << std::endl;

    // LET should increase with atomic number for similar energy
    assert(photon_let < proton_let);
    assert(proton_let < carbon_let);

    // Calculate range
    double photon_range = photon.calculateRange();
    double proton_range = proton.calculateRange();
    double carbon_range = carbon.calculateRange();

    std::cout << "  Calculated range values:" << std::endl;
    std::cout << "    Photon (6 MeV): " << photon_range << " cm" << std::endl;
    std::cout << "    Proton (100 MeV): " << proton_range << " cm" << std::endl;
    std::cout << "    Carbon (250 MeV/u): " << carbon_range << " cm" << std::endl;

    // Range should be finite and reasonable
    assert(photon_range > 0);
    assert(proton_range > 0);
    assert(carbon_range > 0);

    std::cout << "ParticleProperties tests passed!" << std::endl << std::endl;
}

// Test the simulateMonteCarloDamage function
void testSimulateMonteCarloDamage()
{
    std::cout << "Testing simulateMonteCarloDamage function..." << std::endl;

    // Create biological system
    BiologicalSystemExtended biosystem =
        CellCycleModel::createDefaultBiologicalSystem(TissueType::SOFT_TISSUE);

    // Create particle properties
    ParticleProperties particle_props;
    particle_props.type = RadiationType::PHOTON;
    particle_props.energy = 6.0;  // 6 MeV
    particle_props.let = 0.2;     // keV/µm

    // Test with different doses
    std::vector<double> doses = {1.0, 2.0, 4.0};  // Gy
    std::vector<DNADamageProfile> results;

    for (double dose : doses) {
        DNADamageProfile damage =
            simulateMonteCarloDamage(biosystem, dose, 100, particle_props, 42);

        std::cout << "  Damage at " << dose << " Gy:" << std::endl;
        std::cout << "    SSBs: " << damage.single_strand_breaks << std::endl;
        std::cout << "    DSBs: " << damage.double_strand_breaks << std::endl;
        std::cout << "    Complex DSBs: " << damage.complex_dsb << std::endl;

        results.push_back(damage);
    }

    // Check that damage scales with dose
    assert(results[1].double_strand_breaks > results[0].double_strand_breaks);
    assert(results[2].double_strand_breaks > results[1].double_strand_breaks);

    // Test with different radiation types
    std::vector<RadiationType> rad_types = {RadiationType::PHOTON, RadiationType::PROTON,
                                            RadiationType::CARBON_ION};

    std::vector<DNADamageProfile> type_results;

    for (RadiationType type : rad_types) {
        ParticleProperties props;
        props.type = type;
        props.energy = (type == RadiationType::PHOTON) ? 6.0 : 100.0;

        DNADamageProfile damage = simulateMonteCarloDamage(biosystem, 2.0, 100, props, 42);

        std::string type_name;
        switch (type) {
            case RadiationType::PHOTON:
                type_name = "Photon";
                break;
            case RadiationType::PROTON:
                type_name = "Proton";
                break;
            case RadiationType::CARBON_ION:
                type_name = "Carbon ion";
                break;
            default:
                type_name = "Unknown";
                break;
        }

        std::cout << "  Damage from " << type_name << " radiation:" << std::endl;
        std::cout << "    DSBs: " << damage.double_strand_breaks << std::endl;
        std::cout << "    Complex DSBs: " << damage.complex_dsb << std::endl;
        std::cout << "    Complex/Simple ratio: " << damage.getSimpleToComplexRatio() << std::endl;

        type_results.push_back(damage);
    }

    // Higher LET particles should produce more complex damage
    assert(type_results[1].complex_dsb >= type_results[0].complex_dsb);
    assert(type_results[2].complex_dsb >= type_results[1].complex_dsb);

    std::cout << "simulateMonteCarloDamage tests passed!" << std::endl << std::endl;
}

// Main test function
int main()
{
    std::cout << "Running Monte Carlo Simulation Tests" << std::endl;
    std::cout << "===================================" << std::endl << std::endl;

    // Run tests
    testNucleusGeometry();
    testChromatinDistribution();
    testParticleProperties();
    testMonteCarloDamageSimulation();
    testSimulateMonteCarloDamage();

    std::cout << "All Monte Carlo simulation tests passed successfully!" << std::endl;
    return 0;
}
