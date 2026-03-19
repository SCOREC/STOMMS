#include "physicsAttributes.h"

// Model Face Functions
bool isModelFaceOnCore(pGFace gf)
{
  int numPeriodicEdges = getNumPeriodicEdgesOnModelFace(gf);
  if (numPeriodicEdges == 2)
    return true;

  pPList edgesOnFace = GF_edges(gf);
  if (PList_size(edgesOnFace) == 1 && numPeriodicEdges == 1)
      return true;

  return false;
}

int getNumPeriodicEdgesOnModelFace(pGFace gf)
{
  pPList edges = GF_edges(gf);
  int numPeriodicEdges = 0;
  for (int i = 0; i < PList_size(edges); i++)
  {
    pGEdge ge = static_cast<pGEdge>(PList_item(edges, i));
    if(GE_periodic(ge) > 0)
      numPeriodicEdges++;
  }
  PList_delete(edges);

  return numPeriodicEdges;  
}

std::vector <pGVertex> getCriticalPointsOnModelFace(pGFace gf, PointType pointType)
{
  std::vector <pGVertex> criticalPoints;
  pPList vertices = GF_vertices(gf);
  for (int i = 0; i < PList_size(vertices); i++)
  {
    pGVertex gv  = static_cast<pGVertex>(PList_item(vertices,i));
    if (GEN_numNativeIntAttribute(gv, "PointType"))
    {
      int ptType;
      GEN_nativeIntAttribute(gv, "PointType", &ptType);
      if (ptType == static_cast<int>(pointType))
        criticalPoints.push_back(gv);
    }
  }
  PList_delete(vertices);
  
  return criticalPoints;
}

void tagModelFacesAdjacentToXPoint(pGVertex gv, int index)
{
  pPList edges = GV_edges(gv);
  if (PList_size(edges) == 3)
    setFaceTagsOnInnerSeparatrix(gv, index);
  else if (PList_size(edges) == 4)
    setFaceTagsOnOuterSeparatrix(gv, index);

  PList_delete(edges);
}

void setFaceTagsOnInnerSeparatrix(pGVertex gv, int index)
{
  pPList faces = GV_faces(gv);
  for (int i = 0; i < PList_size(faces); i++)
  {
    pGFace gf  = static_cast<pGFace>(PList_item(faces,i));
    if (GEN_numNativeIntAttribute(gf, "PhysicsRegion"))
      continue;  // already tagged.
    int numPeriodicEdges = getNumPeriodicEdgesOnModelFace(gf);
    if (numPeriodicEdges == 2)
      GEN_setNativeIntAttribute(gf, static_cast<int>(FaceType::Core), "PhysicsRegion");
    else if (numPeriodicEdges == 1)
      GEN_setNativeIntAttribute(gf, static_cast<int>(FaceType::ScrapeOffLayer), "PhysicsRegion");
    else
    {
      GEN_setNativeIntAttribute(gf, static_cast<int>(FaceType::Private), "PhysicsRegion");
      GEN_setNativeIntAttribute(gf, index, "PrivateRegionId");
    }  
  }
  PList_delete(faces);
}


