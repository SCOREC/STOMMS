#ifndef XGC_READER_INTERFACE_H
#define XGC_READER_INTERFACE_H

#include "xgc_reader_plane.hpp"

/*
* XGCMesh
*/
class XgcMesh{
  public:
    XgcMesh(){};
    XgcMesh(std::string adiosFileName);
        
    /* 
     * Given the plane number, return PlaneMesh.
     * Made it public for testing underlying APIs. Once development is done, make it private.
     * int& planeNum (in): Plane Number on which mesh is required.
     */
    const PlaneMesh& getMeshAtPlane(const int& planeNum);
 
    /****************************************************************
     ***** Query Functions in XGC class. To be used in XGC code *****
     ***************************************************************/ 

    /*
     * Function to return mesh name.
     */ 
    std::string getMeshName();

    /*
     * Function to return number of planes in a mesh.
     */ 
    int getNumPlanes();

    /*
     * Given the plane number, get Omegah mesh at that plane.
     * int& planeNum (in): Plane Number on which Omega_h mesh is required.
     */
    Omega_h::Mesh getOmegahMeshatPlane(const int& planeNum);

    /*
     * Given the type of physics region, return the number of flux curves
     * on it. The second function returns the indices of flux curves.
     * (Core = 0, SOL = 1, Private_1 = 2, private_2 = 3, ...). As imn
     * current implementation of XGC. Change in future as needed.
     * int physicsRegion (in): Physics region on which model curves are required.
     * PlaneId planeId (in): Id of the plane.
     */
    int getNumCurvesAtRegion(int physicsRegion, PlaneId planeId);
    std::vector <CurveIdType> getCurvesAtRegion (int physicsRegion, PlaneId planeId);

    /* 
     * Given the required point type and plane #, return x-points or o-points on the plane.
     * PointType pType (in): desired critical point type (XPoint, OPoint).
     * PlaneId planeId (in): Id of the plane.
     */
    std::vector <VertexIdType> getCriticalPoints(PointType pType, PlaneId planeId);
    
    /*
     * Given the id of a model curve and Plane #, return curve type.
     * CurveIdType curveId (in): Model curve id for which type is desired.
     * PlaneId planeId (in): Id of the plane.
     */
    CurveType getCurveType(CurveIdType curveId, PlaneId planeId);
   
    /*
     * Given the curveType (closed,open etc.) and plane #, return all flux curves of that type.
     * CurveType cType (in): CurveType for which model curves are needed.
     * PlaneId planeId (in): Id of the plane.
     */
    std::vector <CurveIdType> getCurvesForType(CurveType cType, PlaneId planeId);

    /*
     * Given the model edge id and plane #, return the curve id on which 
     * model edge is classified on.
     * EdgeIdType edgeId (in): Model edge id.
     * PlaneId planeId (in): Id of the plane.
     */
    CurveIdType getCurveIdFromGeometricEdge(EdgeIdType edgeId, PlaneId planeId);

    /*
     * Given the id of a surface and plane #, return physics type of the surface.
     * SurfaceIdType surfaceId (in): Model face id.
     * PlaneId planeId (in): Id of the plane.
     */ 
    SurfaceType getPhysicsRegionType(SurfaceIdType surfaceId, PlaneId planeId);

    /*
     * Given the id of a model curve, and plane #, this function returns all the geometric
     * edges on that model curve.
     * CurveIdType curveId (in): Id of model curve for which model edges will be returned.
     * PlaneId planeId (in): Id of the plane.
     */ 
    std::vector<EdgeIdType> getCurveGeometricEdges(CurveIdType curveId, PlaneId planeId);

    /* 
     * Function to return model topology of the model entity on which mesh entity is 
     * classified. This will return the dimension of the model entity on which mesh 
     * entity is classified.
     * TopoType topoType (in): Topology of the mesh entity for which classification
     *                         information is needed (e.g TopoType::Vertex for mesh
     *                         vertices if classification for mesh vertex is needed). 
     * MeshIdType id (in): id of the mesh entity.
     * PlaneId planeId (in): Id of the plane.
     */
    TopoType getGeometricTopology(TopoType topoType, MeshIdType id, PlaneId planeId);

    /* 
     * Function to return id of the model entity on which mesh entity is classified. 
     * This function must be used with the getGeometricTopology to get complete 
     * classification information.
     * TopoType topoType (in): Topology of the mesh entity for which classification
     *                         information is needed (e.g TopoType::Vertex for mesh
     *                         vertices if classification for mesh vertex is needed). 
     * MeshIdType id (in): id of the mesh entity.
     * PlaneId planeId (in): Id of the plane.
     */
    
    GeomIdType getGeometricClassification(TopoType topoType, MeshIdType id, PlaneId planeId);

