// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "message.h"
#include "../common/utilities.h"
#include "../includes/date.h"
#include <codecvt>
#include <list>
#include <regex>
#include <sstream>
#include <utility>

messages::message::message(int id, // NOLINT(*-pro-type-member-init)
                           std::string author, messages::message_body content,
                           std::chrono::system_clock::time_point start_time,
                           std::chrono::system_clock::time_point message_time)
    : content(std::move(content)) {
  // Set the basic data
  this->id = id;
  this->author = std::move(author);
  this->message_length =
      common::utilities::count_words(this->content.to_print());

  // Set the time data
  this->set_time_data(start_time, message_time);

  // Check for continuation, emphatics, and OOC
  this->check_for_continuation();
  this->check_for_emphatics();
  this->check_for_ooc();
}

void messages::message::highlight_emphatics(const std::string &color) {
  std::string result = this->content.to_print();

  // Replace the emphatics with HTML, with the given color, where the symbols
  // are used to bookend words
  std::list<std::string> emphatics = {"~", "_", "\\*", "/"};

  for (const auto &emphatic : emphatics) {
    // Search for words surrounded by the emphatic mark
    std::regex wordRegex(
        emphatic + "([ \\w]+)" + // NOLINT(*-inefficient-string-concatenation)
        emphatic);
    // Replace the words with the words surrounded by HTML
    result = std::regex_replace( // NOLINT(*-inefficient-string-concatenation)
        result, wordRegex, "<span style=\"color: " + color + ";\">$1</span>");
  }

  // If there are still tildes used to emphasize individual words, handle that
  // too
  std::regex wordRegex(" (\\w+)~");
  result = std::regex_replace(
      result, wordRegex, " <span style=\"color: " + color + ";\">$1</span>");

  this->content = messages::message_body(result);
}

std::string messages::message::format() {
  std::string html = "<div></div>"
                     "<a class='message' href='#" +
                     std::to_string(this->id) + "' id='" +
                     std::to_string(this->id) +
                     "'>"
                     "<div class='header'>" +
                     this->author +
                     "</div>"
                     "<div class='body'>" +
                     this->content.to_html() +
                     "</div>"
                     "<div class='footer'>" +
                     this->datetime + " (" + this->into_session + " in)" +
                     "</div>"
                     "</a>"
                     "<div></div>\n";

  if (this->has_gap_after)
    html += "<div></div><div class=\"message_gap_notice\">"
            "Gap of " +
            date::format("%R",
                         floor<std::chrono::milliseconds>(this->gap_duration)) +
            " found. Adjusting all time-in figures hereafter."
            "</div><div></div>\n";

  return html;
}

void messages::message::check_for_continuation() {
  // TODO: swap this to list of regexes, so I don't have to hardcode message
  //  lengths
  // R"(c[ou]nti?n?u?e?[ds]?.?.?.?.?.?$)" for cont./continued
  // R"( ?\( ?\d+ ?\/ ?[\?\d]+ ?\) ?$)" for (#/#)

  // Check for marks at the end of the message suggesting it is continued
  std::list<std::string> continued_marks = {
      "...",  "..",   "…",    ">>",   "cont", "cont.", "cont.)", "cont)",
      "-",    "1/2)", "1/3)", "2/3)", "1/4)", "2/4)",  "3/4)",   "1/5)",
      "2/5)", "3/5)", "4/5)", "1/6)", "2/6)", "3/6)",  "4/6)",   "5/6)",
      "1/7)", "2/7)", "3/7)", "4/7)", "5/7)", "6/7)",
  };

  if (common::utilities::ends_with_any(this->content.to_print(),
                                       continued_marks)) {
    this->is_continued = true;
  }

  // Check for marks at the beginning or end of the message suggesting it is a
  // continuation of another message
  std::list<std::string> continuation_marks_prefixes = {"...", "..", "…", "-"};
  std::list<std::string> continuation_marks = {
      "2/2)", "2/3)", "3/3)", "2/4)", "3/4)", "4/4)", "2/5)",
      "3/5)", "4/5)", "5/5)", "2/6)", "3/6)", "4/6)", "5/6)",
      "6/6)", "2/7)", "3/7)", "4/7)", "5/7)", "6/7)", "7/7)",
  };

  if (common::utilities::starts_with_any(this->content.to_print(),
                                         continuation_marks_prefixes) ||
      common::utilities::ends_with_any(this->content.to_print(),
                                       continuation_marks)) {
    this->is_continuation = true;
  }
}

