#include "conduit/internal/events/eventRegister.h"
#include "conduit/defines.h"

namespace cndt::internal {

// Default starting capacity of the buffers storage vectors
constexpr usize default_buffers_size = 30;

EventRegister::EventRegister()
    : m_type_id_last(0)
{ 
    m_event_buffers.reserve(default_buffers_size);
}

// Swap and clear the event buffers
void EventRegister::Update() {
    for (auto& buffer : m_event_buffers) {
        buffer->Update();
    }
}

} // namespace cndt::internal
