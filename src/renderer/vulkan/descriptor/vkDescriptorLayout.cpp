#include "conduit/defines.h"

#include "renderer/vulkan/utils/vkExceptions.h"
#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/descriptor/vkDescriptorLayout.h"
#include "renderer/vulkan/vkDevice.h"

#include <format>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Add a binding to the builder binding list
void DescriptorLayoutBuilder::addBinding(
    u32 binding,
    VkDescriptorType type,
    VkShaderStageFlags shader_stage
) {
    VkDescriptorSetLayoutBinding new_binding { };
    new_binding.binding = binding;
    new_binding.descriptorCount = 1;
    new_binding.descriptorType = type;
    new_binding.stageFlags = shader_stage;

    // TODO: immutable samplers
    new_binding.pImmutableSamplers = VK_NULL_HANDLE;

    m_bindings.push_back(new_binding);
}

// Clear the builder binding list
void DescriptorLayoutBuilder::clear()
{
    m_bindings.clear();
}

// Build the descriptor set layout from the builder stored binding list
VkDescriptorSetLayout DescriptorLayoutBuilder::build(
    VkDescriptorSetLayoutCreateFlags flags
) {
    VkDescriptorSetLayout out_layout;
    
    VkDescriptorSetLayoutCreateInfo info = { };
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = (u32)m_bindings.size();
    info.pBindings = m_bindings.data();
    info.flags = flags;

    VkResult res = vkCreateDescriptorSetLayout(
        m_device_p->logical,
        &info,
        m_device_p->allocator(),
        &out_layout
    );

    if (res != VK_SUCCESS) {
        throw DescriptorSetLayoutBuildError(std::format(
            "Descriptor set layout creation error: {}",
            vk_error_str(res)
        ));
    }

    return out_layout;
}

} // namespace cndt::vulkan
 