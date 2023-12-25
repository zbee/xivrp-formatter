/*
 * XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
 * Licensed under GPLv3 - Refer to the LICENSE file for the complete text
 */

#include "settings.h"
#include "json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using json = nlohmann::json;

settings::loader::loader() {
  // Get the user's settings
  this->settings = this->get_settings();

  // Verify the user's settings
  this->log_verified = this->verify_log_file();
  this->template_verified = this->verify_template_file();
}

std::string settings::loader::get_real_path(std::string &path) {
  // Check if the path is empty
  if (path.empty())
    return "";

  // Check if the file path exists
  if (std::filesystem::exists(path))
    return path;

  // Check if variations on the path exist
  if (std::filesystem::exists(".." + path))
    return ".." + path;
  if (std::filesystem::exists("../" + path))
    return "../" + path;
  if (std::filesystem::exists("../examples/" + path))
    return "../examples/" + path;

  // If none of these exist
  return "";
}

bool settings::loader::check_file_exists(std::string file) {
  // Check if the file is a default
  if (file == "d" || file == "ad")
    return true;

  // Check if the file is empty
  if (file.empty())
    return false;

  // Check the file exists, or any variations of it
  return !settings::loader::get_real_path(file).empty();
}

bool settings::loader::check_file_format(std::string file, std::string format) {
  // Check if the file is a default
  if (file == "d" || file == "ad")
    return true;

  // Check if the file is empty
  if (file.empty())
    return false;

  // Check if the file name is shorter than the required format
  if (file.length() < format.length())
    return false;

  // Check the file extension is the correct format
  std::string file_type = file.substr(file.length() - format.length());

  return file_type == format;
}

bool settings::loader::check_hex_color(std::string color) {
  // Pattern for a hex color, 6 or 3 characters long
  std::regex hex_color_pattern{"^#([a-fA-F0-9]{6}|[a-fA-F0-9]{3})$"};

  // Check that the color is a valid hex color string
  bool matches = std::regex_match(color, hex_color_pattern);
  bool matches_with_hash = std::regex_match("#" + color, hex_color_pattern);

  // Return true if it's a valid hex color, or is with a hash sign added
  return matches || matches_with_hash;
}

