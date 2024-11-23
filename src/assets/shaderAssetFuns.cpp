#include "conduit/assets/assetsManagerException.h"
#include "conduit/assets/assetsTypeFuns.h"
#include "conduit/assets/shader.h"
#include <fstream>

namespace cndt {

using json = nlohmann::json;

// Return shader asset table name (the json key to the asset table)
template <>
std::string assetTableName<Shader>() {
    return "shaders";
}

// Parse shader info from json 
template <>
AssetInfo<Shader> parseTableEntry<Shader>(
    std::string_view name,
    json element
) {
    std::filesystem::path vk_code_spv = 
        element.at("vulkan_spv")
        .get<std::filesystem::path>();
    std::filesystem::path vk_code_glsl = 
        element.at("vulkan_glsl")
        .get<std::filesystem::path>();

    std::filesystem::path gl_code_glsl = 
        element.at("opengl_glsl")
        .get<std::filesystem::path>();

    // Test path validity
    if (!std::filesystem::exists(vk_code_spv)) {
        throw AssetTableParseError(
            "File \"{}\" for shader asset \"{}\" not found",
            vk_code_spv.string(),
            name
        );
    }
    if (!std::filesystem::exists(vk_code_glsl)) {
        throw AssetTableParseError(
            "File \"{}\" for shader asset \"{}\" not found",
            vk_code_glsl.string(),
            name
        );
    }

    if (!std::filesystem::exists(gl_code_glsl)) {
        throw AssetTableParseError(
            "File \"{}\" for shader asset \"{}\" not found",
            gl_code_glsl.string(),
            name
        );
    }

    // Get the shader type
    std::string_view type_str = element.at("type").get<std::string_view>();
    Shader::Type type;
    
    if (type_str == "vertex")
        type = Shader::Type::Vertex;
    else if (type_str == "fragment")
        type = Shader::Type::Fragment;
    else if (type_str == "geometry")
        type = Shader::Type::Geometry;
    else if (type_str == "tesseletion_control")
        type = Shader::Type::TessellationControl;
    else if (type_str == "tesseletion_evaluation")
        type = Shader::Type::TessellationEval;
    else if (type_str == "compute")
        type = Shader::Type::Compute;
        
    else {
        throw AssetTableParseError(
            "Asset shader \"{}\" unknow type: \"{}\"",
            name,
            type_str
        );
    }

    // Debug log
    log::core::debug(
        "Using {} shader asset: \"{}\"",
        type_str,
        name
    );
    
    return AssetInfo<Shader>(
        name,
        
        vk_code_spv,
        vk_code_glsl,

        gl_code_glsl,

        type
    );
}

// Load shader from the given shader info 
template <>
std::unique_ptr<Shader> loadAsset<Shader>(
    AssetInfo<Shader>& info
) {
    // Load the vulkan spv code byte in memory 
    std::filesystem::path vk_spv_path = info.pathVkSpv();
    std::vector<u32> vk_spv_code;

    try {
        std::ifstream vk_spv_file(
            vk_spv_path,
            std::ifstream::ate | std::ifstream::binary
        );

        if(vk_spv_file.fail()) {
            throw ShaderLoadingError(
                "failed to open file: {}",
                vk_spv_path.string()
            );
        }

        // Get the code size and go to the beginning of the file
        usize spv_size = vk_spv_file.tellg() / sizeof(u32);
        vk_spv_file.seekg(0);

        // Load the code in the buffer
        vk_spv_code.resize(spv_size);
        vk_spv_file.read((char*)vk_spv_code.data(), spv_size * sizeof(u32));

        vk_spv_file.close();

    } catch (std::exception& e) {
        throw ShaderLoadingError(
            "Shader spv code loading error: {}",
            e.what()
        );
    }

    // Load the vulkan glsl code byte in memory 
    std::filesystem::path vk_glsl_path = info.pathVkGlsl();
    std::vector<char> vk_glsl_code;

    try {
        std::ifstream vk_glsl_file(
            vk_glsl_path,
            std::ifstream::ate | std::ifstream::binary
        );

        if(vk_glsl_file.fail()) {
            throw ShaderLoadingError(
                "failed to open file: {}",
                vk_spv_path.string()
            );
        }

        // Get the code size and go to the beginning of the file
        usize glsl_size = vk_glsl_file.tellg();
        vk_glsl_file.seekg(0);

        // Load the code in the buffer
        vk_glsl_code.resize(glsl_size);
        vk_glsl_file.read((char*)vk_glsl_code.data(), glsl_size);

        vk_glsl_file.close();

    } catch (std::exception& e) {
        throw ShaderLoadingError(
            "Shader vulkan glsl code loading error: {}",
            e.what()
        );
    }

    // Load the vulkan glsl code byte in memory 
    std::filesystem::path gl_glsl_path = info.pathVkGlsl();
    std::vector<char> gl_glsl_code;

    try {
        std::ifstream gl_glsl_file(
            gl_glsl_path,
            std::ifstream::ate | std::ifstream::binary
        );

        if(gl_glsl_file.fail()) {
            throw ShaderLoadingError(
                "failed to open file: {}",
                vk_spv_path.string()
            );
        }

        // Get the code size and go to the beginning of the file
        usize glsl_size = gl_glsl_file.tellg();
        gl_glsl_file.seekg(0);

        // Load the code in the buffer
        gl_glsl_code.resize(glsl_size);
        gl_glsl_file.read((char*)gl_glsl_code.data(), glsl_size);

        gl_glsl_file.close();

    } catch (std::exception& e) {
        throw ShaderLoadingError(
            "Shader OpenGL glsl code loading error: {}",
            e.what()
        );
    }

    // Create and return a shared pointer storing the asset storage
    return std::make_unique<Shader>(
        vk_spv_code,
        vk_glsl_code,

        gl_glsl_code,

        info.shaderType()
    );
}

} // namespace cndt
