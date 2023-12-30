// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef XIVRP_FORMATTER_ASK_H
#define XIVRP_FORMATTER_ASK_H

#include "../includes/json.hpp"
#include <list>

using json = nlohmann::json;

namespace settings {

const std::string all_defaults = "ad";

enum default_source {
  no_default = -1,    // No default answer
  default_answer = 0, // The default answer
  settings = 1,       // A saved setting
};

enum compare {
  is_any = 0,
  not_any = 1,
  is = 2,
  is_not = 3,
};

enum answer_types {
  option = -1,
  hex_color = 0,
  yesno = 1,
  path = 2,
};

enum answer { no = 0, yes = 1 };

struct question {
public:
  // Constructor for guided settings
  question(json structured_question,
           std::map<std::string, std::pair<bool, std::string>> answers_so_far,
           default_source default_source, std::string default_answer = "");
  // Constructor for a single question
  question(std::string question_to_ask, answer_types answer_type,
           std::map<std::string, std::string> options = {});

  // Ask the question
  void ask();

  // Answer data
  std::string answer;
  bool asked{false};
  // Default data
  bool all_defaults_requested{false};
  bool used_default{false};

private:
  // The answers so far received, to check requirements against
  std::map<std::string, std::pair<bool, std::string>> answers_so_far = {};

  // The question data
  json structured_question;
  std::string question_to_ask;
  answer_types answer_type;

  // Default data
  std::string default_answer;
  default_source default_source{default_source::no_default};

  // If the question is from a structured question json
  bool structured{false};
  // If the question has requirements
  bool has_requirements{false};
  // If the question has options
  bool has_options{false};

  // The requirements for the question
  std::map<std::string, std::pair<compare, std::list<int>>> requirements;
  // The options for the question
  std::map<std::string, std::string> options;

  // Load the options
  void load_options();

  // Load the requirements
  void load_requirements();

  // Verify that the question should be asked
  bool check_requirements_met();

  // Actually asks, building the question and default information
  void present_question();

  // Present the options
  void present_options();

  // Validate the working_answer
  bool validate_answer(std::string working_answer);
};

class ask {
public:
  // Collect the ask guide and defaults
  ask(json ask_guide,
      std::map<std::string, std::pair<default_source, std::string>> defaults);

  // Map of answers received
  std::map<std::string, std::pair<bool, std::string>> answers;

private:
  // The collected asking guide
  json ask_guide;

  // The collected defaults
  std::map<std::string, std::pair<default_source, std::string>> defaults;

  // Load the questions
  std::list<question> questions;
};

} // namespace settings

#endif // XIVRP_FORMATTER_ASK_H
