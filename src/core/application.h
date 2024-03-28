#ifndef CNDT_APPLICATION_CORE_H
#define CNDT_APPLICATION_CORE_H

#include "conduit/application.h"

#include <memory>

namespace cndt {
// Define the app runner
class AppRunner {
public:
    // Take ownership of the application and construct an event reader
    AppRunner(std::unique_ptr<Application> application);

    // Start the application
    void Run();
private:
    std::unique_ptr<Application> m_application_p;
};

}


#endif
