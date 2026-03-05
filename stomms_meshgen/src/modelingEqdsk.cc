#include "modelingEqdsk.h"

/***********************************************/
// Class SimmetrixWallCurve
/***********************************************/
SimmetrixWallCurve::SimmetrixWallCurve(const WallCurve& wall, pGModel model)
{
  // Step 1: Get the curves defining the wall
  points = wall.getPoints();
  simCurves = createWallCurve(wall);
  simEdges = createWallEdges(model, simCurves, wall);  
}

SimmetrixWallCurve::~SimmetrixWallCurve()
{
  if (PList_size(simCurves) > 0)
  {
    PList_clear(simCurves);
    PList_delete(simCurves);
  }
  if (PList_size(simEdges) > 0)
  {
    PList_clear(simEdges);
    PList_delete(simEdges);
  }
}

void SimmetrixWallCurve::updateSimEdges(pPList updatedList)
{
  simEdges = updatedList;
}

pPList SimmetrixWallCurve::getSimEdges()
{
  return simEdges;
}

const std::vector <Point>& SimmetrixWallCurve::getPoints()
{
  return points;
}

/***********************************************/
// Model Generation Functions
/***********************************************/
pGModel generateSimModel(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, CurveContainer& curvesMetaData)
{
  std::cout << "\n========== SIMMETRIX MODEL GENERATION  ==========\n";  

  // Step 1: Create a new Simmetrix model(pGModel)
  pGModel model = GM_new(1);
 
  // Step 2: Create the primary model face (defined by wall curve and magnetic axis).
  PhysicsPoint oPoint = curvesMetaData.getOPoints().at(0);
  WallCurve wall = curvesMetaData.getWallCurve();
  SimmetrixWallCurve simWallCurve(wall, model);
  pGFace mainFace = createModelFace(oPoint, simWallCurve, model);

  // Step 3: Insert closed curve to the primary model face & update the primary face (outerface).
  pGFace updatedMainFace = insertClosedCurvesToModelFace(model, mainFace, curvesMetaData.getCurvesClosed());
  insertSeparatricesToModelFace(model, updatedMainFace, simWallCurve, curvesMetaData.getCurvesSeparatrix());

  // Step 4: Classify the model faces.
  classifyModelFaces(model);

  // Step 5: Update the model with open curves.
  genOpenFluxCurves(model, eqdskData, curvesMetaData.getWallCurve(), planeMetaData);
  
  return model;
}

pGFace createModelFace(const PhysicsPoint& oPoint, SimmetrixWallCurve& wall, pGModel model)
{
  std::cout << ".......... Creating Primary Model Face\n\t   between Wall & oPoint\n";

  // Step 1: Get the wallEdges
  pPList wallEdges = wall.getSimEdges();

  int nEdges = PList_size(wallEdges);
  std::vector <pGEdge> edges;
  std::vector <int> dirs;
  for (int i = 0; i < nEdges; i++) 
  {
    edges.push_back(static_cast<pGEdge>(PList_item(wallEdges, i)));
    dirs.push_back(1);  
  }

  std::array <double, 4> bounds = getCurveBounds(wall.getPoints());
  std::vector <double> corner = {bounds[0], bounds[1], 0.0};
  std::vector <double> xpt = {bounds[2], bounds[1], 0.0};
  std::vector <double> ypt = {bounds[0], bounds[3], 0.0};
  pSurface pln = SSurface_createPlane(corner.data(), xpt.data(), ypt.data());
  pGFace simFace = GR_createFace(GIP_outerRegion(GM_rootPart(model)), nEdges, edges.data(), dirs.data(), 1, 0, pln, 1);

  // Step 2: Add axis point to the model
  Point oPt = oPoint.getPoint();
  std::vector <double> oPtCoord= {oPt.x, oPt.y, 0.0};
  pGVertex axisVertex = GF_createVertex(simFace, oPtCoord.data()); 
  GEN_setNativeIntAttribute(axisVertex, static_cast<int>(PointType::OPoint), "PointType");

  return simFace;
}

