#include "tommsOutput.h"

/***********************************************/
// class TommsOutput
/***********************************************/

// TommsOutput constructor takes in StommsMesh and writes nodes, elements, and flx mesh data.
TommsOutput::TommsOutput(const StommsMesh& mesh):stommsMesh(mesh)
{
  std::cout << "============ Writing TOMMS Output Meshes ============\n";

  // Step 1: Get underlying Simmetrix Model stored in mesh meta data.
  const MeshMetaData& meshMetaData = stommsMesh.getMeshMetaData();
  const StommsModel& stommsModel = meshMetaData.getStommsModel();
  const Model& mdl = stommsModel.getModel();
  simModel = mdl.getSimModel();

  // Step 2: Get underlying Simmetrix mesh and initialize a Nex file.
  simMesh = stommsMesh.getSimMesh();
  nex = MeshNex_new(simMesh); 
 
  // Step 3: Sort the flux curves in terms of psi values and physics regions.
  geometricPlanes = stommsModel.getPlanes();  
  setFluxCurvesOnPhysicsRegions();

  // Step 4: Write node file
  writeNodeFile(); 

  // Step 5: Write element file
  writeElementsFile();

  // Step 6: Write flux file
  writeFluxFile();
}

TommsOutput::~TommsOutput()
{
  // Step 1: Remove the wall-curve flag attached to mesh vertices, then its data id.
  for (pVertex vertex : nodes)
    EN_deleteData(vertex, wallId);
  MD_deleteMeshDataId(wallId);

  // Step 2: Close the flux file.
  if (fluxFile != nullptr)
    fclose(fluxFile);

  // Step 2: Delete the node numbering map.
  MeshNex_delete(nex);
}

// High level function to write the mesh data to the nodes file.
void TommsOutput::writeNodeFile()
{
  std::cout << ".......... Writing nodes data to the file nodes.txt\n";
  // Step 1: Write axis node to the vector.
  writeAxisNode();

  // Step 2: Write ordered list of the nodes on the flux curves to nodes vector.
  writeNodesOnFluxCurves();

  // Step 3: Write the mesh vertices on the wall curves.
  writeNodesOnWallCurve();

  // Step 4: Write all the remaining vertices (unstructured mesh vertices).
  writeUnstructuredNodes();
  
  // Step 5: Write all the node data in nodes vector and nex file to the file.
  writeNodeDataToFile();
}

// Function to get the magnetic axis node and write it to the nodes vector.
void TommsOutput::writeAxisNode()
{
  // Step 1: get the model vertex classified on the oPoint and get simmetrix model vertex
  // from it.
  Vertex axis = geometricPlanes[0].oPoint;
  pGVertex axisSimVertex = axis.getSimVertex();

  // Step 2: Find the mesh vertex classified on this model vertex.
  pVertex meshVertex = M_classifiedVertex(simMesh, axisSimVertex);

  // Step 3: Save the vertex to the nodes vector, and set mapping nodeIndx and meshVertex in 
  // the nex file. Also, set 0 to wallId since its not on the wall curve.
  nodes.push_back(meshVertex);
  MeshNex_setNode(nex, meshVertex, 0, nodeIndx++);
  EN_attachDataInt(meshVertex, wallId, 0);
}

// Function to write ordered list of vertices on the flux curves to the nodes vector.
void TommsOutput::writeNodesOnFluxCurves()
{
  // Step 1: Iterate over the sorted list of flux curves.
  for (int i = 0; i < sortedFluxCurves.size(); i++)
  {
    const Flux& curve = sortedFluxCurves[i];

    // Step 2: For each flux curve, iterate over the model edges defining the flux curve.
    const std::vector <Edge>& edges = curve.edgesOnFlux;
    for (int j = 0; j < edges.size(); j++)
    {
      pGEdge ge = edges[j].getSimEdge();

      // Step 3: Iterate over the mesh vertices that are classified on the model edge.
      // If they are already written in nodes files, or at wall, skip them. Else, write
      // them to nodes vector, set the mapping in nex file, and set wallId to 0 since
      // all vertices are on the flux curve.
      VIter vertexIter = M_classifiedVertexIter(simMesh, ge, 1);
      while (pVertex vertex = VIter_next(vertexIter))
      {
        // Step 3.1: Skip if the vertex is already written in nex file, or its on wall curve.
        if (MeshNex_hasNode(nex, vertex, 0))
          continue;
        if (isOnWall(vertex))
          continue;

        // Step 3.2: Else write them to the nodes vector and set it in nex file, and its wallId.
        nodes.push_back(vertex);
        MeshNex_setNode(nex, vertex, 0, nodeIndx++);
        EN_attachDataInt(vertex, wallId, 0);    
      }
      VIter_delete(vertexIter);
    }
  }
}

