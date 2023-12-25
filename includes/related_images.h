/*
 * XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
 * Licensed under GPLv3 - Refer to the LICENSE file for the complete text
 */

#ifndef FF_RP_FORMATTER_RELATED_IMAGES_H
#define FF_RP_FORMATTER_RELATED_IMAGES_H

#include "messages/messages.h"
#include <regex>
#include <string>

namespace related_images {

struct related_image {
public:
  explicit related_image(std::string file_path);

  // The message ID that the image is related to
  int related_message_id;

  // Method to format the encoded image into HTML
  std::string format();

private:
  std::string full_path;
  std::string file_name;

  // Regex to find the timestamp that gshade and reshade use
  std::regex timestamp_regex =
      std::regex(R"(\d{4}).(\d{2}).(\d{2})..?(\d{1,2}).(\d{1,2}).(\d{2})");

  // Timestamp of the image from the file name
  std::chrono::system_clock::time_point timestamp;

  // The base64 encoded image
  std::string encoded_image;

  // Method to encode the image into base64
  void encode_image();

  // Method to get the timestamp from the file name
  std::string get_time_string();

  // Method to convert a date/time string into a time_point
  static std::chrono::system_clock::time_point
  convertDateTimeString(const std::string &dateTimeString);
};

struct structured_related_images {
public:
  structured_related_images();

  std::list<related_image> images;
};

class related_images {
public:
  // Constructor, loads and parses the related images log file, then calls the
  // metadata methods
  related_images(const std::string &log_file_location,
                 messages::structure messages);

  structured_related_images images;

private:
  // Derived path to the log file
  std::string log_folder;

  // Method to find the files in the log folder
  std::list<std::string> find_files();

  // Method to find the images of the discovered files
  std::list<std::string> find_images(std::list<std::string> files);

  // Method to relate the images to the messages based on timestamps in the
  // image name
  void relate_images(std::list<std::string> images,
                     messages::structure messages);
};

} // namespace related_images

#endif // FF_RP_FORMATTER_RELATED_IMAGES_H
