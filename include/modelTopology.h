#ifndef MODELTOPOLOGY_H
#define MODELTOPOLOGY_H

#include "MeshSim.h"
#include "SimAdvModel.h"
#include "SimModel.h"
#include "SimUtil.h"
#include <cassert>
#include <vector>

struct Point{
  double x;
  double y;
  double z;
};

struct PhysicsPoint{
  Point pt;  // Point in physical space
  double psi;  // associated psi value
};

class Vertex{
  public:
    Vertex(pGVertex simVertex);
    Point pt;
    pGVertex getSimVertex();
  private:
    Point setPointFromVertex();
    pGVertex gv;  // geometrix vertex
};

class Edge{
  public:
    Edge(pGEdge simEdge);
    int numVerticesOnE; // Number of vertices on an edge
    std::vector <Vertex> verticesOnE;
    pGEdge getSimEdge();
  private:
    void setEdge();
    pGEdge ge;  // geometric edge
};

class Loop{
  public:
    Loop(pGLoopUse simLoop);
    int numEdgesOnL;
    std::vector <Edge> edgesOnL;
  private:
    void setLoop();
    pGLoopUse gl;  // geometric loop use
};

class Face{
  public:
    Face(pGFace simFace);
    int numEdgesOnF;
    std::vector <Edge> edgesOnF;
    int numLoopsOnF;
    std::vector <Loop> loopsOnF;
    pGFace getSimFace();
  private:
    void setFace();
    void setEdgesOnFace();
    void setLoopsOnFace();
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
    pGModel getSimModel();
  private:
    pGModel model;    

    // Member Functions
    void setModel();
    void setModelVertices();
    void setModelEdges();
    void setModelFaces();
};

#endif
