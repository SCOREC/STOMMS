#ifndef MODELTOPOLOGY_H
#define MODELTOPOLOGY_H

#include "model.h"
#include <cassert>

struct Point{
  double x;
  double y;
  double z;
};

class Vertex{
  public:
    Vertex(pGVertex simVertex);
    Point pt;
  private:
    Point setPointFromVertex();
    pGVertex gv;  // geometrix vertex
};

class Edge{
  public:
    Edge(pGEdge simEdge);
    int numVerticesOnE; // Number of vertices on an edge
    std::vector <Vertex> verticesOnE;
  private:
    pGEdge ge;  // geometric edge
};

class Face{
  public:
    Face(pGFace simFace);
    int numEdgesOnF;
    std::vector <Edge> edgesOnF;
  private:
    pGFace gf;  // geometric face
};

class Model{
  public:
    Model(pGModel simModel);
    
    int numVertices;
    int numEdges;
    int numFaces;
    std::vector <Vertex> vertices;
    std::vector <Edge> edges;
    std::vector <Face> faces;
  private:
    pGModel model;    

    // Member Functions
    void setModel();
    void setModelVertices();
    void setModelEdges();
    void setModelFaces();
};

#endif
