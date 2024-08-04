#include "conduit/assets/assetsManager.h"
#include "conduit/internal/core/deleteQueue.h"

#include "conduit/application.h"
#include "conduit/events/events.h"
#include "conduit/events/eventKeyCode.h"
#include "conduit/renderer/renderer.h"
#include "conduit/time.h"

#include "window/glfw/glfwWindow.h"

#include "core/appRunner.h"

#include <functional>
#include <memory>

namespace cndt {

/*
 *
 *      Engine initialization and shutdown 
 * 
 * */

// Base application constructor
Application::Application() :
    m_run_application(true),
    m_asset_manager(),
    m_event_bus(),
    m_ecs_world(),
    m_window(),
    m_renderer(),
    m_delete_queue()
{ };

// Base application deconstructor
Application::~Application() {
    m_delete_queue.callDeleter();
};

// Initialize the game engine 
void Application::engineStartup(EngineConfig config)
{
    // Load the assets manager user table if needed
    if (config.assets.user_table_path.has_value()) {
        m_asset_manager = AssetsManager(
            config.assets.user_table_path.value()
        );
    }

    // Create the glfw window handle
    m_window = std::make_unique<glfw::GlfwWindow>(
        m_event_bus.getEventWriter()
    );
    
    m_window->initialize(
        config.window,
        config.renderer.backend.value(),

        appName().c_str()
    );
    
    m_delete_queue.addDeleter(std::bind(
        &Window::shutdown, m_window.get()
    ));

    // Create and initialize the renderer 
    m_renderer = Renderer::getRenderer(config.renderer);
    m_renderer->initialize(
        config.renderer,
        appName().c_str(),
        m_window.get()
    );

    m_delete_queue.addDeleter(std::bind(
        &Renderer::shutdown, m_renderer.get()
    ));
    
    // Events callbacks setup
    m_event_bus.addCallback<WindowCloseEvent>(
        [&run = m_run_application](const WindowCloseEvent*) { run = false; }
    );   

    m_event_bus.addCallback<WindowResizeEvent>(
        [&renderer = m_renderer](const WindowResizeEvent* event) {
            renderer->resize(event->width, event->height);
        }
    );

    // Set up engine key binding
    setupKeyBinding();
}

// Shutdown the game engine
void Application::engineShutdown()
{
    m_delete_queue.callDeleter();
}

// Set-up the game engine key bindings
void Application::setupKeyBinding()
{
    m_event_bus.addCallback<KeyPressEvent>(
        [&window = m_window, &renderer = m_renderer]
        (const KeyPressEvent* event) {
            // Toggle fullscreen
            if (event->key_code == keycode::KEY_F11) {
                window->toggleFullscreen();
            }
            
            // Toggle v-sync
            if (event->key_code == keycode::KEY_F7) {
                renderer->toggleVsync();
            }
        }
    );
}

/*
 *
 *      Engine functions
 *
 * */

// Start application main loop
void Application::mainLoop() 
{
    time::StopWatch frame_time;

    while (m_run_application) {
        // Run the user define application update function
        update(frame_time.delta());

        // Draw a frame
        RenderGraph graph = m_renderer->getRenderGraph();

        m_renderer->executeGraph(graph);

        // Pool the window event and update the event buffer
        m_window->poolEvents();
        m_event_bus.update();
    }
}

} // namespace cndt
