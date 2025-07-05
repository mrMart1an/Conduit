#include "conduit/logging.h"
#include "renderer/vulkan/utils/vkExceptions.h"
#include "renderer/vulkan/utils/vkUtils.h"

#include "renderer/vulkan/vkDevice.h"

#include "renderer/vulkan/storage/vkBuffer.h"

namespace cndt::vulkan {


// Resize the buffer, if the preserve data argument is true
// the content of the buffer will be copied to the new buffer
// (preserve data default: false)
// 
// (If the new size is less that the previews one the
// content will be cut to fit the new buffer)
//
// Warning: the buffer MUST be unmapped before resizing
void VulkanBuffer::resize(Size new_size, bool preserve_data)
{
    Size old_size = size();

    // Create the new buffer
    m_info.size = new_size;
    VulkanBuffer new_buffer = m_device_p->createBuffer(m_info);

    // Copy the data to the new buffer is preserve data is true
    if (preserve_data) {
        Size copy_size = old_size > new_size ? new_size : old_size;

        new_buffer.copyBufToBuf(*this, copy_size);
    }

    // Delete the old buffer 
    m_device_p->destroyBuffer(*this);

    // Store the new buffer
    *this = new_buffer;
}

// Map the buffer to a region of host memory and return a pointer to it
void* VulkanBuffer::map()
{
    // Check if the buffer is mapped, during allocation all
    // host visible allocation are mapped
    if (m_allocation_info.pMappedData == nullptr) {
        throw BufferMapError(
            "Vulkan buffer map error: buffer isn't host visible"
        );
    }
        
    VkResult res = vmaInvalidateAllocation(
        m_device_p->vmaAllocator(),
        m_allocation,
        0, static_cast<VkDeviceSize>(size())
    );
   
    if (res != VK_SUCCESS) {
        throw BufferMapError(
            "Vulkan buffer map cache invalidation error: {}",
            vk_error_str(res)
        );
    }

    m_mapped = true;

    return m_allocation_info.pMappedData;
}

// Unmap the given buffer 
void VulkanBuffer::unmap()
{
    if (!m_mapped) {
        log::core::warn("Vulkan buffer unmap: Unmapping not mapped buffer");
        return;
    }

    // Check if the buffer is mapped, during allocation all
    // host visible allocation are mapped
    if (m_allocation_info.pMappedData == nullptr) {
        throw BufferMapError(
            "Vulkan buffer unmap error: buffer isn't host visible"
        );
    }

    VkResult res = vmaFlushAllocation(
        m_device_p->vmaAllocator(),
        m_allocation,
        0, static_cast<VkDeviceSize>(size())
    );

    if (res != VK_SUCCESS) {
        throw BufferMapError(
            "Vulkan buffer unmap cache flush error: ",
            vk_error_str(res)
        );
    }
    
    m_mapped = false;
}

// Load the data at the given pointer to the buffer at the given offset
void VulkanBuffer::copyMemToBuf(
    const void *src_data_p,

    Size copy_size,
    Size dst_offset
) {
    VkResult res = vmaCopyMemoryToAllocation(
        m_device_p->vmaAllocator(),
        src_data_p,
        m_allocation,
        static_cast<VkDeviceSize>(dst_offset),
        static_cast<VkDeviceSize>(copy_size)
    );

    if (res != VK_SUCCESS) {
        throw BufferCopyError(
            "Buffer copy (Mem -> Buf) error: ",
            vk_error_str(res)
        );
    }
}

// Copy the buffer data at the src offset in the destination data pointer
void VulkanBuffer::copyBufToMem(
    void *dst_data_p,

    Size copy_size,
    Size src_offset
) {
    VkResult res = vmaCopyAllocationToMemory(
        m_device_p->vmaAllocator(),
        m_allocation,
        static_cast<VkDeviceSize>(src_offset),
        dst_data_p,
        static_cast<VkDeviceSize>(copy_size)
    );

    if (res != VK_SUCCESS) {
        throw BufferCopyError(
            "Buffer copy (Buf -> Mem) error: ",
            vk_error_str(res)
        );
    }
}

// Copy the content of the given buffer to the buffer with
// the given offsets 
void VulkanBuffer::copyBufToBuf(
    const GpuBuffer& src_buffer,
    Size copy_size,
    
    Size src_offset,
    Size dest_offset
) {
    const VulkanBuffer& src = static_cast<const VulkanBuffer&>(src_buffer);

    m_device_p->copyBuffer(
        src_offset, src,
        dest_offset, *this,
        copy_size
    );
}

} // namespace cndt::vulkan
 
