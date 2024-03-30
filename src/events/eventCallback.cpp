#include "conduit/internal/events/callbackRegister.h"

namespace cndt::internal {

// Execute all the callbacks in the register
void CallbackRegister::ExecuteCallback() 
{
    // Execute the callback in all the callback buffers
    for (auto& buffer : m_callback_buffers) {
        buffer->CallAll();
    }
}

} // namespace cndt::internal
