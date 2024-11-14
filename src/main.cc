#include "input.h"
#include "model.h"
#include "meshing.h"
#include "modelTopology.h"
#include "magneticGeometry.h"
#include "stomms.h"

int main(int argc, char* argv[])
{
  MS_init();
  // NOTE: Sim_readLicenseFile() is for internal testing only.  To use,
  // pass in the location of a file containing your keys.  For a release 
  // product, use Sim_registerKey()
  Sim_readLicenseFile(0);

  pProgress prog = Progress_new();
  Progress_setDefaultCallback(prog);

  // Step 1: Read the input file (mesh_input) for the input parameters
  args a(argc, argv);

  // Step 2: Setup the magnetic field information. 
  MagneticGeometry mg(a);

  // Step 3: Setup the data in STOMMS class
  STOMMS s(mg);

  // Step 4: Setup the poloidal planes.
  for (int i = 0; i < a.in.pd.planeInput.size(); i++)
  {
      double toroidalAngle = a.in.pd.planeInput[i];
      PlaneGeometry pg(a.in.fd, mg, toroidalAngle);
      s.addPlane(pg);
  }

  // Step 5: Generate the core region of the stellarator from the given VMEC file
  pGModel simModel = 0;		// Initialize an empty simmetrix pGModel 
  simModel = generateCoreSimModel(&a);

  // Step 6: Using the information from pVmecFlux object associated with
  // model, define the planes.
  std::vector <Plane> planesContainer;
  getPlanes(simModel, planesContainer, &a);

  // Step 7: Save the model topology information
  Model model(simModel);
  
  // Step 6: Mesh the model by iterating over each plane
  pMesh simMesh = 0;
  simMesh = meshing(simModel,planesContainer, &a);  

  Progress_delete(prog);

  Sim_unregisterAllKeys();
  MS_exit();
  return 0;
}

