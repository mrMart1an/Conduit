#include "renderer/opengl/shaders/glShaderProgram.h"

namespace cndt::gl {

GlShaderProgram::GlShaderProgram(
    GLuint program_id,
    RasterConfig raster_config
) : 
    m_program_id(program_id), 
    m_raster_config(raster_config)
{ }

// Delete the OpenGL shader program
GlShaderProgram::~GlShaderProgram() 
{
    if (m_program_id)
        glDeleteProgram(m_program_id);

    m_program_id = 0;
}

} // namespace cndt::gl
