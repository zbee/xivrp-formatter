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

/**
 * @brief How to look for logs to read
 * @see settings::structure::log_file_type
 * @see settings::structure::log_file_path
 * @see messages::load::load()
 */
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

/**
 * @brief How to look for related images
 * @see settings::structure::related_images_location
 * @see related_images::related_images()
 */
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
  /**
   * @brief The path to the settings file
   */
  std::string settings_file_path{"settings.json"};

  //<editor-fold desc="Actual Settings">
  /**
   * @brief The path to the log file
   * @see settings::structure::log_file_type
   */
  std::string log_file_path{"ChatLogs.json"};
  /**
   * @brief The type of log file to use. Currently unused, will be put to use
   * @see settings::structure::log_file_path
   */
  log_type log_file_type{log_type::smartFind};
  /**
   * @brief The path to the template file
   */
  std::string template_file_path{"template.html"};
  /**
   * @brief The path to the output file
   */
  std::string output_file_path{"formatted_writing.html"};

  /**
   * @brief Whether out of character messages should be removed
   */
  bool remove_out_of_character{true};

  /**
   * @brief Whether emphatics should be highlighted
   * @see settings::structure::emphatic_highlight_color
   */
  bool highlight_emphatics{true};
  /**
   * @brief The color that emphatics should be highlighted with
   * @see settings::structure::highlight_emphatics
   */
  std::string emphatic_highlight_color{"#DD9FC1"};

  /**
   * @brief Whether messages that are continuations of others should be combined
   */
  bool combine_messages{true};

  /**
   * @brief Whether multiple logs should be combined and de-duplicated (from all
   * selected services)
   */
  bool combine_logs{true};

  /**
   * @brief Whether related images should be checked for, and inserted
   * @see settings::structure::related_images_location
   */
  bool find_related_images{true};
  /**
   * @brief Where to look for related images
   * @see settings::structure::find_related_images
   */
  images_location related_images_location{images_location::smartLocate};

  /**
   * @brief Whether timestamps should be included in the output
   */
  bool want_timestamps{true};
  /**
   * @brief Whether gaps in timestamps should be filled
   */
  bool squash_time_gaps{true};

  /**
   * @brief Whether the program should print debug information
   */
  bool debug{false};

  /**
   * @brief The working json, for saving settings
   * @see settings::structure::save_settings()
   */
  json working_json;
  //</editor-fold>

  /**
   * @brief A map of this structure's settings, for less repetition of the whole
   * list of settings
   * @see settings::structure::get_default()
   * @see settings::structure::set_setting()
   */
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

  /**
   * @brief Get the default value for a setting, and whether it is
   * program-default or from saved settings
   * @param setting The setting identifier to get the default value for
   * @return A tuple containing the source of the default, and the default value
   * @see settings::default_source
   */
  [[nodiscard]] static std::tuple<settings::default_source, std::string>
  get_default(const std::string &setting);

  /**
   * @brief Set a setting, and save it to the working json for saving
   * @param setting The setting identifier to set
   * @param value The value to set the setting to
   * @param skip_json_save Whether to skip saving the setting to the working
   * json
   * @return Whether the setting was set successfully in the structure
   */
  bool set_setting(const std::string &setting, const std::string &value,
                   bool skip_json_save = false);

  /**
   * @brief Load the settings from the save file
   * @return The loaded settings
   * @see settings::structure::settings_file_path
   */
  json load_settings();

  /**
   * @brief Save the working json to the save file
   * @see settings::structure::working_json
   * @see settings::structure::settings_file_path
   */
  void save_settings() const;

  /**
   * @brief This is the format given to ask to ask the usr for all settings for
   * the program This is JSON's first-class type despite it's ugliness as
   * otherwise it's impossible to use enum values in an even remotely clean
   * manner
   * @see settings::answer_types
   * @see settings::compare
   * @see settings::loader::get_settings, this is where this is employed
   */
  //<editor-fold desc="Settings Requesting">
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
  /**
   * @brief The settings the user chose
   */
  structure settings;
  /**
   * @brief Whether the log file was verified
   */
  [[maybe_unused]] bool log_verified{false};
  /**
   * @brief Whether the template file was verified
   */
  [[maybe_unused]] bool template_verified{false};

  /**
   * @brief Constructor, to load settings from all sources and verify them
   * @param arg_count The number of arguments
   * @param arguments The arguments
   * @see settings::loader::settings
   */
  explicit loader(int arg_count, char *arguments[]);

private:
  /**
   * @brief Checks the arguments for settings, and returns them
   * @param arg_count The number of arguments
   * @param arguments The arguments
   * @return A tuple containing whether all defaults should be used, and the
   * settings from the arguments
   */
  [[nodiscard]] static std::tuple<bool, json>
  check_arguments(int arg_count, char *arguments[]);

  /**
   * @brief Builds the defaults from program or save file, skips argument-given
   * settings, then asks the user for the rest
   * @param settings_from_arguments The settings provided by the save value,
   * from settings::structure::load_settings()
   * @return The user's settings structure
   */
  [[nodiscard]] static structure
  get_settings(const json &settings_from_arguments);

  /**
   * @brief Checks if the log file is valid
   * @return Whether the log file is valid
   * @todo Switch to using log_sources verification
   */
  [[nodiscard]] bool verify_log_file() const;

  /**
   * @brief Checks if the template file is valid
   * @return Whether the template file is valid
   * @todo Implement templating and do deeper verification
   */
  [[nodiscard]] bool verify_template_file() const;
};
} // namespace settings

#endif // SETTINGS_H
