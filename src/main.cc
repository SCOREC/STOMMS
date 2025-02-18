#include "input.h"
#include "stommsMesh.h"
#include "modelMetaData.h"
#include "modelData.h"
#include "modelTopology.h"
#include "magneticGeometry.h"
#include "stomms.h"
#include "meshMetaData.h"
#include "output.h"

int main(int argc, char* argv[])
{
  // Step 1: Read the input file (mesh_input) for the input parameters
  args a(argc, argv);

  // Step 2: Setup the magnetic field information. 
  MagneticGeometry mg;
  mg.setMagneticGeometry(a);

  // Step 3: Setup the STOMMS class to intialize Simmetrix objects.
  STOMMS s;

  // Step 4: Setup the model metadata by setting up the meta data on poloidal planes.
  ModelMetaData modelMetaData;
  for (int i = 0; i < a.in.pd.planeInput.size(); i++)
  {
      double toroidalAngle = a.in.pd.planeInput[i];
      PlaneMetaData pg(a.in.fd, mg, toroidalAngle);
      modelMetaData.addPlane(pg);
  }

  // Step 5: Generate the stomms model(geometric model) from the given meta data (modelMetaData).
  StommsModel stommsModel(modelMetaData);

  // Step 6: Setup Mesh Meta Data.
  MeshMetaData meshMetaData(stommsModel);

  // Step 7: Mesh the model using mesh meta data.
  StommsMesh stommsMesh(meshMetaData);

  // Step 8: Write output files from STOMMS mesh.
  //StommsOutput stommsOutput(stommsMesh);
  
  return 0;
}

