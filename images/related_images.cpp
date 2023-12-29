// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "related_images.h"
#include "../common/utilities.h"
#include "../includes/base64.hpp"
#include "../includes/date.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

namespace related_images {

related_images::related_images(const std::string &log_file_location,
                               const messages::structure &messages) {
  // Find files near log
  auto nearby_files = common::utilities::find_files_near(log_file_location);

  // Find images next to the log file
  auto image_paths = this->find_images(nearby_files);

  // Find messages that are related to the images
  this->images = this->relate_images(image_paths, messages);
}

std::list<std::string>
related_images::find_images( // NOLINT(*-convert-member-functions-to-static)
    const std::list<std::string> &files) {
  std::list<std::string> image_paths;

  // Find each image in the list of files
  for (const auto &file : files)
    if (file.ends_with(".png"))
      image_paths.push_back(file);

  return image_paths;
}

int related_images::get_message_by_time(
    std::chrono::system_clock::time_point time,
    const messages::structure &messages) {
  int last_message_id = -1;

  // Iterate over each message, returning the last message's ID once a message
  // is beyond the given time
  for (const auto &message : messages.messages) {
    if (message.time >= time)
      return last_message_id;
    last_message_id = message.id;
  }

  return -1;
}

structured_related_images
related_images::relate_images(std::list<std::string> images,
                              const messages::structure &messages) {
  std::list<related_image> related_images;
  std::list<int> message_ids_with_images;

  std::string dateTimeString;
  std::chrono::system_clock::time_point timePoint;

  bool time_from_filename;
  bool timestampFound;

  for (const auto &image : images) {
    this->related_images_found++;

    //<editor-fold desc="File Metadata">
    std::smatch match;
    timestampFound = false;

    // Get the file name
    std::filesystem::path image_path(image);
    std::string file_name = image_path.filename().string();

    // Get the timestamp from the file name
    if (std::regex_search(file_name, match, this->timestamp_regex)) {
      // Build the date time string in one usable format
      dateTimeString = match[1].str() + "-" + match[2].str() + "-" +
                       match[3].str() + "T" + match[4].str() + ":" +
                       match[5].str();
      // Seconds are optional in the timestamp but not for the conversion
      if (match.size() > 5)
        dateTimeString += ":" + match[6].str();
      else
        dateTimeString += ":00";
      // Add "the timezone"
      dateTimeString += "-00:00";

      // Convert the formed timestamp
      timePoint = common::utilities::convert_timestamp(dateTimeString);

      // Check if the timestamp is valid
      std::time_t tt = std::chrono::system_clock::to_time_t(timePoint);
      std::tm *tm = std::localtime(&tt);
      if (tm->tm_year != 70) { // Checking if the year changed from default 1970
        // Save the time point and how we got it
        timestampFound = true;
        time_from_filename = true;
        // Remove the matched regex from the string
        file_name = std::regex_replace(file_name, this->timestamp_regex, "");
      }
    }

    // Get the time point from the file creation date
    if (!timestampFound) {
      // Get the file creation time
      auto fileTime = std::filesystem::last_write_time(image_path);
      // Save the time point and how we got it
      timePoint =
          std::chrono::time_point_cast<std::chrono::system_clock::duration>(
              fileTime - std::filesystem::file_time_type::clock::now() +
              std::chrono::system_clock::now());
      time_from_filename = false;
    }
    //</editor-fold>

    // If the image starts with as many as 4 digits (manually labeled image)
    if (std::isdigit(file_name[0])) {
      std::string message_id_string = file_name.substr(0, 1);

      // Add the second digit (onward) if it exists
      if (std::isdigit(file_name[1])) {
        message_id_string += file_name.substr(1, 1);

        // Add the third digit if it exists
        if (std::isdigit(file_name[2])) {
          message_id_string += file_name.substr(2, 1);

          // Add the fourth digit if it exists
          if (std::isdigit(file_name[3])) {
            message_id_string += file_name.substr(3, 1);
          }
        }
      }

      // Get the message ID
      int message_id = std::stoi(message_id_string);

      // Make sure it is one image per message
      if (std::find(message_ids_with_images.begin(),
                    message_ids_with_images.end(),
                    message_id) != message_ids_with_images.end()) {
        if (!message_ids_with_images.empty()) {
          message_id = message_ids_with_images.back() + 1;
          this->images_pushed_down++;
        }
      }

      // Save the message ID we're using
      message_ids_with_images.push_back(message_id);

      // Add the related image to the list
      related_images.emplace_back(image, message_id);

      this->images_assigned_manually++;
    }
    // If the image is related to a message by time
    else {
      // Get the message ID
      int message_id = this->get_message_by_time(timePoint, messages);

      // If the image date does not fit to a message, use the first message
      // without an image
      if (message_id == -1) {
        for (const auto &message : messages.messages)
          if (std::find(message_ids_with_images.begin(),
                        message_ids_with_images.end(),
                        message.id) == message_ids_with_images.end()) {
            message_id = message.id;
            // Count the image as assigned randomly
            this->images_assigned_randomly++;
            break;
          }
      }
      // Count the image as assigned by time
      else {
        if (time_from_filename) {
          this->images_assigned_by_timestamp++;
        } else {
          this->images_assigned_by_creation_time++;
        }
      }

      // Make sure it is one image per message
      if (std::find(message_ids_with_images.begin(),
                    message_ids_with_images.end(),
                    message_id) != message_ids_with_images.end()) {
        if (!message_ids_with_images.empty()) {
          message_id = message_ids_with_images.back() + 1;
          this->images_pushed_down++;
        }
      }

      // Save the message ID we're using
      message_ids_with_images.push_back(message_id);

      // Add the related image to the list
      related_images.emplace_back(image, message_id);
    }
  }

  return structured_related_images(related_images);
}

structured_related_images::structured_related_images(
    std::list<related_image> images) {
  this->images = std::move(images);
}

related_image::related_image(std::string file_path, int related_message_id) {
  this->related_message_id = related_message_id;

  // Get the full path
  std::filesystem::path image_path(file_path);
  std::filesystem::path absolute_path = std::filesystem::absolute(image_path);
  this->full_path = absolute_path.string();

  // Get the file name
  this->file_name = image_path.filename().string();

  // Encode the image
  this->encode_image();
}

std::string related_image::format() {
  // Format the image into HTML
  std::string html = R"(<div class="message_picture"><img alt=")" +
                     this->file_name + ", " +
                     std::to_string(this->related_message_id) +
                     "\" src=\"data:image/png;"
                     "base64," +
                     this->encoded_image + "\"/ ></div>";

  // Clear the encoded image
  this->encoded_image.clear();

  // Return the HTML
  return "<div></div>" + html + "<div></div>";
}

void related_image::encode_image() {
  // Open the file in binary mode
  std::ifstream file(this->full_path, std::ios::binary);

  // Read the file into a string
  std::string contents((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

  // Encode the string into base64
  this->encoded_image = code::base64_encode(
      reinterpret_cast<const unsigned char *>(contents.c_str()),
      contents.length());
}

} // namespace related_images
