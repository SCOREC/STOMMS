#include "modelDataEqdsk.h"

ModelEqdsk::ModelEqdsk(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, CurveContainer& curvesMetaData)
{
  // Step 1: Generate Model (with underlying simmetrix model)
  pGModel simModel = generateSimModel(planeMetaData, eqdskData, curvesMetaData);
  
  // Step 2: Save it as type Model.
  model.setSimModel(simModel);

  // Step 3: Set up the poloidal plane with curves and model faces data
  curvesContainer = curvesMetaData;  // a local copy for use in the class.
  setPlane();
  
  // Step 4: Update Model classification & write smd model since we will
  // no more changes in model.
  finalModelFaceClassification();
  GM_write(simModel, "simModel.smd", 0, 0);

  // Step 5: Set parametric values of mesh vertices on the flux curves.
  setMeshVerticesOnPlanes();

  // Step 6: Set other properties on the plane.
  std::unordered_map<int, double> meshSizesOnFace = setMeshSizesOnModelFaces(planes[0].modelFaces, eqdskData, planeMetaData.getSizeForUnstructuredMesh());
  planes[0].setMeshSizeOnModelFaces(meshSizesOnFace);
  planes[0].setUnstructuredMeshSizeOnPlane(planeMetaData.getSizeForUnstructuredMesh());
}

void ModelEqdsk::setPlane()
{
  // Step 1: Set the plane and its number.
  Plane p;
  p.planeNumber = 0;

  // Step 2: Get the primary O-point on the plane.
  std::vector <pGVertex> oPoints = getCriticalPointsOnModel(model.getSimModel(), PhysicsPointType::OPoint);
  Vertex axis;
  axis.setSimVertex(oPoints[0]);
  p.oPoint = axis;

  // Step 3: Set x-points on the plane.
  std::vector <pGVertex> xPoints = getCriticalPointsOnModel(model.getSimModel(), PhysicsPointType::XPoint);
  for (int i = 0; i < xPoints.size(); i++)
    p.setXPoint(xPoints[i]);

  // Step 4: Set the flux curves on the plane.
  p.fluxCurves = setFluxCurvesOnPlane();

  // Step 5: Set the model faces on the plane. Set model 
  // edges and vertices from those model faces.
  p.modelFaces = setModelFacesOnPlane();
  p.setModelEntitiesFromModelFaces(p.modelFaces);

  // Step 6: Set the wall edges on the plane.
  p.wallEdges = curvesContainer.getWallEdges();

  // Step 7: Push it back to the vector of planes.
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

void ModelEqdsk::finalModelFaceClassification()
{
  // Step 1: Get all the faces on the model.
  std::vector <Face> modelFaces = model.getModelFaces();

  // Step 2: Iterate over the face and check if they belong to core, sol, pvt, or any other physics region.
  for (int i = 0; i < modelFaces.size(); i++)
  {
    Face f = modelFaces[i];
    pGFace gf = f.getSimFace();

    // Step 2.1: Find faces that are on near vacuum (bounded by wall and last SOL).
    int faceType = -1;
    GEN_nativeIntAttribute(gf, "PhysicsRegion", &faceType);
    if (faceType == static_cast<int>(FaceType::Core) || faceType == static_cast<int>(FaceType::Private) ||
        faceType == static_cast<int>(FaceType::None))
      continue;
    else
    {
      if (!isFaceBoundedByTwoFluxCurves(gf)) 
      {
        GEN_removeNativeIntAttribute(gf, "PhysicsRegion");
        GEN_setNativeIntAttribute(gf, static_cast<int>(FaceType::NearVacuum), "PhysicsRegion"); 
      }
    }
  }
}

void ModelEqdsk::setMeshVerticesOnPlanes()
{
  // Step 1: Set the values on flux curves of plane 0.
  // Only one plane for eqdsk. Just added this function
  // for consistency and easy extension if needed to 
  // do things differently.
  setMeshVerticesOnPlane(0);
}

void ModelEqdsk::setMeshVerticesOnPlane(int planeIndex)
{
  std::vector <FluxParametricPoints> fluxPointsOnPlane;

  // Step 1: Iterate over the flux curves on the plane and set field points on them.
  std::vector <Flux>& fluxCurves = planes[planeIndex].fluxCurves;
  int numPoints = 0;
  for (int i = 0; i < fluxCurves.size(); i++)
  {
    Flux f = fluxCurves[i];

    // Step 1.1: Get the par values of points on the flux curve for field following.
    int pointsPlacementType = 1;
    FluxParametricPoints points(f, pointsPlacementType);
    fluxPointsOnPlane.push_back(points);
  }
  planes[planeIndex].setFieldPointsOnFlux(fluxPointsOnPlane); 
}

// Function to set a map between model face Ids and desired mesh size on them.
std::unordered_map <int, double> ModelEqdsk::setMeshSizesOnModelFaces(const std::vector <Face>& modelFaces, 
                                                                      EqdskData& eqdsk, double meshSize)
{
  std::unordered_map <int, double> meshSizes;

  // Step 1: Iterate over the model faces.
  for (int i = 0; i < modelFaces.size(); i++)
  {
    Face f = modelFaces[i];
    double count = 0;
    double psiNormAvg = 0.0;

    // Step 2: get the edges on face and iterate over them.
    const std::vector <Edge>& edges = f.getEdgesOnFace();
    for (int j = 0; j < edges.size(); j++)
    {
      const pGEdge& ge  = edges[j].getSimEdge();

      // Step 3: Get the psi value (normalized) on the model edges
      // and add them for average calculation
      double psi;
      if (GEN_numNativeDoubleAttribute(ge, "PsiNorm") != 0)
      {
        GEN_nativeDoubleAttribute(ge,"PsiNorm", &psi);
        psiNormAvg += psi;
        ++count;
      }

      // Step 4: Calculate average.
      psiNormAvg /= count;

      // Step 4: If model face has single edge with valid psi value, set
      // the general unstructured mesh size to the face.
      if (count < 2)
      {
        double length = eqdsk.getInterCurveSpacingLinear(psiNormAvg);
        meshSizes[GEN_tag(f.getSimFace())] = std::max(length, 0.01*meshSize);
      }
      else
        meshSizes[GEN_tag(f.getSimFace())] = meshSize;
    }
  }
  return meshSizes;  
}

const Model& ModelEqdsk::getModel() const
{
  return model;
}

// Function to get all the geometric information on individual planes.
const std::vector <Plane>& ModelEqdsk::getPlanes() const
{
  return planes;
}
