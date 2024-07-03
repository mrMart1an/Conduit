#ifndef CNDT_APPLICATION_H
#define CNDT_APPLICATION_H

#include "conduit/assets/assetsManager.h"
#include "conduit/defines.h"

#include "conduit/ecs/world.h"
#include "conduit/events/eventBus.h"
#include "conduit/renderer/renderer.h"
#include "conduit/window/window.h"
#include "conduit/internal/core/deleteQueue.h"

#include <memory>

namespace cndt {

class DeleteQueue;

// Base application class 
class Application {
    friend class AppRunner;

public:
    Application();
    virtual ~Application();

protected:
    // Return a string as the application name
    virtual std::string appName() const { return "Conduit application"; };

    // Application startup function,
    // called after engine initialization and before stating the main loop
    virtual void startup() = 0;

    // Application update function, 
    // called once per frame to update the scene
    virtual void update(f64 delta_time) = 0;

    // Application shutdown function,
    // called before the engine shutdown
    virtual void shutdown() = 0;

// These functions shouldn't be called by the used defined application class
private:
    // Initialize the game engine 
    void engineStartup();

    // Shutdown the game engine
    void engineShutdown();
    
    // Set-up the game engine key bindings
    void setupKeyBinding();

    // Start the main application loop
    void mainLoop();

protected:
    bool m_run_application;

    // Asset manager
    AssetsManager m_asset_manager;

    // Application event bus
    EventBus m_event_bus;

    // ECS world
    World m_ecs_world;

    // Application window handle
    std::unique_ptr<Window> m_window;
    // Application renderer handle
    std::unique_ptr<Renderer> m_renderer;

private:
    // Application deleter queue
    DeleteQueue m_delete_queue;
};

// Return an instance of an user defined application object
// to be implemented by the client
std::unique_ptr<Application> getEntryClass();
    
// Macro to generate the get entry class function
#define CNDT_ENTRY_CLASS(Type)                                  \
    std::unique_ptr<cndt::Application> cndt::getEntryClass() {  \
        return std::make_unique<Type>();                        \
    }  

} // namespace cndt

#endif
