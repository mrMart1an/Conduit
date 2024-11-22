#ifndef CNDT_RENDERER_OPENGL_SHADER_PROGRAM_H
#define CNDT_RENDERER_OPENGL_SHADER_PROGRAM_H

#include "conduit/renderer/shader/program.h"

#include <glad/glad.h>

namespace cndt::gl {

class GlShaderProgram : public ShaderProgram {
public:
    GlShaderProgram(
        GLuint program_id,
        RasterConfig raster_config
    );
    ~GlShaderProgram() override;

    // Return the renderer backend that own this resource
    RendererBackend backend() const override 
    { return RendererBackend::OpenGL; }

private:
    // Shader program ID
    GLuint m_program_id;

    // Raster state config
    RasterConfig m_raster_config;
};

} // namespace cndt::gl

#endif
