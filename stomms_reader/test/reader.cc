#include "xgc_reader_interface.hpp"
#include <string>

// Function to check if a string has given extension
// or not.
bool hasExtension(std::string s, std::string ext) 
{
  if(s.substr(s.find_last_of(".") + 1) == ext) 
    return true;
  else 
    return false;
}

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);
  {
    // Step 1: Verify the inputs and read the input adios2 file
    if (argc < 2)
    {
      std::cout << "Adios2 file missing\n";
      std::cout << "Usage: ./reader Adios2_filename\n";
      exit(1);
    }
  
    std::string adios2File;
    if(hasExtension(argv[1], "bp") || hasExtension(argv[1], "bp/"))
    { 
      adios2File = argv[1];
      std::cout << "Input Adios2 File: " << adios2File << "\n";
    }
    else
      std::cout << "Adios2 file with extension .bp is missing\n";
  
    // Step 2: Create a mesh reader object
    XgcMesh xgcMesh(adios2File);

    std::cout << "\n";
    std::cout <<"***************************************************\n";
    std::cout <<"**************** In Test Function *****************\n";
    std::cout <<"***************************************************\n"; 
    std::cout << "Mesh name: " << xgcMesh.getMeshName() << "\n";
    std::cout << "Number of poloidal planes: " << xgcMesh.getNumPlanes() << "\n";

    for (int i = 0; i < xgcMesh.getNumPlanes(); i++)
    {
      // Verify the query functions:
      SurfaceIdType sId = 5;
      std::cout << "Model face Type for Surface Id " << sId << " = " << static_cast<int>(xgcMesh.getPhysicsRegionType(sId, i)) << "\n";

      CurveIdType cId = 19;
      std::cout << "Model curve type for curve id " << cId << " = " << static_cast<int>(xgcMesh.getCurveType(cId, i)) << "\n";

      std::vector <EdgeIdType> edgesOnCurve = xgcMesh.getCurveGeometricEdges(cId, i);
      std::cout << "Number of Edges on the curve = " << edgesOnCurve.size() << "\n";
      for (const auto& c:edgesOnCurve)
        std::cout << "Edge Id = " << c << "\n";
     
      Omega_h::Mesh omegahMesh = xgcMesh.getOmegahMeshatPlane(i);
      int numVertices = omegahMesh.nverts();
      int numEdges = omegahMesh.nedges();
      int numFaces = omegahMesh.nfaces();
      std::cout << "Number of mesh vertices in Omegah Mesh = " << numVertices << "\n";
      std::cout << "Number of mesh edges in Omegah Mesh = " << numEdges << "\n";
      std::cout << "Number of mesh faces in Omegah Mesh = " << numFaces << "\n";

      // Testing critical points function
      std::vector <VertexIdType> xPoints = xgcMesh.getCriticalPoints(PointType::XPoint, i);
      std::vector <VertexIdType> oPoints = xgcMesh.getCriticalPoints(PointType::OPoint, i);
      std::cout << "Xpoint on plane " << i << " : ";
      for (int j = 0; j < xPoints.size(); j++)
        std::cout << xPoints[j] << " ";
      std::cout << "\n";      

      std::cout << "Opoint on plane " << i << " : ";
      for (int j = 0; j < oPoints.size(); j++)
        std::cout << oPoints[j] << " ";
      std::cout << "\n";
     
      // Testing curves from curve type function
      std::vector <CurveIdType> curves = xgcMesh.getCurvesForType(CurveType::Separatrix, i);
      std::cout << "Curves on plane " << i << " : ";
      for (int j = 0; j < curves.size(); j++)
        std::cout << curves[j] << " ";
      std::cout << "\n"; 

      // Testing out mesh classification functions
      MeshIdType id = 10;
      PlaneId pId = i;
      TopoType vDim = xgcMesh.getGeometricTopology(TopoType::Vertex, id, pId);  
      TopoType eDim = xgcMesh.getGeometricTopology(TopoType::Edge, id, pId);
      TopoType fDim = xgcMesh.getGeometricTopology(TopoType::Face, id, pId);
      GeomIdType vGeomId = xgcMesh.getGeometricClassification(TopoType::Vertex, id, pId);
      GeomIdType eGeomId = xgcMesh.getGeometricClassification(TopoType::Edge, id, pId);
      GeomIdType fGeomId = xgcMesh.getGeometricClassification(TopoType::Face, id, pId);
      std::cout << "Mesh Vertex # " << id << " is classified on Model Ent of dimension = " << static_cast<int>(vDim) << "\n";
      std::cout << "Mesh Vertex # " << id << " is classified on Model Ent = " << vGeomId << "\n";
      std::cout << "Mesh Edge # " << id << " is classified on Model Ent of dimension = " << static_cast<int>(eDim) << "\n";
      std::cout << "Mesh Edge # " << id << " is classified on Model Ent = " << eGeomId << "\n";
      std::cout << "Mesh Face # " << id << " is classified on Model Ent of dimension = " << static_cast<int>(fDim) << "\n";
      std::cout << "Mesh Face # " << id << " is classified on Model Ent = " << fGeomId << "\n";

      const auto coords = omegahMesh.coords();
      
      // Testing reverse classification
      for (int i = 0; i < 3; i++)
      {
        TopoType gTopo = static_cast<TopoType>(i);
        int meshDim = i;
        if (i == 2)
          meshDim = 1;
        TopoType mTopo = static_cast<TopoType>(meshDim);
        GeomIdType entId = 1;
        std::vector <MeshIdType> meshEnt = xgcMesh.getMeshEntities(gTopo, entId, mTopo, true, pId);

        std::cout << "Mesh Entities of dim = " << static_cast<int>(mTopo) <<  " on Model Entity of dim = " << static_cast<int>(gTopo) << " ,and geometric id =  " << entId << " are given below\n";
        for (int j = 0; j < meshEnt.size(); j++)
          std::cout << meshEnt[j] << " ";
        std::cout << "\n";
      }

      // Test Model Topology
      std::cout << " ==================== Testing Model Topology Data ============== \n";
      int entId = 270;
      TopoType inTopo = TopoType::Face;
      TopoType outTopo = TopoType::Edge;
      std::vector <GeomIdType> gEnts = xgcMesh.getModelAdjEnts(inTopo, entId, outTopo, pId);
      std::cout << "Model entities of dim = " << static_cast<int>(outTopo) << " adjacent to model entity of dim = " << static_cast<int>(inTopo) << " ,and geometric id =  " << entId << " are given below\n";
      std::cout << "Number of adjacent entities = " << gEnts.size() << "\n";
      for (int i = 0; i < gEnts.size(); i++)
        std::cout << gEnts[i] << " " ;
      std::cout << "\n";

      // Test adjacent X-point function
      int surfaceId = 215;
      std::vector<int> xPts = xgcMesh.getAdjacentXpoints(surfaceId, 0);
      std::cout << "Number of XPoints on Private Region with model face # " << surfaceId << " are " << xPts.size() << "\n";
      for (int k = 0; k < xPts.size(); k++)
        std::cout << xPts[k] << " ";
      std::cout << "\n";
 
     // Test physics region curves
     int checkRegion = 1;
     
     int numFluxCurves = xgcMesh.getNumCurvesAtRegion(checkRegion, 0);
     std::vector <CurveIdType> fluxCurves = xgcMesh.getCurvesAtRegion(checkRegion, 0);
     std::cout << "Number of flux curves in sol = " << numFluxCurves << "\n";
     for (int i = 0; i < fluxCurves.size(); i++)
       std::cout << "Model Curve # " << fluxCurves[i] << "\n";

     // Test non-aligned vertices
     std::map<MeshIdType, std::vector <CurveIdType>> nonAlignedVertices = xgcMesh.getNonAlignedMeshVerticesOnPlane(0);
     std::cout << "Number of non-aligned vertices = " << nonAlignedVertices.size() << "\n";
    } 
  }
  MPI_Finalize();

  return 0;
}
