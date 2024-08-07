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

// Return the image sample count
VulkanImage::Info::Sample VulkanImage::sample() const
{
    return m_info.sample;
}

} // namespace cndt::vulkan

