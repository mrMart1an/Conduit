#ifndef CNDT_CALLBACK_REGISTER_H
#define CNDT_CALLBACK_REGISTER_H

#include "conduit/internal/events/callbackBuffer.h"
#include "conduit/internal/events/typeRegister.h"

#include <functional>
#include <memory>
#include <vector>

namespace cndt::internal {

// Event callbacks register,
// Store all the callback buffers
class CallbackRegister {
    // Callback function type
    template <class EventType>
    using CallbackFn = std::function<void(const EventType*)>;
    
    // Event buffer weak pointer type for readability
    template <class EventType>
    using EventBufferPtr = std::weak_ptr<EventBuffer<EventType>>;
    
public:
    CallbackRegister();

    // Execute all the callbacks in the register for all the event types
    void executeCallback();

    // Add a callback function to the callback register
    template<class EventType>
    void addCallback(
        EventBufferPtr<EventType> event_buffer_p,
        CallbackFn<EventType> callback_fn
    );
    
private:
    // Add the event type to the register if it doesn't already exist
    template<class EventType>
    void addEventType(EventBufferPtr<EventType> event_buffer_p);
    
private:
    // Event buffer vector
    std::vector<std::unique_ptr<CallbackBufferBase>> m_callback_buffers;
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
    
    // Get the unique event id
    auto type_id = EventTypeRegister::getTypeId<EventType>();
    
    // Get a raw pointer to the callback buffer
    auto buffer = static_cast<CallbackBuffer<EventType>*>(
        m_callback_buffers.at(type_id).get()
    );

    // Add the callback
    buffer->addCallback(callback_fn);
}

// Add the event type to the register if it doesn't already exist
template<class EventType>
void CallbackRegister::addEventType(
    CallbackRegister::EventBufferPtr<EventType> event_buffer_p
) {
    // Get the unique event id
    auto type_id = EventTypeRegister::getTypeId<EventType>();
    
    // Check if resizing the event buffer is necessary 
    if (m_callback_buffers.size() <= type_id) 
        m_callback_buffers.resize(type_id + 1);

    // Check if the component already exist 
    bool type_exist = m_callback_buffers[type_id] != nullptr;

    // Create the buffer if it doesn't already exist
    if (!type_exist) {
        auto buffer = std::make_unique<internal::CallbackBuffer<EventType>>(
            std::move(event_buffer_p)
        );

        m_callback_buffers[type_id] = std::move(buffer);
    }
}

} // namespace cndt::internal

#endif
