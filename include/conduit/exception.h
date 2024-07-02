#ifndef CNDT_BASE_EXCEPTION_H
#define CNDT_BASE_EXCEPTION_H

#include <exception>
#include <string>
#include <string_view>

namespace cndt {

// Conduit generic exception
class Exception : public std::exception {
public:
    Exception(std::string_view message) : m_message(message) { }
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
