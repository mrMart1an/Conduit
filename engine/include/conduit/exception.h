#ifndef CNDT_BASE_EXCEPTION_H
#define CNDT_BASE_EXCEPTION_H

#include <exception>
#include <string>
#include <utility>

#include <fmt/format.h>

namespace cndt {

// Conduit generic exception
class Exception : public std::exception {
public:
    template<typename... Args>
    Exception(fmt::format_string<Args...> msg, Args&&... args) : 
        m_message() 
    { 
        try {
            m_message = fmt::format(msg, std::forward<Args>(args)...);
        } catch (...) { 
            // If an exception is throw in the format function  
            // store a replacment string
            m_message = "Exception in exception formater!";
        }
    }
    Exception() : m_message("Conduit exception") { }
    
    virtual const char* what() const throw() 
    {
        return m_message.c_str();   
    }
    
private:
    // Store the exception message
    std::string m_message;
};

} // namespace cndt

#endif
