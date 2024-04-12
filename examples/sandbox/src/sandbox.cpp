#include "conduit/defines.h"

#include "conduit/application.h"
#include "conduit/ecs/world.h"
#include "conduit/logging.h"

using namespace cndt;
using namespace cndt::log::app;

// Declare the application class
class Sandbox : public cndt::Application {
public:
    void startup() override {
        Entity e1 = m_ecs_world.newEntity();
        Entity e2 = m_ecs_world.newEntity();
        Entity e3 = m_ecs_world.newEntity();

        info("{}, {}, {}", e1.id(), e2.id(), e3.id());

        m_ecs_world.deleteEntity(e2);
        Entity e4 = m_ecs_world.newEntity();
        Entity e5 = m_ecs_world.newEntity();
        
        info("{}, {}", e4.id(), e5.id());
        
    }
    
    void update(f64) override {

    }

    void shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
