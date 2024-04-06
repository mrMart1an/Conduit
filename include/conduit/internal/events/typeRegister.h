#ifndef CNDT_EVENT_TYPE_REGISTER_H
#define CNDT_EVENT_TYPE_REGISTER_H

#include "conduit/defines.h"

#include <atomic>

namespace cndt::internal {

// Track event type and event types ids
class EventTypeRegister {
public:
    using TypeId = u64;
    
    // Return an unique id for the given event type
    template <typename EventType>
    static TypeId getTypeId() {
        static std::atomic<TypeId> id(getNextTypeId());
        return id;
    }
    
private:
    // Make the class non constructable
    EventTypeRegister() = delete;

    // Return an unique id to initialize the event type id 
    // static variable of the child event struct
    static TypeId getNextTypeId() {
        static std::atomic<TypeId> m_last_id(0);
        return m_last_id++;
    }
};

} // namespace cndt::internal

#endif
