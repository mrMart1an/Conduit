#include "conduit/renderer/graph/resources.h"
#include "conduit/renderer/graph/passBuilders.h"

namespace cndt {

// Add a buffer resource read to the pass
void PassBuilder::readBuffer(GraphBuffer buffer, Pass::BufferUsage usage)
{
    ResourceAccess access = { };
    access.resource_handle = buffer;
    access.usage.buffer = usage;

    m_reads.push_back(access);
}

// Add a buffer resource write to the pass
void PassBuilder::writeBuffer(GraphBuffer buffer, Pass::BufferUsage usage)
{
    ResourceAccess access = { };
    access.resource_handle = buffer;
    access.usage.buffer = usage;

    m_writes.push_back(access);
}

// Add a image resource read to the pass
void PassBuilder::readImage(GraphImage image, Pass::ImageUsage usage)
{
    ResourceAccess access = { };
    access.resource_handle = image;
    access.usage.image = usage;

    m_reads.push_back(access);
}

// Add a image resource write to the pass
void PassBuilder::writeImage(GraphImage image, Pass::ImageUsage usage)
{
    ResourceAccess access = { };
    access.resource_handle = image;
    access.usage.image = usage;

    m_writes.push_back(access);
}
 
// Use the buffer as an uniform buffer
GraphBuffer PassBuilder::useUnifromBuffer(GraphBuffer buffer)
{
    readBuffer(buffer, Pass::BufferUsage::Uniform);
    return buffer;
}

// Use the image as a sampled image source 
GraphImage PassBuilder::useSampledImage(GraphImage image)
{
    readImage(image, Pass::ImageUsage::Sampled);
    return image;
}

// Read from the storage buffer during this pass
GraphBuffer PassBuilder::readStorageBuffer(GraphBuffer buffer)
{
    readBuffer(buffer, Pass::BufferUsage::Storage);
    return buffer;
}

// Write to the storage buffer during this pass
GraphBuffer PassBuilder::writeStorageBuffer(GraphBuffer buffer)
{
    writeBuffer(buffer, Pass::BufferUsage::Storage);
    return buffer;
}

// Read and write the storage buffer during this pass
GraphBuffer PassBuilder::readWriteStorageBuffer(GraphBuffer buffer)
{
    readBuffer(buffer, Pass::BufferUsage::Storage);
    writeBuffer(buffer, Pass::BufferUsage::Storage);
    return buffer;
}

// Read from the storage image during this pass
GraphImage PassBuilder::readStorageImage(GraphImage image)
{
    readImage(image, Pass::ImageUsage::Storage);
    return image;
}

// Write to the storage image during this pass
GraphImage PassBuilder::writeStorageImage(GraphImage image)
{
    writeImage(image, Pass::ImageUsage::Storage);
    return image;
}

// Read and write the storage image during this pass
GraphImage PassBuilder::readWriteStorageImage(GraphImage image)
{
    readImage(image, Pass::ImageUsage::Storage);
    writeImage(image, Pass::ImageUsage::Storage);
    return image;
}

// Use the buffer as transfer destination
GraphBuffer PassBuilder::transferSrcBuffer(GraphBuffer buffer)
{
    readBuffer(buffer, Pass::BufferUsage::TransferSrc);
    return buffer;
}

// Use the buffer as transfer source
GraphBuffer PassBuilder::transferDstBuffer(GraphBuffer buffer)
{
    writeBuffer(buffer, Pass::BufferUsage::TransferDst);
    return buffer;
}

// Use the image as transfer destination
GraphImage PassBuilder::transferSrcImage(GraphImage image)
{
    readImage(image, Pass::ImageUsage::TransferSrc);
    return image;
}

// Use the image as transfer source
GraphImage PassBuilder::transferDstImage(GraphImage image)
{
    writeImage(image, Pass::ImageUsage::TransferDst);
    return image;
}

// Graphics pass builder

// Use the image as color attachment in this pass
GraphImage GraphicsPassBuilder::useColorOutput(GraphImage image)
{
    writeImage(image, Pass::ImageUsage::ColorOutput);
    return image;
}

// Use the buffer as an index buffer
GraphBuffer GraphicsPassBuilder::useIndexBuffer(GraphBuffer buffer)
{
    readBuffer(buffer, Pass::BufferUsage::Index);
    return buffer;
}

// Use the buffer as a vertex buffer
GraphBuffer GraphicsPassBuilder::useVertexBuffer(GraphBuffer buffer)
{
    readBuffer(buffer, Pass::BufferUsage::Vertex);
    return buffer;
}

} // namespace cndt
