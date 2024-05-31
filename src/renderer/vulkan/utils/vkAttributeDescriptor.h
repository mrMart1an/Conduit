#ifndef CNDT_VK_ATTRIBUTE_DESCRIPTOR_H
#define CNDT_VK_ATTRIBUTE_DESCRIPTOR_H

#include <vector>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Return the attribute descriptor for the given vertex type
template<typename VertexType>
VkVertexInputBindingDescription getBindingDescriptor();

// Return the attribute descriptor for the given vertex type
template<typename VertexType>
std::vector<VkVertexInputAttributeDescription> getAttributeDescriptor();

} // namespace cndt::vulkan

#endif
