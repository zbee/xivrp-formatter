// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "loading.h"
#include "../common/utilities.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <utility>

using json = nlohmann::json;

messages::load::load(const settings::structure &settings) {
  std::cout << "Loading messages ..." << std::endl;

  // Set the path to the messages log file
  this->messages_log_file = settings.log_file_path;

  // Open the messages log file
  std::stringstream log_file_buffer;
  std::string log_file_content;
  std::string line;
  std::ifstream file;
  file.open(this->messages_log_file);

  // Make sure the log file can be opened
  if (file.is_open()) {
    while (std::getline(file, line)) {
      log_file_buffer << line << '\n';
    }
    log_file_content = log_file_buffer.str();
    file.close();
    std::cout << "...message log file loaded" << std::endl;
  } else { // If the file could not be opened, exit
    std::cout << "Error: Messages log file could not be opened." << std::endl;
    file.close();
    exit(3);
  }

  // Parse the file contents into JSON
  this->messages_log = json::parse(log_file_content);

  // Print some metadata about the messages log
  std::cout << "...message log file parsed" << std::endl;
  std::cout << "...found " << this->get_number_of_messages() << " messages"
            << std::endl;
  std::cout << "...found " << this->get_number_of_participants()
            << " characters" << std::endl;
  std::cout << "...found " << this->get_messages_owner() << " as the log owner"
            << std::endl;

  // Build the message list
  auto messages_list = get_messages();
  std::cout << "...messages parsed" << std::endl;

  // Build the messages object
  this->messages = messages::structure(
      this->get_messages_owner(), this->get_number_of_messages(),
      this->get_number_of_participants(), std::move(messages_list),
      common::utilities::convert_timestamp(this->messages_log[0]["DateSent"]),
      common::utilities::convert_timestamp(
          this->messages_log[this->get_number_of_messages() - 1]["DateSent"]));

  std::cout << "...messages built" << std::endl << std::endl;
}

std::string messages::load::get_messages_owner() {
  // Return the owner of the messages (the first message's owner)
  return this->messages_log[0]["OwnerId"];
}

int messages::load::get_number_of_messages() {
  // Return the number of messages in the log
  return (int)this->messages_log.size();
}

int messages::load::get_number_of_participants() {
  std::list<std::string> participants;

  // Loop through the messages and add the character name to the list,
  // without duplicate entries
  for (int i = 0; i < this->get_number_of_messages(); i++) {
    // Get the character name
    std::string author = this->messages_log[i]["SenderName"];
    // If the character name is not already in the list, add it
    if (std::find(participants.begin(), participants.end(), author) ==
        participants.end()) {
      participants.push_back(author);
    }
  }

  // Return the number of unique character names
  return (int)participants.size();
}

std::list<messages::message> messages::load::get_messages() {
  std::list<messages::message> messages;
  int number_of_messages = this->get_number_of_messages();
  int skipped_messages = 0;

  // Set the number of status updates
  int number_of_updates = 5;
  int frequency = number_of_messages / (number_of_updates - 1);

  int i = 0;
  for (auto &message : this->messages_log) {
    //<editor-fold desc="Status Updates">
    // Check if it's time for a status update
    if (i % frequency == 0) {
      std::cout << "......parsing messages " << i
                << " - "
                // Check if the next update would be out of bounds
                << (((i + frequency) > number_of_messages) ? number_of_messages
                                                           : (i + frequency))
                << std::endl;
    }
    i++;
    //</editor-fold>

    auto message_body = messages::message_body(message["Message"]);

    if (message_body.to_print().size() < 10) {
      skipped_messages++;
      continue;
    }

    messages.emplace_back(
        i, message["SenderName"], message_body,
        common::utilities::convert_timestamp(this->messages_log[0]["DateSent"]),
        common::utilities::convert_timestamp(message["DateSent"]));
  }

  std::cout << "......skipped " << skipped_messages << " short messages"
            << std::endl;

  return messages;
}
