#include "modelTopology.h"

Model::Model(pGModel model)
{
  setModel(model);
  setModelVertices(model);
  setModelEdges(model);
  setModelFaces(model);
}

void Model::setModel(pGModel model)
{
  numVertices = GM_numVertices(model);
  numEdges = GM_numEdges(model);
  numFaces = GM_numFaces(model);
}

void Model::setModelVertices(pGModel model)
{
  GVIter vIter = GM_vertexIter(model);
  while (pGVertex gVertex = GVIter_next(vIter))
  {
    Vertex v; 
    v.gv = gVertex; 
    vertices.push_back(v);
  }
  GVIter_delete(vIter); 
  assert(numVertices == vertices.size());
}

void Model::setModelEdges(pGModel model)
{
  GEIter eIter = GM_edgeIter(model);
  while (pGEdge gEdge = GEIter_next(eIter))
  {
    Edge e;
    e.ge = gEdge;
    pPList verticesOnEdge = GE_vertices(gEdge);
    e.numVerticesOnE = PList_size(verticesOnEdge);
    for (int i = 0; i < PList_size(verticesOnEdge); i++)
    {
      Vertex v;
      v.gv = static_cast<pGVertex>(PList_item(verticesOnEdge,i));
      e.verticesOnE.push_back(v);
    } 
    PList_delete(verticesOnEdge);
    edges.push_back(e);
  }
  GEIter_delete(eIter);
  assert(numEdges == edges.size());
}

void Model::setModelFaces(pGModel model)
{
  GFIter fIter = GM_faceIter(model);
  while (pGFace gFace = GFIter_next(fIter))
  {
    Face f;
    f.gf = gFace;
    pPList edgesOnFace = GF_edges(gFace);
    f.numEdgesOnF = PList_size(edgesOnFace);
    for (int i = 0; i < PList_size(edgesOnFace); i++)
    {
      Edge e;
      e.ge = static_cast<pGEdge>(PList_item(edgesOnFace,i));
      f.edgesOnF.push_back(e);
    }
    PList_delete(edgesOnFace);
    faces.push_back(f);
  }
  GFIter_delete(fIter);
  assert(numFaces == faces.size());
}
