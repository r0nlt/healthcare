#pragma once

#include <map>
#include <string>
#include <vector>

#include "rad_ml/healthcare/bio_quantum_integration.hpp"
#include "rad_ml/healthcare/chemotherapy/chemo_quantum_model.hpp"

namespace rad_ml {
namespace healthcare {
namespace chemotherapy {

class DrugTargetInteractionModel {
   public:
    // Constructor with drug and tissue
    DrugTargetInteractionModel(const ChemotherapeuticAgent& drug, const BiologicalSystem& tissue);

    // Calculate quantum-enhanced binding probability
    double calculateQuantumEnhancedBinding(double temperature, double cell_size,
                                           double drug_concentration);

    // Apply quantum corrections to classical binding
    double applyQuantumCorrections(double classical_binding_probability, double temperature,
                                   double target_barrier_height);

    // Model the time-dependent binding dynamics
    std::vector<double> modelBindingDynamics(double initial_concentration, double time_hours,
                                             double time_step_hours);

    // Calculate intracellular drug concentration
    double calculateIntracellularConcentration(double extracellular_concentration,
                                               double time_hours);

    // Calculate drug transport across membrane
    double calculateMembraneDiffusion(double concentration_gradient, double membrane_thickness_nm,
                                      double temperature);

    // Set drug quantum configuration
    void setDrugQuantumConfig(const DrugQuantumConfig& config);

    // Get drug response for specific tissue
    ChemoDrugResponse getDrugResponse(double extracellular_concentration, double time_hours,
                                      double temperature);

   private:
    ChemotherapeuticAgent drug_;
    BiologicalSystem tissue_;
    DrugQuantumConfig quantum_config_;

    // Cache calculated parameters
    DrugQuantumParameters quantum_parameters_;

    // Tunneling probability through cellular barriers
    double calculateTunnelingProbability(double barrier_height, double barrier_width,
                                         double temperature);

    // Quantum correction factor based on molecular properties
    double quantumCorrectionFactor(double molecular_weight, double temperature);

    // Calculate binding energy for specific target
    double getTargetBindingEnergy(const std::string& target);

    // Initialize quantum parameters
    void initializeQuantumParameters(double temperature);

    // Calculate classical binding using standard models
    double calculateClassicalBinding(double concentration, double binding_energy,
                                     double temperature);

    // Calculate cellular damage from bound drug
    std::map<std::string, double> calculateCellularDamage(double bound_fraction,
                                                          double intracellular_concentration);
};

}  // namespace chemotherapy
}  // namespace healthcare
}  // namespace rad_ml
