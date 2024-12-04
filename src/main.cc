#include "input.h"
#include "meshing.h"
#include "modelMetaData.h"
#include "modelData.h"
#include "modelTopology.h"
#include "magneticGeometry.h"
#include "stomms.h"

int main(int argc, char* argv[])
{
  // Step 1: Read the input file (mesh_input) for the input parameters
  args a(argc, argv);

  // Step 2: Setup the magnetic field information. 
  MagneticGeometry mg;
  mg.setMagneticGeometry(a);

  // Step 3: Setup the data in STOMMS class.
  STOMMS s(mg);

  // Step 4: Setup the poloidal planes using the meta data provided.
  for (int i = 0; i < a.in.pd.planeInput.size(); i++)
  {
      double toroidalAngle = a.in.pd.planeInput[i];
      PlaneMetaData pg(a.in.fd, mg, toroidalAngle);
      s.addPlane(pg);
  }

  // Step 5: Generate the stomms model(geometric model) from the given meta data (pd).
  std::vector <PlaneMetaData> pd = s.getPlanesContainer(); 
  StommsModel stommsModel(pd);

  // Step 6: Get the final model from the stommsModel class for meshing.
  Model model = stommsModel.getModel();

  // Step 7: Mesh the model by iterating over each plane
  pMesh simMesh = 0;
  std::vector <Plane> planes = stommsModel.getPlanes();
  simMesh = meshing(model,planes, &a);  

  return 0;
}

