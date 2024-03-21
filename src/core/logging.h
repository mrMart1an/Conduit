#ifndef CNDT_LOGGING_CORE_H
#define CNDT_LOGGING_CORE_H

#include "conduit/core/logger.h"

namespace cndt::log {

namespace core {

// Return the default core logger
Logger* getDefaultCoreLogger();

// Set the logging level of the core logger
inline void setColor(bool enable_color) {
    getDefaultCoreLogger()->SetColor(enable_color);
}

// Set the color output settings of the core logger
inline void setLoggingLevel(LogLevel log_level) {
    getDefaultCoreLogger()->SetLoggingLevel(log_level);
}

// Set the time formatting string of the core logger
inline void setTimeFormatting(std::string time_fmt_str) {
    getDefaultCoreLogger()->SetTimeFormatting(time_fmt_str);
}

/*
 *
 *      Core logging functions
 *
 * */

// Take a string and format it with the provided arguments
// and log is as an trace message on the core logger
template <typename... Args>
void trace(Args... args) {
    getDefaultCoreLogger()->Trace(args...);
}

// Take a string and format it with the provided arguments
// and log is as an debug message on the core logger
template <typename... Args>
void debug(Args... args) {
    getDefaultCoreLogger()->Debug(args...);
}

// Take a string and format it with the provided arguments
// and log is as an info message on the core logger
template <typename... Args>
void info(Args... args) {
    getDefaultCoreLogger()->Info(args...);
}

// Take a string and format it with the provided arguments
// and log is as an warning message on the core logger
template <typename... Args>
void warn(Args... args) {
    getDefaultCoreLogger()->Warn(args...);
}

// Take a string and format it with the provided arguments
// and log is as an error message on the core logger
template <typename... Args>
void error(Args... args) {
    getDefaultCoreLogger()->Error(args...);
}

// Take a string and format it with the provided arguments
// and log is as an fatal message on the core logger
template <typename... Args>
void fatal(Args... args) {
    getDefaultCoreLogger()->Fatal(args...);
}

} // namespace core

} // namespace cndt::log

#endif
