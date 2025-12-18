#include "xgc_reader_utility.hpp"

/**********************************/
// ADIOS2 Reader Utility Functions
/**********************************/

// Given a group name, returns all the adios2 variables.
std::vector <std::string> findVarInGroup(adios2::IO &io, std::string groupName)
{
  auto variables = io.AvailableVariables();
  std::vector <std::string> varsInGroup;

  for (const auto &v : variables)
  {
    const std::string &name = v.first;

    // Compare variable name with group name
    if (name.rfind(groupName + "/", 0) == 0)
    {
        varsInGroup.push_back(name);
    }
  }
  
  return varsInGroup;
}


/**********************************/
// Other Utility Functions
/**********************************/

// Given the name of the full varibale (from adios2), extract the model surface type.
SurfaceType getSurfaceTypeForString(std::string name)
{
  SurfaceType sType;
  if (name.compare(name.length() - 4, 4, "core") == 0)
    sType = SurfaceType::Core;
  else if (name.compare(name.length() - 30, 30, "nearVacuumRegion/highFieldSide") == 0)
    sType = SurfaceType::HighFieldSideNearVacuum;
  else if (name.compare(name.length() - 29, 29, "nearVacuumRegion/lowFieldSide") == 0)
    sType = SurfaceType::LowFieldSideNearVacuum;
  else if (name.compare(name.length() - 30, 30, "plasmaEdgeRegion/highFieldSide") == 0)
    sType = SurfaceType::HighFieldSideEdge;
  else if (name.compare(name.length() - 29, 29, "plasmaEdgeRegion/lowFieldSide") == 0)
    sType = SurfaceType::LowFieldSideEdge;
  else if (name.compare(name.length() - 19, 13, "privateRegion") == 0)
    sType = SurfaceType::Private;
  else if (name.compare(name.length() - 3, 3, "sol") == 0)
    sType = SurfaceType::ScrapeOffLayer;
  else
    sType = SurfaceType::None;
     
  return sType;
}

// Given two points, find the 2D distance between them.
double dist2DPointToPoint(const std::array<double,2>& point1, const std::array<double,2>& point2)
{
  double dist = sqrt((point2[0] - point1[0])*(point2[0] - point1[0]) + (point2[1] - point1[1])*(point2[1] - point1[1]));
  return dist;
}
