#ifndef CNDT_VK_IMAGE_H
#define CNDT_VK_IMAGE_H

#include <vulkan/vulkan.h>

namespace cndt::vulkan {

class Device;

// Vulkan image with memory stored on a device
class Image {
    friend class Device;
    
    // Private Constructor, only the device can create an Image instance
    Image() = default;

public:
    VkImage handle;
    VkImageView view;
    
    VkExtent2D image_extent;
    VkFormat image_format;
    
    VkImageUsageFlagBits usage_bits;
    VkMemoryPropertyFlags memory_flags;
    
private:
    VkDeviceMemory m_memory;
};

} // namespace cndt::vulkan


#endif
