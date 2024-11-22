#ifndef CNDT_RENDERER_BUFFER_H
#define CNDT_RENDERER_BUFFER_H

#include "conduit/defines.h"

#include "conduit/renderer/backendEnum.h"

namespace cndt {

// Backend independent GPU buffer abstraction 
class GpuBuffer {
public:
    // Buffer size information
    using Size = u64;

    // GPU buffer create info
    struct Info {
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
    
            static constexpr UsageEnum TransferSrc   = CNDT_BIT(0);
            static constexpr UsageEnum TransferDst   = CNDT_BIT(1);
    
            static constexpr UsageEnum UniformBuffer = CNDT_BIT(2);
            static constexpr UsageEnum StorageBuffer = CNDT_BIT(3);
            static constexpr UsageEnum VertexBuffer  = CNDT_BIT(4);
            static constexpr UsageEnum IndexBuffer   = CNDT_BIT(5);
    
            Usage() = delete;
        };
    
    public:
        // Buffer memory domain
        Domain domain = Domain::Device;
        // Buffer usage
        UsageEnum usage = Usage::None;
    
        // Buffer size 
        Size size = 0;
    };

public:
    GpuBuffer() = default;
    virtual ~GpuBuffer() = default;

    // Resize the buffer, if the preserve data argument is true
    // the content of the buffer will be copied to the new buffer
    // (preserve data default: false)
    // 
    // (If the new size is less that the previews one the
    // content will be cut to fit the new buffer)
    //
    // Warning: the buffer MUST be unmapped before resizing
    virtual void resize(Size new_size, bool preserve_data = false) = 0;

    // If the buffer is host visible map it to a CPU visible address and 
    // perform the necessary cache maintenance 
    //
    // The buffer MUST be unmapped before usage by the GPU
    virtual void* map() = 0;

    // Unmap the buffer and 
    // perform the necessary cache maintenance 
    virtual void unmap() = 0;

    // Copy the data at the src pointer in the buffer at the given offset
    //
    // copy_size: size of the data to be copied
    // dest_offset: buffer offset where to store the given data (default = 0)
    virtual void copyMemToBuf(
        const void* data_src_p,

        Size copy_size,
        Size dest_offset = 0
    ) = 0;

    // Copy the data in the buffer at the given offset to the given
    // destination address
    //
    // copy_size: size of the data to be copied
    // src_offset: buffer offset from where to read the data (default = 0)
    virtual void copyBufToMem(
        void* data_dest_p,

        Size copy_size,
        Size dest_offset = 0
    ) = 0;

    // Convenience template function to copy the given type 
    // to the buffer at the given offset
    template <typename T>
    void copyTypeToBuf(
        const T& src_obj, 
        Size dest_offset = 0
    );

    // Convenience template function to copy the data at the given buffer  
    // offset to the object of the given type
    template <typename T>
    void copyBufToType(
        T& dest_obj, 
        Size src_offset = 0
    );

    // Copy the content of the given buffer to the buffer with
    // the given offsets 
    virtual void copyBufToBuf(
        const GpuBuffer& src_buffer,
        Size copy_size,
        
        Size src_offset = 0,
        Size dest_offset = 0
    ) = 0;

    /*
     *
     *      Getter
     *
     * */

    // Return the size of the buffer
    virtual Size size() const = 0;

    // Return true if the buffer is mapped
    virtual bool mapped() const = 0;

    // Return the renderer backend that own this resource
    virtual RendererBackend backend() const = 0;
};

/*
 *
 *      GPU buffer template implementation
 * 
 * */

// Convenience template function to copy the given type 
// to the buffer at the given offset
template <typename T>
void GpuBuffer::copyTypeToBuf(
    const T& src_obj, 
    Size dest_offset
) {
    copyMemToBuf(
        &src_obj,
        sizeof(T),
        dest_offset
    );
}

// Convenience template function to copy the data at the given buffer  
// offset to the object of the given type
template <typename T>
void GpuBuffer::copyBufToType(
    T& dest_obj, 
    Size src_offset
) {
    copyBufToMem(
        &dest_obj,
        sizeof(T),
        src_offset
    );
}

} // namespace cndt

#endif
