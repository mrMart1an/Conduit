#ifndef CNDT_EVENT_WRITER_H
#define CNDT_EVENT_WRITER_H

#include "conduit/logging.h"

#include "conduit/internal/events/eventRegister.h"

#include <memory>

namespace cndt {

/*
 *
 *      Event writer declaration
 *
 * */

// Write event to the event bus that generated it 
class EventWriter {
public:   
    EventWriter(std::shared_ptr<internal::EventRegister> event_register) 
        : m_event_register(event_register) 
    { };
    
    // Send an event to the event bus that generated the writer
    template<class EventType>
    void Send(const EventType& event);

private:
    // Reference to the event bus that generated the event writer
    std::weak_ptr<internal::EventRegister> m_event_register;
};

/*
 *
 *      Event writer template implementation
 *
 * */

template<class EventType>
void EventWriter::Send(const EventType& event) {
    // Get a reference to the type event buffers
    if (auto event_register = m_event_register.lock()) {
        auto event_buffer = event_register->GetEventBuffer<EventType>(); 
        
        // Add the event to the end of the buffer
        event_buffer.lock()->Append(event);
    } else {
        // If the event register was deleted log a error message
        log::core::error("EventWriter::Send -> event register was deleted}");
    }
}

} // namespace cndt

#endif
