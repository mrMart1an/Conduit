#ifndef CNDT_ASSERT_H
#define CNDT_ASSERT_H

#include "conduit/defines.h"

#include "conduit/events/eventBus.h"

#include <memory>

namespace cndt {

// Run the application
class AppRunner;

// Base application class 
class Application {
    friend class AppRunner;
    
public:
    Application();
    virtual ~Application();

    // Application startup function,
    // called after engine initialization and before stating the main loop
    virtual void Startup() = 0;

    // Application update function, 
    // called once per frame to update the scene
    virtual void Update(f32 delta_time) = 0;

    // Application shutdown function,
    // called before the engine shutdown
    virtual void Shutdown() = 0;

protected:
    // Start the main application loop
    // this function shouldn't be called by the client
    void StartMainLoop();

    // Engine startup function
    void EngineStatup();

    // Engine shutdown function
    void EngineShutdown();

protected:
    // Application event bus
    EventBus m_event_bus;
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
