#ifndef CNDT_RENDERER_GRAPH_PASS_H
#define CNDT_RENDERER_GRAPH_PASS_H

#include "conduit/defines.h"

namespace cndt {

class GraphicsPassBuilder;

// Graph render pass
class Pass {
public:
    // Pass id type
    using Id = u64;
    // Pass id null value
    static constexpr Id nullId = UINT64_MAX;

    // Pass type enum
    enum class PassType {
        // Undefined pass type
        Undefined,

        // Graphics pass type
        Graphics,
        // Compute pass type
        Compute,
        // Transfer pass type
        Transfer
    };
    
    // Buffer resources usage in the pass
    enum class BufferUsage {
        Undefined,

        TransferSrc,   
        Uniform, 
        Vertex,  
        Index, 

        TransferDst,   
        Storage,
    };

    // Image resources usage in the pass
    enum class ImageUsage {
        Undefined,

        TransferSrc,   
        Sampled,
        ColorInput,
        DepthStencilInput,
        Present,

        TransferDst,   
        Storage,
        ColorOutput,
        DepthStencilOutput,
    };

public:
    Pass(Id id, PassType pass_type) :
        m_pass_id(id),
        m_pass_type(pass_type) 
    { }

    // Return the pass Id
    Id id() const { return m_pass_id; }
    // Return the pass type
    PassType type() const { return m_pass_type; }

private:
    // Store the pass unique Id
    Id m_pass_id;
    // Store the pass type
    PassType m_pass_type;
};

// Graph graphics render pass
class GraphicsPass : public Pass {
public: 
    GraphicsPass(Id id) : Pass(
        id,
        PassType::Graphics
    ) { }

    // Build the render pass, declare all the read and write dependency
    virtual void build(GraphicsPassBuilder& builder) = 0;
};

} // namespace cndt

#endif
