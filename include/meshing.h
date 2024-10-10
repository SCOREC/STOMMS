#ifndef MESHING_H
#define MESHING_H

#include "model.h"
#include <assert.h>

/*
 * Given the simmetrix model and planes data, this function generates
 * and return a simmetrix mesh.
 * pGModel model (in): The Simmetrix model created from modeling step.
 * std::vector <Plane> planes (in): The data for each plane  as object Plane is written to this container.
 * args* a (in): Input parameters.
 * returns a simmetrix mesh (pMesh).
*/
pMesh meshing(pGModel model, std::vector <Plane> planes, args* a);

/*
 * To specify mesh vertex at O-point (origin/axis of the poloidal plane).
 * pMesh Mesh (in)(out): Gets the pMesh mesh as input and update the specified entities on it.
 * pGVertex axis (in): The model vertex at the O-point.
 * int& numSpecifiedVert (in)(out): To keep record of global number of specified vertices. Its
 *                                  updated in every call of this the function.
 * returns the index of the mesh vertex specified at the O-point.
*/
int specifyMeshVertexOnAxis(pMesh mesh, pGVertex axis, int& numSpecifiedVert);

/* To specify mesh vertices and edges on flux curves (model edges)
 * Assumes periodic edges. Write a new function if edges are open 
 * or have some other behaviour.
 * pMesh Mesh (in)(out): Gets the pMesh mesh as input and update the specified entities on it.
 * Flux f (in): The flux curve on which mesh entities are being specified.
 * int& numSpecifiedVert (in)(out): To keep record of global number of specified vertices. Its 
 * 				    updated in every call of this the function.
 * args* a (in): Input parameters.
 * returns a vector (int) that contains the indices of specified mesh vertices on flux curve f.
*/
std::vector <int> specifyMeshEnt(pMesh mesh, Flux f, int& numSpecifiedVert, args* a);

/*
 * To specify mesh edges on the model face that is adjacent to the O-point.
 * pMesh Mesh (in)(out): Gets the pMesh mesh as input and update the specified entities on it.
 * pGVertex axis (in): The model vertex at the O-point.
 * int indxAtAxis (in): The index of mesh vertex specified at the axis (O-point).
 * std::vector <int> indicesOnInnermostFlux (in): List of indices of mesh vertices specified on the innermost 
 *                                                flux curve (first flux curve after O-point)
*/ 
void specifyMeshEdgesOnFace(pMesh mesh, pGVertex axis, int indxAtAxis, std::vector <int> indicesOnInnermostFlux);

/*
 * For a given flux curve, and the index of the field following point, return the parametric value of point on the edge.
 * Flux f (in): The flux curve on which the parametric value is desired.
 * ind indx (in): The index of the point in the vector of the field following points.
 * returns the parametric value (double)  of the given point on the edge on flux curve f.
 */
double parValueOnEdge(Flux f, int indx);
#endif
