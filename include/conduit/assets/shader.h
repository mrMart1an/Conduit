#ifndef CNDT_SHADER_ASSET_H
#define CNDT_SHADER_ASSET_H

#include "conduit/defines.h"

#include <vector>

namespace cndt {

// Store a conduit shader asset
class Shader {
public:
    Shader() = default;
    Shader(std::vector<u32> spv_code) : m_spv_code(std::move(spv_code)) { }

    // Get the Spriv byte code
    // store the byte code size in the given usize variable
    const u32* getSpv(usize& code_size) const 
    { 
        code_size  = m_spv_code.size();
        return m_spv_code.data(); 
    }
    
private:
    // Store the shader spriv code
    std::vector<u32> m_spv_code;
};

} // namespace cndt

#endif 
