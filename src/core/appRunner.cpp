#include "conduit/config/engineConfig.h"

#include "conduit/application.h"

#include "core/appRunner.h"

namespace cndt {

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
void AppRunner::run(EngineConfig config) 
{
    // Get the application engine config and merge it
    EngineConfig app_config = m_application_p->engineConfig(); 
    config.merge(app_config);

    // Set up the engine and 
    // run the user defined application startup function
    m_application_p->engineStartup(config);
    m_application_p->startup();

    // Run the application main loop function
    m_application_p->mainLoop();
    
    // Run the user defined application shutdown function
    // and shutdown the engine
    m_application_p->shutdown();
    m_application_p->engineShutdown();
}

} // namespace cndt