pGFace insertClosedCurvesToModelFace(pGModel model, pGFace gf, std::vector <Flux> closedCurves)
{
  std::cout << ".......... Creating Closed Model Edges\n";
  for (int i = 0; i < closedCurves.size(); i++)
  {
    Flux f = closedCurves[i];
    double psiNorm = f.psiNormOnFlux;
    std::vector <Point> pointsOnCurve = f.fieldPoints;
    pCurve simCurve = createClosedCurve(f);
    pGEdge ge = createClosedEdge(model, simCurve, pointsOnCurve);
    GEN_setNativeDoubleAttribute(ge, psiNorm, "PsiNorm");
    pGFace newFace = insertPeriodicEdgeToModelFace(gf, ge);
    gf = newFace;
  }

  pGFace updatedFace = gf;
  return updatedFace;
}

bool vertexExist(std::vector <double> xyz, const std::vector <pGVertex>& vertices)
{
  double tol = 1e-8;  // tolerance
  for (int i = 0; i < vertices.size(); i++)
  {
    double xyzTest[3];
    GV_point(vertices[i], xyzTest);
    if (fabs(xyz[0] - xyzTest[0]) < tol && fabs(xyz[1] - xyzTest[1]) < tol)
      return true;
  }

  return false;
}

void insertSeparatricesToModelFace(pGModel model, pGFace gf, SimmetrixWallCurve& wall, std::vector <Flux> separatrices)
{
  std::cout << ".......... Creating Separatrix Model Edges\n";
  std::map <int, std::vector <pGEdge>> sepEdgesMap;
  std::vector <pGVertex> xPtVertices;  // to store xpt vertices to avoid repitition
  pGVertex vXpt; 

  // Step 1: Iterate over the separatrices and create model edges for each leg.
  for (int i = 0; i < separatrices.size(); i++)
  {
    // Step 2: If there is no xPt vertex for xpt in flux curve f, create one. 
    // if does, use the existing one. Save every new xPt vertex to the vector.
    Flux f = separatrices[i];
    Point xPt = f.xPoint.getPoint();
    double psiNorm  =f.psiNormOnFlux;
    std::vector <double> vCoord = {xPt.x, xPt.y, xPt.z};
    if (!vertexExist(vCoord, xPtVertices))
    {
      vXpt = GR_createVertex(GIP_outerRegion(GM_rootPart(model)), vCoord.data()); 
      GEN_setNativeIntAttribute(vXpt, static_cast<int>(PointType::XPoint), "PointType");
      GEN_setNativeDoubleAttribute(vXpt, psiNorm, "PsiNorm");
      xPtVertices.push_back(vXpt);
    }

    // Step 3: Get the model edges (legs) for each separatrix, and save them in map.
    std::vector <pGEdge> sepEdges = createSeparatrixEdges(model, wall, f, vXpt);
    sepEdgesMap[i] = sepEdges;
  }

  // Step 4: Iterate over the model edges created
  insertSeparatrixLegsToModel(gf, sepEdgesMap); 
}

void insertSeparatrixLegsToModel(pGFace gf, const std::map <int, std::vector <pGEdge>>& separatrices)
{
  // Step 1: Iterate over the individual separatrices and based on the type keep inserting
  // the model edges of separatrix curves to the model face.
  for (int i = 0; i < separatrices.size(); i++)
  {
    std::vector <pGEdge> sepEdges = separatrices.at(i);
    
    // Step 2: If three legs, insert one closed leg and two open legs.
    if (sepEdges.size() == 3)
    {      
      pGFace newFace = insertPeriodicEdgeToModelFace(gf, sepEdges[1]); // closed part
      gf = newFace;
      insertLinearEdgeToModel(sepEdges[0], 0);  // Leg 1
      insertLinearEdgeToModel(sepEdges[2], 1);  // Leg 2
    }
    
    // Step 3: if two legs, insert two separatrices leg
    if (sepEdges.size() == 2)
    {
      insertLinearEdgeToModel(sepEdges[0], 0);
      insertLinearEdgeToModel(sepEdges[1], 1);
    }
  }
}

