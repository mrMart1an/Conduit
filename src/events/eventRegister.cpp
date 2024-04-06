#include "conduit/internal/events/eventRegister.h"
#include "conduit/defines.h"

namespace cndt::internal {

// Default starting capacity of the buffers storage vectors
constexpr usize default_buffers_size = 30;

EventRegister::EventRegister()
{ 
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_event_buffers.reserve(default_buffers_size);
}

// Swap and clear the event buffers
void EventRegister::update() 
{
    for (auto& buffer : m_event_buffers) {
        buffer->update();
    }
}

} // namespace cndt::internal
