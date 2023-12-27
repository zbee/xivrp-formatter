// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

namespace settings {

enum log_type {
  smartFind = 0, // Automatically find logs to use;
                 // finding your XIVLauncher config, then finding your logs
                 // depending on configs present.
                 // Then grabbing the most recent logs from supported services

  smart = 1,     // Automatically detect the log type from a nearby log
  chatscan = 2,  // ChatScanner log
  discord = 3,   // Discord log
  chat2 = 4,     // Chat2 database
  gobchat = 5,   // GobChat log
  xivlogger = 6, // XIVLogger log
};

enum images_location {
  smartLocate = 0, // Automatically find images to use;
                   // attempting to find your reshade settings then your reshade
                   // pictures, then the same with gshade, then FF screenshots

  nearby = 1,      // Look for nearby images
  reshade = 2,     // Look for reshade images
  gshade = 3,      // Look for gshade images
  screenshots = 4, // Look for screenshots
};

// TODO: Add saving of these settings to a file, and an input to use that file

struct structure {
  // The message log to use
  std::string log_file_path{"../examples/ChatLogs.json"};
  // The type of log file
  log_type log_file_type{log_type::smartFind};
  // The template file to use
  std::string template_file_path{"../template.html"};
  // The file to output to
  std::string output_file_path{"../formatted_writing.html"};

  // Whether OOC messages should be removed
  bool remove_out_of_character{true};

  // Whether ~emphatics~ should be highlighted
  bool highlight_emphatics{true};
  // The color that emphatics should be highlighted with
  std::string emphatic_highlight_color{"#DD9FC1"};

  // Whether continued messages should be combined
  bool combine_messages{true};

  // Whether multiple logs (including from other services) should be combined
  // and de-duplicated
  bool combine_logs{true};

  // Whether related images should be checked for, and inserted
  bool find_related_images{true};
  // Where to look for related images
  images_location related_images_location{images_location::smartLocate};

  // Whether timestamps should be included in the output reading
  bool want_timestamps{true};
  // Whether gaps in timestamps should be filled
  bool squash_time_gaps{true};

  // Whether the program should print debug information
  bool debug{false};
};

class loader {
public:
  // The settings the user chose
  structure settings;
  // Whether the settings were verified
  bool log_verified{false};
  bool template_verified{false};

  // Constructor - retrieves the user's settings then verifies them
  loader(bool use_default_settings = false);

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
  structure get_settings(bool use_default_settings = false);

  // Method to verify the log file
  [[nodiscard]] bool verify_log_file() const;

  // Method to verify the template file
  [[nodiscard]] bool verify_template_file() const;
};
} // namespace settings

#endif // SETTINGS_H
