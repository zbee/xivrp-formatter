// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include "related_images.h"
#include "../includes/date.h"
#include "../includes/png-base64-webp.h"
#include <filesystem>
#include <iostream>
#include <utility>

namespace related_images {

related_images::related_images(const std::string &log_file_location,
                               messages::structure messages) {
  // Find folder of the log file
  std::filesystem::path log_file_path(log_file_location);
  this->log_folder = log_file_path.parent_path();

  // Find images next to the log file
  auto image_paths = this->find_images(this->find_files());

  // Find messages that are related to the images
  this->images = this->relate_images(image_paths, std::move(messages));
}

std::list<std::string> related_images::find_files() {
  std::list<std::string> files;

  // Find each file next to the log
  for (const auto &entry :
       std::filesystem::directory_iterator(this->log_folder))
    if (entry.is_regular_file())
      files.push_back(entry.path().string());

  return files;
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

std::chrono::system_clock::time_point
related_images::convertDateTimeString(const std::string &dateTimeString) {
  std::istringstream in{dateTimeString};
  std::chrono::system_clock::time_point timePoint;

  // Parse the datetime string into a time point, based on its formatting
  in >> date::parse("%FT%T%Ez", timePoint);

  return timePoint;
}

int related_images::get_message_by_time(
    std::chrono::system_clock::time_point time,
    const messages::structure &messages) {
  int last_message_id;

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

  for (const auto &image : images) {
    this->related_images_found++;

    //<editor-fold desc="File Metadata">
    std::smatch match;

    // Get the file name
    std::filesystem::path image_path(image);
    std::string file_name = image_path.filename().string();

    // Get the timestamp from the file name
    if (std::regex_search(file_name, match, this->timestamp_regex)) {
      // Build the date time string in one usable format
      dateTimeString = match[0].str() + "-" + match[1].str() + "-" +
                       match[2].str() + "T" + match[3].str() + ":" +
                       match[4].str();
      // Seconds are optional
      if (match.size() > 5)
        dateTimeString += ":" + match[5].str();

      // Save the time point an} how we got it
      timePoint = related_images::convertDateTimeString(dateTimeString);
      time_from_filename = true;

      // Remove the matched regex from the string
      file_name = std::regex_replace(file_name, this->timestamp_regex, "");
    }
    // Get the time point from the file creation date
    else {
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
      // Count the image as assigned by time
      if (time_from_filename) {
        this->images_assigned_by_timestamp++;
      } else {
        this->images_assigned_by_creation_time++;
      }

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
            break;
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
  this->full_path = image_path.string();

  // Get the file name
  this->file_name = image_path.filename().string();

  // Encode the image
  this->encode_image();
}

std::string related_image::format() {
  // Format the image into HTML
  std::string html = "<img src=\"data:image/webp;base64," +
                     this->encoded_image + R"(" class="message_picture">)";

  // Clear the encoded image
  this->encoded_image.clear();

  // Return the HTML
  return html;
}

void related_image::encode_image() {
  // Convert the full path into const chr*
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
  const char *full_path = this->full_path.c_str();
#pragma clang diagnostic pop

  // Encode the image into base64
  this->encoded_image = encodePNGToBase64(full_path);
}

} // namespace related_images
