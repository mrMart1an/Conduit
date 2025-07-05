#ifndef CNDT_RENDERER_VERTEX_LAYOUT_H
#define CNDT_RENDERER_VERTEX_LAYOUT_H

#include "conduit/defines.h"
#include <tuple>

namespace cndt {

// Vertex input config
struct VertexLayout {
public:
    VertexLayout() = default;
    ~VertexLayout() = default;

public:
    // Vertex attribute input format
    enum class Format {
        u8, u16, u32, u64,
        i8, i16, i32, i64,
        f16, f32, f64,
        // TODO add packed format option 
    };

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

    // TODO support multiple binding
    //
    // Configure the vertex input state if the current program type allow it
    // Take a list of vertex pointer to member as argument to determine 
    // the size and the offset of the vertex member variable
    //
    // if the program type is not yet determined this function 
    // set it to graphics
    //
    // Take as argument a list of tuple each describing a vertex attribute
    // with the following format:
    // { location, vertex member pointer, format, size }
    //
    // size is the number of times the format is contain in the input 
    template <typename VT, typename... MT>
    void setLayout(
        std::tuple<
            u32, 
            MT VT::*, 
            Format, 
            u32
        >... attributes
    );

public:
    // Vertex attributes
    std::vector<Attribute> attributes;

    // Vertex stride
    usize stride;

    // Vertex buffer binding 
    u32 binding = 0;
};

// Configure the vertex input state if the current program type allow it
// Take a list of vertex pointer to member as argument to determine 
// the size and the offset of the vertex member variable
//
// if the program type is not yet determined this function 
// set it to graphics
template <typename VT, typename... MT>
void VertexLayout::setLayout(
    std::tuple<
        u32, 
        MT VT::*, 
        VertexLayout::Format, 
        u32
    >... in_attributes
) {    
    stride = sizeof(VT);
    binding = 0;

    // Instantiate a vertex to avoid nullptr dereference
    VT vertex;
    
    attributes = {
        VertexLayout::Attribute(
            std::get<0>(in_attributes), 

            // Calculate the member pointer offset
            u32(
                uintptr(&(vertex.*(std::get<1>(in_attributes)))) - 
                uintptr(&vertex)
            ),

            std::get<2>(in_attributes),
            std::get<3>(in_attributes)
        )...
    };
}

} // namespace cndt

#endif
