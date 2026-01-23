#include "genFluxCurvesEqdsk.h"

/**
 * in modelingEqdsk.cc 
 * Model genSimModel(const EqdskData& eqdsk, ....any other info)
 * .. genModelFaces
 * .. genModelVertices
 * .. genModelCurves
 * .. genModelEdges ...
 */

/**
 * in modelDataEqdsk()
 * Set Basic properties of whatever ...
 */

/**
 * Top Level function
 * genFluxCurves(const EqdskData& eqdsk)
 * {
 *   genFluxClosed();
 *   genFluxSeparatrix();
 *   genFluxOpen();
 *     genFluxInPrivateRegion ..???
 *
 * }
 */

std::vector <Flux> genClosedFluxCurves(const std::vector <double>& corePsiValues, EqdskData& eqdskData)
{
  std::vector <PhysicsPoint> startPoints = getStartPointClosed(corePsiValues, eqdskData);

  for (int i = 0; i < startPoints.size(); i++)
  {
    double psi = startPoints[i].getPsi();
    double psiNorm = eqdskData.convertPsiToNorm(psi);
    
  }

}
