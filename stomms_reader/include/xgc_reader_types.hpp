#ifndef XGC_READER_TYPES_H
#define XGC_READER_TYPES_H

#include <iostream>

/**
 * Interger identifier for the physical dimensions. <br>
 * (0D = 0, 1D = 1, 2D = 2, 3D = 3).
 */
using DimType = int;

/**
 * Interger identifier for the model and mesh vertices.
 */
using VertexIdType = int;

/**
 * Interger identifier for the model and mesh edges.
 */
using EdgeIdType = int;

/**
 * Interger identifier for the model curves (both physics and physical curves).
 */
using CurveIdType = int;

/**
 * Interger identifier for the model faces.
 */
using SurfaceIdType = int;

/**
 * Integer identifier for the model entities. Although we do have identifier for 
 * individual entities, this is helpful in functions that take in and return 
 * entities of any dimension.
 */
using GeomIdType = int;

/**
 * Integer identifier for the mesh entities. Although we do have identifier for 
 * individual entities, this is helpful in functions that take in and return 
 * entities of any dimensions.
 */
using MeshIdType = int;

/**
 * Interger identifier for the planes. Unique for each poloidal plane.
 */
using PlaneId = int;

/*
 * Physics geometry defines sets of surfaces, edges, vertices that have specific physical meaning.
 * We have definitions of each model entity (vertex, edge, face) in terms of their physics properties.
 */


/**
 * The set of surfaces can be classified into seven different types depending on the physics
 * they corresponds to. The physics regions can be increased/decreased as we move forward.
 */ 
enum class SurfaceType {
  Core,
  ScrapeOffLayer,
  LowFieldSideEdge,
  HighFieldSideEdge,
  NearVacuum,  // between last flux curve and wall curve
  Private,
  None
};

/**
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

/**
 * Two basic types of model vertices.
 * Magnetic Axis: OPoint.
 * Saddle Points of field: Xpoints
 */
enum class PointType {
  OPoint,
  XPoint,
  None
};

/**
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
