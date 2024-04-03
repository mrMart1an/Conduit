#ifndef CNDT_LOGGING_H
#define CNDT_LOGGING_H

#include <string>

#include "conduit/internal/logger.h"

namespace cndt::log {

// Application level logging functions
namespace app {

// Return the default application logger
internal::Logger* getDefaultAppLogger();

// Set the logging level of the application logger
inline void setColor(bool enable_color) {
    getDefaultAppLogger()->setColor(enable_color);
}

// Set the color output settings of the application logger
inline void setLoggingLevel(LogLevel log_level) {
    getDefaultAppLogger()->setLoggingLevel(log_level);
}

// Set the time formatting string of the application logger
inline void setTimeFormatting(std::string time_fmt_str) {
    getDefaultAppLogger()->setTimeFormatting(time_fmt_str);
}

/*
 *
 *      Application logging functions
 *
 * */

// Take a string and format it with the provided arguments
// and log is as an trace message on the application logger
template <typename... Args>
inline void trace(Args... args) {
    getDefaultAppLogger()->trace(args...);
}

// Take a string and format it with the provided arguments
// and log is as an debug message on the application logger
template <typename... Args>
inline void debug(Args... args) {
    getDefaultAppLogger()->debug(args...);
}

// Take a string and format it with the provided arguments
// and log is as an info message on the application logger
template <typename... Args>
inline void info(Args... args) {
    getDefaultAppLogger()->info(args...);
}

// Take a string and format it with the provided arguments
// and log is as an warning message on the application logger
template <typename... Args>
inline void warn(Args... args) {
    getDefaultAppLogger()->warn(args...);
}

// Take a string and format it with the provided arguments
// and log is as an error message on the application logger
template <typename... Args>
inline void error(Args... args) {
    getDefaultAppLogger()->error(args...);
}

// Take a string and format it with the provided arguments
// and log is as an fatal message on the application logger
template <typename... Args>
inline void fatal(Args... args) {
    getDefaultAppLogger()->fatal(args...);
}

} // namespace app
 
namespace core {

// Return the default core logger
internal::Logger* getDefaultCoreLogger();

// Set the logging level of the core logger
inline void setColor(bool enable_color) {
    getDefaultCoreLogger()->setColor(enable_color);
}

// Set the color output settings of the core logger
inline void setLoggingLevel(LogLevel log_level) {
    getDefaultCoreLogger()->setLoggingLevel(log_level);
}

// Set the time formatting string of the core logger
inline void setTimeFormatting(std::string time_fmt_str) {
    getDefaultCoreLogger()->setTimeFormatting(time_fmt_str);
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
    getDefaultCoreLogger()->trace(args...);
}

// Take a string and format it with the provided arguments
// and log is as an debug message on the core logger
template <typename... Args>
void debug(Args... args) {
    getDefaultCoreLogger()->debug(args...);
}

// Take a string and format it with the provided arguments
// and log is as an info message on the core logger
template <typename... Args>
void info(Args... args) {
    getDefaultCoreLogger()->info(args...);
}

// Take a string and format it with the provided arguments
// and log is as an warning message on the core logger
template <typename... Args>
void warn(Args... args) {
    getDefaultCoreLogger()->warn(args...);
}

// Take a string and format it with the provided arguments
// and log is as an error message on the core logger
template <typename... Args>
void error(Args... args) {
    getDefaultCoreLogger()->error(args...);
}

// Take a string and format it with the provided arguments
// and log is as an fatal message on the core logger
template <typename... Args>
void fatal(Args... args) {
    getDefaultCoreLogger()->fatal(args...);
}

} // namespace core

} // namespace cndt::log

#endif
