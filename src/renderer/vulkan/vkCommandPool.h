#ifndef CNDT_VK_COMMAND_POOL_H
#define CNDT_VK_COMMAND_POOL_H

#include "renderer/vulkan/vkCommandBuffer.h"
#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Vulkan command pool conduit abstraction
class CommandPool {
    friend class Device;

public:
    CommandPool() = default;

    // Reset the command pool 
    void reset(bool release_resources);
    
    // Allocate a command buffer from the pool
    CommandBuffer allocateCmdBuffer(bool primary = true);

    // Free a command buffer allocated from the pool
    void freeCmdBuffer(CommandBuffer &cmd_buffer);

private:
    // Vulkan command pool handle
    VkCommandPool m_handle;

    // A pointer to the device that own the command pool
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif
