#ifndef RAD_ML_HEALTHCARE_CELL_BIOLOGY_CELL_CYCLE_MODEL_HPP
#define RAD_ML_HEALTHCARE_CELL_BIOLOGY_CELL_CYCLE_MODEL_HPP

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace rad_ml {
namespace healthcare {
namespace cell_biology {

/**
 * Enumeration for cell cycle phases
 */
enum class CellCyclePhase {
    G0,  // Quiescent/resting phase
    G1,  // First gap phase
    S,   // DNA synthesis phase
    G2,  // Second gap phase
    M    // Mitosis phase
};

/**
 * Enumeration for tissue type
 */
enum class TissueType {
    SOFT_TISSUE,
    BONE,
    NERVE,
    MUSCLE,
    BLOOD,
    EPITHELIAL,
    STEM_CELL,
    TUMOR_RAPIDLY_DIVIDING,
    TUMOR_HYPOXIC
};

/**
 * Cell cycle distribution structure
 */
struct CellCycleDistribution {
    double g0_fraction = 0.0;   // Fraction of cells in G0 phase
    double g1_fraction = 0.5;   // Fraction of cells in G1 phase
    double s_fraction = 0.25;   // Fraction of cells in S phase
    double g2_fraction = 0.15;  // Fraction of cells in G2 phase
    double m_fraction = 0.1;    // Fraction of cells in M phase

    // Validation to ensure fractions sum to 1.0
    bool isValid() const
    {
        double sum = g0_fraction + g1_fraction + s_fraction + g2_fraction + m_fraction;
        return (sum > 0.99 && sum < 1.01);  // Allow for small floating-point errors
    }
};

/**
 * Phase-specific radiosensitivity parameters
 */
struct PhaseSensitivity {
    double alpha = 0.0;             // Gy⁻¹ (linear component of LQ model)
    double beta = 0.0;              // Gy⁻² (quadratic component of LQ model)
    double oer = 1.0;               // Oxygen enhancement ratio
    double repair_capacity = 1.0;   // Repair capacity (relative to G1)
    double repair_half_time = 1.0;  // Repair half-time (hours)
};

/**
 * Extended BiologicalSystem structure with cell cycle information
 */
struct BiologicalSystemExtended {
    TissueType type = TissueType::SOFT_TISSUE;
    double water_content = 0.7;      // 70% water
    double cell_density = 1.0e6;     // 1 million cells per mm³
    double effective_barrier = 0.3;  // eV
    double repair_rate = 0.3;        // cellular repair capability
    double radiosensitivity = 1.2;   // radiation sensitivity factor

    // Cell cycle parameters
    CellCycleDistribution cycle_distribution;
    std::map<CellCyclePhase, PhaseSensitivity> phase_sensitivity;

    // Oxygen level (affects radiosensitivity)
    double oxygen_tension = 21.0;  // % O₂ (normoxic = 21%, hypoxic < 5%)

    // Proliferation parameters
    double doubling_time = 24.0;       // hours
    double growth_fraction = 0.7;      // fraction of proliferating cells
    double stem_cell_fraction = 0.05;  // fraction of stem cells
};

/**
 * Class for modeling cell cycle-dependent radiation response
 */
class CellCycleModel {
   public:
    /**
     * Constructor with biological system parameters
     */
    explicit CellCycleModel(const BiologicalSystemExtended& biosystem);

    /**
     * Set the cell cycle distribution
     * @param distribution Cell cycle distribution
     */
    void setCellCycleDistribution(const CellCycleDistribution& distribution);

    /**
     * Set the phase-specific radiosensitivity parameters
     * @param phase Cell cycle phase
     * @param sensitivity Radiosensitivity parameters
     */
    void setPhaseSensitivity(CellCyclePhase phase, const PhaseSensitivity& sensitivity);

    /**
     * Set oxygen tension (affects radiosensitivity)
     * @param oxygen_percent Oxygen tension in percent
     */
    void setOxygenTension(double oxygen_percent);

    /**
     * Calculate the cell cycle-weighted radiosensitivity
     * @return Effective alpha/beta values for the cell population
     */
    std::pair<double, double> calculateCellCycleWeightedRadiosensitivity() const;

    /**
     * Calculate survival fraction using the Linear-Quadratic model
     * @param dose Radiation dose in Gy
     * @return Survival fraction
     */
    double calculateSurvivalFraction(double dose) const;

    /**
     * Calculate survival fraction with fractionation
     * @param dose_per_fraction Dose per fraction in Gy
     * @param num_fractions Number of fractions
     * @param time_between_fractions Time between fractions in hours
     * @return Survival fraction
     */
    double calculateFractionatedSurvivalFraction(double dose_per_fraction, int num_fractions,
                                                 double time_between_fractions) const;

    /**
     * Calculate cell cycle redistribution after irradiation
     * @param dose Radiation dose in Gy
     * @param time_after_irradiation Time after irradiation in hours
     * @return Updated cell cycle distribution
     */
    CellCycleDistribution calculateCellCycleRedistribution(double dose,
                                                           double time_after_irradiation) const;

    /**
     * Calculate survival for a specific cell cycle phase
     * @param phase Cell cycle phase
     * @param dose Radiation dose in Gy
     * @return Survival fraction for cells in this phase
     */
    double calculatePhaseSurvivalFraction(CellCyclePhase phase, double dose) const;

    /**
     * Set cell doubling time
     * @param doubling_time_hours Doubling time in hours
     */
    void setCellDoublingTime(double doubling_time_hours);

    /**
     * Get tissue regeneration capacity
     * @return Regeneration capacity index (0-1)
     */
    double getTissueRegenerationCapacity() const;

    /**
     * Calculate optimal timing for fractionated radiotherapy
     * @param total_dose Total prescribed dose in Gy
     * @param min_fraction_size Minimum fraction size in Gy
     * @param max_fraction_size Maximum fraction size in Gy
     * @param target_survival_cutoff Target survival cutoff
     * @return Optimal (fraction size, number of fractions, timing)
     */
    std::tuple<double, int, double> optimizeFractionationSchedule(
        double total_dose, double min_fraction_size, double max_fraction_size,
        double target_survival_cutoff) const;

    /**
     * Create a default biological system with typical parameters for a given tissue type
     * @param type Tissue type
     * @return Default biological system for that tissue type
     */
    static BiologicalSystemExtended createDefaultBiologicalSystem(TissueType type);

    /**
     * Get default cell cycle distribution for a given tissue type
     * @param type Tissue type
     * @return Default cell cycle distribution
     */
    static CellCycleDistribution getDefaultCellCycleDistribution(TissueType type);

    /**
     * Get default phase sensitivity for a given tissue type and cell cycle phase
     * @param type Tissue type
     * @param phase Cell cycle phase
     * @return Default phase sensitivity
     */
    static PhaseSensitivity getDefaultPhaseSensitivity(TissueType type, CellCyclePhase phase);

   private:
    // Biological system parameters
    BiologicalSystemExtended biosystem_;

    // Calculate repopulation effect
    double calculateRepopulationFactor(double time_hours) const;

    // Calculate repair effect
    double calculateRepairFactor(double time_hours) const;

    // Calculate oxygen enhancement effect
    double calculateOxygenEnhancementFactor() const;

    // Cell cycle progression model
    CellCycleDistribution progressCellCycle(const CellCycleDistribution& initial_distribution,
                                            double time_hours) const;
};

/**
 * Calculate the cell cycle-weighted radiosensitivity
 * @param biosystem Extended biological system with cell cycle information
 * @return Effective alpha/beta values
 */
std::pair<double, double> calculateCellCycleWeightedRadiosensitivity(
    const BiologicalSystemExtended& biosystem);

}  // namespace cell_biology
}  // namespace healthcare
}  // namespace rad_ml

#endif  // RAD_ML_HEALTHCARE_CELL_BIOLOGY_CELL_CYCLE_MODEL_HPP
