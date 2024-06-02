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

    // Bind the given buffer to the device with the given memory offset
    void bind(VkDeviceSize memory_offset = 0);
    
    // Map the buffer to a region of host memory and return a pointer to it
    void* mapBuffer(
        VkDeviceSize offset,
        VkDeviceSize size,

        VkMemoryMapFlags map_flags
    );

    // Unmap the given buffer 
    void unmapBuffer();
    
    // Load the data at the given pointer to the buffer at the given offset
    void loadBuffer(
        VkMemoryMapFlags map_flags,
    
        VkDeviceSize buffer_offset,
        VkDeviceSize size,
    
        void *data_p
    );

    /*
     *
     *      Getter
     *
     * */

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

    // A pointer to the device that own the buffer
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif
