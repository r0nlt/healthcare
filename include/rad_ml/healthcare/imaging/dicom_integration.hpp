#ifndef RAD_ML_HEALTHCARE_IMAGING_DICOM_INTEGRATION_HPP
#define RAD_ML_HEALTHCARE_IMAGING_DICOM_INTEGRATION_HPP

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "rad_ml/healthcare/cell_biology/cell_cycle_model.hpp"

namespace rad_ml {
namespace healthcare {
namespace imaging {

using namespace rad_ml::healthcare::cell_biology;

/**
 * Structure for DICOM image data
 */
struct DICOMImage {
    int width = 0;
    int height = 0;
    int depth = 0;
    double pixel_spacing_x = 1.0;  // mm
    double pixel_spacing_y = 1.0;  // mm
    double slice_thickness = 1.0;  // mm
    std::vector<float> pixel_data;
    std::string modality;  // CT, MR, etc.
    std::string patient_id;
    std::string study_instance_uid;
    std::string series_instance_uid;

    // Get a voxel value
    float getVoxel(int x, int y, int z) const;

    // Set a voxel value
    void setVoxel(int x, int y, int z, float value);

    // Convert voxel coordinates to physical coordinates (mm)
    std::array<double, 3> voxelToPhysical(int x, int y, int z) const;

    // Convert physical coordinates (mm) to voxel coordinates
    std::array<int, 3> physicalToVoxel(double x, double y, double z) const;

    // Get dimensions
    std::array<int, 3> getDimensions() const { return {width, height, depth}; }

    // Get spacing
    std::array<double, 3> getSpacing() const
    {
        return {pixel_spacing_x, pixel_spacing_y, slice_thickness};
    }
};

/**
 * Structure for DICOM RT structure set
 */
struct DICOMStructureSet {
    std::string sop_instance_uid;
    std::string referenced_frame_of_reference_uid;

    // Structure defined by name and contour points
    struct Structure {
        std::string name;
        std::string roi_number;
        std::string interpreted_type;  // e.g., "ORGAN", "PTV", "GTV", etc.

        // Contour by slice
        struct Contour {
            int slice_index;
            std::vector<std::array<double, 3>> points;  // 3D points in physical space (mm)
        };

        std::vector<Contour> contours;

        // Check if a point is inside the structure
        bool isInside(const std::array<double, 3>& point) const;
    };

    std::vector<Structure> structures;

    // Find structure by name
    const Structure* findStructureByName(const std::string& name) const;
};

/**
 * Structure for DICOM RT dose
 */
struct DICOMDose {
    int width = 0;
    int height = 0;
    int depth = 0;
    double pixel_spacing_x = 1.0;  // mm
    double pixel_spacing_y = 1.0;  // mm
    double slice_thickness = 1.0;  // mm
    std::vector<float> dose_data;  // Dose values in Gy
    double dose_grid_scaling = 1.0;
    std::string dose_units;
    std::string dose_type;

    // Get dose at a voxel
    float getDose(int x, int y, int z) const;

    // Get dose at a physical point (mm)
    float getDoseAtPoint(double x, double y, double z) const;

    // Convert voxel coordinates to physical coordinates (mm)
    std::array<double, 3> voxelToPhysical(int x, int y, int z) const;

    // Convert physical coordinates (mm) to voxel coordinates
    std::array<int, 3> physicalToVoxel(double x, double y, double z) const;
};

/**
 * Structure for DICOM RT plan
 */
struct DICOMPlan {
    std::string label;
    std::string plan_intent;

    // Beam definition
    struct Beam {
        std::string beam_number;
        std::string beam_name;
        std::string radiation_type;  // e.g., "PHOTON", "ELECTRON", etc.
        double beam_energy;          // MV or MeV
        std::string treatment_machine;

        // Control point represents a beam segment
        struct ControlPoint {
            double gantry_angle;
            double collimator_angle;
            double couch_angle;
            std::vector<double> jaw_positions;  // X1, X2, Y1, Y2
            std::vector<double> mlc_positions;  // MLC leaf positions
            double meterset_weight;
        };

        std::vector<ControlPoint> control_points;
    };

    std::vector<Beam> beams;

    // Fraction group
    struct FractionGroup {
        std::string fractionation_pattern;
        int number_of_fractions;

        // Reference beam with meterset
        struct ReferencedBeam {
            std::string referenced_beam_number;
            double beam_meterset;
        };

        std::vector<ReferencedBeam> referenced_beams;
    };

    std::vector<FractionGroup> fraction_groups;
};

/**
 * Structure for voxelized biological parameters
 */
struct VoxelizedBiologicalSystem {
    DICOMImage geometry;
    std::vector<TissueType> tissue_types;
    std::vector<BiologicalSystemExtended> biological_systems;
    std::vector<float> alpha_values;
    std::vector<float> beta_values;
    std::vector<float> water_content;
    std::vector<float> radiosensitivity;

    // Get tissue type at voxel
    TissueType getTissueType(int x, int y, int z) const;

    // Get biological system at voxel
    const BiologicalSystemExtended& getBiologicalSystem(int x, int y, int z) const;

    // Get alpha/beta at voxel
    std::pair<float, float> getAlphaBeta(int x, int y, int z) const;
};

/**
 * Class for DICOM integration
 */
class DICOMIntegration {
   public:
    /**
     * Constructor
     */
    DICOMIntegration();

    /**
     * Load a DICOM image (CT, MR, etc.)
     * @param file_path Path to DICOM file
     * @return Loaded DICOM image
     */
    DICOMImage loadDICOMImage(const std::string& file_path);

