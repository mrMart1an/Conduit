#include "conduit/application.h"
#include "conduit/events/events.h"
#include "conduit/events/eventKeyCode.h"
#include "conduit/logging.h"

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
    m_event_bus()
{ };

// Base application deconstructor
Application::~Application() { };

// Start application main loop
void Application::startMainLoop() 
{
    while (m_run_application) {
        update(0);

        // Pool the window event and update the event buffer
        m_window->poolEvents();
        m_event_bus.update();
    }
}

// Engine startup function
void Application::engineStatup() 
{
    Window::Config window_config("Conduit test app");
    
    // Create the glfw window handle
    try {
        m_window = std::make_unique<glfw::GlfwWindow>(
            m_event_bus.getEventWriter()
        );
        
        m_window->initialize(window_config);
    } catch (WindowException& e) {
        log::core::fatal("Window initialization failed: {}", e.what());    
        return;
    }

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
}

// Engine shutdown function
void Application::engineShutdown() 
{
    m_window->shutdown();
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
    // Initialize the engine and setup the application
    m_application_p->engineStatup();
    m_application_p->startup();

    // Start the application main loop
    m_application_p->startMainLoop();
    
    // Quit the application and shutdown the engine
    m_application_p->shutdown();
    m_application_p->engineShutdown();
}

} // namespace cndt
