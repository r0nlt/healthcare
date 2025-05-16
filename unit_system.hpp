#ifndef UNIT_SYSTEM_HPP
#define UNIT_SYSTEM_HPP

#include <cmath>
#include <string>
#include <unordered_map>

namespace rad_ml {
namespace units {

// Unit system for radiation biology and quantum physics
// Handles common unit conversions between different systems
class UnitSystem {
   public:
    // Energy conversions
    static double eV_to_joule(double energy_eV)
    {
        return energy_eV * 1.602176634e-19;  // J/eV
    }

    static double joule_to_eV(double energy_J)
    {
        return energy_J / 1.602176634e-19;  // eV/J
    }

    // Length conversions
    static double nm_to_m(double length_nm)
    {
        return length_nm * 1.0e-9;  // m/nm
    }

    static double m_to_nm(double length_m)
    {
        return length_m * 1.0e9;  // nm/m
    }

    static double um_to_m(double length_um)
    {
        return length_um * 1.0e-6;  // m/μm
    }

    static double m_to_um(double length_m)
    {
        return length_m * 1.0e6;  // μm/m
    }

    static double nm_to_um(double length_nm)
    {
        return length_nm * 1.0e-3;  // μm/nm
    }

    static double um_to_nm(double length_um)
    {
        return length_um * 1.0e3;  // nm/μm
    }

    // Mass conversions
    static double amu_to_kg(double mass_amu)
    {
        return mass_amu * 1.66053906660e-27;  // kg/amu
    }

    static double kg_to_amu(double mass_kg)
    {
        return mass_kg / 1.66053906660e-27;  // amu/kg
    }

    // Temperature conversions
    static double celsius_to_kelvin(double temp_C)
    {
        return temp_C + 273.15;  // K
    }

    static double kelvin_to_celsius(double temp_K)
    {
        return temp_K - 273.15;  // °C
    }

    // Radiation dose conversions
    static double gray_to_eV_per_kg(double dose_Gy)
    {
        return dose_Gy * 6.24150907e15;  // eV/kg per Gy
    }

    static double eV_per_kg_to_gray(double dose_eV_per_kg)
    {
        return dose_eV_per_kg / 6.24150907e15;  // Gy per eV/kg
    }

    // Quantum physics constants
    static double get_planck_constant_eV_s()
    {
        return 4.135667696e-15;  // eV·s
    }

    static double get_reduced_planck_constant_eV_s()
    {
        return 6.582119569e-16;  // eV·s (ħ = h/2π)
    }

    static double get_boltzmann_constant_eV_per_K()
    {
        return 8.617333262e-5;  // eV/K
    }

    static double get_electron_mass_kg()
    {
        return 9.1093837015e-31;  // kg
    }

    static double get_electron_charge_coulombs()
    {
        return 1.602176634e-19;  // C
    }

    // Time conversions
    static double hours_to_seconds(double time_hours)
    {
        return time_hours * 3600.0;  // s/h
    }

    static double seconds_to_hours(double time_seconds)
    {
        return time_seconds / 3600.0;  // h/s
    }

    // LET (Linear Energy Transfer) conversions
    static double keV_per_um_to_J_per_m(double let_keV_um)
    {
        return let_keV_um * 1.602176634e-16;  // J/m per keV/μm
    }

    static double J_per_m_to_keV_per_um(double let_J_m)
    {
        return let_J_m / 1.602176634e-16;  // keV/μm per J/m
    }

    // Concentration conversions
    static double mmol_per_L_to_molecules_per_m3(double conc_mmol_L,
                                                 double avogadro = 6.02214076e23)
    {
        return conc_mmol_L * avogadro * 1.0e-3;  // molecules/m³ per mmol/L
    }

    static double molecules_per_m3_to_mmol_per_L(double conc_molecules_m3,
                                                 double avogadro = 6.02214076e23)
    {
        return conc_molecules_m3 / (avogadro * 1.0e-3);  // mmol/L per molecules/m³
    }

    // Cell biology specific unit conversions
    static double gy_to_lethal_lesions_per_cell(double dose_gy, double alpha = 0.2)
    {
        // Simple linear model: lesions = α × dose
        return alpha * dose_gy;  // Lethal lesions per cell per Gy
    }

    // Helper for quantum tunneling calculations (barrier penetration coefficient)
    static double calculate_tunneling_factor(double barrier_eV, double width_nm, double mass_kg)
    {
        // 2 * width * sqrt(2 * m * E) / ħ
        double width_m = nm_to_m(width_nm);
        double energy_J = eV_to_joule(barrier_eV);
        double hbar_Js = get_reduced_planck_constant_eV_s() * get_electron_charge_coulombs();

        double exponent = -2.0 * width_m * std::sqrt(2.0 * mass_kg * energy_J) / hbar_Js;
        return std::exp(exponent);
    }
};

// Value with unit representation
template <typename T>
class Value {
   public:
    Value(T value, std::string unit) : value_(value), unit_(unit) {}

    T getValue() const { return value_; }
    std::string getUnit() const { return unit_; }

    // Conversion constructor
    Value<T> convert(const std::string& target_unit) const
    {
        if (unit_ == target_unit) {
            return *this;
        }

        // Create a conversion key from original unit to target unit
        std::string conversion_key = unit_ + "_to_" + target_unit;

        // Check if conversion exists in the map
        if (conversion_functions_.find(conversion_key) != conversion_functions_.end()) {
            T new_value = conversion_functions_[conversion_key](value_);
            return Value<T>(new_value, target_unit);
        }

        // If no direct conversion exists, throw an error or return the original
        throw std::runtime_error("No conversion from " + unit_ + " to " + target_unit);
    }

   private:
    T value_;
    std::string unit_;

    // Static map of conversion functions
    static std::unordered_map<std::string, T (*)(T)> conversion_functions_;
};

// Initialize the conversion map for double values
template <>
std::unordered_map<std::string, double (*)(double)> Value<double>::conversion_functions_ = {
    {"eV_to_J", UnitSystem::eV_to_joule},
    {"J_to_eV", UnitSystem::joule_to_eV},
    {"nm_to_m", UnitSystem::nm_to_m},
    {"m_to_nm", UnitSystem::m_to_nm},
    {"um_to_m", UnitSystem::um_to_m},
    {"m_to_um", UnitSystem::m_to_um},
    {"nm_to_um", UnitSystem::nm_to_um},
    {"um_to_nm", UnitSystem::um_to_nm},
    {"C_to_K", UnitSystem::celsius_to_kelvin},
    {"K_to_C", UnitSystem::kelvin_to_celsius},
    {"Gy_to_eV/kg", UnitSystem::gray_to_eV_per_kg},
    {"eV/kg_to_Gy", UnitSystem::eV_per_kg_to_gray},
    {"h_to_s", UnitSystem::hours_to_seconds},
    {"s_to_h", UnitSystem::seconds_to_hours}};

}  // namespace units
}  // namespace rad_ml

#endif  // UNIT_SYSTEM_HPP
