#ifndef CNDT_EVENT_WRITER_H
#define CNDT_EVENT_WRITER_H

#include "conduit/internal/events/eventBuffer.h"
#include "conduit/events/eventBus.h"

namespace cndt {

class EventBus;

/*
 *
 *      Event writer declaration
 *
 * */

// Write event to the event bus that generated it 
class EventWriter {
public:   
    EventWriter(EventBus *bus) : m_bus(bus) {};
    
    // Send an event to the event bus that generated the writer
    template<class EventType>
    void Send(const EventType& event);

private:
    // Reference to the event bus that generated the event writer
    EventBus *m_bus;
};

/*
 *
 *      Event writer template implementation
 *
 * */

template<class EventType>
void EventWriter::Send(const EventType& event) {
    // Get a reference to the type event buffers
    auto event_buffer = m_bus->GetEventBuffer<EventType>(); 

    // Add the event to the end of the buffer
    event_buffer->Append(event);
}

} // namespace cndt

#endif
