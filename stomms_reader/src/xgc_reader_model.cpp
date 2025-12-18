#include "xgc_reader_model.hpp"

/****************************/
// Class ModelVertex
/****************************/
ModelVertex::ModelVertex(int modelVertexId, const PointType& vertexType): geomId(modelVertexId), pointType(vertexType)
{
  // Add more properties if needed
}

// Function to return physics type of the model vertex.
const PointType& ModelVertex::getPointType()
{
  return pointType;
}

// Function to return the topology type of model entity.
const TopoType& ModelVertex::getTopoType()
{
  return topoType;
}

// Function to return geometric id of the model vertex.
const GeomIdType& ModelVertex::getGeomId()
{
  return geomId;
}

/****************************/
// Class ModelEdge
/****************************/
ModelEdge::ModelEdge(int modelEdgeId, int curveNumber)
{
  geomId = modelEdgeId;
  curveId = curveNumber;
}

// Function to return the curve id of the model curve on which model edge is classified on.
const int& ModelEdge::getCurveId()
{
  return curveId;
}

// Function to return geometric id of the model edge.
const GeomIdType& ModelEdge::getGeomId()
{
  return geomId;
}

// Function to return the topology type of model entity.
const TopoType& ModelEdge::getTopoType()
{
  return topoType;
}

/****************************/
// Class ModelCurve
/****************************/
ModelCurve::ModelCurve(int curveNumber, const CurveType& type, double psi, const std::vector <int>& edges)
{
  curveId = curveNumber;
  curveType = type;
  curvePsi = psi;
  modelEdgesGeomIds = edges;
}

// Function to return index of the curve in the model.
const int& ModelCurve::getCurveId()
{
  return curveId;
}

// Function to return the physics type of the curve.
const CurveType& ModelCurve::getCurveType()
{
  return curveType;
}

// Function to return the topology type of model entity.
const TopoType& ModelCurve::getTopoType()
{
  return topoType;
}

// Function to return a vector of model edges id classified on the model curve.
const std::vector <GeomIdType>& ModelCurve::getModelEdgesGeomIds()
{
  return modelEdgesGeomIds;
}

// Function to return psi value on the flux curve.
const double& ModelCurve::getCurvePsi()
{
  return curvePsi;
}

/****************************/
// Class ModelFace
/****************************/
ModelFace::ModelFace(int modelFaceId, const SurfaceType& type):geomId(modelFaceId),surfaceType(type)
{
  // Add more properties if needed
}

// Function to return physics surface type of the model face.
const SurfaceType& ModelFace::getSurfaceType()
{
  return surfaceType;
}

// Function to return the topology type of model entity.
const TopoType& ModelFace::getTopoType()
{
  return topoType;
}

// Function to return geometric id of the model face.
const GeomIdType& ModelFace::getGeomId()
{
  return geomId;
}

/****************************/
// Class Model
/****************************/
Model::Model(const adios2::IO& io, const adios2::Engine& reader, const std::string& name, int planeNumber)
      :ioPlane(io), readerPlane(reader), meshName(name), planeNum(planeNumber)
{
  // Step 1: Find the groups for the particular planes
  auto g = ioPlane.InquireGroup('/');  // group identifier
  std::string groupName = meshName + "/planes/" + std::to_string(planeNum) + "/physicsClassification";
  g.setPath(groupName);

  // Step 2: Read groups (Physics classification on model entities).
  auto groups = g.AvailableGroups();
  for (const auto &g : groups)
  {
    std::string name;

    // Step 2.1: Set the model vertices data
    if (g == "vertex")
    {
      name = groupName + "/vertex";
      setModelVertices(name);
    }

    // Step 2.2: Set the model curves data
    if (g == "edge")
    {
      name = groupName + "/edge";
      setModelCurves(name);
    }

    // Step 2.3: From curves data, set model edges
    setModelEdges();

    // Step 2.4: Set the model faces data
    if (g == "face")
    {
      name = groupName + "/face";
      setModelFaces(name);
    }
  }
}

// Function to set model vertices from adios2 input file.
void Model::setModelVertices(const std::string& groupName)
{
  auto g = ioPlane.InquireGroup('/');
  g.setPath(groupName);
  auto groups = g.AvailableGroups();

  // Step 1: Group for top level because we can have multiple properties
  for (const auto &grp : groups)
  {
    std::string name = groupName + "/" + grp;
    g.setPath(name);
    auto vertexTypeGroups = g.AvailableGroups();

    // Step 2: Iterate over the vertex properties.
    for (const auto &vertexGroup : vertexTypeGroups)
    {
      PointType pType;
      std::string vName;
      if (vertexGroup == "oPoint")
      {
        pType = PointType::OPoint;
        vName = name + "/oPoint";
      }
      else if (vertexGroup == "xPoint")
      {
        pType = PointType::XPoint;
        vName = name + "/xPoint";
      }
   
      std::vector <std::string>  variables = findVarInGroup(ioPlane, vName);

      // Step 3: Iterate over the number of critical points of each type and 
      // set model vertices in the model.
      for (int i = 0; i < variables.size(); i++)
      {
        std::string variableName = variables[i];
        int32_t vertexIndx;
        readAdios2Value(ioPlane, readerPlane, &vertexIndx, variableName);
        ModelVertex modelVertex(vertexIndx, pType);  
        modelVertices.push_back(modelVertex); 
      }
    }
  }
}

// Function to set model curves from adios2 input file.
void Model::setModelCurves(const std::string& groupName)
{
  auto g = ioPlane.InquireGroup('/');
  g.setPath(groupName);
  auto groups = g.AvailableGroups(); 

  // Step 1: Loop over the groups and find out the type of 
  // curve (open, closed, separatrix or wall)
  CurveType cType;

  for (const auto &grp : groups)
  {
    // Step 1.2: set curve type to the types (grp) from adios2 file
    if (grp == "closed")
      cType = CurveType::Closed;
    if (grp == "open")
      cType = CurveType::Open;
    if (grp == "separatrix")
      cType = CurveType::Separatrix;
    if (grp == "wall")
      cType = CurveType::Wall;

    // Variables to read data in
    std::vector <int32_t> fluxIds, modelEdgesRange, modelEdgesData;
    std::vector <double> psi;

    // Step 2: Read groups data (fluxIds, psi values, and modeledges data)
    std::string name = groupName + "/" + grp;
    std::string variableName = name + "/fluxIds";
    readAdios2Array(ioPlane, readerPlane, fluxIds, variableName);
    variableName = name + "/psi";
    readAdios2Array(ioPlane, readerPlane, psi, variableName);
    variableName = name + "/modelEdges/range";
    readAdios2Array(ioPlane, readerPlane, modelEdgesRange, variableName);
    variableName = name + "/modelEdges/data";
    readAdios2Array(ioPlane, readerPlane, modelEdgesData, variableName);

    // Step 3: Set the model curves to the model.
    for (int i = 0; i < fluxIds.size(); i++)
    {
      int curveNum = fluxIds[i];
      double psiValue = psi[i];
      std::vector <int> modelEdgesOnCurve;
      for (int j = modelEdgesRange[i]; j < modelEdgesRange[i+1]; j++)
        modelEdgesOnCurve.push_back(modelEdgesData[j]);

      ModelCurve curve(curveNum, cType, psiValue, modelEdgesOnCurve);
      modelCurves[curveNum] = curve;
    }
  } 
}

// Function to set model edges from the model curves already read from adios2 file.
void Model::setModelEdges()
{
  for (auto& curve:modelCurves)
  {
    ModelCurve c = curve.second;
    std::vector <GeomIdType> edges = c.getModelEdgesGeomIds();
    int curveId = c.getCurveId();
    for (int i = 0; i < edges.size(); i++)
    {
      int edgeId = edges[i];
      ModelEdge edge(edgeId, curveId);
      modelEdges[edgeId] = edge;
    }
  }
}

// Function to set model faces from adios2 input file.
void Model::setModelFaces(const std::string& groupName)
{
  auto g = ioPlane.InquireGroup('/');
  g.setPath(groupName);
  auto groups = g.AvailableGroups();
 
  // Step 1: Find the variables for model face (core, sol etc.) and then 
  // set model faces in the model for each face type.
  std::vector <std::string>  variables = findVarInGroup(ioPlane, groupName);
  setModelFacesFromVariables(variables); 
}

// Given the name of the variables in the adios2 file, read the model faces data
// those variables hold.
void Model::setModelFacesFromVariables(std::vector <std::string> variables)
{
  SurfaceType sType;
  int pvtIndx = 1;

  // Step 1: Iterate over face types (differentiated with variables), and read the
  // ids of model faces store in them.
  for (int i = 0; i < variables.size(); i++)
  {
    std::string variableName = variables[i];
    sType = getSurfaceTypeForString(variableName);
    std::vector <int32_t> faceIds;
    readAdios2Array(ioPlane, readerPlane, faceIds, variableName); 

    // Step 2: Iterate over the model faces for each type and
    // set them in the model.
    for (int j = 0; j < faceIds.size(); j++)
    {
      int faceId = faceIds[j];
      ModelFace face(faceId, sType);
      modelFaces[faceId] = face;
    }

    // Step 3: Since private regions can be multiple, set them at their
    // respective private region index.
    if (sType == SurfaceType::Private)
    {
      setPrivateModelFaces(pvtIndx, faceIds);
      pvtIndx++;
    }
  }
}

