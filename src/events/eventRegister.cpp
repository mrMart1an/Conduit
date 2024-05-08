#include "conduit/internal/events/eventRegister.h"

namespace cndt::internal {

// Swap and clear the event buffers
void EventRegister::update() 
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    for (auto& buffer : m_event_buffers) {
        buffer.second->update();
    }
}

} // namespace cndt::internal
