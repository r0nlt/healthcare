#ifndef RAD_ML_HEALTHCARE_ENHANCED_RADIATION_THERAPY_MODEL_HPP
#define RAD_ML_HEALTHCARE_ENHANCED_RADIATION_THERAPY_MODEL_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

// Include all enhanced components
#include "rad_ml/healthcare/cell_biology/cell_cycle_model.hpp"
#include "rad_ml/healthcare/cell_biology/dna_damage_model.hpp"
#include "rad_ml/healthcare/cell_biology/repair_kinetics.hpp"
#include "rad_ml/healthcare/imaging/dicom_integration.hpp"
#include "rad_ml/healthcare/monte_carlo/damage_simulation.hpp"
#include "rad_ml/healthcare/quantum_enhanced/parameter_optimizer.hpp"
#include "rad_ml/healthcare/quantum_enhanced/tunneling_model.hpp"
#include "rad_ml/healthcare/quantum_enhanced/wave_equation_solver.hpp"

namespace rad_ml {
namespace healthcare {

using namespace rad_ml::healthcare::quantum_enhanced;
using namespace rad_ml::healthcare::cell_biology;
using namespace rad_ml::healthcare::monte_carlo;
using namespace rad_ml::healthcare::imaging;

/**
 * Configuration for the enhanced radiation therapy model
 */
struct EnhancedRadiationTherapyConfig {
    // Enable/disable components
    bool enable_quantum_effects = true;
    bool enable_cell_cycle_dependence = true;
    bool enable_monte_carlo_simulation = true;
    bool enable_dna_track_structure = true;
    bool enable_imaging_integration = true;

    // Quantum configuration
    WaveEquationConfig wave_equation_config;
    TunnelingConfig tunneling_config;

    // Cell biology configuration
    RepairKineticsConfig repair_config;

    // Monte Carlo configuration
    MonteCarloConfig monte_carlo_config;

    // Model parameters
    double temperature = 310.0;             // K
    double cell_size_threshold = 12.0;      // µm
    double radiation_dose_threshold = 2.0;  // Gy
    double dose_rate = 1.0;                 // Gy/min
    int num_fractions = 1;                  // Number of fractions

    // Get default configuration
    static EnhancedRadiationTherapyConfig getDefault()
    {
        EnhancedRadiationTherapyConfig config;
        // Default configurations will be used for subcomponents
        return config;
    }
};

/**
 * Result structure for enhanced radiation therapy model
 */
struct EnhancedRadiationTherapyResult {
    // Survival data
    double survival_fraction = 0.0;
    double survival_fraction_no_quantum = 0.0;
    double quantum_enhancement_factor = 0.0;

    // DNA damage data
    DNADamageProfile initial_damage;
    DNADamageProfile residual_damage;

    // Cell cycle redistribution
    CellCycleDistribution initial_cycle_distribution;
    CellCycleDistribution final_cycle_distribution;

    // Therapeutic ratio data
    double therapeutic_ratio = 0.0;
    double tumor_effect = 0.0;
    double normal_tissue_effect = 0.0;

    // Quantum effects data
    double tunneling_probability = 0.0;
    double zero_point_contribution = 0.0;

    // Track structure data
    int dsb_per_gy = 0;
    int clustered_damage_per_gy = 0;
    double complex_to_simple_ratio = 0.0;

    // Treatment planning metrics
    double tcp = 0.0;
    double ntcp = 0.0;
    double complication_free_tumor_control = 0.0;

    // Fractionation effects
    std::vector<double> fraction_survival;
    double biologically_effective_dose = 0.0;
    double equivalent_dose_2gy = 0.0;

    // Time dependence
    std::vector<std::pair<double, double>> time_survival_curve;
    std::vector<std::pair<double, DNADamageProfile>> time_damage_curve;
};

/**
 * Comprehensive enhanced radiation therapy model that integrates
 * all advanced components: quantum effects, cell cycle dependence,
 * DNA damage track structure, repair kinetics, and Monte Carlo simulation
 */
class EnhancedRadiationTherapyModel {
   public:
    /**
     * Constructor with configuration
     */
    explicit EnhancedRadiationTherapyModel(const EnhancedRadiationTherapyConfig& config);

    /**
     * Set the biological system
     * @param biosystem Extended biological system parameters
     */
    void setBiologicalSystem(const BiologicalSystemExtended& biosystem);

    /**
     * Set radiation parameters
     * @param params Radiation parameters
     */
    void setRadiationParams(const RadiationParams& params);

    /**
     * Set the particle properties for Monte Carlo simulation
     * @param properties Particle properties
     */
    void setParticleProperties(const ParticleProperties& properties);

    /**
     * Set nucleus geometry for Monte Carlo simulation
     * @param geometry Nucleus geometry
     */
    void setNucleusGeometry(const NucleusGeometry& geometry);

    /**
     * Set chromatin distribution for Monte Carlo simulation
     * @param distribution Chromatin distribution
     */
    void setChromatinDistribution(const ChromatinDistribution& distribution);

