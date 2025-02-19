#include "conduit/config/engineConfig.h"
#include "conduit/logging.h"

#include "conduit/events/eventWriter.h"
#include "conduit/renderer/backendEnum.h"
#include "conduit/window/window.h"
#include "conduit/window/windowException.h"

#include "window/glfw/glfwWindow.h"

#ifdef CNDT_OPENGL_BACKEND
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>


namespace cndt::glfw {

// Glfw window constructor
GlfwWindow::GlfwWindow(EventWriter event_writer) : 
    m_event_writer(event_writer), 
    m_fullscreen(false),
    m_current_data(),
    m_old_data(),
    m_init_status(Status::Uninitialized)
{ }

// Glfw window destructor
GlfwWindow::~GlfwWindow() 
{
    if (m_init_status != Status::Uninitialized) {
        log::core::warn(
            "GlfwWindow destructor called before shutdown"
        );
        
        shutdown();
    }
}

// Glfw window initialization function
void GlfwWindow::initialize(
    EngineConfig::Window config,
    RendererBackend render_backend,

    const char* title
) {
    if (!glfwInit()) {
        throw WindowInitError(
            "GlfwWindow init error: glfw initialization failed"
        );
        
    } else {
        // Set the status to glfw initialized
        m_init_status = Status::GlfwInitialized;
    }
    
    // Set the error message callback
    glfwSetErrorCallback(callback_error);
    
    // Window creation hints
    glfwWindowHint(
        GLFW_RESIZABLE,
        config.resizable.value_or(false) ? GLFW_TRUE : GLFW_FALSE
    );
    glfwWindowHint(
        GLFW_FLOATING,
        config.floating.value_or(false) ? GLFW_TRUE : GLFW_FALSE
    );   

    // If openGL is not the rendering backend disable
    // openGL context initialization
    if (render_backend == RendererBackend::OpenGL) {
        // TODO: Change openGL version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    } else {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
    
    // Create the glfw window handler
    m_glfw_window = glfwCreateWindow(
        config.width.value_or(800), 
        config.height.value_or(600),
        title,
        NULL, NULL
    );

    // Check if the window was create successfully 
    if (m_glfw_window == nullptr) {
        // Terminate the glfw session and throw an init exception
        glfwTerminate();
        
        throw WindowInitError(
            "GlfwWindow init error: glfw window creation failed"
        );
        
    } else {
        // Set the status to initialized
        m_init_status = Status::Initialized;
    }
 
    // Create OpenGL context and load functions
    if (render_backend == RendererBackend::OpenGL) {
        glfwMakeContextCurrent(m_glfw_window);

        // Load the GL function
        #ifdef CNDT_OPENGL_BACKEND
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            log::core::error("Glad failed to load OpenGL functions");

            glfwTerminate();
            throw WindowInitError("Glad failed to load OpenGL functions");
        }    
        #endif
    }
   
    // Store the current data
    glfwGetWindowPos(
        m_glfw_window, 
        &m_current_data.x_pos, 
        &m_current_data.y_pos 
    );
    
    glfwGetWindowSize(
        m_glfw_window, 
        &m_current_data.window_width, 
        &m_current_data.window_height 
    );
    glfwGetFramebufferSize(
        m_glfw_window, 
        &m_current_data.buffer_width, 
        &m_current_data.buffer_height 
    );
    
    // Set fullscreen if requested
    setFullscreen(config.start_fullscreen.value_or(false));

    // Set the callback data pointer to the current window object
    glfwSetWindowUserPointer(m_glfw_window, this);
    
    // Set input event callbacks
    glfwSetKeyCallback(m_glfw_window, callback_key_event);
    glfwSetMouseButtonCallback(m_glfw_window, callback_mouse_button_event);
    glfwSetCursorPosCallback(m_glfw_window, callback_cursor_pos_event);
    glfwSetScrollCallback(m_glfw_window, callback_scroll_event);

    // Set window event callbacks
    glfwSetWindowSizeCallback(m_glfw_window, callback_window_resize_event);
    glfwSetWindowPosCallback(m_glfw_window, callback_window_move_event);
    glfwSetWindowCloseCallback(m_glfw_window, callback_window_close_event);
    glfwSetCursorEnterCallback(m_glfw_window, callback_cursor_entered_event);
    glfwSetFramebufferSizeCallback(
        m_glfw_window, callback_buffer_resize_event
    );
}

// Glfw window shutdown function 
void GlfwWindow::shutdown() 
{
    if (m_init_status == Status::Initialized) {
        glfwDestroyWindow(m_glfw_window);
        glfwTerminate();
    } else if (m_init_status == Status::GlfwInitialized) {
        glfwTerminate();
    }

    m_init_status = Status::Uninitialized;
}

// Obtain the current window data 
Window::Data GlfwWindow::getWindowData()
{
    return m_current_data;
}

// Toggle the full screen mode of the window
void GlfwWindow::toggleFullscreen() 
{
    setFullscreen(!m_fullscreen);
}

// Set the full screen mode of the window
void GlfwWindow::setFullscreen(bool fullscreen) 
{
    // If the requested mode is already enable return immediately
    if (m_fullscreen == fullscreen)
        return;
    
    // Set full screen
    if (fullscreen) {
        // Store the old windowed mode data
        m_old_data = m_current_data;

        // Get the monitor and the video mode
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(
            m_glfw_window,
            monitor,
            0, 0,
            mode->width, mode->height,
            mode->refreshRate
        );

    } else {
        // Restore the window to the pre-fullscreen state
        glfwSetWindowMonitor(
            m_glfw_window,
            NULL,
            m_old_data.x_pos, m_old_data.y_pos,
            m_old_data.window_width, m_old_data.window_height,
            GLFW_DONT_CARE
        );
    }

    // Store the current data
    glfwGetWindowPos(
        m_glfw_window, 
        &m_current_data.x_pos, 
        &m_current_data.y_pos 
    );
    
    glfwGetWindowSize(
        m_glfw_window, 
        &m_current_data.window_width, 
        &m_current_data.window_height 
    );
    glfwGetFramebufferSize(
        m_glfw_window, 
        &m_current_data.buffer_width, 
        &m_current_data.buffer_height 
    );
 
    // Store the new full screen setting
    m_fullscreen = fullscreen;
}

// Change the window size in windowed mode
void GlfwWindow::setSize(i32 width, i32 height) 
{
    if (m_fullscreen)
        return;
    
    glfwSetWindowSize(m_glfw_window, width, height);
    
    // Store the current data
    glfwGetWindowPos(
        m_glfw_window, 
        &m_current_data.x_pos, 
        &m_current_data.y_pos 
    );
    
    glfwGetWindowSize(
        m_glfw_window, 
        &m_current_data.window_width, 
        &m_current_data.window_height 
    );
    glfwGetFramebufferSize(
        m_glfw_window, 
        &m_current_data.buffer_width, 
        &m_current_data.buffer_height 
    );
}

// Change the window title
void GlfwWindow::setTitle(const char* title)
{
    glfwSetWindowTitle(m_glfw_window, title);
}

// Capture the cursor and enable raw input if available
void GlfwWindow::captureCursor() 
{
    glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(m_glfw_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

// Release the cursor and disable raw input if available
void GlfwWindow::releaseCursor() 
{
    glfwSetInputMode(m_glfw_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
    glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

// Pool the window event and send them to the event bus
void GlfwWindow::poolEvents() 
{
    glfwPollEvents();
}

} // namespace cndt::glfw
