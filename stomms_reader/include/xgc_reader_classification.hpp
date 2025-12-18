#ifndef XGC_READER_CLASSIFICATION_H
#define XGC_READER_CLASSIFICATION_H

#include "xgc_reader_types.hpp"
#include "Omega_h_build.hpp"
#include "Omega_h_mesh.hpp"
#include "Omega_h_file.hpp"

class MeshClassification{
  public:
    MeshClassification(){};
    MeshClassification(const Omega_h::Mesh& mesh);

    /*
     * Given the mesh vertex id, return the dimension/topology of the model entity
     * on which mesh vertex is classified.
     * MeshIdType& vId (in): Mesh vertex id.
     */  
    TopoType getMeshVertexClassDim(const MeshIdType& vId) const;

    /*
     * Given the mesh edge id, return the dimension/topology of the model entity
     * on which mesh edge is classified.
     * MeshIdType& eId (in): Mesh edge id.
     */  
    TopoType getMeshEdgeClassDim(const MeshIdType& eId) const;

    /*
     * Given the mesh face id, return the dimension/topology of the model entity
     * on which mesh face is classified.
     * MeshIdType& fId (in): Mesh face id.
     */  
    TopoType getMeshFaceClassDim(const MeshIdType& fId) const;

    /*
     * Given the mesh vertex id, return the geometric id of the model entity
     * on which mesh vertex is classified.
     * MeshIdType& vId (in): Mesh vertex id.
     */  
    GeomIdType getMeshVertexClassId(const MeshIdType& vId) const;

    /*
     * Given the mesh edge id, return the geometric id of the model entity
     * on which mesh edge is classified.
     * MeshIdType& eId (in): Mesh edge id.
     */  
    GeomIdType getMeshEdgeClassId(const MeshIdType& eId) const;

    /*
     * Given the mesh face id, return the geometric id  of the model entity
     * on which mesh face is classified.
     * MeshIdType& fId (in): Mesh face id.
     */  
    GeomIdType getMeshFaceClassId(const MeshIdType& fId) const;
  private:
    // Functions:

    /*
     * Function to set the classification of the mesh vertices.
     */ 
    void setMeshVerticesClassification();
    
    /*
     * Function to set the classification of the mesh edges.
     */ 
    void setMeshEdgesClassification();

    /*
     * Function to set the classification of the mesh faces.
     */ 
    void setMeshFacesClassification();

    // Variables:
    Omega_h::Mesh omegahMesh;
    std::map <MeshIdType, TopoType> verticesClassDim;
    std::map <MeshIdType, GeomIdType> verticesClassId;
    std::map <MeshIdType, TopoType> edgesClassDim;
    std::map <MeshIdType, GeomIdType> edgesClassId;
    std::map <MeshIdType, TopoType> facesClassDim;
    std::map <MeshIdType, GeomIdType> facesClassId;
};

#endif
