#ifndef MODELINGEQDSK_h
#define MODELINGEQDSK_h

#include "magneticGeometry.h"

class SimmetrixWallCurve{
  public:
    SimmetrixWallCurve(){};
    ~SimmetrixWallCurve();
    SimmetrixWallCurve(const WallCurve& wall, pGModel model);
    void updateSimEdges(pPList updatedList);
    pPList getSimEdges();
    const std::vector <Point>& getPoints();
  private:
    pPList simCurves; 
    pPList simEdges;
    std::vector <Point> points;
};

Model generateModelEqdsk(const PlaneMetaData& planeMetaData, CurveContainer& curvesMetaData);
pGModel generateSimModel(const PlaneMetaData& planeMetaData, CurveContainer& curvesMetaData);
pGFace createModelFace(const PhysicsPoint& oPoint, SimmetrixWallCurve& wall, pGModel model);
pGFace insertClosedCurvesToModelFace(pGModel model, pGFace gf, std::vector <Flux> closedCurves);
void insertSeparatricesToModelFace(pGModel model, pGFace gf, SimmetrixWallCurve& wall, std::vector <Flux> separatrices);

// Model Curve Functions
pCurve createClosedCurve(Flux& f);
pCurve createSepLegCurve(SeparatrixLeg& leg);
pPList createWallCurve(const WallCurve& wallCurve);

// Model Edge Functions
pGEdge createClosedEdge(pGModel model, pCurve simCurve, std::vector <Point> curvePoints);
std::vector <pGEdge> createSeparatrixEdges(pGModel model, SimmetrixWallCurve& wall, Flux& f);
pPList createWallEdges(pGModel model, const pPList& wallSimCurves, const WallCurve& wallCurve);
pGFace insertPeriodicEdgeToFace(pGFace gf, pGEdge ge);

// Functions directly copied from TOMMS with minimal or no cleanup (CLEAN THEM UP WHENEVER HAVE TIME)
void splitWallEdgeAtVertex(pGModel model, SimmetrixWallCurve& wall, pGVertex gv);
double stomms_dist2(double a[3], double b[3]);

#endif
