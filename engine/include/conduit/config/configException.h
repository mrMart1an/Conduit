#ifndef CNDT_CONFIG_EXCEPTION_H
#define CNDT_CONFIG_EXCEPTION_H

#include "conduit/exception.h"

namespace cndt {

// General config exception
class ConfigException : public Exception {
public:
    ConfigException() : Exception("Unknow config exception") { }      

    template<typename... Args>
    ConfigException(fmt::format_string<Args...> msg, Args&&... args) 
    : Exception(msg, std::forward<Args>(args)...) { }      
};

// Config file not found exception
class ConfigFileNotFound : public ConfigException {
public:
    template<typename... Args>
    ConfigFileNotFound(fmt::format_string<Args...> msg, Args&&... args) 
    : ConfigException(msg, std::forward<Args>(args)...) { }      
};

// Config file parse exception
class ConfigParseError : public ConfigException {
public:
    template<typename... Args>
    ConfigParseError(fmt::format_string<Args...> msg, Args&&... args) 
    : ConfigException(msg, std::forward<Args>(args)...) { }      
};

} // namespace cndt

#endif
