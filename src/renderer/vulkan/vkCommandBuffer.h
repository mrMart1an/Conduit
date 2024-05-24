#ifndef CNDT_VK_COMMAND_BUFFER_H
#define CNDT_VK_COMMAND_BUFFER_H

#include "conduit/defines.h"

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Conduit vulkan command buffer abstraction
class CommandBuffer {
    friend class Device;

    // Private constructor, only the device friend class
    // can create a command buffer
    CommandBuffer() = default;
    
public:
    // Return the vulkan command buffer handle
    VkCommandBuffer handle() const { return m_handle; };
    
    // Begin command buffer recording
    void begin(
        bool single_use,
        bool renderpass_continue,
        bool simultaneous
    );
    
    // End command buffer recording
    void end();

    // Reset the command buffer
    void reset();

    // Submit a command buffer to a queue
    void submit(
        VkQueue queue,
        VkFence submit_fence,

        u32 wait_semaphore_count,
        VkSemaphore *wait_semaphore_p,
        
        u32 signal_semaphore_count,
        VkSemaphore *signal_semaphore_p,

        VkPipelineStageFlags wait_dest_stage
    );

private:
    // Vulkan command buffer handle
    VkCommandBuffer m_handle;
};

} // namespace cndt::vulkan

#endif
