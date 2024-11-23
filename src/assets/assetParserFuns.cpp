#include "conduit/assets/mesh.h"
#include "conduit/assets/texture.h"
#include "conduit/logging.h"

#include "conduit/assets/assetsManagerException.h"
#include "conduit/assets/shader.h"
#include "conduit/assets/assetInfo.h"
#include "conduit/assets/assetsTypeFuns.h"

#include <filesystem>

#include <nlohmann/json.hpp>

namespace cndt {

using json = nlohmann::json;

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

// Parse texture info from json 
template <>
AssetInfo<Texture> parseTableEntry<Texture>(
    std::string_view name,
    json element
) {
    std::filesystem::path src = 
        element.at("src")
        .get<std::filesystem::path>();

    // Norm will be empty unless explicitly specified in the json element
    std::filesystem::path norm = {};
    
    if (element.find("norm") != element.end()) {
        json norm_j = element.at("norm");

        if (!norm_j.is_null()) {
            norm = norm_j.get<std::filesystem::path>();
        }
    }

    // Test path validity
    if (!std::filesystem::exists(src)) {
        throw AssetTableParseError(
            "Source file \"{}\" for texture asset \"{}\" not found",
            src.string(),
            name
        );
    }
    if (!std::filesystem::exists(norm) && !norm.empty()) {
        throw AssetTableParseError(
            "Normal file \"{}\" for texture asset \"{}\" not found",
            norm.string(),
            name
        );
    }
    
    // Debug log
    log::core::debug(
        "Using texture asset: \"{}\"",
        name
    );
    
    return AssetInfo<Texture>(
        name,

        src,
        norm
    );
}

// Parse mesh info from json 
template <>
AssetInfo<Mesh> parseTableEntry<Mesh>(
    std::string_view name,
    json element
) {   
    std::filesystem::path src = 
        element.at("src")
        .get<std::filesystem::path>();
    
    // Debug log
    log::core::debug(
        "Using mesh asset: \"{}\"",
        name
    );
    
    // Test path validity
    if (!std::filesystem::exists(src)) {
        throw AssetTableParseError(
            "Source file \"{}\" for mesh asset \"{}\" not found",
            src.string(),
            name
        );
    }
    
    // Get the shader type
    std::string_view type_str = element.at("type").get<std::string_view>();
    AssetInfo<Mesh>::FileType type;
    
    if (type_str == "obj")
        type = AssetInfo<Mesh>::FileType::Obj;
        
    else {
        throw AssetTableParseError(
            "Asset mesh \"{}\" unknow file type: \"{}\"",
            name,
            type_str
        );
    }
    
    return AssetInfo<Mesh>(
        name,
        src,
        type
    );
}

} // namespace cndt::internal
