#ifndef CNDT_LOGGER_H
#define CNDT_LOGGER_H

#include <iostream>
#include <string>
#include <utility>

#include <fmt/core.h>
#include <fmt/format.h>

namespace cndt::log {

// Log message log level
enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};

namespace internal {

class Logger {
public:
    Logger(
        std::string_view name,
        LogLevel log_lvl = LogLevel::Trace,
        std::string_view time_fmt_str = "%H:%M:%S",
        bool enable_color = true
    );

    // Configuration functions
    
    // Set the status of the terminal color output
    inline void setColor(bool enable_color) {
        m_terminal_color = enable_color;
    }
    
    // Change the maximum logging level
    inline void setLoggingLevel(LogLevel log_lvl) {
        m_log_level = log_lvl;
    }

    // Change the time formatting string
    inline void setTimeFormatting(std::string time_fmt_str) {
        m_time_fmt = time_fmt_str;
    }
    
    // Logging functions
     
    // Take a string and format it with the provided arguments
    template <typename... Args>
    void log(
        LogLevel lvl, 
        fmt::format_string<Args...> fmt, 
        Args&&... args
    ) const;
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level trace
    template <typename... Args>
    void trace(fmt::format_string<Args...> fmt, Args&&... args) const 
    { 
        log(LogLevel::Trace, fmt, std::forward<Args>(args)...); 
    };
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level debug
    template <typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args&&... args) const 
    { 
        log(LogLevel::Debug, fmt, std::forward<Args>(args)...); 
    };
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level info
    template <typename... Args>
    void info(fmt::format_string<Args...> fmt, Args&&... args) const 
    { 
        log(LogLevel::Info, fmt, std::forward<Args>(args)...); 
    };
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level warning
    template <typename... Args>
    void warn(fmt::format_string<Args...> fmt, Args&&... args) const 
    { 
        log(LogLevel::Warning, fmt, std::forward<Args>(args)...); 
    };
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level error
    template <typename... Args>
    void error(fmt::format_string<Args...> fmt, Args&&... args) const 
    { 
        log(LogLevel::Error, fmt, std::forward<Args>(args)...);
    };
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level fatal
    template <typename... Args>
    void fatal(fmt::format_string<Args...> fmt, Args&&... args) const 
    { 
        log(LogLevel::Fatal, fmt, std::forward<Args>(args)...); 
    };
    
private:
    // Return a string representation of the given logging level 
    const std::string_view levelToString(LogLevel level) const;
    
    // Return a string containing the formatted current time
    std::string currentTimeString() const;
    
private:
    // The name of the logger used in the header
    std::string_view m_logger_name;
    
    // Time formatting string for the time formatting in the header
    std::string m_time_fmt;

    // Logger max logging level
    LogLevel m_log_level;    
    // Enable terminal color output
    bool m_terminal_color;
};

// Take a string and format it with the provided arguments, 
// then log it to stdout
template <typename... Args>
void Logger::log(
    LogLevel lvl, 
    fmt::format_string<Args...> fmt, 
    Args&&... args
) const {
    // Immediately return it the logging level is too low 
    if (lvl < m_log_level)
        return;

    // Get the logging level string and the time string
    const std::string_view level_str = levelToString(lvl);
    std::string time_str = currentTimeString();

    // Print the log header
    std::cout << fmt::format(
        "{} => [{}]  [{}]: ",
        time_str,
        level_str,
        m_logger_name
    );

    // Format and print the message string
    std::cout << fmt::format(fmt, std::forward<Args>(args)...) << std::endl;
}

} // namespace internal

} // namespace cndt::log

#endif
