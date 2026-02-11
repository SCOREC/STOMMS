#ifndef MODELINGEQDSK_h
#define MODELINGEQDSK_h

#include "magneticGeometry.h"

Model generateModelEqdsk(const PlaneMetaData& planeMetaData, CurveContainer& curvesMetaData);
pGModel generateSimModel(const PlaneMetaData& planeMetaData, CurveContainer& curvesMetaData);
pGFace createModelFace(const PhysicsPoint& oPoint, const WallCurve& wall, pGModel model);
pPList createWallCurve(const WallCurve& wallCurve);
pPList createWallEdges(pGModel model, const pPList& wallSimCurves, const WallCurve& wallCurve);
#endif
