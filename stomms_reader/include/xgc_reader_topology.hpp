#ifndef XGC_READER_TOPOLOGY_H
#define XGC_READER_TOPOLOGY_H

#include "xgc_reader_model.hpp"
#include "xgc_reader_utility.hpp"
#include "Omega_h_adios2.hpp"

class ModelTopology{
  public:
    ModelTopology(){};

    /*
     * ModelTopology Constructor.
     * adios2::IO& io (in): adios2 io to interact with adios2 input file.
     * adios2::Engine& reader (in): adios2 reader engine to read data from adios file.
     * std::string& name (in): Mesh name from adios2 file.
     * int planeNumber (in): Plane number to set model topology. Since data is organized on 
     *                       planes in adios2 file, we need plane number to set topology. 
     */ 
    ModelTopology(const adios2::IO& io, const adios2::Engine& reader, std::string name, int planeNumber);

    /*
     * Given geometric id of the model vertex, return the model edges adjacent it.
     */ 
    std::vector <EdgeIdType> getAdjacentEdgesOnVertex(const VertexIdType& vId) const;

    /*
     * Given geometric id of the model vertex, return the model faces adjacent it.
     */  
    std::vector <SurfaceIdType> getAdjacentFacesOnVertex(const VertexIdType& vId) const;
 
    /*
     * Given geometric id of the model edge, return the model vertices adjacent it.
     */ 
    std::vector <VertexIdType> getAdjacentVerticesOnEdge(const EdgeIdType& eId) const;

    /*
     * Given geometric id of the model edge, return the model faces adjacent it.
     */ 
    std::vector <SurfaceIdType> getAdjacentFacesOnEdge(const EdgeIdType& eId) const;

    /*
     * Given geometric id of the model face, return the model vertices adjacent it.
     */ 
    std::vector <VertexIdType> getAdjacentVerticesOnFace(const SurfaceIdType& fId) const;

    /*
     * Given geometric id of the model face, return the model edges adjacent it.
     */ 
    std::vector <EdgeIdType> getAdjacentEdgesOnFace(const SurfaceIdType& fId) const;
  private:
    // Functions
    
    /*
     * Sets the model adjacencies on model vertices.
     */ 
    void setVertexAdjacencies(std::string groupName);
 
    /*
     * Sets the model adjacencies on model edges.
     */ 
    void setEdgeAdjacencies(std::string groupName);

    /*
     * Sets the model adjacencies on model faces.
     */ 
    void setFaceAdjacencies(std::string groupName);

    // Variables
    adios2::IO ioPlane;
    adios2::Engine readerPlane;
    std::string meshName;
    int planeNum;
    std::map<VertexIdType, std::vector <EdgeIdType>> vertexToEdge;
    std::map<VertexIdType, std::vector <SurfaceIdType>> vertexToFace;
    std::map<EdgeIdType, std::vector <VertexIdType>> edgeToVertex;
    std::map<EdgeIdType, std::vector <SurfaceIdType>> edgeToFace;
    std::map<SurfaceIdType, std::vector <VertexIdType>> faceToVertex;
    std::map<SurfaceIdType, std::vector <EdgeIdType>> faceToEdge;
};

#endif
