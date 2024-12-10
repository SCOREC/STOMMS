#include "meshMetaData.h"

void PlaneMeshMetaData::setModelPlane(const Plane& p)
{
  modelPlane = p;
  std::vector <pGFace> modelFaces = modelPlane.modelFaces;
  faceAttributes = setFaceAttributes(modelFaces);
  std::vector <Flux> fluxCurves = modelPlane.fluxCurves;
  for (int i = 0; i < fluxCurves.size(); i++)
  {
    Flux f = fluxCurves[i];
    std::vector <double> parValuesOnFlux = setMeshVerticesOnFlux(f); 
    meshVerticesLocation.push_back(parValuesOnFlux);      
  }
}

std::vector <int> PlaneMeshMetaData::setFaceAttributes(const std::vector <pGFace> geomFaces)
{
  std::vector <int> faceAttributes;
  for (int i = 0; i < geomFaces.size(); i++)
  {
    pGFace gf = geomFaces[i];
    pPList edgesOnFace = GF_edges(gf);
    int numPeriodicEdges = 0;
    int faceAttribute = 0;
    for (int j = 0; j < PList_size(edgesOnFace); j++)
    {
      pGEdge ge = static_cast<pGEdge>(PList_item(edgesOnFace, 0));
      if (GE_periodic(ge) > 0)
        numPeriodicEdges++;
    }
    if (numPeriodicEdges == 2 || (PList_size(edgesOnFace) == 1 && numPeriodicEdges == 1))
      faceAttribute = 1;

    PList_delete(edgesOnFace);
    faceAttributes.push_back(faceAttribute);
  }
  return faceAttributes;
}

std::vector <double> PlaneMeshMetaData::setMeshVerticesOnFlux(const Flux& f)
{
  std::vector <double> parValuesOnFlux;
  pGEdge ge = f.edgesOnFlux[0];
  double parR[2];
  GE_parRange(ge, &parR[0], &parR[1]);

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

const std::vector <pGFace>& PlaneMeshMetaData::getModelFacesOnPlane()
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

const pGVertex& PlaneMeshMetaData::getOPointOnPlane()
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
