#ifndef CNDT_RENDERER_SHADER_PROGRAM_BUILDER_H
#define CNDT_RENDERER_SHADER_PROGRAM_BUILDER_H

#include "conduit/assets/handle.h"
#include "conduit/assets/shader.h"

#include "conduit/defines.h"
#include "conduit/renderer/backendEnum.h"
#include "conduit/renderer/ResourceRef.h"
#include "conduit/renderer/rendererException.h"
#include "conduit/renderer/shader/program.h"

#include <optional>
#include <tuple>

namespace cndt {

// Conduit renderer shader program builder 
class ShaderProgramBuilder {
public:
    ShaderProgramBuilder() = default;
    virtual ~ShaderProgramBuilder() = default;

    // Build the program from the shader stage and information
    // store in the builder or retrieve it from the cache if
    // no changes were made since the last call to build 
    RenderRef<ShaderProgram> build();

    // Add a shader to the program, the stage is determined from the 
    // shader asset info and an exception is thrown if the stage is
    // not compatible with the program type determined by the over
    // shader stage and stage configuration
    //
    // If the program type is not yet determined this function
    // set it according to the shader stage give
    // (compute -> compute program),
    // (vertex, fragment, tessellation, geometry -> graphics program)
    void addStage(AssetHandle<Shader> shader);

    // Configure the rasterizer if the current program type allow it 
    //
    // if the program type is not yet determined this function 
    // set it to graphics
    void configureRasterizer(const ShaderProgram::RasterConfig &config);

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
    void configureInputVertex(
        std::tuple<
            u32, 
            MT VT::*, 
            ShaderProgram::Format, 
            u32
        >... attributes
    );

    // Clear all the stored configuration of the builder
    // and reset the program type
    void clear();

    // Return the renderer backend that own this resource
    virtual RendererBackend backend() const = 0;

protected:
    // Build the shader program from the information 
    // and shader stage currently stored in the builder
    // and update the program cache
    virtual RenderRef<ShaderProgram> buildCache() = 0;

protected:
    // Store the program type, this settings is determined 
    // automatically by the builder  
    ShaderProgram::Type m_type = ShaderProgram::Type::None;

    // Graphics program info
    std::optional<ShaderProgram::RasterConfig> m_raster_config = std::nullopt;

    // Store a compute shader
    std::optional<AssetHandle<Shader>> m_vertex_shader = std::nullopt;
    // Store a fragment shader
    std::optional<AssetHandle<Shader>> m_fragment_shader = std::nullopt;
    // Store a geometry shader
    std::optional<AssetHandle<Shader>> m_geometry_shader = std::nullopt;
    // Store a tessellation control shader
    std::optional<AssetHandle<Shader>> m_tessel_control_shader = std::nullopt;
    // Store a tessellation evaluation shader
    std::optional<AssetHandle<Shader>> m_tessel_eval_shader = std::nullopt;

    // Vertex input information
    std::optional<ShaderProgram::VertexConfig> m_vertex_config = std::nullopt;

    // Compute program info

    // Store a compute shader
    std::optional<AssetHandle<Shader>> m_compute_shader = std::nullopt;

    // Build cache
    std::optional<RenderRef<ShaderProgram>> m_cache = std::nullopt;
};

// Configure the vertex input state if the current program type allow it
// Take a list of vertex pointer to member as argument to determine 
// the size and the offset of the vertex member variable
//
// if the program type is not yet determined this function 
// set it to graphics
template <typename VT, typename... MT>
void ShaderProgramBuilder::configureInputVertex(
    std::tuple<
        u32, 
        MT VT::*, 
        ShaderProgram::Format, 
        u32
    >... attributes
) {    
    if (m_type == ShaderProgram::Type::Compute) {
        throw ShaderProgramInvalidOption(
            backend(),
            "Invalid settings for compute program"
        );
    }

    // Set the shader program to graphics
    m_type = ShaderProgram::Type::Graphics;
 
    // Store a new vertex config struct
    m_vertex_config = ShaderProgram::VertexConfig();

    m_vertex_config->stride = sizeof(VT);
    m_vertex_config->binding = 0;

    // Instantiate a vertex to avoid nullptr dereference
    VT vertex;
    
    m_vertex_config->attributes = {
        ShaderProgram::VertexConfig::Attribute(
            std::get<0>(attributes), 

            // Calculate the member pointer offset
            u32(
                uintptr(&(vertex.*(std::get<1>(attributes)))) - 
                uintptr(&vertex)
            ),

            std::get<2>(attributes),
            std::get<3>(attributes)
        )...
    };
    
    // Reset the cache
    m_cache = std::nullopt;
}


} // namespace cndt

#endif

