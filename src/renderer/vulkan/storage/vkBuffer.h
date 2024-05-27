#ifndef CNDT_VK_BUFFER_H
#define CNDT_VK_BUFFER_H

#include <vulkan/vulkan.h>

namespace cndt::vulkan {

class Device;

// Vulkan buffer with memory stored on a device
class Buffer {
    friend class Device;

public:
    Buffer() = default;

    // Return the buffer handle
    VkBuffer handle() const { return m_handle; }

    // Return the size of the buffer
    VkDeviceSize size() const { return m_size; };

    // Return true if the buffer is mapped
    bool mapped() const { return m_mapped; };

private:
    VkBuffer m_handle;
    
    VkDeviceSize m_size;
    
    VkBufferUsageFlagBits m_usage_bits;
    VkMemoryPropertyFlags m_memory_flags;
    
    VkDeviceMemory m_memory;
    
    bool m_mapped;
};

} // namespace cndt::vulkan

#endif
