#ifndef XGC_READER_INTERFACE_H
#define XGC_READER_INTERFACE_H

#include "xgc_reader_plane.hpp"
#include "xgc_reader_background_grid.hpp"

/*
* @brief class XGCMesh.
*/
class XgcMesh{
  public:
    XgcMesh(){};

    /**
     * XGC Mesh Class. Takes in adios2 file.
     * @param adiosFileName: Adios2 file name containing mesh and related information.
     */ 
    XgcMesh(std::string adiosFileName);
        
    /** 
     * Given the plane number, return PlaneMesh.
     * Made it public for testing underlying APIs. Once development is done, make it private.
     * @param planeNum: Plane Number on which mesh is required.
     */
    const PlaneMesh& getMeshAtPlane(const int& planeNum);
 
    /****************************************************************
     ***** Query Functions in XGC class. To be used in XGC code *****
     ***************************************************************/ 

    /**
     * Function to return mesh name.
     */ 
    std::string getMeshName();

    /**
     * Function to return number of planes in a mesh.
     */ 
    int getNumPlanes();

    /**
     * Given the plane number, get Omegah mesh at that plane.
     * @param planeNum: Plane Number on which Omega_h mesh is required.
     */
    Omega_h::Mesh getOmegahMeshatPlane(const int& planeNum);

    /**
     * Given the type of physics region, return the number of flux curves
     * on it. Core = 0, SOL = 1, Private_1 = 2, private_2 = 3, ...). As in
     * current implementation of XGC. Change in future as needed.
     * @param physicsRegion: Physics region on which model curves are required.
     * @param planeId: Id of the plane.
     */
    int getNumCurvesAtRegion(int physicsRegion, PlaneId planeId);

    /**
     * Given the type of physics region, return the indices of flux 
     * curves.(Core = 0, SOL = 1, Private_1 = 2, private_2 = 3, ...). 
     * As in current implementation of XGC. Change in future as needed.
     * @param physicsRegion: Physics region on which model curves are required.
     * @param planeId: Id of the plane.
     */
    std::vector <CurveIdType> getCurvesAtRegion (int physicsRegion, PlaneId planeId);

    /** 
     * Given the required point type and plane #, return x-points or o-points on the plane.
     * @param pType: desired critical point type (XPoint, OPoint).
     * @param planeId: Id of the plane.
     */
    std::vector <VertexIdType> getCriticalPoints(PointType pType, PlaneId planeId);
    
    /**
     * Given the id of a model curve and Plane #, return curve type.
     * @param curveId: Model curve id for which type is desired.
     * @param planeId: Id of the plane.
     */
    CurveType getCurveType(CurveIdType curveId, PlaneId planeId);
   
    /**
     * Given the curveType (closed,open etc.) and plane #, return all flux curves of that type.
     * @param cType: CurveType for which model curves are needed.
     * @param planeId: Id of the plane.
     */
    std::vector <CurveIdType> getCurvesForType(CurveType cType, PlaneId planeId);

    /**
     * Given the model edge id and plane #, return the curve id on which 
     * model edge is classified on.
     * @param edgeId: Model edge id.
     * @param planeId: Id of the plane.
     */
    CurveIdType getCurveIdFromGeometricEdge(EdgeIdType edgeId, PlaneId planeId);

    /**
     * Given the id of a surface and plane #, return physics type of the surface.
     * @param surfaceId (in): Model face id.
     * @param planeId: Id of the plane.
     */ 
    SurfaceType getPhysicsRegionType(SurfaceIdType surfaceId, PlaneId planeId);

    /**
     * Given the id of a model curve, and plane #, this function returns all the geometric
     * edges on that model curve.
     * @param curveId: Id of model curve for which model edges will be returned.
     * @param planeId: Id of the plane.
     */ 
    std::vector<EdgeIdType> getCurveGeometricEdges(CurveIdType curveId, PlaneId planeId);

    /** 
     * Function to return model topology of the model entity on which mesh entity is 
     * classified. This will return the dimension of the model entity on which mesh 
     * entity is classified.
     * @param topoType: Topology of the mesh entity for which classification
     *                  information is needed (e.g TopoType::Vertex for mesh
     *                  vertices if classification for mesh vertex is needed). 
     * @param id: id of the mesh entity.
     * @param planeId: Id of the plane.
     */
    TopoType getGeometricTopology(TopoType topoType, MeshIdType id, PlaneId planeId);

