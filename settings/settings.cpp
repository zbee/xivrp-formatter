// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

// TODO: after settings::structure map reduction, common utilities class, and
//  ask are implemented, reduce #includes and reduce Problems and Smells across
//  project

#include "settings.h"
#include "../common/utilities.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <string>

using json = nlohmann::json;

settings::loader::loader(int arg_count, char *arguments[]) {
  // Load settings from arguments
  bool use_default_settings = false;
  json settings_from_arguments;
  std::tie(use_default_settings, settings_from_arguments) =
      this->check_arguments(arg_count, arguments);

  // Get the user's settings
  if (!use_default_settings)
    this->settings = this->get_settings(settings_from_arguments);

  // Verify the user's settings
  this->log_verified = this->verify_log_file();
  this->template_verified = this->verify_template_file();
}

std::tuple<bool, json> settings::loader::check_arguments(int arg_count,
                                                         char **arguments) {
  json settings_raw;

  // Check for the flag to use all defaults
  bool all_defaults = false;
  for (int i = 0; i < arg_count; ++i)
    if (std::string(arguments[i]) == "--all-defaults")
      all_defaults = true;

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

  return std::make_tuple(all_defaults, settings_raw);
}

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

bool settings::loader::verify_template_file() const {
  // TODO: Once templating is implemented, this should be changed to check if
  //  each required template placeholder is present

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

settings::structure
settings::loader::get_settings(const json &settings_from_arguments) {
  // TODO: Break this apart into pieces in ask.h. Namely, so it can be used to
  //  ask impromptu questions, like about discovered cover images, but also just
  //  because it's 200 lines ...

  json working_settings;

  std::cout << "[enter 'AD' at any time to default all further settings]"
            << std::endl;

  // TODO: move this to ask::ask and ask::ask_questions
  // Loop over each setting in the guide
  bool use_all_defaults = false;
  for (auto &setting_raw : working_settings.items()) {
    auto setting = setting_raw.value();

    // TODO: move this to ask::load_question and a question structure
    // Get basic settings data
    bool ask = false;
    std::string question;
    std::string setting_name = setting["setting"];
    int wants = setting["wants"];
    bool default_is_from_saved_settings;
    std::string default_value;
    std::tie(default_is_from_saved_settings, default_value) =
        this->settings.get_default(setting_name);

    // Check for inquiry data
    if (setting.contains("ask")) {
      question = setting["ask"];
      ask = true;
    }

    // Don't ask if not requested
    if (!ask)
      continue;

    // Don't ask for settings that were provided in the arguments
    if (settings_from_arguments.contains(setting_name)) {
      this->settings.set_setting(setting_name,
                                 settings_from_arguments[setting_name]);
      continue;
    }

    // Don't ask for settings if all defaults was turned on
    if (use_all_defaults)
      continue;

    // Check for options data
    bool has_options = false;
    json options;
    if (setting.contains("options")) {
      has_options = true;
      options = setting["options"];
    }

    // Check for requirements
    bool has_requirements = false;
    json requirements;
    if (setting.contains("requires")) {
      has_requirements = true;
      requirements = setting["requires"];
    }

    // TODO: move this to ask::check_requirements
    // Check for requirements
    bool met_requirements = true;
    if (has_requirements) {
      for (auto &requirement : requirements.items()) {
        // Get the requirement's data
        std::string requirement_name = requirement.value()["setting"];
        int requirement_comparison = requirement.value()["comparison"];

        // Get the comparison type from enum
        auto comparison = static_cast<compare>(requirement_comparison);

        // Get the correlated setting value
        std::any correlated_setting_value =
            this->settings.get_setting(requirement_name);

        // If it's an any comparison
        if (requirement.value()["value"].is_array()) {
          std::list<int> requirement_value =
              requirement.value()["value"].get<std::list<int>>();
          auto correlated_setting_value_string =
              std::any_cast<int>(correlated_setting_value);

          // Check the requirement according to the comparison type, skipping if
          // the requirement is not met
          if (comparison == compare::is_any) {
            if (std::find(requirement_value.begin(), requirement_value.end(),
                          correlated_setting_value_string) ==
                requirement_value.end())
              met_requirements = false;
          } else if (comparison == compare::not_any) {
            if (std::find(requirement_value.begin(), requirement_value.end(),
                          correlated_setting_value_string) !=
                requirement_value.end())
              met_requirements = false;
          }

        }
        // If it's an is comparison
        else {
          // Cast the values to strings
          bool requirement_value = requirement.value()["value"].get<int>() == 1;
          auto correlated_setting_value_string =
              std::any_cast<bool>(correlated_setting_value);

          // Check the requirement according to the comparison type, skipping if
          // the requirement is not met
          if (comparison == compare::is) {
            if (correlated_setting_value_string != requirement_value)
              met_requirements = false;
          } else if (comparison == compare::is_not) {
            if (correlated_setting_value_string == requirement_value)
              met_requirements = false;
          }
        }
      }

      if (!met_requirements)
        continue;
    }

    // Get the user's answer, in a loop to verify their answer against what the
    // setting wants
    std::string answer;
    bool valid_answer = false;
    bool options_shown = false;

    // TODO: move this to ask::form_question
    // Ask the user the setting's question, and suggest saved or default values
    if (!default_is_from_saved_settings)
      std::cout << std::endl
                << question << " [press enter for default: " << default_value
                << "]";
    else
      std::cout << std::endl
                << question << " [press enter for saved: " << default_value
                << "]";

    // TODO: move this to ask::ask_questions
    // Loop until the user gives a valid answer
    while (!valid_answer) {
      // TODO: move this to ask::form_question and ask::load_options in
      // ask::load_question
      //<editor-fold desc="Options Handling">
      // If the setting has options, print them
      if (has_options && !options_shown) {
        std::cout << std::endl << "Options:" << std::endl;
        // List Options
        for (auto &option : options.items()) {
          auto real_option = option.value();
          std::cout << "  [" << real_option["value"] << "] for "
                    << real_option["name"].get<std::string>() << std::endl;
        }
        options_shown = true;
        std::cout << "> ";
      } else {
        if (wants == answer_types::yesno)
          std::cout << std::endl
                    << "  [y] for Yes" << std::endl
                    << "  [n] for No" << std::endl;
        std::cout << std::endl << "> ";
      }
      //</editor-fold>

      // Ask the user for their answer
      std::getline(std::cin, answer);

      // TODO: move this to ask::handle_answer
      // Make a lowercase version of the answer
      auto answer_lower = answer;
      std::transform(answer_lower.begin(), answer_lower.end(),
                     answer_lower.begin(), ::tolower);

      // Check for defaults
      if (answer.empty() or answer_lower == "ad") {
        if (answer_lower == "ad")
          use_all_defaults = true;
        break;
      }

      // Check if the answer is valid
      if (wants == answer_types::option) {
        auto int_answer = std::stoi(answer);

        // Check if the answer is a valid option
        for (auto &option : options.items()) {
          auto option_value = option.value()["value"].get<int>();
          if (option_value == int_answer) {
            valid_answer = true;
            break;
          }
        }
      } else if (wants == answer_types::path) {
        // Check if the answer is a valid path
        valid_answer = common::utilities::check_file_exists(answer);
      } else if (wants == answer_types::hex_color) {
        // Check if the answer is a valid hex_color color
        valid_answer = common::utilities::check_hex_color(answer);
      } else if (wants == answer_types::yesno) {
        // Check if the answer is a valid boolean
        valid_answer = answer_lower == "yes" || answer_lower == "y" ||
                       answer_lower == "no" || answer_lower == "n";
      }

      // If the answer is not valid, print an error message
      if (!valid_answer)
        std::cout << std::endl << "Invalid answer. Please try again.";
      else
        this->settings.set_setting(setting_name, answer);
    }
  }

  settings.save_settings();

  return settings;
}

// TODO: reduce this with a map in settings::structure
std::tuple<bool, std::string>
settings::structure::get_default(std::string setting) {
  std::string default_value;
  auto default_settings = settings::structure();
  auto saved_settings = default_settings.load_settings();
  bool default_is_from_saved_settings = false;

  // Check if the setting is in the saved settings
  for (auto &saved_setting : saved_settings.items())
    if (saved_setting.key() == setting) {
      default_is_from_saved_settings = true;
      break;
    }

  if (setting == "log_file_path")
    default_value = default_settings.log_file_path;
  else if (setting == "log_file_type")
    default_value = std::to_string((int)default_settings.log_file_type);
  else if (setting == "template_file_path")
    default_value = default_settings.template_file_path;
  else if (setting == "output_file_path")
    default_value = default_settings.output_file_path;
  else if (setting == "remove_out_of_character")
    default_value = default_settings.remove_out_of_character ? "yes" : "no";
  else if (setting == "highlight_emphatics")
    default_value = default_settings.highlight_emphatics ? "yes" : "no";
  else if (setting == "emphatic_highlight_color")
    default_value = default_settings.emphatic_highlight_color;
  else if (setting == "combine_messages")
    default_value = default_settings.combine_messages ? "yes" : "no";
  else if (setting == "combine_logs")
    default_value = default_settings.combine_logs ? "yes" : "no";
  else if (setting == "find_related_images")
    default_value = default_settings.find_related_images ? "yes" : "no";
  else if (setting == "related_images_location")
    default_value =
        std::to_string((int)default_settings.related_images_location);
  else if (setting == "want_timestamps")
    default_value = default_settings.want_timestamps ? "yes" : "no";
  else if (setting == "squash_time_gaps")
    default_value = default_settings.squash_time_gaps ? "yes" : "no";
  else if (setting == "debug")
    default_value = default_settings.debug ? "yes" : "no";
  else
    throw std::invalid_argument("Unknown setting: " + setting +
                                ". Please request the developer update this "
                                "entry.");

  return std::make_tuple(default_is_from_saved_settings, default_value);
}

// TODO: reduce this with a map in settings::structure
std::any settings::structure::get_setting(std::string setting) {
  if (setting == "log_file_path")
    return this->log_file_path;
  else if (setting == "log_file_type")
    return (int)this->log_file_type;
  else if (setting == "template_file_path")
    return this->template_file_path;
  else if (setting == "output_file_path")
    return this->output_file_path;
  else if (setting == "remove_out_of_character")
    return this->remove_out_of_character;
  else if (setting == "highlight_emphatics")
    return this->highlight_emphatics;
  else if (setting == "emphatic_highlight_color")
    return this->emphatic_highlight_color;
  else if (setting == "combine_messages")
    return this->combine_messages;
  else if (setting == "combine_logs")
    return this->combine_logs;
  else if (setting == "find_related_images")
    return this->find_related_images;
  else if (setting == "related_images_location")
    return (int)this->related_images_location;
  else if (setting == "want_timestamps")
    return this->want_timestamps;
  else if (setting == "squash_time_gaps")
    return this->squash_time_gaps;
  else if (setting == "debug")
    return this->debug;
  else
    throw std::invalid_argument("Unknown setting: " + setting +
                                ". Please request the developer update this "
                                "entry.");
}

// TODO: reduce this with a map in settings::structure
void settings::structure::set_setting(std::string setting, std::any value) {
  if (setting == "log_file_path") {
    auto save_value = std::any_cast<std::string>(value);
    this->working_json[setting] = save_value;
    this->log_file_path = save_value;
  } else if (setting == "log_file_type") {
    auto temp_value = std::any_cast<std::string>(value);
    auto int_value = std::stoi(temp_value);
    auto save_value = static_cast<log_type>(int_value);
    this->working_json[setting] = save_value;
    this->log_file_type = save_value;
  } else if (setting == "template_file_path") {
    auto save_value = std::any_cast<std::string>(value);
    this->working_json[setting] = save_value;
    this->template_file_path = save_value;
  } else if (setting == "output_file_path") {
    auto save_value = std::any_cast<std::string>(value);
    this->working_json[setting] = save_value;
    this->output_file_path = save_value;
  } else if (setting == "remove_out_of_character") {
    auto temp_value = std::any_cast<std::string>(value);
    auto save_value = temp_value == "yes" || temp_value == "y";
    this->working_json[setting] = save_value;
    this->remove_out_of_character = save_value;
  } else if (setting == "highlight_emphatics") {
    auto temp_value = std::any_cast<std::string>(value);
    auto save_value = temp_value == "yes" || temp_value == "y";
    this->working_json[setting] = save_value;
    this->highlight_emphatics = save_value;
  } else if (setting == "emphatic_highlight_color") {
    auto save_value = std::any_cast<std::string>(value);
    this->working_json[setting] = save_value;
    this->emphatic_highlight_color = save_value;
  } else if (setting == "combine_messages") {
    auto temp_value = std::any_cast<std::string>(value);
    auto save_value = temp_value == "yes" || temp_value == "y";
    this->working_json[setting] = save_value;
    this->combine_messages = save_value;
  } else if (setting == "combine_logs") {
    auto temp_value = std::any_cast<std::string>(value);
    auto save_value = temp_value == "yes" || temp_value == "y";
    this->working_json[setting] = save_value;
    this->combine_logs = save_value;
  } else if (setting == "find_related_images") {
    auto temp_value = std::any_cast<std::string>(value);
    auto save_value = temp_value == "yes" || temp_value == "y";
    this->working_json[setting] = save_value;
    this->find_related_images = save_value;
  } else if (setting == "related_images_location") {
    auto temp_value = std::any_cast<std::string>(value);
    auto int_value = std::stoi(temp_value);
    auto save_value = static_cast<images_location>(int_value);
    this->working_json[setting] = save_value;
    this->related_images_location = save_value;
  } else if (setting == "want_timestamps") {
    auto temp_value = std::any_cast<std::string>(value);
    auto save_value = temp_value == "yes" || temp_value == "y";
    this->working_json[setting] = save_value;
    this->want_timestamps = save_value;
  } else if (setting == "squash_time_gaps") {
    auto temp_value = std::any_cast<std::string>(value);
    auto save_value = temp_value == "yes" || temp_value == "y";
    this->working_json[setting] = save_value;
    this->squash_time_gaps = save_value;
  } else if (setting == "debug") {
    auto temp_value = std::any_cast<std::string>(value);
    auto save_value = temp_value == "yes" || temp_value == "y";
    this->working_json[setting] = save_value;
    this->debug = save_value;
  } else
    throw std::invalid_argument("Unknown setting: " + setting +
                                ". Please request the developer update this "
                                "entry.");
}

void settings::structure::save_settings() const {
  // If the working json is empty, don't save
  if (this->working_json.empty())
    return;

  // Write the working json to the save file
  std::ofstream save_file("../settings.json");
  save_file << this->working_json.dump();
  save_file.close();
}

json settings::structure::load_settings() {
  // If file doesn't exist, return an empty json
  if (!std::filesystem::exists("../settings.json"))
    return {};

  // Open and parse the save file
  std::ifstream save_file("../settings.json");
  json saved_json;
  save_file >> saved_json;
  save_file.close();

  // Set the settings from the save file
  for (auto &setting : saved_json.items()) {
    const auto &setting_name = setting.key();
    auto setting_value = setting.value();

    // Convert non-string values to strings how the set_settings method expects
    // them to be as answers to setting questions
    std::string real_setting_value;
    std::string setting_value_type = setting_value.type_name();
    if (setting_value_type == "number")
      real_setting_value = std::to_string(setting_value.get<int>());
    else if (setting_value_type == "boolean")
      real_setting_value = setting_value.get<bool>() ? "yes" : "no";
    else
      real_setting_value = setting_value.get<std::string>();

    this->set_setting(setting_name, real_setting_value);
  }

  // Return the loaded settings, so it can be checked against for whether a
  // setting is default
  return saved_json;
}
