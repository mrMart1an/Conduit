#include "renderer/vulkan/vkRenderer.h"

namespace cndt::vulkan {

// Initialize the renderer implementation
void VkRenderer::initialize(
    const char *app_title,
    Window *window_p
) {
    m_context.initialize(app_title, window_p, VK_NULL_HANDLE);
}

// Shutdown the renderer implementation
void VkRenderer::shutdown() 
{
    m_context.shutdown();
}

// Resize the renderer viewport
void VkRenderer::resize(u32, u32) { }

} // namespace cndt::vulkan
