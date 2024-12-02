#include "modelTopology.h"

// Model Vertex Definition
void Vertex::setSimVertex(pGVertex simVertex)
{
  gv = simVertex;
  pt = setPointFromVertex();
}

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

const pGVertex& Vertex::getSimVertex()
{
  return gv;
}

const Point& Vertex::getPointAtVertex()
{
  return pt;
}

// Model Edge Definition
void Edge::setSimEdge(pGEdge simEdge)
{
  ge = simEdge;
  setEdge();
}

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

const pGEdge& Edge::getSimEdge()
{
  return ge;
}

const std::vector <Vertex>& Edge::getVerticesOnEdge()
{
  return verticesOnE;
}

// Model Loop Defintion
void Loop::setSimLoop(pGLoopUse simLoop)
{
  gl = simLoop;
  setLoop();
}

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

const std::vector <Edge>& Loop::getEdgesOnLoop()
{
  return edgesOnL;
}

// Model Face Definition
void Face::setSimFace(pGFace simFace)
{
  gf = simFace;
  setFace();
}

void Face::setFace()
{
  setEdgesOnFace();
  setLoopsOnFace();
}

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

const pGFace& Face::getSimFace()
{
  return gf;
}

const std::vector <Edge>& Face::getEdgesOnFace()
{
  return edgesOnF;
}

const std::vector <Loop>& Face::getLoopsOnFace()
{
  return loopsOnF;
}

// Model Definition
void Model::setSimModel(pGModel simModel)
{
  model = simModel;
  setModelVertices();
  setModelEdges();
  setModelFaces();
}
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

const pGModel& Model::getSimModel()
{
  return model;
}

const std::vector <Vertex>& Model::getModelVertices()
{
  return vertices;
}

const std::vector <Edge>& Model::getModelEdges()
{
  return edges;
}

const std::vector <Face>& Model::getModelFaces()
{
  return faces;
}
