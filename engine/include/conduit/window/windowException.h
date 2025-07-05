#ifndef CNDT_WINDOW_EXCEPTION_H
#define CNDT_WINDOW_EXCEPTION_H

#include "conduit/exception.h"

#include <fmt/base.h>
#include <utility>

namespace cndt {

// Window generic exception
class WindowException : public Exception {
public:
    template<typename... Args>
    WindowException(fmt::format_string<Args...> msg, Args&&... args) 
    : 
        Exception(msg, std::forward<Args>(args)...) 
    { }
};

// Window initialization exception
class WindowInitError : public WindowException {
public:
    template<typename... Args>
    WindowInitError(fmt::format_string<Args...> msg, Args&&... args) 
    :
        WindowException(msg, std::forward<Args>(args)...) 
    { }
};

} // namespace cndt

#endif
