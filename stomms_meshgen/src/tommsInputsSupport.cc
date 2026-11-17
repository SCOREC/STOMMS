#include "tommsInputsSupport.h"

#include <cstdlib>
#include <iostream>
#include <map>

namespace
{
  // TOMMS parameter names with a direct STOMMS equivalent (TOMMS name -> STOMMS name).
  // Parameters with the same name in both codes (eqdskFile, limiterFile) are not listed.
  const std::map <std::string, std::string> renamedParameters = {
    {"reverse_psi", "reversePsi"},
    {"eqd_psi_factor", "eqdPsiFactor"},
    {"num_phi_planes","numPlanes"},
    {"random_start", "fluxRandomStart"},
    {"inboard_start", "inboardStart"},
    {"step_radians", "stepRadians"},
    {"tolerance_psi", "psiTolerance"},
    {"spacing_tolerance_optimal", "spacingToleranceOptimal"},
    {"spacing_tolerance_absolute","spacingToleranceAbsolute"},
    {"intra_curve_spacing_option","intraCurveSpacingOption"},
    {"intra_curve_minlength_lastedge", "intraCurveMinLengthLastEdge"},
    {"intra_curve_spacing_propf_max",  "intraCurveSpacingPropFacMax"},
    {"intra_curve_spacing_propf_min",  "intraCurveSpacingPropFacMin"},
    {"intra_curve_spacing_small_variation", "intraCurveSpacingSmallVariation"},
    {"last_closed_psi", "lastClosedPsi"},
    {"wall_0Xpt", "useWall"},  
    {"inter_curve_spacing_file", "fluxFile"},
    {"intra_curve_spacing_file", "meshSizeFile"},
    {"meshSizeWall", "meshSizeUnstructured"},
    {"inter_curve_spacing_file", "fluxFile"},
    {"intra_curve_spacing_file", "meshSizeFile"},
  };

  // TOMMS options for which STOMMS supports only some values (TOMMS name -> option).
  const std::map <std::string, tomms::Option> options = {
    {"radial_uniform_meshing_unit", {{"-1"}, "STOMMS only supports flux curves given in a file (-1), set with 'fluxFile'."}},
  };

  // TOMMS parameters ignored by STOMMS (TOMMS name -> reason).
  const std::map <std::string, std::string> unusedParameters = {
    {"max_step_meters", "constant in STOMMS (0.0025)."},
    {"tolerance_psi_deriv", "constant in STOMMS (1e-10)."},
    {"parallel_cutoff", "constant in STOMMS (0.99999)."},
    {"SimLic", "not supported in STOMMS."},
    {"adv_log", "not supported in STOMMS."},
    {"nodeFile", "not supported in STOMMS."},
    {"elmFile", "not supported in STOMMS."},
    {"output_vtk_steps", "not supported in STOMMS."},
    {"inter_curve_spacing", "not supported in STOMMS."},
    {"intra_curve_spacing", "not supported in STOMMS."},
    {"meshSizeXpt", "not supported in STOMMS."},
    {"meshSizeGrad", "not supported in STOMMS."},
    {"maxAspectRatioXpt", "not supported in STOMMS."},
    {"bdyNodeOrder", "not supported in STOMMS."},
  };

  // TOMMS parameters not available in STOMMS yet, but planned for the future.
  const std::set <std::string> plannedParameters = {
    "wallSplinesOn",
    "transitionFacesOn",
    "inter_curve_spacing_private_on",
    "inter_curve_spacing_private_file",
    "inter_curve_spacing_privateR7",
    "inter_curve_spacing_privateR8",
  };
}

// This namespace handles all the functions that are needed to deal with old tomms
// parameters.
namespace tomms
{

  // Function to translate a TOMMS parameter name to its STOMMS name.
  std::string translateParameterName(const std::string& name)
  {
    // Step 1: Find if the parameter exists in the conversion map, if not return.
    auto it = renamedParameters.find(name);
    if (it == renamedParameters.end())
      return name;

    // Step 2: If its in the map, print a message with new parameter name, and return the equivalent name
    // in STOMMS.
    std::cout << "WARNING: '" << name << "' is a deprecated TOMMS parameter. Use '" << it->second << "' instead.\n";
    return it->second;
  }

  
  bool handleLegacyParameter(const std::string& name, std::istream& input)
  {
    std::string value;

    // Option: only some values are available. STOMMS always uses the available
    // value, so the parameter itself is not needed.
    auto option = options.find(name);
    if (option != options.end())
    {
      input >> value;
      if (option->second.availableValues.count(value) == 0)
        reportUnavailableOption(name, value, option->second.details);
      reportUnusedParameter(name, "STOMMS always uses " + value + ".");
      return true;
    }

    // Parameter not used by STOMMS.
    auto unused = unusedParameters.find(name);
    if (unused != unusedParameters.end())
    {
      input >> value;
      reportUnusedParameter(name, unused->second);
      return true;
    }

    // Parameter planned for a future STOMMS version.
    if (plannedParameters.count(name))
    {
      input >> value;
      reportPlannedParameter(name);
      return true;
    }

    return false;
  }

  // Function to send an error message for the option not available in STOMMS.
  void reportUnavailableOption(const std::string& name, const std::string& value,
                               const std::string& details)
  {
    std::cerr << "ERROR: TOMMS option '" << name << " " << value << "' is not available in STOMMS. " << details << "\n";
    std::cerr << "If you need this option, please contact us: " << contactLink << "\n";
    exit(1);
  }

  // Function to return a warning for a parameter ignored by STOMMS (not supported, or a constant in STOMMS).
  void reportUnusedParameter(const std::string& name, const std::string& reason)
  {
    std::cout << "WARNING: TOMMS parameter '" << name << "' is ignored: " << reason << "\n";
  }

  // Function to return a warning for a parameter not available in STOMMS yet but planned for the future.
  void reportPlannedParameter(const std::string& name)
  {
    std::cout << "WARNING: TOMMS parameter '" << name << "' is ignored: not available in STOMMS yet. "
              << "We will add this option in a future version. "
              << "If you need it now, please contact us: " << contactLink << "\n";
  }
}
