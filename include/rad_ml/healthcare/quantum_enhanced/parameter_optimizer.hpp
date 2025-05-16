#ifndef RAD_ML_HEALTHCARE_QUANTUM_ENHANCED_PARAMETER_OPTIMIZER_HPP
#define RAD_ML_HEALTHCARE_QUANTUM_ENHANCED_PARAMETER_OPTIMIZER_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "rad_ml/healthcare/cell_biology/cell_cycle_model.hpp"

namespace rad_ml {
namespace healthcare {
namespace quantum_enhanced {

using namespace rad_ml::healthcare::cell_biology;

/**
 * Structure to represent a parameter with constraints
 */
struct OptimizableParameter {
    std::string name;
    double value;
    double min_value;
    double max_value;
    double step_size;
    bool is_quantum = false;    // Whether parameter affects quantum effects
    bool is_log_scale = false;  // Whether to use logarithmic scale for optimization
    std::string unit;           // Parameter unit
    std::string description;    // Parameter description

    // Create a parameter with min/max constraints
    static OptimizableParameter create(const std::string& name, double value, double min_value,
                                       double max_value, const std::string& unit = "",
                                       const std::string& description = "")
    {
        OptimizableParameter param;
        param.name = name;
        param.value = value;
        param.min_value = min_value;
        param.max_value = max_value;
        param.step_size = (max_value - min_value) / 20.0;
        param.unit = unit;
        param.description = description;
        return param;
    }
};

/**
 * Enumeration for optimization methods
 */
enum class OptimizationMethod {
    GRID_SEARCH,          // Exhaustive grid search
    GRADIENT_DESCENT,     // Gradient descent
    SIMULATED_ANNEALING,  // Simulated annealing
    GENETIC_ALGORITHM,    // Genetic algorithm
    PARTICLE_SWARM,       // Particle swarm optimization
    BAYESIAN              // Bayesian optimization
};

/**
 * Structure for optimization criteria
 */
struct OptimizationCriteria {
    enum class ObjectiveType { MAXIMIZE, MINIMIZE, TARGET, CONSTRAINT };

    struct Objective {
        std::string name;
        ObjectiveType type;
        double weight;
        double target_value;      // For TARGET type
        double constraint_value;  // For CONSTRAINT type
        std::function<double(const std::map<std::string, double>&)> evaluation_function;
    };

    std::vector<Objective> objectives;

    // Add a maximization objective
    void addMaximizationObjective(
        const std::string& name, double weight,
        std::function<double(const std::map<std::string, double>&)> eval_function)
    {
        Objective obj;
        obj.name = name;
        obj.type = ObjectiveType::MAXIMIZE;
        obj.weight = weight;
        obj.evaluation_function = eval_function;
        objectives.push_back(obj);
    }

    // Add a minimization objective
    void addMinimizationObjective(
        const std::string& name, double weight,
        std::function<double(const std::map<std::string, double>&)> eval_function)
    {
        Objective obj;
        obj.name = name;
        obj.type = ObjectiveType::MINIMIZE;
        obj.weight = weight;
        obj.evaluation_function = eval_function;
        objectives.push_back(obj);
    }

    // Add a target objective
    void addTargetObjective(
        const std::string& name, double weight, double target_value,
        std::function<double(const std::map<std::string, double>&)> eval_function)
    {
        Objective obj;
        obj.name = name;
        obj.type = ObjectiveType::TARGET;
        obj.weight = weight;
        obj.target_value = target_value;
        obj.evaluation_function = eval_function;
        objectives.push_back(obj);
    }

    // Add a constraint
    void addConstraint(const std::string& name, double constraint_value,
                       std::function<double(const std::map<std::string, double>&)> eval_function)
    {
        Objective obj;
        obj.name = name;
        obj.type = ObjectiveType::CONSTRAINT;
        obj.weight = 1.0;
        obj.constraint_value = constraint_value;
        obj.evaluation_function = eval_function;
        objectives.push_back(obj);
    }
};

/**
 * Structure for clinical data point
 */
struct ClinicalDataPoint {
    double dose;                 // Radiation dose in Gy
    double survival_fraction;    // Experimental survival fraction
    double standard_error;       // Standard error of measurement
    std::string cell_line;       // Cell line identifier
    std::string radiation_type;  // Radiation type
    double let;                  // LET in keV/µm
    double dose_rate;            // Dose rate in Gy/min
    double oxygen_level;         // Oxygen level

