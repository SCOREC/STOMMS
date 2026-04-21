#include "meshOutput.h"

// Function to get a map between physics classification (int) and list of tags of 
// model faces classified on those physics classes. 
std::map <int, std::vector <int>> modelFaceClassification(const std::vector <Face>& modelFaces)
{
  std::map <int, std::vector <int>> physicsFaces; 
  
  // Step 1: Iterate over model faces, and read the attribute "region" set on those faces
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

  // Step 2: Once curves are sorted in the set, set them in the return container
  std::vector <Flux> curves; // return vector
  curves.insert(curves.end(), curvesSet.begin(), curvesSet.end());
  return curves;
}

// Function to return a map with type of curve as key (closed, open, separatrix, wall) and 
// list of curves in that particular group (stored in class CurveGroup).
std::map <std::string, CurvesGroup> curveClassification(const std::vector <Flux>& fluxCurves, const std::vector<Edge>& wallCurve)
{
  std::map<std::string, CurvesGroup> curves;
  int curveNum = 1;  
 
  // Step 1: Iterate over all the curves, read their curve type, and return the string
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

    // Step 2: Legs of separatrices can be in different curves in raw curve container.
    // Make sure they they all in one separatrix before writing it to adios2 file.
    // To make sure all 3 or 4 legs of a separatrix are in same curve in adios2.
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

    // Step 3: If the model edge range vector is empty, set the first element to 0.
    // Since the range will start from 0 to a number.
    if (curves[curveGroup].modelEdgesRange.size() == 0)
      curves[curveGroup].modelEdgesRange.push_back(0); 
   
    // Step 4: Set the properties of the curves in CurveGroup.
    curves[curveGroup].flxId.push_back(curveNum);
    curves[curveGroup].psi.push_back(curve.psiNormOnFlux);
    curves[curveGroup].modelEdgesVector.insert(curves[curveGroup].modelEdgesVector.end(), modelEdgesTags.begin(), modelEdgesTags.end());
    curves[curveGroup].modelEdgesRange.push_back(curves[curveGroup].modelEdgesVector.size());

    curveNum++;
  }

  // Step 5: Once flux curves are done, set the physical curves in map.
  if (wallCurve.size() > 0)  // only if wall exists
  {
    std::string curveGroup = "wall";
    std::vector <int> modelEdgesTags;
    for (int i = 0; i < wallCurve.size(); i++)
      modelEdgesTags.push_back(GEN_tag(wallCurve[i].getSimEdge())); 
    curves[curveGroup].modelEdgesRange.push_back(0);
    curves[curveGroup].flxId.push_back(curveNum);
    curves[curveGroup].psi.push_back(-1.0);
    curves[curveGroup].modelEdgesVector.insert(curves[curveGroup].modelEdgesVector.end(), modelEdgesTags.begin(), modelEdgesTags.end());
    curves[curveGroup].modelEdgesRange.push_back(curves[curveGroup].modelEdgesVector.size());
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

// Given the dimension of entity (0,1,2), return Adj of all the model entities with
// that dimension on the geometric plane.
Adj getAdjacency(int inDim, const Plane& plane)
{
  Adj adj;
  if (inDim == 0)
  {
    std::vector <Vertex> vertices = plane.getModelVerticesOnPlane();
    adj = getVertexAdj(vertices);
  }
  if (inDim == 1)
  {
    std::vector <Edge> edges = plane.getModelEdgesOnPlane();
    adj = getEdgeAdj(edges);
  }
  if (inDim == 2)
  {
    std::vector <Face> faces = plane.modelFaces;
    adj = getFaceAdj(faces);
  }

  return adj;
}

// Given a vector of model vertices, return both edge and 
// face adjacencies on all the model vertices.
Adj getVertexAdj(const std::vector <Vertex>& vertices)
{
  // Step 1: Declare variables to be populated.
  Adj vAdj;
  vAdj.rangeVector_1.push_back(vAdj.adjVector_1.size());  // should be 0 at this point
  vAdj.rangeVector_2.push_back(vAdj.adjVector_2.size());

  // Step 2: Iterate over the model vertices
  for (int i = 0; i < vertices.size(); i++)
  {
    pGVertex gv = vertices[i].getSimVertex();
    vAdj.entId.push_back(GEN_tag(gv));
 
    // Step 3: First read the adjacent edges on the vertex
    pPList edges = GV_edges(gv);
    for (int i = 0; i < PList_size(edges); ++i)
    {
      pGEdge ge = static_cast<pGEdge>(PList_item(edges, i));
      vAdj.adjVector_1.push_back(GEN_tag(ge));
    }
    PList_delete(edges);
    vAdj.rangeVector_1.push_back(vAdj.adjVector_1.size());

    // Step 4: Now read the adjacent faces on the vertex
    pPList faces = GV_faces(gv);
    for (int i = 0; i < PList_size(faces); ++i)
    {
      pGFace gf = static_cast<pGFace>(PList_item(faces, i));
      vAdj.adjVector_2.push_back(GEN_tag(gf));
    }
    PList_delete(faces);
    vAdj.rangeVector_2.push_back(vAdj.adjVector_2.size());
  }
  if (!vAdj.adjVector_1.size())
    vAdj.adjVector_1.push_back(0);

  return vAdj;
}

// Given a vector of model edges, return both vertex and 
// face adjacencies on all the model edges.
Adj getEdgeAdj(const std::vector <Edge>& edges)
{
  // Step 1: Declare variables to be populated.
  Adj eAdj;
  eAdj.rangeVector_1.push_back(eAdj.adjVector_1.size());  // should be 0 at this point
  eAdj.rangeVector_2.push_back(eAdj.adjVector_2.size());

  // Step 2: Iterate over the model edges
  for (int i = 0; i < edges.size(); i++)
  {
    pGEdge ge = edges[i].getSimEdge();
    eAdj.entId.push_back(GEN_tag(ge));
 
    // Step 3: First read the adjacent vertices on the edge
    pPList vertices = GE_vertices(ge);
    for (int i = 0; i < PList_size(vertices); ++i)
    {
      pGVertex gv = static_cast<pGVertex>(PList_item(vertices, i));
      eAdj.adjVector_1.push_back(GEN_tag(gv));
    }
    PList_delete(vertices);
    eAdj.rangeVector_1.push_back(eAdj.adjVector_1.size());

    // Step 4: Now read the adjacent faces on the edge
    pPList faces = GE_faces(ge);
    for (int i = 0; i < PList_size(faces); ++i)
    {
      pGFace gf = static_cast<pGFace>(PList_item(faces, i));
      eAdj.adjVector_2.push_back(GEN_tag(gf));
    }
    PList_delete(faces);
    eAdj.rangeVector_2.push_back(eAdj.adjVector_2.size());
    
  }
  if (!eAdj.adjVector_1.size())
    eAdj.adjVector_1.push_back(0);
  
  return eAdj;
}

// Given a vector of model faces, return both vertex and 
// face adjacencies on all the model faces.
Adj getFaceAdj(const std::vector <Face>& faces)
{
  // Step 1: Declare variables to be populated.
  Adj fAdj;
  fAdj.rangeVector_1.push_back(fAdj.adjVector_1.size());  // should be 0 at this point
  fAdj.rangeVector_2.push_back(fAdj.adjVector_2.size());
 
  // Step 2: Iterate over the model faces
  for (int i = 0; i < faces.size(); i++)
  {
    pGFace gf = faces[i].getSimFace();
    fAdj.entId.push_back(GEN_tag(gf));
 
    // Step 3: First read the adjacent vertices on the face
    pPList vertices = GF_vertices(gf);
    for (int i = 0; i < PList_size(vertices); ++i)
    {
      pGVertex gv = static_cast<pGVertex>(PList_item(vertices, i));
      fAdj.adjVector_1.push_back(GEN_tag(gv));
    }
    PList_delete(vertices);
    fAdj.rangeVector_1.push_back(fAdj.adjVector_1.size());

    // Step 4: Now read the adjacent edges on the face
    pPList edges = GF_edges(gf);
    for (int i = 0; i < PList_size(edges); ++i)
    {
      pGEdge ge = static_cast<pGEdge>(PList_item(edges, i));
      fAdj.adjVector_2.push_back(GEN_tag(ge));
    }
    PList_delete(edges);
    fAdj.rangeVector_2.push_back(fAdj.adjVector_2.size());
  }

  return fAdj;
}

