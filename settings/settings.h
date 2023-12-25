// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef CAPSTONE_LION_SETTINGS_H
#define CAPSTONE_LION_SETTINGS_H

#include <string>

namespace settings {
struct structure {
  // The message log to use
  std::string log_file_path{"../examples/ChatLogs.json"};
  // The template file to use
  std::string template_file_path{"../template.html"};
  // The file to output to
  std::string output_file_path{"../formatted_writing.html"};
  // Whether OOC messages should be removed
  bool remove_out_of_character{true};
  // Whether ~emphatics~ should be highlighted
  bool highlight_emphatics{true};
  // Whether continued messages should be combined
  bool combine_messages{true};
  // The color that emphatics should be highlighted with
  std::string emphatic_highlight_color{"#DD9FC1"};
};

class loader {
public:
  // The settings the user chose
  structure settings;
  // Whether the settings were verified
  bool log_verified{false};
  bool template_verified{false};

  // Constructor - retrieves the user's settings then verifies them
  loader();

  // Method to check a few other paths for the given file
  [[nodiscard]] static std::string get_real_path(std::string &path);

  // Method to check if a file exists
  [[nodiscard]] static bool check_file_exists(std::string file);

  // Method to check if a file has the right extension
  [[nodiscard]] static bool check_file_format(std::string file,
                                              std::string format);

  // Method to check if a string is a valid hex color
  [[nodiscard]] static bool check_hex_color(std::string color);

private:
  // Method to get the user's settings
  structure get_settings();

  // Method to verify the log file
  [[nodiscard]] bool verify_log_file() const;

  // Method to verify the template file
  [[nodiscard]] bool verify_template_file() const;
};
} // namespace settings

#endif // CAPSTONE_LION_SETTINGS_H
