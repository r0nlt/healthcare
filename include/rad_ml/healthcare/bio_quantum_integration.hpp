#pragma once

#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace rad_ml {
namespace healthcare {

// Biological tissue types
enum TissueType { SOFT_TISSUE, BONE, NERVE, MUSCLE, BLOOD, NUCLEIC_ACID };

// Biological system model (analogous to CrystalLattice)
struct BiologicalSystem {
    TissueType type = SOFT_TISSUE;
    double cell_density = 1.0;       // cells per cubic mm
    double water_content = 0.7;      // percentage (0-1)
    double effective_barrier = 0.3;  // energy barrier in eV

    // Additional properties relevant to biological systems
    double repair_rate = 0.3;       // cellular repair rate
    double radiosensitivity = 1.0;  // tissue-specific radiosensitivity
};

// Configuration for quantum biological corrections
struct BioQuantumConfig {
    bool enable_quantum_corrections = true;
    bool force_quantum_corrections = false;

    // Thresholds for automatic correction application
    double temperature_threshold = 315.0;   // in Kelvin
    double cell_size_threshold = 5.0;       // in μm
    double radiation_dose_threshold = 2.0;  // in Gy

    // Biological-specific parameters
    double dna_damage_threshold = 0.1;  // baseline for genetic damage
    double radiolysis_factor = 0.5;     // water radiolysis enhancement
};

// Radiation damage model for biological systems
using CellularDamageDistribution = std::map<std::string, double>;

// Parameters for quantum field theory in biological context
struct BiologicalQFTParameters {
    double hbar = 6.582119569e-16;    // reduced Planck constant
    double effective_mass = 1.0e-29;  // effective mass in biological context
    double coupling_constant = 0.1;   // coupling to external fields
    double cell_spacing = 1.0;        // average cell spacing
    double water_coupling = 0.14;     // quantum coupling through water
    double time_step = 1.0e-15;       // simulation time step
    int dimensions = 3;               // spatial dimensions
};

// Forward declarations for molecular classes to avoid circular dependencies
class Molecule;
class Vector3;

// Basic molecule class
class Molecule {
   public:
    virtual ~Molecule() = default;

    virtual size_t getAtomCount() const = 0;
    virtual int getGlobalIndex(size_t localIndex) const = 0;
    virtual Vector3 getAtomPosition(size_t index) const = 0;
    virtual std::string getName() const = 0;
    virtual double getMolecularWeight() const = 0;
    virtual double getLogP() const = 0;
    virtual int getHBondDonors() const = 0;
    virtual int getHBondAcceptors() const = 0;
    virtual bool containsMetals() const = 0;
};

// Simple 3D vector class
class Vector3 {
   public:
    double x = 0.0, y = 0.0, z = 0.0;

    Vector3() = default;
    Vector3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    Vector3 operator-(const Vector3& other) const
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    double norm() const { return std::sqrt(x * x + y * y + z * z); }
};

/**
 * Enhanced QM/MM partitioning class that implements tiered QM approach
 * based on system size and required accuracy
 */
class TieredQMMMPartitioner {
   public:
    enum QMMethod {
        SEMIEMPIRICAL,       // For systems up to 2000 atoms (low accuracy)
        DENSITY_FUNCTIONAL,  // For systems up to 500 atoms (medium accuracy)
        COUPLED_CLUSTER      // For systems up to 20 atoms (high accuracy)
    };

    struct QMRegion {
        std::vector<int> atomIndices;
        QMMethod method = SEMIEMPIRICAL;
        double cutoffRadius = 5.0;
    };

    // Automatically determine QM method based on region size
    QMMethod selectMethodForRegion(size_t numAtoms) const;

    // Define QM region around active site or binding pocket
    QMRegion defineQMRegionAroundLigand(const Molecule& ligand, const BiologicalSystem& receptor,
                                        double cutoffRadius = 5.0) const;

