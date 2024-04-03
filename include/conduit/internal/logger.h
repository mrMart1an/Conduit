#ifndef CNDT_LOGGER_H
#define CNDT_LOGGER_H

#include <iostream>
#include <string>
#include <format>
#include <string_view>

namespace cndt::log {

// Log message log level
enum class LogLevel {
    Trace = 0,
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
        std::string name,
        LogLevel log_lvl,
        std::string time_fmt_str = "%H:%M:%S",
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
    void log(LogLevel lvl, std::string_view fmt_str, Args... args) const;
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level trace
    template <typename... Args>
    void trace(Args... args) const { log(LogLevel::Trace, args...); };
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level debug
    template <typename... Args>
    void debug(Args... args) const { log(LogLevel::Debug, args...); };
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level info
    template <typename... Args>
    void info(Args... args) const { log(LogLevel::Info, args...); };
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level warning
    template <typename... Args>
    void warn(Args... args) const { log(LogLevel::Warning, args...); };
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level error
    template <typename... Args>
    void error(Args... args) const { log(LogLevel::Error, args...); };
    
    // Take a string and format it with the provided arguments
    // then log it to stdout with log level fatal
    template <typename... Args>
    void fatal(Args... args) const { log(LogLevel::Fatal, args...); };
    
private:
    // Return a string representation of the given logging level 
    std::string levelToString(LogLevel level) const;

    // Return a string containing the formatted current time
    std::string currentTimeString() const;
    
private:
    // The name of the logger used in the header
    std::string m_logger_name;
    
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
void Logger::log(LogLevel lvl, std::string_view fmt_str, Args... args) const {
    // Immediately return it the logging level is too low 
    if (lvl < m_log_level)
        return;

    // Get the logging level string and the time string
    std::string level_str = levelToString(lvl);
    std::string time_str = currentTimeString();

    // Print the log header
    std::cout << std::format(
        "{} => [{}]  [{}]: ",
        time_str,
        level_str,
        m_logger_name
    );

    // Format and print the message string
    std::format_args fmt_args = std::make_format_args(args...);
    std::cout << std::vformat(fmt_str, fmt_args) << std::endl;
}

} // namespace internal

} // namespace cndt::log

#endif
