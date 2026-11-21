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
    {"fluxC_in_privateR_on", "fluxCurvesInPrivate"},
    {"meshSizeWall", "meshSizeUnstructured"},
  };

  // TOMMS options for which STOMMS supports only some values (TOMMS name -> option).
  const std::map <std::string, tomms::Option> options = {
    {"radial_uniform_meshing_unit", {{"-1"}, "STOMMS only supports flux curves given in a file (-1), set with 'fluxFile'."}},
    {"intra_curve_spacing_option", {{"-1", "-2"}, "STOMMS only supports intra curve spacing given in a file (-1, -2), set with 'meshSizeFile'."}},
  };

  // TOMMS parameters ignored by STOMMS (TOMMS name -> reason).
  const std::map <std::string, std::string> unusedParameters = {
    {"radial_uniform_meshing_unit", "STOMMS always uses -1 (flux curves from 'fluxFile')."},
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
    {"finite_domain_on", "not supported in STOMMS."},
    {"inter_curve_spacing_max", "not supported in STOMMS."},
    {"inter_curve_spacing_min", "not supported in STOMMS."},
    {"inter_curve_spacing_axis_forcing", "not supported in STOMMS."},
    {"grid_bdry_psi2", "not supported in STOMMS."},
    {"grid_bdry_psi2hfs", "not supported in STOMMS."},
    {"inter_curve_spacing_sol", "not supported in STOMMS."},
    {"wall1LayAdapt", "not supported in STOMMS."},
    {"distance_factor_vtx_adjacent2wall", "not supported in STOMMS."},
    {"meshSizeGradXpt", "not supported in STOMMS."},
    {"tolerance_meters", "not supported in STOMMS."},
    {"step_normalized", "not supported in STOMMS."},
    {"addinfoflxFile", "not supported in STOMMS."},
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

  // Function to handle legacy tomms parameters. 
  bool handleLegacyParameter(const std::string& name, std::istream& input)
  {
    std::string value;

    // Step 1: Check the parameters for which only few options are available. For example, 
    // radial_unifrom_meshing_unit = -1 is supported but not others.
    // (or the renaming) decide what happens to the parameter.
    auto option = options.find(name);
    if (option != options.end())
    {
      // Step 1.1: We are only checking if the value of the parameter is valid here. If 
      // its not valid, it will end up throwing an error by triggering the if statement.
      // If its a valid value, we want to make sure, to map it into STOMMS parameter (or
      // any other exception in steps 2 or 3). For that, we will need tothen read its value 
      // again. So we will store the position, read  value and then move back the stream 
      // position to handle this (to read value later).
      std::streampos position = input.tellg(); // saves the position before the value starts
      input >> value;
      input.seekg(position);  // move the stream back to position. Read it again later.
      if (option->second.availableValues.count(value) == 0)
        reportUnavailableOption(name, value, option->second.details);
    }

    // Step 2: Check if the parameter is not used by STOMMS.
    auto unused = unusedParameters.find(name);
    if (unused != unusedParameters.end())
    {
      input >> value;
      reportUnusedParameter(name, unused->second);
      return true;
    }

    // Step 3: Check if parameter is planned for a future STOMMS version.
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
