#ifndef CNDT_VK_DESCRIPTOR_LAYOUT_H
#define CNDT_VK_DESCRIPTOR_LAYOUT_H

#include "conduit/defines.h"

#include <vector>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

class DescriptorLayoutBuilder;

class DescriptorLayout {
    friend class DescriptorLayoutBuilder;

public:
    DescriptorLayout() = default;
    
    // Get the vulkan layout
    VkDescriptorSetLayout layout() const { return m_layout; };

    // Get the layout bindings
    const std::vector<VkDescriptorSetLayoutBinding>& bindings() const 
    {
        return m_bindings;    
    } 

private:
    // Store the vulkan layout 
    VkDescriptorSetLayout m_layout;

    // Store the layout bindings
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;
};

// Vulkan descriptor set layout builder class
class DescriptorLayoutBuilder {
    friend class Device;

public:
    // Add a binding to the builder binding list
    void addBinding(
        u32 binding,
        VkDescriptorType type,
        VkShaderStageFlags shader_stage
    );
    

    // Clear the builder binding list
    void clear();

    // Build the descriptor set layout from the builder stored binding list
    DescriptorLayout build(
        VkDescriptorSetLayoutCreateFlags flags = 0
    );

private:
    // Store a list of binding to build the descriptor layout
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;

    // Store a pointer to the device that own the builder
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif
