
#ifndef CNDT_CALLBACK_BUFFER_H
#define CNDT_CALLBACK_BUFFER_H

#include "conduit/internal/events/eventBuffer.h"
#include "conduit/logging.h"

#include <functional>
#include <memory>
#include <vector>

namespace cndt::internal {

// Default size of the callback buffers vectors
constexpr usize callback_buffer_default_size = 2;

// Generic event callback buffer class base
class CallbackBufferBase {
public:
    CallbackBufferBase() = default;
    virtual ~CallbackBufferBase() = default;

    // Call all the callbacks in the buffer using the events stored
    // in the event buffer provided during the callback buffer construction 
    virtual void callAll() = 0;   
};

// Type specific event callback buffer class
template <class EventType>
class CallbackBuffer : public CallbackBufferBase {
    // Callback function type
    using CallbackFn = std::function<void(const EventType*)>;

    // Event buffer weak pointer type for readability
    using EventBufferPtr = std::weak_ptr<EventBuffer<EventType>>;

public:    
    // Build callback buffer storing the event buffer
    // used to call the callbacks
    CallbackBuffer(EventBufferPtr event_buffer);
    ~CallbackBuffer() = default;

    // Add a callback function to the buffer
    void addCallback(CallbackFn callback_fn);
    
    // Call all the callbacks in the buffer using the events stored
    // in the event buffer provided during the callback buffer construction 
    void callAll() override;

private:
    // Callback function buffer
    std::vector<CallbackFn> m_callback_buffer;
    
    // Store the event buffer associated with the callbacks type
    EventBufferPtr m_event_buffer_p;
};

/*
 *
 *      Callback buffer implementation
 *
 * */

// Build callback buffer storing the event type for the callbacks
template <class EventType>
CallbackBuffer<EventType>::CallbackBuffer(
    CallbackBuffer::EventBufferPtr event_buffer
) 
    : m_event_buffer_p(event_buffer)
{ 
    m_callback_buffer.reserve(callback_buffer_default_size);
}

// Add a callback function to the buffer
template <class EventType>
void CallbackBuffer<EventType>::addCallback(
    CallbackBuffer::CallbackFn callback_fn
) {
    // Add the callback function to the buffer
    m_callback_buffer.push_back(callback_fn);
}

// Call all the callbacks in the buffer
template <class EventType>
void CallbackBuffer<EventType>::callAll() 
{
    // Get the current event buffer
    if (auto event_buffer = m_event_buffer_p.lock()) {
        std::vector<EventType>& current_events = 
            event_buffer->getCurrentEvents();

        // Run all the callbacks on the buffer
        for (auto& callback_fn : m_callback_buffer) {
            //  Run the callback for all the new events
            for (auto& event : current_events) {
                callback_fn(&event);
            }
        }
        
    } else {
        // If the event buffer was deleted log a error message
        log::core::error(
            "CallbackBuffer::CallAll -> event buffer was deleted; Type: {}",
            typeid(EventType).name()
        );
    }
}

} // namespace cndt::internal

#endif
