#ifndef CNDT_EVENT_BUS_H
#define CNDT_EVENT_BUS_H

#include "conduit/internal/events/eventBuffer.h"

#include <memory>

namespace cndt {

class EventWriter;

template<class EventType>
class EventReader;

/*
 *
 *      Event bus declaration
 *
 * */

// Double buffered event bus
class EventBus {
    // Event writer friend class
    friend class EventWriter;
    
public:
    EventBus();
    ~EventBus();
    
    // Swap the event buffers and run all the callbacks
    void Update();
    
    // Return an event writer for this bus
    EventWriter GetEventWriter();

    // Return an event reader for this bus
    template<class EventType>
    EventReader<EventType> GetEventReader();

private:
    // Store an id to a event type
    typedef u64 EventTypeId;
    
    // Return an unique id for each event type added to this bus
    template<class EventType>
    EventTypeId GetEventTypeId();     

    // Add the event type to the bus if it doesn't already exist
    template<class EventType>
    void AddEventType();

    // Get an event buffer for the specific type
    // if the event doesn't exist create it
    template<class EventType>
    internal::EventBuffer<EventType>* GetEventBuffer();

private:
    // Store event buffers
    std::vector<std::shared_ptr<internal::EventBufferBase>> m_event_buffers;

    // Count the number of events types stored in the bus
    EventTypeId m_type_id_last;
};

/*
 *
 *      Event bus template implementation
 *
 * */

// Return an event reader for this bus
template<class EventType>
EventReader<EventType> EventBus::GetEventReader() {
    return EventReader<EventType>( GetEventBuffer<EventType>() );
}

// Return an unique id for each event type added to this bus
template<class EventType>
EventBus::EventTypeId EventBus::GetEventTypeId() {
    static EventTypeId type_id = m_type_id_last++;
    return type_id;
}

// Add the event type to the bus if it doesn't already exist
template<class EventType>
void EventBus::AddEventType() {
    // Check if the component exist only for the first time
    bool type_exist = GetEventTypeId<EventType>() < m_event_buffers.size();

    if (!type_exist) {
        // Generate the two buffers for odd and even updates
        auto buffer = std::make_shared<internal::EventBuffer<EventType>>();

        // push them to the end of the vector
        m_event_buffers.push_back(std::move(buffer));
    }
}

// Get an event buffer for the specific type
// if the event doesn't exist create it
template<class EventType>
internal::EventBuffer<EventType>* EventBus::GetEventBuffer() {
    // Create the buffer if it doesn't already exist and get the type id
    AddEventType<EventType>();
    EventTypeId type_id = GetEventTypeId<EventType>();

    return static_cast<internal::EventBuffer<EventType>*>(
        m_event_buffers[type_id].get()
    );
}

} // namespace cndt

#endif
