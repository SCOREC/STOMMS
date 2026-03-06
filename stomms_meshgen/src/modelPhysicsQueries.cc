#include "modelPhysicsQueries.h"

std::vector <pGFace> getPrivateModelFaces(pGModel model)
{
  std::vector <pGFace> faces;
  GFIter faceIter = GM_faceIter(model);
  while (pGFace gf = GFIter_next(faceIter))
  {
    if (GEN_numNativeIntAttribute(gf, "PhysicsRegion"))
    {
      int regionType = -1;
      GEN_nativeIntAttribute(gf, "PhysicsRegion", &regionType);
      if (regionType != static_cast<int>(FaceType::Private))
        continue;

      faces.push_back(gf);
    }
  }
  GFIter_delete(faceIter);

  return faces;

}

std::vector <pGFace> getSOLModelFaces(pGModel model)
{
  std::vector <pGFace> faces;
  GFIter faceIter = GM_faceIter(model);
  while (pGFace gf = GFIter_next(faceIter))
  {
    if (GEN_numNativeIntAttribute(gf, "PhysicsRegion"))
    {
      int regionType = -1;
      GEN_nativeIntAttribute(gf, "PhysicsRegion", &regionType);
      if (regionType == static_cast<int>(FaceType::Core)
          || regionType == static_cast<int>(FaceType::None)
          || regionType == static_cast<int>(FaceType::Private))
        continue;

      faces.push_back(gf);
    }
  }
  GFIter_delete(faceIter);

  return faces;
}


  
