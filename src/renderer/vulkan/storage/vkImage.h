#ifndef CNDT_VK_IMAGE_H
#define CNDT_VK_IMAGE_H

#include "conduit/renderer/backendEnum.h"
#include "conduit/renderer/image.h"

#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace cndt::vulkan {

class Device;

// Vulkan image with memory stored on a device
class VulkanImage : public GpuImage {
    friend class Device;
    
public:
    VulkanImage() = default;

    /*
     *
     *      Getter
     *
     * */

    // Return the image extent handle
    Extent extent() const override;

    // Return the image format
    Info::Format format() const override;

    // Return the image handle
    VkImage handle() const { return m_handle; }
    // Return the image view handle
    VkImageView view() const { return m_view; }
    
    // Return the image format handle
    VkFormat vkFormat() const { return m_vk_format; }
    
    // Return the renderer backend that own this resource
    RendererBackend backend() const override 
    { return RendererBackend::Vulkan; };

private:
    VkImage m_handle;
    VkImageView m_view;
    
    VmaAllocation m_allocation;
    VmaAllocationInfo m_alloc_info;

    // Image vulkan format
    VkFormat m_vk_format;

    // Store the image creation information
    Info m_info;

    // The number of mipmap of the image
    u32 m_mipmap_levels;
    
    // A pointer to the device that own the image
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif

