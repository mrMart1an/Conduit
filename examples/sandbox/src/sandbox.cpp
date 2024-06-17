#include "conduit/components/transform.h"

#include "conduit/defines.h"

#include "conduit/application.h"

using namespace cndt;
using namespace cndt::log::app;

// Declare the application class
class Sandbox : public cndt::Application {
public:
    std::string appName() const override { return "Sanbox app"; }

    void startup() override {
        Entity e = m_ecs_world.newEntity();
        m_ecs_world.attachComponent<Transform>(e);
    }
    
    void update(f64) override {

    }

    void shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
