#include <iostream>
#include <iomanip>
#include <vector>
#include "rad_ml/healthcare/chemotherapy/drug_target_interaction.hpp"

using namespace rad_ml::healthcare;
using namespace rad_ml::healthcare::chemotherapy;

int main() {
    std::cout << "Testing cell size effects on quantum binding\n";
    std::cout << std::string(50, '=') << std::endl;

    // Create drug (Cisplatin)
    ChemotherapeuticAgent cisplatin = createStandardDrug("Cisplatin");

    // Create tissue
    BiologicalSystem tissue;
    tissue.type = SOFT_TISSUE;
    tissue.water_content = 0.70;
    tissue.cell_density = 2.0e5;
    tissue.effective_barrier = 0.35;
    tissue.repair_rate = 0.4;
    tissue.radiosensitivity = 1.0;

    // Create two interaction models - with and without quantum effects
    DrugTargetInteractionModel quantum_model(cisplatin, tissue);
    DrugQuantumConfig quantum_config;
    quantum_config.enable_tunneling_effects = true;
    quantum_config.enable_binding_enhancement = true;
    quantum_config.enable_zero_point_contribution = true;
    quantum_config.temperature = 310.0;
    quantum_model.setDrugQuantumConfig(quantum_config);

    DrugTargetInteractionModel classical_model(cisplatin, tissue);
    DrugQuantumConfig classical_config;
    classical_config.enable_tunneling_effects = false;
    classical_config.enable_binding_enhancement = false;
    classical_config.enable_zero_point_contribution = false;
    classical_model.setDrugQuantumConfig(classical_config);

    // Test different cell sizes
    std::vector<double> cell_sizes = {5.0, 8.0, 10.0, 15.0, 20.0, 25.0};

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Cell Size (µm) | Classical Binding | Quantum Binding | Enhancement (%)\n";
    std::cout << std::string(70, '-') << std::endl;

    for (double size : cell_sizes) {
        double classical_binding = classical_model.calculateQuantumEnhancedBinding(310.0, size, 0.5);
        double quantum_binding = quantum_model.calculateQuantumEnhancedBinding(310.0, size, 0.5);

        // Calculate enhancement
        double enhancement = (quantum_binding / classical_binding - 1.0) * 100.0;

        // Print results
        std::cout << std::setw(13) << size << " | "
                  << std::setw(17) << classical_binding << " | "
                  << std::setw(15) << quantum_binding << " | "
                  << std::setw(14) << enhancement << std::endl;
    }

    std::cout << "\nTest completed successfully!\n";
    return 0;
}
