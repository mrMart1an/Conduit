#include "conduit/application.h"
#include "conduit/events/events.h"
#include "conduit/events/eventKeyCode.h"
#include "conduit/logging.h"

#include "conduit/renderer/renderer.h"
#include "conduit/time.h"
#include "core/application.h"
#include "window/glfw/glfwWindow.h"

#include <memory>

namespace cndt {

/*
 *
 *      Application implementation
 * 
 * */

// Base application constructor
Application::Application() :
    m_run_application(true),
    m_event_bus(),
    m_ecs_world(),
    m_window(),
    m_renderer()
{
    Window::Config window_config(app_name);
    
    // Create the glfw window handle
    m_window = std::make_unique<glfw::GlfwWindow>(
        m_event_bus.getEventWriter()
    );
    
    m_window->initialize(window_config);

    // Events callbacks setup
    m_event_bus.addCallback<WindowCloseEvent>(
        [&run = m_run_application](const WindowCloseEvent*) { run = false; }
    );   
    m_event_bus.addCallback<KeyPressEvent>(
        [&window = m_window](const KeyPressEvent* event) {
            char c = event->key_code;
            log::core::debug("key press: {}", c);

            if (event->key_code == keycode::KEY_F11) {
                window->toggleFullscreen();
            }
        }
    );

    // Create and initialize the renderer 
    m_renderer = Renderer::getRenderer(RendererBackend::Vulkan);
    m_renderer->initialize(app_name, m_window.get());
};

// Base application deconstructor
Application::~Application() 
{
    m_renderer->shutdown();
    m_window->shutdown();
};

// Start application main loop
void Application::startMainLoop() 
{
    time::StopWatch frame_time;

    while (m_run_application) {
        // Run the user define application update function
        update(frame_time.delta());

        // Pool the window event and update the event buffer
        m_window->poolEvents();
        m_event_bus.update();
    }
}

/*
 *
 *      App runner implementation
 * 
 * */

// Take ownership of the application and construct an event reader
AppRunner::AppRunner(std::unique_ptr<Application> application)
    : m_application_p(std::move(application))
{ }

// Run the application
void AppRunner::run() 
{
    // Run the user defined application startup function
    m_application_p->startup();

    // Run the application main loop function
    m_application_p->startMainLoop();
    
    // Run the user defined application shutdown function
    m_application_p->shutdown();
}

} // namespace cndt
