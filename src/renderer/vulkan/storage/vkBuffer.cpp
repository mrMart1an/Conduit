#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/vkDevice.h"

#include "renderer/vulkan/storage/vkBuffer.h"

#include <cstring>

namespace cndt::vulkan {

// Bind the given buffer to the device with the given memory offset
void Buffer::bind(VkDeviceSize memory_offset)
{
    VkResult res = vkBindBufferMemory(
        m_device_p->logical,
        m_handle,
        m_memory,
        memory_offset
    );

    if (res != VK_SUCCESS) {
        throw BufferBindError(
            "Vulkan buffer bind error: {}", 
            vk_error_str(res)
        );
    }
}

// Map the buffer to a region of host memory and return a pointer to it
void* Buffer::mapBuffer(
    VkDeviceSize offset,
    VkDeviceSize size,

    VkMemoryMapFlags map_flags
) {
    void *data;
    
    VkResult res = vkMapMemory(
        m_device_p->logical,
        m_memory,
        offset, size,
        map_flags, 
        &data
    );

    if (res != VK_SUCCESS) {
        throw BufferMapError(
            "Vulkan buffer map error: {}",
            vk_error_str(res)
        );
    }

    m_mapped = true;

    return data;
}

// Unmap the given buffer 
void Buffer::unmapBuffer()
{
    vkUnmapMemory(m_device_p->logical, m_memory);
    
    m_mapped = false;
}

// Load the data at the given pointer to the buffer at the given offset
void Buffer::loadBuffer(
    VkMemoryMapFlags map_flags,

    VkDeviceSize buffer_offset,
    VkDeviceSize size,

    void *data_p
) {
    void *buffer_data = mapBuffer(buffer_offset, size, map_flags);

    std::memcpy(buffer_data, data_p, size);

    unmapBuffer();
}

} // namespace cndt::vulkan
 
