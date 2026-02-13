#ifndef MODELINGEQDSK_h
#define MODELINGEQDSK_h

#include "magneticGeometry.h"

Model generateModelEqdsk(const PlaneMetaData& planeMetaData, CurveContainer& curvesMetaData);
pGModel generateSimModel(const PlaneMetaData& planeMetaData, CurveContainer& curvesMetaData);
pGFace createModelFace(const PhysicsPoint& oPoint, const WallCurve& wall, pGModel model);
void insertClosedCurvesToModelFace(pGModel model, pGFace gf, std::vector <Flux> closedCurves);

// Model Curve Functions
pCurve createClosedCurve(Flux& f);
pPList createWallCurve(const WallCurve& wallCurve);

// Model Edge Functions
pGEdge createClosedEdge(pGModel model, pCurve simCurve, std::vector <Point> curvePoints);
pPList createWallEdges(pGModel model, const pPList& wallSimCurves, const WallCurve& wallCurve);
#endif
