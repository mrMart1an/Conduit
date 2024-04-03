#include "conduit/defines.h"

#include <array>
#include <ctime>
#include <string>

#include "conduit/logging.h"
#include "conduit/internal/logger.h"

// Text representation of the log levels
static constexpr std::array<const char*, 12> log_level_str = {
    "TRACE", 
    "DEBUG", 
    "INFO ",
    "WARN ",
    "ERROR",
    "FATAL",

    // ANSI Color text
    "\033[1;36mTRACE\033[0m", 
    "\033[1;34mDEBUG\033[0m", 
    "\033[1;32mINFO\033[0m ",
    "\033[1;33mWARN\033[0m ",
    "\033[1;31mERROR\033[0m",
    "\033[1;31mFATAL\033[0m",
};
static constexpr cndt::usize log_level_str_count = log_level_str.size() / 2;

namespace cndt::log {

/*
 *
 *      Global loggers variables and functions definitions
 *
 * */

// Application logger static global variables
static internal::Logger g_app_logger("app", LogLevel::Trace);
// Application logger static global variables
static internal::Logger g_core_logger("core", LogLevel::Trace);

// Return the default application logger
internal::Logger* app::getDefaultAppLogger() {
    return &g_app_logger;
}

// Return the default conduit core logger
internal::Logger* core::getDefaultCoreLogger() {
    return &g_core_logger;
}

namespace internal {

/*
 *
 *      Logger functions definitions
 *
 * */

// Logger constructor
Logger::Logger(
    std::string name,
    LogLevel log_level,
    std::string time_fmt_str,
    bool enable_color
) :
    m_logger_name(name), 
    m_time_fmt(time_fmt_str),
    m_log_level(log_level),
    m_terminal_color(enable_color)
{ }

// Return a string representation of the logging level 
std::string Logger::levelToString(LogLevel level) const {
    const usize color_str_offset = m_terminal_color ? log_level_str_count : 0;
    const usize str_index = static_cast<int>(level) + (color_str_offset); 
    
    const char* level_str = log_level_str[str_index];

    return level_str;
}

// Return a string containing the formatted current time
std::string Logger::currentTimeString() const {
    char time_str[128];

    time_t now = time(NULL);

    // Format the time string
    strftime(
        time_str,
        128,
        m_time_fmt.c_str(),
        localtime(&now)
    );
    
    return time_str;
}

} // namespace internal

} // namespace cndt::log
