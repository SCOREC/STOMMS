#ifndef MODELTOPOLOGY_H
#define MODELTOPOLOGY_H

#include "MeshSim.h"
#include "SimAdvModel.h"
#include "SimModel.h"
#include "GeomSimVmec.h"
#include "SimUtil.h"
#include <cassert>
#include <vector>

// Struct Point defines a point in terms of physical coordinates (x,y,z).
struct Point{
  Point(){};
  Point(double xCoord, double yCoord);
  Point(double xCoord, double yCoord, double zCoord);
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
};

// Vertex class contains the information on geometric vertex
// and related queries. 
class Vertex{
  public:
    
    /*
     * Gets the Simmetrix vertex and uses it in the definition of Vertex. 
     * pGVertex simVertex (in): The simmetrix vertex used for the definition of Vertex.
     */ 
    void setSimVertex(pGVertex simVertex);

    /*
     * Function to return the physical coordinates of the Vertex in terms of Point.
     */ 
    const Point& getPointAtVertex();

    /*
     * Fucntion to return the underlying Simmetrix vertex for the Vertex.
     */ 
    const pGVertex& getSimVertex();
  private:
    /*
     * Function to set the physical coordinates of the Vertex.
     */   
    Point setPointFromVertex();
    pGVertex gv;  // geometrix vertex
    Point pt;  // Physical coordinates of the Vertex stored as Point.
};

// Edge class contains the information on geometric edge
// and related queries. 
class Edge{
  public:
    /*
     * Gets the Simmetrix edge and uses it in the definition of Edge. 
     * pGEdge simEdge (in): The simmetrix model edge used in the definition of Edge.
     */ 
    void setSimEdge(pGEdge simEdge);

    /*
     * Function to return the underlying Simmetrix model edge for the Edge.
     */   
    const pGEdge& getSimEdge();

    /*
     * Function to return model vertices on the Edge.
     */
    const std::vector <Vertex>& getVerticesOnEdge();

    /*
     * Function to get the parametric range of the edge.
     * returns a vector of size 2, with starting and ending parametric bounds.
     */ 
    const std::vector <double>& getEdgeParRange();

    /*
     * Function to check if the edge is periodic or not.
     * returns true if edge is periodic, otherwise false.
     */ 
    const bool& edgeIsPeriodic();

    /**
     * Function to get the length of the model edge.
     */ 
    double getEdgeLength() const;

    /**
     * Function to get length of an edge between two specified points on edge.
     * @param t0: parametric location of start point (first point).
     * @param t1: parametric location of end point (second point).
     * @return the length between parametric location t0 and t1 on the edge.
     */ 
    double getEdgePartialLength(const double& t0, const double& t1) const;
  private:
    /*
     * Function to set the model edge to the the definition of Edge.
     */ 
    void setEdge();

    /*
     * Function to set the properties on the model edge.
     */  
    void setEdgeProperties();

    /*
     *  Function to set the edge parametric bounds.
     */ 
    void setEdgeParRange();
    
    /*
     * Function to check if edge is periodic or not and set it to edge properties.
     */ 
    void setEdgePeriodic();

    pGEdge ge;  // geometric edge
    std::vector <double> edgeParRange;  // vector of size 2 for the edge parametric bounds.
    bool periodicEdge = false;  // a variable to store the periodic state of edge.
    std::vector <Vertex> verticesOnE;  // a vector to hold vertices on an edge.
};

// Loop class contains the information on model loop 
// and related queries.
class Loop{
  public:
    /*
     * Gets the Simmetrix loop and uses it in the definition of Loop. 
     * pGLoopUse simLoop (in): The simmetrix model loop used in the definition of Loop.
     */ 
    void setSimLoop(pGLoopUse simLoop);

    /*
     * Function to return set of edges on a loop.
     */ 
    const std::vector <Edge>& getEdgesOnLoop();
  private:
    /*
     * Function to set the model loop to the the definition of Loop.
     */ 
    void setLoop();
    pGLoopUse gl;  // geometric loop use
    std::vector <Edge> edgesOnL;  // a vector conataining the edges on the model loop. 

};

// Face class contains the information of model face 
// and related queries.
class Face{
  public:
    /*
     * Gets the Simmetrix face and uses it in the definition of Face. 
     * pGFace simFace (in): The simmetrix model face used in the definition of Face.
     */ 
    void setSimFace(pGFace simFace);
 
    /*
     * Fucntion to return Simmetrix model face assoctaed with Face.
     */ 
    const pGFace& getSimFace();

    /*
     * Function to return the model edges on the model face.
     */ 
    const std::vector <Edge>& getEdgesOnFace();

    /*
     * Function to return the model loops on the model face.
     */ 
    const std::vector <Loop>& getLoopsOnFace();
  private:
    /*
     * Function to set the model face to the definition of Face.
     */ 
    void setFace();

    /*
     * Function to set the model edges on the model face.
     */ 
    void setEdgesOnFace();

    /*
     * Function to set the model loops on the model face.
     */ 
    void setLoopsOnFace();
    pGFace gf;  // geometric face
    std::vector <Edge> edgesOnF;  // a vector of edges on the face.
    std::vector <Loop> loopsOnF;  // a vector of loops on the face.
};

// Model class contains all the model information including underlying 
// Simmetrix model (pGModel), model faces, loops, edges, and vertices.
class Model{
  public:
    /*
     * Function to get the pGModel to set it to model in the class.
     * pGModel simModel (in): The input Simmetrix model used in the definition of Model.
     */  
    void setSimModel(pGModel simModel);

    /*
     * Function to return the underlying Simmetrix model (pGModel).
     */ 
    const pGModel& getSimModel();

    /*
     * Function to return a vector of  model vertices on the model.
     */ 
    const std::vector <Vertex>& getModelVertices();

    /*
     * Function to return a vector of  model edges on the model.
     */ 
    const std::vector <Edge>& getModelEdges();

    /*
     * Function to return a vector of  model faces on the model.
     */ 
    const std::vector <Face>& getModelFaces();

  private:
    pGModel model;  // underlying Simmetrix model

    std::vector <Vertex> vertices; // vector of model vertices
    std::vector <Edge> edges;  // vector of model edges
    std::vector <Face> faces;  // vector of model faces
   
    /*
     * Function to set model vertices from pGModel on Model.
     */  
    void setModelVertices();

    /*
     * Function to set model edges from pGModel on Model.
     */  
    void setModelEdges();

    /*
     * Function to set model faces from pGModel on Model.
     */  
    void setModelFaces();
};

#endif
