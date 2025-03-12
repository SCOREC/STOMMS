#include "output.h"

StommsOutput::StommsOutput(const StommsMesh& m):mesh(m)
{
  std::cout << "============ Output Writing Starts ============\n";

  // Step 1: Get underlying Simmetrix Model stored in mesh meta data.
  MeshMetaData meshMetaData = mesh.getMeshMetaData();
  StommsModel stommsModel = meshMetaData.getStommsModel();
  Model mdl = stommsModel.getModel();
  simModel = mdl.getSimModel();

  // Step 2: Get Simmetrix mesh.
  simMesh = mesh.getSimMesh();

  // Step 3: Get the planes data.
  planes = mesh.getMeshDataOnPlanes();

  // Step 4: Write Simmetrix meshes to OmegaH meshes.
  writeOmegahMeshes(); 

  // Step 5: Write vtk meshes for visualization
  writeVtkPlanes(omegahMeshes);
 
  // Step 6: Write data into adios2 file.
  writeAdiosFile();
}

// Function to iterate over all the planes and creates omegah meshes
// for each individual plane. And store them in container (omegahMeshes).
void StommsOutput::writeOmegahMeshes()
{
  for (int i = 0; i < planes.size(); i++)
  {
    PlaneMeshData p = planes[i];
    std::cout << "          Plane # " << i << "          \n";
    auto mesh = simMesh2Omegah(p);
    omegahMeshes.push_back(mesh);
    std::cout << "Omegah mesh for plane # " << i << " is written\n";
    std::cout << "===============================================\n";
  }
  std::cout << "OmegaH mesh files Done\n";
}

// To make sure, vertex indices start from 0 to nVertices-1. 
void StommsOutput::setMeshIndices(std::vector <pVertex> v)
{
  double xyz[3];
  for (int i = 0; i < v.size(); i++)
  {
    V_coord(v[i], xyz);
    EN_setID(v[i],i);
  }
}

// Give Plane as input to this fucntion
Omega_h::Mesh StommsOutput::simMesh2Omegah(const PlaneMeshData& plane)
{
  // Step 1: Get the Simmetrix mesh enteties on each plane and construct SimMesh m
  // to feed into Omegah APIs.
  PlaneMeshData p = plane;
  std::vector <pVertex> meshV = p.getMeshVerticesOnPlane();
  std::vector <pEdge> meshE = p.getMeshEdgesOnPlane();
  std::vector <pFace> meshF = p.getMeshFacesOnPlane();
  std::vector <pRegion> meshR = p.getMeshRegionsOnPlane();

  // Step 2: Make sure vertex indices are consistent and starts from 0 on each plane
  setMeshIndices(meshV);

  // Step 3: Create an object of Omegah mesh.  
  auto lib = Omega_h::Library(NULL, NULL);
  auto comm = lib.world();

  auto mesh = Omega_h::Mesh(comm->library());
  mesh.set_comm(comm);
  mesh.set_parting(OMEGA_H_ELEM_BASED);

  // Step 4: Construct SimMesh to use it in Omegah functions.
  Omega_h::meshsim::SimMesh m(meshV, meshE, meshF, meshR);

  // Step 5: Get entity info from gives mesh data and convert it to omegah mesh.
  auto info = Omega_h::meshsim::getSimMeshInfo(m);
  const bool hasNumbering = false;
  pMeshNex numbering = {};  

  Omega_h::meshsim::SimMeshEntInfo simEnts(m, hasNumbering);
  Omega_h::meshsim::setEntToMesh(&mesh, simEnts, numbering, info);

  return mesh; 
}

void StommsOutput::writeAdiosFile()
{
  // Pracitce code block for adios2.
  adios2::ADIOS adiosTestObject;
  const std::string versionNumber = "=== STOMMS version 1.0 ===";
  adios2::IO io = adiosTestObject.DeclareIO("STOMMS Mesh Writer");
  adios2::Engine writer = io.Open("stomms.bp", adios2::Mode::Write);
  adios2::Variable<std::string> versionVariable = io.DefineVariable<std::string>("Version");
  writer.BeginStep();
  writer.Put(versionVariable, versionNumber);
  writer.EndStep();
  writer.Close();
  std::cout << "Adios2 file written\n";
}

// Free functions to write output files.
void writeOmegah2Vtk(Omega_h::Mesh mesh, int planeNum)
{
  int dim = 2;
  std::string meshName = "meshPlane_" + std::to_string(planeNum) + ".vtk";
  Omega_h::vtk::write_parallel(meshName, &mesh, dim);
}

void writeVtkPlanes(const std::vector <Omega_h::Mesh>& omegahMeshPlanes)
{
  for (int i = 0; i < omegahMeshPlanes.size(); i++)
  {
    Omega_h::Mesh m = omegahMeshPlanes[i];
    writeOmegah2Vtk(m,i);
  }
}

// To debug the routines for output writing - Once development is done, we 
// can get rid of these debug functions.
void debugMesh(std::vector <pVertex> v, std::vector <pEdge> e, 
          std::vector <pFace> f, std::vector <pRegion> r) {
  
  FILE* fp =fopen("meshInfo.txt","w");

  fprintf(fp, "Number of Mesh Vertices = %d\n", v.size());
  double xyz[3];
  // Iterate over vertices
  for (int i = 0; i < v.size(); i++)
  {
    V_coord(v[i], xyz);
    EN_setID(v[i],i);
    fprintf(fp, "%d %lf %lf %lf \n", EN_id(v[i]), xyz[0], xyz[1], xyz[2]);
  }
  fprintf(fp, "=======================================\n");
  fprintf(fp, "Number of Mesh Edges = %d\n", e.size());
  for (int i = 0; i < e.size(); i++)
  {
    pVertex v0 = E_vertex(e[i], 0);
    pVertex v1 = E_vertex(e[i], 1);
    int v_0 = EN_id(v0);
    int v_1 = EN_id(v1);
    fprintf(fp, "%d %d %d \n", EN_id(e[i]), v_0, v_1);
  }
  fprintf(fp, "=======================================\n");
  fprintf(fp, "Number of Mesh Faces = %d\n", f.size());
  for (int i = 0; i < f.size(); i++)
  {
    pPList vOnF = F_vertices(f[i], 1);
    pVertex v0 = static_cast<pVertex>(PList_item(vOnF, 0));
    pVertex v1 = static_cast<pVertex>(PList_item(vOnF, 1));
    pVertex v2 = static_cast<pVertex>(PList_item(vOnF, 2));
    int v_0 = EN_id(v0);
    int v_1 = EN_id(v1);
    int v_2 = EN_id(v2);
    fprintf(fp, "%d %d %d %d \n", EN_id(f[i]), v_0, v_1, v_2);
  }
  fprintf(fp, "=======================================\n");
  fprintf(fp, "Number of Mesh Regions = %d\n", r.size());
  
  fclose(fp);
}
