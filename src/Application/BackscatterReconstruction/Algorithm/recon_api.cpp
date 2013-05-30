
#include <Application/BackscatterReconstruction/Algorithm/markov.h>
#include <Application/BackscatterReconstruction/Algorithm/calibration.h>
#include <Application/BackscatterReconstruction/Algorithm/recon_api.h>
#include <Application/BackscatterReconstruction/Algorithm/recon_params.h>

char *matVacuumText =
#include "materials/foam.txt"
;

char *matFoamText =
#include "materials/foam.txt"
;

char *matAluminumText =
#include "materials/foam.txt"
;

char *matLeadText =
#include "materials/foam.txt"
;



// calibration - segment image stack of calibration pattern to find disks
void CalibrationSegment(const ReconImageVolumeType::Pointer images,
                        std::vector<unsigned char>& diskIds) {


  ReconImageVolumeType::SizeType inSize = images->GetLargestPossibleRegion().GetSize();

  // copy input into vector
  std::vector<float> forward(inSize[0]*inSize[1]*inSize[2]);
  for (size_t i=0; i<forward.size(); i++)
  {
    forward[i] = images->GetBufferPointer()[i];
  }


  // do the segmentation
  int numPoints = 3;
  int maskSize = 14 * inSize[0] / 64;
  int idSize = 6 * inSize[0] / 64;
  int border = 10 * inSize[0] / 64;
  bool dark = false;
  SegmentCalibPoints(inSize[0], inSize[1], inSize[2],
                     forward, diskIds, numPoints, maskSize, idSize, border, dark);
}


// calibration - given images and disk id masks (after mapping to match calibration pattern), fit detector geometry
void CalibrationFitGeometry(const ReconImageVolumeType::Pointer images,
                            const std::vector<unsigned char>& maskv,
                            const char *workDirectory,
                            const char *initialGeometryConfigFile,
                            const char *outputGeometryConfigFile)
{

  // read geometry configuration
  Geometry geometry;
  geometry.LoadFromFile(initialGeometryConfigFile);


  //
  // localize the disks
  //

  ReconImageVolumeType::SizeType inSize = images->GetLargestPossibleRegion().GetSize();

  // copy input into vectors
  std::vector<float> forward(inSize[0]*inSize[1]*inSize[2]);
  for (size_t i=0; i<forward.size(); i++)
  {
    forward[i] = images->GetBufferPointer()[i];
  }

  // run localization
  std::vector<int> proj_i, point_i;
  std::vector<float> point_x, point_y;
  std::vector<float> residuals;
  LocalizeCalibPoints(inSize[0], inSize[1], inSize[2],
                      forward, maskv,
                      proj_i, point_i, point_x, point_y,
                      residuals,
                      false);

  std::cerr << "found " << proj_i.size() << " points" << std::endl;


  // 
  // fit the detector position
  //

  RECON_CALIB_PATTERN_DEF(cpoints);
  Vec3f baseCenter, baseX, baseY, baseZ;
  CalibrateDetector(geometry, proj_i, point_i, point_x, point_y, cpoints,
                    baseCenter, baseX, baseY, baseZ);


  // save the new geometry file
  geometry.SaveToFile(outputGeometryConfigFile);

}




// reconstruction - don't return until reconstruction is completed
bool reconApiAbort = false;
bool reconRunning = false;
MarkovContext *reconMarkovContext = 0;
Geometry *reconGeometry = 0;
std::vector<Material> reconMaterials;
double reconApiProgress = 0;

