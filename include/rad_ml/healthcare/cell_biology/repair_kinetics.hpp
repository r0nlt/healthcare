#ifndef RAD_ML_HEALTHCARE_CELL_BIOLOGY_REPAIR_KINETICS_HPP
#define RAD_ML_HEALTHCARE_CELL_BIOLOGY_REPAIR_KINETICS_HPP

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "rad_ml/healthcare/cell_biology/dna_damage_model.hpp"

namespace rad_ml {
namespace healthcare {
namespace cell_biology {

/**
 * Enumeration for repair pathways
 */
enum class RepairPathway {
    BER,       // Base excision repair
    NER,       // Nucleotide excision repair
    NHEJ,      // Non-homologous end joining
    HR,        // Homologous recombination
    MMR,       // Mismatch repair
    SSBR,      // Single-strand break repair
    ALT_NHEJ,  // Alternative non-homologous end joining
    NONE       // No repair
};

/**
 * Structure for repair pathway kinetics
 */
struct RepairPathwayKinetics {
    double capacity = 1.0;                // Capacity relative to normal
    double half_time = 1.0;               // Repair half-time (hours)
    double fidelity = 0.99;               // Repair fidelity (0-1)
    double saturation_threshold = 100.0;  // Damage threshold for saturation
    double activation_delay = 0.1;        // Time to activate pathway (hours)
    double efficiency_factor = 1.0;       // Efficiency factor
    bool active = true;                   // Whether pathway is active
};

/**
 * Structure for repair kinetics configuration
 */
struct RepairKineticsConfig {
    std::map<RepairPathway, RepairPathwayKinetics> pathway_kinetics;
    double global_modulation_factor = 1.0;      // Global modulation of all repair rates
    double temperature = 310.0;                 // Temperature in K
    double atp_availability = 1.0;              // ATP availability (0-1)
    double oxygen_level = 1.0;                  // Oxygen level (0-1)
    double cell_cycle_repair_modulation = 1.0;  // Cell cycle specific modulation
    bool enable_repair_saturation = true;       // Whether to model repair saturation
    bool enable_pathway_crosstalk = true;       // Whether to model pathway crosstalk
    bool enable_repair_timing = true;           // Whether to model repair timing
};

/**
 * Class for modeling sophisticated repair kinetics
 */
class RepairKineticsModel {
   public:
    /**
     * Constructor with configuration
     */
    explicit RepairKineticsModel(const RepairKineticsConfig& config);

    /**
     * Set the repair pathway kinetics
     * @param pathway Repair pathway
     * @param kinetics Pathway kinetics
     */
    void setRepairPathwayKinetics(RepairPathway pathway, const RepairPathwayKinetics& kinetics);

    /**
     * Enable or disable a repair pathway
     * @param pathway Repair pathway
     * @param active Whether pathway is active
     */
    void setRepairPathwayActive(RepairPathway pathway, bool active);

    /**
     * Set the global repair modulation factor
     * @param factor Global modulation factor
     */
    void setGlobalModulationFactor(double factor);

    /**
     * Set environmental parameters
     * @param temperature Temperature in K
     * @param oxygen_level Oxygen level (0-1)
     * @param atp_availability ATP availability (0-1)
     */
    void setEnvironmentalParameters(double temperature, double oxygen_level,
                                    double atp_availability);

    /**
     * Set cell cycle specific repair modulation
     * @param modulation_factor Cell cycle modulation factor
     */
    void setCellCycleRepairModulation(double modulation_factor);

    /**
     * Calculate time-evolving damage with repair
     * @param initial_damage Initial damage profile
     * @param time_hours Time after damage in hours
     * @return Remaining damage profile
     */
    DNADamageProfile calculateTimeEvolvingDamage(const DNADamageProfile& initial_damage,
                                                 double time_hours) const;

    /**
     * Calculate repair probability for a specific damage type
     * @param damage_type DNA damage type
     * @param time_hours Time after damage in hours
     * @return Repair probability
     */
    double calculateRepairProbability(DNADamageType damage_type, double time_hours) const;