    /** 
     * Function to return id of the model entity on which mesh entity is classified. 
     * This function must be used with the getGeometricTopology to get complete 
     * classification information.
     * @param topoType: Topology of the mesh entity for which classification
     *                  information is needed (e.g TopoType::Vertex for mesh
     *                  vertices if classification for mesh vertex is needed). 
     * @param id: id of the mesh entity.
     * @param planeId: Id of the plane.
     */
    
    GeomIdType getGeometricClassification(TopoType topoType, MeshIdType id, PlaneId planeId);

    /**
     * Reverse Classification.  
     * Given the geometric model id and topology, return the reverse classification for the desired mesh topology.
     * @param geomTopoType: Input geometric topology on which mesh entities are required.
     * @param geomId: Id of the geometric entity.
     * @param meshTopoType: Topology of desired mesh entities.
     * @param includeClosure: set true to get mesh entities on the closure of the model entity.
     * @param planeId: Id of the plane.
     * @par Example 
     * If all mesh vertices classified on a model face N (without closure) are desired, the usage of 
     * this API is: 
     * @code getMeshEntities(TopoType::Face, N, TopoType::Vertex, false, planeId); @endcode  
     */ 
    std::vector<MeshIdType> getMeshEntities(TopoType geomTopoType, GeomIdType geomId, TopoType meshTopoType, bool includeClosure, PlaneId planeId);

    /** 
     * Given the model entity topology, and its geometric id, returns all adjacents entities of dimension geomTopoOut.
     * @param geomTopoType: Input geometric topology on which adjacencies are needed.
     * @param geomId: Id of the geometric entity. 
     * @param planeId: Id of the plane.
     */
    std::vector <GeomIdType> getModelAdjEnts(TopoType geomTopoIn, GeomIdType geomEntId, TopoType geomTopoOut, PlaneId planeId);

    /** 
     * Function to take an index of a private region and returns a vector of the adjacent x-points.
     * throws an exception if the passed index is not a private region.
     * @param privateFaceIndex: Index of the private region.
     * @param planeId: Id of the plane.
     */
    std::vector<VertexIdType> getAdjacentXpoints(SurfaceIdType privateFaceIndex, PlaneId planeId);

    /** 
     * Given the mesh entity (vertex or face) return the physics regions its classified on.
     * meshTopoType: Topology of mesh entity (TopoType::Vertex, Face allowed).
     * id: id of the mesh entity.
     * @param planeId: Id of the plane.
     */
    SurfaceType getPhysicsRegionForMeshEnt(TopoType meshTopoType, MeshIdType id, PlaneId planeId);
  
    /** 
     * Given a surface (model face), return all non-aligned mesh vertices on it. The return map contains vertex id as key 
     * and vector of flux curves bounding it.
     * @param surfaceId: Index of the model face on which non-aligned mesh vertices are required.
     * @param planeId: Id of the plane.
     */
    std::map<MeshIdType, std::vector <CurveIdType>> getNonAlignedMeshVerticesOnModelSurface(SurfaceIdType surfaceId, PlaneId planeId);

    /** 
     * Given just PlaneId, return all non-aligned mesh vertices on it. The return map contains vertex id as key and 
     * vector of flux curves bounding it.
     * @param planeId: Id of the plane.
     */
    std::map<MeshIdType, std::vector <CurveIdType>> getNonAlignedMeshVerticesOnPlane(PlaneId planeId);
  private: 
    // Functions:

    /**
     * Function to read the mesh name from adios2 file and set it to the interface.
     * @param io: adios2 io to interact with adios2 input file.
     */ 
    std::string setMeshName(adios2::IO &io);

    /**
     * Function to read the number of planes from adios2 file and set it to the interface.
     * @param io: adios2 io to interact with adios2 input file.
     */ 
    int setNumPlanes(adios2::IO &io);

