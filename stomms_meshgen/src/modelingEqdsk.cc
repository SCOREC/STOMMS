#include "modelingEqdsk.h"

Model generateModelEqdsk(const PlaneMetaData& planeMetaData, CurveContainer& curvesMetaData)
{
  std::cout << "\n========== SIMMETRIX MODEL GENERATION  ==========\n";
  pGModel model = generateSimModel(planeMetaData, curvesMetaData);
}

pGModel generateSimModel(const PlaneMetaData& planeMetaData, CurveContainer& curvesMetaData)
{
  // Step 1: Create a new Simmetrix model(pGModel)
  pProgress prog = Progress_new();
  Progress_setDefaultCallback(prog);
  pGModel model = GM_new(1);
 
  // Step 2: Create the primary model face (defined by wall curve and magnetic axis).
  PhysicsPoint oPoint = curvesMetaData.getOPoints().at(0);
  WallCurve wall = curvesMetaData.getWallCurve();
  pGFace mainFace = createModelFace(oPoint, wall, model);
   
  // Step 3: Insert closed curves to the primary model face.
  insertClosedCurvesToModelFace(model, mainFace, curvesMetaData.getCurvesClosed());

  GM_write(model, "eqdsk.smd", 0, 0);
  Progress_delete(prog);
  return model;
}

pGFace createModelFace(const PhysicsPoint& oPoint, const WallCurve& wall, pGModel model)
{
  // Step 1: Get the curves defining the wall
  pPList simWallCurves = createWallCurve(wall);
  pPList wallEdges = createWallEdges(model, simWallCurves, wall);

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

  return simFace;
}

void insertClosedCurvesToModelFace(pGModel model, pGFace gf, std::vector <Flux> closedCurves)
{
  for (int i = 0; i < closedCurves.size(); i++)
  {
    Flux f = closedCurves[i];
    std::vector <Point> pointsOnCurve = f.fieldPoints;
    pCurve simCurve = createClosedCurve(f);
    pGEdge ge = createClosedEdge(model, simCurve, pointsOnCurve);

    std::array <pGFace, 2> newFaces;
    GM_insertEdgeOnFace(gf, ge, newFaces.data());

    for (int i = 0; i < newFaces.size(); i++)
    {
      pGFace newFace = newFaces[i];
      int dir = GE_dirUsed(ge, newFace);
      if (dir == 0)  // outer face using this edge as inner loop (clockwise)
        gf = newFace;
    }
  }
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
  return ge;
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
    PList_append(wallEdges, ge);
  }

  return wallEdges;
}

/*
pCurve createSeparatrixCurve()
{
  
}

pCurve createOpenCurve()
{}

*/
