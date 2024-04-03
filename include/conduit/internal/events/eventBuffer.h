#ifndef CNDT_EVENT_BUFFER_H
#define CNDT_EVENT_BUFFER_H

#include "conduit/defines.h"

#include <vector>

namespace cndt {

// Predeclare the event reader class
template <class EventType>
class EventReader;

} // namespace cndt

namespace cndt::internal {

// Event buffer starting capacity
constexpr usize event_buffer_default_size = 10;

/*
 *
 *      Event buffer definition
 *
 * */

// Base event buffer class
class EventBufferBase {
    template <class EventType>
    friend class cndt::EventReader;
    
public:
    EventBufferBase() = default;
    virtual ~EventBufferBase() = default;

    // Swap buffers and clear old events
    virtual void Update() = 0;

protected:
    // Return true if the current update is odd
    inline bool UpdateIsOdd() { return m_update_count % 2; };

protected:
    // Update counter, used by the bus to swap between buffers 
    // and by the event readers to keep track of already read events
    u64 m_update_count;
};

// Event buffer type specific implementation  
template <class EventType>
class EventBuffer : public EventBufferBase {
public:
    EventBuffer();

    // Swap buffers and clear old events
    void Update() override;
    
    // Append an event to the buffer
    void Append(const EventType& event);

    // Return a reference to the events in the current update vectors
    std::vector<EventType>& GetCurrentEvents();
    // Return a reference to the events in the last update vectors
    std::vector<EventType>& GetOldEvents();
    
private:
    // Store the events during odd updates
    std::vector<EventType> m_events_odd;
    // Store the events during odd updates
    std::vector<EventType> m_events_even;
};

/*
 *
 *      Event buffer implementation
 *
 * */

// Event buffer constructor
template <class EventType>
EventBuffer<EventType>::EventBuffer() {
    // Reserve the buffer default size
    m_events_odd.reserve(event_buffer_default_size);
    m_events_even.reserve(event_buffer_default_size);
}

// Swap buffers and clear old events
template <class EventType>
void EventBuffer<EventType>::Update() {
    // Clear the buffer for the next update
    GetOldEvents().clear();

    // Increase the event buffers
    m_update_count += 1;
}

// Append an event to the buffer
template <class EventType>
void EventBuffer<EventType>::Append(const EventType& event) {
    GetCurrentEvents().push_back(event);
}

// Return a reference to the events in the current update vectors
template <class EventType>
std::vector<EventType>& EventBuffer<EventType>::GetCurrentEvents() {
    return UpdateIsOdd() ? m_events_odd : m_events_even;
}

// Return a reference to the events in the last update vectors
template <class EventType>
std::vector<EventType>& EventBuffer<EventType>::GetOldEvents() {
    return UpdateIsOdd() ? m_events_even : m_events_odd;
}

} // namespace cndt::internal

#endif
