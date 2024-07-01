#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/vkDevice.h"

#include "renderer/vulkan/storage/vkImage.h"

#include <fmt/format.h>

namespace cndt::vulkan {

// Bind the  given Image to the device
void Image::bind(VkDeviceSize memory_offset) 
{
    VkResult res = vkBindImageMemory(
        m_device_p->logical,
        m_handle,
        m_memory,
        memory_offset 
    );

    if (res != VK_SUCCESS) {
        throw ImageBindError(fmt::format(
            "Vulkan image bind error: {}",
            vk_error_str(res)
        ));
    }
}

} // namespace cndt::vulkan

