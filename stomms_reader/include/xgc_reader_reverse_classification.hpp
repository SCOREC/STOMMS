#ifndef XGC_READER_REV_CLASSIFICATION_H
#define XGC_READER_REV_CLASSIFICATION_H

#include "xgc_reader_topology.hpp"
#include "xgc_reader_classification.hpp"

class ReverseClassification{
  public:
    ReverseClassification(){};

    /*
     * ReverseClassification Constructor.
     * Omega_h::Mesh& mesh (in): Omega_h mesh.
     * Model& model (in): Model information set from adios2 file.
     * MeshClassification& meshClass (in): Mesh classification information from Omega_h mesh.
     * ModelTopology& modelTopo (in): Model topology set from adios2 file.
     */ 
    ReverseClassification(const Omega_h::Mesh& mesh, const Model& model, const MeshClassification& meshClass, 
                          const ModelTopology& modelTopo);

    /*
     * Given geometric id of the model vertex, return the mesh vertex classified on it.
     * GeomIdType& gId (in): Geometric id of the model vertex.
     */ 
    std::vector <MeshIdType> getVertexToVertexReverseClassification(const GeomIdType& gId) const;
 
    /*
     * Given geometric id of the model edge, return the vector of mesh edges classified on it.
     * GeomIdType& gId (in): Geometric id of the model edge.
     */ 
    std::vector <MeshIdType> getEdgeToEdgeReverseClassification(const GeomIdType& gId) const;

    /*
     * Given geometric id of the model face, return the vector of mesh faces classified on it.
     * GeomIdType& gId (in): Geometric id of the model face.
     */ 
    std::vector <MeshIdType> getFaceToFaceReverseClassification(const GeomIdType& gId) const;

    /*
     * Given geometric id of the model edge, return the vector of mesh vertices classified on it.
     * GeomIdType& gId (in): Geometric id of the model edge.
     * bool includeClosure (in): set true to get mesh entities on the closure of the model entity.
     */ 
    std::vector <MeshIdType> getEdgeToVertexReverseClassification(const GeomIdType& gId, bool includeClosure) const;

    /*
     * Given geometric id of the model face, return the vector of mesh vertices classified on it.
     * GeomIdType& gId (in): Geometric id of the model face.
     * bool includeClosure (in): set true to get mesh entities on the closure of the model entity.
     */ 
    std::vector <MeshIdType> getFaceToVertexReverseClassification(const GeomIdType& gId, bool includeClosure) const;

    /*
     * Given geometric id of the model face, return the vector of mesh edges classified on it.
     * GeomIdType& gId (in): Geometric id of the model face.
     * bool includeClosure (in): set true to get mesh entities on the closure of the model entity.
     */ 
    std::vector <MeshIdType> getFaceToEdgeReverseClassification(const GeomIdType& gId, bool includeClosure) const;
  private:
    // Functions:

    /*
     * Function to set reverse classification from classification information of mesh vertices.
     */ 
    void setVerticesReverseClassification();

    /*
     * Function to set reverse classification from classification information of mesh edges.
     */ 
    void setEdgesReverseClassification();

    /*
     * Function to set reverse classification from classification information of mesh faces.
     */ 
    void setfacesReverseClassification();

    /*
     * Since we are using classification information to construct reverse classification,
     * we only got model entities that have a mesh entity classified on them and missed
     * remaining model entities (for example if a model face doesn't have a mesh vertex
     * on it, it was skipped).Now if we make a query mesh vertices on such model face it 
     * will cause an error.  Make sure to set them (empty vector) also for query purposes 
     * and consistency.
     */ 
    void setRemainingModelEntities();

    // Variables:
    Omega_h::Mesh omegahMesh;
    Model geomModel;
    MeshClassification meshClassification;
    ModelTopology modelTopology;

    // Reverse Classification Information:
    // Name Style: modelEntToMeshEntReverseClassification
    std::map <GeomIdType, std::vector<MeshIdType>> vertexToVertexReverseClassification;
    std::map <GeomIdType, std::vector<MeshIdType>> edgeToVertexReverseClassification;
    std::map <GeomIdType, std::vector<MeshIdType>> faceToVertexReverseClassification;
    std::map <GeomIdType, std::vector<MeshIdType>> edgeToEdgeReverseClassification;
    std::map <GeomIdType, std::vector<MeshIdType>> faceToEdgeReverseClassification;
    std::map <GeomIdType, std::vector<MeshIdType>> faceToFaceReverseClassification;
};

#endif
