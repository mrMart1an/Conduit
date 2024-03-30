
#ifndef CNDT_CALLBACK_BUFFER_H
#define CNDT_CALLBACK_BUFFER_H

#include "conduit/internal/events/eventBuffer.h"

#include <functional>
#include <memory>
#include <vector>

namespace cndt::internal {

constexpr usize callback_buffer_default_size = 5;

// Generic event callback class
class CallbackBufferBase {
public:
    CallbackBufferBase() = default;
    virtual ~CallbackBufferBase() = default;

    virtual void CallAll() = 0;   
};

// Type specific event callback class
template <class EventType>
class CallbackBuffer : public CallbackBufferBase {
    // Callback function type
    using CallbackFn = std::function<void(const EventType*)>;

    // Event buffer weak pointer type for readability
    using EventBufferPtr = std::weak_ptr<EventBuffer<EventType>>;

public:    
    // Build callback buffer storing the event type for the callbacks
    CallbackBuffer(EventBufferPtr event_buffer);
    ~CallbackBuffer() = default;

    // Add a callback to the buffer
    void AddCallback(CallbackFn callback_fn);
    
    // Call all the callbacks in the buffer
    void CallAll() override;

private:
    // Callback buffer
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

// Add a callback to the buffer
template <class EventType>
void CallbackBuffer<EventType>::AddCallback(
    CallbackBuffer::CallbackFn callback_fn
) {
    // Add the callback to the buffer
    m_callback_buffer.push_back(callback_fn);
}

// Call all the callbacks in the buffer
template <class EventType>
void CallbackBuffer<EventType>::CallAll() 
{
    // Get the current event buffer
    if (auto event_buffer = m_event_buffer_p.lock()) {
        auto& current_events = event_buffer->GetCurrentEvents();

        // Run all the callbacks on the buffer
        for (auto callback_fn : m_callback_buffer) {
            //  Run the callback for all the new events
            for (auto& event : current_events) {
                callback_fn(&event);
            }
        }
    }
}

} // namespace cndt::internal

#endif