// Return the outer face if it splits face into two
pGFace insertPeriodicEdgeToModelFace(pGFace gf, pGEdge ge)
{
  std::array <pGFace, 2> newFaces;
  GM_insertEdgeOnFace(gf, ge, newFaces.data());

  for (int i = 0; i < newFaces.size(); i++)
  {
    pGFace newFace = newFaces[i];
    int dir = GE_dirUsed(ge, newFace);
    if (dir == 0)  // outer face using this edge as inner loop (clockwise)
      gf = newFace;
  }
  pGFace updatedFace = gf;
  return updatedFace; 
}

// Given an edge, and what endpoint to use (start, end), inserts linear
// edge to the model.
void insertLinearEdgeToModel(pGEdge ge, int endToUse)
{
  // Step 1: Only allowe ends are 0 and 1 for linear edge, throw error if
  // incorrect end is given
  if (endToUse < 0 || endToUse > 1)
  {
    std::cerr << "ERROR: For linear edge, only allowed ends to use are: 0,1\n";
    std::cerr << "Given end = " << endToUse << " is not valid\n";
    exit(1);
  }

  pGVertex gv = GE_vertex(ge, endToUse);
  pPList facesOnV = GV_faces(gv);
  assert(PList_size(facesOnV) == 1);
  pGFace gf = static_cast<pGFace>(PList_item(facesOnV, 0));

  std::array <pGFace, 2> newFaces;  // return faces in case of split
  GM_insertEdgeOnFace(gf, ge, newFaces.data());  // leg 2
  PList_delete(facesOnV);
}

pCurve createClosedCurve(Flux& f)
{
  pCurve curve;
  int numPts = f.fieldPoints.size();
  std::vector <double> pointsOnCurve;
  for (int i = 0; i < numPts; i++)
  {
    Point pt = f.fieldPoints[i];
    pointsOnCurve.push_back(pt.x);
    pointsOnCurve.push_back(pt.y);
    pointsOnCurve.push_back(0.0);
  }

  curve = SCurve_createPiecewiseLinear(numPts, pointsOnCurve.data());
  return curve;
}

pCurve createSepLegCurve(SeparatrixLeg& leg)
{
  pCurve curve;
  int numPts = leg.fieldPoints.size();
  std::vector <double> pointsOnCurve;
  for (int i = 0; i < numPts; i++)
  {
    Point pt = leg.fieldPoints[i];
    pointsOnCurve.push_back(pt.x);
    pointsOnCurve.push_back(pt.y);
    pointsOnCurve.push_back(0.0);
  }

  curve = SCurve_createPiecewiseLinear(numPts, pointsOnCurve.data());
  return curve;
}

pPList createWallCurve(const WallCurve& wallCurve)
{
  pPList simWallCurves = PList_new();
  std::vector <Point> wallPoints = wallCurve.getPoints();
  bool wallSplineOn = false;  // get it from input later

  if (wallSplineOn)
     std::cout << "Add this option later\n"; // do here - add this option later here
  else
  {
    int numPts = wallPoints.size();
    for (int i = 0; i < numPts - 1; i++)
    {
      std::vector <double> pt1 = {wallPoints[i].x, wallPoints[i].y, 0.0};
      std::vector <double> pt2 = {wallPoints[i+1].x, wallPoints[i+1].y, 0.0};
      PList_append(simWallCurves, SCurve_createLine(pt1.data(), pt2.data()));
    }
  }

  return simWallCurves;
}