// Function to write list of mesh vertices on model edges of the wall curve.
void TommsOutput::writeNodesOnWallCurve()
{
  // Step 1: Iterate over all the model edges classified on the wall curve.
  const std::vector <Edge>& wallEdges = geometricPlanes[0].wallEdges;
  for (int i = 0; i < wallEdges.size(); i++)
  {
    pGEdge ge = wallEdges[i].getSimEdge();

    // Step 2: Iterate over the mesh vertices that are classified on the model edge.
    // If they are already written in nodes files, skip them. Else, write them to the
    // nodes file and set the rest of the data.
    VIter vertexIter = M_classifiedVertexIter(simMesh, ge, 1);
    while (pVertex vertex = VIter_next(vertexIter))
    {
      // Step 2.1: Skip if the vertex is already written in nex file.
      if (MeshNex_hasNode(nex, vertex, 0))
        continue;

      // Step 2,2: Else write them to the nodes vector and set it in nex file, and set 
      // its  wallId (which is 1 since these mesh vertices are on the wall curves).
      nodes.push_back(vertex);
      MeshNex_setNode(nex, vertex, 0, nodeIndx++);
      EN_attachDataInt(vertex, wallId, 1);
    }
    VIter_delete(vertexIter); 
  } 
}

// Function to write any nodes (unstructured) that haven't been written yet by the 
// above three functions.
void TommsOutput::writeUnstructuredNodes()
{
  // Step 1: Iterate over the mesh vertices in the mesh. If they are already written 
  // in nodes files (check nex file), skip them. Else save them.
  VIter vertexIter = M_vertexIter(simMesh);
  while (pVertex vertex = VIter_next(vertexIter))
  {
    // Step 1.1: Skip if the vertex is already written in nex file.
    if (MeshNex_hasNode(nex, vertex, 0))
      continue;
    assert(!isOnWall(vertex));

    // Step 1.2: Else write them to the nodes vector and set it in nex file, and set
    // its wallId.
    nodes.push_back(vertex);
    MeshNex_setNode(nex, vertex, 0, nodeIndx++);
    EN_attachDataInt(vertex, wallId, 0);     
  }
  VIter_delete(vertexIter);
}

// Function to write information stored in nodes vector to an output file
void TommsOutput::writeNodeDataToFile()
{
  // Step 1: Open a file and write number of nodes to it.
  FILE* fp = fopen("nodes.txt","w");
  int numVtx = nodes.size();
  fprintf(fp, "%d  2  0  1\n", numVtx);

  // Step 2: Write the coordinates data to the node file. Also, write
  // wallId to show if the mesh vertex is on the wall curve or not. 
  for (int i = 0; i < numVtx; i++)
  {
    std::array <double, 3> coord;
    V_coord(nodes[i], coord.data());

    int vertexOnWall;
    EN_getDataInt(nodes[i], wallId, &vertexOnWall);
    fprintf(fp, "%d %lf %lf %d\n", i+1, coord[0], coord[1], vertexOnWall);
  }
  fclose(fp);
}

