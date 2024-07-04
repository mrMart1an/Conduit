#include "conduit/config/engineConfig.h"
#include "conduit/application.h"
#include "conduit/logging.h"

#include "core/appRunner.h"

#include <exception>

#include "buildConfig.h"

#if defined CNDT_PLATFORM_LINUX || defined CNDT_PLATFORM_WINDOWS

// Linux and windows entry point
int main(void) {
    try {
        cndt::AppRunner app = cndt::getEntryClass();

        // Get the configuration
        // TODO add config file and command line argument parser 
        cndt::EngineConfig config;
        config.setDefault();
        
        // Run the application
        app.run(config);
        
    } catch (std::exception &e) {
        cndt::log::core::fatal("Exception: {}",  e.what());
    };

    return 0;
}

#else
#error "Unsupported platform" 
#endif
