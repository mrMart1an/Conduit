#include "conduit/defines.h"

#include "conduit/application.h"

using namespace cndt;
using namespace cndt::log::app;

// Declare the application class
class Sandbox : public cndt::Application {
public:
    void startup() override {
        
    }
    
    void update(f64) override {

    }

    void shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
