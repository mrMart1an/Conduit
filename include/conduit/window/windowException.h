#ifndef CNDT_WINDOW_EXCEPTION_H
#define CNDT_WINDOW_EXCEPTION_H

#include "conduit/exception.h"

namespace cndt {

// Window generic exception
class WindowException : public Exception {
public:
    WindowException(std::string_view message) : Exception(message) { }
    WindowException() : Exception("Window exception") { }
};

// Window initialization exception
class WindowInitError : public WindowException {
public:
    WindowInitError(std::string_view message) : WindowException(message) { }
    WindowInitError() : WindowException("Window init exception") { }
};

} // namespace cndt

#endif
