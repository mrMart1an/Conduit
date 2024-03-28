#include "conduit/defines.h"

#include "conduit/application.h"

#include "conduit/events/eventReader.h"
#include "conduit/events/eventWriter.h"
#include "conduit/events/events.h"
#include "conduit/logging.h"

using namespace cndt;
using namespace cndt::log::app;

void testEvent();

// Declare the application class
class Sandbox : public cndt::Application {
public:
    void Startup() override {
        trace("test {} {} {}", 3, 3.14, "conduit");
        debug("test {} {} {}", 3, 3.14, "conduit");
        info("test {} {} {}", 3, 3.14, "conduit");
        warn("test {} {} {}", 3, 3.14, "conduit");
        error("test {} {} {}", 3, 3.14, "conduit");
        fatal("test {} {} {}", 3, 3.14, "conduit");

        EventWriter writer = m_event_bus.GetEventWriter();
        KeyEvent event;
        event.key_code = 12;
        writer.Send(event);

        EventReader<KeyEvent> reader = m_event_bus.GetEventReader<KeyEvent>();
        KeyEvent recv = reader.NextEvent().value();

        info("bus test: {}", recv.key_code);
    }
    
    void Update(f32 delta_time) override {
        trace("updating: {}", delta_time); 
    }

    void Shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
