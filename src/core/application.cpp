#include "conduit/application.h"

namespace cndt {

// Base application constructor
Application::Application() {
    
};

// Base application deconstructor
Application::~Application() {
    
};

// Start application main loop
void Application::StartMainLoop() {
    this->Update(0);
}

} // namespace cndt
