#ifndef CNDT_CALLBACK_REGISTER_H
#define CNDT_CALLBACK_REGISTER_H

#include "conduit/internal/events/eventBuffer.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>
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
    // Execute all the callbacks in the register
    void ExecuteCallback();

    // Add a callback to the callback register
    template<class EventType>
    void AddCallback(
        EventBufferPtr<EventType> event_buffer_p,
        CallbackFn<EventType> callback_fn
    );
    
private:
    // Return an unique id for each event type added to this register
    template<class EventType>
    EventTypeId GetEventTypeId();     
    
    // Add the event type to the register if it doesn't already exist
    template<class EventType>
    void AddEventType(EventBufferPtr<EventType> event_buffer_p);
    
private:
    // Event buffer vector
    std::vector<std::unique_ptr<CallbackBufferBase>> m_callback_buffers;

    // Count the number of events types stored in the register
    EventTypeId m_type_id_last;
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

/*
 *
 *      Callback register implementation
 *
 * */
    
// Add a callback to the callback register
template<class EventType>
void CallbackRegister::AddCallback(
    CallbackRegister::EventBufferPtr<EventType> event_buffer_p,
    CallbackRegister::CallbackFn<EventType> callback_fn
) {
    // Create the buffer if it doesn't already exist and get the type id
    AddEventType<EventType>(std::move(event_buffer_p));
    EventTypeId type_id = GetEventTypeId<EventType>();
    
    // Get a raw pointer to the callback buffer
    auto buffer = static_cast<CallbackBuffer<EventType>*>(
        m_callback_buffers.at(type_id).get()
    );

    // Add the callback
    buffer->AddCallback(callback_fn);
}

// Return an unique id for each event type added to this register
template<class EventType>
CallbackRegister::EventTypeId CallbackRegister::GetEventTypeId() 
{
    static EventTypeId type_id = m_type_id_last++;
    return type_id;
} 

// Add the event type to the register if it doesn't already exist
template<class EventType>
void CallbackRegister::AddEventType(
    CallbackRegister::EventBufferPtr<EventType> event_buffer_p
) {
    // Check if the component already exist 
    bool type_exist = GetEventTypeId<EventType>() < m_callback_buffers.size();

    if (!type_exist) {
        // Generate the two buffers for odd and even updates
        auto buffer = std::make_unique<internal::CallbackBuffer<EventType>>(
            std::move(event_buffer_p)
        );

        // push them to the end of the vector
        m_callback_buffers.push_back(std::move(buffer));
    }
}

} // namespace cndt::internal

#endif
