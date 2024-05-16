#ifndef CNDT_WINDOW_H
#define CNDT_WINDOW_H

#include "conduit/defines.h"

#include <string>
#include <vector>

// Define the vulkan surface type without importing the header
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
// Define the vulkan instance type without importing the header
typedef struct VkInstance_T* VkInstance;
// Define the vulkan allocation callbacks type without importing the header
typedef struct VkAllocationCallbacks VkAllocationCallbacks ;

namespace cndt {

class Application;

// Conduit window cross platform interface
class Window {
    friend class Application;
    
public:
    // Window data storing position and size
    struct Data {
        i32 x_pos, y_pos;      
        
        // Window size in screen coordinate
        i32 window_width, window_height;      
        // Frame buffer size in pixels
        i32 buffer_width, buffer_height;      
    };
    
protected:
    // Window configuration struct
    struct Config {
        Config(
            std::string_view title,
            i32 width = 800, i32 height = 600,
            bool fullscreen = false,
            bool resizable = false,
            bool floating = false
        ) : 
            title(title),
            width(width), height(height),
            fullscreen(fullscreen),
            resizable(resizable),
            floating(floating)
        { }
        
        // Window title
        std::string title;

        // Window dimension,
        // if the window is set to fullscreen, these value will be used
        // as the window dimension on fullscreen exit
        i32 width, height;

        // Ignore the provided width and height and make the window fullscreen
        bool fullscreen;
        // The window will be resizable by the user
        bool resizable;
        // The window will always float on top of other window
        bool floating;
    };
    
public:
    virtual ~Window() = default;
    
    // Obtain the current window data 
    virtual Data getWindowData() = 0;

    // Toggle the full screen mode of the window
    virtual void toggleFullscreen() = 0;
    // Set the full screen mode of the window
    virtual void setFullscreen(bool fullscreen) = 0;

    // Change the window size in windowed mode
    virtual void setSize(i32 width, i32 height) = 0;

    // Change window title
    virtual void setTitle(const char* title) = 0;
    
    // Capture the cursor and enable raw input if available
    virtual void captureCursor() = 0;
    // Release the cursor and disable raw input if available
    virtual void releaseCursor() = 0;

// Vulkan implementation functions 
public:
    // Retrieve a vulkan surface
    virtual VkSurfaceKHR getVkSurface(
        VkInstance instance,
        const VkAllocationCallbacks *allocator_p
    ) = 0;
    
    // Fetch the required vulkan extensions
    virtual std::vector<const char*> getVkExtensions() = 0;

protected:
    // Window initialization function
    virtual void initialize(Config config) = 0;

    // Window shutdown function 
    virtual void shutdown() = 0;

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
