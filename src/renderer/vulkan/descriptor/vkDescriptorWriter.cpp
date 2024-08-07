#include "conduit/defines.h"

#include "renderer/vulkan/vkDevice.h"
#include "renderer/vulkan/descriptor/vkDescriptorWriter.h"

#include <vector>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Write an image to the given binding
void DescriptorWriter::writeImage(
    u32 binding,
    
    VkImageView image,
    VkSampler sampler,
    VkImageLayout layout,

    VkDescriptorType type
) {
    // Create the buffer info
    VkDescriptorImageInfo stored_image_info = { };
    stored_image_info.imageView = image;
    stored_image_info.imageLayout = layout;
    stored_image_info.sampler = sampler;
    
    m_image_infos.push_back(stored_image_info);
    VkDescriptorImageInfo *image_info_p = &m_image_infos.back();

    // Create the write info
    VkWriteDescriptorSet write_info = {};
    write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    // Set up later in the update function
    write_info.dstSet = VK_NULL_HANDLE; 

    write_info.dstBinding = binding;
    write_info.descriptorType = type;
    write_info.descriptorCount = 1;
    write_info.pImageInfo = image_info_p;

    m_writes.push_back(write_info);
}

// Write a buffer to the given binding
void DescriptorWriter::writeBuffer(
    u32 binding,

    VkBuffer buffer,
    usize offset,
    usize size,

    VkDescriptorType type
) {
    // Create the buffer info
    VkDescriptorBufferInfo stored_buffer_info = { };
    stored_buffer_info.buffer = buffer;
    stored_buffer_info.range = size;
    stored_buffer_info.offset = offset;
    
    m_buffer_infos.push_back(stored_buffer_info);
    VkDescriptorBufferInfo *buffer_info_p = &m_buffer_infos.back();

    // Create the write info
    VkWriteDescriptorSet write_info = {};
    write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    // Set up later in the update function
    write_info.dstSet = VK_NULL_HANDLE; 

    write_info.dstBinding = binding;
    write_info.descriptorType = type;
    write_info.descriptorCount = 1;
    write_info.pBufferInfo = buffer_info_p;

    m_writes.push_back(write_info);
}

// Clear the write infos
void DescriptorWriter::clear()
{
    m_writes.clear();
    
    m_buffer_infos.clear();
    m_image_infos.clear();
}

// Write and update the given set
void DescriptorWriter::updateSet(VkDescriptorSet set)
{
    for (auto& write : m_writes) {
        write.dstSet = set;
    }

    vkUpdateDescriptorSets(
        m_device_p->logical(),
        m_writes.size(), m_writes.data(), 
        0, VK_NULL_HANDLE
    );
}

};
