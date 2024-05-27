#ifndef CNDT_APP_RUNNER_H
#define CNDT_APP_RUNNER_H

#include <memory>

namespace cndt {

class Application;

// Run and own an application
class AppRunner {
public:
    // Take ownership of the application and construct an event reader
    AppRunner(std::unique_ptr<Application> application);

    // Start the application
    void run();
    
private:
    // Store the application class in a unique pointer
    std::unique_ptr<Application> m_application_p;
};

} // namespace cndt

#endif
