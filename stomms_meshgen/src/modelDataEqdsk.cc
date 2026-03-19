#include "modelDataEqdsk.h"

ModelEqdsk::ModelEqdsk(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, CurveContainer& curvesMetaData)
{
  // Step 1: Generate Model (with underlying simmetrix model)
  pGModel simModel = generateSimModel(planeMetaData, eqdskData, curvesMetaData);
  GM_write(simModel, "eqdsk.smd", 0, 0);  
  
  // Step 2: Save it as type Model.
  model.setSimModel(simModel);

  // Step 3: Set up the poloidal plane with curves and model faces data
  curvesContainer = curvesMetaData;  // a local copy for use in the class.
  setPlane();
  
  std::cout << "Plane is set\n";
  // Step 4: 
}

void ModelEqdsk::setPlane()
{
  // Step 1: Set the plane and its number.
  Plane p;
  p.planeNumber = 0;

  // step 2: Get the primary O-point on the plane.
  std::vector <pGVertex> oPoints = getCriticalPointsOnModel(model.getSimModel(), PointType::OPoint);
  Vertex axis;
  axis.setSimVertex(oPoints[0]);
  p.oPoint = axis;

  // Step 3: Set the flux curves on the plane.
  p.fluxCurves = setFluxCurvesOnPlane();

  // Step 4: Set the model faces on the plane.
  p.modelFaces = setModelFacesOnPlane();

  // Step 5: Push it back to the vector of planes.
  planes.push_back(p);
}

std::vector <Flux> ModelEqdsk::setFluxCurvesOnPlane()
{
  // Step 1: Get all three types of curves.
  std::vector <Flux> closedCurves = curvesContainer.getCurvesClosed();
  std::vector <Flux> seperatrices = curvesContainer.getCurvesSeparatrix();
  std::vector <Flux> openCurves = curvesContainer.getCurvesOpen();

  // Step 2: Set these curves to return vector.
  std::vector <Flux> fluxCurves;
  fluxCurves.insert(fluxCurves.end(), closedCurves.begin(), closedCurves.end());
  fluxCurves.insert(fluxCurves.end(), seperatrices.begin(), seperatrices.end());
  fluxCurves.insert(fluxCurves.end(), openCurves.begin(), openCurves.end()); 

  return fluxCurves;
}

std::vector <Face> ModelEqdsk::setModelFacesOnPlane()
{
  // Step 1: Fetch the Simmetrix model (pGModel) from Model
  pGModel simModel = model.getSimModel();
  
  // Step 2: Iterate over model faces and set them to the vector.
  std::vector <Face> modelFaces;
  GFIter fIter = GM_faceIter(simModel);
  while (pGFace gf = GFIter_next(fIter))
  {
    // Step 2.1: Set up model face of type Face and push it back in 
    // the vector of faces.
    Face modelFace;
    modelFace.setSimFace(gf);
    modelFaces.push_back(modelFace);
  }
  GFIter_delete(fIter);

  return modelFaces;
}

const Model& ModelEqdsk::getModel() const
{
  return model;
}
