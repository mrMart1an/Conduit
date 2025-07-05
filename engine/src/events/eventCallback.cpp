#include "conduit/internal/events/callbackRegister.h"

namespace cndt::internal {

// Default starting capacity of the buffers storage vectors
constexpr usize default_callback_buffer_size = 5;

CallbackRegister::CallbackRegister()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callback_buffers.reserve(default_callback_buffer_size);
}

// Execute all the callbacks in the register
void CallbackRegister::executeCallback() 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Execute the callback in all the callback buffers
    for (auto& buffer : m_callback_buffers) {
        buffer->callAll();
    }
}

} // namespace cndt::internal
