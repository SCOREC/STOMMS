#ifndef STOMMS_H
#define STOMMS_H

#include "input.h"
#include "magneticGeometry.h"
#include "modelMetaData.h"
#include "modeling.h"
#include "modelData.h"
#include "modelTopology.h"

// Class to initialize and delete Simmetrix handlers.
class STOMMS{
  public:
    STOMMS();
    ~STOMMS();
  private:
    pProgress prog;  // Simmetrix handler to show execution details.
};
#endif