// Given the private region index, and indices of model faces on it, set a map of private
// region and corresponding vector of model faces on it.
void Model::setPrivateModelFaces(int privateRegionIndex, const std::vector <int>& faceIds)
{
  std::vector <ModelFace> pvtFaces;
  SurfaceType sType = SurfaceType::Private;

  // Iterate over the given model faces and set them one by one in the model.
  for (int i = 0; i < faceIds.size(); i++)
  {
    int faceId = faceIds[i];
    ModelFace face(faceId, sType);
    pvtFaces.push_back(face);
  }

  privateRegions[privateRegionIndex] = pvtFaces;
}

// Function to return physics model vertices on the model.
const std::vector <ModelVertex>& Model::getModelVertices() const
{
  return modelVertices;
}

// Function to return all the model edges on the model.
const std::map <EdgeIdType, ModelEdge>& Model::getModelEdges() const
{
  return modelEdges;
}

// Function to return all the model curves on the model.
const std::map <CurveIdType, ModelCurve>& Model::getModelCurves() const
{
  return modelCurves;
}

// Function to return all the model faces on the model.
const std::map <SurfaceIdType, ModelFace>& Model::getModelFaces() const
{
  return modelFaces;
}

// Function to return a map bewtween private region index and a vector of 
// corresponding model faces on that particular private region.
const std::map<int, std::vector <ModelFace>>& Model::getPrivateRegions() const
{
  return privateRegions;
}

// Function to get the model vertex ids of the all the Xpoints in the model.
std::vector <VertexIdType> Model::getXpoints() const
{
  std::vector <VertexIdType> xPoints;
  for (int i = 0; i < modelVertices.size(); i++)
  {
    ModelVertex v = modelVertices[i];
    if (v.getPointType() == PointType::XPoint)
      xPoints.push_back(v.getGeomId());
  }
  
  return xPoints;
}

// Function to get the model vertex ids of the all the Opoints in the model.
std::vector <VertexIdType> Model::getOpoints() const
{
  std::vector <VertexIdType> oPoints;
  for (int i = 0; i < modelVertices.size(); i++)
  {
    ModelVertex v = modelVertices[i];
    if (v.getPointType() == PointType::OPoint)
      oPoints.push_back(v.getGeomId());
  }
  
  return oPoints;
}

// Given the id of the model vertex, return the model vertex.
ModelVertex Model::getModelVertexFromId(const VertexIdType& vertexId) const
{
  ModelVertex v;
  for (int i = 0; i < modelVertices.size(); i++)
  {
    ModelVertex vToCheck = modelVertices[i];
    if (vToCheck.getGeomId() == vertexId)
    {
      v = vToCheck;
      break;
    }
  }
  return v;
}

// Given the type of model curve, this function returns all the model curve of
// that particular type.
std::vector <CurveIdType> Model::getModelCurvesFromType(const CurveType& curveType) const
{
  std::vector <CurveIdType> curves;
  for (const auto& c: modelCurves)
  {
    ModelCurve curve = c.second;
    if (curve.getCurveType() == curveType)
      curves.push_back(curve.getCurveId());
  }

  return curves;
}

// Given the id of the model edge, return the model edge.
ModelEdge Model::getModelEdgeFromId(const EdgeIdType& edgeId) const
{
  ModelEdge modelEdge;
  if (modelEdges.find(edgeId) != modelEdges.end())
    modelEdge = modelEdges.at(edgeId);
  else
  {
    std::cout << "Error: Incorrect Model Edge Id\n";
    std::cout << "Error: Model Edge Id = " << edgeId << " does not exist\n";
    exit(1);
  }
  return modelEdge;
}

// Given the model curve id, this function returns the model curve on it.
ModelCurve Model::getModelCurveFromId(const CurveIdType& curveId) const
{
  ModelCurve modelCurve = modelCurves.at(curveId);
  return modelCurve;
}

// Given the model edge id, this function returns the id of model curve on it.
CurveIdType Model::getModelCurveFromEdgeId(const EdgeIdType& edgeId) const
{
  ModelEdge modelEdge = getModelEdgeFromId(edgeId);
  int curveId = modelEdge.getCurveId();
  return curveId;
}

// Given the id of the model face, return the index of private region on 
// which model face is classified on.
int Model::getPrivateRegionIndex(const SurfaceIdType& fId) const
{
  int pvtId = -1;
  bool pvtFound = false;
  for (auto& pvt:privateRegions)
  {
    for (int i = 0; i < pvt.second.size(); i++)
    {
      ModelFace f = pvt.second[i];
      if (f.getGeomId() == fId)
      {
        pvtId = pvt.first;
        pvtFound = true;
        break;
      }
    }
    if (pvtFound)
      break;
  }
  
  if (!pvtFound)
  {
    std::cerr << "The model face with id " << fId << "is not classified on a private region\n";
    exit(0);
  }

  return pvtId;
}

