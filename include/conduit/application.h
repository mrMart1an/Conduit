#ifndef CNDT_APPLICATION_H
#define CNDT_APPLICATION_H

#include "conduit/defines.h"

#include "conduit/ecs/world.h"
#include "conduit/events/eventBus.h"
#include "conduit/window/window.h"

#include <memory>

namespace cndt {

// Base application class 
class Application {
    friend class AppRunner;
    
public:
    Application();
    virtual ~Application();

    // Application startup function,
    // called after engine initialization and before stating the main loop
    virtual void startup() = 0;

    // Application update function, 
    // called once per frame to update the scene
    virtual void update(f64 delta_time) = 0;

    // Application shutdown function,
    // called before the engine shutdown
    virtual void shutdown() = 0;

protected:
    // Start the main application loop
    // this function shouldn't be called by the client
    void startMainLoop();

protected:
    bool m_run_application;
    
    // Application event bus
    EventBus m_event_bus;

    // ECS world
    World m_ecs_world;

    // Application window handle
    std::unique_ptr<Window> m_window;
};

// Return an instance of an user defined application object
// to be implemented by the client
std::unique_ptr<Application> getEntryClass();
    
} // namespace cndt
 
// Macro to generate the get entry class function
#define CNDT_ENTRY_CLASS(Type)                                  \
    std::unique_ptr<cndt::Application> cndt::getEntryClass() {  \
        return std::make_unique<Type>();                        \
    }  

#endif
