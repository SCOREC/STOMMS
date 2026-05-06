#include "stomms.h"
#include "modelTopology.h"
#include "SimLicense.h"

STOMMS::STOMMS()
{
  // Initialize Simmetrix handlers.
  Sim_logOn("stommsSim.log");
  MS_init();
  // NOTE: Sim_readLicenseFile() is for internal testing only.  To use,
  // pass in the location of a file containing your keys.  For a release 
  // product, use Sim_registerKey()
  //Sim_readLicenseFile("/opt/hpc/software/Simmetrix/simmetrix.lic");
  char simLic[128] = "/opt/hpc/software/Simmetrix/simmetrix.lic";
  std::cout<<__func__<<": simLic = "<<simLic<<"\n";
  SimLicense_start("geomsim_core,geomsim_adv,meshsim_surface,meshsim_adapt,meshsim_adv", simLic); 

  prog = Progress_new();
  Progress_setDefaultCallback(prog);

}

STOMMS::~STOMMS()
{
  // Delete Simmetrix handlers.
  Progress_delete(prog);
  SimLicense_stop();
  //Sim_unregisterAllKeys();
  MS_exit();
  Sim_logOff();

  std::cout <<"\n";
  std::cout <<"====================================================\n";
  std::cout << "Successful: End of Mesh Generation & Output Writing\n";
  std::cout <<"====================================================\n";
}