void messages::message::check_for_emphatics() {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
  auto has_emphatics = false;
#pragma clang diagnostic pop

  has_emphatics |= this->content.to_print().find('~') != std::string::npos;
  has_emphatics |= this->content.to_print().find('_') != std::string::npos;
  has_emphatics |= this->content.to_print().find('*') != std::string::npos;
  has_emphatics |= this->content.to_print().find('/') != std::string::npos;

  this->has_emphatics = has_emphatics;
}

void messages::message::check_for_ooc() {
  std::list<std::string> pre_marks = {
      "[",
      "(",
      "{",
      "<",
  };
  std::list<std::string> post_marks = {
      "]",
      ")",
      "}",
      ">",
  };

  bool starts_with =
      common::utilities::starts_with_any(this->content.to_print(), pre_marks);
  bool ends_with =
      common::utilities::ends_with_any(this->content.to_print(), post_marks);

  if ((starts_with && ends_with) || starts_with) {
    this->is_ooc = true;
  }
}

void messages::message::set_time_data(
    std::chrono::system_clock::time_point start_time,
    std::chrono::system_clock::time_point message_time) {
  // Set the time when this message was sent
  this->time = message_time;
  // Set the time since the start of the log
  this->elapsed_time = message_time - start_time;

  // Format the message time
  this->datetime = date::format("%F T %H:%M",
                                floor<std::chrono::milliseconds>(message_time));
  // Format the time since the start of the log
  this->into_session =
      date::format("%R", floor<std::chrono::milliseconds>(elapsed_time));
}

messages::message_body::message_body(std::string content) {
  this->content = std::move(content);
}

std::string messages::message_body::to_str() { return this->content; }

std::string messages::message_body::to_html() {
  std::regex unicodeRegex(R"(\\u([0-9a-fA-F]{4}))");
  // using &#xXXXX for the hex instead of the decimal notation
  std::string result =
      std::regex_replace(this->content, unicodeRegex, "&#x$1;");
  return result;
}

std::string messages::message_body::to_print() {
  // Set the string to the right encoding
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
  std::string result;
  // Loop over the characters in the message
  for (size_t i = 0; i < this->content.size(); ++i) {
    // If we find \u
    if (this->content[i] == '\\' && this->content[i + 1] == 'u') {
      // Extract the hex code
      std::string code = this->content.substr(i + 2, 4);
      // Cast it into the actual character
      auto c = static_cast<char32_t>(std::stoul(code, nullptr, 16));
      result += converter.to_bytes(c);
      // Move on, 5 for \u-formatted unicode
      i += 5;
    } else {
      // Move on, just copying this non-unicode character
      result += this->content[i];
    }
  }
  return result;
}

void messages::message_body::remove_continuation_marks() {
  std::list<std::regex> marks = {
      // Patterns to match all continuation marks that trigger is_continued
      // They're all a bit messy to remove spaces around the marks
      std::regex(R"(^ ?\. ?\. ?\. ?)"),
      std::regex(R"(^ ?\.\. ?)"),
      std::regex("^ ?… ?"),
      std::regex("^ ?- ?"),
      std::regex(R"( ?\. ?\. ?\. ?$)"),
      std::regex(R"( ?\.\. ?$)"),
      std::regex(" ?… ?$"),
      std::regex(" ?- ?$"),
      std::regex(" ?>> ?$"),
      // Pattern to match "(#/#)" at the end of a message
      std::regex(R"( ?\( ?\d+ ?\/ ?[\?\d]+ ?\) ?$)"),
  };

  std::string result = this->content;
  for (const auto &pattern : marks) {
    result = std::regex_replace(result, pattern, "");
  }

  this->content = result;
}
