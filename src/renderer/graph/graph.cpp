#include "conduit/renderer/graph/graph.h"
#include "conduit/renderer/graph/resources.h"

#include "conduit/renderer/buffer.h"
#include "conduit/renderer/image.h"

namespace cndt {

// Create a transient image and return a resource handle, the lifetime of 
// the image is manage by the graph
GraphImage RenderGraph::createImage(
    std::string_view name,
    GpuImage::Info info
) {
    return m_resource_register.createImage(name, info);
}

// Create a transient buffer and return a resource handle, the lifetime of
// the buffer is manage by the graph
GraphBuffer RenderGraph::createBuffer(
    std::string_view name,
    GpuBuffer::Info info
) {
    return m_resource_register.createBuffer(name, info);
}

// Import a image and return a resource handle, the lifetime of the image
// is manage by the application
GraphImage RenderGraph::importImage(
    std::string_view name,
    RenderRef<GpuImage> image
) {
    return m_resource_register.importImage(name, image);
}

// Import a buffer and return a resource handle, the lifetime of the buffer
// is manage by the application
GraphBuffer RenderGraph::importBuffer(
    std::string_view name,
    RenderRef<GpuBuffer> buffer
) {
    return m_resource_register.importBuffer(name, buffer);
}

} // namespace cndt

