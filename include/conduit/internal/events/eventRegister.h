#ifndef CNDT_EVENT_REGISTER_H
#define CNDT_EVENT_REGISTER_H

#include "conduit/internal/events/eventBuffer.h"

#include <memory>

namespace cndt::internal {

// Store events buffers for the different events types
class EventRegister {
    // Private type definition for readability
    template <class EventType>
    using EventBuffer = internal::EventBuffer<EventType>;
    
    using  EventBufferBase = internal::EventBufferBase;

    // Store an id to a event type
    using EventTypeId = u64;
    
public:
    EventRegister();
    
    // Swap and clear the event buffers
    void Update();
    
    // Get an event buffer for the specific type
    // if the event doesn't exist create it
    template<class EventType>
    std::weak_ptr<EventBuffer<EventType>> GetEventBuffer();

private:
    // Return an unique id for each event type added to this register
    template<class EventType>
    EventTypeId GetEventTypeId();     
    
    // Add the event type to the register if it doesn't already exist
    template<class EventType>
    void AddEventType();
    
private:
    // Store event buffers
    std::vector<std::shared_ptr<EventBufferBase>> m_event_buffers;
    
    // Count the number of events types stored in the register
    EventTypeId m_type_id_last;
};

/*
 *
 *      Event register implementation
 *
 * */

// Get an event buffer for the specific type
// if the event doesn't exist create it
template<class EventType>
std::weak_ptr<EventBuffer<EventType>> EventRegister::GetEventBuffer() 
{
    // Create the buffer if it doesn't already exist and get the type id
    AddEventType<EventType>();
    EventTypeId type_id = GetEventTypeId<EventType>();

    return std::static_pointer_cast<internal::EventBuffer<EventType>>(
        m_event_buffers.at(type_id)
    );
}

// Return an unique id for each event type added to this bus
template<class EventType>
EventRegister::EventTypeId EventRegister::GetEventTypeId() 
{
    static EventTypeId type_id = m_type_id_last++;
    return type_id;
}

// Add the event type to the bus if it doesn't already exist
template<class EventType>
void EventRegister::AddEventType() 
{
    // Check if the component already exist 
    bool type_exist = GetEventTypeId<EventType>() < m_event_buffers.size();

    if (!type_exist) {
        // Generate the two buffers for odd and even updates
        auto buffer = std::make_shared<internal::EventBuffer<EventType>>();

        // push them to the end of the vector
        m_event_buffers.push_back(std::move(buffer));
    }
}

}; // namespace cndt::internal

#endif
