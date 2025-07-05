#include "conduit/events/events.h"
#include "conduit/logging.h"

#include "window/glfw/glfwWindow.h"

#include <GLFW/glfw3.h>

namespace cndt::glfw {

// Get the event writer from the user data in the glfw window handle
#define GET_EVENT_WRITER(window)                                        \
    ((GlfwWindow*)glfwGetWindowUserPointer(window))->m_event_writer     \
 
// Get the window pointer from the user data in the glfw window handle
#define GET_WINDOW(window)                                              \
    (GlfwWindow*)glfwGetWindowUserPointer(window)                       \
    
// Callback functions for mouse button events 
void GlfwWindow::callback_mouse_button_event(
    GLFWwindow* glfw_window, int button, int action, int mods
) {
    EventWriter& writer = GET_EVENT_WRITER(glfw_window);
    
    switch (action) {
        case GLFW_PRESS: {
            MouseKeyPressEvent event = {
                .button_code = static_cast<u32>(button),
                .mods = static_cast<u32>(mods)
            }; 
            writer.send(event);
        }
        
        case GLFW_RELEASE: {
            MouseKeyReleaseEvent event = {
                .button_code = static_cast<u32>(button),
                .mods = static_cast<u32>(mods)
            }; 
            writer.send(event);
        }
    }
}

// Callback functions for mouse position events 
void GlfwWindow::callback_cursor_pos_event(
    GLFWwindow* glfw_window, double x_pos, double y_pos
) {
    EventWriter& writer = GET_EVENT_WRITER(glfw_window);

    MousePositionEvent event = {
        .x_pos = x_pos,
        .y_pos = y_pos
    };
    writer.send(event);
}

// Callback functions for mouse scrolling events 
void GlfwWindow::callback_scroll_event(
    GLFWwindow* glfw_window, double x_offset, double y_offset
) {
    EventWriter& writer = GET_EVENT_WRITER(glfw_window);

    MouseScrollEvent event = {
        .x_scroll = x_offset,
        .y_scroll = y_offset
    };
    writer.send(event);
    
}

// Callback functions for keyboard events 
void GlfwWindow::callback_key_event(
    GLFWwindow* glfw_window, int key, int, int action, int mods
) {
    EventWriter& writer = GET_EVENT_WRITER(glfw_window);

    switch (action) {
        case GLFW_PRESS: {
            KeyPressEvent event = {
                .key_code = static_cast<u32>(key),
                .mods = static_cast<u32>(mods)
            }; 
            writer.send(event);
        }

        case GLFW_REPEAT: {
            KeyRepeatEvent event = {
                .key_code = static_cast<u32>(key),
                .mods = static_cast<u32>(mods)
            }; 
            writer.send(event);
        }
        
        case GLFW_RELEASE: {
            KeyReleaseEvent event = {
                .key_code = static_cast<u32>(key),
                .mods = static_cast<u32>(mods)
            }; 
            writer.send(event);
        }
    }
}

// Callback functions for window cursor enter and exit events
void GlfwWindow::callback_cursor_entered_event(
    GLFWwindow* glfw_window, int entered
) {
    EventWriter& writer = GET_EVENT_WRITER(glfw_window);
    
    if (entered)
        writer.send(WindowFocusGainEvent());
    else 
        writer.send(WindowFocusLostEvent());
}

// Callback functions for window move events
void GlfwWindow::callback_window_move_event(
    GLFWwindow* glfw_window, int x_pos, int y_pos
) {
    EventWriter& writer = GET_EVENT_WRITER(glfw_window);
    GlfwWindow* window = GET_WINDOW(glfw_window);
    
    // Update the current window data
    window->m_current_data.x_pos = x_pos;
    window->m_current_data.y_pos = y_pos;
    
    // Send the window resize event to the bus
    WindowMoveEvent event = {
        .x_pos = x_pos,
        .y_pos = y_pos
    };
    writer.send(event);
}

// Callback functions for window resize events
void GlfwWindow::callback_window_resize_event(
    GLFWwindow* glfw_window, int width, int height
) {
    GlfwWindow* window = GET_WINDOW(glfw_window);

    // Update the current window data
    window->m_current_data.window_width = width;
    window->m_current_data.window_height = height;
}

// Callback functions for window resize events
void GlfwWindow::callback_buffer_resize_event(
    GLFWwindow* glfw_window, int width, int height
) {
    EventWriter& writer = GET_EVENT_WRITER(glfw_window);
    GlfwWindow* window = GET_WINDOW(glfw_window);
    
    // Update the current window data
    window->m_current_data.buffer_width = width;
    window->m_current_data.buffer_height = height;
    
    // Send the window move event to the bus
    WindowResizeEvent event = {
        .width = width,
        .height = height
    };
    writer.send(event);
}

// Callback functions for window close events
void GlfwWindow::callback_window_close_event(GLFWwindow* glfw_window) 
{
    EventWriter& writer = GET_EVENT_WRITER(glfw_window);
    writer.send(WindowCloseEvent());
}

// Glfw error callback function 
// Print the error with the internal logger
void GlfwWindow::callback_error(int error_code, const char* error_msg) 
{
    log::core::error("glfw error: [{}] -> {}", error_code, error_msg);    
}

} // namespace cndt::glfw
