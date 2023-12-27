// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef XIVRP_FORMATTER_GAPS_H
#define XIVRP_FORMATTER_GAPS_H

#include "message.h"
#include "messages.h"
#include <any>
#include <chrono>
#include <list>

namespace messages {

class gaps {
public:
  // Constructor, loading the messages
  explicit gaps(const messages::structure &messages);
  gaps() = default;

  // The number of gaps found
  int number_of_gaps_found{0};

  // Total time of gaps removed
  std::chrono::duration<double> gap_squashed{0};
  // Average gap length
  std::chrono::duration<double> average_gap{0};

  // Storing the messages
  messages::structure messages;

private:
  // message ID, gap length for gaps found
  std::list<std::pair<int, std::chrono::duration<double>>> gaps_found;

  // Method to find the iterator for a message
  bool find_message_iterator(const message &targetMessage,
                             std::list<messages::message>::iterator &iterator);

  // Method to find the gap from the previous message
  std::chrono::duration<double>
  find_gap_from_previous(std::list<messages::message>::iterator &message);

  // Method to find the average gap length
  void find_average_gap();

  // Method to loop through the messages and find the gaps
  void find_gaps();

  // Method to assign the gaps to the messages
  void assign_gaps_to_messages();

  // Method to remove the gap from the time-into from all messages after the
  // gap
  void squash_gap();
};

} // namespace messages

#endif // XIVRP_FORMATTER_GAPS_H