    /*
     * Reverse Classification.  
     * Given the geometric model id and topology, return the reverse classification for the desired mesh topology.
     * TopoType geomTopoType (in): Input geometric topology on which mesh entities are required.
     * GeomIdType geomId (in): Id of the geometric entity.
     * TopoType meshTopoType (in): Topology of desired mesh entities.
     * bool includeClosure (in): set true to get mesh entities on the closure of the model entity.
     * PlaneId planeId (in): Id of the plane.
     * Example: If all mesh vertices classified on a model face N (without closure) are desired, the usage of 
     * this API is: getMeshEntities(TopoType::Face, N, TopoType::Vertex, false, planeId);   
     */ 
    std::vector<MeshIdType> getMeshEntities(TopoType geomTopoType, GeomIdType geomId, TopoType meshTopoType, bool includeClosure, PlaneId planeId);

    /* 
     * Given the model entity topology, and its geometric id, returns all adjacents entities of dimension geomTopoOut.
     * TopoType geomTopoType (in): Input geometric topology on which adjacencies are needed.
     * GeomIdType geomId (in): Id of the geometric entity. 
     * PlaneId planeId (in): Id of the plane.
     */
    std::vector <GeomIdType> getModelAdjEnts(TopoType geomTopoIn, GeomIdType geomEntId, TopoType geomTopoOut, PlaneId planeId);

    /* 
     * Function to take an index of a private region and returns a vector of the adjacent x-points.
     * throws an exception if the passed index is not a private region.
     * SurfaceIdType privateFaceIndex (in): Index of the private region.
     * PlaneId planeId (in): Id of the plane.
     */
    std::vector<VertexIdType> getAdjacentXpoints(SurfaceIdType privateFaceIndex, PlaneId planeId);

    /* 
     * Given the mesh entity (vertex or face) return the physics regions its classified on.
     * TopoType meshTopoType (in): Topology of mesh entity (TopoType::Vertex, Face allowed).
     * MeshIdType id (in): id of the mesh entity.
     * PlaneId planeId (in): Id of the plane.
     */
    SurfaceType getPhysicsRegionForMeshEnt(TopoType meshTopoType, MeshIdType id, PlaneId planeId);
  
    /* 
     * Given a surface (model face), return all non-aligned mesh vertices on it. The return map contains vertex id as key 
     * and vector of flux curves bounding it.
     * SurfaceIdType surfaceId (in): Index of the model face on which non-aligned mesh vertices are required.
     * PlaneId planeId (in): Id of the plane.
     */
    std::map<MeshIdType, std::vector <CurveIdType>> getNonAlignedMeshVerticesOnModelSurface(SurfaceIdType surfaceId, PlaneId planeId);

    /* Given just PlaneId, return all non-aligned mesh vertices on it. The return map contains vertex id as key and 
     * vector of flux curves bounding it.
     * PlaneId planeId (in): Id of the plane.
     */
    std::map<MeshIdType, std::vector <CurveIdType>> getNonAlignedMeshVerticesOnPlane(PlaneId planeId);
  private: 
    // Functions:

    /*
     * Function to read the mesh name from adios2 file and set it to the interface.
     * adios2::IO& io (in): adios2 io to interact with adios2 input file.
     */ 
    std::string setMeshName(adios2::IO &io);

    /*
     * Function to read the number of planes from adios2 file and set it to the interface.
     * adios2::IO& io (in): adios2 io to interact with adios2 input file.
     */ 
    int setNumPlanes(adios2::IO &io);

    /*
     * Function to set a vector of plane meshes.
     * adios2::IO& io (in): adios2 io to interact with adios2 input file.
     * adios2::Engine& reader (in): adios2 reader engine to read data from adios file.
     */ 
    std::vector <PlaneMesh> setPlaneMeshes(adios2::IO &io, adios2::Engine &reader);

    /*
     * Function to set a vector of Omega_h meshes (one for each plane).
     */ 
    std::vector <Omega_h::Mesh> setOmegahMeshes();
    

    // Internal query function

    /*
     * Function to check the validity of the plane when queries are made.
     * int& planeNum (in): number of the plane to check validity for.
     */ 
    bool isPlaneValid(const int& planeNum);

    /*
     * Function to return vector of plane meshes for internal use.
     */ 
    std::vector <PlaneMesh> getPlaneMeshes();

    // Variables
    std::string adiosFile;
    std::string meshName;
    std::vector <Omega_h::Mesh> omegahMeshes;
    std::vector <PlaneMesh> planeMeshes;
    int numPlanes = 1;
};

/* 
 * ** Not implemented yet.
 * Given arbitrary location in RZ, give back which physics region
 * double R (in): R-coordinate of the point to be tested.
 * double Z (in): Z-coordinate of the point to be tested.
 */
SurfaceIdType get_physics_region(double R, double Z);

#endif

