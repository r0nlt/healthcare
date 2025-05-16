#ifndef RAD_ML_HEALTHCARE_CELL_BIOLOGY_DNA_DAMAGE_MODEL_HPP
#define RAD_ML_HEALTHCARE_CELL_BIOLOGY_DNA_DAMAGE_MODEL_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace rad_ml {
namespace healthcare {
namespace cell_biology {

// Forward declarations
struct BiologicalSystemExtended;

/**
 * Enumeration for DNA damage types
 */
enum class DNADamageType {
    SSB,               // Single-strand break
    DSB,               // Double-strand break
    BASE_DAMAGE,       // Base damage (oxidation, methylation, etc.)
    CLUSTERED_DAMAGE,  // Multiple damages within 10 base pairs
    COMPLEX_DSB        // DSB with additional damages nearby
};

/**
 * Enumeration for radiation types
 */
enum class RadiationType {
    PHOTON,      // X-rays, gamma rays
    ELECTRON,    // Beta particles
    PROTON,      // Proton beam
    ALPHA,       // Alpha particles
    CARBON_ION,  // Carbon ions
    NEUTRON      // Neutrons
};

/**
 * Structure for DNA damage profile
 */
struct DNADamageProfile {
    int single_strand_breaks = 0;  // Count of SSBs
    int double_strand_breaks = 0;  // Count of DSBs
    int base_damages = 0;          // Count of base damages
    int clustered_damages = 0;     // Count of clustered damages
    int complex_dsb = 0;           // Count of complex DSBs

    // Calculate total damage
    int getTotalDamage() const
    {
        return single_strand_breaks + double_strand_breaks + base_damages + clustered_damages +
               complex_dsb;
    }

    // Calculate simple to complex damage ratio
    double getSimpleToComplexRatio() const
    {
        double simple = single_strand_breaks + base_damages;
        double complex = double_strand_breaks + clustered_damages + complex_dsb;
        return (complex > 0) ? simple / complex : 0.0;
    }
};

/**
 * Track structure parameters
 */
struct TrackStructureParams {
    double let = 0.2;                 // Linear energy transfer (keV/µm)
    double track_radius = 5.0;        // Track radius (nm)
    double ionization_density = 3.0;  // Ionizations per nm
    double mean_free_path = 2.0;      // Mean free path between ionizations (nm)
    double energy = 1.0;              // Particle energy (MeV)
    double dose_rate = 1.0;           // Dose rate (Gy/min)
};

/**
 * Radiation parameters
 */
struct RadiationParams {
    RadiationType type = RadiationType::PHOTON;
    double dose = 0.0;                  // Dose in Gy
    double energy = 0.0;                // Energy in MeV
    double let = 0.0;                   // LET in keV/µm
    double rbe = 1.0;                   // Relative biological effectiveness
    double oer = 1.0;                   // Oxygen enhancement ratio
    double dose_rate = 0.0;             // Dose rate in Gy/min
    TrackStructureParams track_params;  // Track structure parameters

    // Get LET if not explicitly specified
    double getEffectiveLET() const;

    // Get RBE if not explicitly specified
    double getEffectiveRBE() const;
};

/**
 * DNA repair kinetics parameters
 */
struct DNARepairKinetics {
    double fast_repair_half_time = 0.5;          // Fast repair half-time (hours)
    double slow_repair_half_time = 8.0;          // Slow repair half-time (hours)
    double fast_repair_fraction = 0.7;           // Fraction of damage repaired by fast process
    double repair_fidelity_dsb = 0.95;           // Probability of correct DSB repair
    double repair_fidelity_ssb = 0.999;          // Probability of correct SSB repair
    double repair_fidelity_base = 0.998;         // Probability of correct base damage repair
    double repair_fidelity_complex = 0.8;        // Probability of correct complex damage repair
    double repair_saturation_threshold = 100.0;  // Damage level at which repair saturates
    double misrepair_probability = 0.05;         // Probability of misrepair
};

/**
 * Chromatin structure parameters
 */
struct ChromatinStructure {
    double dna_density = 0.015;               // DNA density (bp/nm³)
    double heterochromatin_fraction = 0.8;    // Fraction of heterochromatin
    double euchromatin_fraction = 0.2;        // Fraction of euchromatin
    double nucleosome_density = 0.0005;       // Nucleosomes per nm³
    double dna_hydration_level = 0.3;         // Hydration level (g water/g DNA)
    double histone_modification_level = 0.5;  // Level of histone modifications (0-1)
    double chromatin_compaction = 0.7;        // Chromatin compaction level (0-1)
};

/**
 * Class for modeling DNA damage with track structure
 */
class DNADamageModel {
   public:
    /**
     * Constructor with biological system
     */
    explicit DNADamageModel(const BiologicalSystemExtended& biosystem);

    /**
     * Set radiation parameters
     * @param params Radiation parameters
     */
    void setRadiationParams(const RadiationParams& params);

    /**
     * Set DNA repair kinetics
     * @param repair_kinetics DNA repair kinetics parameters
     */
    void setDNARepairKinetics(const DNARepairKinetics& repair_kinetics);

    /**
     * Set chromatin structure
     * @param chromatin_structure Chromatin structure parameters
     */
    void setChromatinStructure(const ChromatinStructure& chromatin_structure);

    /**
     * Calculate track structure DNA damage
     * @param dose Radiation dose in Gy
     * @param let Linear energy transfer in keV/µm
     * @return DNA damage profile
     */
    DNADamageProfile calculateTrackStructureDamage(double dose, double let) const;

    /**
     * Calculate damage based on radiation parameters
     * @return DNA damage profile
     */
    DNADamageProfile calculateDamage() const;

    /**
     * Calculate time-evolving damage with repair
     * @param initial_damage Initial damage profile
     * @param time_hours Time after irradiation in hours
     * @return Remaining damage profile
     */
    DNADamageProfile calculateTimeEvolvingDamage(const DNADamageProfile& initial_damage,
                                                 double time_hours) const;

    /**
     * Calculate cell survival based on DNA damage
     * @param damage DNA damage profile
     * @return Survival probability
     */
    double calculateSurvivalFromDamage(const DNADamageProfile& damage) const;

    /**
     * Calculate misrepair probability
     * @param damage DNA damage profile
     * @return Misrepair probability
     */
    double calculateMisrepairProbability(const DNADamageProfile& damage) const;

    /**
     * Calculate mutation probability
     * @param damage DNA damage profile
     * @return Mutation probability
     */
    double calculateMutationProbability(const DNADamageProfile& damage) const;

    /**
     * Calculate repair protein saturation effects
     * @param damage DNA damage profile
     * @return Factor by which repair is slowed (1.0 = no slowdown)
     */
    double calculateRepairSaturation(const DNADamageProfile& damage) const;

    /**
     * Simulate DNA damage distribution in 3D chromatin
     * @param num_tracks Number of particle tracks to simulate
     * @return 3D coordinates of damage sites
     */
    std::vector<std::array<double, 3>> simulateDamageDistribution(int num_tracks) const;

    /**
     * Calculate damage clustering probability
     * @param damage_density Damage density (damages/µm³)
     * @return Clustering probability
     */
    double calculateClusteringProbability(double damage_density) const;

    /**
     * Get default radiation parameters for a given radiation type
     * @param type Radiation type
     * @return Default radiation parameters
     */
    static RadiationParams getDefaultRadiationParams(RadiationType type);

    /**
     * Get default track structure parameters for a given radiation type
     * @param type Radiation type
     * @return Default track structure parameters
     */
    static TrackStructureParams getDefaultTrackStructureParams(RadiationType type);

    /**
     * Get default DNA repair kinetics for a given biological system
     * @param biosystem Biological system
     * @return Default DNA repair kinetics
     */
    static DNARepairKinetics getDefaultDNARepairKinetics(const BiologicalSystemExtended& biosystem);

    /**
     * Convert dose to average number of tracks per nucleus
     * @param dose Dose in Gy
     * @param let LET in keV/µm
     * @param nucleus_diameter Nucleus diameter in µm
     * @return Average number of tracks
     */
    static double doseToTrackNumber(double dose, double let, double nucleus_diameter);

   private:
    // Biological system
    BiologicalSystemExtended biosystem_;

    // Radiation parameters
    RadiationParams radiation_params_;

    // DNA repair kinetics
    DNARepairKinetics repair_kinetics_;

    // Chromatin structure
    ChromatinStructure chromatin_structure_;

    // Calculate single track damage
    DNADamageProfile calculateSingleTrackDamage(const TrackStructureParams& track_params) const;

    // Calculate track overlap probability
    double calculateTrackOverlapProbability(double dose, double track_radius) const;

    // Calculate damage clustering
    int calculateDamageClustering(int num_damages, double damage_density) const;

    // Calculate repair kinetics for a damage type
    double calculateRepairKinetics(DNADamageType damage_type, double time_hours) const;

    // Calculate LET-dependent RBE
    double calculateLETDependentRBE(double let) const;

    // Calculate physical stage (ionizations and excitations)
    void simulatePhysicalStage(TrackStructureParams& track_params,
                               std::vector<std::array<double, 3>>& ionization_sites) const;

    // Calculate chemical stage (radical formation and diffusion)
    void simulateChemicalStage(const std::vector<std::array<double, 3>>& ionization_sites,
                               std::vector<std::array<double, 3>>& radical_sites) const;

    // Calculate biological stage (DNA damage formation)
    DNADamageProfile simulateBiologicalStage(
        const std::vector<std::array<double, 3>>& radical_sites) const;
};

/**
 * Calculate track structure damage for a given biological system
 * @param dose Radiation dose in Gy
 * @param let Linear energy transfer in keV/µm
 * @param biosystem Biological system
 * @return DNA damage profile
 */
DNADamageProfile calculateTrackStructureDamage(double dose, double let,
                                               const BiologicalSystemExtended& biosystem);

}  // namespace cell_biology
}  // namespace healthcare
}  // namespace rad_ml

#endif  // RAD_ML_HEALTHCARE_CELL_BIOLOGY_DNA_DAMAGE_MODEL_HPP
