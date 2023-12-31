// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "utilities.h"
#include "../includes/date.h"
#include "../settings/ask.h"
#include <filesystem>
#include <regex>

namespace common {

//<editor-fold desc="File Utilities">
std::string common::utilities::get_real_path(std::string &path) {
  // Check if the path is empty
  if (path.empty())
    return "";

  // Check if the file path exists
  if (std::filesystem::exists(path))
    return path;

  // Check if variations on the path exist
  if (std::filesystem::exists(".." + path))
    return ".." + path;
  if (std::filesystem::exists("../" + path))
    return "../" + path;
  if (std::filesystem::exists("../../" + path))
    return "../../" + path;

  // If none of these exist
  return "";
}

bool common::utilities::check_file_exists(std::string file) {
  // Check the file exists, or any variations of it
  return !common::utilities::get_real_path(file).empty();
}

bool common::utilities::check_file_format(const std::string &file,
                                          const std::string &format) {
  // Check if the file name is shorter than the required format
  if (file.length() < format.length())
    return false;

  // Check the file extension is the correct format
  std::string file_type = file.substr(file.length() - format.length());

  return file_type == format;
}

std::list<std::string>
common::utilities::find_files_near(const std::string &path_to_file) {
  std::filesystem::path path(path_to_file);
  std::string folder = path.parent_path().string();
  std::list<std::string> files;

  // Find each file next to the log
  for (const auto &entry : std::filesystem::directory_iterator(folder))
    if (entry.is_regular_file())
      files.push_back(entry.path().string());

  return files;
}
//</editor-fold>

//<editor-fold desc="String Utilities">
std::string common::utilities::select_first_n_words(const std::string &str,
                                                    int n) {
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

bool common::utilities::starts_with_any( // NOLINT(*-no-recursion)
    const std::string &str, const std::list<std::string> &arr) {

  // Check that the string is longer than the array items
  for (const auto &prefix : arr)
    if (prefix.size() >= str.size())
      return false;

  // Early returns, for FILLER items
  if (str == "FILLER")
    return false;

  // Search each element in the array, checking if it is the prefix of the
  // string
  bool has_prefix =
      std::any_of(arr.begin(), arr.end(), [&str](const std::string &prefix) {
        // Don't even check the string for this prefix if the prefix is longer
        if (prefix.size() > str.size())
          return false;
        // Check if the prefix is the beginning of the string
        return std::equal(prefix.begin(), prefix.end(), str.begin());
      });

  if (has_prefix) {
    // If the string ends with any of the prefixes, return true
    return true;
  } else if (str.find(' ') != std::string::npos) {
    // If the string does not start with any of the prefixes, return the result
    // of trying again but with no spaces in the string
    std::string no_space = str;
    no_space.erase(std::remove(no_space.begin(), no_space.end(), ' '),
                   no_space.end());
    return starts_with_any(no_space, arr);
  } else {
    // If the string does not end with any of the prefixes, and we already tried
    // removing the spaces, return false
    return false;
  }
}

bool common::utilities::ends_with_any( // NOLINT(*-no-recursion)
    const std::string &str, const std::list<std::string> &arr) {

  // Check that the string is longer than the array items
  for (const auto &suffix : arr)
    if (suffix.size() >= str.size())
      return false;

  // Early returns, for FILLER items
  if (str == "FILLER")
    return false;

  // Search each element in the array, checking if it is the suffix of the
  // string
  bool has_suffix =
      std::any_of(arr.begin(), arr.end(), [&str](const std::string &suffix) {
        // Don't even check the string for this suffix if the suffix is
        // longer
        if (suffix.size() > str.size())
          return false;
        // Check if the suffix is the end of the string
        return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
      });

  if (has_suffix) {
    // If the string ends with any of the suffixes, return true
    return true;
  } else if (str.find(' ') != std::string::npos) {
    // If the string does not end with any of the suffixes, return the
    // result of trying again but with no spaces in the string
    std::string no_space = str;
    no_space.erase(std::remove(no_space.begin(), no_space.end(), ' '),
                   no_space.end());
    return starts_with_any(no_space, arr);
  } else {
    // If the string does not end with any of the suffixes, and we already
    // tried removing the spaces, return false
    return false;
  }
}

int common::utilities::count_words(const std::string &str) {
  std::stringstream ss(str);
  std::string word;
  int numWords = 0;

  // Count the number of words in the string
  while (ss >> word) {
    numWords++;
  }

  // Return the number of words
  return numWords;
}
//</editor-fold>

bool common::utilities::check_hex_color(const std::string &color) {
  // Pattern for a hex_color color, 6 or 3 characters long
  std::regex hex_color_pattern{"^#([a-fA-F0-9]{6}|[a-fA-F0-9]{3})$"};

  // Check that the color is a valid hex_color color string
  bool matches = std::regex_match(color, hex_color_pattern);
  bool matches_with_hash = std::regex_match("#" + color, hex_color_pattern);

  // Return true if it's a valid hex_color color, or is with a hash sign added
  return matches || matches_with_hash;
}

std::chrono::system_clock::time_point
common::utilities::convert_timestamp(const std::string &dateTimeString) {
  std::istringstream in{dateTimeString};
  std::chrono::system_clock::time_point timePoint;

  // Parse the datetime string into a time point, based on its formatting
  in >> date::parse("%FT%T%Ez", timePoint);
  // Ignore an error above. It's not real, and only even shows with MinGW.

  return timePoint;
}

} // namespace common
