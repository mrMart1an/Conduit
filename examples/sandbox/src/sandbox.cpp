#include "conduit/defines.h"

#include "conduit/application.h"
#include "conduit/events/events.h"

#include "conduit/events/eventReader.h"
#include "conduit/events/eventWriter.h"

#include "conduit/logging.h"

using namespace cndt;
using namespace cndt::log::app;

void testEvent();

// Declare the application class
class Sandbox : public cndt::Application {
public:
    Sandbox() {
        trace("test {} {} {}", 3, 3.14, "conduit");
        debug("test {} {} {}", 3, 3.14, "conduit");
        info("test {} {} {}", 3, 3.14, "conduit");
        warn("test {} {} {}", 3, 3.14, "conduit");
        error("test {} {} {}", 3, 3.14, "conduit");
        fatal("test {} {} {}", 3, 3.14, "conduit");


        EventBus bus;
        EventWriter writer = bus.GetEventWriter();
        EventReader reader = bus.GetEventReader<KeyEvent>();
        
        KeyEvent key;
        key.key_code = 42;
        writer.Send(key);
        bus.Update();
        key.key_code = 83;
        writer.Send(key);
        key.key_code = 16;
        writer.Send(key);

        
        auto event = reader.NextEvent(); 
        while (event.has_value()) {
            info("key event: {}", event.value().key_code);
            event = reader.NextEvent(); 
        }
    }
    
    void Update(f32 delta_time) override {
        trace("updating: {}", delta_time); 
    }
};

CNDT_ENTRY_CLASS(Sandbox);
