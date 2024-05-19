#ifndef CNDT_VK_BUFFER_H
#define CNDT_VK_BUFFER_H

#include <vulkan/vulkan.h>

namespace cndt::vulkan {

class Device;

// Vulkan buffer with memory stored on a device
class Buffer {
    friend class Device;

    // Private Constructor, only the device can create a Buffer instance
    Buffer() = default;

public:
    // Return true if the buffer is mapped
    bool mapped() const { return m_mapped; };

    // Return the size of the buffer
    VkDeviceSize size() const { return m_size; };

public:
    VkBuffer handle;
    
    VkBufferUsageFlagBits usage_bits;
    VkMemoryPropertyFlags memory_flags;

private:
    VkDeviceMemory m_memory;
    
    VkDeviceSize m_size;
    
    bool m_mapped;
};

} // namespace cndt::vulkan

#endif
