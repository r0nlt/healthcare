#ifndef RAD_ML_HEALTHCARE_MONTE_CARLO_DAMAGE_SIMULATION_HPP
#define RAD_ML_HEALTHCARE_MONTE_CARLO_DAMAGE_SIMULATION_HPP

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "rad_ml/healthcare/cell_biology/cell_cycle_model.hpp"
#include "rad_ml/healthcare/cell_biology/dna_damage_model.hpp"

namespace rad_ml {
namespace healthcare {
namespace monte_carlo {

using namespace rad_ml::healthcare::cell_biology;

// Forward declaration
namespace cell_biology {
struct BiologicalSystemExtended;
}

/**
 * Structure for particle properties
 */
struct ParticleProperties {
    RadiationType type = RadiationType::PHOTON;
    double energy = 1.0;             // MeV
    double let = 0.2;                // keV/µm
    double charge = 0.0;             // Elementary charge
    double mass = 0.0;               // MeV/c²
    double range = 100.0;            // µm in water
    double relativistic_beta = 0.9;  // v/c

    // Calculate LET from energy and particle type if not specified
    double calculateLET() const;

    // Calculate range from energy and particle type if not specified
    double calculateRange() const;
};

/**
 * Structure for cell nucleus geometry
 */
struct NucleusGeometry {
    enum class Shape { SPHERE, ELLIPSOID, CYLINDER, CUSTOM };

    Shape shape = Shape::SPHERE;
    double radius = 5.0;                                // µm
    double height = 10.0;                               // µm (for cylinder)
    std::array<double, 3> semi_axes = {5.0, 5.0, 5.0};  // µm (for ellipsoid)
    std::array<double, 3> center = {0.0, 0.0, 0.0};     // µm

    // Function for custom shapes that returns true if point is inside nucleus
    std::function<bool(const std::array<double, 3>&)> is_inside_function;

    // Check if a point is inside the nucleus
    bool isInside(const std::array<double, 3>& point) const;
};

/**
 * Structure for chromatin distribution
 */
struct ChromatinDistribution {
    enum class Model { UNIFORM, HETEROCHROMATIN_PERIPHERY, CHROMOSOME_TERRITORIES, CUSTOM };

    Model model = Model::UNIFORM;
    double heterochromatin_fraction = 0.8;                  // Fraction of heterochromatin
    double chromatin_density = 0.015;                       // DNA density (bp/nm³)
    std::vector<std::array<double, 3>> chromosome_centers;  // Centers of chromosome territories

    // Function for custom chromatin distribution that returns chromatin density at a point
    std::function<double(const std::array<double, 3>&)> density_function;

    // Get chromatin density at a point
    double getDensity(const std::array<double, 3>& point) const;
};

/**
 * Configuration for Monte Carlo simulation
 */
struct MonteCarloConfig {
    // Random number generation
    int random_seed = 42;

    // Simulation parameters
    int num_particles = 1000;
    double dose = 2.0;         // Gy
    double voxel_size = 10.0;  // nm
    bool include_indirect_damage = true;
    bool include_chemical_stage = true;
    bool include_dna_structure = true;

    // Physical stage parameters
    double ionization_energy = 15.0;        // eV in water
    double radical_diffusion_length = 6.0;  // nm
    double radical_lifetime = 1.0e-9;       // s

    // Chemical stage parameters
    double oh_reaction_radius = 1.0;       // nm
    double temperature = 310.0;            // K
    double scavenger_concentration = 1.0;  // mM

    // DNA structure parameters
    double dna_segment_length = 0.34;  // nm (base pair)
    double dna_radius = 1.0;           // nm
    double nucleosome_radius = 5.0;    // nm

    // Output configurations
    bool output_track_structure = true;
    bool output_radical_distribution = true;
    bool output_damage_sites = true;
};

/**
 * Result structure for Monte Carlo simulation
 */
struct MonteCarloResult {
    // Damage profile
    DNADamageProfile damage_profile;

    // Microscopic damage distribution
    std::vector<std::array<double, 3>> damage_positions;
    std::map<std::array<double, 3>, DNADamageType> damage_types;

    // Physical stage output
    std::vector<std::array<double, 4>> ionization_events;  // x, y, z, energy
    std::vector<std::array<double, 7>> particle_tracks;    // x, y, z, dx, dy, dz, energy

    // Chemical stage output
    std::vector<std::array<double, 4>> radical_positions;  // x, y, z, time

    // Statistical metrics
    double mean_ionizations_per_gray = 0.0;
    double mean_dsb_per_gray = 0.0;
    double complex_damage_fraction = 0.0;
    double clustered_to_isolated_ratio = 0.0;

    // Track statistics
    std::vector<double> energy_deposition_spectrum;
    std::vector<double> track_length_distribution;
    std::vector<double> let_distribution;
};

/**
 * Class for Monte Carlo simulation of radiation damage
 */
class MonteCarloDamageSimulation {
   public:
    /**
     * Constructor with biological system and configuration
     */
    MonteCarloDamageSimulation(const BiologicalSystemExtended& biosystem,
                               const MonteCarloConfig& config);

    /**
     * Set particle properties
     * @param properties Particle properties
     */
    void setParticleProperties(const ParticleProperties& properties);

    /**
     * Set nucleus geometry
     * @param geometry Nucleus geometry
     */
    void setNucleusGeometry(const NucleusGeometry& geometry);

    /**
     * Set chromatin distribution
     * @param distribution Chromatin distribution
     */
    void setChromatinDistribution(const ChromatinDistribution& distribution);

    /**
     * Run the simulation
     * @return Simulation results
     */
    MonteCarloResult runSimulation();

    /**
     * Run the simulation with a specific number of particles
     * @param num_particles Number of particles to simulate
     * @return Simulation results
     */
    MonteCarloResult runSimulation(int num_particles);

    /**
     * Run the simulation with a specific dose
     * @param dose Dose in Gy
     * @return Simulation results
     */
    MonteCarloResult runSimulationWithDose(double dose);

    /**
     * Get the default particle properties for a radiation type
     * @param type Radiation type
     * @return Default properties
     */
    static ParticleProperties getDefaultParticleProperties(RadiationType type);

    /**
     * Get the default nucleus geometry for a cell type
     * @param cell_type Cell type string
     * @return Default geometry
     */
    static NucleusGeometry getDefaultNucleusGeometry(const std::string& cell_type);

    /**
     * Get the default chromatin distribution for a cell type
     * @param cell_type Cell type string
     * @return Default distribution
     */
    static ChromatinDistribution getDefaultChromatinDistribution(const std::string& cell_type);

    /**
     * Convert dose to number of particles
     * @param dose Dose in Gy
     * @param particle_properties Particle properties
     * @param nucleus_geometry Nucleus geometry
     * @return Number of particles
     */
    static int doseToParticles(double dose, const ParticleProperties& particle_properties,
                               const NucleusGeometry& nucleus_geometry);

    /**
     * Convert number of particles to dose
     * @param num_particles Number of particles
     * @param particle_properties Particle properties
     * @param nucleus_geometry Nucleus geometry
     * @return Dose in Gy
     */
    static double particlesToDose(int num_particles, const ParticleProperties& particle_properties,
                                  const NucleusGeometry& nucleus_geometry);

   private:
    // Biological system
    BiologicalSystemExtended biosystem_;

    // Simulation configuration
    MonteCarloConfig config_;

    // Particle properties
    ParticleProperties particle_properties_;

    // Nucleus geometry
    NucleusGeometry nucleus_geometry_;

    // Chromatin distribution
    ChromatinDistribution chromatin_distribution_;

    // Random number generator
    std::mt19937 rng_;

    // Simulate a single particle track
    std::vector<std::array<double, 7>> simulateParticleTrack(
        const std::array<double, 3>& entry_point, const std::array<double, 3>& direction);

    // Generate a random entry point on the nucleus surface
    std::array<double, 3> generateRandomEntryPoint();

    // Generate a random direction for particle
    std::array<double, 3> generateRandomDirection();

    // Simulate physical stage (ionizations and excitations)
    std::vector<std::array<double, 4>> simulatePhysicalStage(
        const std::vector<std::array<double, 7>>& tracks);

    // Simulate chemical stage (radical formation and diffusion)
    std::vector<std::array<double, 4>> simulateChemicalStage(
        const std::vector<std::array<double, 4>>& ionization_events);

    // Simulate biological stage (DNA damage formation)
    DNADamageProfile simulateBiologicalStage(
        const std::vector<std::array<double, 4>>& radical_positions);

    // Calculate microscopic damage distribution
    void calculateMicroscopicDamageDistribution(
        const std::vector<std::array<double, 4>>& radical_positions,
        std::vector<std::array<double, 3>>& damage_positions,
        std::map<std::array<double, 3>, DNADamageType>& damage_types);

    // Calculate energy deposition in a voxel
    double calculateEnergyDeposition(const std::array<double, 3>& position,
                                     const std::vector<std::array<double, 4>>& ionization_events);

    // Calculate probability of damage for a given energy deposition
    double calculateDamageProbability(double energy_deposition,
                                      const std::array<double, 3>& position);

    // Determine damage type based on energy deposition and location
    DNADamageType determineDamageType(double energy_deposition,
                                      const std::array<double, 3>& position);

    // Calculate damage clustering
    int calculateDamageClustering(const std::vector<std::array<double, 3>>& damage_positions);
};

/**
 * Function to simulate Monte Carlo damage
 * @param biosystem Biological system
 * @param radiation_dose Radiation dose in Gy
 * @param num_particles Number of particles to simulate
 * @param particle_props Particle properties
 * @param random_seed Random seed
 * @return DNA damage profile
 */
DNADamageProfile simulateMonteCarloDamage(const BiologicalSystemExtended& biosystem,
                                          double radiation_dose, int num_particles,
                                          const ParticleProperties& particle_props,
                                          int random_seed);

}  // namespace monte_carlo
}  // namespace healthcare
}  // namespace rad_ml

#endif  // RAD_ML_HEALTHCARE_MONTE_CARLO_DAMAGE_SIMULATION_HPP
