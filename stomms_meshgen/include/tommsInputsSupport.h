#ifndef TOMMS_INPUTS_SUPPORT_H
#define TOMMS_INPUTS_SUPPORT_H

#include <istream>
#include <set>
#include <string>

/** 
 * 2026-11-17: Compatibility Layer for TOMMS.
 * Backward compatibility for TOMMS input files (xgc_mesh_in).
 * Can be removed once users have moved to STOMMS input files (mesh_input).
 */
namespace tomms
{
  // Name of the TOMMS input file.
  const std::string inputFileName = "xgc_mesh_in";

  // Contact information in case a capability is desired that was in TOMMS and not in
  // STOMMS
  const std::string contactLink = "https://scorec.github.io/STOMMS/src_files/contact.html";

  // A TOMMS option for which STOMMS supports only some values.
  struct Option
  {
    std::set <std::string> availableValues;  // Values available in STOMMS.
    std::string details;                     // Shown to the user if the value is not available.
  };

  /**
   * Translate a TOMMS parameter name to its STOMMS name (with a deprecation warning).
   * Names without a TOMMS equivalent are returned unchanged.
   * @param name: name of the tomms legacy parameter.
   * @return name of the parameter in STOMMS.
   */
  std::string translateParameterName(const std::string& name);

  /** 
   * Handle TOMMS options and parameters not used by STOMMS. Reads the value from
   * the input. Returns true if the parameter was handled here.
   * @param name: name of the tomms legacy parameter.
   * @param input: input file name.
   */
  bool handleLegacyParameter(const std::string& name, std::istream& input);

  /** 
   * Error for an option value not available in STOMMS. Prints the contact link and exits.
   * @param name: name of the tomms legacy parameter.
   * @param value: the value (option) not available.
   * @param details: details of the option with contact information.
   */
  void reportUnavailableOption(const std::string& name, const std::string& value,
                               const std::string& details);

  /** 
   * Warning for a parameter ignored by STOMMS (not supported, or a constant in STOMMS).
   * @param name: name of the tomms legacy parameter.
   * @param reason: reason its not used. Either using a default constant value or 
   * parameter is not needed anymore.
   */
  void reportUnusedParameter(const std::string& name, const std::string& reason);

  /** 
   * Warning for a parameter not available in STOMMS yet but planned for the future.
   * @param name: name of the tomms legacy parameter.
   */
  void reportPlannedParameter(const std::string& name);
}

#endif
