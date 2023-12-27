// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#ifndef XIVRP_FORMATTER_LOG_SOURCE_H
#define XIVRP_FORMATTER_LOG_SOURCE_H

namespace log_source {

class log_source {
  // actual class for finding the appropriate parser, alias for parsing
  // messages/images, etc.

  // determine source of log
  // alias methods after the source is known
};

class Ilog_source {
  // interface for log_source

  // check if log is valid for this parser
  // load method
  // image finding method
  // etc.
};

} // namespace log_source

#endif // XIVRP_FORMATTER_LOG_SOURCE_H
