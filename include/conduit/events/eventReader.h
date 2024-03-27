#ifndef CNDT_EVENT_READER_H
#define CNDT_EVENT_READER_H

#include "conduit/defines.h"
#include "conduit/internal/events/eventBuffer.h"
#include "conduit/events/eventBus.h"
#include <optional>

namespace cndt {

class EventBus;

/*
 *
 *      Event reader declaration
 *
 * */

// Read component from the event bus that generated it
template<class EventType>
class EventReader {
public:
    EventReader(internal::EventBuffer<EventType> *buffer_p);
    
    // Return the next event on the bus with the same type of the event reader 
    // this function never return the same event
    // TEMPORARY (Iterator in the future)
    std::optional<EventType> NextEvent(); 
    
private:
    // Private type definition for simplicity
    typedef internal::EventBuffer<EventType> EventBuffer;

    // Store the buffer index index reached by the reader for each buffer
    usize m_current_buffer_index;
    usize m_old_buffer_index;

    // Store the last buffer update 
    u64 m_last_buffer_update;

    // Store the buffer event for the event reader type
    EventBuffer *m_buffer_p;
};

/*
 *
 *      Event reader template implementation
 *
 * */

// Reader constructor
template<class EventType>
EventReader<EventType>::EventReader(
    internal::EventBuffer<EventType> *buffer_p
) : 
    m_current_buffer_index(0),
    m_old_buffer_index(0),
    m_last_buffer_update(0),
    m_buffer_p(buffer_p) 
{ }

template<class EventType>
std::optional<EventType> EventReader<EventType>::NextEvent() {
    auto& new_buffer = m_buffer_p->GetCurrentEvents(); 
    auto& old_buffer = m_buffer_p->GetOldEvents(); 

    // Calculate the new index if the update counts don't match
    u64 delta_update = m_buffer_p->m_update_count - m_last_buffer_update;
    m_last_buffer_update = m_buffer_p->m_update_count;
    
    if (delta_update > 1) {
        m_old_buffer_index = 0;
        m_current_buffer_index = 0;
    } else if (delta_update == 1) {
        m_old_buffer_index = m_current_buffer_index;
        m_current_buffer_index = 0;
    }
    
    // Get events starting from the oldest
    EventType event = { 0 };
    
    if (old_buffer.size() > m_old_buffer_index) {
        event = old_buffer[m_old_buffer_index];
        m_old_buffer_index += 1;
        
    } else if (new_buffer.size() > m_current_buffer_index) {
        event = new_buffer[m_current_buffer_index];
        m_current_buffer_index += 1;
    } else {
        // Return nullopt if no events are available
        return std::nullopt;
    }

    return event;
}

/*
 *
 *      Event reader iterator implementation
 *
 * */

} // namespace cndt

#endif
