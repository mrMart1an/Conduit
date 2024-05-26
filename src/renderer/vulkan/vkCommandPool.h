#ifndef CNDT_VK_COMMAND_POOL_H
#define CNDT_VK_COMMAND_POOL_H

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Vulkan command pool conduit abstraction
class CommandPool {
    friend class Device;

    // Private constructor, only the device friend class
    // can create a command buffer
    CommandPool() = default;
    
private:
    // Vulkan command pool handle
    VkCommandPool m_handle;
};

} // namespace cndt::vulkan

#endif
