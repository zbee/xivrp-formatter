// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "settings.h"
#include "../common/utilities.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <string>

using json = nlohmann::json;

/**
 * @brief Constructor for settings::loader, to load settings from all sources
 * and verify them. To then use settings, access settings::loader.settings
 * @param arg_count The number of arguments
 * @param arguments The arguments
 */
settings::loader::loader(int arg_count, char *arguments[]) {
  // Load settings from arguments
  bool use_default_settings = false;
  json settings_from_arguments;
  std::tie(use_default_settings, settings_from_arguments) =
      settings::loader::check_arguments(arg_count, arguments);

  // Get the user's settings
  if (!use_default_settings)
    this->settings = settings::loader::get_settings(settings_from_arguments);

  // Verify the user's settings
  this->log_verified = this->verify_log_file();
  this->template_verified = this->verify_template_file();
}

/**
 * @brief Checks the arguments for settings, and returns them
 * @param arg_count The number of arguments
 * @param arguments The arguments
 * @return A tuple containing whether all defaults should be used, and the
 * settings from the arguments
 */
std::tuple<bool, json> settings::loader::check_arguments(int arg_count,
                                                         char **arguments) {
  json settings_raw;

  // Check for the flag to use all defaults
  bool all_defaults_flag = false;
  for (int i = 0; i < arg_count; ++i)
    if (std::string(arguments[i]) == "--all-defaults")
      all_defaults_flag = true;

  // Check for the flag to use all defaults
  for (int i = 0; i < arg_count; ++i)
    if (std::string(arguments[i]) == "--debug")
      settings_raw["debug"] = true;

  // Check other arguments for settings
  for (int i = 0; i < arg_count; ++i) {
    auto argument = std::string(arguments[i]);

    //<editor-fold desc="Skipping non-setting arguments">
    // Skip if argument does not begin with double dashes
    if (argument.substr(0, 2) != "--")
      continue;

    // Skip if argument does not have a value
    if (argument.find('=') == std::string::npos)
      continue;
    //</editor-fold>

    // Get the argument name
    auto argument_name = argument.substr(2, argument.find('=') - 2);

    // Get the argument value
    auto argument_value = argument.substr(argument.find('=') + 1);

    // Add the argument to the settings
    settings_raw[argument_name] = argument_value;
  }

  return std::make_tuple(all_defaults_flag, settings_raw);
}

/**
 * @brief Checks if the log file is valid
 * @return Whether the log file is valid
 * @todo Switch to using log_sources verification
 */
bool settings::loader::verify_log_file() const {
  // Double check that the log file exists
  if (!common::utilities::check_file_exists(this->settings.log_file_path))
    return false;
  // Double check that the log file is json
  if (!common::utilities::check_file_format(this->settings.log_file_path,
                                            ".json"))
    return false;

  try {
    // Try to load the message log file, and parse it as json
    std::ifstream log_file(this->settings.log_file_path);
    json log_json = json::parse(log_file);
    // Return true if the log file was parsed
    return true;
  } catch (json::parse_error &e) {
    // If the log file is not valid json, return false
    return false;
  }
}

/**
 * @brief Checks if the template file is valid
 * @return Whether the template file is valid
 * @todo Implement templating and do deeper verification
 */
bool settings::loader::verify_template_file() const {
  // Double check that the template file exists
  if (!common::utilities::check_file_exists(this->settings.template_file_path))
    return false;
  // Double check that the template file is html
  if (!common::utilities::check_file_format(this->settings.template_file_path,
                                            ".html"))
    return false;

  std::string line;
  // Open the template file
  std::ifstream infile(this->settings.template_file_path);

  // Check if any line in the file contains a template opener and closer
  while (std::getline(infile, line)) {
    // Check if this line contains a template opener and closer
    bool line_contains_template_opener = line.find("{{") != std::string::npos;
    bool line_contains_template_closer = line.find("}}") != std::string::npos;

    // If both are found, return true
    if (line_contains_template_opener && line_contains_template_closer)
      return true;
  }

  // If no line contains both, return false
  return false;
}

/**
 * @brief Builds the defaults from program or save file, skips argument-given
 * settings, then asks the user for the rest
 * @param settings_from_arguments The settings provided by the save value, from
 * settings::structure::load_settings()
 * @return The user's settings structure
 */
settings::structure
settings::loader::get_settings(const json &settings_from_arguments) {
  auto working_settings = settings::structure();
  std::map<std::string, std::pair<default_source, std::string>> defaults;

  // Loop over each setting in the guide to form the defaults
  for (auto &setting_raw : working_settings.settings_guide.items()) {
    auto setting = setting_raw.value();

    // Get basic settings data
    std::string setting_name = setting["identifier"];
    default_source default_source;
    std::string default_value;
    std::tie(default_source, default_value) =
        settings::structure::get_default(setting_name);

    // Save the default and default source
    defaults[setting_name] = std::make_pair(default_source, default_value);
  }

  // Remove questions that are already set via arguments
  auto local_settings_guide = working_settings.settings_guide;
  for (auto &item : settings_from_arguments.items()) {
    std::string key = item.key();
    for (auto &setting : local_settings_guide)
      if (setting["identifier"] == key) {
        local_settings_guide.erase(setting);
        break;
      }
  }

  // Ask the questions
  settings::ask ask(local_settings_guide, defaults);

  // Save the answers
  for (auto &answer : ask.answers) {
    auto setting_name = answer.first;
    auto setting_defaulted = answer.second.first;
    auto setting_answer = answer.second.second;

    // Set the setting
    if (!setting_defaulted)
      working_settings.set_setting(setting_name, setting_answer);
  }

  working_settings.save_settings();

  return working_settings;
}

