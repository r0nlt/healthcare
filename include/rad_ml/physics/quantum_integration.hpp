/**
 * Quantum Field Theory Integration Module
 *
 * This file provides integration points for connecting quantum field theory corrections
 * to the main radiation-tolerant ML framework.
 */

#pragma once

#include <algorithm>  // For std::min, std::max
#include <cmath>
#include <map>
#include <rad_ml/physics/field_theory.hpp>
#include <rad_ml/physics/quantum_field_theory.hpp>
#include <rad_ml/physics/quantum_models.hpp>
#include <string>

namespace rad_ml {
namespace physics {

// Crystal lattice types
enum CrystalLatticeType { FCC_TYPE, BCC, DIAMOND };

// Crystal lattice structure
struct CrystalLattice {
    CrystalLatticeType type;
    double lattice_constant;
    double barrier_height;
    // Other properties specific to the crystal
};

// Parameters for quantum field theory calculations
struct QFTParameters {
    double hbar;               // reduced Planck constant
    double mass;               // effective mass
    double coupling_constant;  // coupling to external fields
    double potential_coefficient;
    double lattice_spacing;
    double time_step;
    int dimensions;
};

// Configuration for applying quantum corrections
struct QuantumCorrectionConfig {
    bool enable_quantum_corrections;
    bool force_quantum_corrections;

    // Thresholds for automatic correction application
    double temperature_threshold;
    double feature_size_threshold;
    double radiation_intensity_threshold;
};

// Defect distribution using map
using DefectDistribution = std::map<std::string, double>;

// Function declarations
QFTParameters createQFTParameters(const CrystalLattice& crystal, double feature_size_nm);

bool shouldApplyQuantumCorrections(double temperature, double feature_size,
                                   double radiation_intensity,
                                   const QuantumCorrectionConfig& config);

DefectDistribution applyQuantumCorrectionsToSimulation(const DefectDistribution& defects,
                                                       const CrystalLattice& crystal,
                                                       double temperature, double feature_size_nm,
                                                       double radiation_intensity,
                                                       const QuantumCorrectionConfig& config);

double calculateQuantumEnhancementFactor(double temperature, double feature_size);

DefectDistribution applyQuantumFieldCorrections(const DefectDistribution& defects,
                                                const CrystalLattice& crystal,
                                                const QFTParameters& qft_params,
                                                double temperature);

double calculateQuantumTunnelingProbability(double barrier_height, double mass, double hbar,
                                            double temperature);

double solveKleinGordonEquation(double hbar, double mass, double potential_coeff,
                                double coupling_constant, double lattice_spacing, double time_step);

double calculateZeroPointEnergyContribution(double hbar, double mass, double lattice_constant,
                                            double temperature);

}  // namespace physics
}  // namespace rad_ml
