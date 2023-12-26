// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef CAPSTONE_LION_MESSAGES_H
#define CAPSTONE_LION_MESSAGES_H

#include "message.h"
#include <any>
#include <chrono>
#include <list>
#include <map>
#include <string>

namespace messages {

struct structure {
public:
  // Metadata about the messages
  // Owner of the messages
  std::string owner;

  // Number of messages
  int number_of_messages;

  // Number of participants
  int number_of_participants;

  // Formatted time of the messages
  std::string datetime;
  std::string duration;

  // Constructor, loads message objects into the array, and sets broad metadata
  structure(std::string owner, int number_of_messages,
            int number_of_participants, std::list<messages::message> messages,
            std::chrono::system_clock::time_point start_time,
            std::chrono::system_clock::time_point end_time);
  structure() = default;

  // Method to combine continued messages
  int combine(bool debug);

  // Method to remove OOC messages
  int remove_ooc();

  // Method to highlight ~emphatics~
  int highlight_emphatics(std::string color);

  // Method to format the messages into HTML
  std::map<std::string, std::string> format();

  // Method to format the messages into HTML, with related images
  std::map<std::string, std::string> format(std::any structured_related_images);

  // Method to format the messages out into a debug print
  void debug_print();

  // Array of messages
  std::list<messages::message> messages;

private:
  // Actual time of the messages
  std::chrono::system_clock::time_point start_time;
  std::chrono::system_clock::time_point end_time;
  std::chrono::duration<double> elapsed_time;

  // Method to set the time data
  void set_time_data(std::chrono::system_clock::time_point start_time,
                     std::chrono::system_clock::time_point end_time);
};

} // namespace messages

#endif // CAPSTONE_LION_MESSAGES_H
