/*
 * XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
 * Licensed under GPLv3 - Refer to the LICENSE file for the complete text
 */

#include "templating.h"
#include <fstream>
#include <iostream>

templating::templator::templator(const std::string &template_file) {
  // Try to open the template file
  try {
    std::ifstream template_file_stream(template_file);

    if (!template_file_stream.is_open()) {
      throw std::exception();
    }

    // Read the template file into the template file contents
    std::string line;
    while (std::getline(template_file_stream, line)) {
      this->template_file_contents += line + "\n";
    }

    template_file_stream.close();
  } catch (std::exception &e) {
    std::cout << "...Could not open template file!" << std::endl;
    exit(4);
  }
}

void templating::templator::fill_template(const std::string &output_file) {
  // Fail out it if no template-fill was provided
  if (content.empty()) {
    std::cout << "...No content to fill template with!" << std::endl;
    exit(5);
  }

  // Create or overwrite the output file
  try {
    std::ofstream output(output_file);

    if (!output.is_open()) {
      throw std::exception();
    }

    output.close();
  } catch (std::exception &e) {
    std::cout << "...Could not create output file!" << std::endl;
    exit(6);
  }

  // Fill all the tags in the template file
  this->fill_tag(AUTHORS_TAG, content[AUTHORS_DATA]);
  this->fill_tag(METADATA_TAG, content[METADATA_DATA]);
  this->fill_tag(MESSAGES_TAG, content[MESSAGES_DATA]);

  // Try to write the output file
  try {
    std::ofstream output(output_file);
    output << this->output_file_contents; // Filled from fill_tag()
    output.close();
    std::cout << "...Output file written!" << std::endl;
  } catch (std::exception &e) {
    std::cout << "...Could not write to output file!" << std::endl;
    exit(6);
  }
}

void templating::templator::fill_tag(const std::string &tag,
                                     const std::string &content) {
  // Fill the output file contents with the template file contents the first
  // time
  if (this->output_file_contents.empty()) {
    this->output_file_contents = this->template_file_contents;
  }

  // Find the tag in the output file contents
  std::size_t tag_location = this->output_file_contents.find(tag);

  // Replace the tag with the content
  this->output_file_contents.replace(tag_location, tag.length(), content);
}