    // Special handling for metal-containing systems (e.g., cisplatin)
    QMRegion defineQMRegionForMetalSystem(const Molecule& drug,
                                          const BiologicalSystem& receptor) const;

   private:
    double calculateMinDistance(const Molecule& mol, const Vector3& point) const;
};

/**
 * Enhanced temperature-dependent quantum tunneling model
 * based on Arodola & Soliman implementation insights
 */
class EnhancedQuantumTunnelingModel {
   public:
    // Calculate tunneling probability with enhanced temperature dependency
    double calculateTunnelingProbability(double temperature, double barrierHeight,
                                         double barrierWidth) const;

    // Calculate quantum enhancement factor based on temperature and system properties
    double getQuantumEnhancementFactor(double temperature, bool isBiologicalSystem) const;

   private:
    static constexpr double HBAR = 1.054571817e-34;  // J·s
    static constexpr double MASS = 9.10938356e-31;   // kg
    static constexpr double ALPHA = 0.5;  // Coefficient for temperature effect on tunneling
    static constexpr double BETA = 0.3;   // Temperature dampening factor for quantum enhancement
};

/**
 * Integrated QM/MM Workflow for biological systems
 */
class QMIntegratedWorkflow {
   public:
    enum ComputationalStage {
        PREPROCESSING,  // Initial MM-based screening
        REFINEMENT,     // Mixed MM/QM for promising candidates
        FINAL_ANALYSIS  // Full QM/MM for lead compounds
    };

    QMIntegratedWorkflow();

    // Process a candidate drug through the staged workflow
    void processDrugCandidate(const Molecule& drug, const BiologicalSystem& target,
                              double radiationDose);

   private:
    ComputationalStage currentStage_;

    void advanceToStage(ComputationalStage newStage);

    bool performInitialScreening(const Molecule& drug, const BiologicalSystem& target);

    bool performRefinementAnalysis(const Molecule& drug, const BiologicalSystem& target,
                                   double radiationDose);

    void performFinalQMMMAnalysis(const Molecule& drug, const BiologicalSystem& target,
                                  double radiationDose);

    bool checkRuleOfFive(const Molecule& drug);
};

// Forward declarations of existing functions

// Create QFT parameters for biological systems
BiologicalQFTParameters createBioQFTParameters(const BiologicalSystem& biosystem,
                                               double cell_size_um);

// Determine if quantum corrections should be applied
bool shouldApplyBioQuantumCorrections(double temperature, double cell_size, double radiation_dose,
                                      const BioQuantumConfig& config);

// Apply quantum corrections to biological radiation damage
CellularDamageDistribution applyQuantumCorrectionsToBiologicalSystem(
    const CellularDamageDistribution& damage, const BiologicalSystem& biosystem, double temperature,
    double cell_size_um, double radiation_dose, const BioQuantumConfig& config);

// Calculate quantum enhancement for biological effects
double calculateBioQuantumEnhancementFactor(double temperature, double cell_size);

// Apply quantum field corrections to cellular damage
CellularDamageDistribution applyBioQuantumFieldCorrections(
    const CellularDamageDistribution& damage, const BiologicalSystem& biosystem,
    const BiologicalQFTParameters& qft_params, double temperature);

// Calculate quantum tunneling in biological systems
double calculateBioQuantumTunnelingProbability(double barrier_height, double effective_mass,
                                               double hbar, double temperature);

// Solve quantum wave equations for biological systems
double solveBioQuantumWaveEquation(double hbar, double effective_mass, double potential_coeff,
                                   double coupling_constant, double cell_spacing, double time_step);

// Calculate zero-point energy effects in biological water
double calculateBioZeroPointEnergyContribution(double hbar, double effective_mass,
                                               double water_content, double temperature);

// Factory function to create standard drugs
std::shared_ptr<Molecule> createStandardDrug(const std::string& drugName);

// Forward declarations for simulation components
class QMMMOptimizer;
class QMMMDynamics;
class MolecularMechanicsModel;

}  // namespace healthcare
}  // namespace rad_ml
