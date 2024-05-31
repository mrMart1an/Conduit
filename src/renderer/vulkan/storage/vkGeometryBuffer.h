#ifndef CNDT_VK_GEOMETRY_BUFFER_H
#define CNDT_VK_GEOMETRY_BUFFER_H

#include "conduit/defines.h"
#include "renderer/vulkan/storage/vkBuffer.h"

namespace cndt::vulkan {

class Device;

// Vulkan geometry buffer class
template<typename VertexType>
class GeometryBuffer {
    friend class Device;

public:
    GeometryBuffer() = default;
    
    // Vertex buffer getter functions
    const Buffer& vertex() const { return m_vertex_buffer; };
    
    // Index buffer getter functions
    const Buffer& index() const { return m_index_buffer; };

private:
    Buffer m_vertex_buffer;
    Buffer m_index_buffer;

    u64 m_vertex_load_offset;
    u64 m_index_load_offset;
};

} // namespace cndt::vulkan

#endif
