#ifndef MODELINGEQDSK_h
#define MODELINGEQDSK_h

#include "magneticGeometry.h"
#include "genFluxCurvesEqdsk.h"
#include "physicsAttributes.h"

class SimmetrixWallCurve{
  public:
    SimmetrixWallCurve(){};
    SimmetrixWallCurve(const WallCurve& wall, pGModel model);
    void updateSimEdges(std::vector <pGEdge> updatedEdgesVector);
    std::vector <pGEdge> getSimEdges();
    std::vector <Edge> getEdges();
    const std::vector <Point>& getPoints();
  private:
    std::vector <pCurve> simCurves; 
    std::vector <pGEdge> simEdges;
    std::vector <Point> points;
};

pGModel generateSimModel(const PlaneMetaData& planeMetaData, EqdskData& eqdskData, CurveContainer& curvesMetaData);
pGFace createModelFace(const PhysicsPoint& oPoint, SimmetrixWallCurve& wall, pGModel model);
pGFace insertClosedCurvesToModelFace(pGModel model, pGFace gf, std::vector <Flux>& closedCurves);
void insertSeparatricesToModelFace(pGModel model, pGFace gf, SimmetrixWallCurve& wall, std::vector <Flux>& separatrices);
void insertSeparatrixLegsToModel(pGFace gf, const std::map <int, std::vector <pGEdge>>& separatrices);
void insertOpenCurvesToModel(pGModel model, SimmetrixWallCurve& wall, std::vector <Flux>& openCurves);

// Model Curve Functions
pCurve createClosedCurve(Flux& f);
pCurve createOpenCurve(std::vector <Point> points);
pCurve createSepLegCurve(SeparatrixLeg& leg);
std::vector <pCurve> createWallCurve(const WallCurve& wallCurve);

// Model Edge Functions
pGEdge createClosedEdge(pGModel model, pCurve simCurve, std::vector <Point> curvePoints);
std::vector <pGEdge> createSeparatrixEdges(pGModel model, SimmetrixWallCurve& wall, Flux& f, pGVertex vXpt);
std::vector<pGEdge> createWallEdges(pGModel model, std::vector <pCurve>& wallSimCurves, const WallCurve& wallCurve);
pGFace insertPeriodicEdgeToModelFace(pGFace gf, pGEdge ge);
void insertLinearEdgeToModel(pGEdge ge, int endToUse);

// Classification Function
void classifyModelFaces(pGModel model);

// Functions directly copied from TOMMS with minimal or no cleanup (CLEAN THEM UP WHENEVER HAVE TIME)
void splitWallEdgeAtVertex(pGModel model, SimmetrixWallCurve& wall, pGVertex gv);
double stomms_dist2(double a[3], double b[3]);

#endif
