#ifndef CNDT_EVENT_READER_H
#define CNDT_EVENT_READER_H

#include "conduit/defines.h"
#include "conduit/logging.h"

#include "conduit/internal/events/eventBuffer.h"

#include <memory>
#include <optional>

namespace cndt {

/*
 *
 *      Event reader declaration
 *
 * */

// Event reader iterator definition
template <typename EventType>
class EventIterator;

// Read component from the event bus that generated it
// this event reader should not be shared between threads 
template<typename EventType>
class EventReader 
{
    friend class EventIterator<EventType>;

    // Private event buffer type definition for readability
    using EventBufferPtr = std::weak_ptr<internal::EventBuffer<EventType>>;

public:
    // Event reader iterator 
    using Iterator = EventIterator<EventType>;   
    
public:
    EventReader(EventBufferPtr buffer);

    // Return an iterator pointing to the first event,
    // obtaining the iterator and incrementing it will consume events
    Iterator begin() { return Iterator(this, false); }

    // Return an iterator pointing to the end of the events queue
    Iterator end() { return Iterator(this, true); }
    
    // Return the number of not read events
    std::optional<usize> availableEvent();
    
private:
    // Return the next event on the bus with the same type of the event reader 
    // this function never return the same event
    const EventType* nextEvent(); 

    // Update the event buffer index and the last buffer update
    void updateIndex(u64 buffer_update);
    
private:
    // Store the buffer index reached by the reader in the current update
    usize m_current_buffer_index;
    usize m_old_buffer_index;

    // Store the last buffer update 
    u64 m_last_buffer_update;

    // Store the buffer event for the event reader type
    EventBufferPtr m_buffer_p;
};

// Event reader iterator definition
template<class EventType>
class EventIterator 
{
public:
    using iterator_category = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    
    using value_type        = EventType;
    using pointer           = const EventType*;
    using reference         = const EventType&;
    
    // Constructor the iterator from an event reader 
    EventIterator(EventReader<EventType> *reader_p, bool end) 
        : m_reader_p(reader_p), m_event_p(nullptr) 
    {
        if (!end) {
            m_event_p = m_reader_p->nextEvent();
        }
    }

    // Prefix increment
    EventIterator& operator++() 
    { 
        m_event_p = m_reader_p->nextEvent(); 
        return *this;
    } 
    // Postfix increment
    EventIterator operator++(int) 
    { 
        EventIterator tmp = *this;
        m_event_p = m_reader_p->nextEvent(); 
        return tmp;
    }        
    
    reference operator*() const { return *m_event_p; }
    pointer operator->() const { return m_event_p; }
    
    friend bool operator== (const EventIterator& a, const EventIterator& b) 
    {
        return a.m_event_p == b.m_event_p; 
    };
    friend bool operator!= (const EventIterator& a, const EventIterator& b) 
    {
        return a.m_event_p != b.m_event_p; 
    };     
    
private:
    // Store a reference to the parent event reader
    EventReader<EventType> *m_reader_p;
    
    // Point to the pointer to the current event
    pointer m_event_p;

};

/*
 *
 *      Event reader template implementation
 *
 * */

// Reader constructor
template<typename EventType>
EventReader<EventType>::EventReader(
    EventBufferPtr buffer
) : 
    m_current_buffer_index(0),
    m_old_buffer_index(0),
    m_last_buffer_update(0),
    m_buffer_p(buffer) 
{ }

// Update the event buffer index and the last buffer update
template<typename EventType>
void EventReader<EventType>::updateIndex(u64 buffer_update) {
    // Calculate the new index if the update counts don't match
    u64 delta_update = buffer_update - m_last_buffer_update;
    m_last_buffer_update = buffer_update;
    
    if (delta_update > 1) {
        m_old_buffer_index = 0;
        m_current_buffer_index = 0;
    } else if (delta_update == 1) {
        m_old_buffer_index = m_current_buffer_index;
        m_current_buffer_index = 0;
    }
}

// Return the next event on the bus with the same type of the event reader 
// this function never return the same event
template<typename EventType>
const EventType* EventReader<EventType>::nextEvent() 
{
    if (auto buffer_p = m_buffer_p.lock()) {
        // Update the buffer index
        updateIndex(buffer_p->m_update_count);
        
        auto new_buffer = buffer_p->getCurrentEvents(); 
        auto old_buffer = buffer_p->getOldEvents(); 
        
        // Get events starting from the oldest
        if (old_buffer->size() > m_old_buffer_index) {
            m_old_buffer_index += 1;
            return &old_buffer[m_old_buffer_index - 1];
            
        } else if (new_buffer->size() > m_current_buffer_index) {
            m_current_buffer_index += 1;
            return &new_buffer[m_current_buffer_index - 1];
        }
    } else {
        log::core::error(
            "EventReader::NextEvent -> buffer was deleted; Type: {}",
            typeid(EventType).name()
        );
    }
            
    return nullptr;
}

// Return the number of not read events
template <typename EventType>
std::optional<usize> EventReader<EventType>::availableEvent() {
    if (auto buffer_p = m_buffer_p.lock()) {
        // Update the buffer index
        updateIndex(buffer_p->m_update_count);
        
        // Calculate the events count for each buffer and add them together
        auto new_buffer = buffer_p->getCurrentEvents(); 
        auto old_buffer = buffer_p->getOldEvents(); 
        
        u64 old_count = old_buffer->size() - m_old_buffer_index;
        u64 new_count = new_buffer->size() - m_current_buffer_index;

        return old_count + new_count;
    } else {
        log::core::error(
            "EventReader::availableEvent -> buffer was deleted; Type: {}",
            typeid(EventType).name()
        );
    }
        
    return std::nullopt;
}
    
} // namespace cndt

#endif
