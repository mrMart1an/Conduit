#ifndef CNDT_VK_BUFFER_H
#define CNDT_VK_BUFFER_H

#include "conduit/renderer/backendEnum.h"
#include "conduit/renderer/buffer.h"

#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace cndt::vulkan {

class Device;

// Vulkan buffer with memory stored on a device
class VulkanBuffer : public GpuBuffer {
    friend class Device;

public:
    VulkanBuffer() = default;
    ~VulkanBuffer() override = default;

    // Resize the buffer, if the preserve data argument is true
    // the content of the buffer will be copied to the new buffer
    // (preserve data default: false)
    // 
    // (If the new size is less that the previews one the
    // content will be cut to fit the new buffer)
    //
    // Warning: the buffer MUST be unmapped before resizing
    void resize(Size new_size, bool preserve_data = false) override;

    // Map the buffer to a region of host memory and return a pointer to it
    // and perform cache maintenance if necessary    
    void* map() override;

    // Unmap the given buffer 
    // and perform cache maintenance if necessary    
    void unmap() override;
    
    // Load the data in the src pointer to the buffer with the given
    // destination offset
    void copyMemToBuf(
        const void *src_data_p,

        Size copy_size,
        Size dest_offset = 0
    ) override;

    // Copy the buffer data at the src offset in the destination data pointer
    void copyBufToMem(
        void *dst_data_p,

        Size copy_size,
        Size src_offset = 0
    ) override;

    // Copy the content of the given buffer to the buffer with
    // the given offsets 
    void copyBufToBuf(
        const GpuBuffer& src_buffer,
        Size copy_size,
        
        Size src_offset = 0,
        Size dest_offset = 0
    ) override;

    /*
     *
     *      Getter
     *
     * */

    // Return the size of the buffer
    Size size() const override { return m_info.size; };

    // Return true if the buffer is mapped
    bool mapped() const override { return m_mapped; };

    // Return the buffer handle
    VkBuffer handle() const { return m_handle; }

    // Return the renderer backend that own this resource
    RendererBackend backend() const override 
    { return RendererBackend::Vulkan; };

private:
    VkBuffer m_handle;
    
    VmaAllocation m_allocation;
    VmaAllocationInfo m_allocation_info;

    Info m_info;

    bool m_mapped;

    // A pointer to the device that own the buffer
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif
