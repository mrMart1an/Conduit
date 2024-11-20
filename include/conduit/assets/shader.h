#ifndef CNDT_SHADER_ASSET_H
#define CNDT_SHADER_ASSET_H

#include "conduit/defines.h"

#include <vector>

namespace cndt {

// Store a conduit shader asset
class Shader {
public:
    // Shader type
    enum class Type {
        Undefined,

        Vertex,
        Fragment,
        Geometry,
        TessellationControl,
        TessellationEval,

        Compute
    };
    
public:
    Shader() = default;
    Shader(
        std::vector<u32>& vk_spv,
        std::vector<char>& vk_glsl,

        std::vector<char>& gl_glsl,

        Type shader_type
    ) : 
        m_vk_spv(std::move(vk_spv)),
        m_vk_glsl(std::move(vk_glsl)),

        m_gl_glsl(std::move(gl_glsl)),

        m_type(shader_type)
    { }

    // Get the Vulkan Spriv byte code
    // store the byte code size in bytes in the given usize variable
    const u32* getVkSpv(usize *code_size_p) const 
    { 
        if (code_size_p != nullptr)
            *code_size_p = m_vk_spv.size() * sizeof(u32);

        return m_vk_spv.data(); 
    }

    // Get a const reference to the Vulkan spv vector
    const std::vector<u32>& getVkSpvVec() const 
    { 
        return m_vk_spv; 
    }

    // Get the Vulkan GLSL shader code
    // store the code size in bytes in the given usize variable
    const char* getVkGlsl(usize *code_size_p) const 
    { 
        if (code_size_p != nullptr)
            *code_size_p  = m_vk_glsl.size();

        return m_vk_glsl.data(); 
    }

    // Get the OpenGL GLSL shader code
    // store the code size in bytes in the given usize variable
    const char* getGlGlsl(usize *code_size_p) const 
    { 
        if (code_size_p != nullptr)
            *code_size_p  = m_vk_glsl.size();

        return m_vk_glsl.data(); 
    }

    // Return the shader type
    Type type() const { return m_type; }
    
private:
    // Store the shader vulkan spriv code
    std::vector<u32> m_vk_spv;
    // Store the shader vulkan glsl code
    std::vector<char> m_vk_glsl;

    // Store the shader OpenGL glsl code
    std::vector<char> m_gl_glsl;

    Type m_type;
};

} // namespace cndt

#endif 
