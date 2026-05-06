#include "stomms.h"
#include "modelTopology.h"

#ifdef ENABLE_PPPL
  #include "SimLicense.h"
#endif

STOMMS::STOMMS()
{
  // Initialize Simmetrix handlers.
  Sim_logOn("stommsSim.log");
  MS_init();
  // NOTE: Sim_readLicenseFile() is for internal testing only.  To use,
  // pass in the location of a file containing your keys.  For a release 
  // product, use Sim_registerKey()
  #ifdef ENABLE_PPPL
    char simLic[128] = "/opt/hpc/software/Simmetrix/simmetrix.lic";
    SimLicense_start("geomsim_core,geomsim_adv,meshsim_surface,meshsim_adapt,meshsim_adv", simLic); 
  #else
    Sim_readLicenseFile(0);
  #endif 

  prog = Progress_new();
  Progress_setDefaultCallback(prog);
}

STOMMS::~STOMMS()
{
  // Delete Simmetrix handlers.
  Progress_delete(prog);

  #ifdef ENABLE_PPPL
    SimLicense_stop();
  #else
    Sim_unregisterAllKeys();
  #endif

  MS_exit();
  Sim_logOff();

  std::cout <<"\n";
  std::cout <<"====================================================\n";
  std::cout << "Successful: End of Mesh Generation & Output Writing\n";
  std::cout <<"====================================================\n";
}
