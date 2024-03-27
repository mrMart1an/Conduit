
#include <memory>

#include "conduit/application.h"

#include "buildConfig.h"

#if defined CNDT_PLATFORM_LINUX || defined CNDT_PLATFORM_WINDOWS

// Linux and windows entry point
int main(void) {
    std::unique_ptr<cndt::Application> app = cndt::getEntryClass();

    app->StartMainLoop();

    return 0;
}

#else
#error "Unsupported platform" 
#endif
