#include "meshOutput.h"

// Function to get a map between physics classification (int) and list of tags of 
// model faces classified on those physics classes. 
std::map <int, std::vector <int>> modelFaceClassification(const std::vector <Face>& modelFaces)
{
  std::map <int, std::vector <int>> physicsFaces; 
  
  // Iterate over model faces, and read the attribute "region" set on those faces
  // to get physics region type.
  for (int i = 0; i < modelFaces.size(); i++)
  {
    pGFace gf = modelFaces[i].getSimFace();
    int faceId = -1;
    GEN_nativeIntAttribute(gf, "PhysicsRegion", &faceId);
    physicsFaces[faceId].push_back(GEN_tag(gf));
  }

  return physicsFaces;
}

// Function to arrange and index the curves as needed in XGC.
std::vector<Flux> curveIndexing(const std::vector<Flux>& fluxCurves)
{
  std::multiset <Flux, FluxCompare> curvesSet;

  // Step 1: Iterate over the curves in the container
  for(int i = 0; i < fluxCurves.size(); i++)
  {
    const Flux& flux = fluxCurves[i];
    curvesSet.insert(flux);
  }

  // Step 3: Once curves are sorted in the set, set them in the return container
  std::vector <Flux> curves; // return vector
  curves.insert(curves.end(), curvesSet.begin(), curvesSet.end());
  return curves;
}

// Function to return a map with type of curve as key (closed, open, separatrix, wall) and vectors of curves
// with its properties.
std::map <std::string, CurvesGroup> curveClassification(const std::vector <Flux>& fluxCurves, const std::vector<Edge>& wallCurve)
{
  std::map<std::string, CurvesGroup> curves;
  int curveNum = 1;  
 
  // Iterate over all the curves, read their curve type, and return the string
  // with type/indexNumberof curve.
  for(int i = 0; i < fluxCurves.size(); i++)
  {
    const Flux& curve = fluxCurves[i];

    std::string curveGroup;
    if (curve.curveType == CurveType::Closed)
      curveGroup = "closed";
    else if (curve.curveType == CurveType::Open)
      curveGroup = "open";
    else if (curve.curveType == CurveType::Separatrix)
      curveGroup = "separatrix";

    std::vector <int> modelEdgesTags;
    modelEdgesTags = getModelEdgesTagsOnCurve(curve);

    // Legs of separatrices can be in different curves in raw curve container.
    // Make sure they they all in one separatrix before writing it to adios2 file.
    if (curve.curveType == CurveType::Separatrix)  // Separatrix
    {
      double psiSep = curve.psiNormOnFlux;
      Flux nextCurve = fluxCurves[i+1];
      while (nextCurve.curveType == CurveType::Separatrix && fabs(nextCurve.psiNormOnFlux - psiSep) < 1e-16)
      {
        std::vector <int> edgesOnSep = getModelEdgesTagsOnCurve(nextCurve);
        modelEdgesTags.insert(modelEdgesTags.end(), edgesOnSep.begin(), edgesOnSep.end());
        i++;
        nextCurve = fluxCurves[i+1];
      }
    }

    if (curves[curveGroup].modelEdgesRange.size() == 0)
      curves[curveGroup].modelEdgesRange.push_back(0); 
   
    curves[curveGroup].flxId.push_back(curveNum);
    curves[curveGroup].psi.push_back(curve.psiNormOnFlux);
    curves[curveGroup].modelEdgesVector.insert(curves[curveGroup].modelEdgesVector.end(), modelEdgesTags.begin(), modelEdgesTags.end());
    curves[curveGroup].modelEdgesRange.push_back(curves[curveGroup].modelEdgesVector.size());

    curveNum++;
  }

  // Add wall curves now
  if (wallCurve.size() > 0)  // only if wall exists
  {
    std::vector <int> modelEdgesTags;
    for (int i = 0; i < wallCurve.size(); i++)
      modelEdgesTags.push_back(GEN_tag(wallCurve[i].getSimEdge())); 
    curves["wall"].modelEdgesRange.push_back(0);
    curves["wall"].flxId.push_back(curveNum);
    curves["wall"].psi.push_back(-1.0);
    curves["wall"].modelEdgesVector.insert(curves["wall"].modelEdgesVector.end(), modelEdgesTags.begin(), modelEdgesTags.end());
    curves["wall"].modelEdgesRange.push_back(curves["wall"].modelEdgesVector.size());
  }
  return curves;
}

// Function to get a map between physics classification (int) and list of tags of 
// model vertices classified on those physics classes. 
std::map <int, std::vector <int>> modelVertexClassification(Vertex oPoint, std::vector <Vertex> xPoints)
{
  std::map <int, std::vector <int>> physicsVertices;

  // Step 1: Set the oPoint in the map.
  physicsVertices[0].push_back(GEN_tag(oPoint.getSimVertex()));  

  // Step 2: Find a list of xPoints in the domain.
  for (int i = 0; i < xPoints.size(); i++)
    physicsVertices[1].push_back(GEN_tag(xPoints[i].getSimVertex()));

  return physicsVertices;
}

// Given physics type model id on a face, return the physics type name
std::string getFaceType(int faceId)
{
  switch(faceId)
  {
    case 0: return "core";
    case 1: return "sol";
    case 2: return "plasmaEdgeRegion/lowFieldSide";
    case 3: return "plasmaEdgeRegion/highFieldSide";
    case 4: return "nearVacuumRegion";
    case 5: return "privateRegion";
    case 6: return "none";
    default: return "none";
  }
}

// Given physics type model id on an edge, return the physics type name
std::string getEdgeType(int edgeId)
{
  switch(edgeId)
  {
    case 0: return "closed";
    case 1: return "open";
    case 2: return "separatrix";
    case 3: return "wall";
    default: return "none";
  }
}

// Given physics type model id on a vertex, return the physics type name
std::string getVertexType(int vertexId)
{
  switch(vertexId)
  {
    case 0: return "oPoint";
    case 1: return "xPoint";
    default: return "none";
  }
}

// Function to return a vector of tags of model edges that are classified on a curve.
std::vector <int> getModelEdgesTagsOnCurve(const Flux& curve)
{
  std::vector <int> edgesTags;
  for (int i = 0; i < curve.edgesOnFlux.size(); i++)
  {
    Edge ge = curve.edgesOnFlux[i];
    pGEdge simEdge = ge.getSimEdge();
    edgesTags.push_back(GEN_tag(simEdge));
  }

  return edgesTags;
}
