#ifndef CNDT_RENDERER_IMAGE_H
#define CNDT_RENDERER_IMAGE_H

#include "conduit/defines.h"

#include "conduit/renderer/backendEnum.h"

namespace cndt {

// Backend independent GPU image abstraction 
class GpuImage {
public:
    // Image 2D dimensions 
    struct Extent {
        Extent() : width(0), height(0) { }
        Extent(u32 width, u32 height) : width(width), height(height) { }

        u32 width;
        u32 height;
    };

    // Store the information to create a GPU image
    struct Info {
    public:
        enum class Format {
            R8G8B8A8_UINT,
            R8G8B8A8_SINT,
            R8G8B8A8_UNORM,
            R8G8B8A8_SNORM,
            R8G8B8A8_SRGB,
    
            B8G8R8A8_UNORM,
            B8G8R8A8_SRGB,
    
            R8G8_UNORM,
            R8G8_SNORM,
            R8G8_UINT,
            R8G8_SINT,
    
            R8_UNORM,
            R8_SNORM,
            R8_UINT,
            R8_SINT,
    
            R16G16B16A16_SINT,
            R16G16B16A16_UINT,
            R16G16B16A16_SFLOAT,
    
            R16G16_SFLOAT,
            R16G16_UINT,
            R16G16_SINT,
    
            R16_SFLOAT,
            R16_UINT,
            R16_SINT,
    
            R32G32_SFLOAT,
            R32G32_UINT,
            R32G32_SINT,
    
            R32_SFLOAT,
            R32_UINT,
            R32_SINT,
    
            D32_SFLOAT,
            D16_UNORM
        };
    
        // Samples count enum
        enum class Sample : u32{
            Count_1  = 1,
            Count_2  = 2,
            Count_4  = 4,
            Count_8  = 8,
            Count_16 = 16,
            Count_32 = 32,
            Count_64 = 64
        };

        // Image usage enum with constexpr
        using UsageEnum = u32;

        struct Usage {
            static constexpr UsageEnum None = 0;

            static constexpr UsageEnum TransferSrc            = BIT(0);
            static constexpr UsageEnum TransferDst            = BIT(1);

            static constexpr UsageEnum Storage                = BIT(2);
            static constexpr UsageEnum Sampled                = BIT(3);

            static constexpr UsageEnum ColorAttachment        = BIT(4);
            static constexpr UsageEnum DepthStencilAttachment = BIT(5);
            static constexpr UsageEnum InputAttachment        = BIT(6);

            Usage() = delete;
        };
    
    public:
        // Image 2D dimensions, width: 0, height: 0 default 
        Extent extent = { };
    
        // Image format, R8G8B8A8_UNORM default
        Format format = Format::R8G8B8A8_UNORM;
    
        // Image sample per pixel count, one sample default
        Sample sampe = Sample::Count_1;

        // Image usage enum, default None
        UsageEnum usage = Usage::None;

        // If true the image will store a full mipmap chain,
        // default false
        bool store_mipmap = false;
    };

public:
    GpuImage() = default;
    virtual ~GpuImage() = default;

    /*
     *
     *      Getter
     *
     * */

    // Return the image extent
    virtual Extent extent() const = 0;
   
    // Return the image format
    virtual Info::Format format() const = 0;

    // Return the renderer backend that own this resource
    virtual RendererBackend backend() const = 0;
};

} // namespace cndt

#endif