    /**
     * Calculate misrepair probability for a specific damage type
     * @param damage_type DNA damage type
     * @param time_hours Time after damage in hours
     * @return Misrepair probability
     */
    double calculateMisrepairProbability(DNADamageType damage_type, double time_hours) const;

    /**
     * Get the appropriate repair pathway for a damage type
     * @param damage_type DNA damage type
     * @return Primary repair pathway
     */
    RepairPathway getPrimaryRepairPathway(DNADamageType damage_type) const;

    /**
     * Get all repair pathways for a damage type in order of importance
     * @param damage_type DNA damage type
     * @return Vector of repair pathways in order of importance
     */
    std::vector<RepairPathway> getRepairPathwaysForDamage(DNADamageType damage_type) const;

    /**
     * Calculate repair saturation factor based on damage load
     * @param damage DNA damage profile
     * @return Saturation factor (1.0 = no saturation)
     */
    double calculateRepairSaturationFactor(const DNADamageProfile& damage) const;

    /**
     * Get repair kinetics time function for a damage type
     * @param damage_type DNA damage type
     * @return Function that maps time to repair probability
     */
    std::function<double(double)> getRepairTimeFunction(DNADamageType damage_type) const;

    /**
     * Calculate cell survival based on unrepaired damage
     * @param initial_damage Initial damage profile
     * @param time_hours Time after damage in hours
     * @return Survival probability
     */
    double calculateSurvivalFromRepair(const DNADamageProfile& initial_damage,
                                       double time_hours) const;

    /**
     * Calculate mutation frequency based on misrepair
     * @param initial_damage Initial damage profile
     * @param time_hours Time after damage in hours
     * @return Mutation frequency
     */
    double calculateMutationFrequency(const DNADamageProfile& initial_damage,
                                      double time_hours) const;

    /**
     * Create default repair kinetics for a given biological system
     * @param biosystem Biological system
     * @return Default repair kinetics configuration
     */
    static RepairKineticsConfig createDefaultRepairKinetics(
        const BiologicalSystemExtended& biosystem);

    /**
     * Get default pathway kinetics for a specific pathway
     * @param pathway Repair pathway
     * @return Default pathway kinetics
     */
    static RepairPathwayKinetics getDefaultPathwayKinetics(RepairPathway pathway);

   private:
    // Configuration
    RepairKineticsConfig config_;

    // Calculate bi-exponential repair function
    double calculateBiExponentialRepair(double time_hours, double fast_fraction,
                                        double fast_half_time, double slow_half_time) const;

    // Calculate repair competition between pathways
    double calculateRepairCompetition(DNADamageType damage_type, double time_hours) const;

    // Calculate temperature effect on repair
    double calculateTemperatureEffect() const;

    // Calculate oxygen effect on repair
    double calculateOxygenEffect() const;

    // Calculate ATP availability effect on repair
    double calculateATPEffect() const;

    // Get effective repair rate for a pathway
    double getEffectiveRepairRate(RepairPathway pathway) const;

    // Calculate pathway crosstalk effects
    double calculatePathwayCrosstalk(RepairPathway primary_pathway,
                                     const DNADamageProfile& damage) const;

    // Get weight factors for repair pathways for a damage type
    std::map<RepairPathway, double> getPathwayWeights(DNADamageType damage_type) const;
};

/**
 * Function to calculate time-evolving damage with repair
 * @param initial_damage Initial cellular damage distribution
 * @param repair_kinetics Repair kinetics structure
 * @param time_hours Time after damage in hours
 * @return Remaining damage distribution
 */
DNADamageProfile calculateTimeEvolvingDamage(const DNADamageProfile& initial_damage,
                                             const DNARepairKinetics& repair_kinetics,
                                             double time_hours);

}  // namespace cell_biology
}  // namespace healthcare
}  // namespace rad_ml

#endif  // RAD_ML_HEALTHCARE_CELL_BIOLOGY_REPAIR_KINETICS_HPP
