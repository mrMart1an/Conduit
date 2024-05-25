#include "conduit/defines.h"

#include "renderer/vulkan/utils/vkExceptions.h"
#include "renderer/vulkan/utils/vkUtils.h"

#include "renderer/vulkan/vkCommandBuffer.h"

#include <format>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Begin command buffer recording
void CommandBuffer::begin(
    bool single_use,
    bool renderpass_continue,
    bool simultaneous
) {
    VkCommandBufferBeginInfo begin_info = { };
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    begin_info.flags = 0;
    if (single_use)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (renderpass_continue)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    if (simultaneous)
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    begin_info.pInheritanceInfo = VK_NULL_HANDLE; 

    VkResult res = vkBeginCommandBuffer(m_handle, &begin_info); 

    if (res != VK_SUCCESS) {
        throw CmdBufferBeginError(std::format(
            "command buffer begin error {}",
            vk_error_str(res)
        ));
    }
} 

// End command buffer recording
void CommandBuffer::end()
{
    VkResult res = vkEndCommandBuffer(m_handle); 
    
    if (res != VK_SUCCESS) {
        throw CmdBufferEndError(std::format(
            "command buffer end error {}",
            vk_error_str(res)
        ));
    }   
}

// Reset the command buffer
void CommandBuffer::reset()
{
    VkResult res = vkResetCommandBuffer(m_handle, 0);
    
    if (res != VK_SUCCESS) {
        throw CmdBufferResetError(std::format(
            "command buffer reset error {}",
            vk_error_str(res)
        ));
    }
}

// Submit a command buffer to a queue
void CommandBuffer::submit(
    VkQueue queue,
    VkFence submit_fence,

    u32 wait_semaphore_count,
    VkSemaphore *wait_semaphore_p,
    
    u32 signal_semaphore_count,
    VkSemaphore *signal_semaphore_p,

    VkPipelineStageFlags wait_dest_stage
) {
    VkSubmitInfo submit_info = { };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_handle;
    
    submit_info.pWaitDstStageMask = &wait_dest_stage;
    submit_info.waitSemaphoreCount = wait_semaphore_count;
    submit_info.pWaitSemaphores = wait_semaphore_p;
    submit_info.signalSemaphoreCount = signal_semaphore_count;
    submit_info.pSignalSemaphores = signal_semaphore_p;

    // Submit the command buffer
    VkResult res = vkQueueSubmit(
        queue,
        1, &submit_info,
        submit_fence
    );

    if (res != VK_SUCCESS) {
        throw CmdBufferSubmitError(std::format(
            "command buffer queue submit error %s",
            vk_error_str(res)
        ));
    }
}

} // namespace cndt::vulkan
