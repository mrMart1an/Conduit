#ifndef CNDT_RENDERER_SHADER_PROGRAM_H
#define CNDT_RENDERER_SHADER_PROGRAM_H

#include "conduit/defines.h"
#include "conduit/renderer/backendEnum.h"
#include <vector>

namespace cndt {

// Conduit renderer shader program 
class ShaderProgram {
public:
    enum class Type {
        None,
        Graphics,
        Compute
    };

    // Vertex attribute input format
    enum class Format {
        u8, u16, u32, u64,
        i8, i16, i32, i64,
        f16, f32, f64,
        // TODO add packed format option 
    };
 
    // Rasterizer configuration
    struct RasterConfig {
    public:
        enum class CullMode {
            None,
            Front,
            Back,
            FrontAndBack
        };

        enum class FrontFace {
            Clockwise,
            CounterClockwise
        };

        enum class PolyMode {
            Fill,
            Line
        };

        enum class DepthComparison {
            Always,
            Never,
            Less,
            LessOrEqual,
            Greater,
            GreaterOrEqual,
            Equal,
            NotEqual
        };

        enum class SampleCount {
            Count_1,
            Count_2,
            Count_4,
            Count_8,
            Count_16,
            Count_32,
            Count_64
        };
       
    public:
        // Rasterizer culling mode
        CullMode cull_mode = CullMode::Back;

        // Witch face is to be considered front face
        FrontFace front_face = FrontFace::CounterClockwise;

        // Polygon drawing mode
        PolyMode poly_mode = PolyMode::Fill;

        // Perform depth testing
        bool depth_testing = false;
        // If depth testing enable perform depth write back
        bool depth_write_back = false;
        // Depth comparison operation
        DepthComparison depth_comparison_op = DepthComparison::Less;

        // Rasterization sample count 
        SampleCount sample_count = SampleCount::Count_1;
        // Enable SSAA
        bool ssaa_sampling = false;
    };

    // Vertex input config
    struct VertexConfig {
    public:
       // Store the information for one vertex attribute
        struct Attribute {
            Attribute(u32 l, u32 o, Format f, u32 s) :
                location(l),
                offset(o),
                format(f),
                size(s)
            { }

            // Vertex attribute location
            u32 location;

            // Attribute offset
            u32 offset;
            // Attribute input format
            Format format;
            // The amount of time the format input is repeated
            u32 size;
        };

    public:
        // Vertex attributes
        std::vector<Attribute> attributes;

        // Vertex stride
        usize stride;

        // Vertex buffer binding 
        u32 binding = 0;
    };

public:
    ShaderProgram() = default;
    ShaderProgram(Type type) :
        m_type(type) 
    { };

    virtual ~ShaderProgram() = default;

    // Return the renderer backend that own this resource
    virtual RendererBackend backend() const = 0;

    // Return the program type
    Type type() const { return m_type; }

private:
    // Program type
    Type m_type = Type::None;
};

} // namespace cndt

#endif

