#ifndef CNDT_RENDERER_GRAPH_H
#define CNDT_RENDERER_GRAPH_H

#include "conduit/internal/renderer/graph/resourceRegister.h"
#include "conduit/renderer/graph/resources.h"
#include "conduit/renderer/graph/passBuilders.h"

#include "conduit/renderer/ResourceRef.h"
#include "conduit/renderer/buffer.h"
#include "conduit/renderer/image.h"

#include <functional>

namespace cndt {

// Conduit render graph 
class RenderGraph {
public:
    // Add a graphics pass to the graph, the pass are submitted to the
    // graph in execution order
    template <typename DataType>
    void addPass(
        std::string_view name,

        Pass::CapabilityEnum capability,

        std::function<void(PassBuilder&, DataType&)> builder_fun,
        std::function<void(void*, DataType&)> execute_fun
    );
    
    // Resource handle function

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

private:
    // Graph resource register
    internal::GraphResourceRegister m_resource_register;
};

} // namespace cndt

#endif
