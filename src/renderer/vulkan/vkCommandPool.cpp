#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/vkDevice.h"

#include "renderer/vulkan/vkCommandPool.h"

namespace cndt::vulkan {

// Reset the command pool 
void CommandPool::reset(bool release_resources)
{
    VkCommandPoolResetFlags flag = 0;
    
    if (release_resources)
        flag = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT; 

    VkResult res = vkResetCommandPool(
        m_device_p->logical,
        m_handle,
        flag
    );

    // Reset all the state flags
    for (auto storage_p : m_storages) {
        storage_p->state = CommandBuffer::State::Initial;
    }

    if (res != VK_SUCCESS) {
        throw CommandPoolResetError(
            "Vulkan command pool reset error: {}",
            vk_error_str(res)
        );
    }
}

// Allocate a command buffer from a command pool
CommandBuffer CommandPool::allocateCmdBuffer(bool primary) 
{
    CommandBuffer::Storage *storage_p = new CommandBuffer::Storage();
    storage_p->resettable = m_ressetable_cmd_buffer;

    VkCommandBufferAllocateInfo allocate_info = { };
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = m_handle;
    
    allocate_info.level = primary ? 
        VK_COMMAND_BUFFER_LEVEL_PRIMARY :
        VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    
    allocate_info.commandBufferCount = 1;

    VkResult res = vkAllocateCommandBuffers(
        m_device_p->logical, 
        &allocate_info,
        &storage_p->handle
    );

    if (res != VK_SUCCESS) {
        delete storage_p;

        throw CmdBufferAllocationError(
            "Vulkan buffer allocation error %s",
            vk_error_str(res)
        );
    }

    // Move the command buffer to the Initial state
    storage_p->state = CommandBuffer::State::Initial;

    // Add the storage to the storage pointer set
    m_storages.insert(storage_p);

    return CommandBuffer(storage_p);
}

// Free a command buffer in a command pool
void CommandPool::freeCmdBuffer(CommandBuffer &cmd_buffer) 
{
    CommandBuffer::Storage *storage_p = cmd_buffer.m_storage_p;
    if (storage_p == nullptr)
        return;

    vkFreeCommandBuffers(
        m_device_p->logical, 
        m_handle, 
        1, &storage_p->handle
    );

    // Delete the storage from the set and free it
    m_storages.erase(storage_p);
    delete storage_p;

    cmd_buffer = CommandBuffer();
}

} // namespace cndt::vulkan
