#ifndef CNDT_RENDERER_IMAGE_H
#define CNDT_RENDERER_IMAGE_H

#include "conduit/defines.h"
namespace cndt {

// Store the information to create a GPU image
struct GpuImageInfo {
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

    // Image size struct
    struct Size {
        enum class Type {
            Absolute,
            BackBufferRelative
        };

        Type type = Type::BackBufferRelative;

        f32 width = 1.f;
        f32 height = 1.f;
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

public:
    // Image format, R8G8B8A8_UNORM default
    Format format = Format::R8G8B8A8_UNORM;

    // Image size, back buffer size default
    Size size = { };

    // Image sample count, one sample default
    Sample sampe = Sample::Count_1;
};

} // namespace cndt

#endif
