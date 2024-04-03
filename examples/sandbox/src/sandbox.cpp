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
        m_event_bus.addCallback<KeyEvent>(keyCall);
        m_event_bus.addCallback<WindowResize>(winCall);
        
        EventWriter writer = m_event_bus.getEventWriter();
        EventReader reader = m_event_bus.getEventReader<KeyEvent>();
        
        WindowResize resize;
        KeyEvent event;
        
        event.key_code = 12;
        writer.send(event);
        event.key_code = 62;
        writer.send(event);

        m_event_bus.update();
        
        for (auto& event : reader) {
            info("reader key event: {}", event.key_code);
        }

        resize.height = 20; resize.width = 32;
        writer.send(resize);
        resize.height = 2320; resize.width = 334322;
        writer.send(resize);
        resize.height = 42; resize.width = 69;
        writer.send(resize);

        m_event_bus.update();

    }
    
    void Update(f32 delta_time) override {
        trace("updating: {}", delta_time); 
    }

    void Shutdown() override {
        
    }
};

CNDT_ENTRY_CLASS(Sandbox);
