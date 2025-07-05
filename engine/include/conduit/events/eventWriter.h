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
// this event writer should not be shared between threads 
class EventWriter {
public:   
    EventWriter(std::shared_ptr<internal::EventRegister> event_register) 
        : m_event_register(event_register) 
    { };
    
    // Send an event to the event bus that generated the writer
    template<class EventType>
    void send(const EventType& event);

private:
    // Reference to the event register that generated the event writer
    std::weak_ptr<internal::EventRegister> m_event_register;
};

/*
 *
 *      Event writer template implementation
 *
 * */

template<class EventType>
void EventWriter::send(const EventType& event) {
    // Get a reference to the type event buffers
    if (auto event_register = m_event_register.lock()) {
        auto event_buffer = event_register->getEventBuffer<EventType>(); 
        
        // Add the event to the end of the buffer
        // (this shouldn't fail because m_event_register will always exist)
        event_buffer.lock()->append(event);
        
    } else {
        // If the event register was deleted log a error message
        log::core::error("EventWriter::Send -> event register was deleted");
    }
}

} // namespace cndt

#endif
