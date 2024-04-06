#ifndef CNDT_EVENT_REGISTER_H
#define CNDT_EVENT_REGISTER_H

#include "conduit/internal/events/eventBuffer.h"
#include "conduit/internal/events/typeRegister.h"

#include <memory>
#include <mutex>

namespace cndt::internal {

// Store events buffers for the different events types
class EventRegister {
    // Private type definition for readability
    template <class EventType>
    using EventBuffer = internal::EventBuffer<EventType>;
    
    using  EventBufferBase = internal::EventBufferBase;

public:
    EventRegister();
    
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
    std::mutex m_mutex;
    
    // Store event buffers
    std::vector<std::shared_ptr<EventBufferBase>> m_event_buffers;
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
    // Create the buffer if it doesn't already exist and get the type id
    addEventType<EventType>();
    auto type_id = EventTypeRegister::getTypeId<EventType>();

    // Cast the buffer shared pointer to a weak pointer
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::static_pointer_cast<internal::EventBuffer<EventType>>(
        m_event_buffers.at(type_id)
    );
}

// Add the event type to the bus if it doesn't already exist
template<class EventType>
void EventRegister::addEventType() 
{
    // Get the unique event id
    auto type_id = EventTypeRegister::getTypeId<EventType>();
    
    std::lock_guard<std::mutex> lock(m_mutex);

    // Check if resizing the event buffer is necessary 
    if (m_event_buffers.size() <= type_id) 
        m_event_buffers.resize(type_id + 1);
    
    // Create the buffer if it doesn't already exist
    bool type_exist = m_event_buffers[type_id] != nullptr;
    
    if (!type_exist) {
        auto buffer = std::make_shared<internal::EventBuffer<EventType>>();
        m_event_buffers[type_id] = std::move(buffer);
    }
}

}; // namespace cndt::internal

#endif
