#include "conduit/defines.h"

#include "renderer/vulkan/utils/vkExceptions.h"
#include "renderer/vulkan/utils/vkUtils.h"

#include "renderer/vulkan/vkCommandBuffer.h"

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Begin command buffer recording
void CommandBuffer::begin(
    bool single_use,
    bool renderpass_continue,
    bool simultaneous
) {
    // Make sure the buffer is in the initial state
    if (state() != State::Initial) {
        throw CmdBufferRecordError(
            "Vulkan command buffer begin error: wrong state ({})",
            stateToStr()
        );
    }

    // Update the single use member 
    m_storage_p->single_use = single_use;

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

    VkResult res = vkBeginCommandBuffer(m_storage_p->handle, &begin_info); 

    if (res != VK_SUCCESS) {
        throw CmdBufferBeginError(
            "command buffer begin error {}",
            vk_error_str(res)
        );
    }

    // Move the command buffer to the recording state
    m_storage_p->state = State::Recording;
} 

// Record the command buffer with the given function
void CommandBuffer::record(std::function<void(VkCommandBuffer)> record_fun)
{
    // Make sure the buffer is in the recording state
    if (state() != State::Recording) {
        throw CmdBufferRecordError(
            "Vulkan command buffer record error: wrong state ({})",
            stateToStr()
        );
    }

    // Execute the given recording function
    record_fun(m_storage_p->handle);
}

// End command buffer recording
void CommandBuffer::end()
{
    // Make sure the buffer is in the recording state
    if (state() != State::Recording) {
        throw CmdBufferRecordError(
            "Vulkan command buffer end error: wrong state ({})",
            stateToStr()
        );
    }

    // End the command buffer recording
    VkResult res = vkEndCommandBuffer(m_storage_p->handle); 
    
    if (res != VK_SUCCESS) {
        throw CmdBufferEndError(
            "command buffer end error {}",
            vk_error_str(res)
        );
    }   

    // Move the buffer to the executable state
    m_storage_p->state = State::Executable;
}

// Reset the command buffer
void CommandBuffer::reset(bool release_resource)
{
    if (m_storage_p == nullptr) {
        throw CmdBufferResetError(
            "Vulkan command buffer reset error: buffer uninitialized"
        );
    }   

    // Make sure the buffer is resettable
    if (!m_storage_p->resettable) {
        throw CmdBufferResetError(
            "Vulkan command buffer reset error: buffer not resettale"
        );
    }

    VkCommandBufferResetFlags flags = 0;
    if (release_resource)
        flags |= VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;

    VkResult res = vkResetCommandBuffer(m_storage_p->handle, flags);
    
    if (res != VK_SUCCESS) {
        throw CmdBufferResetError(
            "Vulkan command buffer reset error: {}",
            vk_error_str(res)
        );
    }

    // Move the buffer to the Initial state
    m_storage_p->state = State::Initial;
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
    // Check if the buffer is in the executable state
    if (state() != State::Executable) {
        throw CmdBufferSubmitError(
            "Vulkan command buffer submit error: wrong state ({})",
            stateToStr()
        );
    }

    VkSubmitInfo submit_info = { };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_storage_p->handle;
    
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
        throw CmdBufferSubmitError(
            "command buffer queue submit error %s",
            vk_error_str(res)
        );
    }

    // If the buffer was single use move it to the invalid state,
    // if it wasn't move it to the Executable state
    if (m_storage_p->single_use) {
        m_storage_p->state = State::Invalid;
    } else {
        m_storage_p->state = State::Executable;
    }
}

// Return a string representing the current buffer state
std::string_view CommandBuffer::stateToStr()
{
    switch (state()) { 
        case State::Initial: {
            return "Initial";
        }
        case State::Recording: {
            return "Recording";
        }
        case State::Executable: {
            return "Executable";
        }
        case State::Invalid: {
            return "Invalid";
        }
        default: {
            return "Unknow state";
        }
    }
}

} // namespace cndt::vulkan
