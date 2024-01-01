// XIVRP-Formatter Copyright (C) 2024 Ethan Henderson <ethan@zbee.codes>
// Licensed under GPLv3 - Refer to the LICENSE file for the complete text

#include ".log_source.h"

namespace log_source {

/**
 * @brief Constructor for log files essentially; handles determining the
 * source of a log or finding the log files automatically to pick between, and
 * aliasing a the appropriate parser for the log
 * @param log_find_type The parser to use for the log file, or to us smart
 * find
 * @param log The path to the log file, if type is not smart
 * @return The source of the log file
 * @see settings::structure::settings_guide
 * @see settings::log_type
 */
log_source::log_source(settings::log_type log_find_type, std::string log) {}

/**
 * @brief Determine which source can handle a given log file
 * @see settings::structure::settings_guide
 * @see settings::structure::log_file_type
 */
enum source log_source::determine_source() { return source::discord; }

/**
 * @brief Find the most recent log files of every source, picking the most
 * recent from any, for the user to pick from if there's not an obvious
 * answer
 * @return A chosen log file path
 * @see log_source::Ilog_source::find_logs_of_this_source()
 */
std::string log_source::find_logs() { return std::string(); }

/**
 * @brief Parse messages from the found log file using the log file's source
 * @return A messages::structure object of the parsed log file
 */
messages::structure log_source::parse_messages() { return {}; }

} // namespace log_source