    /**
     * Set voxelized biological system from DICOM data
     * @param voxelized_biosystem Voxelized biological system
     */
    void setVoxelizedBiologicalSystem(const VoxelizedBiologicalSystem& voxelized_biosystem);

    /**
     * Enable or disable quantum effects
     * @param enable Whether to enable quantum effects
     */
    void enableQuantumEffects(bool enable);

    /**
     * Enable or disable cell cycle dependence
     * @param enable Whether to enable cell cycle dependence
     */
    void enableCellCycleDependence(bool enable);

    /**
     * Enable or disable Monte Carlo simulation
     * @param enable Whether to enable Monte Carlo simulation
     */
    void enableMonteCarloSimulation(bool enable);

    /**
     * Enable or disable DNA track structure modeling
     * @param enable Whether to enable DNA track structure modeling
     */
    void enableDNATrackStructure(bool enable);

    /**
     * Predict radiation effect on the biological system
     * @param dose Radiation dose in Gy
     * @return Enhanced radiation therapy result
     */
    EnhancedRadiationTherapyResult predictRadiationEffect(double dose);

    /**
     * Predict fractionated radiation effect
     * @param dose_per_fraction Dose per fraction in Gy
     * @param num_fractions Number of fractions
     * @param time_between_fractions Time between fractions in hours
     * @return Enhanced radiation therapy result
     */
    EnhancedRadiationTherapyResult predictFractionatedRadiationEffect(
        double dose_per_fraction, int num_fractions, double time_between_fractions);

    /**
     * Calculate therapeutic ratio
     * @param tumor_dose Tumor dose in Gy
     * @param normal_tissue_dose Normal tissue dose in Gy
     * @param tumor_biosystem Tumor biological system
     * @param normal_biosystem Normal tissue biological system
     * @return Therapeutic ratio
     */
    double calculateTherapeuticRatio(double tumor_dose, double normal_tissue_dose,
                                     const BiologicalSystemExtended& tumor_biosystem,
                                     const BiologicalSystemExtended& normal_biosystem);

    /**
     * Optimize radiation dose
     * @param min_dose Minimum dose in Gy
     * @param max_dose Maximum dose in Gy
     * @param step_size Step size in Gy
     * @param tumor_biosystem Tumor biological system
     * @param normal_biosystem Normal tissue biological system
     * @return Optimal dose in Gy
     */
    double optimizeRadiationDose(double min_dose, double max_dose, double step_size,
                                 const BiologicalSystemExtended& tumor_biosystem,
                                 const BiologicalSystemExtended& normal_biosystem);

    /**
     * Optimize fractionation schedule
     * @param total_dose Total dose in Gy
     * @param min_fraction_size Minimum fraction size in Gy
     * @param max_fraction_size Maximum fraction size in Gy
     * @param min_fractions Minimum number of fractions
     * @param max_fractions Maximum number of fractions
     * @return Optimal (fraction_size, num_fractions, time_between_fractions)
     */
    std::tuple<double, int, double> optimizeFractionationSchedule(double total_dose,
                                                                  double min_fraction_size,
                                                                  double max_fraction_size,
                                                                  int min_fractions,
                                                                  int max_fractions);

    /**
     * Calculate biological effective dose (BED)
     * @param dose Physical dose in Gy
     * @param num_fractions Number of fractions
     * @return BED in Gy
     */
    double calculateBED(double dose, int num_fractions);

    /**
     * Calculate equivalent dose in 2 Gy fractions (EQD2)
     * @param dose Physical dose in Gy
     * @param num_fractions Number of fractions
     * @return EQD2 in Gy
     */
    double calculateEQD2(double dose, int num_fractions);

    /**
     * Calculate tumor control probability (TCP)
     * @param dose Radiation dose in Gy
     * @param num_fractions Number of fractions
     * @return TCP value (0-1)
     */
    double calculateTCP(double dose, int num_fractions);

    /**
     * Calculate normal tissue complication probability (NTCP)
     * @param dose Radiation dose in Gy
     * @param num_fractions Number of fractions
     * @return NTCP value (0-1)
     */
    double calculateNTCP(double dose, int num_fractions);

    /**
     * Calculate probability of complication-free tumor control (P+)
     * @param dose Radiation dose in Gy
     * @param num_fractions Number of fractions
     * @return P+ value (0-1)
     */
    double calculateComplicationFreeTumorControl(double dose, int num_fractions);

    /**
     * Create a voxelized dose distribution based on a DICOM plan
     * @param plan DICOM plan
     * @param biosystem Voxelized biological system
     * @return DICOM dose distribution
     */
    DICOMDose createVoxelizedDoseDistribution(const DICOMPlan& plan,
                                              const VoxelizedBiologicalSystem& biosystem);