// Function to write the connectivity informaton of elements to the elements file.
void TommsOutput::writeElementsFile()
{
  std::cout << ".......... Writing elements data to the file elements.txt\n";

  // Step 1: Open the file and write number of elements to it.
  FILE* fp = fopen("elements.txt","w");
  int numElements = M_numFaces(simMesh);
  fprintf(fp, "%d  3  0\n", numElements);

  // Step 2: Declare arrays to hold 3 mesh vertices (pVertex) and mesh vertex indices.
  std::array <pVertex, 3> verticesOnFace;
  std::array <int, 3> vertexIndices;

  // Step 3: Iterate over the elements in the mesh, and set their connectivity 
  // information. Use node numbers mapping from nex file.
  int index = 1;
  FIter faceIter = M_faceIter(simMesh);
  while (pFace face = FIter_next(faceIter))
  {
    // Step 3.1: Make sure each element has three mesh vertices.
    int numVerticesOnFace = F_verticesArray(face, verticesOnFace.data(), 1);
    assert (numVerticesOnFace == 3);
   
    // Step 3.2: Get the indices of each mesh vertex from the nex file.
    for(int i = 0; i < 3; i++)
      vertexIndices[i] = MeshNex_node(nex, verticesOnFace[i], 0);

    // Step 3.3: Write the index and vertices indices to the file.
    fprintf(fp," %d %d %d %d\n", index, vertexIndices[0], vertexIndices[1], vertexIndices[2]);  
    index++;
  }
  FIter_delete(faceIter);
  fclose(fp);
}

// Function to write flux curves information to the file.
void TommsOutput::writeFluxFile()
{
  // Step 1: Open the flux file.
  fluxFile = fopen("flux.txt", "w");

  // Step 2: Write line 1 (number of Xpoint) and line 2 (mesh vertices
  // classified on the Xpoints model vertex).
  writeXpointsToFluxFile();

  // Step 3: Write number of flux curves in each physics region on line 3.
  writeFluxNumbersInRegionsToFile();
}

// Function to write xPoints information to the flux file. It writes two pieces.
// 1- Number of Xpoints. 2- Indices of mesh vertices classified on the xPoints 
// model vertices.
void TommsOutput::writeXpointsToFluxFile()
{
  // Step 1: Get the vector of xPoints on the poloidal plane.
  const std::vector <Vertex>& xPoints = geometricPlanes[0].getXPointsOnPlane();

  // Step 2: Write number of xPoints to the line 1 of flux file.
  int numXPoints = xPoints.size();
  fprintf(fluxFile,"%d\n", numXPoints);

  // Step 3: Iterate over the xpoints and write mesh vertex (ordered numbered of vertex) to the file.
  for (int i = 0; i < numXPoints; i++)
  {
    pGVertex simVertex = xPoints[i].getSimVertex();
    pVertex meshVertex = M_classifiedVertex(simMesh, simVertex);
    int index = MeshNex_node(nex, meshVertex, 0);
    fprintf(fluxFile,"%d ", index);
  }
  
  // Step 4: If number of xpoints are 1, make sure to add a -1 for second xpoints.
  // If xpoints are 0, add place holder -1 for both. Its a place holder (hard coded). 
  // I don't like this way but to match XGC logic, I need to do this.
  if (numXPoints == 0)
    fprintf(fluxFile,"%d %d\n", -1, -1);
  else if (numXPoints == 1)
    fprintf(fluxFile,"%d\n", -1);
  else
    fprintf(fluxFile,"\n");
}

// Function to write number of flux curves in each region to the flux file.
void TommsOutput::writeFluxNumbersInRegionsToFile()
{
  // Step 1: Update the ordering of the flux curves. See the description
  // of function updateFluxOrdering() to see why we need this step.
  updateFluxOrdering();

  // Step 2: Get number of flux curves in each region. If number is zero
  // in any region, simply set -1 for the region.
  int nCore = core.size() + 1;  // add 1 for magnetic axis
  int nSol = (sol.size() == 0) ? -1: sol.size();
  int nPvt0 = (pvt0.size() == 0) ? -1: pvt0.size();
  int nPvt1 = (pvt1.size() == 0) ? -1: pvt1.size();

  // Step 3: Write the number of flux cruves to the file.
  fprintf(fluxFile,"%d %d %d %d\n", nCore, nSol, nPvt0, nPvt1);
}

