#ifndef CNDT_VK_BUFFER_H
#define CNDT_VK_BUFFER_H

#include "conduit/renderer/buffer.h"

#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace cndt::vulkan {

class Device;

// Vulkan buffer with memory stored on a device
class Buffer {
    friend class Device;

public:
    Buffer() = default;

    // Map the buffer to a region of host memory and return a pointer to it
    // and perform cache maintenance if necessary    
    void* map();

    // Unmap the given buffer 
    // and perform cache maintenance if necessary    
    void unmap();
    
    // Load the data in the src pointer to the buffer with the given
    // destination offset
    void copyMemToBuf(
        void *src_data_p,

        VkDeviceSize dest_offset,
        VkDeviceSize size
    );

    // Copy the buffer data at the src offset in the destination data pointer
    void copyBufToMem(
        void *dst_data_p,

        VkDeviceSize src_offset,
        VkDeviceSize size
    );

    /*
     *
     *      Getter
     *
     * */

    // Return the buffer handle
    VkBuffer handle() const { return m_handle; }

    // Return the size of the buffer
    VkDeviceSize size() const { return m_info.size; };

    // Return true if the buffer is mapped
    bool mapped() const { return m_mapped; };

private:
    VkBuffer m_handle;
    
    VmaAllocation m_allocation;
    VmaAllocationInfo m_allocation_info;

    GpuBufferInfo m_info;

    bool m_mapped;

    // A pointer to the device that own the buffer
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif
