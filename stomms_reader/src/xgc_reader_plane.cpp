#include "xgc_reader_plane.hpp"

/****************************/
// Class PlaneMesh
/****************************/
PlaneMesh::PlaneMesh(const Omega_h::Mesh& planeOmegahMesh, const adios2::IO& io, const adios2::Engine& reader, std::string name, int planeNumber)
                    :omegahMesh(planeOmegahMesh),planeNum(planeNumber)
{
  // Step 1: Set the name of the mesh and I/O for local use
  setMeshName(name);
  ioPlane = io;
  readerPlane = reader;

  // Step 2: Set model topology.
  modelTopology = ModelTopology(ioPlane, readerPlane, meshName, planeNum);
 
  // Step 3: Set model
  model = Model(ioPlane, readerPlane, meshName, planeNum);

  // Step 4: Identify and store flux curves according to their adjacent physics region types
  setModelCurvesOnPhysicsRegions();

  // Step 5: Set mesh classification information.
  meshClassification = MeshClassification(omegahMesh);

  // Step 6: Set reverse classificaion information.
  reverseClassification = ReverseClassification(omegahMesh, model, meshClassification, modelTopology);
  

  // Step 7: Read non-aligned Vertices.
  nonAlignedVertices = NonAlignedVertices(omegahMesh, model, modelTopology, reverseClassification);
}

// Function to set mesh name to the plane.
void PlaneMesh::setMeshName(const std::string& name)
{
  meshName = name;
}

// Function to return plane number.
int PlaneMesh::getPlaneNumber()
{
  return planeNum;
}

// Function to return Omega_h mesh on the plane.
const Omega_h::Mesh& PlaneMesh::getOmegahMesh() const
{
  return omegahMesh;
}

// Function to return an object of class ModelTopology set on the plane.
const ModelTopology& PlaneMesh::getModelTopology() const
{
  return modelTopology;
}

// Function to return an object of class Model set on the plane.
const Model& PlaneMesh::getModel() const
{
  return model;
}

// Function to return an object of class MeshClassification set on the plane.
const MeshClassification& PlaneMesh::getMeshClassification() const
{
  return meshClassification;
}

// Function to return an object of class ReverseClassification set on the plane.
const ReverseClassification& PlaneMesh::getReverseClassification() const
{
  return reverseClassification;
}

// Function to return an object of class NonAlignedVertices set on the plane.
const NonAlignedVertices& PlaneMesh::getNonAlignedVertices() const
{
  return nonAlignedVertices;
}

// Functions that might be helpful for current XGC version
// but we might need to redesign them in future.

// Function to set model curves on the above four physics regions.
void PlaneMesh::setModelCurvesOnPhysicsRegions()
{
  std::vector <ModelCurve> core, sol, pvt0, pvt1;

  // Iterate over the curves
  std::map <CurveIdType, ModelCurve> modelCurves = model.getModelCurves();
  for (auto& curve: modelCurves)
  {
    ModelCurve c = curve.second;
    if (c.getCurveType() == CurveType::Closed)
      core.push_back(c);
    else if (c.getCurveType() == CurveType::Wall || c.getCurveType() == CurveType::None)
      continue;
    else if (c.getCurveType() == CurveType::Separatrix)
      sol.push_back(c);
    else if (c.getCurveType() == CurveType::Open)
    {
      int regionType = getRegionTypeForCurve(c);
      if (regionType == 1)
        sol.push_back(c);
      else if (regionType == 2)
        pvt0.push_back(c);
      else if (regionType == 3)
        pvt1.push_back(c);
    }
  }
  modelCurvesOnPhysicsRegions[0] = core;
  modelCurvesOnPhysicsRegions[1] = sol;
  modelCurvesOnPhysicsRegions[2] = pvt0;
  modelCurvesOnPhysicsRegions[3] = pvt1;
}

// Given a model curve, this function returns the physics region type it
// is classified on.
int PlaneMesh::getRegionTypeForCurve(ModelCurve modelCurve)
{
  int regionType = 1;
  std::vector <GeomIdType> edges = modelCurve.getModelEdgesGeomIds();
  
  // One edge is enough to check adjacent faces:
  EdgeIdType edge = edges[0];
  std::vector <SurfaceIdType> faces  = modelTopology.getAdjacentFacesOnEdge(edge);
  assert (faces.size() == 2);
  
  // Again one face is enough to check physics region
  std::map<SurfaceIdType, ModelFace> modelFaces = model.getModelFaces();
  ModelFace f = modelFaces.at(faces[0]);
  SurfaceIdType fId = faces[0];
  if (f.getSurfaceType() == SurfaceType::Private)
  {
      int privateRegion = model.getPrivateRegionIndex(fId);
      regionType = privateRegion+1;
  }
  else 
      return 1;

  return regionType;
}

// Function to return curves at specific physics region. Only works with current 4
// types of regions in XGC (core = 0, SOL = 1, Priavte 1 = 2, Private 2 = 3).
std::vector <ModelCurve> PlaneMesh::getCurvesAtPhysicsRegion(int physicsRegion) const
{
  if (modelCurvesOnPhysicsRegions.find(physicsRegion) == modelCurvesOnPhysicsRegions.end())
  {
    std::cout << "Error: Incorrect Physics Region Requested\n";
    std::cout << "Error: Physics Region Id = " << physicsRegion << "does not exist or contain any flux curves\n";
    exit(1);
  }
  return modelCurvesOnPhysicsRegions.at(physicsRegion);
}
