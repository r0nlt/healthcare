#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "rad_ml/healthcare/bio_quantum_integration.hpp"

namespace rad_ml {
namespace healthcare {

// Concrete implementation of the Molecule abstract class
class ConcreteChemotherapeuticMolecule : public Molecule {
   public:
    struct Atom {
        int globalIndex;
        Vector3 position;
        std::string element;
    };

    ConcreteChemotherapeuticMolecule(const std::string& name, double molWeight, double logP,
                                     int hbondDonors, int hbondAcceptors, bool hasMetal)
        : name_(name),
          molecularWeight_(molWeight),
          logP_(logP),
          hbondDonors_(hbondDonors),
          hbondAcceptors_(hbondAcceptors),
          hasMetal_(hasMetal)
    {
    }

    // Add an atom to the molecule
    void addAtom(int globalIndex, const Vector3& position, const std::string& element)
    {
        atoms_.push_back({globalIndex, position, element});

        // Update metal status based on atom types
        if (!hasMetal_ && (element == "Pt" || element == "Fe" || element == "Zn" ||
                           element == "Mg" || element == "Ca" || element == "Cu")) {
            hasMetal_ = true;
        }
    }

    // Implementation of Molecule interface
    size_t getAtomCount() const override { return atoms_.size(); }

    int getGlobalIndex(size_t localIndex) const override
    {
        if (localIndex >= atoms_.size()) {
            std::cerr << "Error: Index out of bounds in getGlobalIndex" << std::endl;
            return -1;
        }
        return atoms_[localIndex].globalIndex;
    }

    Vector3 getAtomPosition(size_t index) const override
    {
        if (index >= atoms_.size()) {
            std::cerr << "Error: Index out of bounds in getAtomPosition" << std::endl;
            return Vector3();
        }
        return atoms_[index].position;
    }

    std::string getName() const override { return name_; }

    double getMolecularWeight() const override { return molecularWeight_; }

    double getLogP() const override { return logP_; }

    int getHBondDonors() const override { return hbondDonors_; }

    int getHBondAcceptors() const override { return hbondAcceptors_; }

    bool containsMetals() const override { return hasMetal_; }

   private:
    std::string name_;
    double molecularWeight_;
    double logP_;
    int hbondDonors_;
    int hbondAcceptors_;
    bool hasMetal_;
    std::vector<Atom> atoms_;
};

// Factory function to create molecules for standard drugs
std::shared_ptr<Molecule> createStandardDrugMolecule(const std::string& drugName)
{
    if (drugName == "CISPLATIN") {
        auto molecule = std::make_shared<ConcreteChemotherapeuticMolecule>("Cisplatin", 300.01,
                                                                           -2.19, 0, 4, true);

        // Add atoms for cisplatin (simplified model)
        molecule->addAtom(0, Vector3(0.0, 0.0, 0.0), "Pt");
        molecule->addAtom(1, Vector3(2.0, 0.0, 0.0), "Cl");
        molecule->addAtom(2, Vector3(-2.0, 0.0, 0.0), "Cl");
        molecule->addAtom(3, Vector3(0.0, 2.0, 0.0), "N");
        molecule->addAtom(4, Vector3(0.0, -2.0, 0.0), "N");
        molecule->addAtom(5, Vector3(1.0, 2.5, 0.0), "H");
        molecule->addAtom(6, Vector3(-1.0, 2.5, 0.0), "H");
        molecule->addAtom(7, Vector3(1.0, -2.5, 0.0), "H");
        molecule->addAtom(8, Vector3(-1.0, -2.5, 0.0), "H");

        return molecule;
    }
    else if (drugName == "PACLITAXEL") {
        auto molecule = std::make_shared<ConcreteChemotherapeuticMolecule>("Paclitaxel", 853.9, 3.0,
                                                                           4, 12, false);

        // Add a few representative atoms for paclitaxel (simplified)
        for (int i = 0; i < 20; ++i) {
            molecule->addAtom(i, Vector3(i * 1.0, i * 0.5, i * 0.25), "C");
        }

        return molecule;
    }
    else if (drugName == "DOXORUBICIN") {
        auto molecule = std::make_shared<ConcreteChemotherapeuticMolecule>("Doxorubicin", 543.52,
                                                                           1.27, 6, 12, false);

        // Add a few representative atoms for doxorubicin (simplified)
        for (int i = 0; i < 15; ++i) {
            molecule->addAtom(i, Vector3(i * 1.2, i * 0.6, i * 0.3), "C");
        }

        return molecule;
    }
    else if (drugName == "FLUOROURACIL") {
        auto molecule = std::make_shared<ConcreteChemotherapeuticMolecule>("Fluorouracil", 130.08,
                                                                           -0.89, 2, 4, false);

        // Add a few representative atoms for fluorouracil (simplified)
        for (int i = 0; i < 9; ++i) {
            molecule->addAtom(i, Vector3(i * 1.1, i * 0.4, 0), "C");
        }
        molecule->addAtom(9, Vector3(5.0, 5.0, 0.0), "F");

        return molecule;
    }

    std::cerr << "Warning: Unknown drug '" << drugName << "'" << std::endl;
    return std::make_shared<ConcreteChemotherapeuticMolecule>("Unknown", 0.0, 0.0, 0, 0, false);
}

// Factory function implementation for the virtual Molecule
inline std::shared_ptr<Molecule> createStandardDrug(const std::string& drugName)
{
    // Use the existing implementation
    return createStandardDrugMolecule(drugName);
}

}  // namespace healthcare
}  // namespace rad_ml
