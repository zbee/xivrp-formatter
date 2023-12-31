// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "ask.h"
#include "../common/utilities.h"
#include <iostream>
#include <utility>

namespace settings {

/**
 * @brief Constructor for guided settings setup, i.e. the initialization of the
 * program
 * @param structured_question The structured question data
 * @param answers_so_far The answers so far received, for checking this
 * question's requirements against
 * @param default_source The default source type, program default, saved
 * settings, or no default, from the enum settings::default_source
 * @param default_answer The default answer
 */
question::question(
    json structured_question,
    std::map<std::string, std::pair<bool, std::string>> answers_so_far,
    enum default_source default_source, std::string default_answer) {
  this->structured = true;

  // Trying to load the question
  if (structured_question.contains("question"))
    this->question_to_ask = structured_question["question"];
  else
    this->question_to_ask =
        "No question provided in guide for: " +
        structured_question["identifier"].get<std::string>() +
        ". Please report this.";

  // Casting the answer data
  this->answer_type =
      static_cast<enum answer_types>(structured_question["wants"].get<int>());
  this->answers_so_far = std::move(answers_so_far);

  // Checking for requirements and options
  this->has_requirements = structured_question.contains("requires");
  this->has_options = structured_question.contains("options") ||
                      this->answer_type == answer_types::option ||
                      this->answer_type == answer_types::yesno;

  this->default_source = default_source;
  this->default_answer = std::move(default_answer);

  // Save the structured question
  this->structured_question = std::move(structured_question);

