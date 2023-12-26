// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef LOADING_H
#define LOADING_H

#include "../includes/json.hpp"
#include "../settings/settings.h"
#include "message.h"
#include "messages.h"
#include <list>
#include <map>
#include <string>

using json = nlohmann::json;

namespace messages {

class load {
public:
  // The generated messages object
  messages::structure messages;

  // Constructor, loads and parses the messages log file, then calls the
  // metadata methods
  explicit load(const settings::structure &settings);

private:
  // Path to the messages log file
  std::string messages_log_file;

  // Parsed JSON messages log
  json messages_log;

  // Metadata about the messages
  // Owner of the messages
  std::string get_messages_owner();

  // Number of messages
  int get_number_of_messages();

  // Number of participants
  int get_number_of_participants();

  // Method to convert a date/time string into a time_point
  static std::chrono::system_clock::time_point
  convertDateTimeString(const std::string &dateTimeString);

  std::list<messages::message> get_messages();
};

} // namespace messages

#endif // LOADING_H
