#ifndef CNDT_RENDERER_GRAPH_H
#define CNDT_RENDERER_GRAPH_H

#include "conduit/renderer/graph/resources.h"

#include "conduit/renderer/ResourceRef.h"
#include "conduit/renderer/buffer.h"
#include "conduit/renderer/image.h"

namespace cndt {

// Conduit render graph 
class RenderGraph {

public:
    // Add a render pass to the graph, the pass are submitted to the
    // graph in execution order
    void addPass();
    
    // Resource handle function

    // Create a transient image and return a resource handle, the lifetime of 
    // the image is manage by the graph
    GraphImage createImage(GpuImage::Info info);
    // Create a transient buffer and return a resource handle, the lifetime of
    // the buffer is manage by the graph
    GraphBuffer createBuffer(GpuBuffer::Info info);

    // Import a image and return a resource handle, the lifetime of the image
    // is manage by the application
    GraphImage importImage(RenderRef<GpuImage> image);
    // Import a buffer and return a resource handle, the lifetime of the buffer
    // is manage by the application
    GraphImage importBuffer(RenderRef<GpuBuffer> buffer);
};

} // namespace cndt

#endif
