#ifndef XGC_READER_MODEL_H
#define XGC_READER_MODEL_H

#include <vector>
#include "xgc_reader_types.hpp"
#include "xgc_reader_utility.hpp"
#include "Omega_h_adios2.hpp"

class ModelVertex {
  public:
    /*
     * ModelVertex constructor.
     * Takes in model vertex id, and physics type to set model vertex in the model.
     * int modelVertexId (in): Geometric id of the model vertex.
     * PointType& vertexType (in): Physics type (OPoint, XPoint, None).
     */ 
    ModelVertex(int modelVertexId, const PointType& vertexType);
    ModelVertex(){};

    /*
     * Function to return physics type of the model vertex.
     */ 
    const PointType& getPointType();

    /*
     * Function to return the topology type of model entity. 
     * 0 or TopeType::Vertex in this case.
     */ 
    const TopoType& getTopoType();

    /*
     * Function to return geometric id of the model vertex.
     */ 
    const GeomIdType& getGeomId();
  private:
    PointType pointType = PointType::None;
    TopoType topoType = TopoType::Vertex;
    GeomIdType geomId;
};

class ModelEdge{
  public:
    /*
     * ModelEdge Constructor. 
     * Takes  in modle edge id, and physics curve number to set model edge in the model.
     * int modelEdgeId (in): Geometric id of the model edge. 
     * int curveNumber (in): Curve Number on which model edge is classified on.
     */ 
    ModelEdge(int modelEdgeId, int curveNumber);
    ModelEdge(){};

    /*
     * Function to return the curve id of the model curve on which model edge is classified on.
     */ 
    const int& getCurveId();

    /*
     * Function to return geometric id of the model edge.
     */ 
    const GeomIdType& getGeomId();

    /*
     * Function to return the topology type of model entity.
     * 1 or TopeType::Edge in this case.
     */ 
    const TopoType& getTopoType();
  private:
    GeomIdType geomId;
    int curveId;
    TopoType topoType = TopoType::Edge;
};

class ModelCurve{
  public:
    /*
     * ModelCurve constructor.
     * int curveNumber (in): Index of the model curve. Unique for each model curve.
     * CurveType& type (in): Curve physics type (closed, open, separatrix, wall)
     * double psi (in) : Magnetic flux field values on curves. Set -1 on wall curve.
     * std::vector <int>& edges (in): set of model edges classified on the model curve.
     */ 
    ModelCurve(int curveNumber, const CurveType& type, double psi, const std::vector <int>& edges);
    ModelCurve(){};

    /*
     * Function to return index of the curve in the model.
     */ 
    const int& getCurveId();

    /*
     * Function to return the physics type of the curve.
     */ 
    const CurveType& getCurveType();

    /*
     * Function to return the topology type of model entity.
     * 1 or TopeType::Edge in this case since set of model edges define a curve. 
     */ 
    const TopoType& getTopoType();

    /*
     * Function to return a vector of model edges id classified on the model curve.
     */ 
    const std::vector <GeomIdType>& getModelEdgesGeomIds();

    /*
     * Function to return psi value on the flux curve.
     */ 
    const double& getCurvePsi();
  private:
    int curveId;  // curve number
    CurveType curveType = CurveType::None;
    TopoType topoType = TopoType::Edge;
    double curvePsi = -1.0;
    std::vector <GeomIdType> modelEdgesGeomIds;
};

class ModelFace{
  public:
    /*
     * ModelFace constructor.
     * Takes in geometric id of model face and physics type to set face in the model.
     * int modelFaceId (in): Geometric id of the model face.
     * SurfaceType& type (in): Physics surface type of the model face.
     */  
    ModelFace(int modelFaceId, const SurfaceType& type);
    ModelFace(){};

    
    /*
     * Function to return physics surface type of the model face.
     */ 
    const SurfaceType& getSurfaceType();

    /*
     * Function to return the topology type of model entity.
     * 2 or TopeType::Face in this case.
     */ 
    const TopoType& getTopoType();

    /*
     * Function to return geometric id of the model face.
     */ 
    const GeomIdType& getGeomId();
  private:
    SurfaceType surfaceType = SurfaceType::None;
    TopoType topoType = TopoType::Face;
    GeomIdType geomId;
};

/* Model class holds all the model properties of a plane model. This class reads the data directly from
 * adios2 file and define model in terms of model vertices, edges, and faces along with their physics
 * properties.
 */
class Model{
  public:
    Model(){};
    
    /*
     * Model contructor.
     * adios2::IO& io (in): adios2 io to interact with adios2 input file.
     * adios2::Engine& reader (in): adios2 reader engine to read data from adios file.
     * std::string& name (in): Mesh name from adios2 file.
     * int planeNumber (in): Plane number to set model on. Since data is organized on planes
     *                       in adios2 file, we need plane number to set models.
     */  
    Model(const adios2::IO& io, const adios2::Engine& reader, const std::string& name, int planeNumber);

