// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef FF_RP_FORMATTER_RELATED_IMAGES_H
#define FF_RP_FORMATTER_RELATED_IMAGES_H

#include "../messages/messages.h"
#include <list>
#include <regex>
#include <string>

namespace related_images {

struct related_image {
public:
  related_image(std::string file_path, int related_message_id);

  // The message ID that the image is related to
  int related_message_id;

  // Method to format the encoded image into HTML
  std::string format();

private:
  std::string full_path;
  std::string file_name;

  // The base64 encoded image
  std::string encoded_image;

  // Method to encode the image into base64
  void encode_image();
};

struct structured_related_images {
public:
  explicit structured_related_images(std::list<related_image> images);

  structured_related_images() = default;

  std::list<related_image> images;
};

class related_images {
public:
  // Constructor, loads and parses the related images log file, then calls the
  // metadata methods
  related_images(const std::string &log_file_location,
                 const messages::structure &messages);

  related_images() = default;

  structured_related_images images;

  int related_images_found{0};
  int images_assigned_manually{0};
  int images_assigned_by_timestamp{0};
  int images_assigned_by_creation_time{0};
  int images_assigned_randomly{0};
  int images_pushed_down{0};

private:
  // Method to find the images of the discovered files
  std::list<std::string> find_images(const std::list<std::string> &files);

  // Method to get the message before a given a time point
  int get_message_by_time(std::chrono::system_clock::time_point time,
                          const messages::structure &messages);

  // Regex to find the timestamps that gshade, reshade, and the output of this
  // program use
  std::regex timestamp_regex = std::regex(
      R"((\d{4}).(\d{1,2}).(\d{1,2})[^\d]{1,}(\d{1,2}).(\d{1,2})[^.]?(\d{2})?)");

  // Method to relate the images to the messages based on timestamps in the
  // image name
  structured_related_images relate_images(std::list<std::string> images,
                                          const messages::structure &messages);
};

} // namespace related_images

#endif // FF_RP_FORMATTER_RELATED_IMAGES_H
