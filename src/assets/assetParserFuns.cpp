#include "conduit/logging.h"

#include "conduit/assets/assetsManagerException.h"
#include "conduit/assets/shader.h"
#include "conduit/assets/assetInfo.h"

#include <filesystem>
#include <format>

#include <nlohmann/json.hpp>

namespace cndt::internal {

using json = nlohmann::json;

// Parse shader info from json 
AssetInfo<Shader> parseShader(std::string_view name, json element) 
{
    std::filesystem::path code_spv = 
        element.at("code_spv")
        .get<std::filesystem::path>();
    std::filesystem::path code_glsl = 
        element.at("code_glsl")
        .get<std::filesystem::path>();

    // Test path validity
    if (!std::filesystem::exists(code_spv)) {
        throw AssetTableParseError(std::format(
            "File \"{}\" for shader asset \"{}\" not found",
            code_spv.string(),
            name
        ));
    }
    if (!std::filesystem::exists(code_glsl)) {
        throw AssetTableParseError(std::format(
            "File \"{}\" for shader asset \"{}\" not found",
            code_glsl.string(),
            name
        ));
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
    else if (type_str == "tesseletion")
        type = Shader::Type::Tessellation;
    else if (type_str == "compute")
        type = Shader::Type::Compute;
        
    else {
        throw AssetTableParseError(std::format(
            "Asset shader \"{}\" unknow type: \"{}\"",
            name,
            type_str
        ));
    }

    // Debug log
    log::core::debug(
        "Using {} shader asset: \"{}\"",
        type_str,
        name
    );
    
    return AssetInfo<Shader>(
        name,
        
        code_spv,
        code_glsl,

        type
    );
}

// Parse texture info from json 
AssetInfo<Texture> parseTexture(std::string_view name, json element) 
{
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
        throw AssetTableParseError(std::format(
            "Source file \"{}\" for texture asset \"{}\" not found",
            src.string(),
            name
        ));
    }
    if (!std::filesystem::exists(norm) && !norm.empty()) {
        throw AssetTableParseError(std::format(
            "Normal file \"{}\" for texture asset \"{}\" not found",
            norm.string(),
            name
        ));
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
AssetInfo<Mesh> parseMesh(std::string_view name, json element) 
{   
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
        throw AssetTableParseError(std::format(
            "Source file \"{}\" for mesh asset \"{}\" not found",
            src.string(),
            name
        ));
    }
    
    // Get the shader type
    std::string_view type_str = element.at("type").get<std::string_view>();
    AssetInfo<Mesh>::FileType type;
    
    if (type_str == "obj")
        type = AssetInfo<Mesh>::FileType::Obj;
        
    else {
        throw AssetTableParseError(std::format(
            "Asset mesh \"{}\" unknow file type: \"{}\"",
            name,
            type_str
        ));
    }
    
    return AssetInfo<Mesh>(
        name,
        src,
        type
    );
}

} // namespace cndt::internal
