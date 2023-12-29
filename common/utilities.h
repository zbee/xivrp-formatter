// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef XIVRP_FORMATTER_UTILITIES_H
#define XIVRP_FORMATTER_UTILITIES_H

#include <chrono>
#include <list>
#include <string>

namespace common {

class utilities {
public:
  static std::string get_real_path(std::string &path);

  static bool check_file_exists(std::string file);

  static bool check_file_format(std::string file, const std::string &format);

  static bool check_hex_color(const std::string &color);

  static std::string select_first_n_words(const std::string &str, int n);

  static bool starts_with_any(const std::string &str,
                              const std::list<std::string> &arr);

  static bool ends_with_any(const std::string &str,
                            const std::list<std::string> &arr);

  static int count_words(const std::string &str);

  static std::chrono::system_clock::time_point
  convert_timestamp(const std::string &dateTimeString);

  static std::list<std::string>
  find_files_near(const std::string &path_to_file);
};

} // namespace common

#endif // XIVRP_FORMATTER_UTILITIES_H
