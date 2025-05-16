#ifndef RAD_ML_HEALTHCARE_QUANTUM_ENHANCED_WAVE_EQUATION_SOLVER_HPP
#define RAD_ML_HEALTHCARE_QUANTUM_ENHANCED_WAVE_EQUATION_SOLVER_HPP

#include <complex>
#include <functional>
#include <memory>
#include <vector>

namespace rad_ml {
namespace healthcare {
namespace quantum_enhanced {

/**
 * Enumeration for different numerical solvers for quantum wave equations
 */
enum class SolverMethod {
    CRANK_NICOLSON,  // Implicit method, unconditionally stable
    SPLIT_OPERATOR,  // Efficient for time-dependent Hamiltonians
    CHEBYSHEV,       // High accuracy for time-independent Hamiltonians
    RUNGE_KUTTA_4    // Fourth-order explicit method
};

/**
 * Boundary condition types for quantum simulations
 */
enum class BoundaryCondition {
    DIRICHLET,  // Wave function vanishes at boundaries
    NEUMANN,    // Derivative vanishes at boundaries
    PERIODIC,   // System has periodic boundaries
    ABSORBING   // Absorbing boundary conditions
};

/**
 * Configuration for the quantum wave equation solver
 */
struct WaveEquationConfig {
    SolverMethod solver_method = SolverMethod::CRANK_NICOLSON;
    BoundaryCondition boundary_condition = BoundaryCondition::DIRICHLET;
    double spatial_step = 0.1;  // nm
    double time_step = 0.001;   // fs
    int grid_points = 1000;
    int time_steps = 1000;
    double hbar = 1.0545718e-34;  // J·s
    bool normalize_wavefunctions = true;
    double stability_factor = 0.5;  // For explicit methods
};

/**
 * Class for solving quantum wave equations in biological systems
 * with advanced numerical methods
 */
class QuantumWaveEquationSolver {
   public:
    using Wavefunction = std::vector<std::complex<double>>;
    using PotentialFunction = std::function<double(double, double)>;  // (x, t) -> V(x,t)

    /**
     * Constructor with configuration parameters
     */
    explicit QuantumWaveEquationSolver(const WaveEquationConfig& config);

    /**
     * Initialize wave function to a Gaussian wave packet
     * @param center_x Center position of the wave packet
     * @param width Width of the wave packet
     * @param k_0 Initial momentum
     */
    void initializeGaussianWavePacket(double center_x, double width, double k_0);

    /**
     * Set a custom initial wave function
     * @param initial_wavefunction Initial wave function values
     */
    void setInitialWavefunction(const Wavefunction& initial_wavefunction);

    /**
     * Set the potential function for the simulation
     * @param potential_function Function that returns potential V(x,t)
     */
    void setPotentialFunction(PotentialFunction potential_function);

    /**
     * Set biological membrane potential barrier
     * @param barrier_height Height of the barrier in eV
     * @param barrier_width Width of the barrier in nm
     * @param barrier_position Position of the barrier
     */
    void setMembranePotentialBarrier(double barrier_height, double barrier_width,
                                     double barrier_position);

    /**
     * Set protein binding site potential (typically a well)
     * @param well_depth Depth of the potential well in eV
     * @param well_width Width of the well in nm
     * @param well_position Position of the well
     */
    void setProteinBindingSitePotential(double well_depth, double well_width, double well_position);

    /**
     * Set DNA groove potential (typically a periodic potential)
     * @param amplitude Amplitude of the potential in eV
     * @param period Period of the potential in nm
     * @param phase Phase shift
     */
    void setDNAGroovePotential(double amplitude, double period, double phase);

    /**
     * Evolve the wave function for a single time step
     */
    void evolveWavefunctionSingleStep();

    /**
     * Evolve the wave function for multiple time steps
     * @param num_steps Number of time steps to evolve
     * @return Final probability density
     */
    std::vector<double> evolveWavefunction(int num_steps);

    /**
     * Calculate the tunneling probability through a barrier
     * @return Tunneling probability
     */
    double calculateTunnelingProbability();

    /**
     * Calculate the expectation value of position
     * @return Expected position
     */
    double calculateExpectedPosition();

    /**
     * Calculate the expectation value of momentum
     * @return Expected momentum
     */
    double calculateExpectedMomentum();

    /**
     * Calculate the energy expectation value
     * @return Expected energy
     */
    double calculateExpectedEnergy();

    /**
     * Get the current wavefunction
     * @return Current wavefunction
     */
    const Wavefunction& getWavefunction() const;

    /**
     * Get the probability density from the wavefunction
     * @return Probability density vector
     */
    std::vector<double> getProbabilityDensity() const;

    /**
     * Get the position grid for plotting
     * @return Position grid vector
     */
    std::vector<double> getPositionGrid() const;

    /**
     * Update effective mass to account for biological environment
     * @param effective_mass New effective mass in kg
     */
    void setEffectiveMass(double effective_mass);

    /**
     * Apply thermal noise to the wavefunction to model finite temperature
     * @param temperature Temperature in Kelvin
     */
    void applyThermalNoise(double temperature);

   private:
    // Configuration parameters
    WaveEquationConfig config_;

    // Effective mass of the particle (e.g., electron, ion, drug molecule)
    double effective_mass_;

    // Current wavefunction
    Wavefunction wavefunction_;

    // Position grid
    std::vector<double> position_grid_;

    // Potential function
    PotentialFunction potential_function_;

    // Implementation of the Crank-Nicolson method
    void solveCrankNicolson();

    // Implementation of the Split-Operator method
    void solveSplitOperator();

    // Implementation of the Chebyshev method
    void solveChebyshev();

    // Implementation of the Runge-Kutta 4 method
    void solveRungeKutta4();

    // Apply boundary conditions
    void applyBoundaryConditions();

    // Normalize the wavefunction
    void normalizeWavefunction();

    // Construct Hamiltonian operator matrix
    std::vector<std::vector<std::complex<double>>> constructHamiltonian();

    // Tridiagonal matrix solver for Crank-Nicolson
    Wavefunction solveTridiagonalSystem(const std::vector<std::complex<double>>& a,
                                        const std::vector<std::complex<double>>& b,
                                        const std::vector<std::complex<double>>& c,
                                        const Wavefunction& d);
};

/**
 * Advanced function to solve bio-quantum wave equation with multiple interfaces
 * @param hbar Planck's constant
 * @param effective_mass Effective mass in biological medium
 * @param potential_coeff Potential coefficient
 * @param coupling_constant Coupling between subsystems
 * @param cell_spacing Cell spacing in nanometers
 * @param time_step Time step in femtoseconds
 * @return Solution at specified time
 */
double solveBioQuantumWaveEquation(double hbar, double effective_mass, double potential_coeff,
                                   double coupling_constant, double cell_spacing, double time_step);

}  // namespace quantum_enhanced
}  // namespace healthcare
}  // namespace rad_ml

#endif  // RAD_ML_HEALTHCARE_QUANTUM_ENHANCED_WAVE_EQUATION_SOLVER_HPP
