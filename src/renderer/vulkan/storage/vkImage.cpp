#include "renderer/vulkan/storage/vkImage.h"

namespace cndt::vulkan {

/*
 *
 *      Getter
 *
 * */

// Return the image extent handle
VulkanImage::Extent VulkanImage::extent() const 
{
    return m_info.extent;
}

// Return the image format
VulkanImage::Info::Format VulkanImage::format() const
{
    return m_info.format;
}

} // namespace cndt::vulkan