    /**
     * Function to set a vector of plane meshes.
     * @param io: adios2 io to interact with adios2 input file.
     * @param reader: adios2 reader engine to read data from adios file.
     */ 
    std::vector <PlaneMesh> setPlaneMeshes(adios2::IO &io, adios2::Engine &reader);

    /**
     * Function to set a vector of Omega_h meshes (one for each plane).
     */ 
    std::vector <Omega_h::Mesh> setOmegahMeshes();
    

    // Internal query function

    /**
     * Function to check the validity of the plane when queries are made.
     * @param planeNum: number of the plane to check validity for.
     */ 
    bool isPlaneValid(const int& planeNum);

    /**
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

/**
* @brief class XGCBackgroundGridData.
*/
class XgcBackgroundGridData{
  public:
    XgcBackgroundGridData(){};

    /**
     * Constructor for XGC Background Grid Data. 
     * @param adiosFileName: Adios2 file name containing mesh and related information.
     */
    XgcBackgroundGridData(std::string adiosFileName);
 
    /**
     * Function to return a vector of R coordinates of the grid.
     */ 
    const std::vector <double>& getGridPointsR() const;

    /**
     * Function to return a vector of Z coordinates of the grid.
     */ 
    const std::vector <double>& getGridPointsZ() const;
   
    /**
     * Function to return a vector of psi field on grid points.
     * Size of psi vector = size of R vector* size of Z vector.
     */ 
    const std::vector <double>& getPsiFieldOnGrid() const;

    /**
     * Function to return a vector of psi flux from Eqdsk.
     */
    const std::vector <double>& getPsiArray() const;

    /**
     * Function to return a vector of poloidal current from Eqdsk.
     */
    const std::vector <double>& getPoloidalCurrentArray() const;
 
    /**
     * Function to return a vector of R coordinates of the limiter.
     */
    const std::vector <double>& getLimiterR() const;

    /**
     * Function to return a vector of Z coordinates of the limiter.
     */
    const std::vector <double>& getLimiterZ() const;
     
    /**
     * Function to return eqdsk domain box.
     * An array of size 4.
     * Contains rMin, zMin, rMax, zMax.
     */
    std::array <double,4> getDomainBox() const; 

    /**
     * Function to return global PSPLINE psi spline coefficients.
     * # of coefficients = 4*rGridPoints.size()*zGridPoints.size()
     */
    const std::vector <double>& getPsplinePsiCoefficients() const;
 
    /**
     * Function to return PSPLINE poloidal current spline coefficients.
     * # of coefficients = 2*psi.size()
     */
    const std::vector <double> getPsplineCurrentCoefficients() const;

    /**
     * Function to return a vector of all the bicubic spline coefficients in each grid cell.
     * It contains 16 coefficients for each cell. 
     */ 
    const std::vector <double>& getBicubicSplineCoefficients() const;

    /**
     * Function to return an array of bicubic spline coefficients for a grid cell.
     * @param rIndex: index of grid cell along R. Ranges from 0 to rGridPoints.size()-1
     * @param zIndex: index of grid cell along Z. Ranges from 0 to zGridPoints.size()-1
     */ 
    std::array <double, 16> getBicubicSplineCoefficientsInCell(int rIndex, int zIndex) const;

    /**
     * Function to return an array of psi values at Chebyshev points for a grid cell.
     * @param rIndex: index of grid cell along R. Ranges from 0 to rGridPoints.size()-1
     * @param zIndex: index of grid cell along Z. Ranges from 0 to zGridPoints.size()-1
     */ 
    std::array <double, 16> getPsiAtChebyshevPointsInCell(int rIndex, int zIndex) const;
       
  private:
   /**
    * Function to read and set the grid data name from adios2 file.
    * @param io: adios2 io to interact with adios2 input file.
    */ 
   std::string setGridDataName(adios2::IO &io);

  // Variables
  std::string adiosFile;
  std::string gridDataName;
  EqdskGridData eqdskGridData; 
};
/* 
 * ** Not implemented yet.
 * Given arbitrary location in RZ, give back which physics region
 * double R (in): R-coordinate of the point to be tested.
 * double Z (in): Z-coordinate of the point to be tested.
 */
SurfaceIdType get_physics_region(double R, double Z);

#endif

