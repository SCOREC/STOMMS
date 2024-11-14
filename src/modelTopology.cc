#include "modelTopology.h"

// Model Vertex Definition
Vertex::Vertex(pGVertex simVertex):gv(simVertex)
{
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

pGVertex Vertex::getSimVertex()
{
  return gv;
}

// Model Edge Definition
Edge::Edge(pGEdge simEdge):ge(simEdge)
{
  setEdge();
}

void Edge::setEdge()
{
  pPList verticesOnEdge = GE_vertices(ge);
  numVerticesOnE = PList_size(verticesOnEdge);
  for (int i = 0; i < PList_size(verticesOnEdge); i++)
  {
    pGVertex gv = static_cast<pGVertex>(PList_item(verticesOnEdge,i));
    Vertex v(gv);
    verticesOnE.push_back(v);
  } 
  PList_delete(verticesOnEdge);
}

pGEdge Edge::getSimEdge()
{
  return ge;
}

// Model Loop Defintion
Loop::Loop(pGLoopUse simLoop):gl(simLoop)
{
  setLoop();
}

void Loop::setLoop()
{
  pGEUIter edgesOnLoop = GLU_edgeUseIter(gl);
  while (pGEdgeUse edgeUse = GEUIter_next(edgesOnLoop))
  {
    pGEdge ge =  GEU_edge(edgeUse);
    Edge e(ge);
    edgesOnL.push_back(e);
  }  
  GEUIter_delete(edgesOnLoop);
  numEdgesOnL = edgesOnL.size();
}

// Model Face Definition
Face::Face(pGFace simFace):gf(simFace)
{
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
  numEdgesOnF = PList_size(edgesOnFace);
  for (int i = 0; i < PList_size(edgesOnFace); i++)
  {
    pGEdge ge = static_cast<pGEdge>(PList_item(edgesOnFace,i));
    Edge e(ge);
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
    Loop l(loopUse);
    loopsOnF.push_back(l);
  }
  GLUIter_delete(loopIter);
  numLoopsOnF = loopsOnF.size();
}

pGFace Face::getSimFace()
{
  return gf;
}

// Model Definition
Model::Model(pGModel simModel):model(simModel)
{
  setModel();
  setModelVertices();
  setModelEdges();
  setModelFaces();
}

void Model::setModel()
{
  numVertices = GM_numVertices(model);
  numEdges = GM_numEdges(model);
  numFaces = GM_numFaces(model);
}

void Model::setModelVertices()
{
  GVIter vIter = GM_vertexIter(model);
  while (pGVertex gVertex = GVIter_next(vIter))
  {
    Vertex v(gVertex); 
    vertices.push_back(v);
  }
  GVIter_delete(vIter); 
  assert(numVertices == vertices.size());
}

void Model::setModelEdges()
{
  GEIter eIter = GM_edgeIter(model);
  while (pGEdge gEdge = GEIter_next(eIter))
  {
    pGEdge ge = gEdge;
    Edge e(ge);
    edges.push_back(e);
  }
  GEIter_delete(eIter);
  assert(numEdges == edges.size());
}

void Model::setModelFaces()
{
  GFIter fIter = GM_faceIter(model);
  while (pGFace gFace = GFIter_next(fIter))
  {
    pGFace gf = gFace;
    Face f(gf);
    faces.push_back(f);
  }
  GFIter_delete(fIter);
  assert(numFaces == faces.size());
}

pGModel Model::getSimModel()
{
  return model;
}
