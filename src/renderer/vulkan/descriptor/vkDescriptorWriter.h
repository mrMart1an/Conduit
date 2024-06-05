#ifndef CNDT_VK_DESCRIPTOR_WRITER_H
#define CNDT_VK_DESCRIPTOR_WRITER_H

#include "conduit/defines.h"

#include <vector>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Vulkan descriptor set writer 
class DescriptorWriter {
    friend class Device;
    
public:
    // Write an image to the given binding
    void writeImage(
        u32 binding,
        
        VkImageView image,
        VkSampler sampler,
        VkImageLayout layout,
        
        VkDescriptorType type
    );
    
    // Write a buffer to the given binding
    void writeBuffer(
        u32 binding,
        
        VkBuffer buffer,
        usize offset,
        usize size,
        
        VkDescriptorType type
    ); 

    // Clear the write infos
    void clear();

    // Write and update the given set
    void updateSet(VkDescriptorSet set);
    
private:
    // Store the descriptor write infos
    std::vector<VkWriteDescriptorSet> m_writes;
    
    // Store descriptor image info to keep the pointers
    // in the writes info vector 
    std::vector<VkDescriptorImageInfo> m_image_infos;
    // Store descriptor buffer info to keep the pointers
    // in the writes info vector 
    std::vector<VkDescriptorBufferInfo> m_buffer_infos;

    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif
