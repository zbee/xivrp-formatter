// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef XIVRP_FORMATTER_ASK_H
#define XIVRP_FORMATTER_ASK_H

#include "../includes/json.hpp"
#include <list>

using json = nlohmann::json;

namespace settings {

/**
 * @brief The answer for the user to give to run with all defaults
 */
const std::string all_defaults = "ad";

/**
 * @brief The source of the default answer
 */
enum default_source {
  no_default = -1,    // No default answer
  default_answer = 0, // The default answer
  settings = 1,       // A saved setting
};

/**
 * @brief The type of comparison to use for requirements
 */
enum compare {
  is_any = 0,
  not_any = 1,
  is = 2,
  is_not = 3,
};

/**
 * @brief The type of answers that questions want to receive
 */
enum answer_types {
  option = -1,
  hex_color = 0,
  yesno = 1,
  path = 2,
  string = 3,
};

/**
 * @brief Simply yes and no, codified to make it easier to handle and convey
 */
enum answer { no = -2, yes = -1 };

struct question {
public:
  /**
   * @brief Constructor for guided settings setup, i.e. the initialization of
   * the program
   * @param structured_question The structured question data
   * @param answers_so_far The answers so far received, for checking this
   * question's requirements against
   * @param default_source The default source type, program default, saved
   * settings, or no default, from the enum settings::default_source
   * @param default_answer The default answer
   */
  question(json structured_question,
           std::map<std::string, std::pair<bool, std::string>> answers_so_far,
           default_source default_source, std::string default_answer = "");
  /**
   * @brief Constructor for a single question
   * @param question_to_ask The question to ask
   * @param answer_type The type of answer to expect, from the enum
   * settings::answer_types
   * @param options The options, if the answer type is option
   */
  question(std::string question_to_ask, answer_types answer_type,
           std::map<std::string, std::string> options = {});

  /**
   * @brief Ask the question if its requirements are met, handling the
   * defaulting and post-formatting to standard responses. Only public method,
   * this is what is used to 'run' the question
   */
  void ask();

  /**
   * @brief The answer the user gave to the question, formatted to standard
   * responses. This is the main output of the question
   */
  std::string answer;
  /**
   * @brief If the question was asked. This should be checked before accessing
   * the answer
   */
  bool asked{false};
  /**
   * @brief If all defaults were requested
   */
  bool all_defaults_requested{false};
  /**
   * @brief If the program-provided default was used. This should be checked
   * before saving the answer
   */
  bool used_default{false};

private:
  /**
   * @brief The answers so far received, for checking this question's
   * requirements against
   */
  std::map<std::string, std::pair<bool, std::string>> answers_so_far = {};

  /**
   * @brief The structured question data, i.e. the raw JSON from the guide
   */
  json structured_question;
  /**
   * @brief The question to ask the user
   */
  std::string question_to_ask;
  /**
   * @brief The type of answer to expect
   * @see settings::answer_types
   */
  answer_types answer_type;

  /**
   * @brief The default answer, if applicable
   */
  std::string default_answer;
  /**
   * @brief The source of the default answer
   * @see settings::default_source
   */
  default_source default_source{default_source::no_default};

  /**
   * @brief If the question is from a structured question json, which controls
   * whether all defaults and question requirements are supported
   */
  bool structured{false};
  /**
   * @brief If the question has requirements
   */
  bool has_requirements{false};
  /**
   * @brief If the question has options
   */
  bool has_options{false};

  // The requirements for the question
  std::map<std::string, std::pair<compare, std::list<int>>> requirements;
  // The options for the question
  std::map<std::string, std::string> options;

  /**
   * @brief Load the options from the structured question data
   */
  void load_options();

  /**
   * @brief Load the requirements from the structured question data
   */
  void load_requirements();

  /**
   * @brief Check if the question's requirements are met based off of previous
   * answers, handling converting previous answers to how they actually would
   * have been typed
   * @return If the requirements are met
   */
  bool check_requirements_met();

  /**
   * @brief Present the question, including the question itself, the default
   * answer if applicable, and the options if applicable
   */
  void present_question();

  /**
   * @brief Present the options to the user
   */
  void present_options();

  /**
   * @brief Validate the answer, checking if it is valid for the question's
   * answer_type, per enum settings::answer_types
   * @param working_answer The answer to validate
   * @return If the answer is valid
   */
  bool validate_answer(std::string working_answer);
};

class ask {
public:
  /**
   * @brief Ask the questions from the given guide, using the given defaults,
   * constructing and passing along previous answers to settings::question as it
   * goes
   * @param ask_guide The guide to ask from
   * @param defaults The defaults to use, including were the default is from
   */
  ask(json ask_guide,
      std::map<std::string, std::pair<default_source, std::string>> defaults);

  // Map of answers received
  std::map<std::string, std::pair<bool, std::string>> answers;
};

} // namespace settings

#endif // XIVRP_FORMATTER_ASK_H
