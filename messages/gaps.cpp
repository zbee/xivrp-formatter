// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "gaps.h"
#include "../includes/date.h"
#include <numeric>

namespace messages {

gaps::gaps(const structure &messages) {
  this->messages = messages;

  this->find_average_gap();
  this->find_gaps();
  this->assign_gaps_to_messages();
  this->squash_gap();
}

bool gaps::find_message_iterator(
    const message &targetMessage,
    std::list<messages::message>::iterator &iterator) {
  // Loop through the messages and find the iterator for the message
  for (iterator = this->messages.messages.begin();
       iterator != this->messages.messages.end(); ++iterator)
    // If this is the message we're checking for
    if ((*iterator).id == targetMessage.id)
      return true;

  return false;
}

std::chrono::duration<double>
gaps::find_gap_from_previous(std::list<message>::iterator &message) {
  // If this is the first message, return 0
  if (message == this->messages.messages.begin())
    return std::chrono::duration<double>(0);

  // Load this message's data
  auto this_message = *message;

  // Load the previous message's data
  auto previous_message = *std::prev(message);

  // Find the gap
  return this_message.time - previous_message.time;
}

void gaps::find_average_gap() {
  std::list<std::chrono::duration<double>> working_gaps;
  std::chrono::duration<double> working_average{};

  // Loop through the messages and find the average gap
  for (auto &message : this->messages.messages) {
    // Find the iterator
    std::list<messages::message>::iterator message_iterator;
    this->find_message_iterator(message, message_iterator);

    // Save the value for averaging
    working_gaps.emplace_back(this->find_gap_from_previous(message_iterator));
  }

  // Recalculate the working average
  working_average = std::accumulate(working_gaps.begin(), working_gaps.end(),
                                    std::chrono::duration<double>(0)) /
                    working_gaps.size();

  // Exclude outliers from average, save this->average_gap
  for (auto &gap : working_gaps)
    if (gap < (working_average * 2))
      this->average_gap += gap;
  this->average_gap /= (double)working_gaps.size();
}

void gaps::find_gaps() {
  // Loop through the messages and find gaps significantly larger than the
  // average
  for (const messages::message &message : this->messages.messages) {
    // Find the iterator
    std::list<messages::message>::iterator message_iterator;
    this->find_message_iterator(message, message_iterator);

    // If this is the first message, skip it
    if (message_iterator == this->messages.messages.begin())
      continue;

    // Find the gap from the previous message
    auto gap_duration = this->find_gap_from_previous(message_iterator);

    // If the gap is more than thrice the average, and it is more than an hour
    if (gap_duration > (this->average_gap * 3) &&
        gap_duration > std::chrono::duration<double>(3600)) {
      // Save the gap
      this->gaps_found.emplace_back(message.id, gap_duration);
      this->number_of_gaps_found++;
    }
  }
}

void gaps::assign_gaps_to_messages() {
  // Loop through the gaps and assign them to the messages
  for (auto &gap : this->gaps_found)
    for (auto &message : this->messages.messages)
      // If this is the message we're looking for
      if (message.id == gap.first) {
        // Set the gap duration
        message.gap_duration = gap.second;

        // Set the gap flag
        message.has_gap_after = true;
      }
}

void gaps::squash_gap() {
  // Loop through the gaps and messages, adjusting the time-into-session for
  // each message after the gap
  for (auto &gap : this->gaps_found) {
    for (auto &message : this->messages.messages)
      // If this is the message we're looking for, or after it
      if (message.id >= gap.first) {
        // Add the average gap to the elapsed time
        message.elapsed_time += this->average_gap;
        // Subtract the gap duration from the elapsed time
        message.elapsed_time -= gap.second;

        // Reformat into_session
        message.into_session = date::format(
            "%R", floor<std::chrono::milliseconds>(message.elapsed_time));
      }

    // Track the total time squashed
    this->gap_squashed += gap.second - this->average_gap;

    // Update the elapsed time for the session
    messages.elapsed_time += this->average_gap;
    messages.elapsed_time -= gap.second;
    messages.duration = date::format(
        "%R", date::floor<std::chrono::milliseconds>(messages.elapsed_time));
  }
}

} // namespace messages
