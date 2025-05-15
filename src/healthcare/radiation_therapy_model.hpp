#pragma once

#include <map>
#include <rad_ml/healthcare/bio_quantum_integration.hpp>
#include <string>

namespace rad_ml {
namespace healthcare {

// Radiation therapy dose planning model
class RadiationTherapyModel {
   private:
    BiologicalSystem patientTissue;
    BioQuantumConfig quantumConfig;
    double roomTemperature;

    // Helper functions
    double calculateCellDensityForTissue(TissueType type);
    double calculateEffectiveBarrierForTissue(TissueType type);
    double calculateTotalDamage(const CellularDamageDistribution& damage);

   public:
    // Constructor
    RadiationTherapyModel(TissueType type, double waterContent, double repairRate,
                          double radiosensitivity);

    // Configuration methods
    void setTemperature(double kelvin);
    void enableQuantumCorrections(bool enable);
    void forceQuantumCorrections(bool force);

    // Radiation damage prediction
    CellularDamageDistribution predictRadiationDamage(double dose_Gy, double cell_size_um);

    // Treatment planning methods
    double calculateTherapeuticRatio(double tumor_dose_Gy, double normal_tissue_dose_Gy,
                                     double tumor_cell_size_um, double normal_cell_size_um);

    double optimizeRadiationDose(double min_dose_Gy, double max_dose_Gy, double step_Gy,
                                 double tumor_cell_size_um, double normal_cell_size_um);

    // Reporting
    void printDamageReport(const CellularDamageDistribution& damage);
};

// Demo function
void demonstrateRadiationTherapyPlanning();

}  // namespace healthcare
}  // namespace rad_ml
