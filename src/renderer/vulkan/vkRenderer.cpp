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
    m_delete_queue.addDeleter(std::bind(&Context::shutdown, m_context));

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
    m_delete_queue.addDeleter(std::bind(&Device::shutdown, m_device));
}

// Shutdown the renderer implementation
void VkRenderer::shutdown() 
{
    m_delete_queue.callDeleter();
}

// Resize the renderer viewport
void VkRenderer::resize(u32, u32) { }

} // namespace cndt::vulkan
