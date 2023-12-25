// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef CAPSTONE_LION_MESSAGE_H
#define CAPSTONE_LION_MESSAGE_H

#include <chrono>
#include <list>
#include <string>

namespace messages {

// Just using a struct wrapper for the message body to use chain calls for html
// and print/actual versions of the message
struct message_body {
public:
  explicit message_body(std::string content);

  // Methods to get the contents out in usable formats
  std::string to_html();
  std::string to_print();
  std::string to_str();

  // Method to remove continuation marks
  void remove_continuation_marks();

private:
  std::string content;
};

struct message {
public:
  // Metadata about the message
  int id;

  // Author of the message
  std::string author;

  // Content of the message
  messages::message_body content;

  // Number of words of message
  int message_length;

  // Time of the message
  std::string datetime;
  std::string time_into_session;

  message(int id, std::string author, messages::message_body content,
          std::chrono::system_clock::time_point start_time,
          std::chrono::system_clock::time_point message_time);

  // Method to highlight ~emphatics~
  void highlight_emphatics(const std::string &color);

  // Method to format the message into HTML
  std::string format();

  // Method to set the time data
  std::string timestamp;
  std::string into_session;

  // Metadata about the message
  bool is_continued = false;
  bool is_continuation = false;
  bool has_emphatics = false;
  bool is_ooc = false;

  // Method to count the number of words in the message
  static int count_words(const std::string &str);

private:
  // Method to check for continuation
  void check_for_continuation();

  // Method to check for emphatics
  void check_for_emphatics();

  // Method to check for OOC
  void check_for_ooc();

  // Actual time of the messages
  std::chrono::system_clock::time_point time;
  std::chrono::duration<double> elapsed_time;

  // Method to set the time data
  void set_time_data(std::chrono::system_clock::time_point start_time,
                     std::chrono::system_clock::time_point message_time);

  static bool starts_with_any(const std::string &str,
                              const std::list<std::string> &arr);
  static bool ends_with_any(const std::string &str,
                            const std::list<std::string> &arr);
};

} // namespace messages

#endif // CAPSTONE_LION_MESSAGE_H
