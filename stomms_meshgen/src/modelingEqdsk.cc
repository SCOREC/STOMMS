#include "modelingEqdsk.h"

/***********************************************/
// Model Generation Functions
/***********************************************/
pGModel generateSimModel(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, CurveContainer& curvesMetaData)
{
  std::cout << "\n========== SIMMETRIX MODEL GENERATION  ==========\n";  

  // Step 1: Create a new Simmetrix model(pGModel)
  pGModel model = GM_new(1);
 
  // Step 2: Get oPoint and wall curve information for the primary model face.
  PhysicsPoint oPoint = curvesMetaData.getOPoints().at(0);
  WallCurve wall = curvesMetaData.getWallCurve();

  // Step 3: Create wallcurve. If no wall is desired, use last closed flux curve
  // as wall curve. 
  bool noWall = false;
  SimmetrixWallCurve simWallCurve;
  if (curvesMetaData.getCurvesSeparatrix().size() == 0 && !eqdskData.useWallCurve())
  {
    simWallCurve = SimmetrixWallCurve(curvesMetaData.getCurvesClosed().back(), model);
    noWall = true;
  }
  else
    simWallCurve = SimmetrixWallCurve(wall, model);

  // Step 4: Create the primary model face (defined by wall curve and magnetic axis).
  // If no wall curve is set, use last closed flux curve.
  pGFace mainFace = createModelFace(oPoint, simWallCurve, model);

  // Step 5: Insert closed curve to the primary model face & update the primary face (outerface).
  pGFace updatedMainFace = insertClosedCurvesToModelFace(model, mainFace, curvesMetaData.getCurvesClosed(), noWall);
  insertSeparatricesToModelFace(model, updatedMainFace, simWallCurve, curvesMetaData.getCurvesSeparatrix());

  // Step 6: Classify the model faces.
  classifyModelFaces(model);

  // Step 7: Generate the open flux curves.
  std::vector <Flux> openCurves = genOpenFluxCurves(model, eqdskData, curvesMetaData.getOPoints().at(0), 
                                                    curvesMetaData.getWallCurve(), planeMetaData);

  // Step 8: Insert open curves to the model.
  insertOpenCurvesToModel(model, simWallCurve, openCurves); 
 
  // Step 9: Set open curves and wall edges in curves container.
  curvesMetaData.setOpenCurves(openCurves);
  curvesMetaData.setWallEdges(simWallCurve.getEdges());

  return model;
}

void classifyModelFaces(pGModel model)
{
  std::vector <pGVertex> xPoints = getCriticalPointsOnModel(model, PhysicsPointType::XPoint);
  std::vector <pGVertex> oPoints = getCriticalPointsOnModel(model, PhysicsPointType::OPoint);
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
    else if (!isModelFaceOnCore(gf) && !xPoints.size())  // If no xPoints, tag the outer one as SOL for open curves
      GEN_setNativeIntAttribute(gf, static_cast<int>(FaceType::ScrapeOffLayer), "PhysicsRegion");
    else
      GEN_setNativeIntAttribute(gf, static_cast<int>(FaceType::None), "PhysicsRegion");  
  }
  GFIter_delete(faceIter);
}
