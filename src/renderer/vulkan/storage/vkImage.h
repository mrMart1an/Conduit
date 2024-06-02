#ifndef CNDT_VK_IMAGE_H
#define CNDT_VK_IMAGE_H

#include <vulkan/vulkan.h>

namespace cndt::vulkan {

class Device;

// Vulkan image with memory stored on a device
class Image {
    friend class Device;
    
public:
    Image() = default;

    // Bind the given Image to the device with the given memory offset
    void bind(VkDeviceSize memory_offset = 0);

    /*
     *
     *      Getter
     *
     * */

    // Return the image handle
    VkImage handle() const { return m_handle; }
    // Return the image view handle
    VkImageView view() const { return m_view; }
    
    // Return the image extent handle
    VkExtent2D extent() const { return m_image_extent; }
    // Return the image format handle
    VkFormat format() const { return m_image_format; }
    
private:
    VkImage m_handle;
    VkImageView m_view;
    
    VkExtent2D m_image_extent;
    VkFormat m_image_format;
    
    VkImageUsageFlagBits m_usage_bits;
    VkMemoryPropertyFlags m_memory_flags;
    
    VkDeviceMemory m_memory;
    
    // A pointer to the device that own the image
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif

