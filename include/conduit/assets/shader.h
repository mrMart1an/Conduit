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
        Tessellation,
        Compute
    };
    
public:
    Shader() = default;
    Shader(
        std::vector<u32>& spv_code,
        Type shader_type
    ) : 
        m_spv_code(std::move(spv_code)),
        m_type(shader_type)
    { }

    // Get the Spriv byte code
    // store the byte code size in the given usize variable
    const u32* getSpv(usize& code_size) const 
    { 
        code_size  = m_spv_code.size();
        return m_spv_code.data(); 
    }

    // Return the shader type
    Type type() const { return m_type; }
    
private:
    // Store the shader spriv code
    std::vector<u32> m_spv_code;

    Type m_type;
};

} // namespace cndt

#endif 
