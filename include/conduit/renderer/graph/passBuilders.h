#ifndef CNDT_RENDERER_GRAPH_PASS_BUILDER_H
#define CNDT_RENDERER_GRAPH_PASS_BUILDER_H

#include "conduit/renderer/graph/passes.h"
#include "conduit/renderer/graph/resources.h"

namespace cndt {

class RenderGraph;

// Graph pass builder
class PassBuilder {
    friend class RenderGraph;

protected:
    // Store data for the resource access of one resource
    struct ResourceAccess {
        GraphResource resource_handle;

        // Store the resource usage date
        union {
            Pass::BufferUsage buffer;
            Pass::ImageUsage image;
        } usage;
    };

public:
    PassBuilder() : m_pass_id(Pass::nullId) { }
 
    // Use the buffer as an uniform buffer
    GraphBuffer useUnifromBuffer(GraphBuffer buffer);

    // Use the image as a sampled image source 
    GraphImage useSampledImage(GraphImage image);

    // Read from the storage buffer during this pass
    GraphBuffer readStorageBuffer(GraphBuffer buffer);
    // Write to the storage buffer during this pass
    GraphBuffer writeStorageBuffer(GraphBuffer buffer);
    // Read and write the storage buffer during this pass
    GraphBuffer readWriteStorageBuffer(GraphBuffer buffer);
 
    // Read from the storage image during this pass
    GraphImage readStorageImage(GraphImage image);
    // Write to the storage image during this pass
    GraphImage writeStorageImage(GraphImage image);
    // Read and write the storage image during this pass
    GraphImage readWriteStorageImage(GraphImage image);

    // Use the buffer as transfer destination
    GraphBuffer transferSrcBuffer(GraphBuffer buffer);
    // Use the buffer as transfer source
    GraphBuffer transferDstBuffer(GraphBuffer buffer);

    // Use the image as transfer destination
    GraphImage transferSrcImage(GraphImage image);
    // Use the image as transfer source
    GraphImage transferDstImage(GraphImage image);

protected:
    PassBuilder(Pass::Id id) : m_pass_id(id) { }

    // Add a buffer resource read to the pass
    void readBuffer(GraphBuffer buffer, Pass::BufferUsage usage);
    // Add a buffer resource write to the pass
    void writeBuffer(GraphBuffer buffer, Pass::BufferUsage usage);

    // Add a image resource read to the pass
    void readImage(GraphImage image, Pass::ImageUsage usage);
    // Add a image resource write to the pass
    void writeImage(GraphImage image, Pass::ImageUsage usage);

private:
    // Id of the pass to build
    Pass::Id m_pass_id;

    // Store the resource reads performed by the pass
    std::vector<ResourceAccess> m_reads;
    // Store the resource writes performed by the pass   
    std::vector<ResourceAccess> m_writes;
};

// Graphics pass builder
class GraphicsPassBuilder : public PassBuilder {
public:
    // Use the image as color attachment in this pass
    GraphImage useColorOutput(GraphImage image);

    // Use the buffer as an index buffer
    GraphBuffer useIndexBuffer(GraphBuffer buffer);
    // Use the buffer as a vertex buffer
    GraphBuffer useVertexBuffer(GraphBuffer buffer);

    // TODO: implement other graphics pass builder functions
};

} // namespace cndt

#endif