    // Other experimental conditions
    std::map<std::string, double> additional_params;
};

/**
 * Configuration for parameter optimization
 */
struct ParameterOptimizerConfig {
    OptimizationMethod method = OptimizationMethod::SIMULATED_ANNEALING;
    int max_iterations = 1000;
    double convergence_threshold = 1e-6;
    int num_parallel_evaluations = 4;
    bool use_quantum_parameters = true;
    int random_seed = 42;
    bool verbose = true;

    // Method-specific parameters
    struct {
        int grid_points_per_dimension = 10;
    } grid_search;

    struct {
        double learning_rate = 0.01;
        double momentum = 0.9;
    } gradient_descent;

    struct {
        double initial_temperature = 10.0;
        double cooling_rate = 0.95;
    } simulated_annealing;

    struct {
        int population_size = 50;
        double mutation_rate = 0.1;
        double crossover_rate = 0.8;
    } genetic_algorithm;

    struct {
        int num_particles = 30;
        double inertia = 0.7;
        double cognitive = 1.5;
        double social = 1.5;
    } particle_swarm;

    struct {
        int num_initial_points = 10;
        std::string acquisition_function = "expected_improvement";
    } bayesian;
};

/**
 * Result of parameter optimization
 */
struct OptimizationResult {
    // Optimized parameters
    std::map<std::string, double> parameters;

    // Objective values
    std::map<std::string, double> objective_values;

    // Overall fitness
    double fitness;

    // Optimization trajectory
    std::vector<std::map<std::string, double>> parameter_history;
    std::vector<double> fitness_history;

    // Convergence metrics
    bool converged;
    int iterations;
    double final_gradient_norm;

    // Parameter sensitivity
    std::map<std::string, double> parameter_sensitivity;
};

/**
 * Class for parameter optimization
 */
class ParameterOptimizer {
   public:
    /**
     * Constructor with configuration
     */
    explicit ParameterOptimizer(const ParameterOptimizerConfig& config);

    /**
     * Add an optimizable parameter
     * @param parameter Optimizable parameter
     */
    void addParameter(const OptimizableParameter& parameter);

    /**
     * Set optimization criteria
     * @param criteria Optimization criteria
     */
    void setOptimizationCriteria(const OptimizationCriteria& criteria);

    /**
     * Set clinical data for fitting
     * @param clinical_data Vector of clinical data points
     */
    void setClinicalData(const std::vector<ClinicalDataPoint>& clinical_data);

    /**
     * Optimize parameters against clinical data
     * @param initial_biosystem Initial biological system
     * @return Optimization result
     */
    OptimizationResult optimizeParameters(const BiologicalSystemExtended& initial_biosystem);

    /**
     * Optimize parameters with custom objective function
     * @param objective_function Function that evaluates parameter fitness
     * @param initial_parameters Initial parameter values
     * @return Optimization result
     */
    OptimizationResult optimizeWithCustomObjective(
        std::function<double(const std::map<std::string, double>&)> objective_function,
        const std::map<std::string, double>& initial_parameters);

    /**
     * Calculate parameter sensitivity
     * @param parameters Parameter values
     * @param objective_function Objective function
     * @return Map of parameter name to sensitivity value
     */
    std::map<std::string, double> calculateParameterSensitivity(
        const std::map<std::string, double>& parameters,
        std::function<double(const std::map<std::string, double>&)> objective_function);

    /**
     * Create default quantum parameters for optimization
     * @return Vector of default quantum parameters to optimize
     */
    static std::vector<OptimizableParameter> createDefaultQuantumParameters();

    /**
     * Create default biological parameters for optimization
     * @return Vector of default biological parameters to optimize
     */
    static std::vector<OptimizableParameter> createDefaultBiologicalParameters();

    /**
     * Create default repair parameters for optimization
     * @return Vector of default repair parameters to optimize
     */
    static std::vector<OptimizableParameter> createDefaultRepairParameters();

