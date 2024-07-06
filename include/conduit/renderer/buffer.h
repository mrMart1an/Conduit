#ifndef CNDT_RENDERER_BUFFER_H
#define CNDT_RENDERER_BUFFER_H

#include "conduit/defines.h"

#include <functional>

namespace cndt {

class Renderer;

// GPU buffer create info
struct GpuBufferInfo {
public:
    // Buffer memory domain enum
    enum class Domain {
        // Memory for efficient device access, prefer host visible  
        // Automatically select the best memory type 
        Device,
        // Host visible memory
        // Automatically select the best memory type 
        Host,

        // Host visible cached memory
        HostCached,
        // Host visible coherent memory
        HostCoherent
    };

    // Buffer usage enum with constexpr
    using UsageEnum = u32;

    struct Usage {
        static constexpr UsageEnum None          = 0;

        static constexpr UsageEnum TransferSrc   = BIT(0);
        static constexpr UsageEnum TransferDst   = BIT(1);

        static constexpr UsageEnum UniformBuffer = BIT(2);
        static constexpr UsageEnum StorageBuffer = BIT(3);
        static constexpr UsageEnum VertexBuffer  = BIT(4);
        static constexpr UsageEnum IndexBuffer   = BIT(5);

        Usage() = delete;
    };

    // Buffer size type
    using BufferSize = usize;

public:
    // Buffer memory domain
    Domain domain = Domain::Device;
    // Buffer usage
    UsageEnum usage = Usage::None;

    // Buffer size 
    BufferSize size = 0;
};

// Ref counted GPU renderer buffer
class GpuBufferRef {
    friend class Renderer;
    
    // Buffer id type
    using Id = u64;
    static constexpr Id nullId = UINT64_MAX;

    // Reference counter type
    using Counter = usize;

public:
    // Public null ref constructor
    GpuBufferRef() : 
        m_id(nullId),
        m_ref_count(nullptr),
        m_deffer_delete_f([](){ })
    { }

    // If the ref counter reaches zero call the deffer delete function
    ~GpuBufferRef();

    // Copy constructor
    GpuBufferRef(const GpuBufferRef&);
    // Copy assignment
    GpuBufferRef& operator=(const GpuBufferRef&);

    // Move constructor
    GpuBufferRef(GpuBufferRef&&);
    // Move assignment
    GpuBufferRef& operator=(GpuBufferRef&&);

private:
    // Private constructor for the rendering backend
    GpuBufferRef(
        Id id,
        std::function<void(void)> deffer_delete_f
    ) : 
        m_id(id),
        m_ref_count(new Counter),
        m_deffer_delete_f(deffer_delete_f)
    { }

    // Decrease the counter and call the delete callback if it reaches 0
    void release();

private:
    // Buffer id used by the rendering backend to identify the buffer
    Id m_id;

    // Count the number of existing buffer reference
    Counter* m_ref_count;

    // Deffer delete function callback
    std::function<void(void)> m_deffer_delete_f;
};

} // namespace cndt

#endif
