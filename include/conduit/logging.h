#ifndef CNDT_LOGGING_H
#define CNDT_LOGGING_H

#include <string>

#include "conduit/core/logger.h"

namespace cndt::log {

// Application level logging functions
namespace app {

// Return the default application logger
Logger* getDefaultAppLogger();

// Set the logging level of the application logger
inline void setColor(bool enable_color) {
    getDefaultAppLogger()->SetColor(enable_color);
}

// Set the color output settings of the application logger
inline void setLoggingLevel(LogLevel log_level) {
    getDefaultAppLogger()->SetLoggingLevel(log_level);
}

// Set the time formatting string of the application logger
inline void setTimeFormatting(std::string time_fmt_str) {
    getDefaultAppLogger()->SetTimeFormatting(time_fmt_str);
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
    getDefaultAppLogger()->Trace(args...);
}

// Take a string and format it with the provided arguments
// and log is as an debug message on the application logger
template <typename... Args>
inline void debug(Args... args) {
    getDefaultAppLogger()->Debug(args...);
}

// Take a string and format it with the provided arguments
// and log is as an info message on the application logger
template <typename... Args>
inline void info(Args... args) {
    getDefaultAppLogger()->Info(args...);
}

// Take a string and format it with the provided arguments
// and log is as an warning message on the application logger
template <typename... Args>
inline void warn(Args... args) {
    getDefaultAppLogger()->Warn(args...);
}

// Take a string and format it with the provided arguments
// and log is as an error message on the application logger
template <typename... Args>
inline void error(Args... args) {
    getDefaultAppLogger()->Error(args...);
}

// Take a string and format it with the provided arguments
// and log is as an fatal message on the application logger
template <typename... Args>
inline void fatal(Args... args) {
    getDefaultAppLogger()->Fatal(args...);
}

} // namespace app

} // namespace cndt::log

#endif
