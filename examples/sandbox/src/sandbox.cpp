#include "conduit/defines.h"

#include "conduit/application.h"

#include "conduit/events/eventWriter.h"
#include "conduit/events/events.h"
#include "conduit/logging.h"

using namespace cndt;
using namespace cndt::log::app;

void keyCall(const KeyEvent* event) {
    info("callback key: {}", event->key_code);
}

void winCall(const WindowResize* event) {
    info("callback resize: {}, {}", event->width, event->height);
}

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

        m_event_bus.AddCallback<KeyEvent>(keyCall);
        m_event_bus.AddCallback<WindowResize>(winCall);
        
        EventWriter writer = m_event_bus.GetEventWriter();
        
        WindowResize resize;
        KeyEvent event;
        
        event.key_code = 12;
        writer.Send(event);
        event.key_code = 62;
        writer.Send(event);

        m_event_bus.Update();
        
        resize.height = 20; resize.width = 32;
        writer.Send(resize);
        resize.height = 2320; resize.width = 334322;
        writer.Send(resize);
        resize.height = 42; resize.width = 69;
        writer.Send(resize);

        m_event_bus.Update();

    }
    
    void Update(f32 delta_time) override {
        trace("updating: {}", delta_time); 
    }

    void Shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
