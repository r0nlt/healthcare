#include <iostream>
#include <map>
#include <rad_ml/healthcare/bio_quantum_integration.hpp>
#include <string>
#include <vector>

namespace rad_ml {
namespace healthcare {

// Radiation therapy dose planning model
class RadiationTherapyModel {
   private:
    BiologicalSystem patientTissue;
    BioQuantumConfig quantumConfig;
    double roomTemperature;

   public:
    RadiationTherapyModel(TissueType type, double waterContent, double repairRate,
                          double radiosensitivity)
    {
        // Initialize biological system based on patient tissue properties
        patientTissue.type = type;
        patientTissue.water_content = waterContent;
        patientTissue.repair_rate = repairRate;
        patientTissue.radiosensitivity = radiosensitivity;
        patientTissue.cell_density = calculateCellDensityForTissue(type);
        patientTissue.effective_barrier = calculateEffectiveBarrierForTissue(type);

        // Default quantum configuration
        quantumConfig.enable_quantum_corrections = true;
        quantumConfig.force_quantum_corrections = false;
        quantumConfig.temperature_threshold = 305.0;   // K
        quantumConfig.cell_size_threshold = 12.0;      // µm
        quantumConfig.radiation_dose_threshold = 2.0;  // Gy
        quantumConfig.dna_damage_threshold = 0.2;
        quantumConfig.radiolysis_factor = 0.3;

        // Default room temperature (Kelvin)
        roomTemperature = 295.0;  // 22°C
    }

    void setTemperature(double kelvin) { roomTemperature = kelvin; }

    void enableQuantumCorrections(bool enable)
    {
        quantumConfig.enable_quantum_corrections = enable;
    }

    void forceQuantumCorrections(bool force) { quantumConfig.force_quantum_corrections = force; }

    // Calculate cell density based on tissue type
    double calculateCellDensityForTissue(TissueType type)
    {
        switch (type) {
            case SOFT_TISSUE:
                return 1.0e6;  // 1 million cells per mm³
            case BONE:
                return 0.5e6;  // Less cellular
            case NERVE:
                return 0.8e6;  // Neural tissue
            case MUSCLE:
                return 1.2e6;  // Muscle tissue
            case BLOOD:
                return 5.0e6;  // Blood cells (higher density)
            default:
                return 1.0e6;  // Default
        }
    }

    // Calculate effective energy barrier based on tissue type
    double calculateEffectiveBarrierForTissue(TissueType type)
    {
        switch (type) {
            case SOFT_TISSUE:
                return 0.3;  // eV
            case BONE:
                return 0.5;  // Higher for dense tissue
            case NERVE:
                return 0.25;  // Lower for neural tissue
            case MUSCLE:
                return 0.35;  // Muscle tissue
            case BLOOD:
                return 0.2;  // Blood cells (lower barrier)
            default:
                return 0.3;  // Default
        }
    }

    // Predict radiation damage for a given dose
    CellularDamageDistribution predictRadiationDamage(double dose_Gy, double cell_size_um)
    {
        // Initialize damage distribution based on standard radiobiological models
        CellularDamageDistribution baseDamage;

        // Initialize different damage types
        baseDamage["dna_strand_break"] = dose_Gy * 0.2 * patientTissue.radiosensitivity;
        baseDamage["protein_damage"] = dose_Gy * 0.15 * patientTissue.water_content;
        baseDamage["membrane_lipid_peroxidation"] = dose_Gy * 0.1 * patientTissue.water_content;
        baseDamage["mitochondrial_damage"] = dose_Gy * 0.05 * patientTissue.radiosensitivity;
        baseDamage["oxidative_stress"] = dose_Gy * 0.25 * patientTissue.water_content;

        // Apply quantum corrections to the base damage model
        CellularDamageDistribution correctedDamage = applyQuantumCorrectionsToBiologicalSystem(
            baseDamage, patientTissue, roomTemperature, cell_size_um, dose_Gy, quantumConfig);

        return correctedDamage;
    }

