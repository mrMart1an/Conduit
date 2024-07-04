#ifndef CNDT_GLFW_WINDOW_H
#define CNDT_GLFW_WINDOW_H

#include "conduit/config/engineConfig.h"
#include "conduit/events/eventWriter.h"
#include "conduit/window/window.h"

struct GLFWwindow;

namespace cndt::glfw {

// Glfw window class
class GlfwWindow : public Window {
private:
    // Window current initialization status enum
    enum class Status {
        Uninitialized = 0,  
        GlfwInitialized,
        Initialized,
    };

public:
    GlfwWindow(EventWriter event_writer);
    ~GlfwWindow() override;

    // Obtain the current window data 
    Data getWindowData() override;

    // Toggle the full screen mode of the window
    void toggleFullscreen() override;
    // Set the full screen mode of the window
    void setFullscreen(bool fullscreen) override;
    
    // Change window title
    void setTitle(const char* title) override;

    // Change the window size in windowed mode
    void setSize(i32 width, i32 height) override;
    
    // Capture the cursor and enable raw input if available
    void captureCursor() override;
    // Release the cursor and disable raw input if available
    void releaseCursor() override;
    
private:
    // Initialize the glfw window
    void initialize(
        EngineConfig::Window config,
        const char* title
    ) override;

    // Shutdown the glfw window
    void shutdown() override;
    
    // Pool the window event and send them to the event bus
    void poolEvents() override;

// Vulkan implementation functions 
public:
    // Retrieve a vulkan surface
    VkSurfaceKHR getVkSurface(
        VkInstance instance,
        const VkAllocationCallbacks *allocator_p
    ) override;
    
    // Fetch the required vulkan extensions
    std::vector<const char*> getVkExtensions() override;

private:
    // Glfw window handle
    GLFWwindow *m_glfw_window;

    // Used to send window event to the application bus
    EventWriter m_event_writer;

    // Current fullscreen status
    bool m_fullscreen;

    // Current window position and size data
    Data m_current_data;
    // Old window position and size data
    // used to restore the window size and position when leaving fullscreen
    Data m_old_data;

    // Initialization status
    Status m_init_status;

// Glfw callbacks declaration
private:
    // Callback functions for mouse button events 
    static void callback_mouse_button_event(
        GLFWwindow* window, int button, int action, int mods
    );
    // Callback functions for mouse position events 
    static void callback_cursor_pos_event(
        GLFWwindow* window, double xpos, double ypos
    );
    // Callback functions for mouse scrolling events 
    static void callback_scroll_event(
        GLFWwindow* window, double xoffset, double yoffset
    );
    // Callback functions for keyboard events 
    static void callback_key_event(
        GLFWwindow* window, int key, int scancode, int action, int mods
    );
    
    // Callback functions for window cursor enter and exit events
    static void callback_cursor_entered_event(
        GLFWwindow* window, int entered
    );
    // Callback functions for window move events
    static void callback_window_move_event(
        GLFWwindow* window, int xpos, int ypos
    );
    // Callback functions for window resize events
    static void callback_window_resize_event(
        GLFWwindow* window, int width, int height
    );
    // Callback functions for window resize events
    static void callback_buffer_resize_event(
        GLFWwindow* window, int width, int height
    );
    
    // Callback functions for window close events
    static void callback_window_close_event(GLFWwindow* window);
    
    // Glfw error callback function 
    // Print the error with the internal logger
    static void callback_error(int error_code, const char* error_msg);
};

} // namespace cndt::glfw

#endif
