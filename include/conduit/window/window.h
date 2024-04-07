#ifndef CNDT_WINDOW_H
#define CNDT_WINDOW_H

#include "conduit/defines.h"

namespace cndt {

class Application;

// Conduit window cross platform interface
class Window {
    friend class Application;
    
public:
    // Window data storing position and size
    struct Data {
        i32 xpos, ypos;      
        i32 width, height;      
    };
    
protected:
    // Window configuration struct
    struct Config {
        // Window title
        const char* title;

        // Window dimension
        i32 width, height;

        // The window will always float on top of other window
        bool floating;
        // The window will be resizable by the user
        bool resizable;
        // Ignore the Provided width and height and make the window fullscreen
        bool fullscreen;
    };
    
public:
    virtual ~Window() = default;
    
    // Obtain the current window data 
    virtual Data getWindowData() = 0;

    // Toggle the full screen mode of the window
    virtual void toggleFullscreen() = 0;
    // Set the full screen mode of the window
    virtual void setFullscreen(bool fullscreen) = 0;

    // Capture the cursor and enable raw input if available
    virtual void captureCursor() = 0;
    // Release the cursor and disable raw input if available
    virtual void releaseCursor() = 0;

protected:
    // Pool the window event and send them to the event bus
    virtual void poolEvents() = 0;
};

/*
 *
 *      Window exception type
 *
 * */

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
