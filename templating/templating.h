// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef TEMPLATING_H
#define TEMPLATING_H

#include <map>
#include <string>

namespace templating {

std::string const AUTHORS_TAG = "{{ authors }}";
std::string const METADATA_TAG = "{{ metadata }}";
std::string const MESSAGES_TAG = "{{ messages }}";

std::string const AUTHORS_DATA = "authors";
std::string const METADATA_DATA = "metadata";
std::string const MESSAGES_DATA = "messages";

class templator {
public:
  explicit templator(const std::string &template_file);

  void fill_template(const std::string &output_file);

  std::map<std::string, std::string> content;

private:
  std::string template_file_contents;
  std::string output_file_contents;

  void fill_tag(const std::string &tag, const std::string &content);
};

} // namespace templating

#endif // TEMPLATING_H
