#include "input.h"
#include "model.h"


int main(int argc, char* argv[])
{
  MS_init();
  // NOTE: Sim_readLicenseFile() is for internal testing only.  To use,
  // pass in the location of a file containing your keys.  For a release 
  // product, use Sim_registerKey()
  Sim_readLicenseFile(0);

  pProgress prog = Progress_new();
  Progress_setDefaultCallback(prog);

  // Step 1: Read the input file (mesh_input) for the input parameters
  args a(argc, argv);

  // Step 2: Generate the core region of the stellarator from the given VMEC file
  generateCoreSimModel(&a);


  Progress_delete(prog);

  Sim_unregisterAllKeys();
  MS_exit();
  return 0;


}

