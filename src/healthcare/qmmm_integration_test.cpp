/************************************************************
 * QM/MM Integration for Radiation-Based Healthcare Test Program
 * Demonstrates the integration of quantum mechanical and molecular
 * mechanical approaches for chemoradiation modeling
 ************************************************************/

#include <iostream>
#include <memory>
#include <rad_ml/crossdomain/qft_bridge.hpp>
#include <rad_ml/healthcare/bio_quantum_integration.hpp>
#include <rad_ml/healthcare/chemotherapy/chemo_quantum_model.hpp>
#include <rad_ml/healthcare/molecule.hpp>
#include <rad_ml/healthcare/qmmm_simulation.hpp>
#include <string>
#include <vector>

// Don't use namespaces to avoid ambiguity
// using namespace rad_ml::healthcare;
// using namespace rad_ml::healthcare::chemotherapy;

namespace rh = rad_ml::healthcare;
namespace rhc = rad_ml::healthcare::chemotherapy;

// Example main program to demonstrate QM/MM integration
int main(int argc, char* argv[])
{
    std::cout << "QM/MM Integration for Radiation-Based Healthcare Quantum Modeling\n";
    std::cout << "================================================================\n\n";

    // Process command line arguments for drug selection
    std::string drugName = "CISPLATIN";  // Default drug
    double radiationDose = 2.0;          // Gy, default dose

    if (argc > 1) {
        drugName = argv[1];
    }
    if (argc > 2) {
        radiationDose = std::stod(argv[2]);
    }

    // Create drug molecule using fully qualified name
    std::cout << "Creating drug molecule: " << drugName << std::endl;
    std::shared_ptr<rh::Molecule> drug = rh::createStandardDrug(drugName);

    std::cout << "Drug properties:\n";
    std::cout << "- Name: " << drug->getName() << std::endl;
    std::cout << "- Molecular weight: " << drug->getMolecularWeight() << " Da" << std::endl;
    std::cout << "- LogP: " << drug->getLogP() << std::endl;
    std::cout << "- H-bond donors: " << drug->getHBondDonors() << std::endl;
    std::cout << "- H-bond acceptors: " << drug->getHBondAcceptors() << std::endl;
    std::cout << "- Contains metals: " << (drug->containsMetals() ? "Yes" : "No") << std::endl;
    std::cout << "- Atom count: " << drug->getAtomCount() << std::endl;
    std::cout << std::endl;

    // Create biological target (DNA)
    std::cout << "Creating biological target (DNA)\n";
    rh::BiologicalSystem dnaTarget;
    dnaTarget.type = rh::NUCLEIC_ACID;
    dnaTarget.water_content = 0.7;
    dnaTarget.cell_density = 1.2e6;  // cells per mm³
    dnaTarget.repair_rate = 0.3;
    dnaTarget.radiosensitivity = 1.2;
    std::cout << std::endl;

    // Run the QM/MM workflow
    std::cout << "Running QM/MM integrated workflow\n";
    std::cout << "--------------------------------\n";
    rh::QMIntegratedWorkflow workflow;
    workflow.processDrugCandidate(*drug, dnaTarget, radiationDose);
    std::cout << std::endl;

    // Use the DrugSpecificQuantumModel to calculate synergy
    std::cout << "Computing drug-specific quantum effects\n";
    std::cout << "-------------------------------------\n";
    rhc::DrugSpecificQuantumModel drugModel;

    // Calculate binding with quantum effects
    double binding = drugModel.calculateQMEnhancedBinding(drugName, 310.0, true);
    double classicalBinding = drugModel.calculateQMEnhancedBinding(drugName, 310.0, false);

    std::cout << "Binding affinity:\n";
    std::cout << "- With quantum effects: " << binding << std::endl;
    std::cout << "- Without quantum effects: " << classicalBinding << std::endl;
    std::cout << "- Quantum enhancement: " << (binding / classicalBinding) - 1.0 << " (factor)\n";
    std::cout << std::endl;

    // Calculate synergy for different schedules
    std::cout << "Chemoradiation synergy (dose: " << radiationDose << " Gy):\n";
    double concurrentSynergy =
        drugModel.calculateChemoRadiationSynergy(drugName, radiationDose, rhc::CONCURRENT, 310.0);
    double radFirstSynergy = drugModel.calculateChemoRadiationSynergy(drugName, radiationDose,
                                                                      rhc::RADIATION_FIRST, 310.0);
    double drugFirstSynergy =
        drugModel.calculateChemoRadiationSynergy(drugName, radiationDose, rhc::DRUG_FIRST, 310.0);

    std::cout << "- Concurrent treatment: " << concurrentSynergy << std::endl;
    std::cout << "- Radiation first: " << radFirstSynergy << std::endl;
    std::cout << "- Drug first: " << drugFirstSynergy << std::endl;
    std::cout << std::endl;

    // Determine optimal schedule
    std::cout << "Optimal treatment schedule: ";
    if (concurrentSynergy >= radFirstSynergy && concurrentSynergy >= drugFirstSynergy) {
        std::cout << "CONCURRENT\n";
    }
    else if (radFirstSynergy >= concurrentSynergy && radFirstSynergy >= drugFirstSynergy) {
        std::cout << "RADIATION FIRST\n";
    }
    else {
        std::cout << "DRUG FIRST\n";
    }

    std::cout << "\nQM/MM integration test completed successfully.\n";
    return 0;
}
