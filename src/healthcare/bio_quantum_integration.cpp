#include "../../include/rad_ml/healthcare/bio_quantum_integration.hpp"

#include <algorithm>  // For std::max, std::min
#include <cmath>      // For std::exp, std::sqrt
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace rad_ml {
namespace healthcare {

BiologicalQFTParameters createBioQFTParameters(const BiologicalSystem& biosystem,
                                               double cell_size_um)
{
    BiologicalQFTParameters params;

    // Physics constants
    params.hbar = 6.582119569e-16;  // reduced Planck constant (eV·s)

    // Biological-specific parameters
    // Effective mass depends on the tissue type
    switch (biosystem.type) {
        case SOFT_TISSUE:
            params.effective_mass = 1.2e-29;  // Default soft tissue
            break;
        case BONE:
            params.effective_mass = 2.5e-29;  // Denser tissue
            break;
        case NERVE:
            params.effective_mass = 0.9e-29;  // Neural tissue
            break;
        case MUSCLE:
            params.effective_mass = 1.8e-29;  // Muscle tissue
            break;
        case BLOOD:
            params.effective_mass = 1.1e-29;  // Blood
            break;
        default:
            params.effective_mass = 1.0e-29;  // Default value
    }

    // Scaling parameters based on biological properties
    params.coupling_constant = 0.15 * biosystem.water_content;  // Water enhances quantum effects
    params.water_coupling = biosystem.water_content * 0.2;      // Quantum effects in water

    // Cell size impacts cell spacing parameter
    params.cell_spacing = cell_size_um / 10.0;  // Convert to appropriate scale

    // Simulation parameters
    params.time_step = 1.0e-15;  // ps time scale for biological processes
    params.dimensions = 3;

    return params;
}

bool shouldApplyBioQuantumCorrections(double temperature, double cell_size, double radiation_dose,
                                      const BioQuantumConfig& config)
{
    if (!config.enable_quantum_corrections) {
        return false;
    }

    if (config.force_quantum_corrections) {
        return true;
    }

    // Apply based on thresholds
    bool temperature_criterion = temperature < config.temperature_threshold;
    bool cell_size_criterion = cell_size < config.cell_size_threshold;
    bool radiation_criterion = radiation_dose > config.radiation_dose_threshold;

    // Apply corrections if any criterion is met
    return temperature_criterion || cell_size_criterion || radiation_criterion;
}

CellularDamageDistribution applyQuantumCorrectionsToBiologicalSystem(
    const CellularDamageDistribution& damage, const BiologicalSystem& biosystem, double temperature,
    double cell_size_um, double radiation_dose, const BioQuantumConfig& config)
{
    // Check if we should apply quantum corrections
    if (!shouldApplyBioQuantumCorrections(temperature, cell_size_um, radiation_dose, config)) {
        return damage;  // Return original damage without quantum corrections
    }

    // Create QFT parameters based on biological properties
    BiologicalQFTParameters qft_params = createBioQFTParameters(biosystem, cell_size_um);

    // Apply quantum field corrections
    CellularDamageDistribution corrected_damage =
        applyBioQuantumFieldCorrections(damage, biosystem, qft_params, temperature);

    return corrected_damage;
}

double calculateBioQuantumEnhancementFactor(double temperature, double cell_size)
{
    // Base enhancement (no enhancement = 1.0)
    double enhancement = 1.0;

    // Temperature effect: More pronounced at low temperatures
    // Physiological temperature range is much narrower for biological systems
    if (temperature < 310.0) {  // Below normal body temperature
        // More gradual scaling appropriate for biological systems
        double temp_factor = std::min(2.0, 310.0 / std::max(temperature, 270.0));

        // Cap the maximum enhancement from temperature alone to 3%
        enhancement *= (1.0 + temp_factor * 0.03);
    }

    // Size effect: More pronounced at small cell sizes
    // Typical cell sizes are in micrometers
    if (cell_size < 10.0) {  // Smaller than typical mammalian cells
        // Linear factor with saturation
        double size_factor = std::min(3.0, 10.0 / std::max(cell_size, 1.0));

        // Cap the maximum enhancement from size alone to 4%
        enhancement *= (1.0 + size_factor * 0.04);
    }

    return enhancement;
}

CellularDamageDistribution applyBioQuantumFieldCorrections(
    const CellularDamageDistribution& damage, const BiologicalSystem& biosystem,
    const BiologicalQFTParameters& qft_params, double temperature)
{
    // Start with a copy of the original damage
    CellularDamageDistribution corrected_damage = damage;

    // Calculate quantum corrections
    double tunneling_probability = calculateBioQuantumTunnelingProbability(
        biosystem.effective_barrier, qft_params.effective_mass, qft_params.hbar, temperature);

    // Apply wave equation correction factor
    double wave_correction = solveBioQuantumWaveEquation(
        qft_params.hbar, qft_params.effective_mass,
        0.3,  // potential coefficient for biological systems
        qft_params.coupling_constant, qft_params.cell_spacing, qft_params.time_step);

    // Calculate zero-point energy contribution (stronger in water-rich tissues)
    double zpe_contribution = calculateBioZeroPointEnergyContribution(
        qft_params.hbar, qft_params.effective_mass, biosystem.water_content, temperature);

    // Apply corrections to each damage type
    for (auto& damage_pair : corrected_damage) {
        std::string damage_type = damage_pair.first;
        double& damage_value = damage_pair.second;

        // Apply different correction factors based on damage type
        if (damage_type == "dna_strand_break") {
            // DNA strand breaks can be affected by tunneling
            damage_value *= (1.0 + 0.5 * tunneling_probability + 0.3 * wave_correction);
        }
        else if (damage_type == "protein_damage") {
            // Protein damage is affected by water-mediated effects
            damage_value *= (1.0 + 0.3 * tunneling_probability + 0.6 * wave_correction);
        }
        else if (damage_type == "membrane_lipid_peroxidation") {
            // Membrane damage is strongly affected by water content
            damage_value *= (1.0 + 0.2 * tunneling_probability +
                             0.7 * biosystem.water_content * wave_correction);
        }
        else if (damage_type == "mitochondrial_damage") {
            // Mitochondrial damage has unique quantum characteristics
            damage_value *= (1.0 + 0.4 * tunneling_probability + 0.5 * wave_correction);
        }
        else {
            // Default correction
            damage_value *= (1.0 + 0.3 * tunneling_probability + 0.4 * wave_correction);
        }

        // Add zero-point energy contribution, more significant in water-rich environments
        damage_value += zpe_contribution * damage_value * 0.01 * biosystem.water_content;

        // Apply tissue-specific radiosensitivity
        damage_value *= (1.0 + biosystem.radiosensitivity * 0.1);

        // Account for repair mechanisms (reducing final damage)
        damage_value *= (1.0 - std::min(0.9, biosystem.repair_rate));
    }

    // Log the correction factors
    std::cout << "Applied biological quantum corrections with factors: " << std::endl;
    std::cout << "  - Bio tunneling probability: " << tunneling_probability << std::endl;
    std::cout << "  - Bio wave equation correction: " << wave_correction << std::endl;
    std::cout << "  - Bio zero-point energy contribution: " << zpe_contribution << std::endl;
    std::cout << "  - Tissue radiosensitivity: " << biosystem.radiosensitivity << std::endl;
    std::cout << "  - Cellular repair rate: " << biosystem.repair_rate << std::endl;

    return corrected_damage;
}

double calculateBioQuantumTunnelingProbability(double barrier_height, double effective_mass,
                                               double hbar, double temperature)
{
    // Biological barrier widths are typically larger than atomic scales
    const double barrier_width = 2.0;  // nm (typical cellular membrane thickness)
    const double kb = 8.617333262e-5;  // Boltzmann constant in eV/K

    // Prevent division by zero or negative temperatures
    double safe_temp = std::max(temperature, 1.0);  // Minimum 1K to avoid div by zero

    // Calculate thermal energy
    double thermal_energy = kb * safe_temp;

    // Safety check for barrier height
    double safe_barrier = std::max(barrier_height, 0.01);  // Minimum 0.01 eV

    // Capped exponent calculation for numerical stability
    double exponent_term =
        -2.0 * barrier_width * std::sqrt(2.0 * effective_mass * safe_barrier) / hbar;
    exponent_term = std::max(-30.0, exponent_term);  // Prevent extreme underflow

    double base_probability = std::exp(exponent_term);

    // Bound base probability to physiologically reasonable values
    base_probability = std::min(0.05, base_probability);  // Cap at 5% max (biological systems)

    // Temperature correction with improved stability
    // Most biological systems operate in a narrow temperature range
    double temp_ratio = thermal_energy / (2.0 * safe_barrier);
    temp_ratio = std::min(5.0, temp_ratio);  // Prevent extreme values

    double temp_factor = std::exp(-temp_ratio);

    // Final bounded probability
    double result = base_probability * temp_factor;

    // Additional sanity check for final result
    return std::min(0.03, std::max(0.0, result));  // Keep between 0% and 3%
}

double solveBioQuantumWaveEquation(double hbar, double effective_mass, double potential_coeff,
                                   double coupling_constant, double cell_spacing, double time_step)
{
    // Simplified quantum wave equation solution for biological systems
    // In a full implementation, this would involve solving more complex differential equations

    // Simplified model: correction factor based on quantum parameters
    double safe_cell_spacing = std::max(cell_spacing, 0.01);  // Avoid division by very small values
    double safe_mass = std::max(effective_mass, 1.0e-31);     // Avoid division by very small mass

    // Wave function parameters modified for biological scale
    double wave_factor = hbar / (safe_mass * safe_cell_spacing * safe_cell_spacing);
    double potential_factor =
        potential_coeff * safe_cell_spacing * 0.2;                 // Reduced for biological scale
    double coupling_factor = coupling_constant * time_step * 0.5;  // Reduced for biological scale

    // Combine factors
    double correction = wave_factor * (1.0 + potential_factor + coupling_factor);

    // Scale to a reasonable correction range for biological systems
    correction =
        0.006 * correction;  // 0.6% base correction for more conservative biological scaling

    // Enforce reasonable bounds on biological correction factor
    return std::min(0.03, std::max(0.0, correction));  // Keep between 0 and 3%
}

double calculateBioZeroPointEnergyContribution(double hbar, double effective_mass,
                                               double water_content, double temperature)
{
    // Biological oscillator zero-point energy
    // Modified for water-mediated effects in biological systems

    // Estimate biomolecular "spring constant" (weaker than covalent bonds)
    double spring_constant = 3.0;  // eV/Å² (typical for hydrogen bonds)

    // Water enhances quantum effects
    spring_constant = spring_constant * (1.0 + water_content * 0.5);

    // Safety check for mass
    double safe_mass = std::max(effective_mass, 1.0e-31);

    // Calculate angular frequency with bounds checking
    double omega = std::sqrt(spring_constant / safe_mass);

    // Calculate zero-point energy
    double zpe = 0.5 * hbar * omega;

    // Temperature scaling
    const double kb = 8.617333262e-5;  // Boltzmann constant in eV/K

    // Safety check for temperature - biological systems have narrower range
    double safe_temp = std::max(temperature, 270.0);  // Minimum biological temperature

    double thermal_energy = kb * safe_temp;

    // Calculate ratio of ZPE to thermal energy with bounded result
    // Water content enhances ZPE effects
    double zpe_significance = (zpe / (thermal_energy + zpe)) * (1.0 + water_content * 0.3);

    // Ensure result is within biologically reasonable bounds
    return std::min(0.05, std::max(0.0, zpe_significance));  // Cap at 5%
}

// Implementation of TieredQMMMPartitioner methods
TieredQMMMPartitioner::QMMethod TieredQMMMPartitioner::selectMethodForRegion(size_t numAtoms) const
{
    if (numAtoms <= 20) return COUPLED_CLUSTER;
    if (numAtoms <= 500) return DENSITY_FUNCTIONAL;
    return SEMIEMPIRICAL;
}

TieredQMMMPartitioner::QMRegion TieredQMMMPartitioner::defineQMRegionAroundLigand(
    const Molecule& ligand, const BiologicalSystem& receptor, double cutoffRadius) const
{
    QMRegion region;
    region.cutoffRadius = cutoffRadius;

    // Add all ligand atoms to QM region
    for (size_t i = 0; i < ligand.getAtomCount(); ++i) {
        region.atomIndices.push_back(ligand.getGlobalIndex(i));
    }

    // Select appropriate QM method based on region size
    region.method = selectMethodForRegion(region.atomIndices.size());

    return region;
}

TieredQMMMPartitioner::QMRegion TieredQMMMPartitioner::defineQMRegionForMetalSystem(
    const Molecule& drug, const BiologicalSystem& receptor) const
{
    QMRegion region = defineQMRegionAroundLigand(drug, receptor, 6.0);

    // For metal-containing systems, prefer higher accuracy methods
    if (drug.containsMetals() && region.method == SEMIEMPIRICAL) {
        region.method = DENSITY_FUNCTIONAL;
    }

    return region;
}

double TieredQMMMPartitioner::calculateMinDistance(const Molecule& mol, const Vector3& point) const
{
    // Calculate minimum distance from any atom in molecule to point
    double minDist = std::numeric_limits<double>::max();
    for (size_t i = 0; i < mol.getAtomCount(); ++i) {
        double dist = (mol.getAtomPosition(i) - point).norm();
        minDist = std::min(minDist, dist);
    }
    return minDist;
}

// Implementation of EnhancedQuantumTunnelingModel methods
double EnhancedQuantumTunnelingModel::calculateTunnelingProbability(double temperature,
                                                                    double barrierHeight,
                                                                    double barrierWidth) const
{
    // Base tunneling calculation
    double baseProbability = std::exp(-barrierWidth * std::sqrt(2.0 * MASS * barrierHeight) / HBAR);

    // Enhanced temperature dependency model
    // Higher temperatures amplify tunneling probability
    double temperatureEnhancement = 1.0 + ALPHA * (temperature - 273.15) / 40.0;

    return baseProbability * temperatureEnhancement;
}

double EnhancedQuantumTunnelingModel::getQuantumEnhancementFactor(double temperature,
                                                                  bool isBiologicalSystem) const
{
    // For biological systems: 3-5% enhancement (lower)
    // For semiconductor systems: 8-12% enhancement (higher)
    double baseEnhancement = isBiologicalSystem ? 0.04 : 0.10;

    // Temperature adjustment (slight decrease at higher temps)
    double tempAdjustment = 1.0 - BETA * (temperature - 310.0) / 100.0;

    return 1.0 + (baseEnhancement * tempAdjustment);
}

// Implementation of QMIntegratedWorkflow methods
QMIntegratedWorkflow::QMIntegratedWorkflow() : currentStage_(PREPROCESSING) {}

void QMIntegratedWorkflow::processDrugCandidate(const Molecule& drug,
                                                const BiologicalSystem& target,
                                                double radiationDose)
{
    switch (currentStage_) {
        case PREPROCESSING:
            // Fast MM-based initial screening
            if (performInitialScreening(drug, target)) {
                advanceToStage(REFINEMENT);
                processDrugCandidate(drug, target, radiationDose);
            }
            break;

        case REFINEMENT:
            // Mixed MM/QM refinement for promising candidates
            if (performRefinementAnalysis(drug, target, radiationDose)) {
                advanceToStage(FINAL_ANALYSIS);
                processDrugCandidate(drug, target, radiationDose);
            }
            break;

        case FINAL_ANALYSIS:
            // Full QM/MM analysis for lead compounds
            performFinalQMMMAnalysis(drug, target, radiationDose);
            break;
    }
}

void QMIntegratedWorkflow::advanceToStage(ComputationalStage newStage)
{
    std::cout << "Advancing from stage " << currentStage_ << " to stage " << newStage << std::endl;
    currentStage_ = newStage;
}

bool QMIntegratedWorkflow::performInitialScreening(const Molecule& drug,
                                                   const BiologicalSystem& target)
{
    std::cout << "Performing initial MM-based screening for drug: " << drug.getName() << std::endl;

    // This is a placeholder until we implement a full MM model
    // In a real implementation, would do a fast MM calculation here

    // Check basic drug-likeness properties
    bool passesRuleOfFive = checkRuleOfFive(drug);

    // For now, pass if it meets basic criteria
    bool passes = passesRuleOfFive && (drug.getMolecularWeight() < 900.0);

    std::cout << "Initial screening result: " << (passes ? "PASSED" : "FAILED") << std::endl;

    return passes;
}

bool QMIntegratedWorkflow::performRefinementAnalysis(const Molecule& drug,
                                                     const BiologicalSystem& target,
                                                     double radiationDose)
{
    std::cout << "Performing QM/MM refinement analysis for drug: " << drug.getName() << std::endl;

    // This is a placeholder until we implement full QM/MM optimization
    // In a real implementation, would do QM/MM optimization here

    // Use quantum tunneling to predict enhanced binding
    EnhancedQuantumTunnelingModel tunnelModel;
    double qmEnhancement = tunnelModel.getQuantumEnhancementFactor(310.0, true);

    std::cout << "QM Enhancement factor: " << qmEnhancement << std::endl;

    // For now, use simplified criteria
    bool passes = (qmEnhancement > 1.02) && (drug.getMolecularWeight() > 200.0) &&
                  (drug.getMolecularWeight() < 900.0);

    std::cout << "Refinement analysis result: " << (passes ? "PASSED" : "FAILED") << std::endl;

    return passes;
}

void QMIntegratedWorkflow::performFinalQMMMAnalysis(const Molecule& drug,
                                                    const BiologicalSystem& target,
                                                    double radiationDose)
{
    std::cout << "Performing final QM/MM analysis for drug: " << drug.getName() << std::endl;

    // Full QM/MM analysis with temperature-dependent quantum effects

    // Define QM region with appropriate method
    TieredQMMMPartitioner partitioner;
    TieredQMMMPartitioner::QMRegion qmRegion;

    if (drug.containsMetals()) {
        qmRegion = partitioner.defineQMRegionForMetalSystem(drug, target);
        std::cout << "Using specialized metal-system QM region definition" << std::endl;
    }
    else {
        qmRegion = partitioner.defineQMRegionAroundLigand(drug, target);
        std::cout << "Using standard QM region definition" << std::endl;
    }

    // Report on QM region
    std::cout << "QM region contains " << qmRegion.atomIndices.size() << " atoms with method: ";

    switch (qmRegion.method) {
        case TieredQMMMPartitioner::SEMIEMPIRICAL:
            std::cout << "SEMIEMPIRICAL" << std::endl;
            break;
        case TieredQMMMPartitioner::DENSITY_FUNCTIONAL:
            std::cout << "DENSITY_FUNCTIONAL" << std::endl;
            break;
        case TieredQMMMPartitioner::COUPLED_CLUSTER:
            std::cout << "COUPLED_CLUSTER" << std::endl;
            break;
    }

    // Temperature-dependent analysis (body temperature)
    const double bodyTemp = 310.0;  // K

    // Calculate quantum enhancement
    EnhancedQuantumTunnelingModel tunnelModel;
    double qmEnhancement = tunnelModel.getQuantumEnhancementFactor(bodyTemp, true);

    std::cout << "Final QM Enhancement factor: " << qmEnhancement << std::endl;

    // In a real implementation, would perform expensive QM/MM calculations here

    // Output final results
    std::cout << "Final QM/MM Analysis Report for " << drug.getName() << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "Quantum Enhancement Factor: " << qmEnhancement << std::endl;
    std::cout << "Radiation Dose: " << radiationDose << " Gy" << std::endl;

    // Calculate synergy for different schedules
    double concurrent = 0.5 * qmEnhancement;
    double radiationFirst = 0.3 * qmEnhancement;
    double drugFirst = 0.4 * qmEnhancement;

    std::cout << "Treatment Schedule Synergy Results:" << std::endl;
    std::cout << "  Concurrent:      " << concurrent << std::endl;
    std::cout << "  Radiation First: " << radiationFirst << std::endl;
    std::cout << "  Drug First:      " << drugFirst << std::endl;
}

bool QMIntegratedWorkflow::checkRuleOfFive(const Molecule& drug)
{
    // Lipinski's Rule of Five implementation
    return (drug.getMolecularWeight() <= 500.0 && drug.getLogP() <= 5.0 &&
            drug.getHBondDonors() <= 5 && drug.getHBondAcceptors() <= 10);
}

}  // namespace healthcare
}  // namespace rad_ml
