// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include <iostream>

#include "settings/settings.h"

#include "messages/loading.h"

#include "messages/messages.h"

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

  int count = 0;

  // Remove OOC messages if requested
  if (user.settings.remove_out_of_character) {
    std::cout << std::endl << "Removing OOC messages..." << std::endl;
    count = messages.remove_ooc();
    std::cout << "...Removed " << count << " messages." << std::endl;
  }

  // messages.debug_print();

  // Combine messages that are continuations of others if requested
  if (user.settings.combine_messages) {
    std::cout << std::endl << "Combining messages..." << std::endl;
    count = messages.combine(false);
    std::cout << "...Combined " << count << " messages." << std::endl;
  }

  // Highlight emphatics if requested
  if (user.settings.highlight_emphatics) {
    std::cout << std::endl << "Highlighting emphatics..." << std::endl;
    count =
        messages.highlight_emphatics(user.settings.emphatic_highlight_color);
    std::cout << "...Highlighted " << count << " messages." << std::endl;
  }

  // Format the messages
  std::cout << std::endl << "Formatting messages..." << std::endl;
  auto formatted_messages = messages.format();

  // Fill the template with the messages
  std::cout << "Filling template..." << std::endl;
  templating::templator templator(user.settings.template_file_path);
  templator.content = formatted_messages;
  templator.fill_template(user.settings.output_file_path);

  // Done!
  std::cout << std::endl << "Done!" << std::endl;
  std::cout << "Output file written to " << user.settings.output_file_path
            << std::endl;

  return 0;
}
