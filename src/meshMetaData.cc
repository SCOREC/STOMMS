#include "meshMetaData.h"

void PlaneMeshMetaData::setModelPlane(const Plane& p)
{
  modelPlane = p;
  std::vector <Face> modelFaces = modelPlane.modelFaces;
  faceAttributes = setFaceAttributes(modelFaces);
  std::vector <Flux> fluxCurves = modelPlane.fluxCurves;
  for (int i = 0; i < fluxCurves.size(); i++)
  {
    Flux f = fluxCurves[i];
    std::vector <double> parValuesOnFlux = setMeshVerticesOnFlux(f); 
    meshVerticesLocation.push_back(parValuesOnFlux);      
  }
}

std::vector <int> PlaneMeshMetaData::setFaceAttributes(const std::vector <Face> geomFaces)
{
  std::vector <int> faceAttributes;
  for (int i = 0; i < geomFaces.size(); i++)
  {
    Face gf = geomFaces[i];
    std::vector <Edge> edgesOnFace = gf.getEdgesOnFace();
    int numPeriodicEdges = 0;
    int faceAttribute = 0;
    for (int j = 0; j < edgesOnFace.size(); j++)
    {
      Edge ge = edgesOnFace[j];
      if (ge.edgeIsPeriodic())
        numPeriodicEdges++;
    }
    if (numPeriodicEdges == 2 || edgesOnFace.size() == 1 && numPeriodicEdges == 1)
      faceAttribute = 1;

    faceAttributes.push_back(faceAttribute);
  }
  return faceAttributes;
}

std::vector <double> PlaneMeshMetaData::setMeshVerticesOnFlux(const Flux& f)
{
  std::vector <double> parValuesOnFlux;
  Edge ge = f.edgesOnFlux[0];
  std::vector <double> parR = ge.getEdgeParRange();

  int numVert = f.meshVerticesOnFlux;
  double parInterval = (parR[1] - parR[0])/numVert;
  double currentPar = 0.0;
  currentPar = parR[0];
  parValuesOnFlux.push_back(currentPar);
  int numIter = 1;  
  while (numIter < numVert)
  {
    currentPar += parInterval;
    parValuesOnFlux.push_back(currentPar);
    numIter++;
  }

  return parValuesOnFlux;
}

const std::vector <Face>& PlaneMeshMetaData::getModelFacesOnPlane()
{
  return modelPlane.modelFaces;
}

const std::vector <int>& PlaneMeshMetaData::getFaceAttributesOnPlane()
{
  return faceAttributes;
}

const std::vector <Flux>& PlaneMeshMetaData::getFluxCurvesOnPlane()
{
  return modelPlane.fluxCurves;
}

const std::vector <std::vector<double>>& PlaneMeshMetaData::getMeshVerticesOnFlux()
{
  return meshVerticesLocation;
}

const Vertex& PlaneMeshMetaData::getOPointOnPlane()
{
  return modelPlane.oPoint;
}
const int& PlaneMeshMetaData::getPlaneNumber()
{
  return modelPlane.planeNumber;
}

// MeshMetaData class functions.
MeshMetaData::MeshMetaData(const StommsModel& m):stommsModel(m)
{
  // Step 1: Get all the model planes from model.
  std::vector <Plane> modelPlanes = stommsModel.getPlanes();

  for (int i = 0; i < modelPlanes.size(); i++)
  {
    PlaneMeshMetaData pMeshData;
    Plane p = modelPlanes[i];
    pMeshData.setModelPlane(p);
    planeMeshData.push_back(pMeshData);
  }
}

const StommsModel& MeshMetaData::getStommsModel()
{
  return stommsModel;
}

const std::vector <PlaneMeshMetaData>& MeshMetaData::getMeshMetaDataPlanes()
{
  return planeMeshData;
}
