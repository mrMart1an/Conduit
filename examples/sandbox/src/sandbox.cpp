#include "conduit/defines.h"

#include "conduit/application.h"

using namespace cndt;
using namespace cndt::log::app;

// Declare the application class
class Sandbox : public cndt::Application {
public:
    std::string appName() const override { return "Sanbox app"; }

    void startup() override {
        m_asset_manager.get<Mesh>("builtin");

    }
    
    void update(f64) override {

    }

    void shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
