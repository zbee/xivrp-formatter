// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "related_images.h"
#include "../includes/png-base64-webp.h"
#include <iostream>
#include <utility>

namespace related_images {

related_images::related_images(const std::string &log_file_location,
                               messages::structure messages) {}

std::list<std::string> related_images::find_files() {
  return std::list<std::string>();
}

std::list<std::string>
related_images::find_images(std::list<std::string> files) {
  return std::list<std::string>();
}

std::list<int> related_images::relate_images(std::list<std::string> images,
                                             messages::structure messages) {}

structured_related_images::structured_related_images(
    std::list<related_image> images) {
  this->images = std::move(images);
}

related_image::related_image(std::string file_path) {}

std::string related_image::format() { return {}; }

void related_image::encode_image() {}

std::string related_image::get_time_string() { return {}; }

std::chrono::system_clock::time_point
related_image::convertDateTimeString(const std::string &dateTimeString) {
  return {};
}

} // namespace related_images
