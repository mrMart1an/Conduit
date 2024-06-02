#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/vkDevice.h"

#include "renderer/vulkan/vkCommandPool.h"

namespace cndt::vulkan {

// Reset the command pool 
void CommandPool::reset(bool release_resources)
{
    VkCommandPoolResetFlags flag;
    
    if (release_resources)
        flag = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT; 
    else
        flag = 0;

    VkResult res = vkResetCommandPool(
        m_device_p->logical,
        m_handle,
        flag
    );

    if (res != VK_SUCCESS) {
        throw CommandPoolResetError(std::format(
            "Command pool reset error: {}",
            vk_error_str(res)
        ));
    }
}

// Allocate a command buffer from a command pool
CommandBuffer CommandPool::allocateCmdBuffer(bool primary) 
{
    CommandBuffer out_buffer;

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
        &out_buffer.m_handle
    );

    if (res != VK_SUCCESS) {
        throw CmdBufferAllocationError(std::format(
            "Vulkan buffer allocation error %s",
            vk_error_str(res)
        ));
    }

    return out_buffer;
}

// Free a command buffer in a command pool
void CommandPool::freeCmdBuffer(CommandBuffer &cmd_buffer) 
{
    vkFreeCommandBuffers(
        m_device_p->logical, 
        m_handle, 
        1, &cmd_buffer.m_handle
    );

    cmd_buffer = CommandBuffer();
}

} // namespace cndt::vulkan