    /*
     * Function to return physics model vertices on the model. 
     * Only contains O-points and X-points for now.
     */ 
    const std::vector <ModelVertex>& getModelVertices() const;

    /*
     * Function to return all the model curves on the model. The key in the map
     * is curve id, and corresponding value contains model curve.
     */ 
    const std::map <CurveIdType, ModelCurve>& getModelCurves() const;

    /*
     * Function to return all the model faces on the model. The key in the map
     * is surface id, and corresponding value contains model face.
     */ 
    const std::map <SurfaceIdType, ModelFace>& getModelFaces() const;

    /*
     * Function to return all the model edges on the model. The key in the map
     * is edge id, and corresponding value contains model edge.
     */ 
    const std::map <EdgeIdType, ModelEdge>& getModelEdges() const;

    /*
     * Function to return a map bewtween private region index and a vector of corresponding 
     * model faces on that particular private region.
     */ 
    const std::map <int, std::vector<ModelFace>>& getPrivateRegions() const;

    /*
     * Function to get the model vertex ids of the all the Xpoints in the model.
     */  
    std::vector <VertexIdType> getXpoints() const;

    /*
     * Function to get the model vertex ids of the all the Opoints in the model.
     */ 
    std::vector <VertexIdType> getOpoints() const;

    /*
     * Given the model curve id, this function returns the model curve on it.
     * CurveIdType& curveId (in): Curve id of the input model curve.
     */ 
    ModelCurve getModelCurveFromId(const CurveIdType& curveId) const;

    /*
     * Given the model edge id, this function returns the id of model curve on it.
     * Model curve can have any number of model edges, so given any model edge id
     * on that model curve, this function will return the model curve id on it.
     * EdgeIdType& edgeId (in): Edge id for which we want parent model curve.
     */ 
    CurveIdType getModelCurveFromEdgeId(const EdgeIdType& edgeId) const;

    /*
     * Given the type of model curve, this function returns all the model curve of 
     * that particular type.
     * CurveType& curveType (in): Curve type (Closed, Open, Separatrix, Wall, None)
     */ 
    std::vector <CurveIdType> getModelCurvesFromType(const CurveType& curveType) const;
    
    /*
     * Given the id of the model vertex, return the model vertex.
     * VertexIdType& vertexId (in): Id of model vertex for which model vertex is needed.
     */ 
    ModelVertex getModelVertexFromId(const VertexIdType& vertexId) const;

    /*
     * Given the id of the model edge, return the model edge.
     * EdgeIdType& edgeId (in) : Id of model edge for which mode edge is needed.
     */ 
    ModelEdge getModelEdgeFromId(const EdgeIdType& edgeId) const;

    /*
     * Given the id of the model face, return the index of private region on which 
     * model face is classified on.
     * SurfaceIdType& fId (in): Input model face id.
     */ 
    int getPrivateRegionIndex(const SurfaceIdType& fId) const;
  private:
    // Functions

    /*
     * Function to set model vertices from adios2 input file.
     * std::string& groupName (in): Group name in adios2 file that holds 
     *                              model vertices data.
     */ 
    void setModelVertices(const std::string& groupName);

    /*
     * Function to set model curves from adios2 input file.
     * std::string& groupName (in): Group name in adios2 file that holds 
     *                              model curves data.
     */ 
    void setModelCurves(const std::string& groupName);

    /*
     * Function to set model edges from the model curves already read from adios2 file.
     */ 
    void setModelEdges(); 

    /*
     * Function to set model faces from adios2 input file.
     * std::string& groupName (in): Group name in adios2 file that holds
     *                              model faces data.
     */ 
    void setModelFaces(const std::string& groupName);

    /*
     * Given the name of the variables in the adios2 file, read the model faces data 
     * those variables hold.
     * std::vector <std::string> variables (in): set of model face variables in adiso2 file.
     */ 
    void setModelFacesFromVariables(std::vector <std::string> variables);

    /*
     * Given the private region index, and indices of model faces on it, set a map of private 
     * region and corresponding vector of model faces on it.
     * int privateRegionIndex (in): Private region index.
     * std::vector <int>& faceIds (in): id of model faces on respective private region.
     */ 
    void setPrivateModelFaces(int privateRegionIndex, const std::vector <int>& faceIds);

    // Variables
    adios2::IO ioPlane;  // io for specific plane
    adios2::Engine readerPlane;  // reader engine for specific plane
    std::string meshName;  // mesh name from adios2 file
    int planeNum;  // input plane number
    std::vector <ModelVertex> modelVertices;  
    std::map<CurveIdType, ModelCurve> modelCurves;  
    std::map<EdgeIdType, ModelEdge> modelEdges;
    std::map<SurfaceIdType, ModelFace> modelFaces;
    std::map<int, std::vector<ModelFace>> privateRegions;
};

#endif
