// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "related_images.h"
#include <iostream>

namespace related_images {

related_images::related_images(const std::string &log_file_location,
                               messages::structure messages) {
  std::cout << "Loading related images ..." << std::endl;
}

std::list<std::string> related_images::find_files() {
  return std::list<std::string>();
}

std::list<std::string>
related_images::find_images(std::list<std::string> files) {
  return std::list<std::string>();
}

void related_images::relate_images(std::list<std::string> images,
                                   messages::structure messages) {}
} // namespace related_images
