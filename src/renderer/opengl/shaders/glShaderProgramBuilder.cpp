#include "renderer/opengl/shaders/glShaderProgramBuilder.h"
#include "conduit/assets/shader.h"
#include "renderer/opengl/shaders/glShaderProgram.h"
#include "renderer/opengl/utils/glExceptions.h"

#include <glad/glad.h>


namespace cndt::gl {

// Build the shader program from the information 
// and shader stage currently stored in the builder
RenderRef<ShaderProgram> GlShaderProgramBuilder::buildCache()
{
    // Create the shader program
    GLuint program = glCreateProgram();

    // TODO Support other shader type
    GLuint vertex_shader;
    GLuint fragment_shader;

    if (m_vertex_shader.has_value() && m_fragment_shader.has_value()) {
        vertex_shader = compileShader(m_vertex_shader.value());
        fragment_shader = compileShader(m_fragment_shader.value());
    } else {
        throw ShaderModuleCreateError(
            "Vertex or fragment shader missing, invalid shader program"
        );
    }

    // Link the program
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // Delete the shader modules
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);  

    // Check for error
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glDeleteProgram(program);

        char error_log[512];
        glGetProgramInfoLog(program, 512, NULL, error_log);

        throw ShaderModuleCreateError(
            "Shader program link error: {}",
            error_log
        );
    }

    // Check for rasterizer config
    if (!m_raster_config.has_value()) {
        throw ShaderModuleCreateError(
            "Rasterizer config missing, invalid shader program"
        );
    }

    // Create the shader program
    GlShaderProgram* out_program_p = new GlShaderProgram(
        program,
        m_raster_config.value()
    );

    return RenderRef<ShaderProgram>(out_program_p);
}

// Parse the shader modules
// Also performs the necessary error checking
GLuint GlShaderProgramBuilder::compileShader(
    const AssetHandle<Shader>& shader
) const {
    // Get the code from the shader handle
    usize size;
    const char* code = shader->getGlGlsl(&size);

    // Get shader type
    GLenum type;
    switch (shader->type()) {
        case Shader::Type::Vertex: {
            type = GL_VERTEX_SHADER;
        }
        case Shader::Type::Fragment: {
            type = GL_FRAGMENT_SHADER;
        }
        case Shader::Type::Compute: {
            throw ShaderModuleCreateError(
                "the OpenGL backend doesn't currently support Compute shader"
            );
        }
        case Shader::Type::Geometry: {
            throw ShaderModuleCreateError(
                "the OpenGL backend doesn't currently support Geometry shader"
            );
        }
        case Shader::Type::TessellationEval: {
            throw ShaderModuleCreateError(
                "the OpenGL backend doesn't currently support TessEval shader"
            );
        }
        case Shader::Type::TessellationControl: {
            throw ShaderModuleCreateError(
                "the OpenGL backend doesn't currently support TessCtrl shader"
            );
        }
        default: {
            throw ShaderModuleCreateError(
                "Unknown shader type"
            );
        }
    }

    // Compile the shader
    GLuint shader_id = glCreateShader(type);

    glShaderSource(shader_id, 1, &code, (i32*)&size);
    glCompileShader(shader_id);

    // Check for compilation error
    int  success;

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        glDeleteShader(shader_id);

        char error_log[512];
        glGetShaderInfoLog(shader_id, 512, NULL, error_log);

        throw ShaderModuleCreateError(
            "Shader compilation error: {}",
            error_log
        );
    }

    return shader_id;
}

} // namespace cndt::gl
