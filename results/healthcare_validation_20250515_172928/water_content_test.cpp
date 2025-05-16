#include <iostream>
#include <iomanip>
#include <vector>
#include "rad_ml/healthcare/chemotherapy/chemo_quantum_model.hpp"

using namespace rad_ml::healthcare;
using namespace rad_ml::healthcare::chemotherapy;

int main() {
    std::cout << "Testing drug diffusion with varying water content\n";
    std::cout << std::string(50, '=') << std::endl;

    // Create standard drug
    ChemotherapeuticAgent cisplatin = createStandardDrug("Cisplatin");

    // Test different water contents
    std::vector<double> water_contents = {0.3, 0.5, 0.7, 0.9};
    double membrane_thickness = 8.0;  // nm

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Water Content | Classical Diff | Quantum Diff | Enhancement (%)\n";
    std::cout << std::string(70, '-') << std::endl;

    for (double water : water_contents) {
        BiologicalSystem tissue;
        tissue.type = SOFT_TISSUE;
        tissue.water_content = water;
        tissue.cell_density = 2.0e5;
        tissue.effective_barrier = 0.35;
        tissue.repair_rate = 0.4;
        tissue.radiosensitivity = 1.2;

        // Classical diffusion (no quantum effects)
        ChemotherapeuticAgent classical_drug = cisplatin;
        classical_drug.quantum_tunneling_factor = 0.0;
        double classical_diff = calculateQuantumCorrectedDiffusion(
            classical_drug, tissue, 310.0, membrane_thickness);

        // Quantum-corrected diffusion
        double quantum_diff = calculateQuantumCorrectedDiffusion(
            cisplatin, tissue, 310.0, membrane_thickness);

        // Calculate enhancement
        double enhancement = (quantum_diff / classical_diff - 1.0) * 100.0;

        // Print results
        std::cout << std::setw(12) << water << " | "
                  << std::setw(14) << classical_diff << " | "
                  << std::setw(12) << quantum_diff << " | "
                  << std::setw(14) << enhancement << std::endl;
    }

    std::cout << "\nTest completed successfully!\n";
    return 0;
}