pGEdge createClosedEdge(pGModel model, pCurve simCurve, std::vector <Point> curvePoints)
{
  std::vector <double> xyz = {curvePoints[0].x, curvePoints[0].y, 0.0};
  pGVertex gv = GR_createVertex(GIP_outerRegion(GM_rootPart(model)), xyz.data());
  pGEdge ge = GR_createEdge(GIP_outerRegion(GM_rootPart(model)), gv, gv, simCurve, 1);
  GEN_setNativeIntAttribute(ge, static_cast<int>(CurveType::Closed), "CurveType");
  return ge;
}

std::vector <pGEdge> createSeparatrixEdges(pGModel model, SimmetrixWallCurve& wall, Flux& f, pGVertex vXpt)
{
  /* We have three possibilites. 
   * First leg: create vertex (v1) on wall, and x-point (v2) vertex. 
   * Second leg (if closed): already have v2, just create a close loop.
   * Second leg (if open) or third leg: already have v2, create a end 
   * vertex on the wall. 
   * Legs are already sticthed together in right order. */

  pGVertex vStart, vEnd;
  std::vector <pGEdge> edgesOnSep;
  double psiNorm = f.psiNormOnFlux;  // to attached to the model entities
  for (int i = 0; i < f.separatrixLegs.size(); i++)
  {
    SeparatrixLeg leg = f.separatrixLegs[i];
    pCurve simCurve = createSepLegCurve(leg);
    pGEdge ge;
    if (leg.numXPts == 1 && leg.xPtAtEnd)
    {
      std::vector <double> xyz1 = {leg.fieldPoints[0].x, leg.fieldPoints[0].y, 0.0};
      vStart = GR_createVertex(GIP_outerRegion(GM_rootPart(model)), xyz1.data());
      ge = GR_createEdge(GIP_outerRegion(GM_rootPart(model)), vStart, vXpt, simCurve, 1);
      splitWallEdgeAtVertex(model, wall, vStart);
      edgesOnSep.push_back(ge);
    }
    else if (leg.numXPts == 2)
    {
      ge = GR_createEdge(GIP_outerRegion(GM_rootPart(model)), vXpt, vXpt, simCurve, 1);
      edgesOnSep.push_back(ge);
    } 
    else if (leg.numXPts == 1 && leg.xPtAtStart)
    {
      std::vector <double> xyz = {leg.fieldPoints.back().x, leg.fieldPoints.back().y, 0.0};
      vEnd = GR_createVertex(GIP_outerRegion(GM_rootPart(model)), xyz.data());
      ge = GR_createEdge(GIP_outerRegion(GM_rootPart(model)), vXpt, vEnd, simCurve, 1);
      splitWallEdgeAtVertex(model, wall, vEnd);
      edgesOnSep.push_back(ge);
    } 
    GEN_setNativeIntAttribute(ge, static_cast<int>(CurveType::Separatrix), "CurveType");
    GEN_setNativeDoubleAttribute(ge, psiNorm, "PsiNorm");   
  }
  return edgesOnSep; 
}

pPList createWallEdges(pGModel model, const pPList& wallSimCurves, const WallCurve& wallCurve)
{
  pPList wallEdges = PList_new();
  std::array <pGVertex, 2> gv;
  pGVertex fv;
  std::vector <Point> wallPoints = wallCurve.getPoints();
  for (int i = 0; i < PList_size(wallSimCurves); i++)
  {
    pCurve sCurve = static_cast<pCurve>(PList_item(wallSimCurves, i));
    std::array <double, 3> v1, v2;
    v1[2] = v2[2] = 0.0;
    if (i)
    {
      gv[0] = gv[1];
      if (i == PList_size(wallSimCurves) - 1)
        gv[1] = fv;
      else
      {
        v2[0] = wallPoints[i+1].x;
        v2[1] = wallPoints[i+1].y;
        gv[1] = GR_createVertex(GIP_outerRegion(GM_rootPart(model)), v2.data());  
      }
    }
    else  // first curve
    {
      v1[0] = wallPoints[i].x;
      v1[1] = wallPoints[i].y;
      gv[0] = fv = GR_createVertex(GIP_outerRegion(GM_rootPart(model)), v1.data());
      v2[0] = wallPoints[i+1].x;
      v2[1] = wallPoints[i+1].y;
      gv[1] = GR_createVertex(GIP_outerRegion(GM_rootPart(model)), v2.data()); 
    }
    pGEdge ge = GR_createEdge(GIP_outerRegion(GM_rootPart(model)), gv[0], gv[1], sCurve, 1);
    GEN_setNativeIntAttribute(ge, static_cast<int>(CurveType::Wall), "CurveType");
    PList_append(wallEdges, ge);
  }

  return wallEdges;
}


