#include "output.h"

StommsOutput::StommsOutput(const StommsMesh& m):mesh(m)
{
  // Get underlying Simmetrix Model stored in mesh meta data.
  MeshMetaData meshMetaData = mesh.getMeshMetaData();
  StommsModel stommsModel = meshMetaData.getStommsModel();
  Model mdl = stommsModel.getModel();
  simModel = mdl.getSimModel();

  // Get Simmetrix mesh.
  simMesh = mesh.getSimMesh();

  // Get the planes data.
  planes = mesh.getMeshDataOnPlanes();

  // Write Simmetrix meshes to OmegaH meshes.
  writeOmegahMeshes(); 
 
  // Write data into adios2 file.
  writeAdiosFile();
}

void StommsOutput::writeOmegahMeshes()
{
  for (int i = 0; i < planes.size(); i++)
  {
    PlaneMeshData p = planes[i];
    auto mesh = simMesh2Omegah(p);
  }
  std::cout << "OmegaH mesh files written\n";
}

// Give Plane as input to this fucntion
Omega_h::Mesh StommsOutput::simMesh2Omegah(const PlaneMeshData& plane)
{
  // Get the Simmetrix mesh enteties on each plane and construct SimMesh m
  // to feed into Omegah APIs.
  PlaneMeshData p = plane;
  std::vector <pVertex> meshV = p.getMeshVerticesOnPlane();
  std::vector <pEdge> meshE = p.getMeshEdgesOnPlane();
  std::vector <pFace> meshF = p.getMeshFacesOnPlane();
  std::vector <pRegion> meshR = p.getMeshRegionsOnPlane();
  
  // Create an object of Omegah mesh.  
  auto lib = Omega_h::Library(NULL, NULL);
  auto comm = lib.world();

  auto mesh = Omega_h::Mesh(comm->library());
  mesh.set_comm(comm);
  mesh.set_parting(OMEGA_H_ELEM_BASED);

  // Construct SimMesh to use it in Omegah functions.
  Omega_h::meshsim::SimMesh m(meshV, meshE, meshF, meshR);

  auto info = Omega_h::meshsim::getSimMeshInfo(m);
  const bool hasNumbering = false;
  pMeshNex numbering = {NULL};  

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
