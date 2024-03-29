// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef MESSAGE_H
#define MESSAGE_H

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
  std::string into_session;
  std::chrono::system_clock::time_point time;

  message(int id, std::string author, messages::message_body content,
          std::chrono::system_clock::time_point start_time,
          std::chrono::system_clock::time_point message_time);

  // Method to highlight ~emphatics~
  void highlight_emphatics(const std::string &color);

  // Method to format the message into HTML
  std::string format();

  // Metadata about the message
  bool is_continued = false;
  bool is_continuation = false;
  bool has_emphatics = false;
  bool is_ooc = false;
  int session_ID = 0;
  bool has_gap_after = false;

  // Duration of a gap after the message
  std::chrono::duration<double> gap_duration{0};

  // Actual time of the messages
  std::chrono::duration<double> elapsed_time;

private:
  // Method to check for continuation
  void check_for_continuation();

  // Method to check for emphatics
  void check_for_emphatics();

  // Method to check for OOC
  void check_for_ooc();

  // Method to set the time data
  void set_time_data(std::chrono::system_clock::time_point start_time,
                     std::chrono::system_clock::time_point message_time);
};

} // namespace messages

#endif // MESSAGE_H
