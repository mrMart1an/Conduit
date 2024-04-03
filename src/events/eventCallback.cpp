#include "conduit/internal/events/callbackRegister.h"

namespace cndt::internal {

// Execute all the callbacks in the register
void CallbackRegister::executeCallback() 
{
    // Execute the callback in all the callback buffers
    for (auto& buffer : m_callback_buffers) {
        buffer->callAll();
    }
}

} // namespace cndt::internal
