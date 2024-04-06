#ifndef CNDT_EVENT_BUFFER_H
#define CNDT_EVENT_BUFFER_H

#include "conduit/defines.h"

#include <atomic>
#include <mutex>
#include <shared_mutex>
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
    virtual void update() = 0;

protected:
    // Return true if the current update is odd
    inline bool updateIsOdd() { return m_update_count % 2; };

protected:
    // Update counter, used by the bus to swap between buffers 
    // and by the event readers to keep track of already read events
    std::atomic<u64> m_update_count;
};

// Event buffer type specific implementation  
template <class EventType>
class EventBuffer : public EventBufferBase {
public:
    // Thread safe buffer access pointer class
    class Buffer {
    public:
        Buffer(std::shared_mutex& mutex, std::vector<EventType> *buffer_p)
            : m_mutex_lock(mutex), m_buffer_p(buffer_p) 
        { }
        
        // arrow operator access
        const std::vector<EventType>* operator->() { return m_buffer_p; }
        // dereferencing operator access
        const std::vector<EventType>& operator*() { return *(m_buffer_p); }
        
        // subscript operator
        EventType& operator[] (int index) { return m_buffer_p->at(index); }
        
    private:
        std::shared_lock<std::shared_mutex> m_mutex_lock;

        // Store a pointer to the actual buffer vector
        std::vector<EventType> *m_buffer_p;
    };

public:
    EventBuffer();

    // Swap buffers and clear old events
    void update() override;
    
    // Append an event to the buffer
    void append(const EventType& event);

    // Return a read only event buffer object to the current event buffer
    Buffer getCurrentEvents();
    // Return a read only event buffer object to the last frame event buffer
    Buffer getOldEvents();
    
private:
    // Store the events during odd updates
    std::vector<EventType> m_events_odd;
    std::shared_mutex m_mutex_odd;
    
    // Store the events during odd updates
    std::vector<EventType> m_events_even;
    std::shared_mutex m_mutex_even;

};

/*
 *
 *      Event buffer implementation
 *
 * */

// Event buffer constructor
template <class EventType>
EventBuffer<EventType>::EventBuffer() {
    // Reserve the buffer with the default size
    m_events_odd.reserve(event_buffer_default_size);
    m_events_even.reserve(event_buffer_default_size);
}

// Swap buffers and clear old events
template <class EventType>
void EventBuffer<EventType>::update() {
    // Clear the buffer for the next update
    if (updateIsOdd()) {
        std::unique_lock<std::shared_mutex> lock(m_mutex_even);
        m_events_even.clear();
    } else {
        std::unique_lock<std::shared_mutex> lock(m_mutex_odd);
        m_events_odd.clear();
    }

    // Increase the event buffers
    m_update_count += 1;
}

// Append an event to the buffer
template <class EventType>
void EventBuffer<EventType>::append(const EventType& event) {
    if (updateIsOdd()) {
        std::unique_lock<std::shared_mutex> lock(m_mutex_odd);
        m_events_odd.push_back(event);
    } else {
        std::unique_lock<std::shared_mutex> lock(m_mutex_even);
        m_events_even.push_back(event);
    }
}

// Return a reference to the events in the current update vectors
template <class EventType>
EventBuffer<EventType>::Buffer EventBuffer<EventType>::getCurrentEvents() {
    if (updateIsOdd()) {
        return Buffer(m_mutex_odd, &m_events_odd);
    } else {
        return Buffer(m_mutex_even, &m_events_even);
    }
}

// Return a reference to the events in the last update vectors
template <class EventType>
EventBuffer<EventType>::Buffer EventBuffer<EventType>::getOldEvents() {
    if (!updateIsOdd()) {
        return Buffer(m_mutex_odd, &m_events_odd);
    } else {
        return Buffer(m_mutex_even, &m_events_even);
    }
}

} // namespace cndt::internal

#endif
