#include "renderer/vulkan/vkRenderer.h"
#include "renderer/vulkan/vkDevice.h"

#include <functional>

namespace cndt::vulkan {

// Initialize the renderer implementation
void VkRenderer::initialize(
    const char *app_title,
    Window *window_p
) {
    // Initialize the vulkan context
    m_context.initialize(app_title, window_p, VK_NULL_HANDLE);
    m_delete_queue.addDeleter(std::bind(&Context::shutdown, &m_context));

    // Initialize the vulkan device
    Device::PhysicalDeviceRequirement requirement = {};
    
    // Enable swap chain extensions
    requirement.required_device_extensions = Device::Extensions(true);
    // Require graphics, transfer and present of queue
    requirement.required_queue =
        Device::QueueFamilyType(true, false, true, true);
    // Require features 
    requirement.required_feature.geometryShader = VK_TRUE;
    requirement.required_feature.fillModeNonSolid = VK_TRUE;
    
    m_device.initialize(&m_context, requirement);
    m_delete_queue.addDeleter(std::bind(&Device::shutdown, &m_device));

    // Initialize the swap chain
    m_swap_chain.initialize(
        m_context,
        m_device,
        window_p->getWindowData().buffer_width,
        window_p->getWindowData().buffer_height,
        true 
    );
    m_delete_queue.addDeleter(std::bind(
        &SwapChain::shutdown,
        &m_swap_chain,
        m_context,
        m_device
    ));
}

// Shutdown the renderer implementation
void VkRenderer::shutdown() 
{
    m_delete_queue.callDeleter();
}

// Resize the renderer viewport
void VkRenderer::resize(u32 width, u32 height) {
    m_swap_chain.reinitialize(m_context, m_device, width, height);
}

} // namespace cndt::vulkan