// Function to update the ordering of the flux curves in sorted flux curves vector. 
// Also, update the vector sol, pvt0, and pvt1.
// See header file (tommsOutput.h) to read why we need this step.
void TommsOutput::updateFluxOrdering()
{
  // Step 1: Set the known starting point of each types of curves in the vector and  
  // then clear the sol, and private region vectors. Core will stay same so keep it.
  int solStart = core.size();  // where core curves end
  int pvt0Start = solStart + sol.size();  // where core + sol curves end
  int pvt1Start = pvt0Start + pvt0.size(); 
  sol.clear();
  pvt0.clear();
  pvt1.clear();

  // Step 2: Get separatrix psi values of all the xPoints. Just to be consistent 
  // with old xgc normal (same as TOMMS), only use first two xpoints.
  const std::vector <Vertex>& xPoints = geometricPlanes[0].getXPointsOnPlane();
  int numXPoints = xPoints.size();

  // Step 2.1: For every xPoint vertex, get its simmetrix vertex and read the psi
  // valuea attribute, attach to it.
  std::vector <double> xPointsPsi;
  for (int i = 0; i < numXPoints; i++)
  {
    pGVertex gv = xPoints[i].getSimVertex();
    double psiNorm = -1.0;
    GEN_nativeDoubleAttribute(gv, "PsiNorm", &psiNorm);
    xPointsPsi.push_back(psiNorm);
  }

  // Step 2.2: Set values of psi to first two separatrices only.
  double psiSep0 = (numXPoints >= 1) ? xPointsPsi[0]: DBL_MAX;
  double psiSep1 = (numXPoints >= 2) ? xPointsPsi[1]: DBL_MAX;

  // Step 3: Iterate over the sol and pvt region curves in the sorted list.
  // It has core curves at the start, so skip them and start where sol starts.
  std::map <std::pair <double, int>, int> solCurves;
  for (int i = solStart; i < sortedFluxCurves.size(); i++)
  {
    // Step 3.1: Get the curve and it's psi value so we can do comparison of this
    // value with separatrix psi values.
    const Flux& curve = sortedFluxCurves[i];
    double psi = curve.psiNormOnFlux;

    // Step 3.2: Check the curves in the private region, and use the psi filter to decide if 
    // they truly belong to the private region or to SOL. If it's in outer private region, 
    // and psi less than outer separatrix, keep them in pvt1 (it will remove any values greater 
    // than psiSep1). Similarly, only keep curves in private region 0 if it's psi norm is less 
    // than 1.00 (psi of inner separatrix). 
    if (i >= pvt1Start && psi <= psiSep1)
      pvt1.push_back(curve);
    else if (i >= pvt0Start && i < pvt1Start && psi <= psiSep0)
      pvt0.push_back(curve);
    else
    { // Keeping them in this map to sort any newly added curve.
      int tag = GEN_tag(curve.edgesOnFlux[0].getSimEdge());
      solCurves[{psi, tag}] = i;
    }
  }

  // Step 4: We already have private regions sorted. Just push back 
  // solCurves to sol region for using it later.
  for (const auto& fluxCurve : solCurves)
    sol.push_back(sortedFluxCurves[fluxCurve.second]);

  // Step 5: Clear and update the sorted curves list.
  sortedFluxCurves.clear();
  sortedFluxCurves.insert(sortedFluxCurves.end(), core.begin(), core.end());
  sortedFluxCurves.insert(sortedFluxCurves.end(), sol.begin(), sol.end());
  sortedFluxCurves.insert(sortedFluxCurves.end(), pvt0.begin(), pvt0.end());
  sortedFluxCurves.insert(sortedFluxCurves.end(), pvt1.begin(), pvt1.end());

   for (int i = 0; i < sol.size(); i++)
    if (sol[i].curveType == CurveType::Separatrix)
      std::cout << i << " psi=" << sol[i].psiNormOnFlux << " edges=" << sol[i].edgesOnFlux.size() << "\n";

}

