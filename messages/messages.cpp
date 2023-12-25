// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "messages.h"
#include "../includes/date.h"
#include <iostream>
#include <utility>

messages::structure::structure(
    std::string owner, int number_of_messages, // NOLINT(*-pro-type-member-init)
    int number_of_participants, std::list<messages::message> messages,
    std::chrono::system_clock::time_point start_time,
    std::chrono::system_clock::time_point end_time) {
  this->owner = std::move(owner);
  this->number_of_messages = number_of_messages;
  this->number_of_participants = number_of_participants;
  this->messages = std::move(messages);
  this->set_time_data(start_time, end_time);
}

std::string selectFirstNWords(const std::string &str, int n) {
  std::istringstream iss(str);
  std::vector<std::string> words;
  std::string word;
  while (iss >> word) {
    words.push_back(word);
  }

  std::string result;
  for (int i = 0; i < n && i < words.size(); ++i) {
    result += words[i] + " ";
  }

  return result;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NullDereference"
int messages::structure::combine(bool debug) {
  std::list<messages::message> combined_messages;
  int count = 0;
  int failed = 0;
  std::string message_content;

  // Variables to track and build combination messages
  message *message_seeking_continuation;
  bool seeking_continuation = false;
  std::string message_to_continue_author;
  std::string message_to_combine_into;

  // Add a filler message to the end of the list to make sure the last message
  // can fail to continue if it needs to
  this->messages.emplace_back(9999999999, "FILLER",
                              messages::message_body("FILLER"),
                              this->start_time, this->end_time);

  if (debug)
    std::cout << std::endl;

  // Iterate over the messages, and combine those that are continued
  for (auto &message : this->messages) {
    message.content.remove_continuation_marks();
    message_content = message.content.to_str();

    if (debug)
      std::cout << "'" << selectFirstNWords(message_content, 5) << "' ... ";

    // If the message needs to be continued
    if (message.is_continued && !seeking_continuation) {
      if (debug)
        std::cout << "continuing ... " << std::endl;

      seeking_continuation = true;
      message_to_continue_author = message.author;
      message_to_combine_into = message_content;
      message_seeking_continuation = &message;
    }

    // If the message is a continuation of the previous message, combine it
    else if (seeking_continuation &&
             (message.author == message_to_continue_author ||
              message.is_continuation)) {

      if (debug)
        std::cout << "continuing '"
                  << selectFirstNWords(message_to_combine_into, 5)
                  << "' ... with ... '" << selectFirstNWords(message_content, 5)
                  << "' ... " << std::endl;

      // Add to the message
      message_to_combine_into += " " + message_content;

      count++;

      // If the message is the last in the continuation, add it to the list of
      // combined messages
      if (!message.is_continued) {
        if (debug)
          std::cout << "fin" << std::endl;

        // Unmark the message as continued
        (*message_seeking_continuation).is_continued = false;
        // This isn't what the field was meant for, but since it will no longer
        // be used ... Mark the message as a continuation, to indicate that it
        // was combined
        (*message_seeking_continuation).is_continuation = true;

        // Set the content of the message to the combined content
        (*message_seeking_continuation).content =
            messages::message_body(message_to_combine_into);

        // Set the message length
        (*message_seeking_continuation).message_length =
            messages::message::count_words(message_to_combine_into);

        // Save the message
        combined_messages.push_back(*message_seeking_continuation);

        // Reset continuation variables
        seeking_continuation = false;
        message_to_continue_author = "";
        message_to_combine_into = "";
        message_seeking_continuation = nullptr;
      }
    }

    //<editor-fold desc="Non-continued messages, handling failures to continue">
    // If the message is not a continuation, add it to the list of combined
    // messages
    else {
      // If we failed to find the continuation, add the last message to the list
      if (seeking_continuation) {
        combined_messages.push_back(*message_seeking_continuation);
        failed++;
      }

      // Save the message, if it wasn't the filler at the end
      if (message.author != "FILLER")
        combined_messages.push_back(message);

      // Reset continuation variables
      seeking_continuation = false;
      message_to_continue_author = "";
      message_to_combine_into = "";
      message_seeking_continuation = nullptr;
    }
    //</editor-fold>
  }

  // Set the messages to the combined messages
  this->messages = combined_messages;
  // Update the message count
  this->number_of_messages = int(this->messages.size());

  std::cout << "...Failed to continue " << failed << " message"
            << (failed != 1 ? "s" : "") << "." << std::endl;

  // Return the number of messages that were combined
  return count;
}
#pragma clang diagnostic pop

int messages::structure::remove_ooc() {
  std::list<messages::message> messages_without_ooc;

  // Iterate over the messages, and only keep those that are not Out Of
  // Character
  for (auto &message : this->messages)
    if (!message.is_ooc)
      messages_without_ooc.push_back(message);

  int count = int(this->messages.size() - messages_without_ooc.size());

  // Set the messages to the messages without OOC
  this->messages = messages_without_ooc;
  this->number_of_messages = int(this->messages.size());

  // Return the number of messages that were removed
  return count;
}

int messages::structure::highlight_emphatics(std::string color) {
  int count = 0;

  // Iterate over the messages, and emphasize those that have emphatics
  for (auto &message : this->messages)
    if (message.has_emphatics) {
      message.highlight_emphatics(color);
      count++;
    }

  // Return the number of messages that were emphasized
  return count;
}

std::map<std::string, std::string> messages::structure::format() {
  std::map<std::string, std::string> template_ready_messages;

  // TODO: look for messages that are a long period apart, remove that time from
  //  the duration, and remove that time from the message's elapsed time
  //  (possibly do it somewhere other than here? a new check_for_breaks method?)

  //<editor-fold desc="Authors">
  // Iterate over the messages and find each author
  std::list<std::string> authors;
  for (auto &message : this->messages)
    if (std::find(authors.begin(), authors.end(), message.author) ==
        authors.end())
      authors.push_back(message.author);

  // Format the authors into a string
  std::string formatted_authors;
  // 1 Author
  if (authors.size() == 1)
    formatted_authors = authors.front();
  // 2 Authors
  else if (authors.size() == 2)
    formatted_authors = authors.front() + " and " + authors.back();
  // 3+ Authors
  else {
    for (auto &author : authors)
      formatted_authors += author + ", ";
    formatted_authors =
        formatted_authors.substr(0, formatted_authors.size() - 2);
  }

  template_ready_messages.insert(
      std::pair<std::string, std::string>("authors", formatted_authors));
  //</editor-fold>

  //<editor-fold desc="Metadata">
  // Get the total number of words in this log
  int word_count = 0;
  for (auto &message : this->messages)
    word_count += message.message_length;

  // Get the average read time, assuming 200 words per minute
  int average_read_time = word_count / 200;

  // Format the metadata into a string
  std::string metadata = std::to_string(this->number_of_messages) +
                         " messages, " + std::to_string(word_count) +
                         " words, " + "~" + std::to_string(average_read_time) +
                         "min read time<br>" + this->datetime;

  template_ready_messages.insert(
      std::pair<std::string, std::string>("metadata", metadata));
  //</editor-fold>

  //<editor-fold desc="Messages">
  // Iterate over the messages, and format them
  std::string formatted_messages;
  for (auto &message : this->messages)
    formatted_messages += message.format();

  template_ready_messages.insert(
      std::pair<std::string, std::string>("messages", formatted_messages));
  //</editor-fold>

  std::cout << "..." << messages.size() << " messages formatted." << std::endl;

  // Return the formatted messages
  return template_ready_messages;
}

void messages::structure::set_time_data(
    std::chrono::system_clock::time_point start_time,
    std::chrono::system_clock::time_point end_time) {
  // Set the start and end time of the log
  this->start_time = start_time;
  this->end_time = end_time;
  // Set how long the log is
  this->elapsed_time = end_time - start_time;
  // Set the formatted start and end time of the log
  this->datetime =
      date::format("%F T %H:%M", floor<std::chrono::milliseconds>(start_time)) +
      " - " +
      date::format("%F T %H:%M", floor<std::chrono::milliseconds>(end_time));
  // Set the formatted duration of the log
  this->duration =
      date::format("%R", date::floor<std::chrono::milliseconds>(elapsed_time));
}

void messages::structure::debug_print() {
  for (auto &message : this->messages)
    std::cout << std::endl
              << message.author << " - " << message.time_into_session
              << "(ooc:" << (message.is_ooc ? "true" : "false")
              << ", has cont:" << (message.is_continued ? "true" : "false")
              << ", is cont:" << (message.is_continuation ? "true" : "false")
              << ")" << std::endl
              << message.content.to_print() << std::endl;
}
