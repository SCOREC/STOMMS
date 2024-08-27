#include "modelTopology.h"

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

Edge::Edge(pGEdge simEdge):ge(simEdge){}

Face::Face(pGFace simFace):gf(simFace){}

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
    pPList verticesOnEdge = GE_vertices(gEdge);
    e.numVerticesOnE = PList_size(verticesOnEdge);
    for (int i = 0; i < PList_size(verticesOnEdge); i++)
    {
      pGVertex gv = static_cast<pGVertex>(PList_item(verticesOnEdge,i));
      Vertex v(gv);
      e.verticesOnE.push_back(v);
    } 
    PList_delete(verticesOnEdge);
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
    pPList edgesOnFace = GF_edges(gFace);
    f.numEdgesOnF = PList_size(edgesOnFace);
    for (int i = 0; i < PList_size(edgesOnFace); i++)
    {
      pGEdge ge = static_cast<pGEdge>(PList_item(edgesOnFace,i));
      Edge e(ge);
      f.edgesOnF.push_back(e);
    }
    PList_delete(edgesOnFace);
    faces.push_back(f);
  }
  GFIter_delete(fIter);
  assert(numFaces == faces.size());
}
