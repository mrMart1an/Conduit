#ifndef CNDT_EVENT_REGISTER_H
#define CNDT_EVENT_REGISTER_H

#include "conduit/internal/events/eventBuffer.h"
#include "conduit/internal/events/typeRegister.h"

#include <map>
#include <memory>
#include <shared_mutex>

namespace cndt::internal {

// Store events buffers for the different events types
class EventRegister {
public:
    EventRegister() = default;
    
    // Swap and clear the event buffers
    void update();
    
    // Get an event buffer for the specific type
    // if the event doesn't exist create it
    template<class EventType>
    std::weak_ptr<EventBuffer<EventType>> getEventBuffer();

private:
    // Add the event type to the register if it doesn't already exist
    template<class EventType>
    void addEventType();
    
private:
    std::shared_mutex m_mutex;
    
    // Store event buffers
    using TypeId = EventTypeRegister::TypeId;
    using EventBufferPtr = std::shared_ptr<EventBufferBase>;
    
    std::map<TypeId, EventBufferPtr> m_event_buffers;
};

/*
 *
 *      Event register implementation
 *
 * */

// Get an event buffer for the specific type
// if the event doesn't exist create it
template<class EventType>
std::weak_ptr<EventBuffer<EventType>> EventRegister::getEventBuffer() 
{
    // Create the buffer if it doesn't already exist
    addEventType<EventType>();

    // Cast the buffer shared pointer to a weak pointer
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    auto type_id = EventTypeRegister::getTypeId<EventType>();
    return std::static_pointer_cast<EventBuffer<EventType>>(
        m_event_buffers[type_id]
    );
}

// Add the event type to the bus if it doesn't already exist
template<class EventType>
void EventRegister::addEventType() 
{
    // Get the unique event id
    auto type_id = EventTypeRegister::getTypeId<EventType>();
    
    // Create the buffer if it doesn't already exist
    bool type_exist = false;
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        if (m_event_buffers.find(type_id) != m_event_buffers.end())
            type_exist = true;
    }

    if (!type_exist) {
        std::lock_guard<std::shared_mutex> lock(m_mutex);
        
        auto buffer = std::make_shared<internal::EventBuffer<EventType>>();
        m_event_buffers[type_id] = std::move(buffer);
    }
}

}; // namespace cndt::internal

#endif