bool settings::loader::verify_log_file() const {
  // Double check that the log file exists
  if (!settings::loader::check_file_exists(this->settings.log_file_path))
    return false;
  // Double check that the log file is json
  if (!settings::loader::check_file_format(this->settings.log_file_path,
                                           ".json"))
    return false;

  std::string line;
  // Open the template file
  std::ifstream infile(this->settings.log_file_path);

  bool contains_bracket = false;
  bool contains_colon = false;
  bool contains_comma = false;
  bool contains_double_quote = false;
  bool contains_curly_bracket = false;
  bool contains_chat_type = false;

  // Check if lines contain {, [, ", :, and ,, and ChatType
  while (std::getline(infile, line)) {
    // Check if this line contains a bracket
    contains_bracket |= line.find('[') != std::string::npos;
    contains_colon |= line.find(':') != std::string::npos;
    contains_comma |= line.find(',') != std::string::npos;
    contains_double_quote |= line.find('"') != std::string::npos;
    contains_curly_bracket |= line.find('{') != std::string::npos;
    contains_chat_type |= line.find("ChatType") != std::string::npos;

    // If all symbols are found, return true
    if (contains_bracket && contains_curly_bracket && contains_double_quote &&
        contains_colon && contains_comma && contains_chat_type)
      return true;
  }

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

bool settings::loader::verify_template_file() const {
  // TODO: Once templating is implemented, this should be changed to check if
  //  each required template placeholder is present

  // Double check that the template file exists
  if (!this->check_file_exists(this->settings.template_file_path))
    return false;
  // Double check that the template file is html
  if (!this->check_file_format(this->settings.template_file_path, ".html"))
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

settings::structure settings::loader::get_settings() {
  //<editor-fold desc="Message Log (and mass default)">
  // Inform of mass defaults
  std::cout << "(enter 'ad' to use all default values)" << std::endl;
  std::string setting_input;

  // Keep asking until a valid answer is given
  while (!this->check_file_exists(setting_input) ||
         !this->check_file_format(setting_input, ".json")) {

    if (!setting_input.empty()) {
      // Inform of error, request again
      std::cout << "!!! Invalid file path or file type, "
                   "please try again: "
                << std::endl
                << ">";
    } else {
      // Request log file setting
      std::cout << "Path to message log file you would like formatted: "
                   "(enter 'd' for default: included example log)"
                << std::endl
                << ">";
    }
    std::cin >> setting_input;
  }

  // Skip all inputs if all defaults is requested
  if (setting_input == "ad")
    return settings;

  // Set log file setting, if not default
  if (setting_input != "d") {
    settings.log_file_path = settings::loader::get_real_path(setting_input);
  }
  //</editor-fold>

  //<editor-fold desc="Template File">
  setting_input = "";

  // Keep asking until a valid answer is given
  while (!this->check_file_exists(setting_input) ||
         !this->check_file_format(setting_input, ".html")) {
    if (!setting_input.empty()) {
      // Inform of error, request again
      std::cout << "!!! Invalid file path or file type, "
                   "please try again: "
                << std::endl
                << ">";
    } else {
      std::cout << "Path to template file you would like used in formatting: "
                   "(enter 'd' for default: included template file)"
                << std::endl
                << ">";
    }
    std::cin >> setting_input;
  }
  if (setting_input != "d") {
    settings.template_file_path =
        settings::loader::get_real_path(setting_input);
  }
  //</editor-fold>

  //<editor-fold desc="Output File">
  setting_input = "";

  // Keep asking until a valid answer is given
  while (!this->check_file_format(setting_input, ".html")) {
    if (!setting_input.empty()) {
      // Inform of error, request again
      std::cout << "!!! Invalid file path or file type, "
                   "please try again: "
                << std::endl
                << ">";
    } else {
      std::cout << "Path to output file: "
                   "(enter 'd' for default: ./formatted_writing.html)"
                << std::endl
                << ">";
    }
    std::cin >> setting_input;
  }
  if (setting_input != "d") {
    settings.output_file_path = setting_input;
  }
  //</editor-fold>

  //<editor-fold desc="OOC Messages">
  setting_input = "";

  // Keep asking until a valid answer is given
  while (setting_input != "y" && setting_input != "n" && setting_input != "d") {
    if (!setting_input.empty()) {
      // Inform of error, request again
      std::cout << "!!! y (yes) or n (no) are the only valid answers, "
                   "please try again: "
                << std::endl
                << ">";
    } else {
      std::cout << "Should out-of-character (OOC) messages be removed? (y/n) "
                   "(enter 'd' for default: y)"
                << std::endl
                << ">";
    }
    std::cin >> setting_input;
  }
  if (setting_input != "d") {
    settings.remove_out_of_character = setting_input == "y";
  }
  //</editor-fold>

  //<editor-fold desc="Highlight Emphatics">
  setting_input = "";

  // Keep asking until a valid answer is given
  while (setting_input != "y" && setting_input != "n" && setting_input != "d") {
    if (!setting_input.empty()) {
      // Inform of error, request again
      std::cout << "!!! y (yes) or n (no) are the only valid answers, "
                   "please try again: "
                << std::endl
                << ">";
    } else {
      std::cout
          << "Should ~emphatic~ and emphatic~ phrases be highlighted? (y/n) "
             "(enter 'd' for default: y)"
          << std::endl
          << ">";
    }
    std::cin >> setting_input;
  }
  if (setting_input != "d") {
    settings.highlight_emphatics = setting_input == "y";
  }
  //</editor-fold>

  //<editor-fold desc="Highlight Color">
  setting_input = "";

  if (settings.highlight_emphatics) {
    // Keep asking until a valid answer is given
    while (!this->check_hex_color(setting_input) && setting_input != "d") {
      if (!setting_input.empty()) {
        // Inform of error, request again
        std::cout << "!!! Value was not a valid hexadecimal color code, "
                     "please try again: "
                  << std::endl
                  << ">";
      } else {
        std::cout << "Color emphatics should be highlighted with: "
                     "(enter 'd' for default: #DD9FC1)"
                  << std::endl
                  << ">";
      }
      std::cin >> setting_input;
    }
    if (setting_input != "d") {
      settings.emphatic_highlight_color = setting_input;
    }
  }
  //</editor-fold>

  //<editor-fold desc="Message Combining">
  setting_input = "";

  // Keep asking until a valid answer is given
  while (setting_input != "y" && setting_input != "n" && setting_input != "d") {
    if (!setting_input.empty()) {
      // Inform of error, request again
      std::cout << "!!! y (yes) or n (no) are the only valid answers, "
                   "please try again: "
                << std::endl
                << ">";
    } else {
      std::cout << "Should messages be combined? (y/n) "
                   "(enter 'd' for default: y)"
                << std::endl
                << ">";
    }
    std::cin >> setting_input;
  }
  if (setting_input != "d") {
    settings.combine_messages = setting_input == "y";
  }
  //</editor-fold>

  return settings;
}