// Function to set model curves to the specific physics region. It is sorting
// curves according to psi values in each physics region.
void TommsOutput::setFluxCurvesOnPhysicsRegions()
{
  // Step 1: Get all the model faces on the plane, and a map of physics
  // type and a vector of all model faces of that type.
  const std::vector <Face>& modelFaces = geometricPlanes[0].modelFaces;
  std::map <int, std::vector <int>> physicsFaces = modelFaceClassification(modelFaces);

  // Step 2: Create a map of model edge and flux Id (flux curve that edge belong to).
  const std::vector <Flux>& fluxCurves = geometricPlanes[0].fluxCurves;
  curveUsed.resize(fluxCurves.size(), false);  // initialize the vector to false
  for (int i = 0; i < fluxCurves.size(); i++)
  {
    // Step 2.1: If the curve is wall curve or of unknown type, ignore it.
    const Flux& curve = fluxCurves[i];
    if (curve.curveType == CurveType::Wall || curve.curveType == CurveType::None)
      continue;

    // Step 2.2: Get the edges on the flux curve, and create a map between
    // edge and the flux curve (index) it is classified on.
    const std::vector <Edge>& edgesOnCurve = curve.edgesOnFlux;
    for (int j = 0; j < edgesOnCurve.size(); j++)
    {
      pGEdge ge = edgesOnCurve[j].getSimEdge();
      edgesToCurveMap[ge] = i;
    }
  }

  // Step 3: Set flux curves according to their psi values in each of the physics regions.
  // We have 4 physics region in XGC(core,sol, private0 and private1). The faces from
  // FaceType::ScrapeOffLayer, LowFieldSideEdge, HighFieldSideEdge all belong to SOL. So
  // we will add flux curves from them to SOL. Second index is for private regions only and
  // -1 is dummy for other types. 
  addCurvesToRegion(physicsFaces[static_cast<int>(FaceType::Core)], -1, core);
  addCurvesToRegion(physicsFaces[static_cast<int>(FaceType::ScrapeOffLayer)], -1, sol);
  addCurvesToRegion(physicsFaces[static_cast<int>(FaceType::LowFieldSideEdge)], -1, sol);
  addCurvesToRegion(physicsFaces[static_cast<int>(FaceType::HighFieldSideEdge)], -1, sol);
  addCurvesToRegion(physicsFaces[static_cast<int>(FaceType::Private)], 0, pvt0);
  addCurvesToRegion(physicsFaces[static_cast<int>(FaceType::Private)], 1, pvt1);

  // Step 4: Once arranged in main 4 groups, set them in the sorted flux curves vector.
  sortedFluxCurves.insert(sortedFluxCurves.end(), core.begin(), core.end());
  sortedFluxCurves.insert(sortedFluxCurves.end(), sol.begin(), sol.end());
  sortedFluxCurves.insert(sortedFluxCurves.end(), pvt0.begin(), pvt0.end());
  sortedFluxCurves.insert(sortedFluxCurves.end(), pvt1.begin(), pvt1.end());
}

