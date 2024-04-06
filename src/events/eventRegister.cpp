#include "conduit/internal/events/eventRegister.h"
#include "conduit/defines.h"

namespace cndt::internal {

// Default starting capacity of the buffers storage vectors
constexpr usize default_buffers_size = 30;

EventRegister::EventRegister()
{ 
    std::lock_guard<std::mutex> lock(m_mutex);
    m_event_buffers.reserve(default_buffers_size);
}

// Swap and clear the event buffers
void EventRegister::update() 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& buffer : m_event_buffers) {
        buffer->update();
    }
}

} // namespace cndt::internal
