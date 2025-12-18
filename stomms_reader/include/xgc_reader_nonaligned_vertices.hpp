#ifndef XGC_READER_NONALIGNED_VERTICES_H
#define XGC_READER_NONALIGNED_VERTICES_H

#include <cassert>
#include "xgc_reader_topology.hpp"
#include "xgc_reader_reverse_classification.hpp"

class NonAlignedVertex{
  public:
    NonAlignedVertex(){};

    /*
     * NonAligned Vertex Constructor.
     * int vertexNumber (in): Mesh vertex id.
     * CurveIdType flux1 (in): Curve id of the flux curve with psi1 closest to the mesh vertex.
     * CurveIdType flux2 (in): Curve id of the flux curve with psi2 closest to the mesh vertex.
     */ 
    NonAlignedVertex(int vertexNumber, CurveIdType flux1, CurveIdType flux2);

    /*
     * Function to return the id of the vertex stored on specific NonAligned vertex.
     */ 
    const int& getVertex() const;

    /*
     * Function to return both bounding flux curves on the face that has NonAligned mesh vertex on it.
     */ 
    const std::vector <CurveIdType>& getBoundingFluxCurves() const;
  private:
    int vertex;
    std::vector <CurveIdType> fluxCurves;
};


class NonAlignedVertices{
  public:
    NonAlignedVertices(){};

    /*
     * NonAlignedVertices Constructor.
     * Omega_h::Mesh& mesh (in): Omega_h mesh.
     * Model& inputModel (in): Model information set from adios2 file.
     * ModelTopology& modelTopo (in): Model topology set from adios2 file.
     * ReverseClassification& reverseClassification (in): Reverse classification class.
     */ 
    NonAlignedVertices(const Omega_h::Mesh& omegahMesh, const Model& inputModel, 
                       const ModelTopology& modelTopology, 
                       const ReverseClassification& reverseClassification);

    /*
     * Given the id of the model surface, return all non-aligned mesh vertices on it.
     * SurfaceIdType faceId (in): input model face id.
     */ 
    std::vector <NonAlignedVertex> getNonAlignedVerticesOnFace(SurfaceIdType faceId) const;

    /*
     * Function to return all the non-aligned mesh vertices stored in a map in the class NonAlignedVertices.
     * This basically returns all non-aligned mesh vertices on a model plane.
     */ 
    const std::map <SurfaceIdType, std::vector <NonAlignedVertex>>& getNonAlignedVerticesOnModel() const; 
  private:
    std::map <SurfaceIdType, std::vector <NonAlignedVertex>> nonAlignedVerticesOnFace;
    Model model;
    Omega_h::Mesh mesh;
    Omega_h::Reals coords;
    ReverseClassification revClass;
    ModelTopology modelTopo;

    // Functions

    /*
     * Function to evaluate if a model face should be considered for non-aligned mesh vertices search or not.
     * ModelFace& modelFace (in): Model face to be evaluated.
     */ 
    bool considerModelFaceForNonAlignedVertices(const ModelFace& modelFace);

    /*
     * Function to return a vector of model edges on a model face to be considered for non-aligned vertices search.
     * SurfaceIdType& faceId (in): Id of the model face on which we are evaluating model edges for search.
     */ 
    std::vector <EdgeIdType> considerModelEdgesForNonAlignedVertices(const SurfaceIdType& faceId);

    /*
     * Function to return a vector of model vertices on a model face to be considered for non-aligned vertices search.
     * SurfaceIdType& faceId (in): Id of the model face on which we are evaluating model vertices for search.
     */ 
    std::vector <VertexIdType> considerModelVerticesForNonAlignedVertices(const SurfaceIdType& faceId);

    /*
     * Given the id of model face, set the non-aligned mesh vertices on it.
     * SurfaceIdType& faceId (in): Id of the model face on which non-aligned vertices are to be set.
     */ 
    std::vector <NonAlignedVertex> setNonAlignedVerticesAtFace(const SurfaceIdType& faceId);

    /*
     * Given the vertex coordinates and model face it is classified on, find the two flux curves with different psi values
     * which are closest to this vertex.
     * SurfaceIdType& faceId (in): Model face id on which mesh vertex is classified.
     * std::array<double, 2>& vertexCoords (in): R-Z coordinates of mesh vertex. 
     */ 
    std::array <CurveIdType, 2> getNearestBoundingFluxCurves(const SurfaceIdType& faceId, const std::array<double, 2>& vertexCoords);

    /*
     * Given the coordinates of a point, and a vector of model edges, find the model edges nearest to the point.
     * std::array<double, 2>& vertexCoords (in): R-Z coordinates of the point.
     * std::vector<EdgeIdType> edges (in): Vector of edges for the evaluation.
     */ 
    EdgeIdType getNearestModelEdgeToPoint(const std::vector<EdgeIdType> edges, const std::array<double, 2>& vertexCoords);

    /*
     * Given the coordinates of a point, and an model edge, find the nearest possible distance from the edge to
     * the point.
     * EdgeIdType edge (in): input model edge.
     * std::array<double, 2>& vertexCoords (in): R-Z coordinates of the point.
     *
     */ 
    double getMinimumDistanceFromPointToEdge(EdgeIdType edge, const std::array<double, 2>& vertexCoords);
};

#endif
