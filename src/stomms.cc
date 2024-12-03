#include "stomms.h"
#include "modelTopology.h"

STOMMS::STOMMS(const MagneticGeometry& magGeom):mg(magGeom)
{
  // Initialize Simmetrix handlers.
  MS_init();
  // NOTE: Sim_readLicenseFile() is for internal testing only.  To use,
  // pass in the location of a file containing your keys.  For a release 
  // product, use Sim_registerKey()
  Sim_readLicenseFile(0);

  prog = Progress_new();
  Progress_setDefaultCallback(prog);

}

STOMMS::~STOMMS()
{
  // Delete Simmetrix handlers.
  Progress_delete(prog);
  Sim_unregisterAllKeys();
  MS_exit();
}

void STOMMS::addPlane(PlaneMetaData pg)
{
  planesContainer.push_back(pg);
}

const std::vector <PlaneMetaData>& STOMMS::getPlanesContainer()
{
  return planesContainer;
}

void STOMMS::setPlanes()
{
  // Step 1: Get the Simmetrix model (pGModel) from Model.
  pGModel simModel = model.getSimModel();
  std::vector <double> planeAngles;
  for (int i = 0; i < planesContainer.size(); i++)
  {
    double angle = planesContainer[i].getPlaneToroidalAngle();
    planeAngles.push_back(angle);
  }

  // Step 2: Sort all the model faces and O-point vertices  according to plane number in a map.
  // In map, the key is the plane # and the element is relevant model data on the plane.
  std::map <int, pGVertex> planesAxisMap = sortOPointsByPlanes(model, planeAngles); 
  std::map <int, std::vector<pGFace>> planesFacesMap = sortFacesByPlanes(model, planeAngles); 

  // Step 3: Iterate over the map and assign the data to each plane (using object Plane)
  std::map <int, std::vector<pGFace>>::iterator itr;
  for (itr = planesFacesMap.begin(); itr != planesFacesMap.end(); itr++)
  {
    Plane p;
    
    // Step 3.1: Get the plane number from the map.
    p.planeNumber = (itr->first);

    // Step 3.2: Get the O-point for this planes from the axis map.
    p.oPoint = planesAxisMap[itr->first];  // Set the Opoint on the plane.

    // Step 3.3: Set the flux curves on each plane.
    std::vector <Flux> fluxCurves = setFluxCurvesOnPlanes(model, p.planeNumber, planesContainer); 
    p.fluxCurves = fluxCurves;  // Set flux curves on the plane.

    // Step 3.4: Set the faces on each plane.
    p.modelFaces = itr->second;  // Set the model faces on the plane.

    // Step 3.5: Save the plane in planes container.
    planes.push_back(p);
  }
}

const std::vector <Plane>& STOMMS::getPlanes()
{
  return planes;
}

void STOMMS::freezeModel()
{
  model = generateCoreSimModel(planesContainer);
  setPlanes();
}

const Model& STOMMS::getModel()
{
  return model;
}
