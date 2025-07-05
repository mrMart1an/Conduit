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
    // Get and check json elements
    json vk_code_spv_j = element["vulkan_spv"];
    json vk_code_glsl_j = element["vulkan_glsl"];
    json gl_code_glsl_j = element["opengl_glsl"];
    if (vk_code_spv_j.is_null()) {
        throw AssetTableParseError(
            "Asset shader \"{}\" missing keyword \"vulkan_spv\"",
            name
        );
    }
    if (vk_code_glsl_j.is_null()) {
        throw AssetTableParseError(
            "Asset shader \"{}\" missing keyword \"vulkan_glsl\"",
            name
        );
    }
    if (gl_code_glsl_j.is_null()) {
        throw AssetTableParseError(
            "Asset shader \"{}\" missing keyword \"opengl_glsl\"",
            name
        );
    }
    json type_j = element["type"];
    if (type_j.is_null()) {
        throw AssetTableParseError(
            "Asset shader \"{}\" missing keyword \"type\"",
            name
        );
    }

    // Store the file path
    std::filesystem::path vk_code_spv = 
        vk_code_spv_j.get<std::filesystem::path>();
    std::filesystem::path vk_code_glsl = 
        vk_code_glsl_j.get<std::filesystem::path>();

    std::filesystem::path gl_code_glsl = 
        gl_code_glsl_j.get<std::filesystem::path>();

    // Get the shader type
    std::string_view type_str = type_j.get<std::string_view>();
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
    // Checking path existence
    std::filesystem::path vk_spv_path = info.pathVkSpv();
    std::filesystem::path vk_glsl_path = info.pathVkGlsl();
    std::filesystem::path gl_glsl_path = info.pathVkGlsl();

    if(!std::filesystem::exists(vk_spv_path)) {
        throw AssetLoadingError(
            "Vulkan spv file at: \"{}\" not found",
            vk_spv_path.string()
        );
    }
    if(!std::filesystem::exists(vk_glsl_path)) {
        throw AssetLoadingError(
            "Vulkan glsl file at: \"{}\" not found",
            vk_spv_path.string()
        );
    }
    if(!std::filesystem::exists(gl_glsl_path)) {
        throw AssetLoadingError(
            "OpenGL glsl file at: \"{}\" not found",
            vk_spv_path.string()
        );
    }

    // Load the vulkan spv code byte in memory 
    std::vector<u32> vk_spv_code;

    try {
        std::ifstream vk_spv_file(
            vk_spv_path,
            std::ifstream::ate | std::ifstream::binary
        );

        if(vk_spv_file.fail()) {
            throw AssetLoadingError(
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
        throw AssetLoadingError(
            "Shader spv code loading error: {}",
            e.what()
        );
    }

    // Load the vulkan glsl code byte in memory 
    std::vector<char> vk_glsl_code;

    try {
        std::ifstream vk_glsl_file(
            vk_glsl_path,
            std::ifstream::ate | std::ifstream::binary
        );

        if(vk_glsl_file.fail()) {
            throw AssetLoadingError(
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
        throw AssetLoadingError(
            "Shader vulkan glsl code loading error: {}",
            e.what()
        );
    }

    // Load the vulkan glsl code byte in memory 
    std::vector<char> gl_glsl_code;

    try {
        std::ifstream gl_glsl_file(
            gl_glsl_path,
            std::ifstream::ate | std::ifstream::binary
        );

        if(gl_glsl_file.fail()) {
            throw AssetLoadingError(
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
        throw AssetLoadingError(
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