void setFaceTagsOnOuterSeparatrix(pGVertex gv, int index)
{
  double max = DBL_MAX;
  double min = -DBL_MAX;
  pPList faces = GV_faces(gv);
  pGFace pvtFace;
  std::vector <pGFace> facesAdjacentToXPoint;
  for (int i = 0; i < PList_size(faces); i++)
  {
    pGFace gf  = static_cast<pGFace>(PList_item(faces,i));
    if (GEN_numNativeIntAttribute(gf, "PhysicsRegion"))
      continue;  // already tagged.
    std::array <double,3> minBounds, maxBounds;
    GEN_bounds(gf, minBounds.data(), maxBounds.data());
    if (index == 1 && minBounds[1] > min)
    {
      min = minBounds[1];
      pvtFace = gf;
    }
    else if (index == 0 && maxBounds[1] < max)
    {
      max = maxBounds[1];
      pvtFace = gf;
    }
    facesAdjacentToXPoint.push_back(gf);
  }
  PList_delete(faces);  

  GEN_setNativeIntAttribute(pvtFace, static_cast<int>(FaceType::Private), "PhysicsRegion");
  GEN_setNativeIntAttribute(pvtFace, index, "PrivateRegionId");
  
  // Remove the private face from vector (will leave only two model faces in vector)
  auto it = std::find(facesAdjacentToXPoint.begin(), facesAdjacentToXPoint.end(), pvtFace);
  if (it != facesAdjacentToXPoint.end())
    facesAdjacentToXPoint.erase(it); 
  assert(facesAdjacentToXPoint.size() == 2);

  // Now tag other two faces:
  pGFace lowFieldSide, highFieldSide;
  std::array<double,3> minBounds0, minBounds1;
  std::array<double,3> maxBounds0, maxBounds1;  // won't use. Just for syntax
  GEN_bounds(facesAdjacentToXPoint[0], minBounds0.data(), maxBounds0.data());
  GEN_bounds(facesAdjacentToXPoint[1], minBounds1.data(), maxBounds0.data());

  if (minBounds0[0] < minBounds1[0])
  {
    highFieldSide = facesAdjacentToXPoint[0];
    lowFieldSide = facesAdjacentToXPoint[1];
  }
  else
  {
    highFieldSide = facesAdjacentToXPoint[1];
    lowFieldSide = facesAdjacentToXPoint[0];
  }
  GEN_setNativeIntAttribute(lowFieldSide, static_cast<int>(FaceType::LowFieldSideEdge), "PhysicsRegion");
  GEN_setNativeIntAttribute(highFieldSide, static_cast<int>(FaceType::HighFieldSideEdge), "PhysicsRegion");
}

// Model Vertex Functions
bool compareVertexPsi(pGVertex gv1, pGVertex gv2)
{
  double psi1, psi2;
  if (GEN_numNativeDoubleAttribute(gv1, "PsiNorm"))
    GEN_nativeDoubleAttribute(gv1, "PsiNorm", &psi1);
  if (GEN_numNativeDoubleAttribute(gv2, "PsiNorm"))
    GEN_nativeDoubleAttribute(gv2, "PsiNorm", &psi2);

  return psi1 <= psi2;
}

// Model Functions
std::vector <pGVertex> getCriticalPointsOnModel(pGModel model, PointType pointType)
{
  std::vector <pGVertex> criticalPoints;
  GVIter vertexIter = GM_vertexIter(model);
  while (pGVertex gv = GVIter_next(vertexIter))
  {
    if (GEN_numNativeIntAttribute(gv, "PointType"))
    {
      int ptType;
      GEN_nativeIntAttribute(gv, "PointType", &ptType);
      if (ptType == static_cast<int>(pointType))
        criticalPoints.push_back(gv);
    }
  }
  GVIter_delete(vertexIter);

  // sort vertices by psi values
  std::sort(criticalPoints.begin(), criticalPoints.end(), compareVertexPsi);
   
  return criticalPoints;
}

// Means atleast two curves with distinct psi values
bool isFaceBoundedByTwoFluxCurves(pGFace gf)
{
  std::vector <double> psiValues;

  pPList edges = GF_edges(gf);
  for (int i = 0; i < PList_size(edges); i++)
  {
    pGEdge ge = static_cast<pGEdge>(PList_item(edges, i));
    double psi;
    bool psiFound = false;
    if (!GEN_numNativeDoubleAttribute(ge, "PsiNorm"))
      continue;
    
    GEN_nativeDoubleAttribute(ge, "PsiNorm", &psi);
    for (int j = 0; j < psiValues.size(); j++)
    {
      if (fabs(psi - psiValues[j]) < 1e-8)
      {
        psiFound = true;
        break;
      }
    }
    if (psiFound)
      continue;

    std::cout << "Psi = " << psi << "\n";
    psiValues.push_back(psi);
  }
  PList_delete(edges);

  std::cout << "Bounding psi curves on face = " << GEN_tag(gf) << " = " << psiValues.size() << "\n";
  if (psiValues.size() == 2)
    return true;

  return false;
}