    /**
     * Create optimization criteria for survival curve fitting
     * @return Optimization criteria for fitting to survival data
     */
    static OptimizationCriteria createSurvivalCurveFittingCriteria();

    /**
     * Create optimization criteria for therapeutic ratio
     * @return Optimization criteria for maximizing therapeutic ratio
     */
    static OptimizationCriteria createTherapeuticRatioCriteria();

    /**
     * Create optimization criteria for treatment planning
     * @return Optimization criteria for treatment planning
     */
    static OptimizationCriteria createTreatmentPlanningCriteria();

   private:
    // Configuration
    ParameterOptimizerConfig config_;

    // Parameters to optimize
    std::vector<OptimizableParameter> parameters_;

    // Optimization criteria
    OptimizationCriteria criteria_;

    // Clinical data
    std::vector<ClinicalDataPoint> clinical_data_;

    // Calculate fitness from parameter values
    double calculateFitness(const std::map<std::string, double>& parameters);

    // Check if parameters are within constraints
    bool checkConstraints(const std::map<std::string, double>& parameters);

    // Calculate survival curve fitness against clinical data
    double calculateSurvivalCurveFitness(const std::map<std::string, double>& parameters,
                                         const BiologicalSystemExtended& biosystem);

    // Implementation of grid search
    OptimizationResult runGridSearch(
        std::function<double(const std::map<std::string, double>&)> objective_function,
        const std::map<std::string, double>& initial_parameters);

    // Implementation of gradient descent
    OptimizationResult runGradientDescent(
        std::function<double(const std::map<std::string, double>&)> objective_function,
        const std::map<std::string, double>& initial_parameters);

    // Implementation of simulated annealing
    OptimizationResult runSimulatedAnnealing(
        std::function<double(const std::map<std::string, double>&)> objective_function,
        const std::map<std::string, double>& initial_parameters);

    // Implementation of genetic algorithm
    OptimizationResult runGeneticAlgorithm(
        std::function<double(const std::map<std::string, double>&)> objective_function,
        const std::map<std::string, double>& initial_parameters);

    // Implementation of particle swarm optimization
    OptimizationResult runParticleSwarm(
        std::function<double(const std::map<std::string, double>&)> objective_function,
        const std::map<std::string, double>& initial_parameters);

    // Implementation of Bayesian optimization
    OptimizationResult runBayesianOptimization(
        std::function<double(const std::map<std::string, double>&)> objective_function,
        const std::map<std::string, double>& initial_parameters);

    // Perturb parameters (for stochastic methods)
    std::map<std::string, double> perturbParameters(const std::map<std::string, double>& parameters,
                                                    double temperature);

    // Generate random parameter values within constraints
    std::map<std::string, double> generateRandomParameters();

    // Calculate numerical gradient
    std::map<std::string, double> calculateNumericalGradient(
        std::function<double(const std::map<std::string, double>&)> objective_function,
        const std::map<std::string, double>& parameters, double epsilon = 1e-6);

    // Update parameter values based on gradient
    std::map<std::string, double> updateParametersWithGradient(
        const std::map<std::string, double>& parameters,
        const std::map<std::string, double>& gradient, double learning_rate);

    // Apply constraints to parameter values
    std::map<std::string, double> applyConstraints(const std::map<std::string, double>& parameters);
};

/**
 * Optimize quantum parameters against clinical data
 * @param clinical_data Clinical data points
 * @param initial_biosystem Initial biological system
 * @param criteria Optimization criteria
 * @return Optimized parameters
 */
std::map<std::string, double> optimizeParameters(
    const std::vector<ClinicalDataPoint>& clinical_data,
    const BiologicalSystemExtended& initial_biosystem, const OptimizationCriteria& criteria);

/**
 * Calculate parameter sensitivity
 * @param biosystem Biological system
 * @param radiation_dose Radiation dose in Gy
 * @return Map of parameter name to sensitivity
 */
std::map<std::string, double> calculateParameterSensitivity(
    const BiologicalSystemExtended& biosystem, double radiation_dose);

}  // namespace quantum_enhanced
}  // namespace healthcare
}  // namespace rad_ml

#endif  // RAD_ML_HEALTHCARE_QUANTUM_ENHANCED_PARAMETER_OPTIMIZER_HPP
