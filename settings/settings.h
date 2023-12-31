// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef SETTINGS_H
#define SETTINGS_H

#include "../common/utilities.h"
#include "../includes/json.hpp"
#include "ask.h"
#include <string>

using json = nlohmann::json;

namespace settings {

enum log_type {
  smartFind = 0, // Automatically find logs to use;
                 // finding your XIVLauncher config, then finding your logs
                 // depending on configs present.
                 // Then grabbing the most recent logs from supported services
  smart = 1,     // Automatically detect the log type from a nearby log

  chatscan = 2,  // Specific ChatScanner log
  discord = 3,   // Specific Discord log
  chat2 = 4,     // Specific Chat2 database
  gobchat = 5,   // Specific GobChat log
  xivlogger = 6, // Specific XIVLogger log
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

struct structure {
  std::string settings_file_path{"settings.json"};

  //<editor-fold desc="Actual Settings">
  // The message log to use
  std::string log_file_path{"ChatLogs.json"};
  // TODO: Pass in WorkingDirectory so this works in debug or from the binary
  // The type of log file
  log_type log_file_type{log_type::smartFind};
  // The template file to use
  std::string template_file_path{"template.html"};
  // The file to output to
  std::string output_file_path{"formatted_writing.html"};

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

  // The JSON for the save file
  json working_json;
  //</editor-fold>

  //<editor-fold desc="Map of Settings">
  std::map<std::string, std::string> settings = {
      {"log_file_path", log_file_path},
      {"log_file_type", std::to_string(log_file_type)},
      {"template_file_path",
       common::utilities::get_real_path(template_file_path)},
      {"output_file_path", output_file_path},
      {"remove_out_of_character", remove_out_of_character ? "yes" : "no"},
      {"highlight_emphatics", highlight_emphatics ? "yes" : "no"},
      {"emphatic_highlight_color", emphatic_highlight_color},
      {"combine_messages", combine_messages ? "yes" : "no"},
      {"combine_logs", combine_logs ? "yes" : "no"},
      {"find_related_images", find_related_images ? "yes" : "no"},
      {"related_images_location", std::to_string(related_images_location)},
      {"want_timestamps", want_timestamps ? "yes" : "no"},
      {"squash_time_gaps", squash_time_gaps ? "yes" : "no"},
      {"debug", debug ? "yes" : "no"},
  };
  //</editor-fold>

  // Method to get the default value for a setting
  [[nodiscard]] static std::tuple<settings::default_source, std::string>
  get_default(const std::string &setting);

  // Method to set a setting
  bool set_setting(const std::string &setting, const std::string &value,
                   bool skip_json_save = false);

  // Method to load the settings from a file
  json load_settings();

  // Method to save the settings to a file
  void save_settings() const;

  //<editor-fold desc="Settings Requesting">
  // This is JSON's first-class type despite it's ugliness as otherwise it's
  // impossible to use enum values in an even remotely clean manner
  json settings_guide = {
      //<editor-fold desc="log_file_type">
      {{"identifier", "log_file_type"},
       {"question", "How should the log file be found?"},
       {"wants", answer_types::option},
       {"options",
        {
            {
                {"name", "Automatically find logs to use"},
                {"value", log_type::smartFind},
            },
            {
                {"name", "Automatically find a nearby log"},
                {"value", log_type::smart},
            },
            {
                {"name", "Specify a ChatScanner log"},
                {"value", log_type::chatscan},
            },
            {
                {"name", "Specify a Discord log"},
                {"value", log_type::discord},
            },
            {
                {"name", "Specify a Chat2 database"},
                {"value", log_type::chat2},
            },
            {
                {"name", "Specify a GobChat log"},
                {"value", log_type::gobchat},
            },
            {
                {"name", "Specify a XIVLogger log"},
                {"value", log_type::xivlogger},
            },
        }}},
      //</editor-fold>
      //<editor-fold desc="log_file_path">
      {{"identifier", "log_file_path"},
       {"question", "Where is the log file located?"},
       {"wants", answer_types::path},
       {"requires",
        {
            {
                {"identifier", "log_file_type"},
                {"comparison", compare::not_any},
                {"value",
                 {
                     log_type::smartFind,
                     log_type::smart,
                 }},
            },
        }}},
      //</editor-fold>
      {{"identifier", "template_file_path"},
       {"question", "Where is the template file you would like to use?"},
       {"wants", answer_types::path}},
      {{"identifier", "output_file_path"},
       {"question", "Where should the output file be saved?"},
       {"wants", answer_types::string}},
      {{"identifier", "remove_out_of_character"},
       {"question", "Should out of character messages be removed?"},
       {"wants", answer_types::yesno}},
      {{"identifier", "highlight_emphatics"},
       {"question", "Should emphatics be highlighted?"},
       {"wants", answer_types::yesno}},
      //<editor-fold desc="emphatic_highlight_color">
      {{"identifier", "emphatic_highlight_color"},
       {"question", "What color should emphatics be highlighted with?"},
       {"wants", answer_types::hex_color},
       {"requires",
        {
            {
                {"identifier", "highlight_emphatics"},
                {"comparison", compare::is},
                {"value", answer::yes},
            },
        }}},
      //</editor-fold>
      {{"identifier", "combine_messages"},
       {"question",
        "Should messages that are continuations of others be combined?"},
       {"wants", answer_types::yesno}},
      {{"identifier", "combine_logs"},
       {"question", "Should multiple logs be combined and de-duplicated?"},
       {"wants", answer_types::yesno}},
      {{"identifier", "find_related_images"},
       {"question", "Should related images be found and inserted?"},
       {"wants", answer_types::yesno}},
      //<editor-fold desc="related_images_location">
      {{"identifier", "related_images_location"},
       {"question", "Where should related images be looked for?"},
       {"wants", answer_types::option},
       {"options",
        {
            {
                {"name", "Automatically find images to use"},
                {"value", images_location::smartLocate},
            },
            {
                {"name", "Look for nearby images"},
                {"value", images_location::nearby},
            },
            {
                {"name", "Look for reshade images"},
                {"value", images_location::reshade},
            },
            {
                {"name", "Look for gshade images"},
                {"value", images_location::gshade},
            },
            {
                {"name", "Look for screenshots"},
                {"value", images_location::screenshots},
            },
        }},
       {"requires",
        {
            {
                {"identifier", "find_related_images"},
                {"comparison", compare::is},
                {"value", answer::yes},
            },
        }}},
      //</editor-fold>
      {{"identifier", "want_timestamps"},
       {"question",
        "Should timestamps be included for messages in the output?"},
       {"wants", answer_types::yesno}},
      {{"identifier", "squash_time_gaps"},
       {"question", "Should gaps in timestamps be filled?"},
       {"wants", answer_types::yesno}},
      {{"identifier", "debug"}, {"wants", answer_types::yesno}},
  };
  //</editor-fold>
};

class loader {
public:
  // The settings the user chose
  structure settings;
  // Whether the settings were verified
  [[maybe_unused]] bool log_verified{false};
  [[maybe_unused]] bool template_verified{false};

  // Constructor - retrieves the user's settings then verifies them
  explicit loader(int arg_count, char *arguments[]);

private:
  [[nodiscard]] static std::tuple<bool, json>
  check_arguments(int arg_count, char *arguments[]);

  // Method to get the user's settings
  [[nodiscard]] static structure
  get_settings(const json &settings_from_arguments);

  // Method to verify the log file
  [[nodiscard]] bool verify_log_file() const;

  // Method to verify the template file
  [[nodiscard]] bool verify_template_file() const;
};
} // namespace settings

#endif // SETTINGS_H
