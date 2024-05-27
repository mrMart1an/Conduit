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
        
        // Run the application
        app.run();
        
    } catch (std::exception &e) {
        cndt::log::core::fatal("Exception: {}", e.what());
    };

    return 0;
}

#else
#error "Unsupported platform" 
#endif
