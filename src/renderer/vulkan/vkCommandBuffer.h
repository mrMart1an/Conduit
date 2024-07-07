#ifndef CNDT_VK_COMMAND_BUFFER_H
#define CNDT_VK_COMMAND_BUFFER_H

#include "conduit/defines.h"

#include <functional>
#include <string_view>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class CommandPool;

// Conduit vulkan command buffer abstraction
class CommandBuffer {
    friend class CommandPool;

public:
    // Command buffer state enum
    enum class State {
        Initial,
        Recording,
        Executable,
        Invalid
    };

private:
    struct Storage {
        Storage() : 
            handle(VK_NULL_HANDLE),
            state(State::Invalid),
            single_use(false),
            resettable(false)
        { }

        // Vulkan command buffer handle
        VkCommandBuffer handle;

        // Store the buffer state
        CommandBuffer::State state;

        // Store true if the command buffer is resettable
        bool resettable;
        // Store true if the command buffer was single use
        bool single_use;
    };

public:
    CommandBuffer() : 
        m_storage_p(nullptr)
    { }
    
    // Begin command buffer recording
    void begin(
        bool single_use = true,
        bool renderpass_continue = false,
        bool simultaneous = false
    );
    
    // Record the command buffer with the given function
    void record(std::function<void(VkCommandBuffer)> record_fun);

    // End command buffer recording
    void end();

    // Reset the command buffer
    void reset(bool release_resource = false);

    // Submit a command buffer to a queue
    void submit(
        VkQueue queue,
        VkFence submit_fence,

        u32 wait_semaphore_count = 0,
        VkSemaphore *wait_semaphore_p = VK_NULL_HANDLE,
        
        u32 signal_semaphore_count = 0,
        VkSemaphore *signal_semaphore_p = VK_NULL_HANDLE,

        VkPipelineStageFlags wait_dest_stage = 0
    );

    /*
     *
     *      Getter
     *
     * */

    // Return the vulkan command buffer handle
    VkCommandBuffer handle() const 
    {
        if (m_storage_p != nullptr)
            return m_storage_p->handle; 
        else 
            return VK_NULL_HANDLE;
    }

    // Return the buffer state
    State state() const 
    { 
        if (m_storage_p != nullptr)
            return m_storage_p->state; 
        else 
            return State::Invalid;
    }
 
private:
    CommandBuffer(Storage *storage_p) : 
        m_storage_p(storage_p)
    { }

    // Return a string representing the current buffer state
    std::string_view stateToStr();

private:
    // Pointer to the buffer storage block
    Storage* m_storage_p;
};

} // namespace cndt::vulkan

#endif