void splitWallEdgeAtVertex(pGModel model, SimmetrixWallCurve& wall, pGVertex gv)
{
  pGEdge ge, splitEdge = 0;
  pGVertex newVertex;
  pPList SimEdges = wall.getSimEdges();

  // Find the line segment that is closest to the vertex point
  double xyz[3];
  GV_point(gv, xyz);
  double closest = DBL_MAX;
  double par;
  int idx;

  for (int i = 0; i < PList_size(SimEdges); i++) 
  {
    ge = static_cast<pGEdge>(PList_item(SimEdges, i));
    double outPt[3], outPar;
    GE_closestPoint(ge, xyz, outPt, &outPar);
    double dist = stomms_dist2(outPt, xyz);
    if (dist < closest) 
    {
      closest = dist;
      par = outPar;
      splitEdge = ge;
      idx = i;
    }
  }
 
  if (splitEdge) 
  {
    newVertex = GE_split(splitEdge, par);
    if (!newVertex)
      std::cout<<"*** Unexpected: Unable to insert vertex into the wall\n";
    else 
    {
      pPList newEdges = GV_edges(newVertex);
      double coord[3];

      GM_mergeVertices(gv, newVertex);

      pPList newList = PList_new();
      for(int i = 0; i<idx; i++)
        PList_append(newList, PList_item(SimEdges, i));
      PList_append(newList, PList_item(newEdges, 0));
      PList_append(newList, PList_item(newEdges, 1));
      PList_delete(newEdges);
      for(int i = idx+1; i<PList_size(SimEdges); i++)
        PList_append(newList, PList_item(SimEdges, i));
      PList_delete(SimEdges);
      wall.updateSimEdges(newList);
    }
  }
}

double stomms_dist2(double a[3], double b[3])
{
  return (a[0] - b[0])*(a[0] - b[0]) + (a[1] - b[1])*(a[1] - b[1]) +
         (a[2] - b[2])*(a[2] - b[2]);
}

void classifyModelFaces(pGModel model)
{
  std::vector <pGVertex> xPoints = getCriticalPointsOnModel(model, PointType::XPoint);
  std::vector <pGVertex> oPoints = getCriticalPointsOnModel(model, PointType::OPoint);
  std::array<double,3> oCoord, xCoord;
  GV_point(oPoints[0], oCoord.data());
  for (int i = 0; i < xPoints.size(); i++)
  {
    pGVertex gv = xPoints[i];
    GV_point(gv,xCoord.data());
    int index = (xCoord[1] < oCoord[1]) ? 0 : 1; 
    if (i <= 1)  // First two separatrices
      tagModelFacesAdjacentToXPoint(gv, index);    
  }

  GFIter faceIter = GM_faceIter(model);
  while (pGFace gf = GFIter_next(faceIter))
  {
    if (GEN_numNativeIntAttribute(gf, "PhysicsRegion"))
      continue;  // already tagged

    if (isModelFaceOnCore(gf))
      GEN_setNativeIntAttribute(gf, static_cast<int>(FaceType::Core), "PhysicsRegion");
    else
      GEN_setNativeIntAttribute(gf, static_cast<int>(FaceType::None), "PhysicsRegion");  
  }
  GFIter_delete(faceIter);

}