void ReconstructionStart(const ReconImageVolumeType::Pointer images,
                         const ReconMaterialIdVolumeType::Pointer initialGuess, // possibly NULL
                         const char *geometryConfigFile,
                         const float voxelSizeCM[3],
                         int iterations,
                         ReconMaterialIdVolumeType::Pointer finalReconVolume)
{
  int samplesPerPixel = 1;
  float voxelStepSize = RECON_VOXEL_STEP_SIZE;
  int niter = iterations;
  float regWeight = RECON_REGULARIZATION_WEIGHT;
  float startTemp = RECON_TEMP_START;
  float endTemp = RECON_TEMP_END;
  int numThreads = 1;
  int numSubThreads = 1;


  if (reconRunning)
  {
    std::cerr << "ReconstructionStart(): reconstruction already running!" << std::endl;
    return;
  }
  reconRunning = true;
  reconApiProgress = 0;

  // cleanup if we're rerunning
  if (reconMarkovContext)
  {
    delete reconMarkovContext;
    reconMarkovContext = 0;
    delete reconGeometry;
    reconGeometry = 0;
  }


  // initialize materials - only need to do this once
  if (reconMaterials.empty())
  {
    reconMaterials.resize(3);
    reconMaterials[0].InitFromString(matVacuumText, XRAY_ENERGY_LOW, XRAY_ENERGY_HIGH);
    reconMaterials[1].InitFromString(matFoamText, XRAY_ENERGY_LOW, XRAY_ENERGY_HIGH);
    reconMaterials[2].InitFromString(matAluminumText, XRAY_ENERGY_LOW, XRAY_ENERGY_HIGH);
  }

  // load geometry
  reconGeometry = new Geometry();
  reconGeometry->LoadFromFile(geometryConfigFile);
  reconGeometry->SetVoxelSize(Vec3f(voxelSizeCM[0]/100,
                                    voxelSizeCM[1]/100,
                                    voxelSizeCM[2]/100));  

  ReconImageVolumeType::SizeType projSize = images->GetLargestPossibleRegion().GetSize();
  if (projSize[0] != reconGeometry->GetDetectorSamplesWidth() ||
      projSize[1] != reconGeometry->GetDetectorSamplesHeight() ||
      projSize[2] != reconGeometry->GetNumProjectionAngles())
  {

    std::cerr << "input forward projection dimensions ( "
      << projSize[0] << ", " << projSize[1] << ", " << projSize[2] << " ) do not match geometry configuration file ( "
      << reconGeometry->GetDetectorSamplesWidth() << ", "
      << reconGeometry->GetDetectorSamplesHeight() << ", "
      << reconGeometry->GetNumProjectionAngles() << " )!" << std::endl;

    delete reconGeometry;
    reconGeometry = 0;
    reconRunning = false;
    return;
  }

  reconMarkovContext = new MarkovContext(*reconGeometry, reconMaterials, samplesPerPixel, voxelStepSize, regWeight);
  reconMarkovContext->SetBaselineProjection(images);


  // initialize reconstruciton
  if (initialGuess.IsNull())
  {
    reconMarkovContext->FindInitialGuess();
  }
  else
  {
    ByteVolumeType::SizeType initvolumeSize = initialGuess->GetLargestPossibleRegion().GetSize();

    if (initvolumeSize[0] != reconGeometry->GetVolumeNodeSamplesX() ||
        initvolumeSize[1] != reconGeometry->GetVolumeNodeSamplesY() ||
        initvolumeSize[2] != reconGeometry->GetVolumeNodeSamplesZ())
    {      
      std::cerr << "initial volume dimensions ( "
        << initvolumeSize[0] << ", " << initvolumeSize[1] << ", " << initvolumeSize[2] << " ) do not match geometry configuration file ( "
        << reconGeometry->GetVolumeNodeSamplesX() << ", "
        << reconGeometry->GetVolumeNodeSamplesY() << ", "
        << reconGeometry->GetVolumeNodeSamplesZ() << " )!" << std::endl;
      delete reconMarkovContext;
      reconMarkovContext = 0;
      delete reconGeometry;
      reconGeometry = 0;
      reconRunning = false;
      return;
    }

    reconMarkovContext->SetCurrentVolume(initialGuess);
  }


  // run reconstruction
  reconApiAbort = false;
  reconMarkovContext->Gibbs(numThreads, numSubThreads, niter, startTemp, endTemp);


  // save output
  reconMarkovContext->GetCurrentVolume(finalReconVolume);


  // don't cleanup the context / geometry since we may still call ReconstructionGetMaterialVolume()
  reconRunning = true;
}



// reconstruction - stop it if it's currently running
void ReconstructionAbort() {
  std::cerr << "ReconstructionAbort()" << std::endl;
  reconApiAbort = true;
}


// reconstruction - get the latest reconstructed volume material ids, can be called any time during reconstruction
double ReconstructionGetMaterialVolume(ReconMaterialIdVolumeType::Pointer reconVolume)
{
  if (!reconMarkovContext)
  {
    std::cerr << "ReconstructionGetMaterialVolume(): no reconstruction to get results from!" << std::endl;
  }
  else
  {
    reconMarkovContext->GetCurrentVolume(reconVolume);
    std::cerr << "ReconstructionGetMaterialVolume(): progress=" << reconApiProgress << std::endl;
  }

  return reconApiProgress;
}
