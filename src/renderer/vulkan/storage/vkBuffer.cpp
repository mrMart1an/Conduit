#include "renderer/vulkan/utils/vkExceptions.h"
#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/vkDevice.h"

#include "renderer/vulkan/storage/vkBuffer.h"

namespace cndt::vulkan {

// Map the buffer to a region of host memory and return a pointer to it
void* Buffer::map()
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
        0, size()
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
void Buffer::unmap()
{
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
        0, size()
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
void Buffer::copyMemToBuf(
    void *src_data_p,

    VkDeviceSize dst_offset,
    VkDeviceSize size
) {
    VkResult res = vmaCopyMemoryToAllocation(
        m_device_p->vmaAllocator(),
        src_data_p,
        m_allocation,
        dst_offset,
        size
    );

    if (res != VK_SUCCESS) {
        throw BufferCopyError(
            "Buffer copy (Mem -> Buf) error: ",
            vk_error_str(res)
        );
    }
}

// Copy the buffer data at the src offset in the destination data pointer
void Buffer::copyBufToMem(
    void *dst_data_p,

    VkDeviceSize src_offset,
    VkDeviceSize size
) {
    VkResult res = vmaCopyAllocationToMemory(
        m_device_p->vmaAllocator(),
        m_allocation,
        src_offset,
        dst_data_p,
        size
    );

    if (res != VK_SUCCESS) {
        throw BufferCopyError(
            "Buffer copy (Buf -> Mem) error: ",
            vk_error_str(res)
        );
    }
}

} // namespace cndt::vulkan
 
