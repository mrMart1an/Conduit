#ifndef CNDT_INTERNAL_RENDERER_GRAPH_RESOURCE_REGISTER_H
#define CNDT_INTERNAL_RENDERER_GRAPH_RESOURCE_REGISTER_H

#include "conduit/defines.h"

#include "conduit/renderer/ResourceRef.h"
#include "conduit/renderer/buffer.h"
#include "conduit/renderer/image.h"

#include "conduit/renderer/graph/passes.h"
#include "conduit/renderer/graph/resources.h"

#include <vector>

namespace cndt::internal {

// Graph resource register
class GraphResourceRegister {
public:
    struct AccessData {
        // Id of the pass that read from the resource last
        Pass::Id last_read_pass_id; 
        // The resource usage used in the last pass that read the resource
        union {
            Pass::BufferUsage buffer;
            Pass::ImageUsage image;
        } last_read_usage;

        // Id of the last pass that wrote on the resource 
        Pass::Id last_write_pass_id; 
        // The resource usage used in the last pass that wrote on the resource
        union {
            Pass::BufferUsage buffer;
            Pass::ImageUsage image;
        } last_write_usage;
    };

public:
    // Create a transient image and return a resource handle, the lifetime of 
    // the image is manage by the graph
    GraphImage createImage(
        std::string_view name,
        GpuImage::Info info
    );
    // Create a transient buffer and return a resource handle, the lifetime of
    // the buffer is manage by the graph
    GraphBuffer createBuffer(
        std::string_view name,
        GpuBuffer::Info info
    );

    // Import a image and return a resource handle, the lifetime of the image
    // is manage by the application
    GraphImage importImage(
        std::string_view name,
        RenderRef<GpuImage> image
    );
    // Import a buffer and return a resource handle, the lifetime of the buffer
    // is manage by the application
    GraphBuffer importBuffer(
        std::string_view name, 
        RenderRef<GpuBuffer> buffer
    );

public:
    // Store the resource access data
    std::vector<AccessData> m_access_data;

    // Store the indices of the resource info and reference
    std::vector<usize> m_info_indices;

    // Transient image infos
    std::vector<GpuImage::Info> m_transient_image_infos;
    // Transient buffer infos
    std::vector<GpuBuffer::Info> m_transient_buffer_infos;
    // Imported image resource references
    std::vector<RenderRef<GpuImage>> m_imported_image_refs;
    // Imported buffer resource references
    std::vector<RenderRef<GpuBuffer>> m_imported_buffer_refs;
    
    // Id to assign to the next graph resource
    GraphResource::Id m_next_id;
};

} // namespace cndt::internal

#endif
