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
    //Vertex(pGVertex simVertex);
    void setSimVertex(pGVertex simVertex);
    const Point& getPointAtVertex();
    const pGVertex& getSimVertex();
  private:
    Point setPointFromVertex();
    pGVertex gv;  // geometrix vertex
    Point pt;
};

class Edge{
  public:
    //Edge(pGEdge simEdge);
    void setSimEdge(pGEdge simEdge);
    const pGEdge& getSimEdge();
    const std::vector <Vertex>& getVerticesOnEdge();
  private:
    void setEdge();
    pGEdge ge;  // geometric edge
    std::vector <Vertex> verticesOnE;
};

class Loop{
  public:
    //Loop(pGLoopUse simLoop);
    void setSimLoop(pGLoopUse simLoop);
    const std::vector <Edge>& getEdgesOnLoop();
  private:
    void setLoop();
    pGLoopUse gl;  // geometric loop use
    std::vector <Edge> edgesOnL;

};

class Face{
  public:
    //Face();
    void setSimFace(pGFace simFace);
    const pGFace& getSimFace();
    const std::vector <Edge>& getEdgesOnFace();
    const std::vector <Loop>& getLoopsOnFace();
  private:
    void setFace();
    void setEdgesOnFace();
    void setLoopsOnFace();
    pGFace gf;  // geometric face
    std::vector <Edge> edgesOnF;
    std::vector <Loop> loopsOnF;
};

class Model{
  public:
    //Model();
    const pGModel& getSimModel();
    void setSimModel(pGModel simModel);
    const std::vector <Vertex>& getModelVertices();
    const std::vector <Edge>& getModelEdges();
    const std::vector <Face>& getModelFaces();

  private:
    pGModel model;    

    std::vector <Vertex> vertices;
    std::vector <Edge> edges;
    std::vector <Face> faces;
    
    // Member Functions
    void setModelVertices();
    void setModelEdges();
    void setModelFaces();
};

#endif
