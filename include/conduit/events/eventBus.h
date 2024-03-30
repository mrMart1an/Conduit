#ifndef CNDT_EVENT_BUS_H
#define CNDT_EVENT_BUS_H

#include "conduit/events/eventReader.h"
#include "conduit/events/eventWriter.h"

#include "conduit/internal/events/callbackRegister.h"
#include "conduit/internal/events/eventRegister.h"

#include <functional>
#include <memory>

namespace cndt {

/*
 *
 *      Event bus declaration
 *
 * */

// Double buffered event bus
class EventBus {
    // Private type definition for readability
    template <class EventType>
    using EventBufferPtr = std::shared_ptr<internal::EventBuffer<EventType>>;
    
    // Store an id to a event type
    using EventTypeId = u64;
    
    // Callback function type
    template <class EventType>
    using CallbackFn = std::function<void(const EventType*)>;
    
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

    // Add callbacks to the bus
    template<class EventType>
    void AddCallback(CallbackFn<EventType> callback_fn);

private:
    // Store event buffers
    std::shared_ptr<internal::EventRegister> m_event_register;

    // Store the event callbacks
    internal::CallbackRegister m_callback_register;
};

/*
 *
 *      Event bus template implementation
 *
 * */

// Return an event reader for this bus
template<class EventType>
EventReader<EventType> EventBus::GetEventReader() {
    return EventReader<EventType>( 
        m_event_register->GetEventBuffer<EventType>() 
    );
}

// Add callbacks to the bus
template<class EventType>
void EventBus::AddCallback(
    EventBus::CallbackFn<EventType> callback_fn
) {
    auto event_buffer_p = m_event_register->GetEventBuffer<EventType>();
    m_callback_register.AddCallback<EventType>(event_buffer_p, callback_fn);
}

} // namespace cndt

#endif
