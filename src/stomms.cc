#include "stomms.h"
#include "modelTopology.h"

STOMMS::STOMMS()
{
  // Initialize Simmetrix handlers.
  MS_init();
  // NOTE: Sim_readLicenseFile() is for internal testing only.  To use,
  // pass in the location of a file containing your keys.  For a release 
  // product, use Sim_registerKey()
  Sim_readLicenseFile(0);

  prog = Progress_new();
  Progress_setDefaultCallback(prog);

}

STOMMS::~STOMMS()
{
  // Delete Simmetrix handlers.
  Progress_delete(prog);
  Sim_unregisterAllKeys();
  MS_exit();
}
