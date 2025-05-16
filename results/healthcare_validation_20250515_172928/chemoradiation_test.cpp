#include <iostream>
#include <iomanip>
#include <vector>
#include "rad_ml/healthcare/chemotherapy/chemoradiation_synergy.hpp"

using namespace rad_ml::healthcare;
using namespace rad_ml::healthcare::chemotherapy;

int main() {
    std::cout << "Testing chemoradiation sequencing effects\n";
    std::cout << std::string(50, '=') << std::endl;

    // Create drugs to test
    std::vector<std::string> drug_names = {
        "Cisplatin", "Paclitaxel", "Doxorubicin", "Fluorouracil"
    };

    // Create tissue (lung tumor)
    BiologicalSystem tissue;
    tissue.type = SOFT_TISSUE;
    tissue.water_content = 0.70;
    tissue.cell_density = 2.0e5;
    tissue.effective_barrier = 0.35;
    tissue.repair_rate = 0.4;
    tissue.radiosensitivity = 1.2;

    // Test parameters
    double radiation_dose = 2.0;  // Gy
    double drug_concentration = 0.5;  // μmol/L
    std::vector<double> time_gaps = {0.0, 6.0, 12.0, 24.0};  // hours

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Drug        | Sequence      | Time Gap | Efficacy | Survival | Synergy\n";
    std::cout << std::string(75, '-') << std::endl;

    for (const auto& drug_name : drug_names) {
        ChemotherapeuticAgent drug = createStandardDrug(drug_name);
        ChemoradiationSynergyModel model(drug, tissue);

        // Test different sequences
        for (const auto& sequence : {CONCURRENT, RADIATION_FIRST, DRUG_FIRST}) {
            std::string seq_name;
            if (sequence == CONCURRENT) seq_name = "Concurrent";
            else if (sequence == RADIATION_FIRST) seq_name = "Radiation First";
            else seq_name = "Drug First";

            for (double gap : time_gaps) {
                // Skip time gaps for concurrent
                if (sequence == CONCURRENT && gap > 0.0) continue;

                double efficacy = model.predictEfficacy(
                    radiation_dose, drug_concentration, sequence, gap);

                double survival = model.calculateSurvivalFraction(
                    radiation_dose, drug_concentration, sequence, gap);

                // Calculate synergy (efficacy beyond additive effects)
                double radiation_only = radiation_dose * 0.3;  // simplified
                double drug_only = drug_concentration / (drug_concentration + 0.5);
                double synergy = efficacy - (radiation_only + drug_only);

                std::cout << std::setw(12) << drug_name << " | "
                          << std::setw(14) << seq_name << " | "
                          << std::setw(8) << gap << " | "
                          << std::setw(8) << efficacy << " | "
                          << std::setw(8) << survival << " | "
                          << std::setw(7) << synergy << std::endl;
            }
        }
        std::cout << std::string(75, '-') << std::endl;
    }

    std::cout << "\nTest completed successfully!\n";
    return 0;
}
