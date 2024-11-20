#include "conduit/renderer/shader/programBuilder.h"
#include "conduit/logging.h"
#include "conduit/renderer/rendererException.h"
#include "conduit/renderer/shader/program.h"

namespace cndt {

// Build the program from the shader stage and information
// store in the builder or retrieve it from the cache if
// no changes were made since the last call to build 
RenderRef<ShaderProgram> ShaderProgramBuilder::build()
{
    if (m_cache.has_value()) {
        return m_cache.value();
    }

    // Build the cache and return the result
    m_cache = buildCache();

    return m_cache.value();
}

// Add a shader to the program, the stage is determined from the 
// shader asset info and an exception is thrown if the stage is
// not compatible with the program type determined by the over
// shader stage and stage configuration
//
// If the program type is not yet determined this function
// set it according to the shader stage give
// (compute -> compute program),
// (vertex, fragment, tessellation, geometry -> graphics program)
void ShaderProgramBuilder::addStage(AssetHandle<Shader> shader)
{
    ShaderProgram::Type shader_type;
        
    // Check the shader type
    if (shader.info().shaderType() == Shader::Type::Compute) {
        shader_type = ShaderProgram::Type::Compute;
    } else if (
        shader.info().shaderType() == Shader::Type::Vertex ||
        shader.info().shaderType() == Shader::Type::Fragment ||
        shader.info().shaderType() == Shader::Type::Geometry ||
        shader.info().shaderType() == Shader::Type::TessellationEval ||
        shader.info().shaderType() == Shader::Type::TessellationControl
    ) {
        shader_type = ShaderProgram::Type::Graphics;
    } else {
        throw ShaderProgramInvalidOption(
            backend(),
            "Invalid shader asset type"
        );
    }

    // Check shader program type
    if (m_type != shader_type && m_type != ShaderProgram::Type::None) {
        throw ShaderProgramInvalidOption(
            backend(),
            "Invalid shader asset type"
        );
    }

    // Store the shader handle
    switch (shader.info().shaderType()) {
        case Shader::Type::Compute:
            m_compute_shader = shader; break;
    
        case Shader::Type::Vertex:
            m_vertex_shader = shader; break;
        case Shader::Type::Fragment:
            m_fragment_shader = shader; break;
        case Shader::Type::Geometry:
            m_geometry_shader = shader; break;
        case Shader::Type::TessellationEval:
            m_tessel_eval_shader = shader; break;
        case Shader::Type::TessellationControl:
            m_tessel_control_shader = shader; break;

        default:
            log::core::warn("something weird is goning on");
            return;
    }

    // Reset the cache
    m_cache = std::nullopt;
}

// Configure the rasterizer if the current program type allow it 
//
// if the program type is not yet determined this function 
// set it to graphics
void ShaderProgramBuilder::configureRasterizer(
    const ShaderProgram::RasterConfig &config
) {
    if (m_type == ShaderProgram::Type::Compute) {
        throw ShaderProgramInvalidOption(
            backend(),
            "Invalid settings for compute program"
        );
    }

    // Set the shader program to graphics
    m_type = ShaderProgram::Type::Graphics;

    // Store the rasterizer settings
    m_raster_config = config;

    // Reset the cache
    m_cache = std::nullopt;
}

// Clear all the stored configuration of the builder
// and reset the program type
void ShaderProgramBuilder::clear()
{
    m_type = ShaderProgram::Type::None; 

    m_raster_config = std::nullopt;

    m_vertex_shader = std::nullopt;
    m_fragment_shader = std::nullopt;
    m_geometry_shader = std::nullopt;
    m_tessel_control_shader = std::nullopt;
    m_tessel_eval_shader = std::nullopt;

    m_compute_shader = std::nullopt;

    // Reset the cache
    m_cache = std::nullopt;
}

} // namespace cndt
