#include "conduit/application.h"
#include "core/application.h"

#include <memory>

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
void Application::startMainLoop() {
    update(0);
}

// Engine startup function
void Application::engineStatup() {
    
}

// Engine shutdown function
void Application::engineShutdown() {
    
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
void AppRunner::run() {
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
