#include "renderer/vulkan/vkDevice.h"

#include "vulkan/vulkan_core.h"

namespace cndt::vulkan {

// Convert backend agnostic buffer usage to vulkan buffer usage
VkBufferUsageFlags Device::getVkBufferUsage(GpuBuffer::Info::UsageEnum usage)
{
    VkBufferUsageFlags buffer_usage = 0;

    if (usage & GpuBuffer::Info::Usage::TransferDst)
        buffer_usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (usage & GpuBuffer::Info::Usage::TransferSrc)
        buffer_usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    if (usage & GpuBuffer::Info::Usage::StorageBuffer)
        buffer_usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (usage & GpuBuffer::Info::Usage::UniformBuffer)
        buffer_usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    if (usage & GpuBuffer::Info::Usage::VertexBuffer)
        buffer_usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (usage & GpuBuffer::Info::Usage::IndexBuffer)
        buffer_usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    return buffer_usage;
}

// Convert backend agnostic image usage to vulkan image usage
VkImageUsageFlags Device::getVkImageUsage(GpuImage::Info::UsageEnum usage)
{
    VkImageUsageFlags image_usage = 0;

    if (usage & GpuImage::Info::Usage::TransferDst)
        image_usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if (usage & GpuImage::Info::Usage::TransferSrc)
        image_usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    if (usage & GpuImage::Info::Usage::Storage)
        image_usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    if (usage & GpuImage::Info::Usage::Sampled)
        image_usage |= VK_IMAGE_USAGE_SAMPLED_BIT;

    if (usage & GpuImage::Info::Usage::ColorAttachment)
        image_usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (usage & GpuImage::Info::Usage::DepthStencilAttachment)
        image_usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if (usage & GpuImage::Info::Usage::InputAttachment)
        image_usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

    return image_usage;

}

// Convert backend agnostic image format to vulkan image format
// TODO query for format support and fallback
VkFormat Device::getVkFormat(GpuImage::Info::Format format)
{
    VkFormat vk_format = VK_FORMAT_R8G8B8A8_UNORM;

    switch (format) {
        case GpuImage::Info::Format::R8G8B8A8_UINT:
            vk_format = VK_FORMAT_R8G8B8A8_UINT; break;
        case GpuImage::Info::Format::R8G8B8A8_SINT:
            vk_format = VK_FORMAT_R8G8B8A8_SINT; break;
        case GpuImage::Info::Format::R8G8B8A8_UNORM:
            vk_format = VK_FORMAT_R8G8B8A8_UNORM; break;
        case GpuImage::Info::Format::R8G8B8A8_SNORM:
            vk_format = VK_FORMAT_R8G8B8A8_SNORM; break;
        case GpuImage::Info::Format::R8G8B8A8_SRGB:
            vk_format = VK_FORMAT_R8G8B8A8_SRGB; break;
    
        case GpuImage::Info::Format::B8G8R8A8_UNORM:
            vk_format = VK_FORMAT_B8G8R8A8_UNORM; break;
        case GpuImage::Info::Format::B8G8R8A8_SRGB:
            vk_format = VK_FORMAT_B8G8R8A8_SRGB; break;
    
        case GpuImage::Info::Format::R8G8_UNORM:
            vk_format = VK_FORMAT_R8G8_UNORM; break;
        case GpuImage::Info::Format::R8G8_SNORM:
            vk_format = VK_FORMAT_R8G8_SNORM; break;
        case GpuImage::Info::Format::R8G8_UINT:
            vk_format = VK_FORMAT_R8G8_UINT; break;
        case GpuImage::Info::Format::R8G8_SINT:
            vk_format = VK_FORMAT_R8G8_SINT; break;
    
        case GpuImage::Info::Format::R8_UNORM:
            vk_format = VK_FORMAT_R8_UNORM; break;
        case GpuImage::Info::Format::R8_SNORM:
            vk_format = VK_FORMAT_R8_SNORM; break;
        case GpuImage::Info::Format::R8_UINT:
            vk_format = VK_FORMAT_R8_UINT; break;
        case GpuImage::Info::Format::R8_SINT:
            vk_format = VK_FORMAT_R8_SINT; break;
    
        case GpuImage::Info::Format::R16G16B16A16_SINT:
            vk_format = VK_FORMAT_R16G16B16A16_SINT; break;
        case GpuImage::Info::Format::R16G16B16A16_UINT:
            vk_format = VK_FORMAT_R16G16B16A16_UINT; break;
        case GpuImage::Info::Format::R16G16B16A16_SFLOAT:
            vk_format = VK_FORMAT_R16G16B16A16_SFLOAT; break;
    
        case GpuImage::Info::Format::R16G16_SFLOAT:
            vk_format = VK_FORMAT_R16G16_SFLOAT; break;
        case GpuImage::Info::Format::R16G16_UINT:
            vk_format = VK_FORMAT_R16G16_UINT; break;
        case GpuImage::Info::Format::R16G16_SINT:
            vk_format = VK_FORMAT_R16G16_SINT; break;
    
        case GpuImage::Info::Format::R16_SFLOAT:
            vk_format = VK_FORMAT_R16_SFLOAT; break;
        case GpuImage::Info::Format::R16_UINT:
            vk_format = VK_FORMAT_R16_UINT; break;
        case GpuImage::Info::Format::R16_SINT:
            vk_format = VK_FORMAT_R16_SINT; break;
    
        case GpuImage::Info::Format::R32G32_SFLOAT:
            vk_format = VK_FORMAT_R32G32_SFLOAT; break;
        case GpuImage::Info::Format::R32G32_UINT:
            vk_format = VK_FORMAT_R32G32_UINT; break;
        case GpuImage::Info::Format::R32G32_SINT:
            vk_format = VK_FORMAT_R32G32_SINT; break;
    
        case GpuImage::Info::Format::R32_SFLOAT:
            vk_format = VK_FORMAT_R32_SFLOAT; break;
        case GpuImage::Info::Format::R32_UINT:
            vk_format = VK_FORMAT_R32_UINT; break;
        case GpuImage::Info::Format::R32_SINT:
            vk_format = VK_FORMAT_R32_SINT; break;
    
        case GpuImage::Info::Format::D32_SFLOAT:
            vk_format = VK_FORMAT_D32_SFLOAT; break;
        case GpuImage::Info::Format::D16_UNORM:
            vk_format = VK_FORMAT_D16_UNORM; break;
    }

    return vk_format;
}

// Convert the backend agnostic sample count to vulkan sample count
// TODO check for sample count support
VkSampleCountFlagBits Device::getVkSampleCount(GpuImage::Info::Sample sample)
{
    VkSampleCountFlagBits  out_sample;

    switch (sample) {
        case GpuImage::Info::Sample::Count_1:
            out_sample = VK_SAMPLE_COUNT_1_BIT;
            break;
        case GpuImage::Info::Sample::Count_2:
            out_sample = VK_SAMPLE_COUNT_2_BIT;
            break;
        case GpuImage::Info::Sample::Count_4:
            out_sample = VK_SAMPLE_COUNT_4_BIT;
            break;
        case GpuImage::Info::Sample::Count_8:
            out_sample = VK_SAMPLE_COUNT_8_BIT;
            break;
        case GpuImage::Info::Sample::Count_16:
            out_sample = VK_SAMPLE_COUNT_16_BIT;
            break;
        case GpuImage::Info::Sample::Count_32:
            out_sample = VK_SAMPLE_COUNT_32_BIT;
            break;
        case GpuImage::Info::Sample::Count_64:
            out_sample = VK_SAMPLE_COUNT_64_BIT;
            break;

        default:
            out_sample = VK_SAMPLE_COUNT_1_BIT;
    }

    return out_sample;
}

} // namespace cndt

