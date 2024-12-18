#include "modelTopology.h"

// Model Vertex Definitions
// Function to get the Simmetrix vertex and to use it in the definition of Vertex.
void Vertex::setSimVertex(pGVertex simVertex)
{
  gv = simVertex;
  pt = setPointFromVertex();
}

// Function to set the physical coordinates of the Vertex.
Point Vertex::setPointFromVertex()
{
  Point p;
  double pt[3];
  GV_point(gv,pt);
  p.x = pt[0];
  p.y = pt[1];
  p.z = pt[2];

  return p;
}

// Fucntion to return the underlying Simmetrix vertex for the Vertex.
const pGVertex& Vertex::getSimVertex()
{
  return gv;
}

// Function to return the physical coordinates of the Vertex in terms of Point.
const Point& Vertex::getPointAtVertex()
{
  return pt;
}

// Model Edge Definitions
// Function to get the Simmetrix edge and to use it in the definition of Edge
void Edge::setSimEdge(pGEdge simEdge)
{
  ge = simEdge;
  setEdge();
  setEdgeProperties();
}

// Function to set the model edge to the the definition of Edge.
// This includes setting up the model vertices on the edge.
void Edge::setEdge()
{
  pPList verticesOnEdge = GE_vertices(ge);
  for (int i = 0; i < PList_size(verticesOnEdge); i++)
  {
    pGVertex gv = static_cast<pGVertex>(PList_item(verticesOnEdge,i));
    Vertex v;

    v.setSimVertex(gv);
    verticesOnE.push_back(v);
  } 
  PList_delete(verticesOnEdge);
}

// Function to set properties on the model edge.
void Edge::setEdgeProperties()
{
  setEdgeParRange();
  setEdgePeriodic();
}

// Function to set the parametric bounds on edge.
void Edge::setEdgeParRange()
{
  double parR[2];
  GE_parRange(ge, &parR[0], &parR[1]); 
  edgeParRange.push_back(parR[0]);
  edgeParRange.push_back(parR[1]);
}

// Function to set the periodic state of the edge.
void Edge::setEdgePeriodic()
{
  if(GE_periodic(ge) > 0)
    periodicEdge = true;
}

// Function to return the underlying Simmetrix model edge for the Edge.
const pGEdge& Edge::getSimEdge()
{
  return ge;
}

// Function to return model vertices on the Edge.
const std::vector <Vertex>& Edge::getVerticesOnEdge()
{
  return verticesOnE;
}

// Function to region the vector of parametric bounds of edge.
const std::vector <double>& Edge::getEdgeParRange()
{
  return edgeParRange; 
}

// Function to return the periodic state of edge.
const bool& Edge::edgeIsPeriodic()
{
  return periodicEdge;
}

// Model Loop Defintions
// A function to get the Simmetrix loop and to use it in the definition of Loop.
void Loop::setSimLoop(pGLoopUse simLoop)
{
  gl = simLoop;
  setLoop();
}

// Function to set the model loop to the the definition of Loop.
// This includes setting up the model edges on the loop.
void Loop::setLoop()
{
  pGEUIter edgesOnLoop = GLU_edgeUseIter(gl);
  while (pGEdgeUse edgeUse = GEUIter_next(edgesOnLoop))
  {
    pGEdge ge =  GEU_edge(edgeUse);
    Edge e;
    e.setSimEdge(ge);
    edgesOnL.push_back(e);
  }  
  GEUIter_delete(edgesOnLoop);
}

// Function to return vector of edges on a loop.
const std::vector <Edge>& Loop::getEdgesOnLoop()
{
  return edgesOnL;
}

// Model Face Definitions
// A function to get the Simmetrix face and to use it in the definition of Face.
void Face::setSimFace(pGFace simFace)
{
  gf = simFace;
  setFace();
}

// Function to set the model face to the definition of Face.
// This includes setting up both model edges and loops on the Face.
void Face::setFace()
{
  setEdgesOnFace();
  setLoopsOnFace();
}

// Function to set the model edges on the model face.
void Face::setEdgesOnFace()
{
  pPList edgesOnFace = GF_edges(gf);
  for (int i = 0; i < PList_size(edgesOnFace); i++)
  {
    pGEdge ge = static_cast<pGEdge>(PList_item(edgesOnFace,i));
    Edge e;
    e.setSimEdge(ge);
    edgesOnF.push_back(e);
  }
  PList_delete(edgesOnFace);
}

// Function to set the model loops on the model face.
void Face::setLoopsOnFace()
{
  int side = 1;
  pGFaceUse fu = GF_use(gf,side);
  pGLUIter loopIter = GFU_loopIter(fu);
  while (pGLoopUse loopUse = GLUIter_next(loopIter))
  {
    Loop l;
    l.setSimLoop(loopUse);
    loopsOnF.push_back(l);
  }
  GLUIter_delete(loopIter);
}

// Fucntion to return Simmetrix model face assoctaed with Face.
const pGFace& Face::getSimFace()
{
  return gf;
}

// Function to return the model edges on the model face.
const std::vector <Edge>& Face::getEdgesOnFace()
{
  return edgesOnF;
}

// Function to return the model loops on the model face.
const std::vector <Loop>& Face::getLoopsOnFace()
{
  return loopsOnF;
}

// Model Definition
// Function to get the pGModel to set it to the model in the class.
void Model::setSimModel(pGModel simModel)
{
  model = simModel;
  setModelVertices();
  setModelEdges();
  setModelFaces();
}

// Function to set model vertices from pGModel on the Model.
void Model::setModelVertices()
{
  GVIter vIter = GM_vertexIter(model);
  while (pGVertex gVertex = GVIter_next(vIter))
  {
    Vertex v;
    v.setSimVertex(gVertex); 
    vertices.push_back(v);
  }
  GVIter_delete(vIter); 
}

// Function to set model edges from pGModel on Model.
void Model::setModelEdges()
{
  GEIter eIter = GM_edgeIter(model);
  while (pGEdge gEdge = GEIter_next(eIter))
  {
    pGEdge ge = gEdge;
    Edge e;
    e.setSimEdge(ge);
    edges.push_back(e);
  }
  GEIter_delete(eIter);
}

// Function to set model edges from pGModel on Model.
void Model::setModelFaces()
{
  GFIter fIter = GM_faceIter(model);
  while (pGFace gFace = GFIter_next(fIter))
  {
    pGFace gf = gFace;
    Face f;
    f.setSimFace(gf);
    faces.push_back(f);
  }
  GFIter_delete(fIter);
}

// Function to return the underlying Simmetrix model (pGModel).
const pGModel& Model::getSimModel()
{
  return model;
}

// Function to return a vector of  model vertices on the model.
const std::vector <Vertex>& Model::getModelVertices()
{
  return vertices;
}

// Function to return a vector of  model edges on the model.
const std::vector <Edge>& Model::getModelEdges()
{
  return edges;
}

// Function to return a vector of  model faces on the model.
const std::vector <Face>& Model::getModelFaces()
{
  return faces;
}