    // Calculate therapeutic ratio (tumor damage / normal tissue damage)
    double calculateTherapeuticRatio(double tumor_dose_Gy, double normal_tissue_dose_Gy,
                                     double tumor_cell_size_um, double normal_cell_size_um)
    {
        // Save current tissue state
        BiologicalSystem originalTissue = patientTissue;

        // Set up tumor tissue (typically more radiosensitive, less repair capability)
        patientTissue.radiosensitivity = 1.5 * originalTissue.radiosensitivity;
        patientTissue.repair_rate = 0.5 * originalTissue.repair_rate;

        // Calculate tumor damage
        CellularDamageDistribution tumorDamage =
            predictRadiationDamage(tumor_dose_Gy, tumor_cell_size_um);
        double totalTumorDamage = calculateTotalDamage(tumorDamage);

        // Restore original tissue and calculate normal tissue damage
        patientTissue = originalTissue;
        CellularDamageDistribution normalTissueDamage =
            predictRadiationDamage(normal_tissue_dose_Gy, normal_cell_size_um);
        double totalNormalTissueDamage = calculateTotalDamage(normalTissueDamage);

        // Calculate therapeutic ratio (higher is better)
        if (totalNormalTissueDamage < 0.001) {
            return 100.0;  // Avoid division by very small numbers
        }

        double therapeuticRatio = totalTumorDamage / totalNormalTissueDamage;
        return therapeuticRatio;
    }

    // Sum up total damage across all damage types
    double calculateTotalDamage(const CellularDamageDistribution& damage)
    {
        double total = 0.0;

        for (const auto& damage_pair : damage) {
            total += damage_pair.second;
        }

        return total;
    }

    // Optimize radiation dose for maximum therapeutic ratio
    double optimizeRadiationDose(double min_dose_Gy, double max_dose_Gy, double step_Gy,
                                 double tumor_cell_size_um, double normal_cell_size_um)
    {
        double bestDose = min_dose_Gy;
        double bestRatio = 0.0;

        for (double dose = min_dose_Gy; dose <= max_dose_Gy; dose += step_Gy) {
            // Assume normal tissue receives 30% of tumor dose (simplified model)
            double normalTissueDose = 0.3 * dose;

            double ratio = calculateTherapeuticRatio(dose, normalTissueDose, tumor_cell_size_um,
                                                     normal_cell_size_um);

            if (ratio > bestRatio) {
                bestRatio = ratio;
                bestDose = dose;
            }
        }

        return bestDose;
    }

    // Print detailed damage report
    void printDamageReport(const CellularDamageDistribution& damage)
    {
        std::cout << "Cellular Damage Report:" << std::endl;
        std::cout << "---------------------" << std::endl;

        for (const auto& damage_pair : damage) {
            std::string damage_type = damage_pair.first;
            double damage_value = damage_pair.second;

            std::cout << damage_type << ": " << damage_value << std::endl;
        }

        std::cout << "Total damage: " << calculateTotalDamage(damage) << std::endl;
        std::cout << "---------------------" << std::endl;
    }
};

// Example usage of the radiation therapy model
void demonstrateRadiationTherapyPlanning()
{
    // Create model for lung tissue (soft tissue with high water content)
    RadiationTherapyModel lungModel(SOFT_TISSUE, 0.8, 0.3, 1.2);

    // Treatment parameters
    double dose_Gy = 2.0;            // 2 Gy per fraction
    double tumor_cell_size = 8.0;    // μm
    double normal_cell_size = 12.0;  // μm

    // Compare with and without quantum corrections
    std::cout << "Standard Radiobiological Model:" << std::endl;
    lungModel.enableQuantumCorrections(false);
    CellularDamageDistribution standardDamage =
        lungModel.predictRadiationDamage(dose_Gy, tumor_cell_size);
    lungModel.printDamageReport(standardDamage);

    std::cout << "\nQuantum-Corrected Model:" << std::endl;
    lungModel.enableQuantumCorrections(true);
    CellularDamageDistribution quantumDamage =
        lungModel.predictRadiationDamage(dose_Gy, tumor_cell_size);
    lungModel.printDamageReport(quantumDamage);

    // Calculate therapeutic ratio
    double ratio = lungModel.calculateTherapeuticRatio(dose_Gy, 0.3 * dose_Gy, tumor_cell_size,
                                                       normal_cell_size);
    std::cout << "\nTherapeutic ratio: " << ratio << std::endl;

    // Optimize dose
    double optimalDose =
        lungModel.optimizeRadiationDose(1.0, 5.0, 0.5, tumor_cell_size, normal_cell_size);
    std::cout << "Optimal dose: " << optimalDose << " Gy" << std::endl;
}

}  // namespace healthcare
}  // namespace rad_ml

// Main function to demonstrate the radiation therapy model
int main()
{
    rad_ml::healthcare::demonstrateRadiationTherapyPlanning();
    return 0;
}
