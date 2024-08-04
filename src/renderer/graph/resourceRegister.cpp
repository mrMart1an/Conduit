#include "conduit/renderer/ResourceRef.h"
#include "conduit/renderer/buffer.h"
#include "conduit/renderer/image.h"

#include "conduit/renderer/graph/resources.h"
#include "conduit/internal/renderer/graph/resourceRegister.h"

namespace cndt::internal {

// Create a transient image and return a resource handle, the lifetime of 
// the image is manage by the graph
GraphImage GraphResourceRegister::createImage(
    std::string_view name,
    GpuImage::Info info
) {
    GraphResource::Id id = m_next_id;
    m_next_id += 1;

    // Insert the transient image info in the vector
    // and add the index to the indices vector
    m_info_indices.push_back(m_transient_image_infos.size());
    m_transient_image_infos.push_back(info);

    // Add the resource access info
    AccessData access_data = { };
    access_data.last_read_pass_id = Pass::nullId;
    access_data.last_read_usage.image = Pass::ImageUsage::Undefined;
    access_data.last_write_pass_id = Pass::nullId;
    access_data.last_write_usage.image = Pass::ImageUsage::Undefined;

    m_access_data.push_back(access_data);

    // Create the image handle
    return GraphImage(id, GraphResource::LifetimeType::Transient, name);
}

// Create a transient buffer and return a resource handle, the lifetime of
// the buffer is manage by the graph
GraphBuffer GraphResourceRegister::createBuffer(
    std::string_view name,
    GpuBuffer::Info info
) {
    GraphResource::Id id = m_next_id;
    m_next_id += 1;

    // Insert the transient buffer info in the vector
    // and add the index to the indices vector
    m_info_indices.push_back(m_transient_buffer_infos.size());
    m_transient_buffer_infos.push_back(info);

    // Add the resource access info
    AccessData access_data = { };
    access_data.last_read_pass_id = Pass::nullId;
    access_data.last_read_usage.buffer = Pass::BufferUsage::Undefined;
    access_data.last_write_pass_id = Pass::nullId;
    access_data.last_write_usage.buffer = Pass::BufferUsage::Undefined;

    m_access_data.push_back(access_data);

    // Create the buffer handle
    return GraphBuffer(id, GraphResource::LifetimeType::Transient, name);
}

// Import a image and return a resource handle, the lifetime of the image
// is manage by the application
GraphImage GraphResourceRegister::importImage(
    std::string_view name,
    RenderRef<GpuImage> image
) {
    GraphResource::Id id = m_next_id;
    m_next_id += 1;

    // Insert the imported image ref in the vector
    // and add the index to the indices vector
    m_info_indices.push_back(m_imported_image_refs.size());
    m_imported_image_refs.push_back(image);

    // Add the resource access info
    // TODO track usage of imported resource
    AccessData access_data = { };
    access_data.last_read_pass_id = Pass::nullId;
    access_data.last_read_usage.image = Pass::ImageUsage::Undefined;
    access_data.last_write_pass_id = Pass::nullId;
    access_data.last_write_usage.image = Pass::ImageUsage::Undefined;

    m_access_data.push_back(access_data);

    // Create the image handle
    return GraphImage(id, GraphResource::LifetimeType::External, name);
}

// Import a buffer and return a resource handle, the lifetime of the buffer
// is manage by the application
GraphBuffer GraphResourceRegister::importBuffer(
    std::string_view name,
    RenderRef<GpuBuffer> buffer
) {
    GraphResource::Id id = m_next_id;
    m_next_id += 1;

    // Insert the imported buffer ref in the vector
    // and add the index to the indices vector
    m_info_indices.push_back(m_imported_buffer_refs.size());
    m_imported_buffer_refs.push_back(buffer);

    // Add the resource access info
    AccessData access_data = { };
    access_data.last_read_pass_id = Pass::nullId;
    access_data.last_read_usage.buffer = Pass::BufferUsage::Undefined;
    access_data.last_write_pass_id = Pass::nullId;
    access_data.last_write_usage.buffer = Pass::BufferUsage::Undefined;

    m_access_data.push_back(access_data);

    // Create the buffer handle
    return GraphBuffer(id, GraphResource::LifetimeType::External, name);
}

} // namespace cndt::internal
