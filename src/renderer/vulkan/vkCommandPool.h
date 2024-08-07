#ifndef CNDT_VK_COMMAND_POOL_H
#define CNDT_VK_COMMAND_POOL_H

#include "renderer/vulkan/vkCommandBuffer.h"

#include <set>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Vulkan command pool conduit abstraction
class CommandPool {
    friend class Device;

public:
    CommandPool() : 
        m_handle(VK_NULL_HANDLE),
        m_ressetable_cmd_buffer(false),
        m_storages(),
        m_device_p(nullptr)
    { };

    // Reset the command pool 
    void reset(bool release_resources = false);
    
    // Allocate a command buffer from the pool
    CommandBuffer allocateCmdBuffer(bool primary = true);

    // Free a command buffer allocated from the pool
    void freeCmdBuffer(CommandBuffer &cmd_buffer);

private:
    // Vulkan command pool handle
    VkCommandPool m_handle;

    // Store true is the buffer allocated from this pool are resettable
    bool m_ressetable_cmd_buffer;

    // Store a list of pointer to the storage struct allocated from this pool
    std::set<CommandBuffer::Storage*> m_storages;

    // A pointer to the device that own the command pool
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif
