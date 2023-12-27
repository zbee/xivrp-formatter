// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include <iostream>
#include <windows.h>

#include "settings/settings.h"

#include "messages/loading.h"

#include "messages/messages.h"

#include "images/related_images.h"
#include "templating/templating.h"

using messages::load;
using settings::loader;
using settings::structure;

// 0 : success
// 1 : messages file not verified
// 2 : template file not verified
// 3 : messages file could not open
// 4 : template file could not open
// 5 : contents not provided to templator
// 6 : output file could not be created
int main() {
  std::cout << "XIVRP-Formatter Copyright (C) 2024" << std::endl << std::endl;

  //<editor-fold desc="Settings">
  // Get the user's settings
  settings::loader user = settings::loader();

  // Fail out if the settings were not suitable
  if (!user.log_verified) {
    std::cout << std::endl
              << "The log file could not be verified. Please try again."
              << std::endl;
    return 2;
  }
  if (!user.template_verified) {
    std::cout << std::endl
              << "The template file could not be verified. Please try again."
              << std::endl;
    return 3;
  }
  //</editor-fold>

  std::cout << std::endl << "---" << std::endl << std::endl;

  // Load the messages
  messages::load load(user.settings);
  // Save the loaded messages
  messages::structure messages = load.messages;

  int count;

  // Remove OOC messages if requested
  if (user.settings.remove_out_of_character) {
    std::cout << std::endl << "Removing OOC messages..." << std::endl;
    count = messages.remove_ooc();
    std::cout << "...Removed " << count << " messages." << std::endl;
  }

  if (user.settings.debug)
    messages.debug_print();

  // Combine messages that are continuations of others if requested
  if (user.settings.combine_messages) {
    std::cout << std::endl << "Combining messages..." << std::endl;
    count = messages.combine(user.settings.debug);
    std::cout << "...Combined " << count << " messages." << std::endl;
  }

  // Highlight emphatics if requested
  if (user.settings.highlight_emphatics) {
    std::cout << std::endl << "Highlighting emphatics..." << std::endl;
    count =
        messages.highlight_emphatics(user.settings.emphatic_highlight_color);
    std::cout << "...Highlighted " << count << " messages." << std::endl;
  }

  related_images::related_images related;

  // Find images, and relate them to messages if requested
  if (user.settings.find_related_images) {
    std::cout << std::endl << "Finding related images..." << std::endl;
    related =
        related_images::related_images(user.settings.log_file_path, messages);
    std::cout << "...Found " << related.related_images_found << " image"
              << (related.related_images_found == 1 ? "" : "s") << "."
              << std::endl;
    std::cout << "......" << related.images_assigned_manually
              << " manually related." << std::endl;
    std::cout << "......" << related.images_assigned_by_timestamp
              << " related by filename time." << std::endl;
    std::cout << "......" << related.images_assigned_by_creation_time
              << " related by creation time." << std::endl;
    std::cout << "..." << related.images_pushed_down
              << " were pushed down, to unrelated messages." << std::endl;
    std::cout << "......" << related.images_assigned_randomly
              << " related randomly." << std::endl;
  }

  // Format the messages
  std::map<std::string, std::string> formatted_messages;
  std::cout << std::endl << "Formatting messages..." << std::endl;
  if (user.settings.find_related_images)
    formatted_messages = messages.format(related.images);
  else
    formatted_messages = messages.format();

  // Fill the template with the messages
  std::cout << "Filling template..." << std::endl;
  templating::templator templator(user.settings.template_file_path);
  templator.content = formatted_messages;
  templator.fill_template(user.settings.output_file_path);

  // Done!
  std::cout << std::endl << "Done!" << std::endl;
  std::cout << "Output file written to " << user.settings.output_file_path
            << std::endl;

  // Open the output file
  std::filesystem::path absolute_path =
      std::filesystem::absolute(user.settings.output_file_path);
  std::string url = "file:///" + absolute_path.string();
  ShellExecute(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

  return 0;
}
