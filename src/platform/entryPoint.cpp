#include "conduit/application.h"
#include "core/application.h"

#include "buildConfig.h"

#if defined CNDT_PLATFORM_LINUX || defined CNDT_PLATFORM_WINDOWS

// Linux and windows entry point
int main(void) {
    cndt::AppRunner app = cndt::getEntryClass();
    
    // Run the application
    app.Run();

    return 0;
}

#else
#error "Unsupported platform" 
#endif
