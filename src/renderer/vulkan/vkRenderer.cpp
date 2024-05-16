#include "renderer/vulkan/vkRenderer.h"
#include <functional>

namespace cndt::vulkan {

// Initialize the renderer implementation
void VkRenderer::initialize(
    const char *app_title,
    Window *window_p
) {
    // Initialize the vulkan context
    m_context.initialize(app_title, window_p, VK_NULL_HANDLE);
    m_delete_queue.addDeleter(std::bind(&Context::shutdown, m_context));
}

// Shutdown the renderer implementation
void VkRenderer::shutdown() 
{
    m_delete_queue.callDeleter();
}

// Resize the renderer viewport
void VkRenderer::resize(u32, u32) { }

} // namespace cndt::vulkan