// Given the tags of model faces on the specific region, add all the flux curves (sorted by psi) bounding these 
// model faces to the curves vector. privateId = -1 for non-private regions.
void TommsOutput::addCurvesToRegion(const std::vector <int>& faceTags, int privateId, std::vector <Flux>& curves)
{
  // Step 1: Get the vector of all the flux curves on the poloidal plane.
  const std::vector <Flux>& fluxCurves = geometricPlanes[0].fluxCurves;

  // Step 2: Save the psi - edge indx pair as a key of the map with flux curves id
  // as the element. The reason is to sort values in terms of psi as key. But if
  // different psi values are same, the next tie braker in the sorting is the edge index.
  // That's why we are using the std::pair as the key of the map.
  std::map <std::pair <double, int>, int> curvesInRegion;

  // Step 2.1: Iterate over the model faces in the vector (belonged to a specific physics region).
  // Get the Simmetrix model face (gf) from model using the tag.
  for (int i = 0; i < faceTags.size(); i++)
  {
    int faceTag = faceTags[i];
    pGFace gf = static_cast<pGFace>(GM_entityByTag(simModel, 2, faceTag));

    // Step 2.2: If the region provided is a private region (defined by privateId), make sure
    // the given vector of faces of belongs to that specific private region. If not, skip the face.
    if (privateId >= 0)
    {
      int regionId = -1;
      GEN_nativeIntAttribute(gf, "PrivateRegionId", &regionId);
      if (regionId != privateId)
        continue;
    }

    // Step 2.3: Iterate over the model edges of each model face (gf), and find the 
    // edge in the edges-curve map. If not found, skip it. If found, get the psi
    // value and save the edge psi-id pair against flux id in the map.
    int faceType = -1;
    GEN_nativeIntAttribute(gf, "PhysicsRegion", &faceType);
    pPList edges = GF_edges(gf);
    for (int j = 0; j < PList_size(edges); j++)
    {
      pGEdge ge = static_cast<pGEdge>(PList_item(edges, j));
      auto it = edgesToCurveMap.find(ge);
      if (it == edgesToCurveMap.end())
        continue;  // wall edge
     
      // Step 2.3.1: Since first separatrix will appaer as adjacent edge of a core region face,
      // make sure to skip it from the core.
      int curveType = -1;
      GEN_nativeIntAttribute(ge, "CurveType", &curveType);
      if (faceType == static_cast<int>(FaceType::Core) && curveType == static_cast<int>(CurveType::Separatrix))
        continue;


      // Step 2.3.2: If found, get the psi value and save pair (psi-tag) with flux id in the map.
      double psi = fluxCurves[it->second].psiNormOnFlux;
      curvesInRegion[{psi, GEN_tag(ge)}] = it->second;
    }
    PList_delete(edges);
  }

  // Step 3: Take each curve once, in the first region it bounds.
  for (const auto& curve : curvesInRegion)
  {
    int index = curve.second;  // the element in the map is flux Id.
   
    // Step 3.1: if the curve is already used either in this function, or by another
    // region in some other call, skip it. 
    if (curveUsed[index])  
      continue;
    
    // Step 3.2: Else set it to used, and save it to the vector (curves).
    curveUsed[index] = true;
    curves.push_back(fluxCurves[index]);
  }
}

// Utility Functions
// Function to check if a mesh vertex is classified on the wall curve or not.
bool TommsOutput::isOnWall(pVertex v)
{
  // Step 1: Check what model entity mesh vertex is classified on. 
  // If its classified on model vertex, it will return 0.
  // If its classified on model edge, it will return 1.
  switch (EN_whatInType(v)) 
  {
    // Step 1: Depending on the type of model entity it is classified on,
    // check if that model entity is on wall or not.
    case 0:
      return isOnWall(static_cast<pGVertex>(EN_whatIn(v)));
    case 1:
      return isOnWall(static_cast<pGEdge>(EN_whatIn(v)));
  }
  return false;
}

// Function to check if a model edge is classified on the wall curve or not.
bool TommsOutput::isOnWall(pGVertex gv)
{
  // Step 1: Iterate over the model edges adjacent to the model vertex, and
  // if any of that edge is on wall curve, break the loop and return true. 
  bool isEdgeOnWall = false;
  pPList edges = GV_edges(gv);
  for (int i = 0; i < PList_size(edges); i++) 
  {
    pGEdge ge = static_cast<pGEdge>(PList_item(edges, i));
    if (isOnWall(ge))
    {
      isEdgeOnWall = true;
      break;
    }
  }
  PList_delete(edges);

  return isEdgeOnWall;
}

// Function to check if a model edge is classified on the wall curve or not.
bool TommsOutput::isOnWall(pGEdge ge)
{
  // Step 1: Check if the edge has the attribute "CurveTpe". If not, return false.
  // If yes, check if its on the wall curve or not.
  if (GEN_numNativeIntAttribute(ge, "CurveType") == 1)
  {
    int curveType = -1;

    // Step 1.1: If CurveType attribute is Wall, return true.
    GEN_nativeIntAttribute(ge, "CurveType", &curveType);
    if (curveType == static_cast <int>(CurveType::Wall))
      return true;
  }
  return false;
}
