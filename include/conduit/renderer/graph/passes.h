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

    // Pass capability enum type
    using CapabilityEnum = u32;
    // Pass capability bit field
    struct Capability {
        static constexpr CapabilityEnum Graphics = BIT(0);
        static constexpr CapabilityEnum Compute = BIT(1);
        static constexpr CapabilityEnum Transfer = BIT(2);

        Capability() = delete;
        ~Capability() = delete;
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
    Pass(Id id, CapabilityEnum pass_type) :
        m_pass_id(id),
        m_capability(pass_type) 
    { }

    // Return the pass Id
    Id id() const { return m_pass_id; }
    // Return the pass required capability
    CapabilityEnum capability() const { return m_capability; }

private:
    // Store the pass unique Id
    Id m_pass_id;
    // Store the pass type
    CapabilityEnum m_capability;
};

} // namespace cndt

#endif
