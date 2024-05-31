#include "renderer/vulkan/utils/vkAttributeDescriptor.h"

#include "conduit/renderer/vertex.h"

#include <cstddef>
#include <vector>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Return the attribute descriptor for the given vertex type
template<>
VkVertexInputBindingDescription getBindingDescriptor<cndt::Vertex3D>()
{
    VkVertexInputBindingDescription binding_description { };
    
    binding_description.binding = 0;
    binding_description.stride = sizeof(Vertex3D);
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_description;
}

// Return the attribute descriptor for the Vertex type
template<>
std::vector<VkVertexInputAttributeDescription> getAttributeDescriptor
<cndt::Vertex3D>() {
    std::vector<VkVertexInputAttributeDescription> descriptor(3);

    // Position attribute
	descriptor[0].binding = 0;
	descriptor[0].location = 0;
	descriptor[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	descriptor[0].offset = offsetof(Vertex3D, position);
	
    // Color attribute
	descriptor[1].binding = 0;
	descriptor[1].location = 1;
	descriptor[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	descriptor[1].offset = offsetof(Vertex3D, color);
	
    // Color attribute
	descriptor[2].binding = 0;
	descriptor[2].location = 2;
	descriptor[2].format = VK_FORMAT_R32G32_SFLOAT;
	descriptor[2].offset = offsetof(Vertex3D, text_coord);
	
	return descriptor;
}

} // namespace cndt::vulkan
