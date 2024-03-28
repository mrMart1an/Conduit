#include "conduit/application.h"
#include "core/application.h"
#include <memory>
#include <utility>

namespace cndt {

/*
 *
 *      Application implementation
 * 
 * */

// Base application constructor
Application::Application() :
    m_event_bus()
{ };

// Base application deconstructor
Application::~Application() { };

// Start application main loop
void Application::StartMainLoop() {
    Update(0);
}

// Engine startup function
void Application::EngineStatup() {
    
}

// Engine shutdown function
void Application::EngineShutdown() {
    
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
void AppRunner::Run() {
    // Initialize the engine and setup the application
    m_application_p->EngineStatup();
    m_application_p->Startup();

    // Start the application main loop
    m_application_p->StartMainLoop();
    
    // Quit the application and shutdown the engine
    m_application_p->Shutdown();
    m_application_p->EngineShutdown();
}

} // namespace cndt