  // Loading in more detailed data
  if (this->has_options)
    this->load_options();
  if (this->has_requirements)
    this->load_requirements();
}

/**
 * @brief Constructor for a single question
 * @param question_to_ask The question to ask
 * @param answer_type The type of answer to expect, from the enum
 * settings::answer_types
 * @param options The options, if the answer type is option
 */
question::question(std::string question_to_ask, answer_types answer_type,
                   std::map<std::string, std::string> options) {
  this->question_to_ask = std::move(question_to_ask);
  this->answer_type = answer_type;
  this->has_options = !options.empty() || answer_type == answer_types::option ||
                      answer_type == answer_types::yesno;
  this->options = std::move(options);
}

/**
 * @brief Ask the question if its requirements are met, handling the defaulting
 * and post-formatting to standard responses. Only public method, this is what
 * is used to 'run' the question
 */
void question::ask() {
  // If the question has already been asked, return
  if (this->asked)
    return;

  // If the question has requirements, check them
  bool passed_requirements = true;
  if (this->has_requirements)
    if (!this->check_requirements_met())
      passed_requirements = false;

  if (passed_requirements) {
    // Present the question
    this->present_question();

    // Get the answer
    std::cout << ">";
    std::getline(std::cin, this->answer);

    // Validate the answer, and ask again if it's invalid
    while (!this->validate_answer(this->answer)) {
      std::cout << "Invalid answer. Please try again." << std::endl << ">";
      std::getline(std::cin, this->answer);
    }

    // Save the answer
    this->asked = true;
  }

  // Handling defaulting
  if (this->default_source != default_source::no_default) {
    if (this->structured) {
      // If the answer is requesting all defaults, set the flag
      auto lowercase_answer = this->answer;
      std::transform(lowercase_answer.begin(), lowercase_answer.end(),
                     lowercase_answer.begin(), ::tolower);
      // Check for all defaults
      if (lowercase_answer == settings::all_defaults) {
        this->all_defaults_requested = true;
        this->answer.clear();
      }
    }

    // If the answer is empty, use the default
    if (this->answer.empty()) {
      this->answer = this->default_answer;
      if (this->default_source == default_source::default_answer)
        this->used_default = true;
    }
  }

  // Lowercase yes/nos
  if (this->answer_type == answer_types::yesno) {
    std::transform(this->answer.begin(), this->answer.end(),
                   this->answer.begin(), ::tolower);
    if (this->answer == "y" || this->answer == "yes" ||
        this->answer == std::to_string(answer::yes))
      this->answer = "yes";
    else
      this->answer = "no";
  }

  // Real paths
  auto real_path = common::utilities::get_real_path(this->answer);
  if (this->answer_type == answer_types::path && real_path != this->answer) {
    this->answer = real_path;
    this->used_default = false;
  }
}

/**
 * @brief Load the options from the structured question data
 */
void question::load_options() {
  for (auto &option : this->structured_question["options"]) {
    // Get both option values as strings
    std::string option_name = option["name"].get<std::string>();
    std::string option_value = std::to_string(option["value"].get<int>());
    // Save the option
    this->options[option_value] = option_name;
  }
}

/**
 * @brief Load the requirements from the structured question data
 */
void question::load_requirements() {
  for (auto &requirement : this->structured_question["requires"]) {
    // Get the requirement name
    std::string requirement_name = requirement["identifier"].get<std::string>();
    // Get the requirement type
    auto requirement_type =
        static_cast<enum compare>(requirement["comparison"].get<int>());

    // Get the requirement values
    std::list<int> requirement_values;
    if (requirement["value"].is_array())
      for (auto &value : requirement["value"])
        requirement_values.push_back(value.get<int>());
    else
      requirement_values.push_back(requirement["value"].get<int>());

    // Create requirement pair
    auto requirement_pair = std::pair<enum compare, std::list<int>>(
        requirement_type, requirement_values);

    // Save the requirement
    this->requirements[requirement_name] = requirement_pair;
  }
}

/**
 * @brief Check if the question's requirements are met based off of previous
 * answers, handling converting previous answers to how they actually would have
 * been typed
 * @return If the requirements are met
 */
bool question::check_requirements_met() {
  // Check each requirement
  for (auto &requirement : this->requirements) {
    // Get the requirement name
    std::string requirement_name = requirement.first;
    // Get the requirement type
    auto requirement_comparison_type = requirement.second.first;
    // Get the requirement values
    std::list<int> requirement_values = requirement.second.second;

    // Get the previous_answer
    std::string previous_answer = this->answers_so_far[requirement_name].second;

    // Convert all the requirements to strings, as that is what answers are in
    std::list<std::string> string_requirement_values;
    for (auto &value : requirement_values)
      string_requirement_values.push_back(std::to_string(value));

    // Convert yes/no to their respective words
    for (auto &value : string_requirement_values)
      if (value == std::to_string(answer::yes))
        value = "yes";
      else if (value == std::to_string(answer::no))
        value = "no";

    // Check the requirement against given values
    switch (requirement_comparison_type) {
    case compare::is_any:
    case compare::is:
      for (auto &value : string_requirement_values)
        if (previous_answer == value)
          return true;
      return false;
    case compare::not_any:
    case compare::is_not:
      for (auto &value : string_requirement_values)
        if (previous_answer == value)
          return false;
      return true;
    }
  }

  return false;
}

/**
 * @brief Present the question, including the question itself, the default
 * answer if applicable, and the options if applicable
 */
void question::present_question() {
  // Present the question
  std::cout << std::endl << this->question_to_ask << std::endl;

  // If the question has a default answer, present it
  if (this->default_source != default_source::no_default) {
    std::cout << "[press enter to accept ";

    if (this->default_source == default_source::default_answer)
      std::cout << "default";
    else if (this->default_source == default_source::settings)
      std::cout << "saved setting";

    std::cout << ": " << this->default_answer << "]" << std::endl;
  }

  // If the question has options, present them
  if (this->has_options)
    this->present_options();
}

/**
 * @brief Present the options to the user
 */
void question::present_options() {
  if (this->answer_type == answer_types::option)
    for (auto &option : this->options)
      std::cout << "  [" << option.first << "] for " << option.second
                << std::endl;
  else if (this->answer_type == answer_types::yesno)
    std::cout << "  [y] for yes" << std::endl << "  [n] for no" << std::endl;
}

/**
 * @brief Validate the answer, checking if it is valid for the question's
 * answer_type, per enum settings::answer_types
 * @param working_answer The answer to validate
 * @return If the answer is valid
 */
bool question::validate_answer(std::string working_answer) {
  // If the working_answer is empty, it's default
  if (working_answer.empty() &&
      this->default_source != default_source::no_default)
    return true;

  // If the working_answer is all defaults, it's valid
  auto lowercase_answer = working_answer;
  std::transform(lowercase_answer.begin(), lowercase_answer.end(),
                 lowercase_answer.begin(), ::tolower);
  // Check for all defaults
  if (lowercase_answer == settings::all_defaults)
    return true;

  // If the working_answer is a number, it's valid
  if (this->answer_type == answer_types::option)
    if (std::isdigit(working_answer[0]) &&
        this->options.contains(working_answer))
      return true;

  // If the working_answer is yes or no, it's valid
  if (this->answer_type == answer_types::yesno)
    if (lowercase_answer == "y" || lowercase_answer == "n" ||
        lowercase_answer == "yes" || lowercase_answer == "no")
      return true;

  // If the working_answer is a valid path
  if (this->answer_type == answer_types::path)
    if (common::utilities::check_file_exists(working_answer))
      return true;

  // If the working_answer is a hex color, it's valid
  if (this->answer_type == answer_types::hex_color)
    if (common::utilities::check_hex_color(working_answer))
      return true;

  // If the working_answer is a string, it's valid
  if (this->answer_type == answer_types::string)
    return true;

  // If the working_answer is none of the above, it's invalid
  return false;
}

/**
 * @brief Ask the questions from the given guide, using the given defaults,
 * constructing and passing along previous answers to settings::question as it
 * goes
 * @param ask_guide The guide to ask from
 * @param defaults The defaults to use, including were the default is from
 */
ask::ask(
    json ask_guide,
    std::map<std::string, std::pair<default_source, std::string>> defaults) {
  bool all_defaults_requested = false;

  std::cout << "[enter '" << settings::all_defaults
            << "' at any time to default all further settings]" << std::endl;

  // Load and ask the questions
  for (auto &question_data : ask_guide) {
    // Get the question name
    std::string question_ID = question_data["identifier"].get<std::string>();

    // Check if this should be asked
    if (!question_data.contains("question"))
      continue;

    // Check if all defaults were requested
    if (all_defaults_requested) {
      // If so, set the default answer
      this->answers[question_ID] = std::pair<bool, std::string>(
          defaults[question_ID].first == default_source::default_answer,
          defaults[question_ID].second);
      continue;
    }

    // Create the question
    question question(question_data, this->answers, defaults[question_ID].first,
                      defaults[question_ID].second);

    // Ask the question
    question.ask();

    // Check if it was asked
    if (!question.asked) {
      this->answers[question_ID] = std::pair<bool, std::string>(
          defaults[question_ID].first == default_source::default_answer,
          question.answer);
      continue;
    }

    // Check if all defaults were requested
    if (question.all_defaults_requested)
      all_defaults_requested = true;

    // Save the answer
    this->answers[question_ID] =
        std::pair<bool, std::string>(question.used_default, question.answer);
  }
}

} // namespace settings
