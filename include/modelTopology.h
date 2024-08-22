#ifndef MODELTOPOLOGY_H
#define MODELTOPOLOGY_H

#include "model.h"
#include <cassert>

class Vertex{
  public:
    pGVertex gv;	// geometrix vertex
};

class Edge{
  public:
    pGEdge ge;	// geometric edge
    int numVerticesOnE; // Number of vertices on an edge
    std::vector <Vertex> verticesOnE;
};

class Face{
  public:
    pGFace gf;	// geometric face
    int numEdgesOnF;
    std::vector <Edge> edgesOnF;
};

class Model{
  public:
    int numVertices;
    int numEdges;
    int numFaces;
    std::vector <Vertex> vertices;
    std::vector <Edge> edges;
    std::vector <Face> faces;
    Model(pGModel model);
  private:
    void setModel(pGModel model);
    void setModelVertices(pGModel model);
    void setModelEdges(pGModel model);
    void setModelFaces(pGModel model);
};

#endif
