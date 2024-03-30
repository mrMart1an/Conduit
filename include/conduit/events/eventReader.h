#ifndef CNDT_EVENT_READER_H
#define CNDT_EVENT_READER_H

#include "conduit/defines.h"
#include "conduit/internal/events/eventBuffer.h"
#include "conduit/events/eventBus.h"

#include <memory>

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
    // Private type definition for readability
    using EventBufferPtr = std::weak_ptr<internal::EventBuffer<EventType>>;
    
public:
    EventReader(EventBufferPtr buffer);
    
    // Return the next event on the bus with the same type of the event reader 
    // this function never return the same event
    // TEMPORARY (Iterator in the future)
    const EventType* NextEvent(); 
    
private:
    // Store the buffer index reached by the reader in the current update
    usize m_current_buffer_index;
    usize m_old_buffer_index;

    // Store the last buffer update 
    u64 m_last_buffer_update;

    // Store the buffer event for the event reader type
    EventBufferPtr m_buffer_p;
};

/*
 *
 *      Event reader template implementation
 *
 * */

// Reader constructor
template<class EventType>
EventReader<EventType>::EventReader(
    EventBufferPtr buffer
) : 
    m_current_buffer_index(0),
    m_old_buffer_index(0),
    m_last_buffer_update(0),
    m_buffer_p(buffer) 
{ }

// Return the next event on the bus with the same type of the event reader 
// this function never return the same event
template<class EventType>
const EventType* EventReader<EventType>::NextEvent() {
    if (auto buffer_p = m_buffer_p.lock()) {
        auto& new_buffer = buffer_p->GetCurrentEvents(); 
        auto& old_buffer = buffer_p->GetOldEvents(); 
    
        // Calculate the new index if the update counts don't match
        u64 delta_update = buffer_p->m_update_count - m_last_buffer_update;
        m_last_buffer_update = buffer_p->m_update_count;
        
        if (delta_update > 1) {
            m_old_buffer_index = 0;
            m_current_buffer_index = 0;
        } else if (delta_update == 1) {
            m_old_buffer_index = m_current_buffer_index;
            m_current_buffer_index = 0;
        }
        
        // Get events starting from the oldest
        if (old_buffer.size() > m_old_buffer_index) {
            m_old_buffer_index += 1;
            return &old_buffer[m_old_buffer_index - 1];
            
        } else if (new_buffer.size() > m_current_buffer_index) {
            m_current_buffer_index += 1;
            return &new_buffer[m_current_buffer_index - 1];
        }
    }
            
    // Return nullopt if no events are available
    return nullptr;
}

} // namespace cndt

#endif