    /**
     * Load a DICOM image series
     * @param directory_path Path to directory containing DICOM series
     * @return Loaded DICOM image
     */
    DICOMImage loadDICOMSeries(const std::string& directory_path);

    /**
     * Load a DICOM RT structure set
     * @param file_path Path to DICOM RT structure set file
     * @return Loaded structure set
     */
    DICOMStructureSet loadStructureSet(const std::string& file_path);

    /**
     * Load a DICOM RT dose
     * @param file_path Path to DICOM RT dose file
     * @return Loaded dose
     */
    DICOMDose loadDose(const std::string& file_path);

    /**
     * Load a DICOM RT plan
     * @param file_path Path to DICOM RT plan file
     * @return Loaded plan
     */
    DICOMPlan loadPlan(const std::string& file_path);

    /**
     * Create a voxelized biological system from DICOM data
     * @param image DICOM image (CT)
     * @param structures DICOM structure set
     * @return Voxelized biological system
     */
    VoxelizedBiologicalSystem createVoxelizedBiologicalSystem(const DICOMImage& image,
                                                              const DICOMStructureSet& structures);

    /**
     * Create a voxelized biological system from DICOM data with dose
     * @param image DICOM image (CT)
     * @param structures DICOM structure set
     * @param dose DICOM dose
     * @return Voxelized biological system
     */
    VoxelizedBiologicalSystem createVoxelizedBiologicalSystem(const DICOMImage& image,
                                                              const DICOMStructureSet& structures,
                                                              const DICOMDose& dose);

    /**
     * Calculate biological effective dose (BED) distribution
     * @param dose DICOM dose
     * @param bio_system Voxelized biological system
     * @param num_fractions Number of fractions
     * @return BED distribution as a DICOM dose object
     */
    DICOMDose calculateBED(const DICOMDose& dose, const VoxelizedBiologicalSystem& bio_system,
                           int num_fractions);

    /**
     * Calculate equivalent dose in 2 Gy fractions (EQD2) distribution
     * @param dose DICOM dose
     * @param bio_system Voxelized biological system
     * @param num_fractions Number of fractions
     * @return EQD2 distribution as a DICOM dose object
     */
    DICOMDose calculateEQD2(const DICOMDose& dose, const VoxelizedBiologicalSystem& bio_system,
                            int num_fractions);

    /**
     * Calculate tumor control probability (TCP) for a target structure
     * @param dose DICOM dose
     * @param bio_system Voxelized biological system
     * @param structure_name Target structure name
     * @param num_fractions Number of fractions
     * @return TCP value
     */
    double calculateTCP(const DICOMDose& dose, const VoxelizedBiologicalSystem& bio_system,
                        const std::string& structure_name, int num_fractions);

    /**
     * Calculate normal tissue complication probability (NTCP) for an organ at risk
     * @param dose DICOM dose
     * @param bio_system Voxelized biological system
     * @param structure_name Organ at risk structure name
     * @param num_fractions Number of fractions
     * @return NTCP value
     */
    double calculateNTCP(const DICOMDose& dose, const VoxelizedBiologicalSystem& bio_system,
                         const std::string& structure_name, int num_fractions);

    /**
     * Create a biological system from a structure
     * @param structure_name Structure name
     * @param structures DICOM structure set
     * @return Biological system with appropriate parameters
     */
    BiologicalSystemExtended createBiologicalSystemFromStructure(
        const std::string& structure_name, const DICOMStructureSet& structures);

    /**
     * Map HU value to tissue type
     * @param hu_value Hounsfield unit value
     * @return Tissue type
     */
    TissueType mapHUToTissueType(float hu_value);

    /**
     * Map tissue type to default biological parameters
     * @param tissue_type Tissue type
     * @return Biological system with default parameters
     */
    BiologicalSystemExtended getDefaultBiologicalSystem(TissueType tissue_type);

    /**
     * Save a DICOM dose file
     * @param dose DICOM dose object
     * @param file_path Output file path
     * @return Success flag
     */
    bool saveDose(const DICOMDose& dose, const std::string& file_path);

   private:
    // Map structure name to tissue type
    TissueType mapStructureToTissueType(const std::string& structure_name);

    // Calculate volume of a structure
    double calculateStructureVolume(const DICOMStructureSet::Structure& structure);

    // Calculate mean dose in a structure
    double calculateMeanDose(const DICOMDose& dose, const DICOMStructureSet::Structure& structure);

    // Calculate dose-volume histogram (DVH) for a structure
    std::vector<std::pair<double, double>> calculateDVH(
        const DICOMDose& dose, const DICOMStructureSet::Structure& structure, int num_bins = 100);

    // Apply biological models to dose data
    std::vector<float> applyBiologicalModel(const std::vector<float>& physical_dose,
                                            const VoxelizedBiologicalSystem& bio_system,
                                            int num_fractions, const std::string& model_type);
};

/**
 * Create a biological system from DICOM structure
 * @param structure_path Path to DICOM RT structure set file
 * @param dose_path Path to DICOM RT dose file
 * @return Biological system
 */
BiologicalSystemExtended createBiologicalSystemFromDICOM(const std::string& structure_path,
                                                         const std::string& dose_path);

/**
 * Create a voxelized biological system
 * @param ct_image CT image
 * @param structures Structure set
 * @param dose Dose distribution
 * @return Voxelized biological system
 */
VoxelizedBiologicalSystem createVoxelizedBiologicalSystem(const DICOMImage& ct_image,
                                                          const DICOMStructureSet& structures,
                                                          const DICOMDose& dose);

}  // namespace imaging
}  // namespace healthcare
}  // namespace rad_ml

#endif  // RAD_ML_HEALTHCARE_IMAGING_DICOM_INTEGRATION_HPP
