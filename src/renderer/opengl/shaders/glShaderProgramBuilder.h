#ifndef CNDT_RENDERER_GL_SHADER_PROGRAM_BUILDER_H
#define CNDT_RENDERER_GL_SHADER_PROGRAM_BUILDER_H


#include "conduit/assets/handle.h"
#include "conduit/assets/shader.h"

#include "conduit/renderer/shader/programBuilder.h"

#include "renderer/opengl/shaders/glShaderProgram.h"

namespace cndt::gl {

// OpneGL shader program builder
class GlShaderProgramBuilder : public ShaderProgramBuilder {

public:
    GlShaderProgramBuilder() = default;
    ~GlShaderProgramBuilder() override = default;

    // Build the shader program from the information 
    // and shader stage currently stored in the builder
    RenderRef<ShaderProgram> buildCache() override;

    // Return the renderer backend that own this resource
    RendererBackend backend() const override
    { return RendererBackend::OpenGL;}

    // Parse the shader modules
    // Also performs the necessary error checking
    GLuint compileShader(const AssetHandle<Shader>& shader) const;
};

} // namespace cndt::gl

#endif
