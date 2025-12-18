#ifndef XGC_READER_TYPES_H
#define XGC_READER_TYPES_H

#include <iostream>

using GeomIdType = int;
using MeshIdType = int;
using DimType = int;
using PhysicsIndexType = int;
using CurveIdType = int;
using SurfaceIdType = int;
using VertexIdType = int;
using EdgeIdType = int;
using PlaneId = int;


/*
* Physics geometry defines sets of surfaces, edges, vertices that have specific physical meaning.
* We have definitions of each model entity (vertex, edge, face) in terms of their physics properties.
*/


/*
 * The set of surfaces can be classified into seven different types depending on the physics
 * they corresponds to. The physics regions can be increased/decreased as we move forward.
*/ 
enum class SurfaceType {
  Core,
  ScrapeOffLayer,
  LowFieldSideEdge,
  HighFieldSideEdge,
  LowFieldSideNearVacuum,  // between last flux curve and wall curve
  HighFieldSideNearVacuum,
  Private,
  None
};

/*
 * Curves are divided into four basics types. Three of them (closed, open, separatrix) are purely
 * defined by physics and wall curve is the only one that is physical curve.
*/
enum class CurveType {
  Closed,
  Open,
  Separatrix,
  Wall,
  None
};

/*
 * Two basic types of model vertices.
 * Magnetic Axis: OPoint.
 * Saddle Points of field: Xpoints
*/
enum class PointType {
  OPoint,
  XPoint,
  None
};

/*
 * Basic Topology definition of model/mesh entities. The lowest is the vertex which is 
 * considered 0D, and region is the largest entity (3D). 
*/ 
enum class TopoType {
  Vertex = 0,
  Edge = 1,
  Face = 2,
  Region = 3
};

#endif
