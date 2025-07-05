#ifndef CNDT_LOGGING_H
#define CNDT_LOGGING_H

#include <string>

#include "conduit/internal/logger.h"

#include <fmt/base.h>

#include "buildConfig.h"

namespace cndt::log {

// Compile time log level
constexpr int compile_log_level = CNDT_CORE_LOGGER_LEVEL;

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
inline void trace(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level == 0)
        getDefaultAppLogger()->trace(fmt, std::forward<Args>(args)...);
}

// Take a string and format it with the provided arguments
// and log is as an debug message on the application logger
template <typename... Args>
inline void debug(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level <= 1)
        getDefaultAppLogger()->debug(fmt, std::forward<Args>(args)...);
}

// Take a string and format it with the provided arguments
// and log is as an info message on the application logger
template <typename... Args>
inline void info(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level <= 2)
        getDefaultAppLogger()->info(fmt, std::forward<Args>(args)...);
}

// Take a string and format it with the provided arguments
// and log is as an warning message on the application logger
template <typename... Args>
inline void warn(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level <= 3)
        getDefaultAppLogger()->warn(fmt, std::forward<Args>(args)...);
}

// Take a string and format it with the provided arguments
// and log is as an error message on the application logger
template <typename... Args>
inline void error(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level <= 4)
        getDefaultAppLogger()->error(fmt, std::forward<Args>(args)...);
}

// Take a string and format it with the provided arguments
// and log is as an fatal message on the application logger
template <typename... Args>
inline void fatal(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level <= 5)
        getDefaultAppLogger()->fatal(fmt, std::forward<Args>(args)...);
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
void trace(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level == 0)
        getDefaultCoreLogger()->trace(fmt, std::forward<Args>(args)...);
}

// Take a string and format it with the provided arguments
// and log is as an debug message on the core logger
template <typename... Args>
void debug(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level <= 1)
        getDefaultCoreLogger()->debug(fmt, std::forward<Args>(args)...);
}

// Take a string and format it with the provided arguments
// and log is as an info message on the core logger
template <typename... Args>
void info(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level <= 2)
        getDefaultCoreLogger()->info(fmt, std::forward<Args>(args)...);
}

// Take a string and format it with the provided arguments
// and log is as an warning message on the core logger
template <typename... Args>
void warn(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level <= 3)
        getDefaultCoreLogger()->warn(fmt, std::forward<Args>(args)...);
}

// Take a string and format it with the provided arguments
// and log is as an error message on the core logger
template <typename... Args>
void error(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level <= 4)
        getDefaultCoreLogger()->error(fmt, std::forward<Args>(args)...);
}

// Take a string and format it with the provided arguments
// and log is as an fatal message on the core logger
template <typename... Args>
void fatal(fmt::format_string<Args...> fmt, Args&&... args) 
{
    if constexpr (compile_log_level <= 5)
        getDefaultCoreLogger()->fatal(fmt, std::forward<Args>(args)...);
}

} // namespace core

} // namespace cndt::log

#endif