    /**
     * Calculate biologically effective dose distribution
     * @param physical_dose DICOM physical dose
     * @param biosystem Voxelized biological system
     * @param num_fractions Number of fractions
     * @return DICOM biological dose
     */
    DICOMDose calculateBiologicalDoseDistribution(const DICOMDose& physical_dose,
                                                  const VoxelizedBiologicalSystem& biosystem,
                                                  int num_fractions);

    /**
     * Get access to the quantum wave equation solver
     * @return Quantum wave equation solver reference
     */
    QuantumWaveEquationSolver& getWaveEquationSolver();

    /**
     * Get access to the quantum tunneling model
     * @return Quantum tunneling model reference
     */
    QuantumTunnelingModel& getTunnelingModel();

    /**
     * Get access to the cell cycle model
     * @return Cell cycle model reference
     */
    CellCycleModel& getCellCycleModel();

    /**
     * Get access to the DNA damage model
     * @return DNA damage model reference
     */
    DNADamageModel& getDNADamageModel();

    /**
     * Get access to the repair kinetics model
     * @return Repair kinetics model reference
     */
    RepairKineticsModel& getRepairKineticsModel();

    /**
     * Get access to the Monte Carlo simulation model
     * @return Monte Carlo simulation reference
     */
    MonteCarloDamageSimulation& getMonteCarloSimulation();

    /**
     * Get access to the DICOM integration module
     * @return DICOM integration reference
     */
    DICOMIntegration& getDICOMIntegration();

   private:
    // Configuration
    EnhancedRadiationTherapyConfig config_;

    // Biological system
    BiologicalSystemExtended biosystem_;

    // Radiation parameters
    RadiationParams radiation_params_;

    // Voxelized biological system
    std::shared_ptr<VoxelizedBiologicalSystem> voxelized_biosystem_;

    // Component models
    std::unique_ptr<QuantumWaveEquationSolver> wave_equation_solver_;
    std::unique_ptr<QuantumTunnelingModel> tunneling_model_;
    std::unique_ptr<CellCycleModel> cell_cycle_model_;
    std::unique_ptr<DNADamageModel> dna_damage_model_;
    std::unique_ptr<RepairKineticsModel> repair_kinetics_model_;
    std::unique_ptr<MonteCarloDamageSimulation> monte_carlo_simulation_;
    std::unique_ptr<DICOMIntegration> dicom_integration_;

    // Initialize component models
    void initializeModels();

    // Calculate survival fraction
    double calculateSurvivalFraction(double dose);

    // Calculate survival fraction without quantum effects
    double calculateSurvivalFractionNoQuantum(double dose);

    // Calculate DNA damage
    DNADamageProfile calculateDNADamage(double dose);

    // Calculate DNA damage using Monte Carlo simulation
    DNADamageProfile calculateMonteCarloDNADamage(double dose);

    // Calculate residual damage after repair
    DNADamageProfile calculateResidualDamage(const DNADamageProfile& initial_damage,
                                             double time_hours);

    // Calculate quantum enhancement factor
    double calculateQuantumEnhancementFactor(double dose);

    // Calculate cell cycle redistribution
    CellCycleDistribution calculateCellCycleRedistribution(double dose, double time_hours);

    // Calculate time-dependent survival curve
    std::vector<std::pair<double, double>> calculateTimeSurvivalCurve(double dose, double max_time,
                                                                      int num_points);

    // Calculate time-dependent damage curve
    std::vector<std::pair<double, DNADamageProfile>> calculateTimeDamageCurve(double dose,
                                                                              double max_time,
                                                                              int num_points);

    // Apply quantum corrections to damage model
    DNADamageProfile applyQuantumCorrections(const DNADamageProfile& damage, double temperature,
                                             double cell_size);

    // Calculate repair probability
    double calculateRepairProbability(const DNADamageProfile& damage, double time_hours);
};

/**
 * Free function to predict radiation damage with quantum corrections
 * @param dose Radiation dose in Gy
 * @param biosystem Biological system
 * @param config Configuration
 * @return Enhanced radiation therapy result
 */
EnhancedRadiationTherapyResult predictEnhancedRadiationEffect(
    double dose, const BiologicalSystemExtended& biosystem,
    const EnhancedRadiationTherapyConfig& config = EnhancedRadiationTherapyConfig::getDefault());

/**
 * Free function to calculate therapeutic ratio with quantum corrections
 * @param tumor_dose Tumor dose in Gy
 * @param normal_tissue_dose Normal tissue dose in Gy
 * @param tumor_biosystem Tumor biological system
 * @param normal_biosystem Normal tissue biological system
 * @param config Configuration
 * @return Therapeutic ratio
 */
double calculateEnhancedTherapeuticRatio(
    double tumor_dose, double normal_tissue_dose, const BiologicalSystemExtended& tumor_biosystem,
    const BiologicalSystemExtended& normal_biosystem,
    const EnhancedRadiationTherapyConfig& config = EnhancedRadiationTherapyConfig::getDefault());

}  // namespace healthcare
}  // namespace rad_ml

#endif  // RAD_ML_HEALTHCARE_ENHANCED_RADIATION_THERAPY_MODEL_HPP
