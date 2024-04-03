#ifndef CNDT_CALLBACK_REGISTER_H
#define CNDT_CALLBACK_REGISTER_H

#include "conduit/internal/events/eventBuffer.h"
#include "conduit/internal/events/callbackBuffer.h"

#include <functional>
#include <memory>
#include <vector>

namespace cndt::internal {

// Event callbacks register,
// Store all the callback buffers
class CallbackRegister {
    // Store an id to a event type
    using EventTypeId = u64;
    
    // Callback function type
    template <class EventType>
    using CallbackFn = std::function<void(const EventType*)>;
    
    // Event buffer weak pointer type for readability
    template <class EventType>
    using EventBufferPtr = std::weak_ptr<EventBuffer<EventType>>;
    
public:
    // Execute all the callbacks in the register for all the event types
    void executeCallback();

    // Add a callback function to the callback register
    template<class EventType>
    void addCallback(
        EventBufferPtr<EventType> event_buffer_p,
        CallbackFn<EventType> callback_fn
    );
    
private:
    // Return an unique id for each event type added to this register
    template<class EventType>
    EventTypeId getEventTypeId();     
    
    // Add the event type to the register if it doesn't already exist
    template<class EventType>
    void addEventType(EventBufferPtr<EventType> event_buffer_p);
    
private:
    // Event buffer vector
    std::vector<std::unique_ptr<CallbackBufferBase>> m_callback_buffers;

    // Count the number of events types stored in the register
    EventTypeId m_type_id_last;
};

/*
 *
 *      Callback register implementation
 *
 * */
 
// Add a callback to the callback register
template<class EventType>
void CallbackRegister::addCallback(
    CallbackRegister::EventBufferPtr<EventType> event_buffer_p,
    CallbackRegister::CallbackFn<EventType> callback_fn
) {
    // Create the buffer if it doesn't already exist and get the type id
    addEventType<EventType>(std::move(event_buffer_p));
    EventTypeId type_id = getEventTypeId<EventType>();
    
    // Get a raw pointer to the callback buffer
    auto buffer = static_cast<CallbackBuffer<EventType>*>(
        m_callback_buffers.at(type_id).get()
    );

    // Add the callback
    buffer->addCallback(callback_fn);
}

// Return an unique id for each event type added to this register
template<class EventType>
CallbackRegister::EventTypeId CallbackRegister::getEventTypeId() 
{
    static EventTypeId type_id = m_type_id_last++;
    return type_id;
} 

// Add the event type to the register if it doesn't already exist
template<class EventType>
void CallbackRegister::addEventType(
    CallbackRegister::EventBufferPtr<EventType> event_buffer_p
) {
    // Static variable initialize to false the first time the function
    // is called, this variable is set to true after adding 
    // the type to the register
    static bool type_exist = false;

    // Check if the component already exist 
    if (!type_exist) {
        // Generate the two buffers for odd and even updates
        auto buffer = std::make_unique<internal::CallbackBuffer<EventType>>(
            std::move(event_buffer_p)
        );

        // push them to the end of the vector
        m_callback_buffers.push_back(std::move(buffer));

        // Set type exist to true for the given type
        type_exist = true;
    }
}

} // namespace cndt::internal

#endif
