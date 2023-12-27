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
  gaps(const messages::structure &messages);
  gaps() = default;

  std::any messages;

  std::chrono::duration<double> average_gap;

  std::list<std::pair<int, std::chrono::duration<double>>> gaps_found;

  int number_of_gaps_found;

  std::chrono::duration<double>
  find_gap_from_previous(std::list<messages::message>::iterator &message);

  void find_average_gap();

  void find_gaps();

  std::chrono::duration<double>
  find_what_gap_should_be(std::list<messages::message>::iterator &message);

  void squash_gap(std::list<messages::message>::iterator &message);

  void assign_gaps_to_messages();
};

} // namespace messages

#endif // XIVRP_FORMATTER_GAPS_H