/**
 * @brief Get the default value for a setting, and whether it is program-default
 * or from saved settings
 * @param setting The setting identifier to get the default value for
 * @return A tuple containing the source of the default, and the default value
 */
std::tuple<settings::default_source, std::string>
settings::structure::get_default(const std::string &setting) {
  std::string default_value;
  default_source default_is_from_saved_settings =
      default_source::default_answer;

  // Get the default settings, including saved settings
  auto default_settings = settings::structure();
  auto saved_settings = default_settings.load_settings();

  // Check if the setting is in the saved settings
  for (auto &saved_setting : saved_settings.items())
    if (saved_setting.key() == setting) {
      default_is_from_saved_settings = default_source::settings;
      break;
    }

  // Get the default value
  if (default_settings.settings.contains(setting))
    default_value = default_settings.settings[setting];
  else
    throw std::invalid_argument("Unknown setting: " + setting +
                                ". Please request the developer update this "
                                "entry.");

  // Return the default value and whether it is from saved settings
  return std::make_tuple(default_is_from_saved_settings, default_value);
}

/**
 * @brief Set a setting, and save it to the working json for saving
 * @param setting The setting identifier to set
 * @param value The value to set the setting to
 * @param skip_json_save Whether to skip saving the setting to the working json
 * @return Whether the setting was set successfully in the structure, or only
 * the json
 */
bool settings::structure::set_setting(const std::string &setting,
                                      const std::string &value,
                                      bool skip_json_save) {
  // Save basic casts
  auto const string_value = std::any_cast<std::string>(value);
  bool bool_value = string_value == "yes";
  int int_value;
  if (std::isdigit(string_value[0]))
    int_value = std::stoi(string_value);

  // Save the setting to the working json and map
  this->working_json[setting] = string_value;
  this->settings[setting] = string_value;

  // Don't run the gauntlet if the setting is not in the settings map
  if (!this->settings.contains(setting))
    return false;

  if (setting == "log_file_path")
    this->log_file_path = string_value;
  else if (setting == "log_file_type") {
    auto save_value = static_cast<log_type>(int_value);
    this->log_file_type = save_value;
  } else if (setting == "template_file_path")
    this->template_file_path = string_value;
  else if (setting == "output_file_path")
    this->output_file_path = string_value;
  else if (setting == "remove_out_of_character")
    this->remove_out_of_character = bool_value;
  else if (setting == "highlight_emphatics")
    this->highlight_emphatics = bool_value;
  else if (setting == "emphatic_highlight_color")
    this->emphatic_highlight_color = string_value;
  else if (setting == "combine_messages")
    this->combine_messages = bool_value;
  else if (setting == "combine_logs")
    this->combine_logs = bool_value;
  else if (setting == "find_related_images")
    this->find_related_images = bool_value;
  else if (setting == "related_images_location") {
    auto save_value = static_cast<images_location>(int_value);
    this->related_images_location = save_value;
  } else if (setting == "want_timestamps")
    this->want_timestamps = bool_value;
  else if (setting == "squash_time_gaps")
    this->squash_time_gaps = bool_value;
  else if (setting == "debug")
    this->debug = bool_value;
  else
    // Failed to find the setting
    return false;

  // Successfully set the setting
  return true;
}

/**
 * @brief Save the working json to the save file
 */
void settings::structure::save_settings() const {
  // If the working json is empty, don't save
  if (this->working_json.empty())
    return;

  // Write the working json to the save file
  std::ofstream save_file(this->settings_file_path);
  save_file << this->working_json.dump(4);
  save_file.close();
}

/**
 * @brief Load the settings from the save file
 * @return The loaded settings
 */
json settings::structure::load_settings() {
  // If file doesn't exist, return an empty json
  if (!common::utilities::check_file_exists(this->settings_file_path))
    return {};

  // Open and parse the save file
  auto file = common::utilities::get_real_path(this->settings_file_path);
  std::ifstream save_file(file);
  json saved_json;
  save_file >> saved_json;
  save_file.close();

  // Set the settings from the save file
  for (auto &setting : saved_json.items()) {
    const auto &setting_name = setting.key();
    auto setting_value = setting.value().get<std::string>();

    this->set_setting(setting_name, setting_value, true);
  }

  // Return the loaded settings, so it can be checked against for whether a
  // setting is default
  return saved_json;
}
