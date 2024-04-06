#include "conduit/defines.h"

#include "conduit/application.h"

#include "conduit/logging.h"

using namespace cndt;
using namespace cndt::log::app;

// Declare the application class
class Sandbox : public cndt::Application {
public:
    void startup() override {
        
    }
    
    void update(f32 delta_time) override {
        trace("updating: {}", delta_time); 
    }

    void shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
